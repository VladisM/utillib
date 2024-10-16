/**
 * @defgroup options_group Options
 *
 * @brief Simplify argument parsing!
 *
 * Create new "parser object" with options_new, append few flags with
 * options_append_flag/options funtions, call options_parse and then
 * check result with options_is_flag_set.
 *
 * @todo Add documentations
 *
 * @ingroup cli_group
 *
 * @{
 */

#ifndef OPTIONS_H_included
#define OPTIONS_H_included

#include <utillib/core.h>
#include <utillib/utils.h>

#include <stdint.h>
#include <stdbool.h>

typedef struct {
    list_t *options_configured;
    list_t *options_given;
    array_t *arguments_given;
    char *version;
    char *prog_name;
    char *about;
    char *footer;
    unsigned output_width;
    error_buffer_t *error_buffer;
} options_t;

extern void options_init(options_t **this, char *version, char *prog_name);
extern void options_destroy(options_t *this);

extern void options_append_about(options_t *this, char *about);
extern void options_append_footer(options_t *this, char *footer);

extern int options_parse(options_t *this, int argc, char **argv);
extern char **options_get_argv(options_t *this);

extern void options_append_flag_1(options_t *this, char *short_form, char *help);
extern void options_append_flag_2(options_t *this, char *long_form, char *help);
extern void options_append_flag_3(options_t *this, char *short_form, char *long_form, char *help);

extern void options_append_string_option_1(options_t *this, char *short_form, char *help);
extern void options_append_string_option_2(options_t *this, char *long_form, char *help);
extern void options_append_string_option_3(options_t *this, char *short_form, char *long_form, char *help);

extern void options_append_number_option_1(options_t *this, char *short_form, char *help);
extern void options_append_number_option_2(options_t *this, char *long_form, char *help);
extern void options_append_number_option_3(options_t *this, char *short_form, char *long_form, char *help);

extern void options_append_section(options_t *this, char *name, char *help);

extern void options_print_help(options_t *this);
extern void options_print_version(options_t *this);
extern void options_print_ussage(options_t *this);

extern bool options_is_flag_set(options_t *this, char *s);
extern bool options_is_option_set(options_t *this, char *s);

extern bool options_get_option_value_string(options_t *this, char *s, char **val);
extern bool options_get_option_value_number(options_t *this, char *s, intmax_t *val);
extern void options_get_option_multiple_values_string(options_t *this, char *s, list_t **val);

extern void options_config_output_width(options_t *this, unsigned width);

extern char *options_error(options_t *this);

#endif

/**
 * @}
 */
