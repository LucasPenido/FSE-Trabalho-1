#include <stdio.h>
#include <string.h>
#include <unistd.h>  //Used for UART
#include <fcntl.h>   //Used for UART
#include <termios.h> //Used for UART

#ifndef ARDUINO_H_
#define ARDUINO_H_

float arduino_requisitaTemperaturaInterna();

float arduino_requisitaTemperaturaPotenciometro();

#endif /* ARDUINO_H_ */
