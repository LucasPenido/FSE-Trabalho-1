all: clean atividade2    

atividade2:
	gcc -Wall ./BME280_driver/bme280Driver.c bme280.c arduino.c lcd.c lcdDriver.c bcm2835.c trabalho1.c -I ./BME280_driver/ -o trabalho1 -lncurses -lpthread -lbcm2835 -lwiringPi

clean:
	-rm -f trabalho1