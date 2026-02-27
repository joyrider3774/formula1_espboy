#ifndef commonvars_h
#define commonvars_h

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include "framebuffer.h"
#include "lib/ESPboyInit.h"

#define gsGame 0
#define gsIntro 1
#define gsGameIntro 2
#define gsGameOver 3

#define gsInitDiff 50

#define gsInitGame (gsInitDiff + gsGame)
#define gsInitIntro (gsInitDiff + gsIntro)
#define gsInitGameIntro (gsInitDiff + gsGameIntro)
#define gsInitGameOver (gsInitDiff + gsGameOver)


#define FPS 30


extern char* verifyCode;
extern uint8_t gameState, debugMode;
extern bool EnemyStates[3][3];
extern bool PlayerStates[3];
extern uint8_t HitPosition, LivesLost;
extern uint32_t Teller, FlashesDelay, Flashes, Delay;
extern bool CanMove, CrashSoundPlayed;
extern uint32_t frames;
extern uint8_t alternateSource;
extern uint32_t Score;
extern uint8_t buttons,prevbuttons;
extern Framebuffer *fb;
extern ESPboyInit myESPboy;
#endif