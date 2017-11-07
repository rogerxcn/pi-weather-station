// ECE 4180 Final Project - Weather Station

// Include statements
#include "mbed.h"
#include "utils.h"

// I/O declarations
RawSerial s(USBTX, USBRX);   // Raspberry Pi is connected using USB serial

// Global variables
SerialState s_state = RESET;        // Serial state
SerialCommand s_cmd = INVALID;      // Last valid serial command


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
            if (command_char == 'A') {
                s_cmd = ACK;
            } else if (command_char == 'n') {
                s_cmd = nACK;
            } else {
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
    pi.baud(9600);
    // Attach interrupt
    pi.attach(&s_recv, Serial::RxIrq);
    // Go to sleep zzZ
    while(1) {
        sleep();
    }
}
