#ifndef MEMORY_H
#define MEMORY_H

#include <stdint.h>

uint8_t inportb(unsigned short _port);

void outportb(unsigned short _port, uint8_t _data);

uint16_t inports(unsigned short _port);

void outports(unsigned short _port, uint16_t _data);

uint32_t inportl(unsigned short _port);

void outportl(unsigned short _port, uint16_t _data);

void call_asm(void *fn, void *arg);

void swap(int *p1, int *p2);

unsigned char *memcpy(unsigned char *dest, const unsigned char *src, int count);

void *fast_memcpy(void *dest, const void *src, uint32_t n);

unsigned char *memset(unsigned char *dest, unsigned char val, int count);

void *fast_memset(void *dest, int value, uint32_t n);

unsigned short *memsetw(unsigned short *dest, unsigned short val, int count);

#endif