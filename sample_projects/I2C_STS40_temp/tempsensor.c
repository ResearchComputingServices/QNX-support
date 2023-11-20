
/*
 * tempsensor.c
 *
 *  Created on: Jun. 13, 2023
 *      Author: rogerselzler
 */


#include <errno.h>
#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/iofunc.h>
#include <sys/dispatch.h>
#include <sys/resmgr.h>
#include <sys/iomsg.h>
#include <sys/iofunc.h>
#include <string.h>

#include <hw/i2c.h>

#include "sts40.h"
#include "errno.h"


int optv=0;
int optt=0;
int optf=0;

static resmgr_connect_funcs_t    connect_funcs;
static resmgr_io_funcs_t         io_funcs;
static iofunc_attr_t             attr;

sts40_t sts40;

struct send_recv
{
    i2c_send_t hdr;
    uint8_t buf[3];
} i2c_data;

uint16_t temperature_raw;

void options(int argc, char *argv[]);

int main( int argc, char *argv[]){
	options(argc, argv);

	sts40.addr = sts40_addr_46;
	sts40.temp.i.temp = 0;
	sts40.precision = sts40_precision_low;

	int fd = open("/dev/i2c1", O_RDWR);
	if (fd == -1){
		perror("Could not open /dev/i2c1");
		exit(ENOENT);
	}
	int ret;

	resmgr_attr_t        resmgr_attr;
	dispatch_t           *dpp;
	dispatch_context_t   *ctp;
	int                  id;


	if((dpp = dispatch_create()) == NULL) {
		fprintf(stderr,
				"%s: Unable to allocate dispatch handle.\n",
				argv[0]);
		return EXIT_FAILURE;
	}

	memset(&resmgr_attr, 0, sizeof resmgr_attr);
	resmgr_attr.nparts_max = 1;
	resmgr_attr.msg_max_size = 256;

	double temperature = 0;
	do {
		//TODO replace with i2c read.
		i2c_data.hdr.slave.addr = sts40.addr;
		i2c_data.hdr.slave.fmt = I2C_ADDRFMT_7BIT;
		i2c_data.hdr.len =1;
		i2c_data.hdr.stop = 1;
		ret = sts40_cmd_read_temp(&sts40, &i2c_data.buf[0]);

		if (ret != EOK){
			return ret;
		}

		ret = devctl(fd, DCMD_I2C_SEND, &i2c_data, sizeof (i2c_data), NULL);
		if (ret != EOK) {
			if (optv>0){
				printf("Error while writing command I2C: %d\n", ret);
			}
			continue;
		}


		i2c_data.hdr.len = 3;
		int trials;
		trials = 0;
		do {
			usleep(1000);

			ret = devctl(fd, DCMD_I2C_RECV, &i2c_data, sizeof (i2c_data), NULL);
			if (ret != EOK) {
				if (optv>0){
					printf("Error reading command from I2C %d, trials: %d.\n", ret, trials);
				}
			}
			else {
				printf("Reading data was ok %d, trials: %d.\n", ret, trials);
				break;
			}
			trials++;
		} while ((ret != EOK) & (trials < 100));

		if (ret != EOK) {
			printf("Error reading command from I2C.\n");
		} else {
			temperature_raw = i2c_data.buf[0]<<8 | i2c_data.buf[1];
			if (optf == 1)
			{
				temperature = sts40_temperature_fahrenheit(temperature_raw);
				printf("Temperature: %.3fF\n", temperature);
			} else {
				temperature = sts40_temperature_celcius(temperature_raw);
				printf("Temperature: %.3fC\n", temperature);
			}
		}
		usleep(optt*1000);
	} while (optt>0);
	return 0;
}


int
io_read (resmgr_context_t *ctp, io_read_t *msg, RESMGR_OCB_T *tocb)
{
	int status;
	if ((status = iofunc_read_verify(ctp, msg, &tocb->ocb, NULL)) != EOK)
	{
		if (optv>2) {
			fprintf(stderr, "Unauthorized user. Error %d\n", status);
		}
		return status;
	}

	return EOK;
}



void print_usage(){
	printf("tempsensor: \n");
	printf("\t-v\tVerbose\n");
	printf("\t-t\tinterval. If not set, do only one reading\n");
	printf("\t-f\tPrint temperature in Farenheit\n");
	printf("\t-p\tSet precision: low, medium, high\n");
	printf("\nexample:\n\t ./tempsensor -v -t 1000 -p medium\n");
	exit(EOK);
}

void options(int argc, char *argv[]){
	int opt;

	while( (opt = getopt(argc, argv, "hfvp:t:")) != -1){
		switch(opt) {
		case 'h':
			print_usage();
		case 'f':
			optf =1;
		case 'v':
					optv += 1;
					break;
		case 'p':
					if (strcmp(optarg, "low") == 0) {
						sts40.precision = sts40_precision_low;
					} else if (strcmp(optarg,"medium") == 0) {
						sts40.precision = sts40_precision_medium;
					}
					else if (strcmp(optarg,"high") == 0) {
						sts40.precision = sts40_precision_high;
					} else {
						perror("Invalid precision option.");
						exit(EINVAL);
					}

					if (optv>0){printf("Setting precision to %s\n", optarg);}

					break;
		case 't':
			optt = atoi(optarg);
			if (optv>0){printf("Setting interval to %d ms\n", optt);}
			break;
        default:
        	fprintf(stderr, "Unknown option %c\n", opt);
        	exit(EINVAL);
        	break;
		}
	}
	if (optv > 0){
		printf("Verbose level: %d\n", optv);
	}
}

