#ifndef __PCA9685_H__
#define __PCA9685_H__

#include <stdint.h>
#include "stm32f7xx_hal.h"

#define PCA9685_DEFAULT_ADDRESS 0x80
#define PCA9685_REGS 0x45
#define PCA9685_PWMS 0x10

#define PCA9685_MODE1         0x00u
#define PCA9685_MODE1_RESTART 0x80u
#define PCA9685_MODE1_EXTCLK  0x40u
#define PCA9685_MODE1_AI      0x20u
#define PCA9685_MODE1_SLEEP   0x10u
#define PCA9685_MODE1_SUB1    0x08u
#define PCA9685_MODE1_SUB2    0x04u
#define PCA9685_MODE1_SUB3    0x02u
#define PCA9685_MODE1_ALLCALL 0x01u

#define PCA9685_MODE2 0x01u
#define PCA9685_MODE2_RESERVED 0xE0u
#define PCA9685_MODE2_INVRT    0x10u
#define PCA9685_MODE2_OCH      0x08u
#define PCA9685_MODE2_OUTDRV   0x04u
#define PCA9685_MODE2_OUTNE_MASK 0x03u
#define PCA9685_MODE2_OUTNE_ZERO 0x00u
#define PCA9685_MODE2_OUTNE_OUT  0x01u
#define PCA9685_MODE2_OUTNE_HIZ  0x02u

#define PCA9685_LED_0_ON_L  0x06u

#define PCA9685_MODE1_RESTART_WAIT 1


typedef struct pca9685_led_s {
	union {
		uint16_t LED_ON;
		struct {
			uint8_t LED_ON_L;
			uint8_t LED_ON_H;
		};
	};
	union {
		uint16_t LED_OFF;
		struct {
			uint8_t LED_OFF_L;
			uint8_t LED_OFF_H;
		};
	};
} pca9685_led_t;

typedef struct pca9685_s {
	I2C_HandleTypeDef *hi2c;
	uint8_t address;
	union {
		uint8_t pca9685_reg[PCA9685_REGS];
		struct {
			union {
				uint8_t MODE1;
				struct {
					uint8_t M1_RESTART : 1;
					uint8_t M1_EXTCLK  : 1;
					uint8_t M1_AI      : 1;
					uint8_t M1_SLEEP   : 1;
					uint8_t M1_SUB1    : 1;
					uint8_t M1_SUB2    : 1;
					uint8_t M1_SUB3    : 1;
					uint8_t M1_ALLCALL : 1;
				};
			};
			union {
				uint8_t MODE2;
				struct {
					uint8_t M2_RESERVED : 3;
					uint8_t M2_INVRT    : 1;
					uint8_t M2_OCH      : 1;
					uint8_t M2_OUTDRV   : 1;
					uint8_t M2_OUTNE    : 2;
				};
			};
			uint8_t SUBADR1;
			uint8_t SUBADR2;
			uint8_t SUBADR3;
			uint8_t ALLCALLADR;
			pca9685_led_t led[PCA9685_PWMS];
		};
	};
} pca9685_t;

void pca9865_init(pca9685_t *pca, I2C_HandleTypeDef *hi2c, uint8_t address);
HAL_StatusTypeDef  pca9865_load(pca9685_t *pca);
HAL_StatusTypeDef  pca9865_pwm(pca9685_t *pca,  uint8_t num, uint16_t on, uint16_t off);
HAL_StatusTypeDef  pca9865_servo(pca9685_t *pca,  uint8_t num, uint16_t angle);

#endif // __PCA9685_H__
