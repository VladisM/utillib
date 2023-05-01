#include "evaluate.h"

#include "error_buffer.h"
#include "tokenizer.h"
#include "convert.h"

#include <utillib/core.h>

#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#include <stdio.h>

typedef struct {
    union{
        long long number;
        char *text;
    }payload;
    bool is_num;
} stack_value_t;

typedef struct {
    char op;
    int precedence;
    evaluator_op_associativity_t associativity;
    unsigned arg_count;
    bool (*compute_fun)(evaluator_t *this, long long *result, list_t *args);
} evaluator_op_record_t;

typedef struct {
    char *func;
    unsigned arg_count;
    bool (*compute_fun)(evaluator_t *this, long long *result, list_t *args);
} evaluator_function_record_t;

static void parse(char *input, queue_t **output);
static bool sort_infix_to_postfix(evaluator_t *this, queue_t *input, queue_t **output);
static bool solve(evaluator_t *this, queue_t *rpn_expresion, long long *result);
static bool _evaluator_convert(evaluator_t *this, stack_value_t *input, long long *output);

void evaluate_new(evaluator_t **evaluator){
    CHECK_NULL_ARGUMENT(evaluator);
    CHECK_NOT_NULL_ARGUMENT(*evaluator);

    *evaluator = (evaluator_t *)dynmem_calloc(1, sizeof(evaluator_t));

    (*evaluator)->error_buffer = NULL;
    (*evaluator)->func_records = NULL;
    (*evaluator)->op_records = NULL;

    error_buffer_init(&((*evaluator)->error_buffer));
    list_init(&((*evaluator)->func_records), sizeof(evaluator_function_record_t *));
    list_init(&((*evaluator)->op_records), sizeof(evaluator_op_record_t *));
}

void evaluate_destroy(evaluator_t *evaluator){
    CHECK_NULL_ARGUMENT(evaluator);

    if(evaluator->error_buffer != NULL){
        error_buffer_destroy(evaluator->error_buffer);
    }

    if(evaluator->func_records != NULL){
        while(list_count(evaluator->func_records) > 0){
            evaluator_function_record_t *head = NULL;
            list_pop(evaluator->func_records, (void *)&head);
            dynmem_free(head->func);
            dynmem_free(head);
        }
        list_destroy(evaluator->func_records);
    }

    if(evaluator->op_records != NULL){
        while(list_count(evaluator->op_records) > 0){
            evaluator_op_record_t *head = NULL;
            list_pop(evaluator->op_records, (void *)&head);
            dynmem_free(head);
        }
        list_destroy(evaluator->op_records);
    }

    dynmem_free(evaluator);
}

void evaluate_append_function(evaluator_t *this, char *func_name, unsigned argc, bool (*compute_function)()){
    CHECK_NULL_ARGUMENT(this);
    CHECK_NULL_ARGUMENT(func_name);
    CHECK_NULL_ARGUMENT(compute_function);

    evaluator_function_record_t *new_record = (evaluator_function_record_t *)dynmem_calloc(1, sizeof(evaluator_function_record_t));

    new_record->arg_count = argc;
    new_record->compute_fun = compute_function;
    new_record->func = dynmem_strdup(func_name);

    list_append(this->func_records, (void *)&new_record);
}

void evaluate_append_operator(evaluator_t *this, char op, int precedence, evaluator_op_associativity_t associativity, unsigned argc, bool (*compute_function)()){
    CHECK_NULL_ARGUMENT(this);
    CHECK_NULL_ARGUMENT(compute_function);

    evaluator_op_record_t *new_record = (evaluator_op_record_t *)dynmem_calloc(1, sizeof(evaluator_op_record_t));

    new_record->arg_count = argc;
    new_record->compute_fun = compute_function;
    new_record->associativity = associativity;
    new_record->op = op;
    new_record->precedence = precedence;

    list_append(this->op_records, (void *)&new_record);
}

bool evaluate_expression(evaluator_t *this, char *expresion, long long *result){
    CHECK_NULL_ARGUMENT(this);
    CHECK_NULL_ARGUMENT(expresion);
    CHECK_NULL_ARGUMENT(result);

    queue_t *parsed_tokens = NULL;
    queue_t *sorted_expresion = NULL;

    parse(expresion, &parsed_tokens);

    if(!sort_infix_to_postfix(this, parsed_tokens, &sorted_expresion)){
        error_buffer_write(this->error_buffer, "Failed to convert expresion '%s' to postfix notation!", expresion);

        tokenizer_clean_output_queue(parsed_tokens);

        if(sorted_expresion != NULL){
            queue_destroy(sorted_expresion);
        }

        return false;
    }

    if(!solve(this, sorted_expresion, result)){
        error_buffer_write(this->error_buffer, "Failed to sort expresion '%s'!", expresion);

        tokenizer_clean_output_queue(parsed_tokens);
        queue_destroy(sorted_expresion);

        return false;
    }

    tokenizer_clean_output_queue(parsed_tokens);
    queue_destroy(sorted_expresion);

    return true;
}

bool evaluate_expression_string(evaluator_t *this, string_t *expresion, long long *result){
    CHECK_NULL_ARGUMENT(this);
    CHECK_NULL_ARGUMENT(expresion);
    CHECK_NULL_ARGUMENT(result);

    return evaluate_expression(this, string_get(expresion), result);
}

char *evaluate_error(evaluator_t *this){
    CHECK_NULL_ARGUMENT(this);
    return error_buffer_get(this->error_buffer);
}

void evaluate_clear_error(evaluator_t *this){
    CHECK_NULL_ARGUMENT(this);

    error_buffer_destroy(this->error_buffer);
    this->error_buffer = NULL;
    error_buffer_init(&(this->error_buffer));
}

bool evaluator_convert(evaluator_t *this, list_t *args, unsigned int arg_pos, long long *output){
    CHECK_NULL_ARGUMENT(this);
    CHECK_NULL_ARGUMENT(args);
    CHECK_NULL_ARGUMENT(output);

    stack_value_t *input = NULL;
    list_at(args, arg_pos, &input);

    return _evaluator_convert(this, input, output);
}

void evaluate_register_variable_callback(evaluator_t *this, bool (*variable_resolve_callback)(char *variable_name, long long *value)){
    CHECK_NULL_ARGUMENT(this);
    CHECK_NULL_ARGUMENT(variable_resolve_callback);

    this->variable_resolve_callback = variable_resolve_callback;
}

//------------------------------------------------------------------------------
// common

static bool is_function(evaluator_t *this, char *s){
    CHECK_NULL_ARGUMENT(s);
    CHECK_NULL_ARGUMENT(this);

    for(unsigned int i = 0; i < list_count(this->func_records); i++){
        evaluator_function_record_t *head = NULL;
        list_at(this->func_records, i, &head);

        if(strcmp(head->func, s) == 0){
            return true;
        }
    }

    return false;
}

static bool is_operator(evaluator_t *this, char *s){
    CHECK_NULL_ARGUMENT(this);

    if(strlen(s) != 1){
        return false;
    }

    for(unsigned int i = 0; i < list_count(this->op_records); i++){
        evaluator_op_record_t *head = NULL;
        list_at(this->op_records, i, &head);

        if(s[0] == head->op){
            return true;
        }
    }

    return false;
}

static evaluator_op_record_t *get_op_record(evaluator_t *this, char *s){
    CHECK_NULL_ARGUMENT(this);
    CHECK_NULL_ARGUMENT(s);

    if(strlen(s) != 1){
        error("Operator can be only one char!");
    }

    for(unsigned int i = 0; i < list_count(this->op_records); i++){
        evaluator_op_record_t *head = NULL;
        list_at(this->op_records, i, &head);

        if(s[0] == head->op){
            return head;
        }
    }

    error("Can't found operator! Use is_operator() before calling get_op_record()!");
}

static evaluator_function_record_t *get_func_record(evaluator_t *this, char *s){
    CHECK_NULL_ARGUMENT(this);
    CHECK_NULL_ARGUMENT(s);

    for(unsigned int i = 0; i < list_count(this->func_records); i++){
        evaluator_function_record_t *head = NULL;
        list_at(this->func_records, i, &head);

        if(strcmp(head->func, s) == 0){
            return head;
        }
    }

    error("Can't found operator! Use is_function() before calling get_func_record()!");
}

static bool can_be_variable(evaluator_t *this, char *s){
    if(s == NULL)
        return false;

    if(strlen(s) == 1){
        if(is_operator(this, s) == true){
            return false;
        }
    }

    if(is_function(this, s) == true){
        return false;
    }

    for(unsigned i = 0; s[i] != '\0'; i++){
        char x = s[i];
        if((isalnum(x) == 0) && (x != '_') && (i > 0))
            return false;

        if((isalpha(x) == 0) && (x != '_') && (i == 0))
            return false;
    }

    return true;
}

static bool _evaluator_convert(evaluator_t *this, stack_value_t *input, long long *output){
    if(input->is_num == true){
        *output =  input->payload.number;
        return true;
    }
    else{
        if(is_number(input->payload.text)){
            str_to_num(input->payload.text, output);
            return true;
        }

        if(can_be_variable(this, input->payload.text)){
            if(this->variable_resolve_callback != NULL){
                if((*this->variable_resolve_callback)(input->payload.text, output) == true){
                    return true;
                }
                else{
                    error_buffer_write(this->error_buffer, "Variable '%s' cannot be resolved!", input->payload.text);
                }
            }
        }
        error_buffer_write(this->error_buffer, "Cannot resolve argument '%s'.", input->payload.text);
    }

    return false;
}

//------------------------------------------------------------------------------
// Parsing

static void parse(char *input, queue_t **output){
    CHECK_NULL_ARGUMENT(input);
    CHECK_NULL_ARGUMENT(output);
    CHECK_NOT_NULL_ARGUMENT(*output);

    tokenizer_t *tokenizer = NULL;

    tokenizer_init(&tokenizer);
    tokenizer_tokenize_char_string(tokenizer, input);
    tokenizer_end(tokenizer, output);
}

//------------------------------------------------------------------------------
// Sorting infix to postfix

static bool is_right_parenthesis(char *s){
    if(strcmp(s, ")") == 0)
        return true;
    else
        return false;
}

static bool is_left_parenthesis(char *s){
    if(strcmp(s, "(") == 0)
        return true;
    else
        return false;
}

static char *stack_char_peek(stack_t *stack){
    char *ptr = NULL;
    stack_peek(stack, (void *)&ptr);
    return ptr;
}

static bool sort_infix_to_postfix(evaluator_t *this, queue_t *input, queue_t **output){
    CHECK_NULL_ARGUMENT(input);
    CHECK_NULL_ARGUMENT(output);
    CHECK_NOT_NULL_ARGUMENT(*output);

    bool retVal = false;
    stack_t *operator_stack = NULL;

    stack_init(&operator_stack, sizeof(char *));
    queue_init(output, sizeof(char *));

    for(unsigned int i = 0; i < list_count(input); i++){
        token_t *token = NULL;
        list_at((list_t *)input, i, (void *)&token);

        if(is_number(token->token)){
            queue_append((*output), &token->token);
        }
        else if(is_function(this, token->token)){
            stack_push(operator_stack, &token->token);
        }
        else if(can_be_variable(this, token->token)){
            stack_push((*output), &token->token);
        }
        else if(is_operator(this, token->token)){
            while(stack_count(operator_stack) > 0 && !is_left_parenthesis(stack_char_peek(operator_stack))){
                evaluator_op_record_t *token_record = get_op_record(this, token->token);
                evaluator_op_record_t *stack_record = get_op_record(this, stack_char_peek(operator_stack));

                if(token_record->associativity == left){
                    if(token_record->precedence <= stack_record->precedence){
                        char *data = NULL;
                        stack_pop(operator_stack, &data);
                        queue_append((*output), &data);
                    }
                    else{
                        break;
                    }
                }
                else{
                    if(token_record->precedence < stack_record->precedence){
                        char *data = NULL;
                        stack_pop(operator_stack, &data);
                        queue_append((*output), &data);
                    }
                    else{
                        break;
                    }
                }
            }
            stack_push(operator_stack, &token->token);
        }
        else if(is_left_parenthesis(token->token)){
            stack_push(operator_stack, &token->token);
        }
        else if(is_right_parenthesis(token->token)){
            while(!is_left_parenthesis(stack_char_peek(operator_stack))){
                if(stack_count(operator_stack) == 0){
                    error_buffer_write(this->error_buffer, "Misleaded parentheses in expression processing!");
                    goto _end;
                }
                else{
                    char *data = NULL;
                    stack_pop(operator_stack, &data);
                    queue_append((*output), &data);
                }
            }

            volatile char *to_delete = NULL;
            stack_pop(operator_stack, &to_delete);

            if(stack_count(operator_stack) > 0){
                if(is_function(this, stack_char_peek(operator_stack))){
                    char *data = NULL;
                    stack_pop(operator_stack, &data);
                    queue_append((*output), &data);
                }
            }
        }
        else{
            error_buffer_write(this->error_buffer, "Found token that is not recognized! Token: '%s'.", token->token);
            goto _end;
        }
    }

    while(stack_count(operator_stack) > 0){
        char *token = NULL;
        stack_pop(operator_stack, &token);

        if(is_left_parenthesis(token) || is_right_parenthesis(token)){
            error_buffer_write(this->error_buffer, "Misleaded parentheses in expression processing!");
            goto _end;
        }

        queue_append((*output), &token);
    }

    retVal = true;

_end:
    stack_destroy(operator_stack);
    return retVal;
}

//------------------------------------------------------------------------------
// solver

static void append_to_stack_from_string(stack_t *stack, char *string, queue_t *to_free){
    CHECK_NULL_ARGUMENT(stack);
    CHECK_NULL_ARGUMENT(string);
    CHECK_NULL_ARGUMENT(to_free);

    stack_value_t *value = dynmem_calloc(1, sizeof(stack_value_t));
    value->is_num = false;
    value->payload.text = string;

    queue_append(to_free, (void *)&value);
    stack_push(stack, (void *)&value);
}

static void append_to_stack_from_number(stack_t *stack, long long number, queue_t *to_free){
    CHECK_NULL_ARGUMENT(stack);
    CHECK_NULL_ARGUMENT(to_free);

    stack_value_t *value = dynmem_calloc(1, sizeof(stack_value_t));
    value->is_num = true;
    value->payload.number = number;

    queue_append(to_free, (void *)&value);
    stack_push(stack, (void *)&value);
}

static bool solve(evaluator_t *this, queue_t *rpn_expresion, long long *result){
    CHECK_NULL_ARGUMENT(this);
    CHECK_NULL_ARGUMENT(rpn_expresion);
    CHECK_NULL_ARGUMENT(result);

    bool retVal = false;

    stack_t *stack = NULL;
    stack_init(&stack, sizeof(void *));

    queue_t *to_free = NULL;
    queue_init(&to_free, sizeof(void *));

    for(unsigned int i = 0; i < list_count(rpn_expresion); i++){
        char *token = NULL;
        list_at((list_t *)rpn_expresion, i, (void *)&token);

        if(is_number(token)){
            append_to_stack_from_string(stack, token, to_free);
        }
        else if(can_be_variable(this, token)){
            append_to_stack_from_string(stack, token, to_free);
        }
        else if(is_operator(this, token) || is_function(this, token)){
            bool (*func)(evaluator_t *this, long long *result, list_t *args) = NULL;
            unsigned int argc_needed = 0;

            if(is_operator(this, token)){
                evaluator_op_record_t *op_record = get_op_record(this, token);
                func = op_record->compute_fun;
                argc_needed = op_record->arg_count;
            }

            if(is_function(this, token)){
                evaluator_function_record_t *func_record = get_func_record(this, token);
                func = func_record->compute_fun;
                argc_needed = func_record->arg_count;
            }

            if(argc_needed > stack_count(stack)){
                error_buffer_write(this->error_buffer, "Not enough values in stack! This mean syntax error in expresion!");
                goto _end;
            }

            list_t *args = NULL;
            list_init(&args, sizeof(stack_value_t *));

            long long op_result = 0;

            for(unsigned int i = 0; i < argc_needed; i++){
                stack_value_t *tmp = NULL;
                stack_pop(stack, &tmp);
                list_insert(args, 0, &tmp); //reverse order of arguments
            }

            if(!(*(func))(this, &op_result, args)){
                goto _end;
            }

            //objects that was in this list will be free using to_free list
            list_destroy(args);

            append_to_stack_from_number(stack, op_result, to_free);
        }
        else{
            error_buffer_write(this->error_buffer, "Found token that is not recognized! This mean syntax error in expresion!");
            goto _end;
        }
    }

    if(stack_count(stack) > 1){
        error_buffer_write(this->error_buffer, "Some values left in stacks after computation done! Probably syntax error!");
        goto _end;
    }
    else if(stack_count(stack) == 0){
        error_buffer_write(this->error_buffer, "No result is found on stack! This mean syntax error in expresion!");
        goto _end;
    }
    else{
        stack_value_t *tmp = NULL;
        stack_pop(stack, (void *)&tmp);

        if(!_evaluator_convert(this, tmp, result)){
            error_buffer_write(this->error_buffer, "Can't get result from stack! This mean syntax error in expresion!");
            goto _end;
        }

        retVal = true;
    }

_end:

    while(queue_count(to_free) > 0){
        void *tmp = NULL;
        queue_windraw(to_free, &tmp);
        dynmem_free(tmp);
    }

    queue_destroy(to_free);
    stack_destroy(stack);

    return retVal;
}

//------------------------------------------------------------------------------
// basic math

static bool evaluate_basic_math_add(evaluator_t *this, long long *result, list_t *args){
    long long a = 0;
    long long b = 0;
    if(!evaluator_convert(this, args, 0, &a)) return false;
    if(!evaluator_convert(this, args, 1, &b)) return false;
    *result = a + b;
    return true;
}

static bool evaluate_basic_math_sub(evaluator_t *this, long long *result, list_t *args){
    long long a = 0;
    long long b = 0;
    if(!evaluator_convert(this, args, 0, &a)) return false;
    if(!evaluator_convert(this, args, 1, &b)) return false;
    *result = a - b;
    return true;
}

static bool evaluate_basic_math_mul(evaluator_t *this, long long *result, list_t *args){
    long long a = 0;
    long long b = 0;
    if(!evaluator_convert(this, args, 0, &a)) return false;
    if(!evaluator_convert(this, args, 1, &b)) return false;
    *result = a * b;
    return true;
}

static bool evaluate_basic_math_div(evaluator_t *this, long long *result, list_t *args){
    long long a = 0;
    long long b = 0;
    if(!evaluator_convert(this, args, 0, &a)) return false;
    if(!evaluator_convert(this, args, 1, &b)) return false;
    *result = a / b;
    return true;
}

static bool evaluate_basic_math_pow(evaluator_t *this, long long *result, list_t *args){
    long long a = 0;
    long long b = 0;
    if(!evaluator_convert(this, args, 0, &a)) return false;
    if(!evaluator_convert(this, args, 1, &b)) return false;
    *result = (long long)pow(a, b);
    return true;
}

static bool evaluate_basic_math_log(evaluator_t *this, long long *result, list_t *args){
    long long a = 0;
    if(!evaluator_convert(this, args, 0, &a)) return false;
    *result = (long long)log10(a);
    return true;
}

static bool evaluate_basic_math_log2(evaluator_t *this, long long *result, list_t *args){
    long long a = 0;
    if(!evaluator_convert(this, args, 0, &a)) return false;
    *result = (long long)log2(a);
    return true;
}

void evaluate_load_basic_math(evaluator_t *this){
    CHECK_NULL_ARGUMENT(this);

    evaluate_append_operator(this, '+', 10, left, 2, &evaluate_basic_math_add);
    evaluate_append_operator(this, '-', 10, left, 2, &evaluate_basic_math_sub);
    evaluate_append_operator(this, '*', 20, left, 2, &evaluate_basic_math_mul);
    evaluate_append_operator(this, '/', 20, left, 2, &evaluate_basic_math_div);
    evaluate_append_operator(this, '^', 30, right, 2, &evaluate_basic_math_pow);
    evaluate_append_function(this, "log10", 1, &evaluate_basic_math_log);
    evaluate_append_function(this, "log2", 1, &evaluate_basic_math_log2);
}
