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

void* alarm_display(void* data) 
{
    alarm_data *ad = (alarm_data*) data; // 取得輸入資料
    while(1) {
        printf("%d\n", ad->alarm); 
        printf("%s\n", ad->disp_str); 
        sleep(1);
    }
    pthread_exit(NULL); 
}


int main(int argc, char *argv[])
{
    char sys_time[128];
	int font_type = VGA10x18_IDX;
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


	if((90 == draw_info.portrait_mode) || (270 == draw_info.portrait_mode)) {
		switch (draw_info.resolution) {
		case RES_480x272:
			font_type = VGA8x8_IDX;
			break;
		case RES_800x480:
			font_type = VGA8x16_IDX;
			break;
		default:
			font_type = VGA10x18_IDX;
			break;
		}
	}

    draw_info.portrait_mode = 180;
    font_type = VGA10x18_IDX;

    if(strcmp(argv[1], "FB")==0) {
        fb_open("/dev/fb0");
        while (1)
        {
            rawtime = time(NULL);
            tmp = gmtime(&rawtime);
            sprintf(sys_time,"time: %d %d %d", tmp->tm_hour, tmp->tm_min, tmp->tm_sec);
            display_text(font_type, sys_time, TEXT_UPPER);

            data.alarm++;
            data.disp_str = "hihihi";
            //marquee_display(font_type,"Time is up! Time is up!",TEXT_LOWER); //test
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
