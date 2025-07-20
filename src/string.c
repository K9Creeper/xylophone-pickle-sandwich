#include <string.h>

bool isletter(const char c) { return (c >= 'A' && c <= 'z'); }
bool islower(const char c) { return (c >= 'a' && c <= 'z'); }
bool isdigit(const char c) { return (c >= '0' && c <= '9'); }
bool ispunct(const char c)
{
  return (c == '!' || c == '"' || c == '#' || c == '$' || c == '%' || c == '&' || c == '\'' || c == '(' || c == ')' || c == '*' || c == '+' || c == ',' || c == '-' || c == '.' || c == '/' || c == ':' || c == ';' || c == '?' || c == '@' || c == '[' || c == '\\' || c == ']' || c == '^' || c == '_' || c == '`' || c == '{' || c == '|' || c == '}' || c == '~');
}

int strlen(const char *str)
{
  uint32_t len = 0;
  while (str[len])
    len++;
  return len;
}

char *strtok(char *str, const char *delim, char *out)
{
  int i = 0;
  for (; i < strlen(str); i++)
  {
    out[i] = str[i];
  }
  
  {
    out[i] = '\0';
  }

  static char *next = 0;
  if (out)
    next = out;

  if (!next)
    return 0;

  while (*next && (*next == *delim))
    next++;

  if (*next == '\0')
    return 0;

  char *token = next;

  while (*next && (*next != *delim))
    next++;

  if (*next)
  {
    *next = '\0';
    next++;
  }
  else
  {
    next = 0;
  }

  return token;
}

bool equal(const char* a, const char* b){
  int len = strlen(a);
  if(len == strlen(b))
  {
    for(int i = 0; i < len; i++)
    {
      if(a[i] != b[i])
        return false;
    }

    if(len > 0)
      return true;
  }

  return false;
}

int find(const char *str, const char *f) {
  if (str == 0 || f == 0) {
    return -1;
  }

  if (*f == '\0') {
    return 0;
  }

  int str_len = strlen(str);
  int f_len = strlen(f);

  if (f_len > str_len) {
    return -1;
  }

  for (int i = 0; i <= str_len - f_len; ++i) {
    int j;
    for (j = 0; j < f_len; ++j) {
      if (str[i + j] != f[j]) {
        break;
      }
    }
    if (j == f_len) {
      return i;
    }
  }

  return -1;
}

void itoa(int num, char *str, int base)
{
  int i = 0;
  int isNegative = 0;

  // Handle 0 explicitly
  if (num == 0)
  {
    str[i++] = '0';
    str[i] = '\0';
    return;
  }

  // Handle negative numbers only if base is 10
  if (num < 0 && base == 10)
  {
    isNegative = 1;
    num = -num;
  }

  // Process individual digits
  while (num != 0)
  {
    int rem = num % base;
    str[i++] = (rem > 9) ? (rem - 10) + 'a' : rem + '0';
    num = num / base;
  }

  // Append negative sign for negative numbers
  if (isNegative)
  {
    str[i++] = '-';
  }

  str[i] = '\0'; // Null-terminate the string

  // Reverse the string
  for (int start = 0, end = i - 1; start < end; start++, end--)
  {
    char temp = str[start];
    str[start] = str[end];
    str[end] = temp;
  }

  return;
}

void uitoa(uint32_t num, char *str, int base)
{
  int i = 0;
  int isNegative = 0;

  // Handle 0 explicitly
  if (num == 0)
  {
    str[i++] = '0';
    str[i] = '\0';
    return;
  }

  // Process individual digits
  while (num != 0)
  {
    int rem = num % base;
    str[i++] = (rem > 9) ? (rem - 10) + 'a' : rem + '0';
    num = num / base;
  }

  // Append negative sign for negative numbers
  if (isNegative)
  {
    str[i++] = '-';
  }

  str[i] = '\0'; // Null-terminate the string

  // Reverse the string
  for (int start = 0, end = i - 1; start < end; start++, end--)
  {
    char temp = str[start];
    str[start] = str[end];
    str[end] = temp;
  }

  return;
}