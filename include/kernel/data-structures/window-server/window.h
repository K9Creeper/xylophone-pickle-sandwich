/// --------------
/// @file window.h

#ifndef WINDOW_H
#define WINDOW_H

#include <stdint.h>
#include <stdatomic.h>

#define MAX_WINDOW_NAME 32
#define MAX_DISPATCH_QUEUE 256

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

typedef enum buffer_state_e
{
    BUFFER_STATE_FREE = 0,
    BUFFER_STATE_PAINTING = 1,
    BUFFER_STATE_COMMITTED = 2
} buffer_state_t;

typedef struct window_s
{
    char name[MAX_WINDOW_NAME];
    int id;
    
    int x, y;
    int width, height;
    int pitch;
    uint32_t *buffer;
    
    _Atomic buffer_state_t buffer_state;
    int z_index;
    
    int creator_pid;

    ws_msg_t dispatch_queue[MAX_DISPATCH_QUEUE];
    int dispatch_head;
    int dispatch_tail;
} window_t;

#endif