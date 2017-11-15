// ECE 4180 Final Project - Weather Station

// Include statements
#include "mbed.h"
#include "utils.h"

// I/O declarations
RawSerial s(USBTX, USBRX);   // Raspberry Pi is connected using USB serial

// Global variables
SerialState s_state = RESET;        // Serial state
SerialCommand s_cmd = INVALID;      // Last valid serial command

//Sensor data
volatile float pressure = 0;
volatile float humidity = 0;
volatile float temperature = 0;
volatile float wind_speed = 0;
volatile char wind_dir[2];

// Interrupt routine for active serial transactions
void s_recv() {
    // Command buffer
    char command_char = 0;
    // State transition
    switch (s_state) {
        case RESET:
            if (s.getc() == '!') s_state = EXCLAMATION_RECV;
            else s_state = RESET;
            break;

        case EXCLAMATION_RECV:
            command_char = s.getc();
            if (command_char == '1') {
                s_cmd = GET_DATA;
                printf("!t PRES %.2f HUM %.2f TEMP %.2f WIND %.2f %s\r\n", pressure, humidity, temperature, wind_speed, wind_dir);
            } else if (command_char == 'A') {
                s_cmd = ACK;
            } else if (command_char == 'N') {
                s_cmd = nACK;
            } else if (command_char == 't') {
                s_cmd = TEST;
                printf("!t PRES 1017 HUM 43.6 TEMP 78.2 WIND 3.2 NE\r\n");  // test data
            }else {
                s_cmd = INVALID;
            }
            s_state = RESET;
            break;

        default:
            s.getc();
            s_state = RESET;
    }

}

int main() {
    // Set baud rate
    s.baud(9600);
    // Attach interrupt
    s.attach(&s_recv, Serial::RxIrq);
    // Go to sleep zzZ
    while(1) {
        sleep();
    }
}
