#ifndef FILTERS_H
#define FILTERS_H

#define NUM_FILTERS 7

typedef enum {
        FILTER_CUTOFF,
        FILTER_LOW_PASS,
        FILTER_DETUNE,
        FILTER_BUZZ,
        FILTER_NOISE,
        FILTER_REVERB,
        FILTER_DAMPEN
} FilterType;

typedef struct {
    FilterType type;
    void *data;
} Filter;

Filter *filter_create(FilterType type, double param);
void filter_update (Filter *filter, double param);
double filter_apply(Filter *filter, double input, double time);
void filter_destroy(Filter *filter);

#endif // FILTERS_H
