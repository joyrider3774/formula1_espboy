#include <stdint.h>
#include "commonvars.h"
#include "printfuncs.h"
#include "intro.h"
#include "images/bigfont_RGB565_LE.h"
#include "framebuffer.h"

#define FRAMEDELAY 60


int ay;

void initIntro(void)
{
    ay = fb->height;
}

void intro(void)
{
    if (gameState == gsInitIntro)
    {
        initIntro();
        gameState -= gsInitDiff;
    }
    
    frames++;
    bufferFillRect(fb,0,0,fb->width,fb->height, 0x0000);
    if (frames < FRAMEDELAY)
    {
        printMessage(bigfont_data, (10-6) >> 1, 4, 12, "WILLEMS");
        printMessage(bigfont_data, (10-4) >> 1, 6, 12, " DAVY");
    }
    else
    {
        if (frames < FRAMEDELAY *2)
        {
            printMessage(bigfont_data, (10-8)>>1, 5, 12, "PRESENTS");
        }
        else
        {
            if(ay > 0)
            {
                ay -= 10;
            }
            else
            {
                gameState = gsInitGameIntro;
            }
        }
    }
        
    if (buttons & PAD_ANY)
    {            
        gameState = gsInitGameIntro;
    }
}
