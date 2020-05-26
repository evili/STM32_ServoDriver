/*
 * pca9685.c
 *
 *  Created on: 25 de maig 2020
 *      Author: evili
 */

#include "pca9685.h"
#include "i2c.h"

void pca9865_init(pca9685_t *pca, I2C_HandleTypeDef *hi2c, uint8_t address) {
	pca->address = address;
	pca->hi2c = hi2c;
	uint8_t oldmode;
	// Get current MODE1
	HAL_I2C_Mem_Read(pca->hi2c, pca->address, PCA9685_MODE1, I2C_MEMADD_SIZE_8BIT, &oldmode,1, 1);
	// If RESTART, clear SLEEP and wait 500us
	if(oldmode & PCA9685_MODE1_RESTART) {
		oldmode |= ~PCA9685_MODE1_SLEEP;
		HAL_I2C_Mem_Write(pca->hi2c, pca->address, PCA9685_MODE1, I2C_MEMADD_SIZE_8BIT, &oldmode, 1, 1);
		HAL_Delay(PCA9685_MODE1_RESTART_WAIT);
	}
	// Set RESTART mode.
	oldmode &= PCA9685_MODE1_RESTART;
	HAL_I2C_Mem_Write(pca->hi2c, pca->address, PCA9685_MODE1, I2C_MEMADD_SIZE_8BIT, &oldmode, 1, 1);
}

HAL_StatusTypeDef  pca9865_pwm(pca9685_t *pca,  uint8_t num, uint16_t on, uint16_t off)
{
	pca9685_led_t led = {.LED_ON = on, .LED_OFF = off};
	uint8_t led_addr = PCA9685_LED_0_ON_L + (num*4);
	return HAL_I2C_Mem_Write(pca->hi2c, pca->address, led_addr, I2C_MEMADD_SIZE_8BIT,(uint8_t *) &led, 4, 1);
}

HAL_StatusTypeDef  pca9865_servo(pca9685_t *pca,  uint8_t num, uint16_t angle)
{
	uint16_t on = 10;
	uint16_t off = on+150+(angle*500)/180;
	return pca9865_pwm(pca, num, on, off);
}


HAL_StatusTypeDef  pca9865_load(pca9685_t *pca) {
	return  HAL_I2C_Mem_Read(pca->hi2c, pca->address, PCA9685_MODE1, I2C_MEMADD_SIZE_8BIT, pca->pca9685_reg, PCA9685_REGS, 1000);
}



