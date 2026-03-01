#include <stdlib.h>
#include <float.h>
#include <math.h>
#include <string.h>
#include "commonvars.h"
#include "intro.h"
#include "sound.h"
#include "game.h"
#include "savestate.h"
#include "framebuffer.h"
#include "lib/ESPboyInit.h"
#include "lib/ESPboyInit.cpp"
#include "lib/ESPboyMCP.cpp"
#include "lib/ESPboyLED.cpp"

ESPboyInit myESPboy;
const uint16_t timePerFrame =  1000000 / FPS; 
static float frameRate = 0;
static uint32_t currentTime = 0, lastTime = 0, frameTime = 0;
static bool endFrame = true;

uint32_t getFreeRam() {	
	return ESP.getFreeHeap();
}

void printDebugCpuRamLoad()
{
    if(debugMode)
    {
        int currentFPS = (int)frameRate;
        char debuginfo[80];
        
        int fps_int = (int)frameRate;
        int fps_frac = (int)((frameRate - fps_int) * 100);
        sprintf(debuginfo, "F:%3d.%2d R:%3d ", fps_int, fps_frac, getFreeRam());
        //Serial.println(debuginfo); 
        bufferPrint(fb, 0, 0, debuginfo, myESPboy.tft.color565(255,255,255), myESPboy.tft.color565(0,0,0), 1, font);
    }
}

void setup()
{   
    myESPboy.begin("Formula 1 v1.1");
    fb = createFramebuffer(myESPboy.tft.width(), myESPboy.tft.height(), true, false);
    initSound();
    initMusic();
    setSoundOn(true);
    setMusicOn(true);
    loadSaveState();
    gameState = gsInitIntro;
    currentTime = micros();
    lastTime = 0;
}

void loop()
{   
    currentTime = micros();
    frameTime  = currentTime - lastTime;      
    if((frameTime < timePerFrame) || !endFrame)
       return;            
    endFrame = false;;
    frameRate = 1000000.0 / frameTime;
    lastTime = currentTime;
    processSound();
    prevbuttons = buttons;
    buttons = myESPboy.getKeys();
    //gamestate handling

    if((buttons & PAD_UP) && (buttons & PAD_DOWN) && !(prevbuttons & PAD_DOWN))
        debugMode = !debugMode;

    switch (gameState)
    {
        case gsInitIntro:
        case gsIntro:
            intro();
            break;
        case gsInitGame:
        case gsGame:
            game();
            break;
        case gsInitGameIntro:
        case gsGameIntro:
            gameIntro();
            break;
        case gsInitGameOver:
        case gsGameOver:
            gameOver();
            break;
        default:
            break;
    }

    printDebugCpuRamLoad();
    
    myESPboy.tft.pushImage(0,0,fb->width,fb->height,fb->buffer);
    endFrame = true;
}
