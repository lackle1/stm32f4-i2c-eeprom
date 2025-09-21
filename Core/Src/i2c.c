/***********************************************************************************
 * @file        i2c.c                                                              *
 * @author      Lachie Keane                                                       *
 * @addtogroup  I2C                                                                *
 * @brief       Implements functions for I2C device interface initialization and   *
 *              communication.                                                     *
 *                                                                                 *
 * TODO:        Add functions for receiving data                                   *
 *                                                                                 *
 *                                                                                 *
 ***********************************************************************************/

#include "i2c.h"
#include "stm32f4xx.h"

/**
 * @brief  Initialises I2C
 *
 * @param  i2c Pointer to the I2C_TypeDef struct representing the I2C interface.
 *
 * @return @c NULL
 **/
void I2C_config(I2C_TypeDef *i2c) {

    // Enable GPIO and I2C clocks
    RCC->APB1ENR |= 1<<21;  // I2C
    RCC->AHB1ENR |= 1<<1;   // GPIOB

    // Configure pins PB8 and PB9 to use alternate functions
    GPIOB->MODER |= 2<<16 | 2<<18;      // Alternative function mode (0b10)
    GPIOB->OTYPER |= 1<<8 & 1<<9;       // Output open-drain
    GPIOB->OSPEEDR |= 3<<16 | 3<<18;    // Highspeed (0b11)
    GPIOB->PUPDR |= 1<<16 | 1<<18;      // Set pins to pull-up

    // Configure which alternate function will be used
    GPIOB->AFR[1] |= 4<<0 | 4<<4;       // Set each to AF4 (I2C)

    // Reset I2C
    i2c->CR1 |= 1<<15;
    i2c->CR1 &= ~(1<<15);

    i2c->CR2 |= 42;                    // Set frequency to the APB1 frequency (MHz)

    // Configure clock control register
    //i2c->CCR &= ~(1<<15);            // Standard mode. Not needed because it is set to 0 on reset anyway.
    i2c->CCR |= 210;                   // Calculated from reference manual. Datasheet values for clock times were missing so I looked at similar boards.

    i2c->TRISE |= 46;                  // Configure TRISE
    i2c->CR1 |= 1<<0;                  // Enable peripheral
}

/**
 * @brief  Prepares for  I2C transmission
 *
 * @param  i2c Pointer to the I2C_TypeDef struct representing the I2C interface.
 *
 * @return @c NULL
 **/
void I2C_start(I2C_TypeDef *i2c) {
    i2c->CR1 |= 1<<8;                  // Set START bit
    while (!(i2c->SR1 & 1<<0));        // Wait for the SB bit to be set
}

/**
 * @brief  Writes a single byte of data to the address specified with 'sendAddress'
 *
 * @param  i2c  Pointer to the I2C_TypeDef struct representing the I2C interface.
 * @param  data One byte of data
 *  
 * @return @c NULL
 **/
void I2C_write(I2C_TypeDef *i2c, uint8_t data) {
    while (!(i2c->SR1 & 1<<7));        // Wait for TxE bit to be set (data register empty)
    i2c->DR = data;
    while (!(i2c->SR1 & 1<<2));        // Wait for BTF to be set (byte transfer finished)
}

/**
 * @brief  Writes multiple bytes of data to the address specified with 'sendAddress'
 *
 * @param  i2c  Pointer to the I2C_TypeDef struct representing the I2C interface.
 * @param  data Pointer to the beginning of the data
 * @param  size Number of bytes to be sent
 *  
 * @return @c NULL
 **/
void I2C_writeMulti(I2C_TypeDef *i2c, uint8_t *data, uint8_t size) {
    for (int i = 0; i < size; i++) {
        while (!(i2c->SR1 & 1<<7));    // Wait for TxE bit to be set (data register empty)
        i2c->DR = data[i];
        size--;
    }

    while (!(i2c->SR1 & 1<<2));        // Wait for BTF to be set (byte transfer finished)
}

/**
 * @brief  Sets the address of the slave receiver
 *
 * @param  i2c  Pointer to the I2C_TypeDef struct representing the I2C interface.
 * @param  addr The address of the slave receiver to which data will be sent
 *  
 * @return @c NULL
 **/
void I2C_sendAddress(I2C_TypeDef *i2c, uint8_t addr) {
    i2c->DR = addr;
    while (!(i2c->SR1 & 1<<1));            // Wait for ADDR bit to be set
    uint8_t tmp = i2c->SR1 | i2c->SR2;    // Read status registers to clear ADDR
}

/**
 * @brief  Stops transmission
 *
 * @param  i2c  Pointer to the I2C_TypeDef struct representing the I2C interface.
 *  
 * @return @c NULL
 **/
void I2C_stop(I2C_TypeDef *i2c) {
    i2c->CR1 |= 1<<9;   // Sets stop bit
}