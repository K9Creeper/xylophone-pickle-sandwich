#ifndef STRING_H
#define STRING_H
#include <stdbool.h>
#include <stdint.h>

bool isletter(const char c);
bool islower(const char c);
bool isdigit(const char c);
bool ispunct(const char c);

int strlen(const char *str);
char *strtok(char *str, const char *delim, char *out);

bool equal(const char *a, const char *b);

int find(const char *str, const char *f);
void itoa(int num, char *str, int base);
void uitoa(uint32_t num, char *str, int base);

#endif