/**
 * Title: Sailor Moon Music Box
 * Author: D Cooper Dalrymple
 * Created: 10/27/2020
 * Updated: 10/27/2020
 * Description: Simple music box melody using Mozzi audio synthesis library. Made for Adafruit Trinket M0 with modified 10 bit Mozzi HiFi mode. Powered by LiPo battery and controlled with power shutoff switch.
 * https://dcooperdalrymple.com/
 */

#include <MozziGuts.h>
#include <Oscil.h>
#include <tables/sin2048_int8.h>
#include <EventDelay.h>
#include <ADSR.h>

#include "notes.h"

#define CONTROL_RATE 128

#define NUM_NOTES 43
#define TEMPO 140
#define NOTE_SPEED 60*4*1000/TEMPO
#define NOTE_ATTACK 25
#define NOTE_RELEASE NOTE_SPEED * 1.5

int melody[NUM_NOTES] = {
  NOTE_OFF, NOTE_C5, NOTE_F5, NOTE_GS5, // Starts 2 beats in
  NOTE_C6, NOTE_C6, NOTE_AS5, NOTE_AS5, NOTE_GS5,
  NOTE_G5, NOTE_AS5, NOTE_OFF, NOTE_C5, NOTE_E5, NOTE_G5,
  NOTE_AS5, NOTE_AS5, NOTE_GS5, NOTE_GS5, NOTE_G5,
  NOTE_F5, NOTE_GS5, NOTE_OFF, NOTE_C5, NOTE_F5, NOTE_GS5,
  NOTE_C6, NOTE_C6, NOTE_AS5, NOTE_AS5, NOTE_C6,
  NOTE_DS6, NOTE_CS6, NOTE_OFF, NOTE_CS6, NOTE_C6, NOTE_AS5,
  NOTE_C6, NOTE_AS5, NOTE_GS5, NOTE_G5, NOTE_F5, // Last note ties into next measure
  NOTE_OFF
};

// 4 = quarter note, 8 = eighth note, etc.
int noteDurations[NUM_NOTES] = {
  8, 8, 8, 8,
  4, 8, 4, 8, 4,
  4, 4, 8, 8, 8, 8,
  4, 8, 4, 8, 4,
  4, 4, 8, 8, 8, 8,
  4, 8, 4, 8, 4,
  4, 4, 8, 8, 8, 8,
  4, 4, 8, 4, 8,
  2
};

Oscil <SIN2048_NUM_CELLS, AUDIO_RATE> oscillator(SIN2048_DATA);

ADSR <CONTROL_RATE, CONTROL_RATE> envelope;

EventDelay noteDelay;
int currentIndex = -1;
byte vca = 1;

void setup() {
  startMozzi(CONTROL_RATE);
  
  envelope.setADLevels(255, 255);
  envelope.setTimes(NOTE_ATTACK, 0, 0, NOTE_RELEASE);
}

void updateControl() {
  if (noteDelay.ready()) {
    currentIndex++;
    if (currentIndex >= NUM_NOTES) currentIndex = 0;

    if (melody[currentIndex] != NOTE_OFF) {
      oscillator.setFreq(melody[currentIndex]);
      envelope.noteOn();
    }

    // Set delay for next note
    noteDelay.set(NOTE_SPEED / noteDurations[currentIndex]);
    noteDelay.start();
  }
  envelope.update();
  vca = envelope.next();
}

int updateAudio() {
  return (int) (vca * oscillator.next()) >> 6;
}

void loop() {
  audioHook();
}
