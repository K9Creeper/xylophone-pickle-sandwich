/// ------
/// vesa.h
/// @brief This file contains the VESA interface for the project.

#ifndef DRIVERS_VESA_H
#define DRIVERS_VESA_H 

#include <data-structures/drivers/vesa.h>

extern int vesa_init(void);

// FIXME: Implement
//
extern void vesa_destroy(void);

extern int vesa_set_mode(uint32_t mode);
extern int vesa_set_specs(uint32_t width, uint32_t height);

extern vea_mode_t* vesa_current_mode(void);

#endif