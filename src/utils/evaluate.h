/**
 * @defgroup evaluate_group Evaluate
 *
 * @brief Evaluate integer mathematic expresion.
 *
 * This module let you solve mathematical expresions written as string in infix
 * form. String will be parsed using tokenizer, then sorted to RPN and as last
 * step, RPN representation get solved and result is returned.
 *
 * By using two methods one can simply implement own mathematical functions, see
 * evaluate_append_operator() and evaluate_append_function().
 *
 * This module is also capable to deal with variables by registering one optional
 * callback. Please see evaluate_register_variable_callback().
 *
 * Very simple example of ussage follows.
 *
 * @code{.c}
 * evaluator_t *evaluator = NULL;
 * long long result = 0;
 *
 * evaluate_new(&evaluator);
 * evaluate_load_basic_math(evaluator);
 *
 * if(!evaluate_expression(evaluator, expresion, &result)){
 *     //failed, use evaluator_error()
 * }
 * else{
 *     //print result
 * }
 *
 * evaluate_destroy(evaluator);
 * @endcode
 *
 * @todo test what will happend in case of syntax errors
 *
 * @ingroup utils_group
 *
 * @{
 */

#ifndef EVALUATE_H_included
#define EVALUATE_H_included

#include <utillib/core.h>

#include "error_buffer.h"

/**
 * @brief Associativity for mathematical operations.
 */
typedef enum {
    left = 0,
    right
} evaluator_op_associativity_t;

/**
 * @brief Evaluator object.
 */
typedef struct {
    error_t *error_buffer;
    list_t *op_records;
    list_t *func_records;
    bool (*variable_resolve_callback)(char *variable_name, long long *value);
} evaluator_t;

/**
 * @brief Create new evaluator object.
 *
 * @param evaluator Pointer to pointer to NULL where new object will be stored.
 */
void evaluate_new(evaluator_t **evaluator);

/**
 * @brief Destroy evaluator object. Free it's dynamic memory.
 *
 * @param evaluator Pointer to object.
 */
void evaluate_destroy(evaluator_t *evaluator);

/**
 * @brief Append function into math engine.
 *
 * @param this Pointer to evaluator instance.
 * @param func_name Name of function as will be written in expresion.
 * @param argc Count of arguments that function need.
 * @param compute_function Call back to resolve function
 */
void evaluate_append_function(
    evaluator_t *this,
    char *func_name,
    unsigned argc,
    bool (*compute_function)(evaluator_t *this, long long *result, list_t *args)
);

/**
 * @brief Append operator into math engine.
 *
 * @param this Pointer to evaluator instance.
 * @param op Character as will be written in expresion.
 * @param precedence Operator precedence.
 * @param associativity Operator associativity.
 * @param argc Count of arguments that function need.
 * @param compute_function Call back to resolve function
 *
 * @note You probably don't want to use this, you can create all basic mathematical
 * operation by calling evaluate_load_basic_math()
 */
void evaluate_append_operator(
    evaluator_t *this,
    char op,
    int precedence,
    evaluator_op_associativity_t associativity,
    unsigned argc,
    bool (*compute_function)(evaluator_t *this, long long *result, list_t *args)
);

/**
 * @brief Function to convert arguments to real numbers.
 *
 * @note This is that one function that you will need to use if you wanna
 * convert argument in math computing function implementation to real number.
 *
 * @param this Pointer to evaluator instance.
 * @param args List with arguments given to math computing function.
 * @param arg_pos Position of argument that you want resolve.
 * @param output Where to store result.
 * @return true If everything was OK.
 * @return false When for example number cann't be converted due to syntax error.
 *
 * @note It this gonna fail, error is filled up, you should return false and
 * evaluate_expresion then can return false too. After this, you can get error
 * msg using evaluate_error().
 *
 * @note If evaluator have registered variable resolving callback function
 * (see void evaluate_register_variable_callback()) then this method also try
 * resolve arguments as variables.
 */
bool evaluator_convert(evaluator_t *this, list_t *args, unsigned int arg_pos, long long *output);

/**
 * @brief Solve mathematical expression with given evaluator.
 *
 * @param this Evaluator to be used.
 * @param expresion Expresion to be solved.
 * @param result Where to store result.
 * @return true If everything was OK ther true is returned.
 * @return false If, for example, syntax error occurred, then false is returned.
 *
 * @note Appropiate error msg can be obtained by evaluate_error()
 */
bool evaluate_expression(evaluator_t *this, char *expresion, long long *result);

/**
 * @brief Solve mathematical expression with given evaluator.
 *
 * @param this Evaluator to be used.
 * @param expresion Expresion to be solved.
 * @param result Where to store result.
 * @return true If everything was OK ther true is returned.
 * @return false If, for example, syntax error occurred, then false is returned.
 *
 * @note Appropiate error msg can be obtained by evaluate_error()
 */
bool evaluate_expression_string(evaluator_t *this, string_t *expresion, long long *result);

/**
 * @brief Give error msg.
 *
 * @param this Pointer to evaluator object.
 * @return char* Error message.
 */
char *evaluate_error(evaluator_t *this);

/**
 * @brief Clear error message so new one can appear. :)
 *
 * @param this Pointer to evaluator object.
 */
void evaluate_clear_error(evaluator_t *this);

/**
 * @brief Load basic math operations into evaluator.
 *
 * Basic math support contain:
 * A + B; A - B; A * B; A / B; A^n
 * log10(x), log2(x)
 *
 * @param this Pointer to evaluator object.
 */
void evaluate_load_basic_math(evaluator_t *this);

/**
 * @brief Register callback function to resolve variable values.
 *
 * @param this Evaluator to register.
 * @param variable_resolve_callback Function that will for given string name
 * find actual value of variable and return it.
 *
 * @note Registered function have to return false if variable is not found,
 * and true if value is set correctly. Place to store result is given by pointer.
 * Name of variable also.
 *
 * @note Storage of variables have to be done outside of this library in user code.
 */
void evaluate_register_variable_callback(
    evaluator_t *this,
    bool (*variable_resolve_callback)(char *variable_name, long long *value)
);

#endif

/**
 * @}
 */
