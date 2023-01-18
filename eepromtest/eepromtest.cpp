#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/i2c-dev.h>

#define DEVICE_NODE "/dev/i2c_eeprom"
#define DEVICE_ADDRESS 0x54

int i2c_write(int fd, unsigned char addr, unsigned char value)
{
    unsigned char buf[2];
    buf[0] = addr;
    buf[1] = value;
    return write(fd, buf, 2);
}

int i2c_read(int fd, unsigned char addr, unsigned char *buf, int size)
{
    write(fd, &addr, 1);
    return read(fd, buf, size);
}

int main(int argc, char *argv[])
{
    unsigned char buf[20] = {0};
    int i = 0;
    int ret = 0;

    int fd = open(DEVICE_NODE, O_RDWR);
    if (fd < 0) {
        printf("Failed opening %s\n", DEVICE_NODE);
        return 1;
    }

    if (ioctl(fd, I2C_SLAVE_FORCE, DEVICE_ADDRESS) < 0) {
        printf("Failed addressing device at %02X\n", DEVICE_ADDRESS);
        close(fd);
        return 1;
    }

    for (i = 0; i < 20; i++) {
        i2c_write(fd, 0xc0+i, i);
        usleep(3000);
    }

    ret = i2c_read(fd, 0xc0, buf, sizeof(buf));
    for (i = 0; i < 20; i++) {
        printf("%x ",buf[i]);
    }
    printf("\n");

    close(fd);
    return 0;
}
