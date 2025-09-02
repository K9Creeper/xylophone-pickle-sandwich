/// ------------
/// @file vesa.h

#ifndef DRIVERS_VESA_H
#define DRIVERS_VESA_H

#include <data-structures/vesa/vesa.h>

// Not sure how many there actually are...
#define VESA_MODE_SIZE 64

extern int vesa_init(void);

// FIXME: Implement
//
extern void vesa_destroy(void);

extern int vesa_set_mode(uint32_t mode);
extern int vesa_set_specs(uint32_t width, uint32_t height);

extern vesa_mode_t* vesa_current_mode(void);

extern vesa_mode_t* vesa_get_all_modes(void);

#endif