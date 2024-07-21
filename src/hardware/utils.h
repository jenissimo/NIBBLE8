#ifndef nibble_utils_h
#define nibble_utils_h

#include <stdio.h>
#include <time.h>
#include <string.h>
#include <math.h>
#include <stdlib.h>

#define MAX(x, y) (((x) > (y)) ? (x) : (y))
#define MIN(x, y) (((x) < (y)) ? (x) : (y))

extern uint32_t nibble_frame_count;

double nibble_api_rnd(int x);
char nibble_api_chr(int x);
double nibble_api_time(void);
int nibble_api_flr(double x);
int nibble_api_ceil(double x);
char* nibble_api_sub(char* str, int start, int end);
double nibble_api_sin(double x);
double nibble_api_cos(double x);
double nibble_api_atan2(double x, double y);
void nibble_api_trace(char* str);
double nibble_api_min(double x, double y);
double nibble_api_max(double x, double y);
double nibble_api_mid(double x, double y, double z);

#endif