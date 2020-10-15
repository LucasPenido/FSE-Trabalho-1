#include <stdio.h>
#include <stdlib.h>
#include <wiringPi.h>
#include <wiringPiI2C.h>

#define I2C_ADDR 0x27

#define LCD_CHR 1
#define LCD_CMD 0

#define LINE1 0x80
#define LINE2 0xC0

#define ENABLE 0b00000100

#define LCD_BACKLIGHT 0x08

#ifndef LCD_DRIVER_H_
#define LCD_DRIVER_H_

void lcd_init();
void ClrLcd(void);
void lcdLoc(int line);
void typeFloat(float myFloat);
void typeln(const char *s);
void lcd_byte(int bits, int mode);
void lcd_toggle_enable(int bits);

#endif /* LCD_DRIVER_H_ */