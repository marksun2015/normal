#include <stdio.h>
#include <fcntl.h>
#include <string.h>
#include <stdlib.h>
#include <sys/times.h>
#include <sys/types.h>
#include <termios.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <sys/reboot.h>
#include <pthread.h>
#include <signal.h>
#include <errno.h>
#include "time.h"
#include "../display/display.h"
#include "../display/font_type.h"
#include "../display/progressbar.h"

typedef struct alarm_data {
    int alarm;
    const char *disp_str;
} alarm_data;

#define MISC_IOCTBEEPCTL                (0x6B2D)
void beep_wakeup()
{
    printf("111 \n");
    int handle = open("/dev/hwmisc", O_RDWR);
    if (!handle) {
        return;
    }

    ioctl(handle, MISC_IOCTBEEPCTL, 1);
    usleep(200000);
    ioctl(handle, MISC_IOCTBEEPCTL, 0);
    usleep(50000);

    ioctl(handle, MISC_IOCTBEEPCTL, 1);
    usleep(200000);
    ioctl(handle, MISC_IOCTBEEPCTL, 0);
    usleep(50000);

    ioctl(handle, MISC_IOCTBEEPCTL, 1);
    usleep(200000);
    ioctl(handle, MISC_IOCTBEEPCTL, 0);
    usleep(50000);

    ioctl(handle, MISC_IOCTBEEPCTL, 1);
    usleep(800000);
    ioctl(handle, MISC_IOCTBEEPCTL, 0);

    close(handle);
    return;
}

void* alarm_display(void* data) 
{
    alarm_data *ad = (alarm_data*) data; // 取得輸入資料
    int font_type = VGA25x57_IDX;
    while(1) {
        if(ad->alarm)
            flicker_display(font_type, ad->disp_str, TEXT_LOWER);

        ad->alarm = 0;
        sleep(1);
    }
    pthread_exit(NULL); 
}


int main(int argc, char *argv[])
{
    char sys_time[128];
	//int font_type = VGA10x18_IDX;
	int font_type;
	int i;
  
	alarm_data data;
    data.alarm = 0;

    time_t rawtime = time(NULL);
    struct tm *tmp = gmtime(&rawtime);
    sprintf(sys_time,"time: %d %d %d", tmp->tm_hour, tmp->tm_min, tmp->tm_sec);

    pthread_t pt; // 宣告 pthread 變數
    pthread_create(&pt, NULL, alarm_display, (void *)&data); // 建立子執行緒

	if(argc < 2)
		return 0;

	if(-1 == get_direction()) {
		printf("can't get portrait mode from hwmisc\n");
		return 0;
	}


    draw_info.portrait_mode = 180;
    font_type = VGA25x57_IDX;

    if(strcmp(argv[1], "FB")==0) {
        fb_open("/dev/fb0");
        while (1)
        {
            rawtime = time(NULL);
            tmp = gmtime(&rawtime);
            //sprintf(sys_time,"Day %02d/%02d(%02d) Time %02d:%02d:%02d",
            //        tmp->tm_mon+1, tmp->tm_mday, tmp->tm_wday, tmp->tm_hour, tmp->tm_min, tmp->tm_sec);
            sprintf(sys_time,"Time %02d:%02d:%02d", tmp->tm_hour, tmp->tm_min, tmp->tm_sec);
            display_text(font_type, sys_time, TEXT_UPPER);

            if((tmp->tm_hour == 9) && (tmp->tm_min == 0) && (tmp->tm_sec == 0)) {
                data.alarm = 1;
                data.disp_str = "Time is up! Get to work!";
            }

            if((tmp->tm_hour == 10) && (tmp->tm_min == 0) && (tmp->tm_sec == 0)) {
                //beep_wakeup();
                data.alarm = 1;
                data.disp_str = "Time is up! Meeting!";
            }

            if((tmp->tm_hour == 12) && (tmp->tm_min == 0) && (tmp->tm_sec == 0)) {
                beep_wakeup();
                data.alarm = 1;
                data.disp_str = "Time is up! Lunch Time!";
            }

            if((tmp->tm_hour == 13) && (tmp->tm_min == 0) && (tmp->tm_sec == 0)) {
                data.alarm = 1;
                data.disp_str = "Time is up! Wake Up!";
            }

            if((tmp->tm_hour == 18) && (tmp->tm_min == 0) && (tmp->tm_sec == 0)) {
                beep_wakeup();
                data.alarm = 1;
                data.disp_str = "Time is up! Get off work!";
            }

            //display_text(font_type,"=============",TEXT_LOWER);
            //display_margin();                 //test
            //progressbar(100 , COLOR_WHITE);   //test
            //progressbar(30 , COLOR_GREEN);    //test
            //display_ratio(30, VGA10x18_IDX);  //test
            sleep(1);
        }	
        fb_close();
    } else if (strcmp(argv[1], "DISP") == 0) {
        disp_open();
        display_text(font_type,"Parsing OS file ...",TEXT_LOWER); //test
        display_text(font_type,"!! Machine is on recovery mode !!",TEXT_UPPER);//test
        display_margin();//test
        progressbar(100 , COLOR_WHITE); //test
        progressbar(30 , COLOR_GREEN); //test
        display_ratio(30, VGA10x18_IDX); //test
        disp_close();
    }
}
