#include <stdlib.h>

#include "atron.h"


int min(int a, int b) {
    if (a < b) {
        return a;
    }
    return b;
}

int max(int a, int b) {
    if (a > b) {
        return a;
    }
    return b;
}

int remap(int margin, int i, int sample_width, short* buffer) {
    int box_height = SCREEN_HEIGHT - (2 * margin);
    int middle = SCREEN_HEIGHT / 2;
    int index = ((float) i / SCREEN_WIDTH) * sample_width;
    short audio_value = buffer[index];
    float normalized = (float) audio_value / (2 * MAX_AMP);
    float fliped = -normalized;
    int scaled = fliped * box_height;
    int offseted = scaled + middle;
    int z = offseted;
    // printf("i: %d\n", i);
    // printf("z: %d\n", z);
    // printf("index: %d\n", index);
    // printf("audio_value: %d\n", audio_value);

    return z;
}

void oscilloscope(void) {
    short audio_buffer[SAMPLE_RATE * FRAME_COUNT] = {0};
    // get audio info
    pthread_mutex_lock(audio_mutex_ptr);
    for (int i = 0; i < SAMPLE_RATE * FRAME_COUNT; i++) {
        audio_buffer[i] = audio_channel[i];
        // printf("%d: %d\n", i, audio_buffer[i]);
    }
    pthread_mutex_unlock(audio_mutex_ptr);


    int margin = 50;
    for (int i = 0; i < SCREEN_WIDTH * SCREEN_HEIGHT; i++) {
        pixels[i].r = 0;
        pixels[i].g = 0;
        pixels[i].b = 0;
    }

    int prev = remap(margin, 0, SAMPLE_RATE * FRAME_COUNT, audio_buffer);
    for (int i = 0; i < SCREEN_WIDTH; i++) {
        int z = remap(margin, i, SAMPLE_RATE * FRAME_COUNT, audio_buffer);
        pixels[z * SCREEN_WIDTH + i].r = 255;
        pixels[z * SCREEN_WIDTH + i].g = 255;
        pixels[z * SCREEN_WIDTH + i].b = 255;
        // printf("%d: %d\n", i, z);
        if (prev != z) {
            int bottom = min(prev, z);
            for (int j = 0; j < abs(prev - z); j++) {
                pixels[(bottom + j) * SCREEN_WIDTH + i].r = 255;
                pixels[(bottom + j) * SCREEN_WIDTH + i].g = 255;
                pixels[(bottom + j) * SCREEN_WIDTH + i].b = 255;
            }
            prev = z;
        }
    }
}
