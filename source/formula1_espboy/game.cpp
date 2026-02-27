#include <Arduino.h>
#include <stdlib.h>
#include <string.h>
#include "commonvars.h"
#include "sound.h"
#include "printfuncs.h"
#include "savestate.h"
#include "images/enemy_RGB565_LE.h"
#include "images/player_RGB565_LE.h"
#include "images/background_RGB565_LE.h"
#include "images/lcdFont_RGB565_LE.h"
#include "framebuffer.h"

void DrawGame(void)
{
	int X, Y;
	for (X = 0; X < 3; X++)
		for (Y = 0; Y < 3; Y++)
			if (EnemyStates[X][Y])
			{
				bufferDrawImage(fb,22 + (X * 34), 10 + (Y * 36), enemy_data,enemy_width,enemy_height,false,true,false);
			}

	for (X = 0; X < 3; X++)
	{
		if (PlayerStates[X])
		{
			bufferDrawImage(fb,22 + (X * 34), 104, player_data, player_width, player_height,false,true,false);
		}
	}
}

void MoveEnemy(void)
{
	int X, Y;
	for (X = 0; X < 3; X++)
		for (Y = 2; Y >= 1; Y--)
			EnemyStates[X][Y] = EnemyStates[X][Y - 1];
	for (X = 0; X < 3; X++)
		EnemyStates[X][0] = false;
	for (X = 0; X <= 1; X++)
		EnemyStates[rand() % 3][0] = true;
}

void MoveLeft(void)
{
	int X;
	for (X = 0; X < 2; X++)
	{
		if (PlayerStates[X + 1])
		{
			PlayerStates[X] = true;
			PlayerStates[X + 1] = false;
		}
	}
}

void MoveRight(void)
{
	int X;
	for (X = 2; X >= 1; X--)
	{
		if (PlayerStates[X - 1])
		{
			PlayerStates[X] = true;
			PlayerStates[X - 1] = false;
		}
	}
}

bool IsCollided(void)
{
	int X;
	bool Temp;
	Temp = false;
	for (X = 0; X < 3; X++)
		if (PlayerStates[X] && EnemyStates[X][2])
		{
			Temp = true;
			HitPosition = X;
		}
	return Temp;
}

void HitFlash(void)
{
	PlayerStates[HitPosition] = !PlayerStates[HitPosition];
	EnemyStates[HitPosition][2] = !EnemyStates[HitPosition][2];
}

void InitialiseStates(void)
{
	int X, Y;
	for (X = 0; X < 3; X++)
		for (Y = 0; Y < 3; Y++)
			EnemyStates[X][Y] = false;
	for (X = 0; X < 3; X++)
		PlayerStates[X] = false;
	PlayerStates[1] = true;
}

void initGame(void)
{
	srand(millis());
	Teller = 25;
	FlashesDelay = 14*FPS/60;
	Flashes = 0;
	CanMove = true;
	Score = 0;
	Delay = 60 *FPS/60;
	LivesLost = 0;
	CrashSoundPlayed = false;
	InitialiseStates();
}


void game(void)
{
	
	int X;

    if (gameState == gsInitGame)
    {
        initGame();
        gameState -= gsInitDiff;
    }
   
    if (((buttons & PAD_RIGHT) && !(prevbuttons & PAD_RIGHT)) || 
		((buttons & PAD_ESC) && !(prevbuttons & PAD_ESC)))
    {
		if (CanMove)
		{
			MoveRight();
		}
    }

    if ((buttons & PAD_LEFT) && !(prevbuttons & PAD_LEFT))
    {
		if (CanMove)
		{
			MoveLeft();
		}
    }
	
	bufferDrawImage(fb,0, 0, background_data, background_width,background_height,false,true,false);
	printNumber(lcdfont_data, 2, 0, 8, getHiScore(), 5);
	printNumber(lcdfont_data, 9, 0, 8, Score, 5);
	if (LivesLost >= 1)
	{
		for (X = 0; X < LivesLost; X++)
			printMessage(lcdfont_data, 0, 15-X, 8, "X");
		DrawGame();		
	}

	Teller++;
	if (Teller >= Delay)
	{
		if (!IsCollided() && CanMove)
		{		
			Teller = 0;
			for (X = 0; X < 3; X++)
				if (EnemyStates[X][2])
				{
					Score += 10;
					if (Score > getHiScore())
						setHiScore(Score);
					if ((Score % 100 == 0) && (Delay > 8*FPS/60))
						Delay--;
				}
			MoveEnemy();
			playTickSound();			
		}
		else
		{
			if (!CrashSoundPlayed)
			{
				SelectMusic(musNone, 0);
				SelectMusic(musCrash, 0);
				CrashSoundPlayed = true;
			}
			CanMove = false;
			FlashesDelay++;
			if (FlashesDelay == 20*FPS/60)
			{
				Flashes++;
				HitFlash();
				FlashesDelay = 0;
				if (Flashes == 6)
				{
					Flashes = 0;
					CanMove = true;
					Teller = 0;
					CrashSoundPlayed = false;
					EnemyStates[HitPosition][2] = false;
					LivesLost++;
					FlashesDelay = 14*FPS/60;
					if (LivesLost == 3)
						gameState = gsInitGameOver;
				}
			}
		}
	}
	DrawGame();
}

void initGameOver(void)
{
	Teller = 0;
	Delay = 15;
}

void gameOver(void)
{
	uint8_t X;
	if (gameState == gsInitGameOver)
	{
		initGameOver();
		gameState -= gsInitDiff;
	}
	Teller++;
	if (Teller == Delay)
	{
		SelectMusic(musGameOver, 0);
	}
	if (buttons & PAD_ANY)
	{
		gameState = gsInitGame;
	}


	bufferDrawImage(fb, 0, 0, background_data, background_width,background_height,false,true,false);
	printNumber(lcdfont_data, 2, 0, 8, getHiScore(), 5);
	printNumber(lcdfont_data, 9, 0, 8, Score, 5);
	if (LivesLost >= 1)
	{
		for (X = 0; X < LivesLost; X++)
			printMessage(lcdfont_data, 0, 15-X, 8, "X");
		DrawGame();		
	}
}

void FlashIntro(void)
{
	uint8_t X, Y;
	for (X = 0; X < 3; X++)
		for (Y = 0; Y < 3; Y++)
			EnemyStates[X][Y] = !EnemyStates[X][Y];
	for (X = 0; X < 3; X++)
		PlayerStates[X] = !PlayerStates[X];
}


void gameIntroInit(void)
{
	LivesLost = 0;
	FlashesDelay = 0;
	Score = 0;
}

void gameIntro(void)
{
	uint8_t X;
	bufferFillRect(fb,0,0,myESPboy.tft.width(),myESPboy.tft.height(), 0x0000);
	if (gameState == gsInitGameIntro)
	{
		gameIntroInit();
		gameState -= gsInitDiff;
	}

	if (buttons & PAD_ANY)
	{
		gameState = gsInitGame;
	}

	bufferDrawImage(fb, 0, 0, background_data, background_width,background_height,false,true,false);
	FlashesDelay++;
	if (FlashesDelay == 25*FPS/60)
	{
		FlashesDelay = 0;
		FlashIntro();
		if (PlayerStates[0])
		{
			LivesLost = 3;
			Score = 88888;
		}
		else
		{
			LivesLost = 0;
			Score = 0;
		}
	}
	if (Score > 0)
	{
		printNumber(lcdfont_data, 2, 0, 8, Score, 5);
		printNumber(lcdfont_data, 9, 0, 8, Score, 5);
	}
	if (LivesLost >= 1)
	{
		for (X = 0; X < LivesLost; X++)
			printMessage(lcdfont_data, 0, 15-X, 8, "X");
	}
	DrawGame();		
}

