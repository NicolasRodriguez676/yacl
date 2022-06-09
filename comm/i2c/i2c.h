#ifndef _I2C_H_
#define _I2C_H_

#include <stddef.h>
#include <stdint.h>

typedef void(*comm_gpio)(void);
typedef void(*comm_gpio_get)(uint32_t*);

void i2c_init(comm_gpio usr_gpio_set, comm_gpio usr_gpio_rst, comm_gpio_get usr_gpio_state);
void i2c_write(uint32_t argc, char** argv, uint32_t* data_out);
void i2c_read(uint32_t argc, char** argv, uint32_t* data_out);

#endif //_I2C_H_
