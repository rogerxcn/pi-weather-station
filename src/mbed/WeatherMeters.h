/*
 * mbed library to use a Sparkfun Weather Meters
 * Copyright (c) 2011 Hiroshi Suga
 * Released under the MIT License: http://mbed.org/license/mit
 */

/** @file WeatherMeters.h
 * @brief mbed library to use a Sparkfun Weather Meters
 * Anemometer, Wind vane, Rain gauge
 * interface: digital, analog
 */

#ifndef WeatherMeters_H
#define WeatherMeters_H

#include "mbed.h"

/**
 * @brief parameter of Anemometer
 */
enum Weather_mode {
    Weather_manual = 0,
    Weather_auto = 1
};

/**
 * @brief WeatherMeters class
 */
class WeatherMeters : public Base {
public:
    WeatherMeters(PinName p_anemometer, PinName p_windvane, PinName p_raingauge, Weather_mode w_mode = Weather_auto);

    float get_windspeed();
    float get_windvane();
    float get_raingauge();
    void update();

protected:
    void int_anemometer ();
    void int_raingauge ();
    void int_timer ();

    InterruptIn int01, int02;
    AnalogIn ain01;
    Ticker ticker01;
    int windspeed, raingauge;
    float windvane;
    int mode;

private:

    int count_anemo, count_rain, time_anemo, time_rain;
    int buf_anemo[12], buf_rain[12];
};

#endif
