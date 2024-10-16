#include "convert.h"

#include <utillib/core.h>

#include <stdbool.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <stdio.h>
#include <limits.h>

bool is_number(char *s){
    return is_hex_number(s) || is_dec_number(s) || is_oct_number(s) || is_bin_number(s);
}

bool is_number_1(string_t *s){
    CHECK_NULL_ARGUMENT(s);
    return is_number(string_get(s));
}

bool is_hex_number(char *s){
    CHECK_NULL_ARGUMENT(s);

    if(strncmp(s, "0x", 2) != 0)
        return false;

    for(unsigned i = 2; s[i] != '\0'; i++){
        char x = s[i];
        if(isxdigit(x) == 0) return false;
    }

    return true;
}

bool is_hex_number_1(string_t *s){
    CHECK_NULL_ARGUMENT(s);
    return is_hex_number(string_get(s));
}

bool is_dec_number(char *s){
    CHECK_NULL_ARGUMENT(s);

    if(s[0] == '0')
        return false;

    for(unsigned i = 0; s[i] != '\0'; i++){
        char x = s[i];
        char xp1 = s[i + 1];
        if(x == '-' && i == 0 && xp1 != '\0') continue;
        if(isdigit(x) == 0) return false;
    }

    return true;
}

bool is_dec_number_1(string_t *s){
    CHECK_NULL_ARGUMENT(s);
    return is_dec_number(string_get(s));
}

bool is_oct_number(char *s){
    CHECK_NULL_ARGUMENT(s);

    if(s[0] != '0')
        return false;

    for(unsigned i = 1; s[i] != '\0'; i++){
        char x = s[i];
        if(isdigit(x) == 0 || x == '8' || x == '9' ) return false;
    }

    return true;
}

bool is_oct_number_1(string_t *s){
    CHECK_NULL_ARGUMENT(s);
    return is_oct_number(string_get(s));
}

bool is_bin_number(char *s){
    CHECK_NULL_ARGUMENT(s);

    if(strncmp(s, "0b", 2) != 0)
        return false;

    for(int i = 2; s[i] != '\0'; i++){
        if(s[i] != '1' && s[i] != '0') return false;
    }

    return true;
}

bool is_bin_number_1(string_t *s){
    CHECK_NULL_ARGUMENT(s);
    return is_bin_number(string_get(s));
}

bool str_to_num_signed(char *s, intmax_t *x){
    CHECK_NULL_ARGUMENT(s);
    CHECK_NULL_ARGUMENT(x);

    if(is_number(s) == false)
        return false;

    long long tmp = 0;

    if(is_hex_number(s)){
        tmp = strtoll(s + 2, NULL, 16);
    }
    else if(is_dec_number(s)){
        tmp = strtoll(s, NULL, 10);
    }
    else if(is_oct_number(s)){
        tmp = strtoll(s + 1, NULL, 8);
    }
    else if(is_bin_number(s)){
        tmp = strtoll(s + 2, NULL, 2);
    }
    else{
        return false;
    }

    //should never happen but for good feeling
    if(sizeof(intmax_t) < sizeof(long long)){
        if(can_fit_in_size_signed((intmax_t)tmp, sizeof(intmax_t))){
            return false;
        }
    }

    *x = tmp;

    return true;
}

bool str_to_num_unsigned(char *s, uintmax_t *x){
    CHECK_NULL_ARGUMENT(s);
    CHECK_NULL_ARGUMENT(x);

    if(is_number(s) == false)
        return false;

    unsigned long long tmp = 0;

    if(is_hex_number(s)){
        tmp = strtoull(s + 2, NULL, 16);
    }
    else if(is_dec_number(s)){
        tmp = strtoull(s, NULL, 10);
    }
    else if(is_oct_number(s)){
        tmp = strtoull(s + 1, NULL, 8);
    }
    else if(is_bin_number(s)){
        tmp = strtoull(s + 2, NULL, 2);
    }
    else{
        return false;
    }

    //should never happen but for good feeling
    if(sizeof(uintmax_t) < sizeof(unsigned long long)){
        if(can_fit_in_size_signed((uintmax_t)tmp, sizeof(uintmax_t))){
            return false;
        }
    }

    *x = tmp;

    return true;
}

bool str_to_num_signed_string(string_t *s, intmax_t *x){
    CHECK_NULL_ARGUMENT(s);
    return str_to_num_signed(string_get(s), x);
}

bool str_to_num_unsigned_string(string_t *s, uintmax_t *x){
    CHECK_NULL_ARGUMENT(s);
    return str_to_num_unsigned(string_get(s), x);
}

bool can_fit_in_size_unsigned(uintmax_t x, size_t size){
    return can_fit_in_bits_unsigned(x, size * CHAR_BIT);
}

bool can_fit_in_bytes_unsigned(uintmax_t x, unsigned bytes){
    return can_fit_in_bits_unsigned(x, bytes * 8);
}

bool can_fit_in_bits_unsigned(uintmax_t x, unsigned bits){
    uintmax_t max_size = 1;

    if(bits == (sizeof(uintmax_t) * CHAR_BIT)){
        max_size = UINTMAX_MAX;
    }
    else if(bits < (sizeof(uintmax_t) * CHAR_BIT)){
        max_size = (1ULL << bits) - 1;
    }
    else{
        return false;
    }

    return (x <= max_size);
}

bool can_fit_in_size_signed(intmax_t x, size_t size){
    return can_fit_in_bits_signed(x, size * CHAR_BIT);
}

bool can_fit_in_bytes_signed(intmax_t x, unsigned bytes){
    return can_fit_in_bits_signed(x, bytes * 8);
}

bool can_fit_in_bits_signed(intmax_t x, unsigned bits){
    intmax_t max_size = 1;
    intmax_t min_size = 0;

    if(bits == (sizeof(intmax_t) * CHAR_BIT)){
        max_size = INTMAX_MAX;
        min_size = INTMAX_MIN;
    }
    else if(bits < (sizeof(intmax_t) * CHAR_BIT)){
        max_size = (1LL << (bits - 1)) - 1;
        min_size = -(1LL << (bits - 1));
    }
    else{
        return false;
    }

    return (x >= min_size && x <= max_size);
}
