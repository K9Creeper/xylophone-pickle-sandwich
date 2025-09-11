/// --------------
/// @file window.h

#ifndef WINDOW_H
#define WINDOW_H

#include <stdint.h>

typedef struct window_s{
    int tgid_owner;

    int win_id;

    int x,y;
    int width, height;
    
    int z;

    uint32_t* raw_buffer;
}window_t;

#endif