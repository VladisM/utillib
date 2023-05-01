#include "mif.h"

#include "common.h"

#include <utillib/core.h>

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <limits.h>

/**
 * @brief Recursive function to print in binary
 * @warning This is sketchy shit!
 * @param output At first call, this have to be NULL, subsequent calls have this set to string_t object.
 * @param input number to deal with it
 * @return char* first iteration will return char string with converted number ... hopefully ...
 */
static char *_printf_mif_number_bin(string_t *output, uintmax_t input){
    if(output == NULL){
        string_t *tmp_obj = NULL;
        string_init(&tmp_obj);
        _printf_mif_number_bin(tmp_obj, input);
        char *tmp_char = dynmem_strdup(string_get(tmp_obj));
        string_destroy(tmp_obj);
        return tmp_char;
    }
    else{
        if(input > 1) _printf_mif_number_bin(output, input / 2);
        string_append(output, (input % 2 == 1) ? "1" : "0");
        return NULL;
    }
}

static char *_printf_mif_number(char *fmt, uintmax_t input){
    CHECK_NULL_ARGUMENT(fmt);

    int len_needed = 0;
    char *tmp = NULL;

    len_needed = snprintf(NULL, 0, fmt, input);
    tmp = (char *)dynmem_calloc(len_needed + 1, sizeof(char));
    sprintf(tmp, fmt, input);
    return tmp;
}

static char *printf_mif_number(mif_radix_t number_format, uintmax_t input){
    char *result = NULL;

    switch(number_format){
        case RADIX_HEX: result = _printf_mif_number("%"PRIXMAX, input); break;
        case RADIX_OCT: result = _printf_mif_number("%"PRIoMAX, input); break;
        case RADIX_DEC: result = _printf_mif_number("%"PRIdMAX, input); break;
        case RADIX_UNS: result = _printf_mif_number("%"PRIuMAX, input); break;
        case RADIX_BIN: result = _printf_mif_number_bin(NULL, input); break;
        default:
            error("Wanted to print unsupported format, missing check???");
            break;
    }

    return result;
}

static bool check_if_radix_supported(mif_radix_t radix){
    switch(radix){
        case RADIX_HEX:
        case RADIX_OCT:
        case RADIX_DEC:
        case RADIX_UNS:
        case RADIX_BIN:
            return true;
        default:
            return false;
    }
}

static char *get_radix_string(mif_radix_t radix){
    switch(radix){
        case RADIX_UNK: return "UNK";
        case RADIX_HEX: return "HEX";
        case RADIX_BIN: return "BIN";
        case RADIX_OCT: return "OCT";
        case RADIX_DEC: return "DEC";
        case RADIX_UNS: return "UNS";
        default: return NULL;
    }
}

void mif_init(mif_file_t **file, unsigned size, size_t data_size){
    CHECK_NULL_ARGUMENT(file);
    CHECK_NOT_NULL_ARGUMENT(*file);

    mif_file_t *tmp = NULL;

    tmp = (mif_file_t *)dynmem_calloc(1, sizeof(mif_file_t));
    tmp->data = (uintmax_t *)dynmem_calloc(size, sizeof(uintmax_t));

    tmp->settings.data_width = data_size * CHAR_BIT;
    tmp->settings.depth = size;

    tmp->settings.data_radix = RADIX_UNK;
    tmp->settings.address_radix = RADIX_UNK;

    *file = tmp;
}

void mif_destroy(mif_file_t *file){
    CHECK_NULL_ARGUMENT(file);

    if(file->data != NULL)
        dynmem_free(file->data);

    dynmem_free(file);
}

unsigned mif_size(mif_file_t *file){
    CHECK_NULL_ARGUMENT(file);
    return file->settings.depth;
}

uintmax_t *mif_data(mif_file_t *file){
    CHECK_NULL_ARGUMENT(file);
    return file->data;
}

void mif_config_radixes(mif_file_t *file, mif_radix_t data_radix, mif_radix_t address_radix){
    CHECK_NULL_ARGUMENT(file);

    file->settings.address_radix = address_radix;
    file->settings.data_radix = data_radix;
}

bool mif_set(mif_file_t *file, unsigned offset, unsigned len, uintmax_t *data){
    CHECK_NULL_ARGUMENT(file);
    CHECK_NULL_ARGUMENT(data);

    unsigned tmp_sum = 0;

    tmp_sum += offset;
    tmp_sum += len;

    if(tmp_sum > file->settings.depth){
        return false;
    }

    for(unsigned i = 0; i < len; i++){
        file->data[offset + i] = data[i];
    }

    return true;
}

bool mif_write(mif_file_t *file, char *filename){
    CHECK_NULL_ARGUMENT(file);
    CHECK_NULL_ARGUMENT(filename);

    string_t *tmp = NULL;
    bool retVal = true;

    if(!check_if_radix_supported(file->settings.address_radix))
        return false;

    if(!check_if_radix_supported(file->settings.data_radix))
        return false;

    string_init(&tmp);

    char *address_radix = get_radix_string(file->settings.address_radix);
    char *data_radix = get_radix_string(file->settings.data_radix);

    string_appendf(tmp, "DEPTH = %d;\r\n", file->settings.depth);
    string_appendf(tmp, "WIDTH = %d;\r\n", file->settings.data_width);
    string_appendf(tmp, "ADDRESS_RADIX = %s;\r\n", address_radix);
    string_appendf(tmp, "DATA_RADIX = %s;\r\n", data_radix);

    string_append(tmp, "CONTENT\r\nBEGIN\r\n");

    for(unsigned i = 0; i < file->settings.depth; i++){
        char *addr_string = printf_mif_number(file->settings.address_radix, i);
        char *data_string = printf_mif_number(file->settings.data_radix, file->data[i]);

        string_appendf(tmp, "%s : %s\r\n", addr_string, data_string);

        dynmem_free(addr_string);
        dynmem_free(data_string);
    }

    string_append(tmp, "END;\r\n");

    if(!utillib_file_write_file(tmp, filename))
        retVal = false;

    string_destroy(tmp);
    return retVal;
}
