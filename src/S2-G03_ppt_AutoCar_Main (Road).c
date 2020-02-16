/*Base register adddress header file*/
#include "stm32l1xx.h"
/*Library related header files*/
#include "stm32l1xx_ll_gpio.h"
#include "stm32l1xx_ll_pwr.h"
#include "stm32l1xx_ll_rcc.h"
#include "stm32l1xx_ll_bus.h"
#include "stm32l1xx_ll_utils.h"
#include "stm32l1xx_ll_system.h"
#include "stm32l1xx_ll_tim.h"

#include "stm32l152_glass_lcd.h"

#include "dwt_delay.h"
#include <stdio.h>


void SystemClock_Config(void);
void TIM_Config(void);
void Init_GPIO(void);

char disp_str[7];

uint32_t us_count = 0;
float us_pulse  = 0;
uint8_t us_flag = 0;
int go = 1;
int steps = 0;

int main() {
  SystemClock_Config();

  TIM_Config();
  Init_GPIO();

	DWT_Init();
	
	LCD_GLASS_Init();
	
	sprintf(disp_str, "GO");
  LCD_GLASS_DisplayString((uint8_t * ) disp_str);		
	
  while (1) {
		
		LL_GPIO_SetOutputPin(GPIOA, LL_GPIO_PIN_11);
		DWT_Delay(10);
		LL_GPIO_ResetOutputPin(GPIOA, LL_GPIO_PIN_11);
		
		
		while(!(LL_GPIO_IsInputPinSet(GPIOA, LL_GPIO_PIN_12)));
		
		us_count = 0;
		us_flag = 1;
		
		while(LL_GPIO_IsInputPinSet(GPIOA, LL_GPIO_PIN_12));
		us_pulse = (us_count/1000.0) * 340.0 / 2.0;
		
		if (us_pulse == 0) {
			sprintf(disp_str, "STOP");
			LCD_GLASS_DisplayString((uint8_t * ) disp_str);
			go = -1;

		} else {
			
		if (go == 1) {
			sprintf(disp_str, "GO");
			LCD_GLASS_DisplayString((uint8_t * ) disp_str);
			}
			
			us_flag = 0;
		}
		
		if (go == -1) {
			if (LL_GPIO_IsInputPinSet(GPIOA, LL_GPIO_PIN_0)) {
				go = 0;
			}
		}
		
		if (go == 0) {					
			sprintf(disp_str, "WALK");
			LCD_GLASS_DisplayString((uint8_t * ) disp_str);
			
			for (int i = 0; i < 12; i++) {
				LL_GPIO_TogglePin(GPIOC, LL_GPIO_PIN_13);
				LL_mDelay(500);
			}
			
			LL_GPIO_ResetOutputPin(GPIOC, LL_GPIO_PIN_13);
			
			go = 1;
			
			LCD_GLASS_Clear();
		}
	}

}

void Init_GPIO(void) {
  // Enable clock for GPIOB
  LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOA);

	LL_GPIO_InitTypeDef GPIO_InitStruct;
	
	
	
  GPIO_InitStruct.Pin = LL_GPIO_PIN_13;
  GPIO_InitStruct.Mode = LL_GPIO_MODE_OUTPUT;
  GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_HIGH;
  GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
  GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;

  LL_GPIO_Init(GPIOC, & GPIO_InitStruct);
	
	
  GPIO_InitStruct.Pin = LL_GPIO_PIN_11 | LL_GPIO_PIN_9;
  GPIO_InitStruct.Mode = LL_GPIO_MODE_OUTPUT;
  GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_HIGH;
  GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
  GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;

  LL_GPIO_Init(GPIOA, & GPIO_InitStruct);

	LL_GPIO_InitTypeDef input_a;
	input_a.Pin = LL_GPIO_PIN_0 | LL_GPIO_PIN_12;
	input_a.Mode = LL_GPIO_MODE_INPUT;
	input_a.Pull = LL_GPIO_PULL_DOWN;
	LL_GPIO_Init(GPIOA, &input_a);
}

void TIM_Config(void) {
  LL_TIM_InitTypeDef time_init;

  LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_TIM2);

  time_init.ClockDivision = LL_TIM_CLOCKDIVISION_DIV1;
  time_init.CounterMode = LL_TIM_COUNTERMODE_UP;
  time_init.Autoreload = 499;
  time_init.Prescaler = 63;

  LL_TIM_Init(TIM2, & time_init);
  LL_TIM_EnableIT_UPDATE(TIM2);

  // 0 is the highest priority
  NVIC_SetPriority(TIM2_IRQn, 0);
  NVIC_EnableIRQ(TIM2_IRQn);

  LL_TIM_EnableCounter(TIM2);
}

void TIM2_IRQHandler(void) {
  if (LL_TIM_IsActiveFlag_UPDATE(TIM2) == SET) {
    LL_TIM_ClearFlag_UPDATE(TIM2);
		
		us_count++;
  }
}
void SystemClock_Config(void)
{
  /* Enable ACC64 access and set FLASH latency */ 
  LL_FLASH_Enable64bitAccess();; 
  LL_FLASH_SetLatency(LL_FLASH_LATENCY_1);

  /* Set Voltage scale1 as MCU will run at 32MHz */
  LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_PWR);
  LL_PWR_SetRegulVoltageScaling(LL_PWR_REGU_VOLTAGE_SCALE1);
  
  /* Poll VOSF bit of in PWR_CSR. Wait until it is reset to 0 */
  while (LL_PWR_IsActiveFlag_VOSF() != 0)
  {
  };
  
  /* Enable HSI if not already activated*/
  if (LL_RCC_HSI_IsReady() == 0)
  {
    /* HSI configuration and activation */
    LL_RCC_HSI_Enable();
    while(LL_RCC_HSI_IsReady() != 1)
    {
    };
  }
  
	
  /* Main PLL configuration and activation */
  LL_RCC_PLL_ConfigDomain_SYS(LL_RCC_PLLSOURCE_HSI, LL_RCC_PLL_MUL_6, LL_RCC_PLL_DIV_3);

  LL_RCC_PLL_Enable();
  while(LL_RCC_PLL_IsReady() != 1)
  {
  };
  
  /* Sysclk activation on the main PLL */
  LL_RCC_SetAHBPrescaler(LL_RCC_SYSCLK_DIV_1);
  LL_RCC_SetSysClkSource(LL_RCC_SYS_CLKSOURCE_PLL);
  while(LL_RCC_GetSysClkSource() != LL_RCC_SYS_CLKSOURCE_STATUS_PLL)
  {
  };
  
  /* Set APB1 & APB2 prescaler*/
  LL_RCC_SetAPB1Prescaler(LL_RCC_APB1_DIV_1);
  LL_RCC_SetAPB2Prescaler(LL_RCC_APB2_DIV_1);

  /* Set systick to 1ms in using frequency set to 32MHz                             */
  /* This frequency can be calculated through LL RCC macro                          */
  /* ex: __LL_RCC_CALC_PLLCLK_FREQ (HSI_VALUE, LL_RCC_PLL_MUL_6, LL_RCC_PLL_DIV_3); */
  LL_Init1msTick(32000000);
  
  /* Update CMSIS variable (which can be updated also through SystemCoreClockUpdate function) */
  LL_SetSystemCoreClock(32000000);
}