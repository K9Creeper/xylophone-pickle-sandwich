/// ------------
/// @file vesa.c

#include "vesa.h"

#include <memory.h>
#include <kernel/util.h>

#include "../../bios32/bios32.h"
#include <kernel/data-structures/kernel-context.h>

extern kernel_context_t *kernel_context;

REGISTER_LOW_MEMORY_VARIABLE(vbe_info_block_t, vbe_info);

static uint16_t vesa_mode_count;
static vesa_mode_t vesa_modes[VESA_MODE_SIZE];

static uint8_t vbe_call(uint16_t ax, uint16_t bx, uint16_t cx, void *buffer, registers16_t* out)
{
    registers16_t reg_in = {0};
    registers16_t reg_out = {0};

    reg_in.ax = ax;
    reg_in.cx = cx;
    reg_in.bx = bx;

    if (buffer != NULL)
    {
        reg_in.di = FP_OFF(buffer);
        reg_in.es = FP_SEG(buffer);
    }

    kernel_bios32_service(0x10, &reg_in, &reg_out);

    if (out != NULL)
        *out = reg_out;

    if (!VBE_IS_FUNCTION_SUPPORTED(reg_out.ax))
        return 0x4E;

    return VBE_AH(reg_out.ax);
}

static uint8_t load_vbe()
{
    return vbe_call(0x4F00, 0, 0, &vbe_info, NULL);
}

static uint8_t vesa_get_mode(uint16_t mode, vbe_mode_info_t *mode_info)
{
    return vbe_call(0x4F01, 0, mode, mode_info, NULL);
}

static uint16_t vesa_current_mode(void)
{
    registers16_t reg_out = {0};
    uint8_t ret = vbe_call(0x4F03, 0, 0, NULL, &reg_out);

    if (ret)
        return (uint8_t)-1;

    return (uint16_t)((reg_out.bx) & 0x3FFF);
}

static vesa_mode_t *vesa_get_modes(void)
{
    vbe_info_block_t *block = &vbe_info;
    REGISTER_LOW_MEMORY_VARIABLE(vbe_mode_info_t, tmp);

    memset((uint8_t *)&tmp, 0, sizeof(vbe_mode_info_t));
    memset((unsigned char *)(vesa_modes), 0, sizeof(vesa_modes));

    uint8_t c = 0;
    uint16_t *list = (uint16_t *)(block->videoModes);

    for (uint16_t mode_number = *list++;
         mode_number != 0xFFFF && c < VESA_MODE_SIZE &&
         ((uint32_t)list < ((uint32_t)block + sizeof(vbe_info_block_t)));
         mode_number = *list++)
    {
        if (vesa_get_mode(mode_number, &tmp))
            continue;

        if ((tmp.attributes & 0x90) != 0x90)
            continue;

        if (tmp.memoryModel != 4 && tmp.memoryModel != 6)
            continue;

        int duplicate = 0;
        for (uint8_t i = 0; i < c; i++)
        {
            if (vesa_modes[i].number == mode_number)
            {
                duplicate = 1;
                break;
            }
        }
        if (duplicate)
            continue;

        vesa_modes[c].number = mode_number;
        memcpy((unsigned char *)(&vesa_modes[c].info),
               (unsigned char *)(&tmp),
               sizeof(vbe_mode_info_t));

        c++;
    }

    vesa_mode_count = c;
    return vesa_modes;
}

static uint8_t _vesa_set_mode(uint16_t mode)
{
    return vbe_call(0x4F02, mode, 0, NULL, NULL);
}

int vesa_init(void)
{
    if (load_vbe())
        return -1;

    if (vbe_info.signature[0] != 'V' || vbe_info.signature[1] != 'E' ||
        vbe_info.signature[2] != 'S' || vbe_info.signature[3] != 'A')
        return 1;
    
    vesa_get_modes();
    return 0;
}

void vesa_destroy(void)
{
    uint16_t current_mode_number = vesa_current_mode();

    if (current_mode_number != (uint16_t)-1 && _vesa_set_mode(0x0) == 0)
    {
        for (int i = 0; i < VESA_MODE_SIZE; i++)
        {
            if (vesa_modes[i].number != current_mode_number)
                continue;
            break;
        }
    }
}

int vesa_set_mode(uint16_t mode)
{
    for (uint8_t i = 0; i < vesa_mode_count; i++)
    {
        if (vesa_modes[i].number != mode)
            continue;

        if (_vesa_set_mode(mode))
            break;

        vesa_mode_t current_mode = vesa_modes[i];

        kernel_context->video_state.is_text_mode = 0;
        kernel_context->video_state.lfb = current_mode.info.physbase;
        kernel_context->video_state.width = current_mode.info.width;
        kernel_context->video_state.height = current_mode.info.height;
        kernel_context->video_state.bits_per_pixel = current_mode.info.bpp;
        kernel_context->video_state.bytes_per_pixel = current_mode.info.bpp / 8;
        kernel_context->video_state.pitch = current_mode.info.pitch;

        const uint32_t lfb_size = current_mode.info.width *
                                  current_mode.info.height *
                                  (current_mode.info.bpp / 8);
        kernel_context->video_state.size = lfb_size;

        return 0;
    }

    return 1;
}

vesa_mode_t *vesa_get_all_modes(uint16_t *count)
{
    if (count != NULL)
        *count = vesa_mode_count;

    return vesa_modes;
}