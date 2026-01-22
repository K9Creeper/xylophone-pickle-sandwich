/// ------------
/// @file vesa.h

#ifndef DRIVERS_VESA_H
#define DRIVERS_VESA_H

#include <kernel/data-structures/drivers/vesa.h>

// Not sure how many there actually are...
#define VESA_MODE_SIZE 111

extern int vesa_init(void);
extern void vesa_destroy(void);

extern int vesa_set_mode(uint16_t mode);

extern vesa_mode_t* vesa_get_all_modes(uint16_t* count);

#endif