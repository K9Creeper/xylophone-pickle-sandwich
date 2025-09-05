/// ------------
/// @file vesa.c

#include "vesa.h"

#include <memory.h>
#include <dbgprintf.h>

#include "../../bios32/bios32.h"
#include <kernel/memory-management/paging-manager.h>
#include <kernel/data-structures/kernel-context.h>

extern kernel_context_t *kernel_context;

static vbe_info_block_t vbe_info;
static vesa_mode_t vesa_modes[VESA_MODE_SIZE];
static vesa_mode_t current_mode;

static uint8_t load_vbe()
{
    registers16_t reg_in = {0};
    registers16_t reg_out = {0};
    reg_in.ax = 0x4F00;
    kernel_bios32_service(0x10, &reg_in, &reg_out);

    if ((uint16_t)(reg_out.ax & 0x00FF) != 0x4F)
        return 2;

    if ((uint16_t)(reg_out.ax >> 8))
        return 1;

    memcpy((unsigned char *)(&vbe_info), (unsigned char *)(reg_out.di), sizeof(vbe_info_block_t));

    return 0;
}

static uint8_t vesa_get_mode(uint16_t mode, vbe_mode_info_t *mode_info)
{
    registers16_t reg_in = {0};
    registers16_t reg_out = {0};
    reg_in.ax = 0x4F01;
    reg_in.cx = mode;
    kernel_bios32_service(0x10, &reg_in, &reg_out);
    if ((uint16_t)(reg_out.ax & 0xFF) != 0x4F)
        return 2;

    if ((uint16_t)(reg_out.ax >> 8))
        return 1;

    memcpy((unsigned char *)(mode_info), (unsigned char *)(reg_out.di), sizeof(vbe_mode_info_t));

    return 0;
}

static vesa_mode_t *vesa_get_modes(void)
{
    vbe_info_block_t *block = &vbe_info;
    vbe_mode_info_t tmp;

    uint8_t c = 0;

    memset((unsigned char *)(vesa_modes), 0, sizeof(vesa_modes));

    uint16_t *list = (uint16_t *)(block->videoModes);
    uint16_t mode_number = *list++;
    for (; mode_number != 0xffff && c < VESA_MODE_SIZE; mode_number = *list++)
    {
        if(vesa_get_mode(mode_number, &tmp)) continue;

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

static uint8_t _vesa_set_mode(uint32_t mode)
{
    registers16_t reg_in = {0};
    registers16_t reg_out = {0};
    reg_in.ax = 0x4F02;
    reg_in.bx = mode;
    kernel_bios32_service(0x10, &reg_in, &reg_out);
    if ((uint16_t)(reg_out.ax & 0x00FF) != 0x4F)
        return 2;

    if ((uint16_t)(reg_out.ax >> 8))
        return 1;

    return 0;
}

static uint8_t vesa_exit_mode(void)
{
    registers16_t reg_in = {0};
    registers16_t reg_out = {0};
    kernel_bios32_service(0x10, &reg_in, &reg_out);

    if ((uint16_t)(reg_out.ax & 0x00FF) != 0x4F)
        return 2;

    if ((uint16_t)(reg_out.ax >> 8))
        return 1;

    return 0;
}

void vesa_map_buffer(void);
void vesa_unmap_buffer(void);

int vesa_init(void)
{
    if (load_vbe())
        return -1;

    if (vbe_info.signature[0] != 'V' || vbe_info.signature[1] != 'E' || vbe_info.signature[2] != 'S' || vbe_info.signature[3] != 'A')
        return 1;

    vesa_get_modes();

    return 0;
}

void vesa_destroy(void)
{
    vesa_unmap_buffer();
}

int vesa_set_mode(uint32_t mode)
{
    for (uint8_t i = 0; i < VESA_MODE_SIZE; i++)
    {
        if (vesa_modes[i].number != mode)
            continue;

        uint8_t ret = _vesa_set_mode(mode);

        if (ret)
            return 2;

        current_mode = vesa_modes[i];

        kernel_context->video_state.is_text_mode = 0;
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

        if (vesa_set_mode(vesa_modes[i].number))
            return 2;

        current_mode = vesa_modes[i];

        kernel_context->video_state.is_text_mode = 0;
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

vesa_mode_t *vesa_current_mode(void)
{
    return &current_mode;
}

vesa_mode_t *vesa_get_all_modes(void)
{
    return vesa_modes;
}

void vesa_map_buffer(void)
{
    if (!kernel_context->paging_manager.is_initialized)
        return;
    const uint32_t lfb = current_mode.info.physbase;
    const uint32_t lfb_size = current_mode.info.width * current_mode.info.height * (current_mode.info.bpp / 8);
    const uint32_t lfb_max = lfb + lfb_size;

    paging_manager_identity_allocate_range(&kernel_context->paging_manager, lfb, lfb_max, 1, 1);
}

void vesa_unmap_buffer(void)
{
    if (!kernel_context->paging_manager.is_initialized)
        return;
    const uint32_t lfb = current_mode.info.physbase;
    const uint32_t lfb_size = current_mode.info.width * current_mode.info.height * (current_mode.info.bpp / 8);
    const uint32_t lfb_max = lfb + lfb_size;

    paging_manager_free_range(&kernel_context->paging_manager, lfb, lfb_max, 0);
}
