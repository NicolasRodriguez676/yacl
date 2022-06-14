#include "stm32f4xx_setup.h"

void usr_gpio_write(yacl_inout_data_t* inout_data);
void usr_gpio_read(yacl_inout_data_t* inout_data);

int main(void)
{
	HAL_Init();

	initGPIO();
	initUART();

	// 1kHz ticks
	HAL_SYSTICK_Config(SystemCoreClock / 1000);

	yacl_usr_callbacks_t callbacks;
	yacl_set_cb_null(&callbacks);

	callbacks.usr_print_funcs.usr_printf = printf;
	callbacks.usr_print_funcs.usr_snprintf = snprintf;

	callbacks.usr_gpio_read = usr_gpio_read;
	callbacks.usr_gpio_write = usr_gpio_write;

	callbacks.usr_i2c_read = usr_gpio_read;
	callbacks.usr_i2c_write = usr_gpio_write;
	yacl_init(&callbacks);

	LL_USART_EnableIT_RXNE(UART5);

	while(1)
	{
		HAL_Delay(100);
		yacl_parse_cmd();
	}
	return 0;
}

void usr_gpio_write(yacl_inout_data_t* inout_data)
{
	if (inout_data->data)
		LL_GPIO_SetOutputPin(LED_PORT, LED_PIN);
	else
		LL_GPIO_ResetOutputPin(LED_PORT, LED_PIN);
}

void usr_gpio_read(yacl_inout_data_t* inout_data)
{
	for (uint32_t i = 0; i < inout_data->range; ++i)
		inout_data->bufr[i] = LL_GPIO_IsOutputPinSet(LED_PORT, LED_PIN);
}
