/// ---------------------
/// @file window-server.h

#ifndef WINDOW_SERVER_H
#define WINDOW_SERVER_H

#include <stdint.h>

#define MAX_WINDOWS 64
#define MAX_WS_MESSAGES 256

#include <kernel/data-structures/window-server/window.h>

#ifndef WS_MSG_T
#define WS_MSG_T

typedef enum
{
    MSG_CREATE_WINDOW,
    MSG_DRAW_READY,
    MSG_DESTROY_WINDOW,
    MSG_MOVE_WINDOW,
    // MSG_RESIZE_WINDOW,
} ws_msg_type_t;

typedef enum
{
    WEV_PAINT,
} wev_type_t;

typedef struct
{
    uint32_t type;
    int window_id;
    int x, y, w, h;

    int pid;
} ws_msg_t;

#endif

#endif