/*
 * sts40.c
 *
 *  Created on: Jun. 13, 2023
 *      Author: rogerselzler
 */


#include <stdio.h>
#include "sts40.h"
#include <errno.h>

int sts40_cmd_softreset(sts40_t *dev, uint8_t *buf){
	// send command
	if (buf == NULL) {
		return EFAULT;
	}
	*buf = dev->addr;
	*(buf+1) = STS40_REG_SOFTRESET;
	return EOK;
}

int sts40_cmd_read_temp(sts40_t *dev, uint8_t *buf){
	switch(dev->precision){
	case sts40_precision_low:
		*(buf) = STS40_REG_PRECISION_LOW;
		break;
	case sts40_precision_medium:
		*(buf) =STS40_REG_PRECISION_MEDIUM;
		break;
	case sts40_precision_high:
		*(buf) = STS40_REG_PRECISION_HIGH;
		break;
	default:
		return EINVAL;

	}
	return EOK;
}

double  sts40_temperature_celcius(uint16_t val){
	return -45 + 175 * (double) val / UINT16_MAX;
}

double  sts40_temperature_fahrenheit(uint16_t val) {
	return -49 + 315 * (double) val / UINT16_MAX;
}
