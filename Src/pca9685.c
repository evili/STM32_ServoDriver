/*
 * pca9685.c
 *
 *  Created on: 25 de maig 2020
 *      Author: evili
 */

#include "pca9685.h"
#include "i2c.h"

HAL_StatusTypeDef pca9685_init(pca9685_t *pca, I2C_HandleTypeDef *hi2c, uint8_t address) {
	HAL_StatusTypeDef status;
	pca->address = address;
	pca->hi2c = hi2c;
	status = pca9685_restart(pca);
	if(status == HAL_OK)
		status = pca9685_set_prescale(pca, PCA9685_SERVO_PRESCALE);
	return status;
}

HAL_StatusTypeDef pca9685_restart(pca9685_t *pca)
{
	uint8_t newmode = PCA9685_MODE1_RESTART;
	HAL_StatusTypeDef status;

	status = HAL_I2C_Mem_Write(pca->hi2c, pca->address, PCA9685_MODE1, I2C_MEMADD_SIZE_8BIT
			, &newmode, 1, PCA9685_MEM_WAIT);

	if (status==HAL_OK)
	{
		HAL_Delay(PCA9685_MODE1_RESTART_WAIT);
		// Get current MODE1
		status = HAL_I2C_Mem_Read(pca->hi2c, pca->address, PCA9685_MODE1, I2C_MEMADD_SIZE_8BIT,
				&newmode, 1, PCA9685_MEM_WAIT);
	}
	return status;
}

HAL_StatusTypeDef pca9685_sleep(pca9685_t *pca)
{
	HAL_StatusTypeDef status;
	uint8_t awake, sleep;

	status = HAL_I2C_Mem_Read(pca->hi2c, pca->address, PCA9685_MODE1, I2C_MEMADD_SIZE_8BIT, &awake, 1, PCA9685_MEM_WAIT);
	if(status == HAL_OK)
	{
		sleep = awake | PCA9685_MODE1_SLEEP;
		status = HAL_I2C_Mem_Write(pca->hi2c, pca->address, PCA9685_MODE1, I2C_MEMADD_SIZE_8BIT, &sleep, 1, PCA9685_MEM_WAIT);
	}
	return status;
}

HAL_StatusTypeDef pca9685_wakeup(pca9685_t *pca)
{
	HAL_StatusTypeDef status;
	uint8_t awake, sleep;

	status = HAL_I2C_Mem_Read(pca->hi2c, pca->address, PCA9685_MODE1, I2C_MEMADD_SIZE_8BIT, &sleep, 1, PCA9685_MEM_WAIT);
	if(status == HAL_OK)
	{
		awake = sleep & ~PCA9685_MODE1_SLEEP;
		status = HAL_I2C_Mem_Write(pca->hi2c, pca->address, PCA9685_MODE1, I2C_MEMADD_SIZE_8BIT, &awake, 1, PCA9685_MEM_WAIT);
	}
	return status;
}


HAL_StatusTypeDef pca9685_set_prescale(pca9685_t *pca, uint8_t prescale)
{
	uint8_t oldmode, newmode;
	HAL_StatusTypeDef status;

	prescale = (prescale < PCA9685_PRESCALE_MIN) ? PCA9685_PRESCALE_MIN : prescale;
	prescale = (prescale > PCA9685_PRESCALE_MAX) ? PCA9685_PRESCALE_MAX : prescale;
	// Save Present Mode
	status = HAL_I2C_Mem_Read(pca->hi2c, pca->address, PCA9685_MODE1, I2C_MEMADD_SIZE_8BIT, &oldmode, 1, 1);
	if(status == HAL_OK)
	{
		newmode =  (oldmode & ~PCA9685_MODE1_RESTART) | PCA9685_MODE1_SLEEP;
		// Enter Sleep Mode
		status = HAL_I2C_Mem_Write(pca->hi2c, pca->address, PCA9685_MODE1, I2C_MEMADD_SIZE_8BIT, &newmode, 1, PCA9685_MEM_WAIT);
		if (status == HAL_OK) {
			// Set PRE_SCALE
			status = HAL_I2C_Mem_Write(pca->hi2c, pca->address, PCA9685_PRE_SCALE, I2C_MEMADD_SIZE_8BIT, &prescale, 1, PCA9685_MEM_WAIT);
			if(status == HAL_OK) {
				// Restore Previous Mode
				status = HAL_I2C_Mem_Write(pca->hi2c, pca->address, PCA9685_MODE1, I2C_MEMADD_SIZE_8BIT, &oldmode, 1, PCA9685_MEM_WAIT);
				HAL_Delay(PCA9685_MODE1_RESTART_WAIT);
				// Restart with increment
				newmode = oldmode | PCA9685_MODE1_RESTART | PCA9685_MODE1_AI;
				status = HAL_I2C_Mem_Write(pca->hi2c, pca->address, PCA9685_MODE1, I2C_MEMADD_SIZE_8BIT, &newmode, 1, PCA9685_MEM_WAIT);
			}
		}
	}
	return status;
}

HAL_StatusTypeDef  pca9685_pwm(pca9685_t *pca,  uint8_t num, uint16_t on, uint16_t off)
{
	HAL_StatusTypeDef status;

	pca9685_led_t *led = &(pca->led[num]);
	if(off == 0) {
		led->LED_OFF_H = PCA9685_LED_FULL_OFF;
	}
	else if (on >= PCA9685_PWM_MAX){
		led->LED_ON_H = PCA9685_LED_FULL_ON;
	}
	else {
		led->LED_ON_L = on & 0x00FF;
		led->LED_ON_H = (on>>8) && 0x0F;
		led->LED_OFF_L = off & 0x00FF;
		led->LED_OFF_H = (off>>8) && 0x0F;
	}

	pca9685_led_t led_mem;

	uint8_t led_addr = PCA9685_LED_0_ON_L + (num*4);
	status = HAL_I2C_Mem_Write(pca->hi2c, pca->address, led_addr, I2C_MEMADD_SIZE_8BIT, &(led->LED_ON_L), 4, 1);
	if(status == HAL_OK) {
		status = HAL_I2C_Mem_Read(pca->hi2c, pca->address, led_addr, I2C_MEMADD_SIZE_8BIT, &(led_mem.LED_ON_L), 4, 1);
		if(status == HAL_OK)
			if(led->LED_OFF_H != led_mem.LED_OFF_H)
				status = HAL_ERROR;
	}
	return status;
}

HAL_StatusTypeDef  pca9685_servo(pca9685_t *pca,  uint8_t num, double angle)
{
	angle = (angle > 180.0) ? 180.0 : angle;
	angle = (angle <   0.0) ?   0.0 : angle;
	uint16_t on = 0;
	uint16_t off = on+PCA9685_OFF_MIN+(angle/180.0)*(PCA9685_OFF_MAX-PCA9685_OFF_MIN);
	return pca9685_pwm(pca, num, on, off);
}


HAL_StatusTypeDef  pca9685_load(pca9685_t *pca) {
	HAL_StatusTypeDef status = HAL_I2C_Mem_Read(pca->hi2c, pca->address, PCA9685_MODE1, I2C_MEMADD_SIZE_8BIT,
			         &(pca->pca9685_reg[0]), PCA9685_REGS, 100);
	if(status == HAL_OK)
		status = HAL_I2C_Mem_Read(pca->hi2c, pca->address, PCA9685_ALL_LED_ON_L, I2C_MEMADD_SIZE_8BIT,
					         &(pca->ALL_LED), PCA9685_LAST_REGS, 100);
	return status;
}



