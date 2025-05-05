#ifndef ATRON_H
#define ATRON_H

#include <stdio.h>
#include <pthread.h>

#include "raylib.h"

#define SAMPLE_RATE 441
#define MAX_AMP 4000
#define FRAME_COUNT 4

#define SCREEN_WIDTH 1280
#define SCREEN_HEIGHT 720

typedef struct note_t note_t;
struct note_t {
    int type; // 0 = normal note | 1 = no note | 2 = stop note
    float frequency; // stored as frequency
    int instrument;
};

typedef struct pattern_t pattern_t;
struct pattern_t {
    note_t* notes;
};

typedef struct profile_t profile_t;
struct profile_t {
    int length;
    int *values;
};

typedef struct pulse_channel_t pulse_channel_t;
struct pulse_channel_t {
    profile_t *volume;
    short *data;
    double duty;
    int period;
    int current;
    int value;
    int old_note;
};

typedef struct triangle_channel_t triangle_channel_t;
struct triangle_channel_t {
    short *data;
    int slope;
    int value;
    int direction;
    int old_note;
};

typedef struct noise_channel_t noise_channel_t;
struct noise_channel_t {
    profile_t *volume;
    short *data;
    int period;
    int current;
    int value;
    int old_note;
    int active;
};

pthread_mutex_t* audio_mutex_ptr;

int frame_counter;

pulse_channel_t* pulse_channel1;
pulse_channel_t* pulse_channel2;
triangle_channel_t* triangle_channel;
noise_channel_t* noise_channel;

short* audio_channel;
int audio_index;

AudioStream audio_stream;

profile_t** instruments;
int* map;
pattern_t** patterns;

Color* pixels;

FILE *fp;

#endif // ATRON_H

