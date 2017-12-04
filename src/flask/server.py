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
    return "OK"

@app.route('/getJSON')
def get_JSON():
    data = {
        'temperature'   : temperature,
        'humidity'      : humidity,
        'pressure'      : pressure,
        'wind_speed'    : wind_speed,
        'wind_dir'      : wind_dir,
        'rain_size'     : rain_size,
    }
    return json.dumps(data)

@app.route('/getPressure')
def get_pressure():
    return str(pressure)

@app.route('/getHumidity')
def get_humidity():
    return str(humidity)

@app.route('/getTemperature')
def get_temperature():
    return str(pressure)

@app.route('/getWindSpeed')
def get_windspeed():
    return str(wind_speed)

@app.route('/getWindDir')
def get_winddir():
    return str(wind_dir)

# Main render
@app.route('/')
def index():
    f = open('/home/pi/Documents/4180-proj/weather.data', 'r');
    pressure = f.readline()
    humidity = f.readline()
    temperature = f.readline()
    wind_speed = f.readline()
    wind_dir = f.readline()
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
