#ifndef _TOGGLE_LED_H_
#define _TOGGLE_LED_H_

#include "stm32f4xx_ll_usart.h"
#include "stm32f4xx_ll_bus.h"
#include "stm32f4xx_ll_gpio.h"

#include <stm32f4xx_hal.h>

#include "yacl.h"
#include "printf.h"

// STM32F4-Nucleo green led - PA5
#define LED_PORT                GPIOA
#define LED_PIN                 LL_GPIO_PIN_5
#define LED_PORT_CLK_ENABLE()   do{ RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN; } while(0)

// initialize gpio and uart
void initGPIO()
{
	LED_PORT_CLK_ENABLE();

	LL_GPIO_SetPinMode(LED_PORT, LED_PIN, LL_GPIO_MODE_OUTPUT);
	LL_GPIO_SetPinOutputType(LED_PORT, LED_PIN, LL_GPIO_OUTPUT_PUSHPULL);
}

void initUART()
{
	/* Peripheral clock enable */
	LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_UART5);

	LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOC);
	LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOD);

	/**UART5 GPIO Configuration
	PC12   ------> UART5_TX
	PD2   ------> UART5_RX
	*/
	LL_GPIO_InitTypeDef GPIO_InitStruct = { 0 };

	GPIO_InitStruct.Pin         = LL_GPIO_PIN_12;
	GPIO_InitStruct.Mode        = LL_GPIO_MODE_ALTERNATE;
	GPIO_InitStruct.Speed       = LL_GPIO_SPEED_FREQ_VERY_HIGH;
	GPIO_InitStruct.OutputType  = LL_GPIO_OUTPUT_PUSHPULL;
	GPIO_InitStruct.Pull        = LL_GPIO_PULL_NO;
	GPIO_InitStruct.Alternate   = LL_GPIO_AF_8;
	LL_GPIO_Init(GPIOC, &GPIO_InitStruct);

	GPIO_InitStruct.Pin         = LL_GPIO_PIN_2;
	GPIO_InitStruct.Mode        = LL_GPIO_MODE_ALTERNATE;
	GPIO_InitStruct.Speed       = LL_GPIO_SPEED_FREQ_VERY_HIGH;
	GPIO_InitStruct.OutputType  = LL_GPIO_OUTPUT_PUSHPULL;
	GPIO_InitStruct.Pull        = LL_GPIO_PULL_NO;
	GPIO_InitStruct.Alternate   = LL_GPIO_AF_8;
	LL_GPIO_Init(GPIOD, &GPIO_InitStruct);

	/* UART5 interrupt Init */
	NVIC_SetPriority(UART5_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(),0, 0));
	NVIC_EnableIRQ(UART5_IRQn);

	LL_USART_InitTypeDef USART_InitStruct = { 0 };

	USART_InitStruct.BaudRate               = 115200;
	USART_InitStruct.DataWidth              = LL_USART_DATAWIDTH_8B;
	USART_InitStruct.StopBits               = LL_USART_STOPBITS_1;
	USART_InitStruct.Parity                 = LL_USART_PARITY_NONE;
	USART_InitStruct.TransferDirection      = LL_USART_DIRECTION_TX_RX;
	USART_InitStruct.HardwareFlowControl    = LL_USART_HWCONTROL_NONE;
	USART_InitStruct.OverSampling           = LL_USART_OVERSAMPLING_16;
	LL_USART_Init(UART5, &USART_InitStruct);

	LL_USART_ConfigAsyncMode(UART5);
	LL_USART_Enable(UART5);
}

// implement printf
void my_print_func(char data)
{
	LL_USART_TransmitData8(UART5, (uint8_t)data);
	HAL_Delay(3);
}

void _putchar(char character)
{
	my_print_func(character);
}

// interrupt handlers
void UART5_IRQHandler(void)
{
	if (LL_USART_IsActiveFlag_RXNE(UART5) && LL_USART_IsEnabledIT_RXNE(UART5))
	{
		char rx_data = LL_USART_ReceiveData8(UART5);
		yacl_wr_buf(rx_data);

		LL_USART_TransmitData8(UART5, (uint8_t)rx_data);
	}
}

void SysTick_Handler(void)
{
	HAL_IncTick();
}

#endif //_TOGGLE_LED_H_
