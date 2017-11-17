/*
 * mbed library to use a Sparkfun Weather Meters
 * Copyright (c) 2011 Hiroshi Suga
 * Released under the MIT License: http://mbed.org/license/mit
 */

/** @file WeatherMeters.cpp
 * @brief mbed library to use a Sparkfun Weather Meters
 * Anemometer, Wind vane, Rain gauge
 * interface: digital, analog
 */

#include "mbed.h"
#include "WeatherMeters.h"

#define WEATHER_VREF 3.3

const float tbl_windvane[16][2] = {
  {0.0, 33000}, {22.5, 6570}, {45.0, 8200}, {67.5, 891},
  {90.0, 1000}, {112.5, 688}, {135.0, 2200}, {157.5, 1410},
  {180.0, 3900}, {202.5, 3140}, {225.0, 16000}, {247.5, 14120},
  {270.0, 120000}, {292.5, 42120}, {315.0, 64900}, {337.5, 21880}
};

/**
 * @brief Initializes interface
 * @param p_anemometer port of Anemometer
 * @param p_windvane port of Wind vane (analog)
 * @param p_raingauge parameter of Rain gauge
 * @param w_mode parameter of Anemometer
 */
WeatherMeters::WeatherMeters (PinName p_anemometer, PinName p_windvane, PinName p_raingauge, Weather_mode w_mode) : int01(p_anemometer), int02(p_raingauge), ain01(p_windvane) {
    int i;

    windspeed = 0; windvane = 0; raingauge = 0;
    count_anemo = 0; count_rain = 0; time_anemo = 0; time_rain = 0;
    for (i = 0; i < 12; i ++) {
        buf_anemo[i] = 0;
        buf_rain[i] = 0;
    }

    mode = w_mode;
    int01.fall(this, &WeatherMeters::int_anemometer);
    int02.fall(this, &WeatherMeters::int_raingauge);
    ticker01.attach(this, &WeatherMeters::int_timer, 0.25);
}

/**
 * @brief Get wind speed
 * @return wind speed (m/s)
 * mode=Weather_auto: average from 12sample/2sec
 * mode=Weather_manual: average from last measurement
 */
float WeatherMeters::get_windspeed() {
    float s;

    if (mode == Weather_auto) {
        s = (float)windspeed * 0.667 / 3.0; // 0.667m/s/Hz (/3s)
        windspeed = 0;
    } else {
        s = (float)count_anemo * 0.667 / ((float)time_anemo / 4.0); // 0.667m/s/Hz
        time_anemo = 0;
        count_anemo = 0;
    }
    return s;
}

/**
 * @brief Get wind vane
 * @return wind vane (`)
 * mode=Weather_auto: wind speed at the highest
 * mode=Weather_manual: current wind vane
 */
float WeatherMeters::get_windvane() {
    int i;
    float v;

    if (windspeed == 0) windvane = ain01;
    v = windvane * WEATHER_VREF; // V
    v = v / ((WEATHER_VREF - v) / 10000.0); // ohm
    for (i = 0; i < 16; i ++) {
        if (v > tbl_windvane[i][1] * 0.9 && v < tbl_windvane[i][1] * 1.1) {
            return tbl_windvane[i][0];
        }
    }
    return 0;
}

/**
 * @brief Get rain gauge
 * @return wind speed (mm/h)
 * average from 12sample/60min (update 5min)
 */
float WeatherMeters::get_raingauge() {
    return (float)raingauge * 0.2794; // 0.2794mm/Hz
}

void WeatherMeters::int_anemometer () {
    count_anemo ++;
}

void WeatherMeters::int_raingauge () {
    count_rain ++;
}

void WeatherMeters::int_timer () {
    int i, n;

    if (mode == Weather_auto) {
        n = 0;
        for (i = 11; i > 0; i --) {
            buf_anemo[i] = buf_anemo[i - 1];
            n = n + buf_anemo[i];
        }
        buf_anemo[0] = count_anemo;
        n = n + buf_anemo[0];
        count_anemo = 0;
        if (n > windspeed) {
            windspeed = n;
            windvane = ain01;
        }
    } else {
        time_anemo ++;
    }

    time_rain ++;
    if (time_rain >= 4 * 300) {
        n = 0;
        for (i = 11; i > 0; i --) {
            buf_rain[i] = buf_rain[i - 1];
            n = n + buf_rain[i];
        }
        buf_rain[0] = count_rain;
        n = n + buf_rain[0];
        count_rain = 0;
        time_rain = 0;
        raingauge = n;
    }
}
