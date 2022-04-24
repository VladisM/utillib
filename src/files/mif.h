/**
 * @defgroup mif_file_group Altera/Intel MIF
 *
 * @brief Implementation of Altera memory initialization file format.
 *
 * @todo add support for reading mif files
 * @todo add documentation
 *
 * @ingroup files_group
 *
 * @{
 */

#ifndef MIF_H_included
#define MIF_H_included

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

typedef enum{
    RADIX_UNK = 0,
    RADIX_HEX,
    RADIX_BIN,
    RADIX_OCT,
    RADIX_DEC,
    RADIX_UNS
} mif_radix_t;

typedef struct{
    uintmax_t *data;
    struct{
        mif_radix_t data_radix;
        unsigned data_width;
        mif_radix_t address_radix;
        unsigned depth;
    }settings;
} mif_file_t;

extern void mif_init(mif_file_t **file, unsigned size, size_t data_size);
extern void mif_destroy(mif_file_t *file);

extern unsigned mif_size(mif_file_t *file);
extern uintmax_t *mif_data(mif_file_t *file);

extern void mif_config_radixes(mif_file_t *file, mif_radix_t data_radix, mif_radix_t address_radix);

bool mif_set(mif_file_t *file, unsigned offset, unsigned len, uintmax_t *data);

extern bool mif_write(mif_file_t *file, char *filename);

#endif

/**
 * @}
 */
