#ifndef MEMORY_H
#define MEMORY_H

#include <stdint.h>

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

void swap(int *p1, int *p2);

unsigned char *memcpy(unsigned char *dest, const unsigned char *src, int count);

void *fast_memcpy(void *dest, const void *src, uint32_t n);

unsigned char *memset(unsigned char *dest, unsigned char val, int count);

void *fast_memset(void *dest, int value, uint32_t n);

unsigned short *memsetw(unsigned short *dest, unsigned short val, int count);

#endif