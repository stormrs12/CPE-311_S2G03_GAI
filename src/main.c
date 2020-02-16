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
#include "stm32l1xx_ll_usart.h"
#include <string.h>
#include "ESP8266_lowlevel_conf.h"

#define MAX_RESP_BUFFER_SIZE			200

uint8_t resp[MAX_RESP_BUFFER_SIZE] = {0};
uint8_t idx;


const char* HelloText = "Hello";

void Init_GPIO(void);

uint8_t ESP8266_SendCmd(uint8_t* cmd)
{
	ESP_USART_LOWLEVEL_Transmit(cmd);
	while(1)
	{
		
		(ESP_USART_LOWLEVEL_Recv(resp, idx) != 1)?(idx = (idx + 1) % MAX_RESP_BUFFER_SIZE):(idx);		
		if(strstr((const char*)resp, "OK"))
		{
			return 0;
		}
	}
}

void ESP_ServerStart()
{
	while(1)
	{
		
		(ESP_USART_LOWLEVEL_Recv(resp, idx) != 1)?(idx = (idx + 1) % MAX_RESP_BUFFER_SIZE):(idx);		
		if(strstr((const char*)resp, "CONNECT"))
		{
			return;
		}
	}
}
void ESP8266_RespBufferReset(void)
{
	memset(resp, NULL, MAX_RESP_BUFFER_SIZE);
	idx = 0;
}

//TEST
uint8_t state = 0;
void SystemClock_Config(void);

int main()
{
	SystemClock_Config();
	Init_GPIO();
	
	LL_GPIO_SetOutputPin(GPIOB, LL_GPIO_PIN_6);
	
	ESP_USART_LOWLEVEL_Conf();
	ESP_USART_Start();
	
	
	ESP8266_SendCmd((uint8_t*)"AT+CWJAP=\"ChickenNetwork\",\"13371337\"\r\n");
	ESP8266_RespBufferReset();	
	ESP8266_SendCmd((uint8_t*)"AT+CWJAP?\r\n");
	ESP8266_RespBufferReset();	
	LL_mDelay(1000);
	
	ESP8266_SendCmd((uint8_t*)"AT+CIPMUX=0\r\n");
	ESP8266_RespBufferReset();
	
	LL_mDelay(1000);
	
	
	ESP8266_SendCmd((uint8_t*)"AT+CIPSTART=\"TCP\",\"192.168.43.28\",5005\r\n");
	ESP8266_RespBufferReset();
	
	ESP8266_SendCmd((uint8_t*)"AT+CIPSEND=6\r\n");
	ESP8266_SendCmd((uint8_t*)"First!");
	ESP8266_RespBufferReset();
	
	while(1)
	{
		(ESP_USART_LOWLEVEL_Recv(resp, idx) != 1)?(idx = (idx + 1) % MAX_RESP_BUFFER_SIZE):(idx);	 
	
		if(strstr((const char*)resp, "car_stop"))
		{
			LL_GPIO_ResetOutputPin(GPIOB, LL_GPIO_PIN_6);
			LL_GPIO_SetOutputPin(GPIOA, LL_GPIO_PIN_8);
			
			ESP8266_RespBufferReset();
			ESP8266_SendCmd((uint8_t*)"AT+CIPSEND=7\r\n");
			ESP8266_SendCmd((uint8_t*)"cs-stop");
			ESP8266_RespBufferReset();			
		}
		
		if(strstr((const char*)resp, "car_go"))
		{
			LL_GPIO_SetOutputPin(GPIOB, LL_GPIO_PIN_6);
			LL_GPIO_ResetOutputPin(GPIOA, LL_GPIO_PIN_8);
			ESP8266_RespBufferReset();			
		}	
	}
}


void Init_GPIO(void) {
  // Enable clock for GPIOB
  LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOB);
  LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOA);

	LL_GPIO_InitTypeDef GPIO_InitStruct;
	
  GPIO_InitStruct.Pin = LL_GPIO_PIN_6;
  GPIO_InitStruct.Mode = LL_GPIO_MODE_OUTPUT;
  GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_HIGH;
  GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
  GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;

  LL_GPIO_Init(GPIOB, & GPIO_InitStruct);
	
	
  GPIO_InitStruct.Pin = LL_GPIO_PIN_8;
  GPIO_InitStruct.Mode = LL_GPIO_MODE_OUTPUT;
  GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_HIGH;
  GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
  GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;

  LL_GPIO_Init(GPIOA, & GPIO_InitStruct);
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
