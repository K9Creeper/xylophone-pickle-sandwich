#include <memory.h>

uint8_t inportb(unsigned short _port)
{
  uint8_t rv;
  __asm__ volatile("inb %1, %0" : "=a"(rv) : "dN"(_port));
  return rv;
}

void outportb(unsigned short _port, uint8_t _data)
{
  __asm__ volatile("outb %1, %0" ::"dN"(_port), "a"(_data));
}

uint16_t inports(unsigned short _port)
{
  uint16_t rv;
  __asm__ volatile("inw %1, %0" : "=a"(rv) : "dN"(_port));
  return rv;
}

void outports(unsigned short _port, uint16_t _data)
{
  __asm__ volatile("outw %1, %0" : : "dN" (_port), "a" (_data));
}

uint32_t inportl(unsigned short _port)
{
  uint32_t rv;
  __asm__ volatile("inl %%dx, %%eax" : "=a"(rv) : "dN"(_port));
  return rv;
}

void outportl(unsigned short _port, uint16_t  _data)
{
  __asm__ volatile ("outl %%eax, %%dx" : : "dN" (_port), "a" (_data));
}

void call_asm(void *fn, void *arg)
{
  __asm__ volatile(
      "movl  %[FUNC], %%eax\n"
      "movl %[ARG], %%edi\n"
      "call *%%eax"
      : : [FUNC] "a"((int)fn), [ARG] "b"((int)arg));
}

void swap(int *p1, int *p2)
{
  int temp = *p1;
  *p1 = *p2;
  *p2 = temp;
}

unsigned char *memcpy(unsigned char *dest,
                      const unsigned char *src, int count)
{
  unsigned char *csrc = (unsigned char *)src;
  unsigned char *cdest = (unsigned char *)dest;

  for (int i = 0; i < count; i++)
    cdest[i] = csrc[i];
  return cdest;
}

unsigned char *memset(unsigned char *dest, unsigned char val, int count)
{
  register unsigned char *ptr = (unsigned char *)dest;
  while (count-- > 0)
    *ptr++ = val;
  return dest;
}

unsigned short *memsetw(unsigned short *dest, unsigned short val, int count)
{
  register unsigned short *ptr = (unsigned short *)dest;
  while (count-- > 0)
    *ptr++ = val;
  return dest;
}