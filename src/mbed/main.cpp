// ECE 4180 Final Project - Weather Station

// Include statements
#include "mbed.h"
#include "rtos.h"
#include "utils.h"
#include "WeatherMeters.h"


// I/O declarations
RawSerial s(USBTX, USBRX);   // Raspberry Pi is connected using USB serial
WeatherMeters wm(p21, p15, p22, Weather_auto);/*Weather meter init*/
//SHTx::SHT15 sensor(p28, p27);  /*Temp & Humidity Init*/
//BMP085 bmp085(p9, p10);

// Global variables
SerialState s_state = RESET;        // Serial state
SerialCommand s_cmd = INVALID;      // Last valid serial command

//Sensor data
volatile float pressure = 0;
volatile float humidity = 0;
volatile float temperature = 0;
volatile float wind_speed = 0;
volatile float raingauge = 0;
volatile char wind_dir[2];
volatile float wind_d = 0;

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
                s.printf("!t PRES %.2f HUM %.2f TEMP %.2f WIND %.2f %s RAIN %0.2f\r\n", pressure, humidity, temperature, wind_speed, wind_dir, raingauge);
            } else if (command_char == 'A') {
                s_cmd = ACK;
            } else if (command_char == 'N') {
                s_cmd = nACK;
            } else if (command_char == 't') {
                s_cmd = TEST;
                s.printf("!t PRES 1017 HUM 43.6 TEMP 78.2 WIND 3.2 NE\r\n");  // test data
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

void update_weather_station_data() {
    while(1){
    /* Get weather readings from sensors*/
    wind_speed = wm.get_windspeed();
    wind_d = wm.get_windvane();
    raingauge = wm.get_raingauge();
//   temperature = sensor.getTemperature();
 //   humidity = sensor.getHumidity();
//    bmp085.update();
//    pressure = bmp085.get_pressure();
   //temeperature = bmp085.get_temperature();
   /* Transfer from degree to char direction*/
   //s.printf("%.2f\n", wind_d );
    if (wind_d<22.5){
        wind_dir[0] = 'N';
        wind_dir[1] = ' ';
        }
    else if (wind_d<67.5){
        wind_dir[0] = 'N';
        wind_dir[1] = 'E';}
    else if (wind_d<112.5){
        wind_dir[0] = 'E';
        wind_dir[1] = ' ';}
    else if (wind_d<157.5){
        wind_dir[0] = 'S';
        wind_dir[1] = 'E';}
    else if (wind_d<202.5){
        wind_dir[0] = 'S';
        wind_dir[1] = ' ';}
    else if (wind_d<247.5){
        wind_dir[0] = 'S';
        wind_dir[1] = 'W';}
    else if (wind_d<292.5){
        wind_dir[0] = 'W';
        wind_dir[1] = ' ';}
    else if (wind_d<337.5){
        wind_dir[0] = 'N';
        wind_dir[1] = 'W';}
    else {
        wind_dir[0] = 'N';
        wind_dir[1] = ' ';}
    //Thread::wait(500);
    }
}

int main() {
    // Initialize thread instance
    Thread t1;
    t1.start(update_weather_station_data);
    // Set baud rate
    s.baud(9600);
    // Attach interrupt
    s.attach(&s_recv, Serial::RxIrq);
    // Go to sleep zzZ
    while(1) {
        sleep();
    }
}
