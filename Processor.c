/*
 * Processor.c
 *
 *  Created on: May 29, 2026
 *      Author: mickp
 */

#include "Processor.h"
#include <stdint.h>
#include <math.h>

//Notes
const Note notes[NOTE_COUNT] = {
    {"E2", 82.41f},   // default scale target note
    {"A2", 110.00f},  // default scale target note
    {"D3", 146.83f},  // default scale target note
    {"G3", 196.00f},  // default scale target note
    {"B3", 246.94f},  // default scale target note
    {"E4", 329.63f}   // default scale target note
};

NoteIndex note;
float cents        = 0.0f;
float percent_error = 0.0f;


void Processor_init(void)
{
    note          = E2; //since it's index is 0
    cents         = 0.0f;
    percent_error = 0.0f;
}

/* Classifies frequency into the nearest guitar string note using
 geometric-mean boundaries, then updates cents and percent_error*/

void Processor(float frequency)
{
    if      (frequency < 95.11f)  note = E2;
    else if (frequency < 127.00f) note = A2;
    else if (frequency < 169.57f) note = D3;
    else if (frequency < 220.00f) note = G3;
    else if (frequency < 285.30f) note = B3;
    else                          note = E4;

    float target = notes[note].frequency;
    percent_error = 100.0f * (frequency - target) / target;
    cents         = 1200.0f * log2f(frequency / target);
}
