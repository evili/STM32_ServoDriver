/*
 * pca9685.c
 *
 *  Created on: 25 de maig 2020
 *      Author: evili
 */

#include "pca9685.h"
#include "i2c.h"

HAL_StatusTypeDef pca9865_init(pca9685_t *pca, I2C_HandleTypeDef *hi2c, uint8_t address) {
	HAL_StatusTypeDef status;
	pca->address = address;
	pca->hi2c = hi2c;
	// Restart
	status = pca9865_restart(pca);
	// Set PRE_SCALE
	if(status == HAL_OK)
		status = pca9865_set_prescale(pca, PCA9685_SERVO_PRESCALE);
	return status;
}


HAL_StatusTypeDef pca9865_restart(pca9685_t *pca)
{
	uint8_t oldmode;
	HAL_StatusTypeDef status;
	// Get current MODE1
	status = HAL_I2C_Mem_Read(pca->hi2c, pca->address, PCA9685_MODE1, I2C_MEMADD_SIZE_8BIT, &oldmode, 1, 1);

	// If RESTART, clear SLEEP and wait 500us
	if ((status == HAL_OK) && (oldmode & PCA9685_MODE1_RESTART)) {
		oldmode |= ~PCA9685_MODE1_SLEEP;
		status = HAL_I2C_Mem_Write(pca->hi2c, pca->address, PCA9685_MODE1, I2C_MEMADD_SIZE_8BIT, &oldmode, 1, 1);
		HAL_Delay(PCA9685_MODE1_RESTART_WAIT);
	}
	if (status == HAL_OK)
	{
		// Set RESTART mode.
		oldmode &= PCA9685_MODE1_RESTART;
		status = HAL_I2C_Mem_Write(pca->hi2c, pca->address, PCA9685_MODE1, I2C_MEMADD_SIZE_8BIT, &oldmode, 1, 1);
	}
	return status;
}

HAL_StatusTypeDef pca9865_set_prescale(pca9685_t *pca, uint8_t prescale)
{
	uint8_t oldmode;
	HAL_StatusTypeDef status;

	// Save Present Mode
	status = HAL_I2C_Mem_Read(pca->hi2c, pca->address, PCA9685_MODE1, I2C_MEMADD_SIZE_8BIT, &oldmode, 1, 1);
	if(status == HAL_OK)
	{
		oldmode &= PCA9685_MODE1_SLEEP;
		// Enter Sleep Mode
		status = HAL_I2C_Mem_Write(pca->hi2c, pca->address, PCA9685_MODE1, I2C_MEMADD_SIZE_8BIT, &oldmode, 1, 1);
		if (status == HAL_OK) {
			// Set PRE_SCALE
			status = HAL_I2C_Mem_Write(pca->hi2c, pca->address, PCA9685_PRE_SCALE, I2C_MEMADD_SIZE_8BIT, &prescale, 1, 1);
			if(status == HAL_OK) {
				// Restore Previous Mode
				oldmode |= ~PCA9685_MODE1_SLEEP;
				status = HAL_I2C_Mem_Write(pca->hi2c, pca->address, PCA9685_MODE1, I2C_MEMADD_SIZE_8BIT, &oldmode, 1, 1);
			}
		}
	}
	HAL_Delay(PCA9685_MODE1_RESTART_WAIT);
	return status;
}

HAL_StatusTypeDef  pca9865_pwm(pca9685_t *pca,  uint8_t num, uint16_t on, uint16_t off)
{
	HAL_StatusTypeDef status;
	pca9685_led_t led = {.LEDONOFF = {on & 0x00FF, (on>>8) && 0x0F, off & 0x00FF, (off>>8) && 0x0F}
//			.LED_ON_L = on & 0x00FF,
//			.LED_ON_H = (on>>8) && 0x0F,
//			.LED_OFF_L = off & 0x00FF,
//			.LED_OFF_H = (off>>8) && 0x0F
	};
	pca9685_led_t led_mem;

	uint8_t led_addr = PCA9685_LED_0_ON_L + (num*4);
	status = HAL_I2C_Mem_Write(pca->hi2c, pca->address, led_addr, I2C_MEMADD_SIZE_8BIT, &(led.LEDONOFF[0]), 4, 1);
	if(status == HAL_OK) {
		status = HAL_I2C_Mem_Read(pca->hi2c, pca->address, led_addr, I2C_MEMADD_SIZE_8BIT, &(led_mem.LEDONOFF[0]), 4, 1);
		if(status == HAL_OK)
			if(led.LED_OFF_H != led_mem.LED_OFF_H)
				status = HAL_ERROR;
	}
	return status;
}

HAL_StatusTypeDef  pca9865_servo(pca9685_t *pca,  uint8_t num, double angle)
{
	angle = (angle > 170.0) ? 170.0 : angle;
	angle = (angle <  10.0) ?  10.0 : angle;
	uint16_t on = 0;
	uint16_t off = on+PCA9685_OFF_MIN+(angle/180.0)*(PCA9685_OFF_MAX-PCA9685_OFF_MIN);
	return pca9865_pwm(pca, num, on, off);
}


HAL_StatusTypeDef  pca9865_load(pca9685_t *pca) {
	return  HAL_I2C_Mem_Read(pca->hi2c, pca->address, PCA9685_MODE1, I2C_MEMADD_SIZE_8BIT, pca->pca9685_reg, PCA9685_REGS, 1000);
}



