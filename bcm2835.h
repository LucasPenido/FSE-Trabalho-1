#include <stdio.h>
#include <stdlib.h>
#include <bcm2835.h>

#include"bcm2835Driver.h"

#ifndef ARDUINO_H
#define ARDUINO_H

int bcm2835_inicializa();
void bcm2835_desliga();

#endif /* ARDUINO_H */
