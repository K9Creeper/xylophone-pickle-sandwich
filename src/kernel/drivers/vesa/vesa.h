/// ------------
/// @file vesa.h

#ifndef DRIVERS_VESA_H
#define DRIVERS_VESA_H

#include <kernel/data-structures/drivers/vesa.h>

// Not sure how many there actually are...
#define VESA_MODE_SIZE 111

typedef enum vesa_return_status_e {
    VESA_OK,
    VESA_ERR,
    VESA_UNSUPPORTED
} vesa_return_status_t;

extern vesa_return_status_t vesa_init(void);
extern void vesa_destroy(void);

extern vesa_return_status_t vesa_set_mode(uint16_t mode);

extern vesa_mode_t* vesa_get_all_modes(uint16_t* count);

#endif