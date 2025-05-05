#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <string.h>
#include <pthread.h>

#include "raylib.h"

#include "atron.h"
#include "audio.h"
#include "graphics.h"
#include "io.h"

pthread_mutex_t audio_mutex = PTHREAD_MUTEX_INITIALIZER;

int main(void) {
    frame_counter = 0;
    audio_index = 0;
    audio_mutex_ptr = &audio_mutex;

    srand(time(NULL));
    SetTraceLogLevel(LOG_WARNING);
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Atron the Hero");
    SetTargetFPS(60);

    Image image = GenImageColor(SCREEN_WIDTH, SCREEN_HEIGHT, BLACK);
    ImageFormat(&image, PIXELFORMAT_UNCOMPRESSED_R8G8B8A8);

    Texture2D texture = LoadTextureFromImage(image);

    pixels = image.data;

    init_audio();

    patterns = malloc(30 * sizeof(pattern_t));
    read_music();

    int pause = 0;
    while (!WindowShouldClose()) {
        // float dt = GetFrameTime();

        // input
        if (IsKeyPressed(KEY_P)) {
            if (pause) {
                PlayAudioStream(audio_stream);
            } else {
                PauseAudioStream(audio_stream);
            }
            pause = !pause;
        }

        if (!pause) {
            oscilloscope();
            UpdateTexture(texture, pixels);
            frame_counter++;
        }

        BeginDrawing();

        ClearBackground(WHITE);

        DrawTexture(texture, 0, 0, WHITE);
        DrawFPS(10, 10);

        EndDrawing();
    }

    UnloadImage(image);
    UnloadTexture(texture);

    CloseWindow();

    exit(EXIT_SUCCESS);
}

