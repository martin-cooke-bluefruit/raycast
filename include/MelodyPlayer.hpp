#ifndef MELODY_PLAYER_HPP
#define MELODY_PLAYER_HPP

void MelodyPlayer_SetPassiveBuzzerPin(int passiveBuzzerPin);
void MelodyPlayer_SetMelody(const int *melody, const int arraySize);
void MelodyPlayer_Play();

#endif