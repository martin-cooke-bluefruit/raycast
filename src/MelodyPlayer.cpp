#include "MelodyPlayer.hpp"

#include <Arduino.h>

static int _passiveBuzzerPin;
static int *_melody;
static int _melodyLength;
static int _currentMelodyPosition;
static IntervalTimer _noteTimer;

static void NextNote(void);

void MelodyPlayer_SetPassiveBuzzerPin(int passiveBuzzerPin)
{
  _passiveBuzzerPin = passiveBuzzerPin;
}

void MelodyPlayer_SetMelody(const int *melody, const int arraySize)
{
  if (_melody != nullptr)
    delete(_melody);

  _melodyLength = arraySize;
  _melody = new int[_melodyLength];

  for (int i = 0; i < _melodyLength; i++)
    *(_melody + i) = melody[i];
}

void MelodyPlayer_Play()
{
  _currentMelodyPosition = 0;

  NextNote();
}

static void NextNote(void)
{
  if (_currentMelodyPosition + 1 < _melodyLength)
  {
    const int note = *(_melody + _currentMelodyPosition);
    if (note > 31)
      tone(_passiveBuzzerPin, note);
    else
      noTone(_passiveBuzzerPin);
      
    _noteTimer.begin(NextNote, *(_melody + _currentMelodyPosition + 1) * 1000);
    _currentMelodyPosition += 2;
  }
  else
  {
    noTone(_passiveBuzzerPin);
    _noteTimer.end();
  }
}
