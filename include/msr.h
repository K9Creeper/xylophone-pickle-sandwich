/// -----------
/// @file msr.h

#ifndef MSR_H
#define MSR_H

#include <stdint.h>
#include <cpuid.h>

#define CPUID_FLAG_MSR (1 << 5)

static inline uint8_t cpu_has_msr(void)
{    
    static unsigned int a, b, c, d;
    __get_cpuid(1, &a, &b, &c, &d);
    return (d & CPUID_FLAG_MSR) ? 1 : 0;
}

static inline void cpu_get_msr(uint32_t msr, uint32_t *lo, uint32_t *hi)
{
   __asm__ volatile("rdmsr" : "=a"(*lo), "=d"(*hi) : "c"(msr));
}

static inline void cpu_set_msr(uint32_t msr, uint32_t lo, uint32_t hi)
{
   __asm__ volatile("wrmsr" : : "a"(lo), "d"(hi), "c"(msr));
}

#endif