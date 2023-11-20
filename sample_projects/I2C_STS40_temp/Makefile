
#
#	Makefile for i2ctests project
#

DEBUG = -g
CC = qcc
LD = qcc

TARGET = -Vgcc_ntoaarch64le
#TARGET = -Vgcc_ntoarmv7le
#TARGET = -Vgcc_ntoaarch64le

CFLAGS += $(DEBUG) $(TARGET) -Wall 
#LDFLAGS+= $(DEBUG) $(TARGET) 

SRC = tempsensor.c sts40.c 
BINS = tempsensor 

#all:	$(SRC) $(BINS) 
all:
	$(CC) $(CFLAGS) -o $(BINS) $(SRC)

clean:
	rm -f *.o $(BINS)


#tempsensor.o: tempsensor.c sts40.h
