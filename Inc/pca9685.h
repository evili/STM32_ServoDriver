#ifndef __PCA9685_H__
#define __PCA9685_H__

#include <stdint.h>
#include "stm32f7xx_hal.h"

#define PCA9685_DEFAULT_ADDRESS 0x80
#define PCA9685_REGS 70
#define PCA9685_LAST_REGS 6
#define PCA9685_PWMS 16
#define PCA9685_FREQ 25000000
#define PCA9685_SERVO_FREQ 50
#define PCA9685_SERVO_PRESCALE  121u // == ((PCA9685_SERVO_FREQ) /((1ul<<12)* (PCA9685_SERVO_FREQ))-1)
#define PCA9685_PWM_PRECISION (12)
#define PCA9685_PWM_MAX       (1<<PCA9685_PWM_PRECISION) // This is 4096

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

#define PCA9685_SUBADR1 0x02
#define PCA9685_SUBADR2 0x03
#define PCA9685_SUBADR3 0x04
#define PCA9685_ALLCALL 0x05

#define PCA9685_ALL_LED_ON_L    0xFA
#define PCA9685_ALL_LED_ON_H    0xFB
#define PCA9685_ALL_LED_OFF_L   0xFC
#define PCA9685_ALL_LED_OFF_H   0xFD

#define PCA9685_PRE_SCALE 0xFE
#define PCA9685_TEST_MODE 0xFF


#define PCA9685_LED_0_ON_L  0x06u
#define PCA9685_LED_FULL_ON  0x10u
#define PCA9685_LED_FULL_OFF 0x10u

// Initial values after reset
#define PCA9685_MODE1_INITIAL_VALUE 0x11u
#define PCA9685_MODE2_INITIAL_VALUE 0x04u


#define PCA9685_MODE1_RESTART_WAIT 5

#define PCA9685_OFF_MIN 150
#define PCA9685_OFF_MAX 600

typedef struct pca9685_led_s {
	union {
		uint8_t LEDONOFF[4];
		struct {
			uint8_t LED_ON_L;
			uint8_t LED_ON_H;
			uint8_t LED_OFF_L;
			uint8_t LED_OFF_H;
		};
	};
} pca9685_led_t;

typedef struct pca9685_s {
	I2C_HandleTypeDef *hi2c;
	uint8_t address;
	union {
		volatile uint8_t pca9685_reg[PCA9685_REGS];
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
	pca9685_led_t ALL_LED;
	uint8_t PRE_SCALE;
	uint8_t TEST_MODE;
} pca9685_t;

HAL_StatusTypeDef pca9865_init(pca9685_t *pca, I2C_HandleTypeDef *hi2c, uint8_t address);
HAL_StatusTypeDef pca9865_restart(pca9685_t *pca);
HAL_StatusTypeDef pca9865_set_prescale(pca9685_t *pca, uint8_t prescale);
HAL_StatusTypeDef pca9865_load(pca9685_t *pca);
HAL_StatusTypeDef pca9865_pwm(pca9685_t *pca,  uint8_t num, uint16_t on, uint16_t off);
HAL_StatusTypeDef pca9865_servo(pca9685_t *pca,  uint8_t num, double angle);

#endif // __PCA9685_H__
