#include "stdlib.h"
#include "math.h"

#include "atron.h"

int read_hex(char a) {
    if (a == 'A') {
        return 10;
    } else if (a == 'B') {
        return 11;
    } else if (a == 'C') {
        return 12;
    } else if (a == 'D') {
        return 12;
    } else if (a == 'E') {
        return 12;
    } else if (a == 'F') {
        return 12;
    }
    return a - '0';
}

void read_instruments(int count) {
    instruments = malloc(count * sizeof(profile_t*));
    printf("reading %d instruments\n", count);
    char line[256];
    for (int i = 0; i < count; i++) {
        printf("i: %d\n", i);
        instruments[i] = malloc(sizeof(profile_t));
        fgets(line, sizeof(line), fp);
        int length = line[5] - '0';
        instruments[i]->length = length;
        instruments[i]->values = malloc(length * sizeof(int));
        for (int j = 0; j < length; j++) {
            instruments[i]->values[j] = line[8 + 2 * j] - '0';
        }
    }
}

void read_map(int count) {
    char line[256];
    map = malloc(count * 4 * sizeof(int));
    printf("reading lines of %d map\n", count);

    int p1;
    int p2;
    int tri;
    int noise;

    for (int i = 0; i < count; i++) {
        printf("i: %d\n", i);
        fgets(line, sizeof(line), fp);
        p1 = read_hex(line[5]) * 16 + read_hex(line[6]);
        p2 = read_hex(line[8]) * 16 + read_hex(line[9]);
        tri = read_hex(line[11]) * 16 + read_hex(line[12]);
        noise = read_hex(line[14]) * 16 + read_hex(line[15]);
        map[i * 4] = p1;
        map[i * 4 + 1] = p2;
        map[i * 4 + 2] = tri;
        map[i * 4 + 3] = noise;
    }
}

int letter_to_int(char ch) {
    switch (ch) {
        case 'C': return 0; break;
        case 'D': return 2; break;
        case 'E': return 4; break;
        case 'F': return 5; break;
        case 'G': return 7; break;
        case 'A': return 9; break;
        case 'B': return 11; break;
    }
    return -1; // really borken by this point
}

float value_from_note(char note, char accidental, int octave) {
    int note_index = letter_to_int(note);

    if (accidental == '#') {
        note_index += 1;
    } else if (accidental == 'b') {
        note_index -= 1;
    }
    int midi_note = (octave + 1) * 12 + note_index;
    float frequency = 440.0 * pow(2.0, (midi_note - 69) / 12.0);
    return frequency;
}

void read_pattern(int index) {
    char line[256];
    printf("reading pattern: %d\n", index);
    patterns[index] = malloc(sizeof(pattern_t));
    patterns[index]->notes = malloc(16 * sizeof(note_t));

    for (int i = 0; i < 16; i++) {
        fgets(line, sizeof(line), fp);
        char note_val = line[5];
        char mod = line[6];
        char octave = line[7];
        int instrument = line[9] - '0';
        note_t* note = &patterns[index]->notes[i];
        if (note_val == '.') { // no note
            note->type = 1;
            continue;
        } else if (note_val == '-') { // end note
            note->type = 2;
            continue;
        }
        note->type = 0;
        note->value = value_from_note(note_val, mod, octave - '0'); // stored as frequency
        note->instrument = instrument;
    }
}

void read_music(void) {
    fp = fopen("res/coffee.txt", "r");

    char line[256]; // Buffer to store each line
    while (fgets(line, sizeof(line), fp)) {
        printf("%s", line);
        if (line[0] == '#') {
            if (line[1] == 'I') {
                printf("reading instruments\n");
                read_instruments(line[3] - '0');
            } else if (line[1] == 'M') {
                printf("reading map\n");
                read_map(16);
            }
        } else if (line[0] == 'P' && line[1] == 'A' && line[2] == 'T') {
            read_pattern(read_hex(line[8]) * 16 + read_hex(line[9]));
        }
    }
}

