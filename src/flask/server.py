import json, time, os
from flask import Flask, render_template
import netifaces as ni

ip = ni.ifaddresses('wlan0')[ni.AF_INET][0]['addr']
cmd = '/home/pi/Documents/4180-proj/main.o'

print("Testing MBED connectivity...")
res = os.system(cmd)
time.sleep(1)
while (res == -1):
    print("Test failed, reconnecting...")
    res = os.system(cmd)
    time.sleep(1)
print("Connectivity test passed.")

app = Flask(__name__)
temperature = 0
humidity = 0
pressure = 0
wind_speed = 0
rain_size = 0
wind_dir = 'N/A'

@app.route('/read')
def read():
    res = os.system(cmd)
    time.sleep(1)
    while (res == -1):
        res = os.system(cmd)
        time.sleep(1)
    f = open('/home/pi/Documents/4180-proj/weather.data', 'r');
    global pressure
    pressure = f.readline()
    global humidity
    humidity = f.readline()
    global temperature
    temperature = f.readline()
    global wind_speed
    wind_speed = f.readline()
    global wind_dir
    wind_dir = f.readline()
    global rain_size
    rain_size = f.readline()
    return "OK"

@app.route('/getPressure')
def get_pressure():
    global pressure
    return str(pressure)

@app.route('/getHumidity')
def get_humidity():
    global humidity
    return str(humidity)

@app.route('/getTemperature')
def get_temperature():
    global temperature
    return str(temperature)

@app.route('/getWindSpeed')
def get_windspeed():
    global wind_speed
    return str(wind_speed)

@app.route('/getWindDir')
def get_winddir():
    global wind_dir
    return str(wind_dir)

@app.route('/getRain')
def get_rain():
    global rain_size
    return str(rain_size)

# Main render
@app.route('/')
def index():
    f = open('/home/pi/Documents/4180-proj/weather.data', 'r');
    global pressure
    pressure = f.readline()
    global humidity
    humidity = f.readline()
    global temperature
    temperature = f.readline()
    global wind_speed
    wind_speed = f.readline()
    global wind_dir
    wind_dir = f.readline()
    global rain_size
    rain_size = f.readline()
    f.close()
    return  render_template('weather.html',
                            html_temperature=temperature,
                            html_humidity=humidity,
                            html_pressure=pressure,
                            html_windspeed=wind_speed,
                            html_winddir=wind_dir,
                            html_rain=rain_size,
                            html_server_ip=ip)

if __name__ == '__main__':
  app.run(debug=True, host='0.0.0.0')
