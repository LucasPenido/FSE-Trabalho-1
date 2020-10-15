#include "bcm2835.h"
#include "bcm2835Driver.h"

#define RESISTOR RPI_V2_GPIO_P1_16
#define VENTOINHA RPI_V2_GPIO_P1_18

void configura_pinos() {
    bcm2835_gpio_fsel(RESISTOR, BCM2835_GPIO_FSEL_OUTP);
    bcm2835_gpio_fsel(VENTOINHA, BCM2835_GPIO_FSEL_OUTP);
}

void bcm2835_ligarResistor(int ligar) {
    bcm2835_gpio_write(RESISTOR, ligar);
}

void bcm2835_ligarVentoinha(int ligar) {
    bcm2835_gpio_write(VENTOINHA, ligar);
}

void bcm2835_desliga() {
    bcm2835_close();
    exit(0);
}

void bcm2835_inicializa() {
    if (!bcm2835_init()) {
        exit(1);
    }

    configura_pinos();
}