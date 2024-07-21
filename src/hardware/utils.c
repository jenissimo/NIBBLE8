#include "utils.h"
#include "nibble8.h"
#include "debug/debug.h"

#define TAU 6.2831853071795864769252867665590057683936
uint32_t nibble_frame_count = 0;

double nibble_api_rnd(int x)
{
    return (double)rand() / RAND_MAX * (x);
}

char nibble_api_chr(int x)
{
    return (char)x;
}

int nibble_api_flr(double x)
{
    return (int)floor(x);
}

int nibble_api_ceil(double x)
{
    return (int)ceil(x);
}

int nibble_api_sqrt(int x)
{
    return (int)sqrt(x);
}

double nibble_api_time(void)
{
    return (double)nibble_frame_count / (double)NIBBLE_FPS;
}

char *nibble_api_sub(char *str, int start, int end)
{
    char *result;
    int i = 0;

    if (end <= -1)
    {
        end = strlen(str);
    }

    result = malloc(end - start + 1);

    for (i = 0; i < end - start; i++)
    {
        result[i] = str[start + i];
    }
    result[i] = '\0';
    return result;
}

double nibble_api_sin(double x)
{
    return -sin(x * 3.1415 * 2);
}

double nibble_api_cos(double x)
{
    return cos(x * 3.1415 * 2);
}

double nibble_api_atan2(double x, double y)
{
    return 0.75 + atan2((double)x, (double)y) / TAU;
}

void nibble_api_trace(char *text)
{
    DEBUG_LOG("%s", text);
}

double nibble_api_min(double x, double y)
{
    return MIN(x, y);
}

double nibble_api_max(double x, double y)
{
    return MAX(x, y);
}

double nibble_api_mid(double x, double y, double z)
{
    return x > y   ? y > z ? y : MIN(x, z)
             : x > z ? x
                   : MIN(y, z);
}