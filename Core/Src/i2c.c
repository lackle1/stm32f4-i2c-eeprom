/***********************************************************************************
 * @file        i2c.c                                                              *
 * @author      Lachie Keane                                                       *
 * @addtogroup  I2C                                                                *
 * @brief       Implements functions for I2C interface initialization and          *
 *              communication.                                                     *
 ***********************************************************************************/

#include "i2c.h"

/**
 * @brief  Initialises I2C
 *
 * @param  i2c Pointer to the I2C_TypeDef struct representing the I2C interface.
 *
 * @return @c NULL
 **/
void I2C_config(I2C_TypeDef *i2c) {

    // Enable GPIO and I2C clocks
    RCC->APB1ENR |= RCC_APB1ENR_I2C1EN;  // I2C
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOBEN;   // GPIOB

    // Configure pins PB8 and PB9 to use alternate functions
    GPIOB->MODER |= 2<<16 | 2<<18;      // Alternative function mode (0b10)
    GPIOB->OTYPER |= 1<<8 & 1<<9;       // Output open-drain
    GPIOB->OSPEEDR |= 3<<16 | 3<<18;    // Highspeed (0b11)
    GPIOB->PUPDR |= 1<<16 | 1<<18;      // Set pins to pull-up

    // Configure which alternate function will be used
    GPIOB->AFR[1] |= 4<<0 | 4<<4;       // Set each to AF4 (I2C)

    // Reset I2C
    i2c->CR1 |= I2C_CR1_SWRST;
    i2c->CR1 &= ~I2C_CR1_SWRST;

    i2c->CR2 |= 42;                    // Set frequency to the APB1 frequency (MHz)

    // Configure clock control register
    //i2c->CCR &= ~(1<<15);            // Standard mode. Not needed because it is set to 0 on reset anyway.
    i2c->CCR |= 210;                   // Calculated from reference manual. Datasheet values for clock times were missing so I looked at similar boards.

    i2c->TRISE |= 46;                  // Configure TRISE
    i2c->CR1 |= I2C_CR1_PE;                  // Enable peripheral
}

/**
 * @brief  Prepares for  I2C transmission
 *
 * @param  i2c Pointer to the I2C_TypeDef struct representing the I2C interface.
 *
 * @return @c NULL
 **/
void I2C_start(I2C_TypeDef *i2c) {
    i2c->CR1 |= I2C_CR1_ACK;                // Enable ACK
    i2c->CR1 |= I2C_CR1_START;              // Set START bit
    while (!(i2c->SR1 & I2C_SR1_SB));       // Wait for the SB bit to be set
}

/**
 * @brief  Sets the 7-bit address of the slave
 *
 * @param  i2c  Pointer to the I2C_TypeDef struct representing the I2C interface.
 * @param  addr 7-bit address
 *  
 * @return @c NULL
 **/
void I2C_sendAddress(I2C_TypeDef *i2c, uint8_t addr) {
    i2c->DR = addr;
    while (!(i2c->SR1 & I2C_SR1_ADDR));         // Wait for ADDR bit to be set
    (void)(i2c->SR1 | i2c->SR2);        // Read status registers to clear ADDR
}

/**
 * @brief  Stops transmission
 *
 * @param  i2c  Pointer to the I2C_TypeDef struct representing the I2C interface.
 *  
 * @return @c NULL
 **/
void I2C_stop(I2C_TypeDef *i2c) {
    i2c->CR1 |= I2C_CR1_STOP;   // Sets stop bit
}

/**
 * @brief  Sends data to the device at the address
 *
 * @param  i2c  Pointer to the I2C_TypeDef struct representing the I2C interface.
 * @param  addr 7-bit address
 * @param  data Pointer to the beginning of the data
 * @param  size Number of bytes to be sent
 *  
 * @return @c NULL
 **/
void I2C_write(I2C_TypeDef *i2c, uint8_t addr, uint8_t *data, uint8_t size) {

    for (int i = 0; i < size; i++) {
        while (!(i2c->SR1 & I2C_SR1_TXE));    // Wait for TxE bit to be set (data register empty)
        i2c->DR = data[i];
        size--;
    }

    while (!(i2c->SR1 & I2C_SR1_BTF));        // Wait for BTF to be set (byte transfer finished)
}

/**
 * @brief  Reads data into a buffer
 *
 * @param  i2c  Pointer to the I2C_TypeDef struct representing the I2C interface
 * @param  addr 7-bit address
 * @param  buf  Buffer where the data will be written
 * @param  size Numbe of bytes to be read
 *  
 * @return @c NULL
 **/
void I2C_read(I2C_TypeDef *i2c, uint8_t addr, uint8_t *buf, uint8_t size) {

    if (size == 1) {
        i2c->DR = addr;
        while (!(i2c->SR1 & I2C_SR1_ADDR));             // Wait for ADDR bit to be set

        i2c->CR1 &= ~(I2C_CR1_ACK);                     // Disable ACK
        i2c->CR1 |= I2C_CR1_POS;                        // Set POS bit
        
        i2c->CR1 |= I2C_CR1_STOP;                       // STOP

        while (!(i2c->SR1 & I2C_SR1_RXNE));             // Wait until RxNE is set (data register not empty)
        buf[0] = i2c->DR;
    }
    else {
        uint8_t remaining = size;

        while (remaining > 2) {
            while (!(i2c->SR1 & I2C_SR1_RXNE));         // Wait until RxNE is set (data register not empty)
            buf[size - remaining] = i2c->DR;
            i2c->CR1 |= I2C_CR1_ACK;                    // Enable ACK (to acknowledge data has been received)
            remaining--;
        }

        // Read second last byte
        while (!(i2c->SR1 & I2C_SR1_RXNE));             // Wait until RxNE is set (data register not empty)
        buf[size - remaining] = i2c->DR;

        i2c->CR1 &= ~I2C_CR1_ACK;                       // Disable ACK
        I2C_stop(i2c);

        remaining--;

        // Read last byte
        while (!(i2c->SR1 & I2C_SR1_RXNE));             // Wait until RxNE is set (data register not empty)
        buf[size - remaining] = i2c->DR;
    }
}