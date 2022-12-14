// SpaceInvaders.c
// Runs on TM4C123
// Jonathan Valvano and Daniel Valvano
// This is a starter project for the EE319K Lab 10

// Last Modified: 8/24/2022 
// http://www.spaceinvaders.de/
// sounds at http://www.classicgaming.cc/classics/spaceinvaders/sounds.php
// http://www.classicgaming.cc/classics/spaceinvaders/playguide.php

// ******* Possible Hardware I/O connections*******************
// Slide pot pin 1 connected to ground
// Slide pot pin 2 connected to PD2/AIN5
// Slide pot pin 3 connected to +3.3V 
// buttons connected to PE0-PE3
// 32*R resistor DAC bit 0 on PB0 (least significant bit)
// 16*R resistor DAC bit 1 on PB1
// 8*R resistor DAC bit 2 on PB2 
// 4*R resistor DAC bit 3 on PB3
// 2*R resistor DAC bit 4 on PB4
// 1*R resistor DAC bit 5 on PB5 (most significant bit)
// LED on PD1
// LED on PD0


#include <stdint.h>
#include "../inc/tm4c123gh6pm.h"
#include "../inc/ST7735.h"
#include "Random.h"
#include "TExaS.h"
#include "../inc/ADC.h"
#include "Images.h"
#include "Sound.h"
#include "Timer1.h"
#define bottom 159
#define max 15


void DisableInterrupts(void); // Disable interrupts
void EnableInterrupts(void);  // Enable interrupts
void Delay100ms(uint32_t count); // time delay in 0.1 seconds
void hitcheck(); //function that checks if anything hit each other
void draw();//draw anything that exist
void bulletmove();
void playermove();
void fire();
void spawn();
void reset();
void gameover();
void callme();
uint32_t Convert(uint32_t x);
void enemyfire(int x);
void enemymove();
int flag;
int counter=5;
int ce;

struct bhd{
	int coordx,coordy;
	const uint16_t *p;
	int w,h,exist,health,score;
};
typedef struct bhd state;
state enemy[12]={
	{0,0,SmallEnemy30pointB,16,10,0,0,30},//e1
	{0,0,SmallEnemy20pointB,16,10,0,0,20},//e2
	{0,0,SmallEnemy10pointB,16,10,0,0,10},//e3
	{0,0,SmallEnemy30pointB,16,10,0,0,30},//e4
	{0,0,SmallEnemy20pointB,16,10,0,0,20},//e5
	{0,0,SmallEnemy10pointB,16,10,0,0,10},//e6
	{0,0,SmallEnemy30pointB,16,10,0,0,30},//e7
	{0,0,SmallEnemy20pointB,16,10,0,0,20},//e8
	{0,0,SmallEnemy10pointB,16,10,0,0,10},//e9
	{0,0,SmallEnemy30pointB,16,10,0,0,30},//e10
	{0,0,SmallEnemy20pointB,16,10,0,0,20},//e11
	{0,0,SmallEnemy10pointB,16,10,0,0,10},//e12
};
state enemy_bullet[3]={ 
	{0,0,bullet,2,3,0,0,0},
	{0,0,bullet,2,3,0,0,0},
	{0,0,bullet,2,3,0,0,0}
};
state player[1]={ 
	{0,bottom,PlayerShip0,18,8,1,3,0}
};
state player_bullet[1]={ 
	{0,0,bullet,2,3,0,0,0}
};
state bunker[3]={
	{17,140,Bunker0,18,5,0,3,0},
	{55,140,Bunker0,18,5,0,3,0},
	{93,140,Bunker0,18,5,0,3,0}
};


void Timer1A_Handler(void){ // can be used to perform tasks in background
  TIMER1_ICR_R = TIMER_ICR_TATOCINT;// acknowledge TIMER1A timeout
   // execute user task
}

int main1(void){
  DisableInterrupts();
  TExaS_Init(NONE);       // Bus clock is 80 MHz 
  Random_Init(1);

  Output_Init();
  ST7735_FillScreen(0x0000);            // set screen to black
  
  ST7735_DrawBitmap(22, 159, PlayerShip0, 18,8); // player ship bottom
  ST7735_DrawBitmap(53, 151, Bunker0, 18,5);
  ST7735_DrawBitmap(42, 159, PlayerShip1, 18,8); // player ship bottom
  ST7735_DrawBitmap(62, 159, PlayerShip2, 18,8); // player ship bottom
  ST7735_DrawBitmap(82, 159, PlayerShip3, 18,8); // player ship bottom

  ST7735_DrawBitmap(0, 9, SmallEnemy10pointA, 16,10);
  ST7735_DrawBitmap(20,9, SmallEnemy10pointB, 16,10);
  ST7735_DrawBitmap(40, 9, SmallEnemy20pointA, 16,10);
  ST7735_DrawBitmap(60, 9, SmallEnemy20pointB, 16,10);
  ST7735_DrawBitmap(80, 9, SmallEnemy30pointA, 16,10);
  ST7735_DrawBitmap(100, 9, SmallEnemy30pointB, 16,10);

  Delay100ms(50);              // delay 5 sec at 80 MHz

  ST7735_FillScreen(0x0000);   // set screen to black
  ST7735_SetCursor(1, 1);
  ST7735_OutString("GAME OVER");
  ST7735_SetCursor(1, 2);
  ST7735_OutString("Nice try,");
  ST7735_SetCursor(1, 3);
  ST7735_OutString("Earthling!");
  ST7735_SetCursor(2, 4);
  ST7735_OutUDec(1234);
  while(1){
  }

}


// You can't use this timer, it is here for starter code only 
// you must use interrupts to perform delays
void Delay100ms(uint32_t count){uint32_t volatile time;
  while(count>0){
    time = 727240;  // 0.1sec at 80 MHz
    while(time){
      time--;
    }
    count--;
  }
}
typedef enum {English, Spanish, Portuguese, French} Language_t;
Language_t myLanguage=English;
typedef enum {HELLO, GOODBYE, LANGUAGE} phrase_t;
const char Hello_English[] ="Hello";
const char Hello_Spanish[] ="\xADHola!";
const char Hello_Portuguese[] = "Ol\xA0";
const char Hello_French[] ="All\x83";
const char Goodbye_English[]="Goodbye";
const char Goodbye_Spanish[]="Adi\xA2s";
const char Goodbye_Portuguese[] = "Tchau";
const char Goodbye_French[] = "Au revoir";
const char Language_English[]="English";
const char Language_Spanish[]="Espa\xA4ol";
const char Language_Portuguese[]="Portugu\x88s";
const char Language_French[]="Fran\x87" "ais";
const char *Phrases[3][4]={
  {Hello_English,Hello_Spanish,Hello_Portuguese,Hello_French},
  {Goodbye_English,Goodbye_Spanish,Goodbye_Portuguese,Goodbye_French},
  {Language_English,Language_Spanish,Language_Portuguese,Language_French}
};
//PortE initilize, PE0 and PE1 for language choosing, PE2 and PE3 for game starting/ending
	void PortE_Init(void){
	uint32_t delay;
	SYSCTL_RCGCGPIO_R |= 0x10;
	delay=0;
	SYSCTL_RCGCGPIO_R |= 0x10;
	while((SYSCTL_PRGPIO_R&0x10) == 0){};
	GPIO_PORTE_DIR_R &= ~0x0F;
	GPIO_PORTE_AFSEL_R &= ~0x0F;
	GPIO_PORTE_AMSEL_R &= ~0x0F;
	GPIO_PORTE_DEN_R |= 0x0F;
}
int score;
int main(void){ 
	char l;
  DisableInterrupts();
  TExaS_Init(NONE);       // Bus clock is 80 MHz 
  Output_Init();
	PortE_Init();
	ADC_Init();
	reset();
  flag=0;
	ce=max/2;
  while(1){
		int b=0;
		ST7735_FillScreen(0x0000); 
		ST7735_SetCursor(0,0);
		playermove();
		bulletmove();
		hitcheck();
		draw();
		fire();
		ce--;
		if(ce==0){
			callme();
		}
		if(player[0].health==0||enemy[2].coordy>=140){
			gameover();		
		}
		for (int i=0; i<12; i++)
		{
			b = b|enemy[i].exist;
		}
		if(b==0){
			spawn();
		}
  }  
}

void draw(){
	int i;
	for(i=0;i<12;i++){
		if(enemy[i].exist==1){
			ST7735_DrawBitmap(enemy[i].coordx,enemy[i].coordy,enemy[i].p,enemy[i].w,enemy[i].h);
		}
	}
	for(i=0;i<3;i++){
		if(enemy_bullet[i].exist==1){
			ST7735_DrawBitmap(enemy_bullet[i].coordx,enemy_bullet[i].coordy,enemy_bullet[i].p,enemy_bullet[i].w,enemy_bullet[i].h);
		}
	}
	for(i=0;i<3;i++){
		if(bunker[i].exist==1){
			ST7735_DrawBitmap(bunker[i].coordx,bunker[i].coordy,bunker[i].p,bunker[i].w,bunker[i].h);
		}
	}
		if(player[0].exist==1){
			if(player[0].health==3){
			ST7735_DrawBitmap(player[0].coordx,player[0].coordy,PlayerShip0,player[0].w,player[0].h);}
			else if(player[0].health==2){
				ST7735_DrawBitmap(player[0].coordx,player[0].coordy,PlayerShip1,player[0].w,player[0].h);
			}
			else if(player[0].health==1){
				ST7735_DrawBitmap(player[0].coordx,player[0].coordy,PlayerShip2,player[0].w,player[0].h);

			}
		}
		if(player_bullet[0].exist==1){
			ST7735_DrawBitmap(player_bullet[0].coordx,player_bullet[0].coordy,player_bullet[0].p,player_bullet[0].w,player_bullet[0].h);
		}
		ST7735_OutString("Score: ");
		ST7735_OutUDec(score);
}

void bulletmove(){
	if(player_bullet[0].exist==1){
		if(player_bullet[0].coordy<=0){
			player_bullet[0].exist=0;
		}
		else{
			player_bullet[0].coordy-=10;
		}
	}
	//else{
		for(int i=0;i<3;i++){
			if(enemy_bullet[i].exist==1){
				if(enemy_bullet[i].coordy>=bottom){
					enemy_bullet[i].exist=0;
				}
				else{
					enemy_bullet[i].coordy+=10;
				}
			}
		}
	//}
}
void playermove(){
	if(player[0].exist){
		player[0].coordx=Convert(ADC_In());
	}
}
void enemyfire(int x){
	if(enemy_bullet[0].exist==0){
		enemy_bullet[0].exist=1;
		enemy_bullet[0].coordy=enemy[x].coordy;
		enemy_bullet[0].coordx=enemy[x].coordx+(enemy[x].w/2);
	}
	else if(enemy_bullet[1].exist==0){
		enemy_bullet[1].exist=1;
		enemy_bullet[1].coordy=enemy[x].coordy;
		enemy_bullet[1].coordx=enemy[x].coordx+(enemy[x].w/2);
	}
	else if(enemy_bullet[2].exist==0){
		enemy_bullet[2].exist=1;
		enemy_bullet[2].coordy=enemy[x].coordy;
		enemy_bullet[2].coordx=enemy[x].coordx+(enemy[x].w/2);
	}
}
void fire(){
	if(player_bullet[0].exist==0){
		int val = GPIO_PORTE_DATA_R&0x04;
		val = val >> 2;
		if(val == 1)
		{
			player_bullet[0].exist=1;
			player_bullet[0].coordy=bottom;
			player_bullet[0].coordx=player[0].coordx+(player[0].w/2);
		}
	}
}
void reset(){
	player[0].exist=1;
	player[0].health=3;
	for(int i=0;i<3;i++){
		bunker[i].exist=1;
		bunker[i].health=3;
	}
	score=0;
	flag=0;
}
void spawn(){
    for (int i=0; i<12; i++)
    {
        enemy[i].exist = 1;
    }
    //point 30 on top
    enemy[0].coordx=22; 
    enemy[0].coordy=20;
    enemy[3].coordx=43; 
    enemy[3].coordy=20;
    enemy[6].coordx=64; 
    enemy[6].coordy=20;
    enemy[9].coordx=85; 
    enemy[9].coordy=20;
    //point 20 in middle
    enemy[1].coordx=22; 
    enemy[1].coordy=33;
    enemy[4].coordx=43; 
    enemy[4].coordy=33;
    enemy[7].coordx=64; 
    enemy[7].coordy=33;
    enemy[10].coordx=85; 
    enemy[10].coordy=33;
    //point 10 at bottom
    enemy[2].coordx=22; 
    enemy[2].coordy=46;
    enemy[5].coordx=43; 
    enemy[5].coordy=46;
    enemy[8].coordx=64; 
    enemy[8].coordy=46;
    enemy[11].coordx=85; 
    enemy[11].coordy=46;
}
void gameover(){
	int t=0;
	ST7735_FillScreen(0x0000); 
	ST7735_SetCursor(0,0);
	ST7735_OutString("Score: ");
	ST7735_OutUDec(score);
  ST7735_OutChar(13);
	ST7735_OutString("press pause to restart");
	while(t==0){
		int val = GPIO_PORTE_DATA_R&0x08;
		val = val >> 3;
		if(val == 1){
			t=1;
		}
	}
	spawn();
	reset();
}

uint32_t Convert(uint32_t x){
  // write this
  return x/35;
}

void hitcheck(){
	for(int i=0;i<12;i++){
			if(player_bullet[0].exist==1&&enemy[i].exist==1){
				if(player_bullet[0].coordx>=enemy[i].coordx
					&&player_bullet[0].coordx<=enemy[i].coordx+enemy[i].w
					&&player_bullet[0].coordy>=enemy[i].coordy-enemy[i].h
					&&player_bullet[0].coordy<=enemy[i].coordy){
						player_bullet[0].exist=0;
						enemy[i].exist=0;
						score+=enemy[i].score;
					}
			}
	}
	for(int i=0;i<3;i++){
		if(enemy_bullet[i].exist==1){
			if(enemy_bullet[i].coordx>=player[0].coordx
				&&enemy_bullet[i].coordx<=player[0].coordx+player[0].w
				&&enemy_bullet[i].coordy>=player[0].coordy-player[0].h
				&&enemy_bullet[i].coordy<=player[0].coordy){
					enemy_bullet[i].exist=0;
					player[0].health--;			
				}
		}
	}
	for(int i=0;i<3;i++){
		for(int s=0;s<3;s++){
			if(bunker[s].exist==1&&enemy_bullet[i].exist==1){
				if(enemy_bullet[i].coordx>=bunker[s].coordx
				&&enemy_bullet[i].coordx<=bunker[s].coordx+bunker[s].w
				&&enemy_bullet[i].coordy>=bunker[s].coordy-bunker[s].h
				&&enemy_bullet[i].coordy<=bunker[s].coordy){
					enemy_bullet[i].exist=0;
					bunker[s].health--;
					if(bunker[s].health==0){
						bunker[s].exist=0;
					}
				}
			}
		}
	}
}

void enemymove(){
	for(int i=0;i<12;i++){
		if(enemy[i].p==SmallEnemy10pointB){
			enemy[i].p=SmallEnemy10pointA;
		}
		else if(enemy[i].p==SmallEnemy10pointA){
			enemy[i].p=SmallEnemy10pointB;
		}
		else if(enemy[i].p==SmallEnemy20pointA){
			enemy[i].p=SmallEnemy20pointB;
		}
		else if(enemy[i].p==SmallEnemy20pointB){
			enemy[i].p=SmallEnemy20pointA;
		}
		else if(enemy[i].p==SmallEnemy10pointA){
			enemy[i].p=SmallEnemy10pointA;
		}
		else if(enemy[i].p==SmallEnemy30pointB){
			enemy[i].p=SmallEnemy30pointA;
		}
		else if(enemy[i].p==SmallEnemy30pointA){
			enemy[i].p=SmallEnemy30pointB;
		}
	}
	if(flag==0){
		if(enemy[9].coordx>=110){
			for(int i=0;i<12;i++){
				enemy[i].coordy+=5;
			}
			flag=1;
		}
		else{
			for(int i=0;i<12;i++){
				enemy[i].coordx+=5;
			}
		}
	}
	else{
		if(enemy[0].coordx<=2){
			for(int i=0;i<12;i++){
				enemy[i].coordy+=5;
			}
			flag=0;
		}
		else{
			for(int i=0;i<12;i++){
				enemy[i].coordx-=5;
			}
		}
	}
}
void callme(){
		counter--;
	  enemymove();
		if(counter==0){
			enemyfire((Random32()>>24)%12);
			counter=(Random32()>>24)%5+1;
		}
		ce=max;
		for(int i=0;i<12;i++){
			if(enemy[i].exist==0){
				ce--;
			}
		}
		ce=ce/2;
		
}