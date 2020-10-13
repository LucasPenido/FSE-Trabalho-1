#include <curses.h>
#include <pthread.h>
#include <signal.h>
#include <stdio.h>

#include "arduino.c"
#include "bcm2835.c"
#include "bme280.c"

volatile int lerTemperaturaPotenciometro = 0, histerese = 0;
volatile float temperaturaDefinida = 0, temperaturaInterna = 0, temperaturaExterna = 0;
int tempoGerenciar = 0, tempoGravarArquivo = 0;
pthread_mutex_t lock;

void fechaConexoes() {
    bcm2835_ligarResistor(0);
    bcm2835_ligarVentoinha(0);
    bcm2835_desliga();
}

void trata_interrupcao() {
    fechaConexoes();
    exit(0);
}

void trata_alarme() {
    ualarm(100000, 0);
    if (tempoGerenciar == 500) {
        tempoGerenciar = 0;
    }
    if (tempoGravarArquivo == 2000) {
        tempoGravarArquivo = 0;
    }
    tempoGerenciar += 100;
    tempoGravarArquivo += 100;
}

void definirTemperaturaUsuario() {
    float temperaturaDefinidaUsuario = 0;
    clear();
    printf("0 - Para voltar ao menu anterior\n");
    printf("Digite a temperatura a ser definida: ");
    scanf("%f", &temperaturaDefinidaUsuario);

    if (temperaturaDefinidaUsuario > 0) {
        temperaturaDefinida = temperaturaDefinidaUsuario;
    }
}

void definirHisterese() {
    int histereseUsuario = 0;
    clear();
    printf("0 - Para voltar ao menu anterior\n");
    printf("Digite uma histerese: ");
    scanf("%d", &histereseUsuario);

    if (histereseUsuario > 0) {
        histerese = histereseUsuario;
    }
}

int menu(WINDOW *menu) {
    int op;

    // pthread_mutex_lock(&lock);
    // mvwprintw(menu, 0, 0, "1 - Definir uma temperatura");
    // mvwprintw(menu, 1, 0, "2 - Ler a temperatura do potenciômetro");
    // mvwprintw(menu, 2, 0, "0 - Sair do programa");
    // wrefresh(menu);
    // pthread_mutex_unlock(&lock);
    // op = wgetch(menu);

    printf("1 - Definir uma temperatura\n");
    printf("2 - Ler a temperatura do potenciômetro\n");
    printf("3 - Definir Histerese\n");
    printf("0 - Sair do programa\n");
    scanf("%d", &op);

    return op;
}

void *defineTemperatura() {
    int op;
    WINDOW *menuWin;
    menuWin = newwin(4, 40, 0, 0);
    // cbreak();

    while (1) {
        op = menu(menuWin);
        switch (op) {
            case 1:
                lerTemperaturaPotenciometro = 0;
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
    float temperaturaMinima;
    float temperaturaMaxima;
    int podeGerenciar = 0;

    while (1) {
        if (tempoGerenciar == 500) {
            podeGerenciar = 1;
        }

        if (podeGerenciar == 1) {
            temperaturaMinima = temperaturaDefinida - histerese;
            temperaturaMaxima = temperaturaDefinida + histerese;
            if (temperaturaInterna < temperaturaMinima) {
                bcm2835_ligarVentoinha(0);
                bcm2835_ligarResistor(1);
            } else if (temperaturaInterna > temperaturaMaxima) {
                bcm2835_ligarResistor(0);
                bcm2835_ligarVentoinha(1);
            } else if (temperaturaInterna > temperaturaMinima && temperaturaInterna < temperaturaMaxima) {
                bcm2835_ligarResistor(0);
                bcm2835_ligarVentoinha(0);
            }

            podeGerenciar = 0;
        }
    }
}

void *lerTemperaturaInterna() {
    while (1) {
        temperaturaInterna = arduino_requisitaTemperaturaInterna();
        if (lerTemperaturaPotenciometro) {
            temperaturaDefinida = arduino_requisitaTemperaturaPotenciometro();
        }
    }
}

void *lerTemperaturaExterna() {
    while (1) {
        temperaturaExterna = bme280_requisitaTemperaturaExterna();
    }
}

void *menuTemperaturas() {
    // WINDOW *menuTemp;
    // menuTemp = newwin(2, 40, 7, 0);

    // while (1) {
    //     pthread_mutex_lock(&lock);
    //     mvwprintw(menuTemp, 0, 0, "Temperatura Externa: %0.2lf", temperaturaExterna);
    //     mvwprintw(menuTemp, 1, 0, "Temperatura Interna: %0.2lf", temperaturaInterna);
    //     wrefresh(menuTemp);
    //     pthread_mutex_unlock(&lock);
    // }

    int podeGravarArquivo = 0;
    FILE *file = fopen("temp.csv", "w");
    fprintf(file, "Interna,Externa, Pot, Usuario\n");
    fclose(file);

    while (1) {
        if (tempoGravarArquivo == 2000) {
            podeGravarArquivo = 1;
        }
        if (podeGravarArquivo == 1) {
            FILE *file = fopen("temp.csv", "a");

            sleep(1);
            fprintf(file, "%0.2lf,%0.2lf,%0.2lf,%d\r\n", temperaturaInterna, temperaturaExterna, temperaturaDefinida, histerese);
            fclose(file);

            podeGravarArquivo = 0;
        }
    }
}

int main(int argc, char const *argv[]) {
    pthread_t thread_id;
    pthread_t thread_id2;
    pthread_t thread_id3;
    pthread_t thread_id4;
    pthread_t thread_id5;

    // pthread_mutex_init(&lock, NULL);

    // initscr();
    // curs_set(0);

    signal(SIGINT, trata_interrupcao);
    signal(SIGALRM, trata_alarme);

    ualarm(100000, 0);

    pthread_create(&thread_id, NULL, &defineTemperatura, NULL);
    pthread_create(&thread_id2, NULL, &lerTemperaturaInterna, NULL);
    pthread_create(&thread_id3, NULL, &lerTemperaturaExterna, NULL);
    pthread_create(&thread_id4, NULL, &menuTemperaturas, NULL);
    pthread_create(&thread_id5, NULL, &gerenciaTemperatura, NULL);

    pthread_join(thread_id, NULL);

    // endwin();

    fechaConexoes();

    return 0;
}
