#include "stm32f4xx.h"							// Device header

/*
Control : PA0
Gas : PC0
Brake : PC1

*/

#define		CONTROL_PORT		GPIOA
#define BRAKE_AND_GAS_PORT		GPIOC

#define ENG_CONTROL			(1U<<0)
#define ENG_BRAKE			(1U<<1)
#define ENG_GAS				(1U<<0)

#define ENG_BRAKE_MODE_BIT		(1U<<2)
#define ENG_GAS_MODE_BIT		(1U<<1)

struct State {
	
	uint32_t output[2];
	uint32_t time;
	const struct State *next_state[2];
	
};

typedef const struct State stateType;

#define stop				&STATE_MACHINE[0]
#define go				&STATE_MACHINE[1]

stateType STATE_MACHINE[2]={
	{{2,0},100,{stop,go}},
	{{0,1},100,{stop,go}}
};

void delayMs(int n);
void EngineSystemInit();

stateType *statePtr;
uint32_t system_input;

int main(void){
	
	//Initialize hardware then set initial state
	EngineSystemInit();
	
	statePtr = stop;
	
	while(1){
		system_input = CONTROL_PORT->IDR & ENG_CONTROL;
		BRAKE_AND_GAS_PORT->ODR = statePtr -> output[system_input]; /*get new input from control*/
		delayMs(statePtr->time);
		statePtr = statePtr->next_state[system_input];
	}
}

void EngineSystemInit(){
	RCC->AHB1ENR != 0x01 | 0x04; //Enable clock access to PORTA and PORTC
	
	BRAKE_AND_GAS_PORT->MODER |= ENG_BRAKE_MODE_BIT | ENG_BRAKE_MODE_BIT;
	
}

void delayMs(int n){
	/*Configure SysTick*/
	SysTick->LOAD = 16000; /*Reload with number of clocks per millisecond*/
	SysTick->VAL = 0;			/*Clear current value register*/
	SysTick->CTRL = 0x05;	/*Enable the timer*/
	
	for(int i = 0; i < n; i++){
		while ((SysTick->CTRL & 0x10000)==0){}	/*Wait until COUNTFLAG is set*/
	}
	
	SysTick->CTRL = 0;
}
