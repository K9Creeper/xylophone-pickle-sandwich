#ifndef MEMORY_H
#define MEMORY_H

#include <stdint.h>
#include <stddef.h>

static inline __attribute__((always_inline))
uint8_t inportb(uint16_t port) {
    uint8_t rv;
    __asm__ volatile ("inb %1, %0" : "=a"(rv) : "dN"(port));
    return rv;
}

static inline __attribute__((always_inline))
void outportb(uint16_t port, uint8_t data) {
    __asm__ volatile ("outb %1, %0" :: "dN"(port), "a"(data));
}

static inline __attribute__((always_inline))
uint16_t inports(uint16_t port) {
    uint16_t rv;
    __asm__ volatile ("inw %1, %0" : "=a"(rv) : "dN"(port));
    return rv;
}

static inline __attribute__((always_inline))
void outports(uint16_t port, uint16_t data) {
    __asm__ volatile ("outw %1, %0" :: "dN"(port), "a"(data));
}

static inline __attribute__((always_inline))
uint32_t inportl(uint16_t port) {
    uint32_t rv;
    __asm__ volatile ("inl %1, %0" : "=a"(rv) : "dN"(port));
    return rv;
}

static inline __attribute__((always_inline))
void outportl(uint16_t port, uint32_t data) {
    __asm__ volatile ("outl %1, %0" :: "dN"(port), "a"(data));
}

static inline void io_wait(void) {
    outportb(0x80, 0);
}

/* Swap two ints */
static inline void swap(int *p1, int *p2) {
    int tmp = *p1;
    *p1 = *p2;
    *p2 = tmp;
}

/* Basic memcpy */
static inline unsigned char *memcpy(unsigned char *restrict dest,
                                    const unsigned char *restrict src,
                                    size_t count) {
    for (size_t i = 0; i < count; i++)
        dest[i] = src[i];
    return dest;
}

/* Fast memcpy with 32-bit alignment optimization */
static inline void *fast_memcpy(void *restrict dest, const void *restrict src, size_t n) {
    uint8_t *d = dest;
    const uint8_t *s = src;

    /* Align to 4 bytes */
    while (n > 0 && ((uintptr_t)d & 3)) {
        *d++ = *s++;
        n--;
    }

    uint32_t *d32 = (uint32_t *)d;
    const uint32_t *s32 = (const uint32_t *)s;

    while (n >= 16) {
        d32[0] = s32[0];
        d32[1] = s32[1];
        d32[2] = s32[2];
        d32[3] = s32[3];
        d32 += 4;
        s32 += 4;
        n -= 16;
    }

    while (n >= 4) {
        *d32++ = *s32++;
        n -= 4;
    }

    d = (uint8_t *)d32;
    s = (const uint8_t *)s32;
    while (n--) {
        *d++ = *s++;
    }

    return dest;
}

/* Basic memset */
static inline unsigned char *memset(unsigned char *dest, unsigned char val, size_t count) {
    for (size_t i = 0; i < count; i++)
        dest[i] = val;
    return dest;
}

/* Fast memset with 32-bit alignment optimization */
static inline void *fast_memset(void *dest, int value, size_t n) {
    uint8_t *d = dest;
    uint32_t val32 = (uint8_t)value;
    val32 |= val32 << 8;
    val32 |= val32 << 16;

    while (n > 0 && ((uintptr_t)d & 3)) {
        *d++ = (uint8_t)value;
        n--;
    }

    uint32_t *d32 = (uint32_t *)d;
    while (n >= 4) {
        *d32++ = val32;
        n -= 4;
    }

    d = (uint8_t *)d32;
    while (n--) {
        *d++ = (uint8_t)value;
    }

    return dest;
}

static inline unsigned short *memsetw(unsigned short *dest, unsigned short val, size_t count) {
    for (size_t i = 0; i < count; i++)
        dest[i] = val;
    return dest;
}

#endif