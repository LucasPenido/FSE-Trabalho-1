#include <curses.h>
#include <pthread.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>

#include "arduino.h"
#include "bcm2835.h"
#include "bme280.h"
#include "lcd.h"

volatile int lerTemperaturaPotenciometro = 0, tempoDoisSegundos = 0;
volatile float temperaturaDefinida = 0, temperaturaInterna = 0, temperaturaExterna = 0, histerese = 0;

pthread_mutex_t gravarArquivoMutex, imprimirLCDMutex, lerTemperaturasArduinoMutex, lerTemperaturaBme280Mutex, gerenciarTemperaturaMutex;
pthread_cond_t gravarArquivoCond, imprimirLCDCond, lerTemperaturasArduinoCond, lerTemperaturaBme280Cond, gerenciarTemperaturaCond;

void fechaConexoes() {
    bcm2835_ligarResistor(1);
    bcm2835_ligarVentoinha(1);
    bcm2835_desliga();
}

void trata_interrupcao() {
    fechaConexoes();
    exit(0);
}

void trata_alarme() {
    ualarm(500000, 0);

    pthread_cond_signal(&lerTemperaturasArduinoCond);
    pthread_cond_signal(&gerenciarTemperaturaCond);
    pthread_cond_signal(&lerTemperaturaBme280Cond);

    if (tempoDoisSegundos == 2000) {
        pthread_cond_signal(&gravarArquivoCond);
        pthread_cond_signal(&imprimirLCDCond);
        tempoDoisSegundos = 0;
    }
    tempoDoisSegundos += 500;
}

void definirTemperaturaUsuario() {
    float temperaturaDefinidaUsuario;
    system("clear");
    printf("0 - Para voltar ao menu anterior\n");
    printf("Digite a temperatura a ser definida: ");
    scanf("%f", &temperaturaDefinidaUsuario);
    system("clear");

    if (temperaturaDefinidaUsuario > 0) {
        temperaturaDefinida = temperaturaDefinidaUsuario;
        lerTemperaturaPotenciometro = 0;
    }
}

void definirHisterese() {
    float histereseUsuario = 0;
    system("clear");
    printf("0 - Para voltar ao menu anterior\n");
    printf("Digite uma histerese: ");
    scanf("%f", &histereseUsuario);
    system("clear");

    if (histereseUsuario > 0) {
        histerese = histereseUsuario;
    }
}

int menu() {
    int op;
    system("clear");
    if (lerTemperaturaPotenciometro) {
        printf("Lendo temperatura do potenciômetro\n\n");
    }
    printf("1 - Definir uma temperatura\n");
    printf("2 - Ler a temperatura do potenciômetro\n");
    printf("3 - Definir Histerese\n");
    printf("0 - Sair do programa\n");
    scanf("%d", &op);

    return op;
}

void *defineTemperatura() {
    int op;
    while (1) {
        if (lerTemperaturaPotenciometro) {
        }
        op = menu();
        switch (op) {
            case 1:
                definirTemperaturaUsuario();
                break;
            case 2:
                lerTemperaturaPotenciometro = 1;
                break;
            case 3:
                definirHisterese();
                break;
            case 0:
                return NULL;
            default:
                break;
        }
    }
}

void *gerenciaTemperatura() {
    float temperaturaMinima, temperaturaMaxima;
    int resistorLigado = 0, ventoinhaLigada = 0;

    while (1) {
        pthread_mutex_lock(&gerenciarTemperaturaMutex);
        pthread_cond_wait(&gerenciarTemperaturaCond, &gerenciarTemperaturaMutex);
        if (temperaturaDefinida > 0) {
            temperaturaMinima = temperaturaDefinida - histerese;
            temperaturaMaxima = temperaturaDefinida + histerese;
            if (temperaturaInterna < temperaturaMinima) {
                resistorLigado = 1;
                ventoinhaLigada = 0;
                bcm2835_ligarVentoinha(1);
                bcm2835_ligarResistor(0);
            } else if (temperaturaInterna > temperaturaMaxima) {
                resistorLigado = 0;
                ventoinhaLigada = 1;
                bcm2835_ligarResistor(1);
                bcm2835_ligarVentoinha(0);
            } else if ((temperaturaInterna > temperaturaDefinida && resistorLigado) ||
                       (temperaturaInterna < temperaturaDefinida && ventoinhaLigada)) {
                resistorLigado = 0;
                ventoinhaLigada = 0;
                bcm2835_ligarResistor(1);
                bcm2835_ligarVentoinha(1);
            }
        }
        pthread_mutex_unlock(&gerenciarTemperaturaMutex);
    }
}

void *lerTemperaturasArduino() {
    while (1) {
        pthread_mutex_lock(&lerTemperaturasArduinoMutex);
        pthread_cond_wait(&lerTemperaturasArduinoCond, &lerTemperaturasArduinoMutex);
        temperaturaInterna = arduino_requisitaTemperaturaInterna();
        if (lerTemperaturaPotenciometro) {
            temperaturaDefinida = arduino_requisitaTemperaturaPotenciometro();
        }
        pthread_mutex_unlock(&lerTemperaturasArduinoMutex);
    }
}

void *lerTemperaturaBme280() {
    float temp;
    while (1) {
        pthread_mutex_lock(&lerTemperaturaBme280Mutex);
        pthread_cond_wait(&lerTemperaturaBme280Cond, &lerTemperaturaBme280Mutex);
        if ((temp = bme280_requisitaTemperaturaExterna()) > 0) {
            temperaturaExterna = temp;
        }
        pthread_mutex_unlock(&lerTemperaturaBme280Mutex);
    }
}

void defineDataHoraAtual(char **dataHora) {
    char cur_time[128];
    *dataHora = malloc(128);

    time_t t;
    struct tm *ptm;

    t = time(NULL);
    ptm = localtime(&t);

    strftime(cur_time, 128, "%d/%m/%Y %H:%M:%S", ptm);

    strcpy(*dataHora, cur_time);
}

void *gravaTemperaturas() {
    char *dataHora;

    FILE *file = fopen("temp.csv", "w");
    fprintf(file, "DataHora, TI, TE, TR\n");
    fclose(file);

    while (1) {
        pthread_mutex_lock(&gravarArquivoMutex);
        pthread_cond_wait(&gravarArquivoCond, &gravarArquivoMutex);
        file = fopen("temp.csv", "a");
        defineDataHoraAtual(&dataHora);
        fprintf(file, "%s,%0.2lf,%0.2lf,%0.2lf\r\n", dataHora, temperaturaInterna, temperaturaExterna, temperaturaDefinida);
        fclose(file);
        pthread_mutex_unlock(&gravarArquivoMutex);
    }
}

void *imprimirLCD() {
    while (1) {
        pthread_mutex_lock(&imprimirLCDMutex);
        pthread_cond_wait(&imprimirLCDCond, &imprimirLCDMutex);
        lcd_escreverTemperaturas(temperaturaInterna, temperaturaExterna, temperaturaDefinida);
        pthread_mutex_unlock(&imprimirLCDMutex);
    }
}

int main(int argc, char const *argv[]) {
    pthread_t thread_menu_temperatura, thread_arduino,
        thread_bme280, thread_gravar_temperaturas,
        thread_gerenciar_temperatura, thread_lcd;

    pthread_mutex_init(&gravarArquivoMutex, NULL);
    pthread_mutex_init(&imprimirLCDMutex, NULL);
    pthread_mutex_init(&lerTemperaturasArduinoMutex, NULL);

    pthread_cond_init(&gravarArquivoCond, NULL);
    pthread_cond_init(&imprimirLCDCond, NULL);
    pthread_cond_init(&lerTemperaturasArduinoCond, NULL);

    bcm2835_inicializa();
    lcd_inicializa();
    bme280_inicializa();

    signal(SIGINT, trata_interrupcao);
    signal(SIGALRM, trata_alarme);

    ualarm(500000, 0);

    pthread_create(&thread_menu_temperatura, NULL, &defineTemperatura, NULL);
    pthread_create(&thread_arduino, NULL, &lerTemperaturasArduino, NULL);
    pthread_create(&thread_bme280, NULL, &lerTemperaturaBme280, NULL);
    pthread_create(&thread_gravar_temperaturas, NULL, &gravaTemperaturas, NULL);
    pthread_create(&thread_gerenciar_temperatura, NULL, &gerenciaTemperatura, NULL);
    pthread_create(&thread_lcd, NULL, &imprimirLCD, NULL);

    pthread_join(thread_menu_temperatura, NULL);

    fechaConexoes();

    return 0;
}
