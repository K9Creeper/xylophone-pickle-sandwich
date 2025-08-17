#ifndef STRING_H
#define STRING_H

#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>

// Character checks
static inline bool isletter(char c) {
    return (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z');
}

static inline bool islower(char c) {
    return (c >= 'a' && c <= 'z');
}

static inline bool isdigit(char c) {
    return (c >= '0' && c <= '9');
}

static inline bool ispunct(char c) {
    return (c >= '!' && c <= '/') ||
           (c >= ':' && c <= '@') ||
           (c >= '[' && c <= '`') ||
           (c >= '{' && c <= '~');
}

static inline char toupper(char c) {
    if (c >= 'a' && c <= 'z') return c - ('a' - 'A');
    return c;
}

// String functions
static inline int strlen(const char *str) {
    int len = 0;
    while (str[len] != '\0') len++;
    return len;
}

static inline int strcmp(const char *s1, const char *s2) {
    while (*s1 && (*s1 == *s2)) {
        s1++; s2++;
    }
    return (int)(unsigned char)(*s1) - (int)(unsigned char)(*s2);
}

static inline int memcmp(const void* s1, const void* s2, uint32_t n) {
    const uint8_t *p1 = (const uint8_t*)s1;
    const uint8_t *p2 = (const uint8_t*)s2;
    for (uint32_t i = 0; i < n; i++) {
        if (p1[i] != p2[i]) return p1[i] - p2[i];
    }
    return 0;
}

static inline char *strchr(const char *s, char c) {
    while (*s) {
        if (*s == c) return (char *)s;
        s++;
    }
    return (c == '\0') ? (char *)s : NULL;
}

static inline char *strrchr(const char *s, int c)
{
    const char *last = NULL;
    while (*s) {
        if (*s == (char)c)
            last = s;
        s++;
    }
    return (c == 0 ? (char*) s : (char *)last); 
}

static inline char *strtok(char *str, const char *delim, char *out) {
    static char *saveptr = 0;
    if (str) saveptr = str;

    if (!saveptr) return 0;

    char *start = saveptr;
    while (*start && strchr(delim, *start)) start++;
    if (!*start) return 0;

    char *end = start;
    while (*end && !strchr(delim, *end)) end++;

    int len = end - start;
    for (int i = 0; i < len; i++) out[i] = start[i];
    out[len] = '\0';

    saveptr = (*end) ? end + 1 : 0;
    return out;
}

// Utility functions
static inline bool equal(const char *a, const char *b) {
    return strcmp(a, b) == 0;
}

static inline void trim(char *str) {
    if (!str) return;
    int start = 0;
    while (str[start] == ' ' || str[start] == '\t') start++;
    int end = strlen(str) - 1;
    while (end >= start && (str[end] == ' ' || str[end] == '\t')) end--;
    int j = 0;
    for (int i = start; i <= end; i++) str[j++] = str[i];
    str[j] = '\0';
}

static inline int find(const char *str, const char *f) {
    int slen = strlen(str);
    int flen = strlen(f);
    for (int i = 0; i <= slen - flen; i++) {
        bool match = true;
        for (int j = 0; j < flen; j++) {
            if (str[i + j] != f[j]) { match = false; break; }
        }
        if (match) return i;
    }
    return -1;
}

// Integer conversions
static inline void itoa(int num, char *str, int base) {
    if (!str) return;
    char buf[33] = {0};
    int i = 0;
    bool neg = false;

    if (num < 0 && base == 10) { neg = true; num = -num; }

    do {
        int rem = num % base;
        buf[i++] = (rem < 10) ? ('0' + rem) : ('A' + rem - 10);
        num /= base;
    } while (num > 0);

    int j = 0;
    if (neg) str[j++] = '-';
    for (int k = i - 1; k >= 0; k--) str[j++] = buf[k];
    str[j] = '\0';
}

static inline void uitoa(uint32_t num, char *str, int base) {
    if (!str) return;
    char buf[33] = {0};
    int i = 0;
    do {
        int rem = num % base;
        buf[i++] = (rem < 10) ? ('0' + rem) : ('A' + rem - 10);
        num /= base;
    } while (num > 0);
    int j = 0;
    for (int k = i - 1; k >= 0; k--) str[j++] = buf[k];
    str[j] = '\0';
}

static inline int atoi(const char *str) {
    if (!str) return 0;
    int res = 0, i = 0, sign = 1;
    if (str[0] == '-') { sign = -1; i = 1; }
    while (str[i]) {
        if (!isdigit(str[i])) break;
        res = res * 10 + (str[i] - '0');
        i++;
    }
    return res * sign;
}

static inline uint32_t uatoi(const char *str) {
    if (!str) return 0;
    uint32_t res = 0;
    int i = 0;
    while (str[i]) {
        if (!isdigit(str[i])) break;
        res = res * 10 + (str[i] - '0');
        i++;
    }
    return res;
}

#endif // STRING_H