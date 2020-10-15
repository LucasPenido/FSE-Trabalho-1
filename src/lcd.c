#include "lcd.h"

#include "lcdDriver.h"

extern int fd;

void lcd_inicializa() {
    if (wiringPiSetup() == -1) exit(1);

    fd = wiringPiI2CSetup(I2C_ADDR);

    lcd_init();
}

void lcd_escreverTemperaturas(float temperaturaInterna, float temperaturaExterna,
                              float temperaturaReferencia) {
    // ClrLcd();
    lcdLoc(LINE1);
    typeln("TI:");
    typeFloat(temperaturaInterna);
    typeln("TE:");
    typeFloat(temperaturaExterna);
    lcdLoc(LINE2);
    typeln("TR:");
    typeFloat(temperaturaReferencia);
}