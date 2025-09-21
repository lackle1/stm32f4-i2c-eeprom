#ifndef I2C
#define I2C

#include "i2c.h"

void I2C_config(I2C_TypeDef *i2c);
void I2C_start(I2C_TypeDef *i2c);
void I2C_write(I2C_TypeDef *i2c, uint8_t data);
void I2C_writeMulti(I2C_TypeDef *i2c, uint8_t *data, uint8_t size);
void I2C_sendAddress(I2C_TypeDef *i2c, uint8_t addr);
void I2C_stop(I2C_TypeDef *i2c);

#endif