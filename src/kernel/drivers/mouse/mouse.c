/// -------
/// mouse.c
/// @brief This file

#include "mouse.h"

#include <stdbool.h>
#include <memory.h>
#include <stddef.h>

#include <data-structures/kernel/kernel-context.h>
#include <data-structures/kernel/kernel-interrupts.h>
#include "../../interrupts/interrupt-request.h"

#define NUM_OF_MOUSE_HANDLES 16

// kernel-main.c
extern kernel_context_t *kernel_context;

static bool is_initialized;
static mouse_input_handle_t input_handles[NUM_OF_MOUSE_HANDLES];
static mouse_info_t mouse_info;
static uint8_t cycle = 0;
static char mouse_byte[3];

static void mouse_wait(uint8_t x)
{
    uint32_t timeout = 100000;
    if (x == 0)
    {
        while (timeout--)
            if ((inportb(0x64) & 1) == 1)
                return;
        return;
    }
    else
    {
        while (timeout--)
            if (!(inportb(0x64) & 2))
            {
                return;
            }
        return;
    }
}

static void mouse_write(uint8_t x) // unsigned char
{
    mouse_wait(1);
    outportb(0x64, 0xD4);
    mouse_wait(1);

    outportb(0x60, x);
}

static uint8_t mouse_read(void)
{
    mouse_wait(0);
    return inportb(0x60);
}

static void mouse_handler(registers_t*);

void mouse_init(void)
{
    for (uint8_t i = 0; i < NUM_OF_MOUSE_HANDLES; i++)
        input_handles[i] = NULL;

    is_initialized = true;

    uint8_t temp;

    mouse_wait(1);
    outportb(0x64, 0xA8);

    mouse_wait(1);
    outportb(0x64, 0x20);
    mouse_wait(0);
    temp = (inportb(0x60) | 2);
    mouse_wait(1);
    outportb(0x64, 0x60);
    mouse_wait(1);
    outportb(0x60, temp);

    mouse_write(0xF6);
    mouse_read();

    mouse_write(0xF4);
    mouse_read();

    kernel_interrupt_request_set_handle(12, (kernel_interrupt_request_handle_t)mouse_handler);
}

int mouse_add_input_handle(mouse_input_handle_t handle)
{
    for (uint8_t i = 0; i < NUM_OF_MOUSE_HANDLES; i++)
        if (!input_handles[i])
        {
            input_handles[i] = handle;
            return i;
        }

    return -1;
}

void mouse_remove_input_handle(int index)
{
    if(index < 0 || index > NUM_OF_MOUSE_HANDLES) return;

    input_handles[index] = NULL;
}

mouse_info_t mouse_get_info(void)
{
    return mouse_info;
}

static mouse_info_t temp;
static void mouse_handler(registers_t* __regs__ /* unused */)
{
    
    if (cycle == 0)
    {
        mouse_byte[0] = mouse_read();

        if (MOUSE_LEFT_BUTTON(mouse_byte[0]))
        {
            temp.curr_state[0] = 1;
        }
        else
        {
            temp.curr_state[0] = 0;
        }

        if (MOUSE_RIGHT_BUTTON(mouse_byte[0]))
        {
            temp.curr_state[2] = 1;
        }
        else
        {
            temp.curr_state[2] = 0;
        }
        cycle++;
    }
    else if (cycle == 1)
    {
        mouse_byte[1] = mouse_read();
        cycle++;
    }
    else
    {
        mouse_byte[2] = mouse_read();

        temp.delta_x = mouse_byte[1];
        temp.delta_y = -mouse_byte[2];

        temp.x += temp.delta_x;
        temp.y += temp.delta_y;

        if (temp.x < 0)
            temp.x = 0;
        if (temp.y < 0)
            temp.y = 0;

        cycle = 0;
    }

    // Finished 1 cycle
    if (cycle == 0)
    {
        if(!kernel_context->video_state.is_text_mode)
        {
            if(temp.x > kernel_context->video_state.width)
                temp.x = kernel_context->video_state.width;
            else if(temp.x < 0)
                temp.x = 0;

            if(temp.y > kernel_context->video_state.height)
                temp.y = kernel_context->video_state.height;
            else if(temp.y < 0)
                temp.y = 0;             
        }

        mouse_info = temp;
        for (uint8_t i = 0; i < 16; i++)
            if (input_handles[i] != NULL)
                ((mouse_input_handle_t)(input_handles[i]))(&mouse_info);
        
        memcpy(temp.prev_state, temp.curr_state, 3);
        memset(temp.curr_state, 0x00, 3);
    }
}