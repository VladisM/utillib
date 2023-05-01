/**
 * @defgroup convert_group Convert
 *
 * @brief Convert strings into integers.
 *
 * This module is set of functions that are usefull when one need convert
 * strings to numbers. For example when reading files or parsing user input.
 *
 * There is various functions to determine if string is number in specified base
 * and one global is_number() that check string no matter what base it is.
 *
 * To convert string into number there is str_to_num() function that will do
 * exactly that. This function also handle different numeric base automatically.
 *
 * All conversions are made using long long type, so basically 64bits, if you
 * want to store result in smaller variable, there is function can_fit_in() to
 * check if number can be safely stored.
 *
 * Lets assume we have string that contain some number, for first, we will check
 * if number is correct, then we will convert it and put in uint8_t variable.
 *
 * @code{.c}
 * char *input_string = "0x1FF";
 * long long tmp = 0;
 * uint8_t out = 0;
 *
 * if(!is_number(input_string))
 *     return false;   //string isn't valid number
 *
 * if(!str_to_num(input_string, &tmp))
 *     return false;   //conversion failed
 *
 * if(!can_fit_in(tmp, sizeof out))
 *     return false;   //cannot be stored in output variable
 *
 * return true;
 * @endcode
 *
 * @note HEX format have to start with 0x prefix, OCT format have to
 * start with leading zero, DEC format is only one allowed to have sign
 * before number itself but can't have leading zeros, finally BIN format,
 * although not specified by C standart, is using 0b as prefix.
 *
 * @ingroup utils_group
 *
 * @{
 */

#ifndef CONVERT_H_included
#define CONVERT_H_included

#include <stdbool.h>
#include <stddef.h>
#include <utillib/core.h>

/**
 * @brief Check if string is number.
 *
 * @param s Pointer to string.
 *
 * @return true/false
 *
 * This function chose right base according standart C convension. So,
 * 0x for HEX, leading 0 for OCT and so on.
 */
extern bool is_number(char *s);
extern bool is_number_1(string_t *s);

/**
 * @brief Heck if string is HEX coded number.
 *
 * @param s Pointer to string.
 *
 * @return true/false
 */
extern bool is_hex_number(char *s);
extern bool is_hex_number_1(string_t *s);

/**
 * @brief Heck if string is DEC coded number.
 *
 * @param s Pointer to string.
 *
 * @return true/false
 */
extern bool is_dec_number(char *s);
extern bool is_dec_number_1(string_t *s);

/**
 * @brief Heck if string is OCT coded number.
 *
 * @param s Pointer to string.
 *
 * @return true/false
 */
extern bool is_oct_number(char *s);
extern bool is_oct_number_1(string_t *s);

/**
 * @brief Heck if string is BIN coded number.
 *
 * @param s Pointer to string.
 *
 * @return true/false
 */
extern bool is_bin_number(char *s);
extern bool is_bin_number_1(string_t *s);

/**
 * @brief Convert given string to number.
 *
 * @param s Pointer to string.
 * @param x Pointer to variable where result will be stored.
 *
 * @return True or false to signalize if convert was successful.
 */
extern bool str_to_num(char *s, long long *x);
extern bool str_to_num_1(string_t *s, long long *x);

/**
 * @brief Check if given number can be fitted in specified type.
 *
 * @param x Number to check.
 * @param size Size of specified data type.
 *
 * @return true/false
 */
extern bool can_fit_in(long long x, size_t size);

#endif

/**
 * @}
 */
