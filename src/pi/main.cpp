#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <termios.h>
#include <fstream>

int main(int argc, char ** argv) {
	int fd;
	char buf[256];
	int n;

	// Open the Port. We want read/write, no "controlling tty" status,
	// and open it no matter what state DCD is in
	fd = open("/dev/ttyACM0", O_RDWR | O_NOCTTY | O_NDELAY);

	//open mbed's USB virtual com port
	if (fd == -1) {
		perror("open_port: Unable to open /dev/ttyACM0 - ");
		return -1;
	}

	// Turn off blocking for reads, use (fd, F_SETFL, FNDELAY) if you want that
	fcntl(fd, F_SETFL, 0);

	//Linux Raw serial setup options
	struct termios options;
	tcgetattr(fd,&options); //Get current serial settings in structure
	cfsetspeed(&options, B9600); //Change a only few
	options.c_cflag &= ~CSTOPB;
	options.c_cflag |= CLOCAL;
	options.c_cflag |= CREAD;
	cfmakeraw(&options);
	tcsetattr(fd,TCSANOW,&options);  //Set serial to new settings
	sleep(0.5);

	// Write to the port
	read(fd, buf, 256); // clear extra chars in the rx buffer
	n = write(fd,"!t",2);
	if (n < 0) {
		perror("Write failed - ");
		return -1;
	}

	// Read the characters from the port if they are there
	sleep(0.5);
	n = read(fd, buf, 256);
	if (n < 0) {
		perror("Read failed - ");
		return -1;
	} else if (n == 0) {
        printf("No data on port\n");
    } else {
		buf[n] = 0;
		printf("%i bytes read back: %s\n\r", n, buf);
	}

    // Parse packet sent by mbed
    float  pressure;
    float  humidity;
    float  temperature;
    float  wind;
	float  rain_size;
    char   wind_dir[2];

    int result = sscanf(buf, "!t PRES %f HUM %f TEMP %f WIND %f %s RAIN %f",
                        &pressure, &humidity, &temperature, &wind, wind_dir, &rain_size);

    if (buf[0] != '!') result = -1;

    if (result < 0) {
        printf("sscanf: parse failed\n");
		return -1;
    } else {
        printf("data acquired:\n\
                pressure: %.2f\n\
                humidity: %.2f\n\
                temperature: %.2f\n\
                wind: %.2f %s\n\
				rain: %.2f\n",
                pressure, humidity, temperature, wind, wind_dir, rain_size);
    }

	std::ofstream data("weather.data");

	data << pressure << std::endl;
	data << humidity << std::endl;
	data << temperature << std::endl;
	data << wind << std::endl;
	data << wind_dir << std::endl;
	data << rain_size << std:endl;

	// Don't forget to clean up and close the port
	tcdrain(fd);
	close(fd);
	return 0;
}
