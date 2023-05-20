#include "filters.h"
#include <stdlib.h>
#include <math.h>
#include <time.h>

typedef struct
{
    double threshold;
} CutOffFilterData;

typedef struct
{
    double amount;
} DetuneFilterData;

typedef struct
{
    double amount;
} BuzzFilterData;

typedef struct
{
    double cutoff;
} LowPassFilterData;

typedef struct
{
    double amount;
} NoiseFilterData;

typedef struct
{
    double feedback;
    double *buffer;
    int buffer_size;
    int buffer_pos;
} ReverbFilterData;

typedef struct
{
    double amount;
} DampenFilterData;

static double cut_off_filter(double input, CutOffFilterData *data, double time)
{
    if (data->threshold < 0.01) return input;

    // Implement a simple cut-off filter using a threshold
    if (input > data->threshold)
    {
        return input;
    }
    else
    {
        return 0.0;
    }
}

static double detune_filter(double input, DetuneFilterData *data, double time)
{
    if (data->amount < 0.01) return input;
    // Apply detuning by changing the frequency
    return input + data->amount * 100 * sin(2.0 * M_PI * time);
}

static double buzz_filter(double input, BuzzFilterData *data, double time)
{
    if (data->amount < 0.01) return input;
    // Add a buzz effect by increasing the amplitude of the input
    return input * (1.0 + data->amount);
}

static double low_pass_filter(double input, LowPassFilterData *data, double time)
{
    if (data->cutoff < 0.01) return input;
    // Implement a simple low-pass filter using exponential smoothing
    static double prev_output = 0.0;
    double output = data->cutoff * input + (1.0 - data->cutoff) * prev_output;
    prev_output = output;
    return output;
}

static double noise_filter(double input, NoiseFilterData *data, double time)
{
    if (data->amount < 0.01) return input;
    // Add noise to the input signal
    double noise = 2.0 * ((double)rand() / (double)RAND_MAX) - 1.0;
    return input + data->amount * noise;
}

static double reverb_filter(double input, ReverbFilterData *data, double time)
{
    if (data->feedback < 0.01) return input;
    // Implement a simple reverb effect using a circular buffer and feedback
    double output = data->buffer[data->buffer_pos];
    data->buffer[data->buffer_pos] = input + data->feedback * output;
    data->buffer_pos = (data->buffer_pos + 1) % data->buffer_size;
    return output;
}

static double dampen_filter(double input, DampenFilterData *data, double time)
{
    if (data->amount < 0.01) return input;
    // Reduce the amplitude of the input signal
    return input * (1.0 - data->amount);
}

Filter *filter_create(FilterType type, double param)
{
    Filter *filter = (Filter *)malloc(sizeof(Filter));
    filter->type = type;

    filter_update(filter, param);

    return filter;
}

void filter_update(Filter *filter, double param)
{
    switch (filter->type)
    {
    case FILTER_CUTOFF:
        filter->data = malloc(sizeof(CutOffFilterData));
        ((CutOffFilterData *)filter->data)->threshold = param;
        break;
    case FILTER_DETUNE:
        filter->data = malloc(sizeof(DetuneFilterData));
        ((DetuneFilterData *)filter->data)->amount = param;
        break;

    case FILTER_BUZZ:
        filter->data = malloc(sizeof(BuzzFilterData));
        ((BuzzFilterData *)filter->data)->amount = param;

        break;

    case FILTER_LOW_PASS:
        filter->data = malloc(sizeof(LowPassFilterData));
        ((LowPassFilterData *)filter->data)->cutoff = param;
        break;

    case FILTER_NOISE:
        filter->data = malloc(sizeof(NoiseFilterData));
        ((NoiseFilterData *)filter->data)->amount = param;
        break;

    case FILTER_REVERB:
        filter->data = malloc(sizeof(ReverbFilterData));
        ((ReverbFilterData *)filter->data)->feedback = param;
        ((ReverbFilterData *)filter->data)->buffer_size = 4410; // 100ms reverb buffer
        ((ReverbFilterData *)filter->data)->buffer = calloc(((ReverbFilterData *)filter->data)->buffer_size, sizeof(double));
        ((ReverbFilterData *)filter->data)->buffer_pos = 0;
        break;

    case FILTER_DAMPEN:
        filter->data = malloc(sizeof(DampenFilterData));
        ((DampenFilterData *)filter->data)->amount = param;
        break;

    default:
        free(filter);
        return;
    }
}

double filter_apply(Filter *filter, double input, double time)
{
    switch (filter->type)
    {
    case FILTER_CUTOFF:
        return cut_off_filter(input, (CutOffFilterData *)filter->data, time);
    case FILTER_DETUNE:
        return detune_filter(input, (DetuneFilterData *)filter->data, time);

    case FILTER_BUZZ:
        return buzz_filter(input, (BuzzFilterData *)filter->data, time);

    case FILTER_LOW_PASS:
        return low_pass_filter(input, (LowPassFilterData *)filter->data, time);
    case FILTER_NOISE:
        return noise_filter(input, (NoiseFilterData *)filter->data, time);

    case FILTER_REVERB:
        return reverb_filter(input, (ReverbFilterData *)filter->data, time);

    case FILTER_DAMPEN:
        return dampen_filter(input, (DampenFilterData *)filter->data, time);

    default:
        return input;
    }
}

void filter_destroy(Filter *filter)
{
    if (filter->type == FILTER_REVERB)
    {
        free(((ReverbFilterData *)filter->data)->buffer);
    }
    free(filter->data);
    free(filter);
}
