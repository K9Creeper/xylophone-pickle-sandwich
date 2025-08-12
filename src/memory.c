#include <memory.h>

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

void *fast_memcpy(void *dest, const void *src, uint32_t n)
{
    uint8_t *d = (uint8_t *)dest;
    const uint8_t *s = (const uint8_t *)src;

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

unsigned char *memset(unsigned char *dest, unsigned char val, int count)
{
  register unsigned char *ptr = (unsigned char *)dest;
  while (count-- > 0)
    *ptr++ = val;
  return dest;
}

void *fast_memset(void *dest, int value, uint32_t n)
{
    uint8_t *d = (uint8_t *)dest;

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


unsigned short *memsetw(unsigned short *dest, unsigned short val, int count)
{
  register unsigned short *ptr = (unsigned short *)dest;
  while (count-- > 0)
    *ptr++ = val;
  return dest;
}