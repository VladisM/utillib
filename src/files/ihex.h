/**
 * @defgroup ihex_file_group Intel HEX
 *
 * @brief Implementation of Intel HEX file format.
 *
 * @todo add support for reading ihex files
 * @todo add documentation
 *
 * @ingroup files_group
 *
 * @{
 */

#ifndef IHEX_H_included
#define IHEX_H_included

#include <stdint.h>
#include <stdbool.h>

typedef struct{
    uint8_t *payload;
    uint32_t size;
    uint32_t begin;
    struct{
        bool given;
        uint32_t address;
    }start_linear_address;
} ihex_file_t;

extern void ihex_init(ihex_file_t **file, uint32_t size, uint32_t begin_address);
extern void ihex_destroy(ihex_file_t *file);

extern uint32_t ihex_size(ihex_file_t *file);
extern uint8_t *ihex_data(ihex_file_t *file);

extern void ihex_set_start_linear_address(ihex_file_t *file, uint32_t address);

extern bool ihex_set_relative(ihex_file_t *file, uint32_t offset, uint32_t len, uint8_t *data);
extern bool ihex_set_absolute(ihex_file_t *file, uint32_t address, uint32_t len, uint8_t *data);

extern bool ihex_write(ihex_file_t *file, char *filename);

#endif

/**
 * @}
 */
