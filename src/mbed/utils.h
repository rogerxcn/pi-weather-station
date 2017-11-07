// Utility definitions
// @file utils.h

#ifndef __UTILS_H
#define __UTILS_H

// Serial command states definition
typedef enum SerialState {
    RESET = 0,
    EXCLAMATION_RECV,
    NUM_SERIAL_STATES,
} SerialState;

// Serial commands definition
typedef enum SerialCommand {
    INVALID = 0,
    ACK,
    nACK,
    TEST,
} SerialCommand;

#endif
