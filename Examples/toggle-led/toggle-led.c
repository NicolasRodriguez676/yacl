#include "stm32f4xx_ll_usart.h"
#include "stm32f4xx_ll_bus.h"
#include "stm32f4xx_ll_gpio.h"

#include <stm32f4xx_hal.h>

#include "yacl.h"

// STM32F4-Nucleo green led - PA5
#define LED_PORT                GPIOA
#define LED_PIN                 LL_GPIO_PIN_5
#define LED_PORT_CLK_ENABLE()   do{ RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN; } while(0)

void initGPIO();
void initUART();
void my_callback(uint32_t argc, char** argv);
void my_print_func(char data);

yacl_cmd_cb_t usr_cmd[] = {
	{ "help", my_callback },
	{ "read", my_callback },
	{ "write", my_callback },
};

void UART5_IRQHandler(void)
{
	if (LL_USART_IsActiveFlag_RXNE(UART5) && LL_USART_IsEnabledIT_RXNE(UART5))
	{
		char rx_data = LL_USART_ReceiveData8(UART5);
		yacl_wr_buf(rx_data);

		LL_USART_TransmitData8(UART5, (uint8_t)rx_data);
	}
}

int main(void)
{
	HAL_Init();

	initGPIO();
	initUART();

	// 1kHz ticks
	HAL_SYSTICK_Config(SystemCoreClock / 1000);

	yacl_init(usr_cmd, sizeof(usr_cmd) / sizeof(usr_cmd[0]), my_print_func);

	LL_USART_EnableIT_RXNE(UART5);

	while(1)
	{
		HAL_Delay(100);

		yacl_parse_cmd();
	}

	return 0;
}

void my_print_func(char data)
{
	LL_USART_TransmitData8(UART5, (uint8_t)data);
	HAL_Delay(5);
}

void my_callback(uint32_t argc, char** argv)
{
	if (argc == 0)
		return;

	if (argv[0][0] == 'w')
	{
		if (argv[1][0] == '1')
			LL_GPIO_SetOutputPin(LED_PORT, LED_PIN);
		else if (argv[1][0] == '0')
			LL_GPIO_ResetOutputPin(LED_PORT, LED_PIN);
	}
}

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

void SysTick_Handler(void)
{
	HAL_IncTick();
}
