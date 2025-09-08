/// ---------------------
/// @file window-server.c

#include "window-server.h"

#include <memory.h>
#include <string.h>

#include <kernel/data-structures/kernel-context.h>
#include <kernel/data-structures/window-server/window-server.h>
#include <kernel/data-structures/kthread/kthread.h>
#include "kernel/data-structures/syscalls.h"
#include <kernel/util.h>
#include <kernel/window-server/window-list.h>

#include "../kthread/kthread.h"

extern kernel_context_t *kernel_context;

static uint8_t is_initialized = 0;
static window_t windows[MAX_WINDOWS];
static window_list_t window_list;
static uint32_t *back_buffer = NULL;
static ws_msg_t message_queue[MAX_WS_MESSAGES];
static int msg_head = 0;
static int msg_tail = 0;

static inline uint16_t rgb32_to_16(uint32_t c)
{
    uint8_t r = (c >> 16) & 0xFF;
    uint8_t g = (c >> 8) & 0xFF;
    uint8_t b = c & 0xFF;
    return ((r >> 3) << 11) | ((g >> 2) << 5) | (b >> 3);
}

static int window_compare_z(const void *a, const void *b)
{
    const window_t *wa = (const window_t *)a;
    const window_t *wb = (const window_t *)b;
    return wa->z_index - wb->z_index;
}

static inline int ws_queue_empty() { return (msg_head == msg_tail) ? 1 : 0; }
static inline int ws_queue_full() { return (((msg_tail + 1) % MAX_WS_MESSAGES) == msg_head) ? 1 : 0; }

static inline int wd_queue_empty(window_t* win) { return (win->dispatch_head == win->dispatch_tail) ? 1 : 0; }
static inline int wd_queue_full(window_t* win) { return (((win->dispatch_tail + 1) % MAX_DISPATCH_QUEUE) == win->dispatch_head) ? 1 : 0; }

static inline void fill_framebuffer(void *lfb, int width, int height, int pitch, int bytes_per_pixel, uint32_t color)
{
    for (int y = 0; y < height; y++)
    {
        uint8_t *row = (uint8_t *)lfb + y * pitch;
        if (bytes_per_pixel == 4)
        {
            uint32_t *p32 = (uint32_t *)row;
            for (int x = 0; x < width; x++)
                p32[x] = color;
        }
        else if (bytes_per_pixel == 2)
        {
            uint16_t *p16 = (uint16_t *)row;
            uint16_t c16 = rgb32_to_16(color);
            for (int x = 0; x < width; x++)
                p16[x] = c16;
        }
    }
}

static void blit_16(uint32_t *src, uint8_t *dst, int w, int h, int dst_pitch)
{
    for (int y = 0; y < h; y++)
    {
        uint16_t *d = (uint16_t *)(dst + y * dst_pitch);
        uint32_t *s = src + y * w;
        for (int x = 0; x < w; x++)
            d[x] = rgb32_to_16(s[x]);
    }
}

static void blit_32(uint32_t *src, uint8_t *dst, int w, int h, int dst_pitch)
{
    for (int y = 0; y < h; y++)
    {
        fast_memcpy((void *)(dst + y * dst_pitch), (void *)(src + y * w), w * sizeof(uint32_t));
    }
}

static int ws_enqueue(ws_msg_t *msg)
{
    if (ws_queue_full())
        return -1;
    message_queue[msg_tail] = *msg;
    msg_tail = (msg_tail + 1) % MAX_WS_MESSAGES;
    return 0;
}

static ws_msg_t *ws_dequeue()
{
    if (ws_queue_empty())
        return NULL;
    ws_msg_t* msg = &message_queue[msg_head];
    msg_head = (msg_head + 1) % MAX_WS_MESSAGES;
    return msg;
}

static int wd_enqueue(window_t* win, ws_msg_t msg)
{
    if (wd_queue_full(win))
        return -1;
    win->dispatch_queue[win->dispatch_tail] = msg;
    win->dispatch_tail = (win->dispatch_tail + 1) % MAX_WS_MESSAGES;
    return 0;
}

static ws_msg_t *wd_dequeue(window_t* win)
{
    if (wd_queue_empty(win))
        return NULL;
    ws_msg_t *msg = &win->dispatch_queue[win->dispatch_head];
    win->dispatch_head = (win->dispatch_head + 1) % MAX_WS_MESSAGES;
    return msg;
}

static void (*blit_func)(uint32_t *, uint8_t *, int, int, int);

uint8_t window_server_init(void)
{
    if (is_initialized || kernel_context->video_state.is_text_mode)
        return 1;

    memset((void *)windows, 0, sizeof(windows));
    for (int i = 0; i < MAX_WINDOWS; i++)
        windows[i].id = -1;

    window_list_init(&window_list, windows, MAX_WINDOWS);

    back_buffer = (uint32_t *)kernel_amalloc(kernel_context->video_state.size);
    if (!back_buffer)
        return 1;

    switch (kernel_context->video_state.bits_per_pixel)
    {
    case 16:
        blit_func = blit_16;
        break;
    case 32:
        blit_func = blit_32;
        break;
    default:
        blit_func = NULL;
        break;
    }

    is_initialized = 1;

    kthread_start("window_server_composite", 0, NULL);
    return 0;
}

#include <../lib/syscalls-lib.h>

static int window_server_create_window(ws_msg_t *msg)
{
    int x = msg->x;
    int y = msg->y;

    int32_t packed_dim = (msg->w << 16) | msg->h;

    uint16_t width = (packed_dim >> 16) & 0xFFFF;
    uint16_t height = packed_dim & 0xFFFF;

    if (window_list.size >= MAX_WINDOWS)
        return -1;

    for (int i = 0; i < MAX_WINDOWS; i++)
    {
        window_t *win = &windows[i];
        if (win->id != -1)
            continue;

        win->id = i;
        memset((void *)win->name, 0, MAX_WINDOW_NAME);

        win->x = x;
        win->y = y;
        win->width = width;
        win->height = height;
        win->pitch = width * sizeof(uint32_t);

        win->buffer = (uint32_t *)malloc(win->pitch * height);
        if (!win->buffer)
        {
            win->id = -1;
            return -1;
        }

        win->creator_pid = get_proccess_id();

        fill_framebuffer((void *)win->buffer, width, height, win->pitch, sizeof(uint32_t), 0xFFFFFF);
        atomic_store(&win->buffer_state, BUFFER_STATE_FREE);
        win->z_index = 0;
        
        memset((void*)win->dispatch_queue, 0, sizeof(ws_msg_t) * MAX_DISPATCH_QUEUE);
        
        win->dispatch_head = 0;
        win->dispatch_tail = 0;

        window_list_push(&window_list, win);
        window_list_raise(&window_list, win);

        return win->id;
    }

    return -1;
}

static int window_server_destroy_window(ws_msg_t *msg)
{
    uint8_t is_window = (msg->window_id >= 0 && msg->window_id < MAX_WINDOWS) ? 1 : 0;
    if (is_window)
    {
        window_t *win = &windows[msg->window_id];
        if (win->creator_pid == msg->pid)
        {
            win->id = -1;
            free(windows->buffer);
            return 0;
        }
    }
    return -1;
}

static int window_server_send_message(void *_msg)
{
    ws_msg_t *msg = (ws_msg_t *)_msg;
    if (msg == NULL)
        return -1;

    msg->pid = get_proccess_id();

    int ret = -1;

    switch (msg->type)
    {
    case MSG_CREATE_WINDOW:
        ret = window_server_create_window(msg);
        break;
    case MSG_DESTROY_WINDOW:
        ret = window_server_destroy_window(msg);
        break;
    default:
        ret = ws_enqueue(msg);
        break;
    }

    return ret;
}
REGISTER_SYSCALL(SYSCALLS_WS_SEND_MSG, window_server_send_message);

static void window_server_composite(void)
{
    while (is_initialized)
    {
        int width = kernel_context->video_state.width;
        int height = kernel_context->video_state.height;
        int pitch = kernel_context->video_state.pitch;
        int bpp_bytes = kernel_context->video_state.bytes_per_pixel;

        fill_framebuffer((void *)back_buffer, width, height, pitch, bpp_bytes, 0xFF0000);

        ws_msg_t *msg;
        while (!ws_queue_empty())
        {
            if (msg = ws_dequeue())
            {
                uint8_t is_window = msg->window_id >= 0 && msg->window_id < MAX_WINDOWS;
                window_t *win = NULL;
                if (is_window)
                {
                    win = &windows[msg->window_id];
                    if (win->creator_pid == msg->pid)
                        switch (msg->type)
                        {
                        case MSG_DRAW_READY:
                            atomic_store(&win->buffer_state, BUFFER_STATE_COMMITTED);
                            break;
                        case MSG_MOVE_WINDOW:

                            win->x = msg->x;
                            win->y = msg->y;
                            atomic_store(&win->buffer_state, BUFFER_STATE_COMMITTED);

                            break;
                        }
                }
            }
        }

        for (int i = window_list.size - 1; i >= 0; i--)
        {
            window_t *win = window_list.data[i];
            if (win->id < 0 || atomic_load(&win->buffer_state) != BUFFER_STATE_COMMITTED)
                continue;

            blit_func(win->buffer,
                      (uint8_t *)back_buffer + (win->y * pitch + win->x * (bpp_bytes)),
                      win->width,
                      win->height,
                      pitch);
            
            atomic_store(&win->buffer_state, BUFFER_STATE_FREE);
            wd_enqueue(win, (ws_msg_t){
                .type = WEV_PAINT
            });
        }

        fast_memcpy((void *)kernel_context->video_state.lfb, (void *)back_buffer, kernel_context->video_state.size);
        __asm__ volatile("hlt");
    }

    exit();
}
REGISTER_KTHREAD("window_server_composite", window_server_composite);