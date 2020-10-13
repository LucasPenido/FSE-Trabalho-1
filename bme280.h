#include <fcntl.h>
#include <linux/i2c-dev.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

#include "BME280_driver/bme280Driver.h"

#ifndef BME280_H_
#define BME280_H_

float bme280_requisitaTemperaturaExterna();

#endif /* BME280_H_ */