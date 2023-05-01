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

bool str_to_num(char *s, long long *x){
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

    *x = tmp;

    return true;
}

bool str_to_num_1(string_t *s, long long *x){
    CHECK_NULL_ARGUMENT(s);
    return str_to_num(string_get(s), x);
}

bool can_fit_in(long long x, size_t size){
    long long unsigned max_size = 1;

    if(size < sizeof(unsigned long long)){
        for(size_t i = 0; i < (size * 8); i++){
            max_size *= 2;
        }
        max_size -= 1;
    }
    else if(size == sizeof(unsigned long long)){
        max_size = ULLONG_MAX;
    }
    else{
        return false;
    }

    if(((unsigned long long)x) <= max_size)
        return true;
    else
        return false;
}
