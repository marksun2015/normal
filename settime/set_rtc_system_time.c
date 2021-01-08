#include <errno.h>
#include <fcntl.h>
#include <linux/rtc.h>
#include <stdio.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/time.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>
#include <unistd.h>

int main() {
  int ret;
  // struct tm info;
  struct tm info = {0};
  char buffer[80];

  time_t tval;
  struct timeval tvval;

  info.tm_year = 2017 - 1900;
  info.tm_mon = 12 - 1;
  info.tm_mday = 9;
  info.tm_hour = 3;
  info.tm_min = 1;
  info.tm_sec = 30;
  // info.tm_isdst = -1;

  printf("mktime \n");
  tval = mktime(&info);
  printf(">> %d \n", tval);

  tvval.tv_sec = (long)tval;
  tvval.tv_usec = 0;
  printf("settimeofday \n");
  printf(">> %d \n", tvval.tv_sec);
#if 1
  // set system time
  if (settimeofday(&tvval, 0) == 0) {
    strftime(buffer, sizeof(buffer), "%c", &info);
    printf("set time: %s \n", buffer);

    struct rtc_time time;
    struct rtc_time new_time;
    int fd = -1, ret;

    memcpy(&time, &info, sizeof(struct rtc_time));
    // set RTC
    fd = open("/dev/rtc0", O_RDWR);
    if (fd >= 0) {
      ioctl(fd, RTC_SET_TIME, &time);
      ret = ioctl(fd, RTC_RD_TIME, &new_time);
    }
  } else {
    printf("settimeofday failed \n");
  }
#endif

  return 0;
}
