
/*
 * Processor.h
 *
 *  Created on: May 29, 2026
 *      Author: mickp
 */
#ifndef PROCESSOR_H_
#define PROCESSOR_H_

typedef enum {
    E2 = 0,
    A2,
    D3,
    G3,
    B3,
    E4,
    NOTE_COUNT
} NoteIndex;

typedef struct {
    const char *name;
    float frequency;
} Note;

extern const Note notes[NOTE_COUNT];

extern NoteIndex note;
extern float cents;
extern float percent_error;

void Processor_init(void);
void Processor(float frequency);

#endif

