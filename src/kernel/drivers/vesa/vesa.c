/// ------
/// vesa.c
/// @brief This file contains the VESA driver implementation for the project.

#include "vesa.h"

#include <stdio.h>
#include <memory.h>

#include "../../bios32/bios32.h"

#include <data-structures/kernel-context/kernel-context.h>

// Not sure how many there actually are...
#define VESA_MODE_SIZE 64

// kernel-main.c
extern kernel_context_t* kernel_context;

static vbe_info_block_t vbe_info;
static vea_mode_t vesa_modes[VESA_MODE_SIZE];
static vea_mode_t current_mode;

static void load_vbe()
{
    registers16_t reg_in = {0};
    registers16_t reg_out = {0};
    reg_in.ax = 0x4F00;
    reg_in.di = 0x9500;
    bios32_service(0x10, &reg_in, &reg_out);
    memcpy((unsigned char *)(&vbe_info), (unsigned char *)(0x9500), sizeof(vbe_info_block_t));
}

static void vesa_get_mode(uint16_t mode, vbe_mode_info_t *mode_info)
{
    registers16_t reg_in = {0};
    registers16_t reg_out = {0};
    reg_in.ax = 0x4F01;
    reg_in.cx = mode;
    reg_in.di = 0x9000;
    bios32_service(0x10, &reg_in, &reg_out);
    memcpy((unsigned char *)(mode_info), (unsigned char *)(0x9000), sizeof(vbe_mode_info_t));
}

static vea_mode_t *vesa_get_modes(void)
{
    vbe_info_block_t *block = &vbe_info;
    vbe_mode_info_t tmp;

    uint8_t c = 0;

    memset((unsigned char *)(vesa_modes), 0, sizeof(vesa_modes));

    uint16_t *list = (uint16_t *)(block->videoModes);
    uint16_t mode_number = *list++;
    for (; mode_number != 0xffff && c < VESA_MODE_SIZE; mode_number = *list++)
    {
        vesa_get_mode(mode_number, &tmp);

        if ((tmp.attributes & 0x90) != 0x90)
            continue;

        if (tmp.memoryModel != 4 && tmp.memoryModel != 6)
            continue;

        vesa_modes[c].number = mode_number;
        memcpy((unsigned char *)(&vesa_modes[c].info), (unsigned char *)(&tmp), sizeof(vbe_mode_info_t));

        c++;
    }
    return vesa_modes;
}

static void _vesa_set_mode(uint32_t mode)
{
    registers16_t reg_in = {0};
    registers16_t reg_out = {0};
    reg_in.ax = 0x4F02;
    reg_in.bx = mode;
    bios32_service(0x10, &reg_in, &reg_out);
}

static void _vesa_exit_mode(void)
{
    registers16_t reg_in = {0};
    registers16_t reg_out = {0};
    bios32_service(0x10, &reg_in, &reg_out);
}

void vesa_map_buffer(void);

int vesa_init(void)
{
    load_vbe();
    if (vbe_info.signature[0] != 'V' || vbe_info.signature[1] != 'E' || vbe_info.signature[2] != 'S' || vbe_info.signature[3] != 'A')
    {
        printf("VESA not supported\n");
        return 1;
    }

    vesa_get_modes();

    return 0;
}

void vesa_destroy(void)
{
}

// Basically a wrapper
int vesa_set_mode(uint32_t mode)
{
    for (uint8_t i = 0; i < VESA_MODE_SIZE; i++)
    {
        if (vesa_modes[i].number != mode)
            continue;
        _vesa_set_mode(mode);

        current_mode = vesa_modes[i];

        kernel_context->video_state.is_text_mode = false;
        kernel_context->video_state.lfb = current_mode.info.physbase;
        kernel_context->video_state.width = current_mode.info.width;
        kernel_context->video_state.height = current_mode.info.height;
        kernel_context->video_state.bpp = current_mode.info.bpp;
        kernel_context->video_state.pitch = current_mode.info.pitch;

        vesa_map_buffer();
        return 0;
    }

    return 1;
}

int vesa_set_specs(uint32_t width, uint32_t height)
{
    const uint16_t bpp = 32;

    for (uint8_t i = 0; i < VESA_MODE_SIZE; i++)
    {
        if (vesa_modes[i].number == 0)
            break;

        if (vesa_modes[i].info.width != width || vesa_modes[i].info.height != height || vesa_modes[i].info.bpp != bpp)
            continue;

        _vesa_set_mode(vesa_modes[i].number);
        current_mode = vesa_modes[i];

        kernel_context->video_state.is_text_mode = false;
        kernel_context->video_state.lfb = current_mode.info.physbase;
        kernel_context->video_state.width = current_mode.info.width;
        kernel_context->video_state.height = current_mode.info.height;
        kernel_context->video_state.bpp = current_mode.info.bpp;
        kernel_context->video_state.pitch = current_mode.info.pitch;

        vesa_map_buffer();
        return 0;
    }

    return 1;
}

vea_mode_t *vesa_current_mode(void)
{
    return &current_mode;
}

void vesa_map_buffer(void)
{
    if (!kernel_context->paging_manager.is_initialized)
        return;
    const uint32_t lfb = current_mode.info.physbase;
    const uint32_t lfb_size = ((current_mode.info.height - 1) * current_mode.info.pitch + ((current_mode.info.width - 1) * (current_mode.info.bpp / 8)));
    const uint32_t lfb_max = lfb + lfb_size;

    paging_manager_identity_allocate_range(&kernel_context->paging_manager, lfb, lfb_max, true, true);
}