#include "ihex.h"

#include "common.h"

#include <utillib/core.h>

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <inttypes.h>
#include <stdio.h>

#define MAX_IHEX_DATA_RECORD_SIZE 16

static inline uint8_t crc_sum(uint8_t crc, uint8_t data){
    return crc + data;
}

static inline uint8_t crc_get(uint8_t crc){
    return ~crc + 1;
}

static bool ihex_data_record(uint16_t address, uint8_t *data, uint8_t len, string_t *output){
    CHECK_NULL_ARGUMENT(output);
    CHECK_NULL_ARGUMENT(data);

    if(len > MAX_IHEX_DATA_RECORD_SIZE){
        return false;
    }

    uint8_t crc = 0;

    string_appendf(output, ":%02"PRIX8, len);
    string_appendf(output, "%04"PRIX16, address);
    string_appendf(output, "00");

    crc = crc_sum(crc, len);
    crc = crc_sum(crc, (uint8_t)address);
    crc = crc_sum(crc, (uint8_t)(address >> 8));

    for(uint8_t i = 0; i < len; i++){
        string_appendf(output, "%02"PRIX8, data[i]);
        crc = crc_sum(crc, data[i]);
    }

    string_appendf(output, "%02"PRIX8, crc_get(crc));
    string_appendf(output, "\r\n");

    return true;
}

static void ihex_end_record(string_t *output){
    CHECK_NULL_ARGUMENT(output);
    string_append(output, ":00000001FF");
    string_appendf(output, "\r\n");
}

static void ihex_extended_linear_address_record(uint16_t address_upper, string_t *output){
    CHECK_NULL_ARGUMENT(output);

    uint8_t crc = 0;

    string_append(output, ":02000004");
    string_appendf(output, "%04"PRIX16, address_upper);

    crc = crc_sum(crc, 0x02U);
    crc = crc_sum(crc, 0x04U);
    crc = crc_sum(crc, (uint8_t)address_upper);
    crc = crc_sum(crc, (uint8_t)(address_upper >> 8));

    string_appendf(output, "%02"PRIX8, crc_get(crc));
    string_appendf(output, "\r\n");
}

static void ihex_start_linear_address_record(uint32_t address, string_t *output){

    CHECK_NULL_ARGUMENT(output);

    uint8_t crc = 0;

    string_append(output, ":04000005");
    string_appendf(output, "%08"PRIX32, address);

    crc = crc_sum(crc, 0x04);
    crc = crc_sum(crc, 0x05);
    crc = crc_sum(crc, (uint8_t)address);
    crc = crc_sum(crc, (uint8_t)(address >> 8) );
    crc = crc_sum(crc, (uint8_t)(address >> 16));
    crc = crc_sum(crc, (uint8_t)(address >> 24));

    string_appendf(output, "%02"PRIX8, crc_get(crc));
    string_appendf(output, "\r\n");
}

void ihex_init(ihex_file_t **file, uint32_t size, uint32_t begin_address){
    CHECK_NULL_ARGUMENT(file);
    CHECK_NOT_NULL_ARGUMENT(*file);

    ihex_file_t *tmp = NULL;

    tmp = (ihex_file_t *)dynmem_calloc(1, sizeof(ihex_file_t));
    tmp->payload = (uint8_t *)dynmem_calloc(size, sizeof(uint8_t));

    tmp->size = size;
    tmp->begin = begin_address;

    tmp->start_linear_address.given = false;
    tmp->start_linear_address.address = 0;

    *file = tmp;
}

void ihex_destroy(ihex_file_t *file){
    CHECK_NULL_ARGUMENT(file);

    if(file->payload != NULL)
        dynmem_free(file->payload);

    dynmem_free(file);
}

uint32_t ihex_size(ihex_file_t *file){
    CHECK_NULL_ARGUMENT(file);
    return file->size;
}

uint8_t *ihex_data(ihex_file_t *file){
    CHECK_NULL_ARGUMENT(file);
    return file->payload;
}

void ihex_set_start_linear_address(ihex_file_t *file, uint32_t address){
    CHECK_NULL_ARGUMENT(file);

    file->start_linear_address.given = true;
    file->start_linear_address.address = address;
}

bool ihex_set_relative(ihex_file_t *file, uint32_t offset, uint32_t len, uint8_t *data){
    CHECK_NULL_ARGUMENT(file);
    CHECK_NULL_ARGUMENT(data);

    uint32_t tmp_sum = 0;

    tmp_sum += offset;
    tmp_sum += len;

    if(tmp_sum > file->size){
        return false;
    }

    for(uint32_t i = 0; i < len; i++){
        file->payload[offset + i] = data[i];
    }

    return true;
}

bool ihex_set_absolute(ihex_file_t *file, uint32_t address, uint32_t len, uint8_t *data){
    CHECK_NULL_ARGUMENT(file);
    return ihex_set_relative(file, address - file->begin, len, data);
}

bool ihex_write(ihex_file_t *file, char *filename){
    CHECK_NULL_ARGUMENT(file);
    CHECK_NULL_ARGUMENT(filename);

    bool i32hex = false;
    bool retVal = true;
    string_t *tmp = NULL;
    uint16_t last_top_address = 0;

    if((file->begin + file->size) > 0xFFFF)
        i32hex = true;

    string_init(&tmp);

    for(uint32_t i = 0; i < file->size; i = i + MAX_IHEX_DATA_RECORD_SIZE){
        uint8_t record_len = MAX_IHEX_DATA_RECORD_SIZE;
        uint16_t current_top_address = (uint16_t)((file->begin + i) >> 16);

        if(i + MAX_IHEX_DATA_RECORD_SIZE > file->size)
            record_len = file->size - i;

        if((i32hex == true) && (last_top_address != current_top_address)){
            last_top_address = current_top_address;

            ihex_extended_linear_address_record(current_top_address, tmp);
        }

        ihex_data_record((uint16_t)(file->begin + i), file->payload + i, record_len, tmp);
    }

    if(file->start_linear_address.given == true){
        ihex_start_linear_address_record(file->start_linear_address.address, tmp);
    }

    ihex_end_record(tmp);

    if(!utillib_file_write_file(tmp, filename))
        retVal = false;

    string_destroy(tmp);

    return retVal;
}
