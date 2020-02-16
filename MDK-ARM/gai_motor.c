#include "gai_motor.h"


uint32_t _gai_en = 0;
uint32_t _gai_a1 = 1;
uint32_t _gai_a2 = 2;

uint32_t _gai_motor_pins[5][3];
GPIO_TypeDef * _gai_gpio_x[5];

void gai_motor_init(
	int motorInstance,
	GPIO_TypeDef * gpiox, 
	uint32_t en, uint32_t a1, uint32_t a2) {
	LL_GPIO_InitTypeDef gpio;
	
		/*
	gpio.Pin = en | a1 | a2;
	_gai_motor_pins[motorInstance][_gai_en] = en;
	_gai_motor_pins[motorInstance][_gai_a1] = a1;
	_gai_motor_pins[motorInstance][_gai_a2] = a2;
		
	gpio.Mode = LL_GPIO_MODE_OUTPUT;
	gpio.Speed = LL_GPIO_SPEED_FREQ_HIGH;
	gpio.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
	gpio.Pull = LL_GPIO_PULL_NO;
	
	LL_GPIO_Init(gpiox, &gpio);
	_gai_gpio_x[motorInstance] = gpiox;
		*/

	
}
	
void gai_motor_turn_left(int motorInstance) {
	//LL_GPIO_SetOutputPin(_gai_gpio_x[motorInstance], _gai_motor_pins[motorInstance][0]);
}

void gai_motor_turn_right(int motorInstance) {
	//LL_GPIO_ResetOutputPin(_gai_gpio_x[motorInstance], _gai_motor_pins[motorInstance][0]);
}
