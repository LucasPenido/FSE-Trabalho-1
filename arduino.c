#include "arduino.h"

void escreverUART(int uart0_filestream, char *mensagem) {
    unsigned char tx_buffer[20];
    unsigned char *p_tx_buffer;

    p_tx_buffer = &tx_buffer[0];
    for (int i = 0; i < strlen(mensagem); i++) {
        *p_tx_buffer++ = mensagem[i];
    }

    if (uart0_filestream != -1) {
        int count = write(uart0_filestream, &tx_buffer[0], (p_tx_buffer - &tx_buffer[0]));
        if (count < 0) {
            printf("UART TX error\n");
        }
    }
}

float lerUART(int uart0_filestream) {
    float rx_buffer[4];
    //----- CHECK FOR ANY RX BYTES -----
    if (uart0_filestream != -1) {
        int rx_length = read(uart0_filestream, (void *)rx_buffer, 4);
        if (rx_length < 0) {
            printf("Erro na leitura.\n");
        } else if (rx_length == 0) {
            printf("Nenhum dado disponível.\n");
        }
    }
    return *rx_buffer;
}

int openUART() {
    int uart0_filestream = -1;

    uart0_filestream = open("/dev/serial0", O_RDWR | O_NOCTTY | O_NDELAY);  //Open in non blocking read/write mode
    if (uart0_filestream == -1) {
        printf("Erro - Não foi possível iniciar a UART.\n");
    }

    struct termios options;
    tcgetattr(uart0_filestream, &options);
    options.c_cflag = B115200 | CS8 | CLOCAL | CREAD;  //<Set baud rate
    options.c_iflag = IGNPAR;
    options.c_oflag = 0;
    options.c_lflag = 0;
    tcflush(uart0_filestream, TCIFLUSH);
    tcsetattr(uart0_filestream, TCSANOW, &options);

    return uart0_filestream;
}

float arduino_requisitaTemperaturaInterna() {
    int uart0_filestream;
    float temperatura;

    uart0_filestream = openUART();
    char mensagem[] = {161, 3, 1, 4, 3};
    escreverUART(uart0_filestream, mensagem);
    usleep(100000);
    temperatura = lerUART(uart0_filestream);

    close(uart0_filestream);

    return temperatura;
}

float arduino_requisitaTemperaturaPotenciometro() {
    int uart0_filestream;
    float temperatura;

    uart0_filestream = openUART();
    char mensagem[] = {162, 3, 1, 4, 3};
    escreverUART(uart0_filestream, mensagem);
    usleep(100000);
    temperatura = lerUART(uart0_filestream);
    close(uart0_filestream);

    return temperatura;
}
