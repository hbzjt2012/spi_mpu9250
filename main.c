
/*
 * This is for use with the MPU-9250 IMU Chip, SPI interface
 *
 * Mode is 0,0
 * Max SPI frequency is 1 MHz
 *
 */


#include <stdint.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/time.h>
#include <linux/types.h>
#include <linux/spi/spidev.h>

#include "mpu9250_accel_and_gyro.h"

static void pabort(const char *s) {
	perror(s);
	abort();
}

static const char *device = "/dev/spidev1.0";
static uint8_t mode = 0;
static uint8_t bits = 8;
static uint32_t speed = 500000;
static uint16_t delay = 1;

#define MPU9250_ADDRESS0            0x68
#define MPU9250_ADDRESS1            0x69
#define MPU9250_ID                  0x71

/*
int read_write(int fd) {
	int ret;

	//uint8_t read_address = 0x80 | MPU9250_ADDRESS0;
	uint8_t read_address = 0x80 | 0x75; // gyro+accel WHO_AM_I register

	uint8_t tx[2] = {read_address, 0};
	uint8_t rx[2] = {0, 0};

	struct spi_ioc_transfer tr = {
		.tx_buf = (unsigned long)tx,
		.rx_buf = (unsigned long)rx,
		//.len = DATA_SIZE,
		.len = 2,
		.delay_usecs = delay,
		.speed_hz = speed,
		.bits_per_word = bits,
	};

	ret = ioctl(fd, SPI_IOC_MESSAGE(1), &tr);
	if (ret < 1) {
		pabort("can't send spi message");
	}

	printf("rx[0]: %d\n", rx[0]);
	printf("rx[1]: %d\n", rx[1]);

	return 0;
}

*/


int main(int argc, char *argv[]) {
	int ret = 0;
	int fd;

	struct timeval tv;
	int16_t accel[3];
	int16_t temp;
	int16_t gyro[3];

	char rx_buffer[2] = {0, 0};

	fd = open(device, O_RDWR);
	if (fd < 0) {
		pabort("can't open device");
	}

	// SPI mode
	ret = ioctl(fd, SPI_IOC_WR_MODE, &mode);
	if (ret == -1) {
		pabort("can't set spi mode");
	}

	ret = ioctl(fd, SPI_IOC_RD_MODE, &mode);
	if (ret == -1) {
		pabort("can't get spi mode");
	}


	// bits per word
	ret = ioctl(fd, SPI_IOC_WR_BITS_PER_WORD, &bits);
	if (ret == -1) {
		pabort("can't set bits per word");
	}

	ret = ioctl(fd, SPI_IOC_RD_BITS_PER_WORD, &bits);
	if (ret == -1) {
		pabort("can't get bits per word");
	}


	// max speed Hz
	ret = ioctl(fd, SPI_IOC_WR_MAX_SPEED_HZ, &speed);
	if (ret == -1) {
		pabort("can't set max speed hz");
	}

	ret = ioctl(fd, SPI_IOC_RD_MAX_SPEED_HZ, &speed);
	if (ret == -1) {
		pabort("can't get max speed hz");
	}

	printf("spi mode: %d\n", mode);
	printf("bits per word: %d\n", bits);
	printf("max speed: %d Hz (%d KHz)\n", speed, speed/1000);


	//read_write(fd);
	ret = read_register(fd, 0x75, rx_buffer, 2);
	printf("rx[0]: 0x%x\n", rx_buffer[0]);
	printf("rx[1]: 0x%x\n", rx_buffer[1]);

	while(1) {

		get_accel_temp_gyro(fd, accel, &temp, gyro);
		gettimeofday(&tv, NULL);

		printf("%ld.%06ld|atg:%d,%d,%d,%d,%d,%d,%d\n",
			tv.tv_sec, tv.tv_usec,
			accel[0], accel[1], accel[2],
			temp,
			gyro[0], gyro[1], gyro[2]);

		usleep(10000);
	}

	close(fd);
	return ret;
}


