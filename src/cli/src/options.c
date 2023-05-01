#include "options.h"

#include "config.h"

#include <utillib/core.h>
#include <utillib/utils.h>

#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

typedef enum {
    FLAG,
    OPTION,
    SECTION
} option_type_t;

typedef enum {
    STRING,
    NUMBER,
    VOID
} options_arg_type_t;

typedef struct {
    option_type_t option_type;
    options_arg_type_t arg_type;
    char *short_form;
    char *long_form;
    char *help_text;
} flag_t;

typedef struct {
    char *flag;
    char *value;
} set_option_t;

static void options_append(options_t *this, option_type_t type, options_arg_type_t arg_type, char *short_form, char *long_form, char *help);
static void print_auto_newline(FILE *FP, char *s, unsigned max_len, unsigned first_line_len, unsigned indentation, unsigned indentation_first_line);

void options_init(options_t **this, char *version, char *prog_name){
    CHECK_NULL_ARGUMENT(this);
    CHECK_NULL_ARGUMENT(version);
    CHECK_NULL_ARGUMENT(prog_name);
    CHECK_NOT_NULL_ARGUMENT(*this);

    options_t *tmp = (options_t *)dynmem_malloc(sizeof(options_t));

    tmp->options_configured = NULL;
    tmp->options_given = NULL;
    tmp->arguments_given = NULL;
    tmp->error_buffer = NULL;

    list_init(&(tmp->options_configured), sizeof(flag_t));
    list_init(&(tmp->options_given), sizeof(set_option_t));
    array_init(&(tmp->arguments_given), sizeof(char *), 16);

    error_buffer_init(&(tmp->error_buffer));

    tmp->prog_name = prog_name;
    tmp->version = version;
    tmp->about = NULL;
    tmp->footer = NULL;
    tmp->output_width = DEFAULT_CLI_LINE_WIDTH;

    *this = tmp;
}

void options_destroy(options_t *this){
    CHECK_NULL_ARGUMENT(this);

    list_destroy(this->options_configured);
    list_destroy(this->options_given);
    array_destroy(this->arguments_given);
    error_buffer_destroy(this->error_buffer);

    dynmem_free(this);
}

void options_append_about(options_t *this, char *about){
    CHECK_NULL_ARGUMENT(this);
    CHECK_NULL_ARGUMENT(about);

    this->about = about;
}

void options_append_footer(options_t *this, char *footer){
    CHECK_NULL_ARGUMENT(this);
    CHECK_NULL_ARGUMENT(footer);

    this->footer = footer;
}

int options_parse(options_t *this, int argc, char **argv){
    CHECK_NULL_ARGUMENT(this);
    CHECK_NULL_ARGUMENT(argv);

    bool flags_finished = false;
    int _argc = 0;

    for(int i = 1; i < argc; i++){
        char *arg = argv[i];

        bool is_flag = false;
        bool is_option = false;
        bool is_long_form = false;

        set_option_t tmp_set;

        if(flags_finished == false){
            for(unsigned bagr = 0; bagr < list_count(this->options_configured); bagr++){
                flag_t tmp_flag;
                list_at(this->options_configured, bagr, (void *)&tmp_flag);

                if(tmp_flag.option_type == SECTION)
                    continue;

                if(strncmp(arg, "--" , 2) == 0 && tmp_flag.long_form != NULL){
                    if(strcmp(arg + 2, tmp_flag.long_form) == 0){
                        if(tmp_flag.option_type == OPTION)
                            is_option = true;
                        else
                            is_flag = true;
                        is_long_form = true;
                        break;
                    }
                }

                if(strncmp(arg, "-" , 1) == 0 && tmp_flag.short_form != NULL){
                    if(strcmp(arg + 1, tmp_flag.short_form) == 0){
                        if(tmp_flag.option_type == OPTION)
                            is_option = true;
                        else
                            is_flag = true;
                        break;
                    }
                }
            }

            if(is_option == true){
                if(i + 1 < argc){
                    char *val = argv[++i];
                    tmp_set.flag = arg + (is_long_form ? 2 : 1);
                    tmp_set.value = val;
                    list_append(this->options_given, (void *)&tmp_set);
                    continue;
                }
                else{
                    error("Missing option argument!");
                }
            }

            if(is_flag == true){
                tmp_set.flag = arg + (is_long_form ? 2 : 1);
                tmp_set.value = NULL;
                list_append(this->options_given, (void *)&tmp_set);
                continue;
            }

            if(strcmp(arg, "--") == 0){
                flags_finished = true;
                continue;
            }
        }

        while((unsigned)(_argc + 1) > array_get_size(this->arguments_given)){
            array_enlarge(this->arguments_given);
        }

        array_set(this->arguments_given, _argc++, (void *)&arg);
    }

    return _argc;
}

char **options_get_argv(options_t *this){
    CHECK_NULL_ARGUMENT(this);
    return (char **)array_get_data(this->arguments_given);
}

bool options_is_flag_set(options_t *this, char *s){
    CHECK_NULL_ARGUMENT(this);
    CHECK_NULL_ARGUMENT(s);

    for(unsigned i = 0; i < list_count(this->options_given); i++){
        set_option_t tmp_set;
        list_at(this->options_given, i, (void *)&tmp_set);

        if(tmp_set.flag != NULL && strcmp(tmp_set.flag, s) == 0)
            return true;
    }
    return false;
}

bool options_is_option_set(options_t *this, char *s){
    CHECK_NULL_ARGUMENT(this);
    CHECK_NULL_ARGUMENT(s);

    return options_is_flag_set(this, s);
}

void options_get_option_multiple_values_string(options_t *this, char *s, list_t **val){
    CHECK_NULL_ARGUMENT(this);
    CHECK_NULL_ARGUMENT(s);
    CHECK_NULL_ARGUMENT(val);
    CHECK_NOT_NULL_ARGUMENT(*val);

    list_init(val, sizeof(char *));

    for(unsigned i = 0; i < list_count(this->options_given); i++){
        set_option_t tmp_set;
        list_at(this->options_given, i, (void *)&tmp_set);

        if(tmp_set.flag != NULL && strcmp(tmp_set.flag, s) == 0){
            list_append(*val, (void *)&tmp_set.value);
        }
    }
}

bool options_get_option_value_string(options_t *this, char *s, char **val){
    CHECK_NULL_ARGUMENT(this);
    CHECK_NULL_ARGUMENT(s);
    CHECK_NULL_ARGUMENT(val);
    CHECK_NOT_NULL_ARGUMENT(*val);

    for(unsigned i = 0; i < list_count(this->options_given); i++){
        set_option_t tmp_set;
        list_at(this->options_given, i, (void *)&tmp_set);

        if(tmp_set.flag != NULL && strcmp(tmp_set.flag, s) == 0){
            *val = tmp_set.value;
            return true;
        }
    }

    error_buffer_write(this->error_buffer, "Argument %s is not given.", s);

    return false;
}

bool options_get_option_value_number(options_t *this, char *s, long long *val){
    CHECK_NULL_ARGUMENT(this);
    CHECK_NULL_ARGUMENT(s);
    CHECK_NULL_ARGUMENT(val);

    char *tmp_string = NULL;

    if(!options_get_option_value_string(this, s, &tmp_string))
        return false;

    if(!is_number(tmp_string)){
        error_buffer_write(this->error_buffer, "Given value of %s argument is not number!", s);
        return false;
    }

    if(!str_to_num(tmp_string, val))
        error("Function is_number returned true but can't convert!");

    return true;
}

void options_print_help(options_t *this){
    CHECK_NULL_ARGUMENT(this);

    options_print_version(this);
    options_print_ussage(this);

    fprintf(stdout, "\r\n");

    if(this->about != NULL){
        print_auto_newline(stdout, this->about, this->output_width, this->output_width, 0, 0);
        fprintf(stdout, "\r\n");
    }

    unsigned short_form_len = 0;
    unsigned long_form_len = 0;

    for(unsigned i = 0; i < list_count(this->options_configured); i++){
        flag_t tmp;
        list_at(this->options_configured, i, (void *)&tmp);

        if(tmp.option_type == SECTION)
            continue;

        if(tmp.short_form != NULL && tmp.option_type == FLAG && short_form_len < strlen(tmp.short_form))
            short_form_len = strlen(tmp.short_form);

        if(tmp.long_form != NULL && tmp.option_type == FLAG && long_form_len < strlen(tmp.long_form))
            long_form_len = strlen(tmp.long_form);

        if(tmp.short_form != NULL && tmp.option_type == OPTION && short_form_len < (strlen(tmp.short_form) + 4))
            short_form_len = strlen(tmp.short_form) + 4;

        if(tmp.long_form != NULL && tmp.option_type == OPTION && long_form_len < (strlen(tmp.long_form) + 4))
            long_form_len = strlen(tmp.long_form) + 4;
    }

    short_form_len += 1; //have "-" as prefix
    long_form_len += 2; //have "--" as prefix

    char *short_form = (char *)dynmem_malloc(sizeof(char) * (short_form_len + 1));
    char *long_form = (char *)dynmem_malloc(sizeof(char) * (long_form_len + 1));

    for(unsigned i = 0; i < list_count(this->options_configured); i++){
        flag_t tmp;
        list_at(this->options_configured, i, (void *)&tmp);

        memset((void *)short_form, '\0', short_form_len);
        memset((void *)long_form, '\0', long_form_len);

        if(tmp.option_type == SECTION){
            fprintf(stdout, "\r\n%s\r\n", tmp.long_form);

            if(tmp.help_text != NULL){
                print_auto_newline(stdout, tmp.help_text, this->output_width - 1, this->output_width - 1, 1, 1);
                fprintf(stdout, "\r\n");
            }
        }
        else{
            if(tmp.option_type == OPTION){
                if(tmp.short_form != NULL) sprintf(short_form, "%s%s%s", "-", tmp.short_form, " val");
                if(tmp.long_form != NULL) sprintf(long_form, "%s%s%s", "--", tmp.long_form, " val");
            }

            if(tmp.option_type == FLAG){
                if(tmp.short_form != NULL) sprintf(short_form, "%s%s", "-", tmp.short_form);
                if(tmp.long_form != NULL) sprintf(long_form, "%s%s", "--", tmp.long_form);
            }

            fprintf(stdout, " %-*s %-*s ", short_form_len, short_form, long_form_len, long_form);
            unsigned indentation = short_form_len + long_form_len + 3;
            print_auto_newline(stdout, tmp.help_text, this->output_width, this->output_width - indentation, indentation, 0);
        }
    }

    dynmem_free(short_form);
    dynmem_free(long_form);

    fprintf(stdout, "\r\n");

    if(this->footer != NULL){
        fprintf(stdout, "\r\n");
        print_auto_newline(stdout, this->footer, this->output_width, this->output_width, 0, 0);
    }

    fflush(stdout);
}

void options_print_version(options_t *this){
    CHECK_NULL_ARGUMENT(this);
    fprintf(stdout, "%s version %s\r\n", this->prog_name, this->version);
}

void options_print_ussage(options_t *this){
    CHECK_NULL_ARGUMENT(this);
    char *str = list_count(this->options_configured) > 0 ? "[options] " : "";

    fprintf(stdout, "Ussage: %s %s[arguments]\r\n", this->prog_name, str);
}

void options_config_output_width(options_t *this, unsigned width){
    CHECK_NULL_ARGUMENT(this);
    this->output_width = width;
}

void options_append_flag_1(options_t *this, char *short_form, char *help){
    CHECK_NULL_ARGUMENT(this);
    CHECK_NULL_ARGUMENT(help);
    CHECK_NULL_ARGUMENT(short_form);
    options_append(this, FLAG, VOID, short_form, NULL, help);
}

void options_append_flag_2(options_t *this, char *long_form, char *help){
    CHECK_NULL_ARGUMENT(this);
    CHECK_NULL_ARGUMENT(help);
    CHECK_NULL_ARGUMENT(long_form);
    options_append(this, FLAG, VOID, NULL, long_form, help);
}

void options_append_flag_3(options_t *this, char *short_form, char *long_form, char *help){
    CHECK_NULL_ARGUMENT(this);
    CHECK_NULL_ARGUMENT(help);
    CHECK_NULL_ARGUMENT(short_form);
    CHECK_NULL_ARGUMENT(long_form);
    options_append(this, FLAG, VOID, short_form, long_form, help);
}

void options_append_string_option_1(options_t *this, char *short_form, char *help){
    CHECK_NULL_ARGUMENT(this);
    CHECK_NULL_ARGUMENT(help);
    CHECK_NULL_ARGUMENT(short_form);
    options_append(this, OPTION, STRING, short_form, NULL, help);
}

void options_append_string_option_2(options_t *this, char *long_form, char *help){
    CHECK_NULL_ARGUMENT(this);
    CHECK_NULL_ARGUMENT(help);
    CHECK_NULL_ARGUMENT(long_form);
    options_append(this, OPTION, STRING, NULL, long_form, help);
}

void options_append_string_option_3(options_t *this, char *short_form, char *long_form, char *help){
    CHECK_NULL_ARGUMENT(this);
    CHECK_NULL_ARGUMENT(help);
    CHECK_NULL_ARGUMENT(short_form);
    CHECK_NULL_ARGUMENT(long_form);
    options_append(this, OPTION, STRING, short_form, long_form, help);
}

void options_append_number_option_1(options_t *this, char *short_form, char *help){
    CHECK_NULL_ARGUMENT(this);
    CHECK_NULL_ARGUMENT(help);
    CHECK_NULL_ARGUMENT(short_form);
    options_append(this, OPTION, NUMBER, short_form, NULL, help);
}

void options_append_number_option_2(options_t *this, char *long_form, char *help){
    CHECK_NULL_ARGUMENT(this);
    CHECK_NULL_ARGUMENT(help);
    CHECK_NULL_ARGUMENT(long_form);
    options_append(this, OPTION, NUMBER, NULL, long_form, help);
}

void options_append_number_option_3(options_t *this, char *short_form, char *long_form, char *help){
    CHECK_NULL_ARGUMENT(this);
    CHECK_NULL_ARGUMENT(help);
    CHECK_NULL_ARGUMENT(short_form);
    CHECK_NULL_ARGUMENT(long_form);
    options_append(this, OPTION, NUMBER, short_form, long_form, help);
}

void options_append_section(options_t *this, char *name, char *help){
    CHECK_NULL_ARGUMENT(this);
    CHECK_NULL_ARGUMENT(name);
    options_append(this, SECTION, VOID, NULL, name, help);
}

static void options_append(options_t *this, option_type_t type, options_arg_type_t arg_type, char *short_form, char *long_form, char *help){
    flag_t new;

    new.option_type = type;
    new.arg_type = arg_type;
    new.help_text = help;
    new.long_form = long_form;
    new.short_form = short_form;

    list_append(this->options_configured, (void *)&new);
}

static void print_auto_newline(FILE *FP, char *s, unsigned max_len, unsigned first_line_len, unsigned indentation, unsigned indentation_first_line){
    unsigned printed = 0;
    bool first_line = true;

    for(unsigned i = 0; i < indentation_first_line; i++, printed++)
        fprintf(FP, " ");

    while(*s != '\0'){
        unsigned chars_left = 0;

        while(*(s + chars_left) != '\0'){
            if(*(s + chars_left) == ' ') break;
            else chars_left++;
        }

        unsigned len = first_line ? first_line_len : max_len;

        if((printed + chars_left) >= len){
            fprintf(FP, "\r\n");
            printed = 0;
            first_line = false;
            for(unsigned i = 0; i < indentation; i++, printed++)
                fprintf(FP, " ");
        }

        while(*s != '\0'){
            fprintf(FP, "%c", *s);
            printed++;
            if(*s++ == ' ')
                break;
        }
    }

    fprintf(FP, "\r\n");
}

extern char *options_error(options_t *this){
    CHECK_NULL_ARGUMENT(this);
    return error_buffer_get(this->error_buffer);
}
