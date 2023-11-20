/*
 * sts40.h
 *
 *  Created on: Jun. 13, 2023
 *      Author: rogerselzler
 */

#ifndef STS40_H_
#define STS40_H_

#include <stdint.h>

#define STS40_ADDR_44 0x44
#define STS40_ADDR_45 0x45
#define STS40_ADDR_46 0x46

#define STS40_REG_SOFTRESET 		0x94
#define STS40_REG_PRECISION_HIGH 	0xFD
#define STS40_REG_PRECISION_MEDIUM 	0xF6
#define STS40_REG_PRECISION_LOW 	0xE0
#define STS40_REG_SERIAL 			0x89

typedef enum {
	sts40_precision_low,
	sts40_precision_medium,
	sts40_precision_high,
} sts40_precision_t;


typedef enum {
	sts40_addr_44 = STS40_ADDR_44,
	sts40_addr_45 = STS40_ADDR_45,
	sts40_addr_46 = STS40_ADDR_46,
} sts40_addr_t;


typedef struct sts40_temp {
	union {
		uint8_t buf[2];
		uint16_t temp;
	}i;
}sts40_temp_t;

typedef struct {
	sts40_addr_t addr;
	sts40_precision_t precision;
	sts40_temp_t temp;
} sts40_t;


int sts40_cmd_softreset(sts40_t *dev, uint8_t *buf);

int sts40_cmd_read_temp(sts40_t *dev, uint8_t *buf);

double  sts40_temperature_celcius(uint16_t val);

double  sts40_temperature_fahrenheit(uint16_t val);

#endif /* STS40_H_ */
