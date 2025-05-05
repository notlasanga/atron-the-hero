#include <string.h>
#include <stdlib.h>

#include "raylib.h"

#include "atron.h"

void update_pulse(pulse_channel_t* channel, unsigned int frames) {
    for (unsigned int i = 0; i < frames; i++) {
        if (channel->current == channel->period) {
            channel->current = 0;
            channel->value = MAX_AMP;
        }
        if (channel->current == (int) (channel->period * channel->duty)) {
            channel->value = -MAX_AMP;
        }
        channel->data[i] = channel->value;
        channel->current++;
    }
}

void update_triangle(triangle_channel_t* channel, unsigned int frames) {
    for (unsigned int i = 0; i < frames; i++) {
        if (channel->value > MAX_AMP) {
            channel->direction = -1;
        }
        if (channel->value < -MAX_AMP) {
            channel->direction = 1;
        }

        channel->value += channel->slope * channel->direction;
        channel->data[i] = channel->value;
    }
}

void update_noise(noise_channel_t* channel, unsigned int frames) {
    for (unsigned int i = 0; i < frames; i++) {
        if (channel->current == 0) {
            channel->current = channel->period;
            channel->value = GetRandomValue(-MAX_AMP, MAX_AMP);
        }
        for (; channel->current > 0 && i < frames; channel->current--, i++) {
            channel->data[i] = channel->value;
        }
        i--;
    }
}

void AudioInputCallback(void *buffer, unsigned int frames) {
    if (audio_index == 0) {
        pthread_mutex_lock(audio_mutex_ptr);
        memset(audio_channel, 0, sizeof(int) * frames);

        // update_pulse(pulse_channel1, SAMPLE_RATE * FRAME_COUNT);
        // update_pulse(pulse_channel2, SAMPLE_RATE * FRAME_COUNT);
        // update_triangle(triangle_channel, SAMPLE_RATE * FRAME_COUNT);
        update_noise(noise_channel, SAMPLE_RATE * FRAME_COUNT);

        // mix all channels together
        for (int i = 0; i < SAMPLE_RATE * FRAME_COUNT; i++) {
            audio_channel[i] = pulse_channel1->data[i] + pulse_channel2->data[i] + triangle_channel->data[i] + noise_channel->data[i];
            audio_channel[i] /= 4;
        }
        pthread_mutex_unlock(audio_mutex_ptr);
    }

    short* d = buffer;
    int offset = audio_index * SAMPLE_RATE;
    for (int i = 0; i < SAMPLE_RATE; i++) {
        d[i] = audio_channel[i + offset];
    }
    audio_index = (audio_index + 1) % FRAME_COUNT;
}

void init_audio(void) {
    audio_channel = malloc(FRAME_COUNT * sizeof(int) * SAMPLE_RATE);
    memset(audio_channel, 0, FRAME_COUNT * sizeof(int) * SAMPLE_RATE);

    InitAudioDevice();              // Initialize audio device

    SetAudioStreamBufferSizeDefault(4096);

    audio_stream = LoadAudioStream(SAMPLE_RATE * 100, 16, 1);

    SetAudioStreamCallback(audio_stream, AudioInputCallback);
    PlayAudioStream(audio_stream);        // Start processing stream buffer (no data loaded currently)

    pulse_channel1 = malloc(sizeof(pulse_channel_t));

    pulse_channel1->data = malloc(FRAME_COUNT * sizeof(short) * SAMPLE_RATE);
    memset(pulse_channel1->data, 0, FRAME_COUNT * sizeof(short) * SAMPLE_RATE);
    pulse_channel1->duty = 0.125;
    pulse_channel1->period = 100;
    pulse_channel1->current = 0;
    pulse_channel1->value = 0;

    pulse_channel2 = malloc(sizeof(pulse_channel_t));

    pulse_channel2->data = malloc(FRAME_COUNT * sizeof(short) * SAMPLE_RATE);
    memset(pulse_channel2->data, 0, FRAME_COUNT * sizeof(short) * SAMPLE_RATE);
    pulse_channel2->duty = 0.125;
    pulse_channel2->period = 50;
    pulse_channel2->current = 0;
    pulse_channel2->value = 0;

    triangle_channel = malloc(sizeof(triangle_channel_t));

    triangle_channel->data = malloc(FRAME_COUNT * sizeof(short) * SAMPLE_RATE);
    memset(triangle_channel->data, 0, FRAME_COUNT * sizeof(short) * SAMPLE_RATE);
    triangle_channel->slope = 200;
    triangle_channel->value = 0;
    triangle_channel->direction = 1;

    noise_channel = malloc(sizeof(noise_channel_t));

    noise_channel->data = malloc(FRAME_COUNT * sizeof(short) * SAMPLE_RATE);
    memset(noise_channel->data, 0, FRAME_COUNT * sizeof(short) * SAMPLE_RATE);
    noise_channel->period = 1;
    noise_channel->current = 0;
    noise_channel->value = 0;
}

