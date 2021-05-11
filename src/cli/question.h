/**
 * @defgroup question_group Question
 *
 * @brief User keyboard input handling.
 *
 * @todo missing documentation
 *
 * @ingroup cli_group
 *
 * @{
 */

#ifndef QUESTION_H_included
#define QUESTION_H_included

#include <stdbool.h>

extern bool question_yes_no(char *s);
extern bool question_YES_no(char *s);
extern bool question_yes_NO(char *s);

extern char *user_input_string(char *s);
extern long long user_input_number(char *s);

#endif

/**
 * @}
 */
