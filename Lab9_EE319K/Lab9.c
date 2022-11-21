// Lab9.c
// Runs on LM4F120 or TM4C123
// Student names: solution, do not post
// Last modification date: change this to the last modification date or look very silly
// Last Modified: 8/24/2022 

// Analog Input connected to PD2=ADC1
// Labs 8 and 9 specify PD2
// displays on Sitronox ST7735
// PF3, PF2, PF1 are heartbeats
// UART1 on PC4-5
// * Start with where you left off in Lab8. 
// * Get Lab8 code working in this project.
// * Understand what parts of your main have to move into the UART1_Handler ISR
// * Rewrite the SysTickHandler
// * Implement the s/w Fifo on the receiver end 
//    (we suggest implementing and testing this first)

#include <stdint.h>

#include "../inc/ST7735.h"
#include "TExaS.h"
#include "../inc/ADC.h"
#include "../inc/tm4c123gh6pm.h"
#include "UART1.h"
#include "Fifo.h"
#define PF1       (*((volatile uint32_t *)0x40025008))
#define PF2       (*((volatile uint32_t *)0x40025010))
#define PF3       (*((volatile uint32_t *)0x40025020))
#define PC54      (*((volatile uint32_t *)0x400060C0)) // bits 5-4
#define PF321     (*((volatile uint32_t *)0x40025038)) // bits 3-1
// TExaSdisplay logic analyzer shows 7 bits 0,PC5,PC4,PF3,PF2,PF1,0 
void LogicAnalyzerTask(void){
  UART0_DR_R = 0x80|PF321|PC54; // sends at 10kHz
}

//*****the first three main programs are for debugging *****
// main1 tests just the ADC and slide pot, use debugger to see data
// main2 adds the LCD to the ADC and slide pot, ADC data is on Nokia
// main3 adds your convert function, position data is no Nokia

void DisableInterrupts(void); // Disable interrupts
void EnableInterrupts(void);  // Enable interrupts

uint32_t Data;      // 12-bit ADC
uint32_t Position;  // 32-bit fixed-point 0.001 cm
int32_t TxCounter = 0;

// Initialize Port F so PF1, PF2 and PF3 are heartbeats
void PortF_Init(void){
  SYSCTL_RCGCGPIO_R |= 0x20;      // activate port F
  while((SYSCTL_PRGPIO_R&0x20) != 0x20){};
  GPIO_PORTF_DIR_R |=  0x0E;   // output on PF3,2,1 (built-in LED)
  GPIO_PORTF_PUR_R |= 0x10;
  GPIO_PORTF_DEN_R |=  0x1E;   // enable digital I/O on PF
}
// **************SysTick_Init*********************
// Initialize Systick periodic interrupts
// Input: interrupt period
//        Units of period are 12.5ns
//        Maximum is 2^24-1
//        Minimum is determined by length of ISR
// Output: none
void SysTick_Init(uint32_t period){
  NVIC_ST_CTRL_R = 0;
	NVIC_ST_RELOAD_R = period - 1;
	NVIC_ST_CURRENT_R = 0;
	NVIC_ST_CTRL_R = 0x0007; //enable with interrupts
}

// Get fit from excel and code the convert routine with the constants
// from the curve-fit
uint32_t Convert(uint32_t input){
  // copy this from Lab 8 
    return (2100*input)/4095-74;
}

char dataReceive;
// final main program for bidirectional communication
// Sender sends using SysTick Interrupt
// Receiver receives using RX
int main(void){  
  DisableInterrupts();
  //PLL_Init();
  TExaS_Init(&LogicAnalyzerTask);
    // write this
	PortF_Init();
	ST7735_InitR(INITR_REDTAB); 
	SysTick_Init(8000000);
	ADC_Init();
	__nop();
	__nop();
	UART1_Init();
	__nop();
	__nop();
	Fifo_Init();
	__nop();
	__nop();
  EnableInterrupts();
  while(1){ // runs every 10ms
  // write this
	//ST7735_SetCursor(0,0);
	//		ST7735_OutString(" cm");
			Fifo_Get(&dataReceive);
		if(dataReceive=='<'){
//			ST7735_FillScreen(0);
			ST7735_SetCursor(0,0);
			Fifo_Get(&dataReceive);
			ST7735_OutChar(dataReceive);
			Fifo_Get(&dataReceive);
			ST7735_OutChar(dataReceive);		//"."
			Fifo_Get(&dataReceive);
			ST7735_OutChar(dataReceive);
			Fifo_Get(&dataReceive);
			ST7735_OutChar(dataReceive);
			Fifo_Get(&dataReceive);
			ST7735_OutChar(dataReceive);
			ST7735_OutString(" cm");

			Fifo_Get(&dataReceive);
	//		ST7735_SetCursor(0,1);
	//		ST7735_OutString("                 ");
				Fifo_Init();
  }
}
}


void SysTick_Handler(void){ // every 16.6 ms
 //Similar to Lab8 except rather than grab sample and put in mailbox
 //        format message and transmit 
  PF1 ^= 0x02;  // Heartbeat
	int holder;
	int holder1;
	Data=ADC_In();
	Data=Convert(Data);
	//UART1_OutChar(0x02);
	UART1_OutChar(0x3C);
	holder=Data/1000;
	holder1=holder+0x30;
	UART1_OutChar(holder1);
	UART1_OutChar(0x2E);
	Data=Data-(holder*1000);
	holder=Data/100;
	holder1=holder+0x30;
	UART1_OutChar(holder1);
	Data=Data-(holder*100);
	holder=Data/10;
	holder1=holder+0x30;
	UART1_OutChar(holder1);
	Data=Data-(holder*10);
	holder1=holder+0x30;
	UART1_OutChar(holder1);
	UART1_OutChar(0x3E);
	UART1_OutChar(0x0A);
	//UART1_OutChar(0x0D);
	//UART1_OutChar(0x03);
	TxCounter++;
}

uint32_t M;
uint32_t Random32(void){
  M = 1664525*M+1013904223;
  return M;
}  
uint32_t Random(uint32_t n){
  return (Random32()>>16)%n;
}  
void MyFifo_Init(uint32_t size);
uint32_t MyFifo_Put(char data);
uint32_t MyFifo_Get(char *datapt);
uint32_t FifoError;
int main2(void){ // Make this main to test Fifo
  uint32_t me,you;
  char mydata,yourdata,data;
  PLL_Init();
  PortF_Init();
  // your FIFO must be larger than 8 and less than 63
  Fifo_Init();     // your FIFO
  M = 4; // seed
  FifoError = 0;
  MyFifo_Init(60); // change 17 to match your FIFO size
  for(uint32_t i = 0; i<10000; i++){
    uint32_t k = Random(4);
    for(uint32_t l=0; l<k ;l++){
      data = Random(256);
      you = Fifo_Put(data);
      me = MyFifo_Put(data);
      if(me != you){
        FifoError++; // put a breakpoint here
      }
    } 
    uint32_t j = Random(40);
    for(uint32_t l=0; l<j ;l++){
      data = Random(256);
      you = Fifo_Put(data);
      me = MyFifo_Put(data);
      if(me != you){
        FifoError++; // put a breakpoint here
      }
    }
    for(uint32_t l=0; l<j ;l++){
      you = Fifo_Get(&yourdata);
      me = MyFifo_Get(&mydata);
      if((me != you)||(yourdata != mydata)){
        FifoError++; // put a breakpoint here
      }
    } 
    for(uint32_t l=0; l<k ;l++){
      you = Fifo_Get(&yourdata);
      me = MyFifo_Get(&mydata);
      if((me != you)||(yourdata != mydata)){
        FifoError++; // put a breakpoint here
      }
    }  
  }    
  if(FifoError == 0){
    for(;;){ // success
      GPIO_PORTF_DATA_R ^= 0x08; // green
      for(int i=0; i<100000; i++){
      }
    }
  }else{
    for(;;){ // failure
      GPIO_PORTF_DATA_R ^= 0x02; // red
      for(int i=0; i<50000; i++){
      }
    }
  }
}

