#include <linux/rtc.h> 
#include <sys/ioctl.h> 
#include <sys/time.h> 
#include <sys/types.h> 
#include <stdio.h> 
#include <fcntl.h> 
#include <unistd.h> 
#include <errno.h> 
#include <unistd.h>  
#include <time.h>
#include <string.h>

int main ()
{
    int ret;
    struct tm info;
    char buffer[80];

    time_t tval;
    struct timeval tvval;

    info.tm_year = 2002 - 1900;
    info.tm_mon = 7 - 1;
    info.tm_mday = 4;
    info.tm_hour = 0;
    info.tm_min = 0;
    info.tm_sec = 1;
    info.tm_isdst = -1;

    tval = mktime(&info);
    tvval.tv_sec = (long)tval;
    tvval.tv_usec = 0;
    //set system time
    if (settimeofday(&tvval, NULL) == 0) {
        strftime(buffer, sizeof(buffer), "%c", &info);
        printf("set time: %s \n",buffer);

        struct rtc_time time;
        struct rtc_time new_time;
        int fd = -1, ret;

        memcpy(&time, &info, sizeof(struct rtc_time));
        //set RTC
        fd = open("/dev/rtc0", O_RDWR);
        if (fd >= 0) {
            ioctl(fd, RTC_SET_TIME, &time);
            ret = ioctl(fd, RTC_RD_TIME, &new_time);
        }
    }

    return 0;
}
