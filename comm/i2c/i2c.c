#include "i2c.h"

static comm_gpio gpio_set = NULL;
static comm_gpio gpio_rst = NULL;
static comm_gpio_get gpio_state = NULL;

void i2c_init(comm_gpio usr_gpio_set, comm_gpio usr_gpio_rst, comm_gpio_get usr_gpio_state)
{
	gpio_set = usr_gpio_set;
	gpio_rst = usr_gpio_rst;
	gpio_state = usr_gpio_state;
}

void i2c_write(uint32_t argc, char** argv, uint32_t* data_out)
{
	if (argc == 0)
		return;

	if (argv[0][0] == 'w')
	{
		if (argv[1][0] == '1')
			gpio_set();
		else if (argv[1][0] == '0')
			gpio_rst();
	}
}

void i2c_read(uint32_t argc, char** argv, uint32_t* data_out)
{
	gpio_state(data_out);
}
