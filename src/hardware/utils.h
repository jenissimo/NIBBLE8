#ifndef nibble_utils_h
#define nibble_utils_h

#include <stdio.h>
#include <time.h>
#include <string.h>
#include <math.h>
#include <stdlib.h>

#define MAX(x, y) (((x) > (y)) ? (x) : (y))
#define MIN(x, y) (((x) < (y)) ? (x) : (y))

extern clock_t begin_t;
extern clock_t end_t;

void init_clock(void);
void tick_clock(void);

double nibble_rnd(int x);
char nibble_chr(int x);
double nibble_time(void);
int nibble_flr(double x);
int nibble_ceil(double x);
char* nibble_sub(char* str, int start, int end);
double nibble_sin(double x);
double nibble_cos(double x);
double nibble_atan2(double x, double y);
void nibble_trace(char* str);
double nibble_api_mid(double x, double y, double z);

#endif