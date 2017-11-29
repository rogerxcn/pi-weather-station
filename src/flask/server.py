from flask import Flask, render_template, Responce
import json
from camera import Camera


app = Flask(__name__)
temperature = 0
humidity = 0
pressure = 0
wind_speed = 0
wind_dir = 'N/A'

@app.route('/getJSON')
def get_JSON():
    data = {
        'temperature'   : temperature,
        'humidity'      : humidity,
        'pressure'      : pressure,
        'wind_speed'    : wind_speed,
        'wind_dir'      : wind_dir
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
                            html_winddir=wind_dir)

def gen(camera):
    while True:
        frame = camera.get_frame()
        yield (b'--frame\r\n'
               b'Content-Type: image/jpeg\r\n\r\n' + frame + b'\r\n')

@app.route('/video_feed')
def video_feed():
    return Response(gen(Camera()),
                    mimetype='multipart/x-mixed-replace; boundary=frame')

if __name__ == '__main__':
  app.run(debug=True, host='0.0.0.0')
