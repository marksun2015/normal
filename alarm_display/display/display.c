/*
 *  display.c
 *
 *  Copyright (C) 2015 MarkSun (mark@weintek.com)
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License version 2 as
 *  published by the Free Software Foundation.
 *
 *  Overview:
 *   This utility display progressbar and text in screen
 *
 */

#include <string.h>
#include <stdlib.h>

#include "progressbar.h"
#include "font_8x8.h"
#include "font_8x16.h"
#include "font_10x18.h"
#include "font_25x57.h"

#define ADDR_DISP       0x27C00000 ///MAP FROM BOOT ARGS "fb"
#define DISP_WIDTH      800
#define DISP_HEIGHT     480

//#define DBGMSG( x... )  fprintf(stdout, ##x )
#define DBGMSG( x... )

#define ARGB_R(argb) (argb >> 16) & 0xFF
#define ARGB_G(argb) (argb >> 8) & 0xFF
#define ARGB_B(argb) (argb) & 0xFF

typedef unsigned char byte;

typedef void (* FUNC_DRAW_SCREEN)(int x, int y, int color);
FUNC_DRAW_SCREEN draw_screen;

u32 *fb_buffer;
unsigned disp_mapped_size;

void DrawPixNV12(void *addrbase, int x, int y , int R, int G, int B)
{
	int i = 0, j = 0;
	int offset;
	int len = 1;

	int Y, U, V;
	byte *lineY, *lineUV;

	offset = y*DISP_WIDTH + x;

	if (y > DISP_HEIGHT)
		return;

	lineY = (unsigned char *) malloc(len);
	lineUV = (unsigned char *) malloc(len);

	Y = ((66 * R + 129 * G + 25 * B + 128) >> 8) + 16;
	U = ((-38 * R - 74 * G + 112 * B + 128) >> 8) + 128;
	V = ((112 * R - 94 * G - 18 * B + 128) >> 8) + 128;

	for (i = 0; i < len; i++) {
		lineY[i] = (byte)((Y < 0) ? 0 : ((Y > 255) ? 255 : Y));
		lineUV[j++] = (byte)((U < 0) ? 0 : ((U > 255) ? 255 : U));
		lineUV[j++] = (byte)((V < 0) ? 0 : ((V > 255) ? 255 : V));
	}

	len = ((x+len) > DISP_WIDTH) ? DISP_WIDTH-x:len ;
	memcpy(addrbase+offset,lineY, len);
	offset = (y/2)*DISP_WIDTH + x + DISP_WIDTH*DISP_HEIGHT;
	memcpy(addrbase+offset-x%2,lineUV, len*2);

	free(lineY);
	free(lineUV);
}

void draw_RGB_screen(int x, int y, int color)
{
	u32 *p,*p1;
	p = (u32 *)(draw_info.addr + draw_info.scr_var.xres*y*sizeof(u32) + x*sizeof(u32));
	*p = color;
}

void draw_YUV_screen(int x, int y, int color)
{
	DrawPixNV12(draw_info.addr, x, y, ARGB_R(color), ARGB_G(color), ARGB_B(color));
}

static void draw_box(int x,int y,int x1,int y1,int screenx)
{
	int i;
	unsigned int color32 = draw_info.src_color.margin32;
	//printf("x %d y %d x1 %d y1 %d \n", x,y,x1,y1);
	if (draw_info.scr_var.bits_per_pixel ==32)
	{
		u32 *p,*p1;
		for (i = x;i<x1;i++)  //draw v_line
		{
			draw_screen(i, y, color32);
			draw_screen(i, y1, color32);
		}

		for (i = y;i<y1;i++)	//draw h_line
		{
			draw_screen(x, i, color32);
			draw_screen(x1, i, color32);
		}
	}
}

static void draw_bar_v(int startx,int starty,int len,int hight,int screenx, unsigned int color32)
{
	int i,j;

	if (draw_info.scr_var.bits_per_pixel == 32)
	{
		u32 *p;
		for (i = starty; i<starty+hight; i++)
			for (j = startx; j<len+startx; j++)
			{
				draw_screen(j, i, color32);
			}
	}
}

static void put_char_v3(int x, int y, int c, int selection, int font_w,int font_h, unsigned int color32)
{
	int i,j,bits;
	unsigned int *p;

	int xc=x;
	int yc=y;

	if(selection == 0){
		for (i = 0; i < font_h; i++) {
			bits = font_descriptor.data[font_h * c * 4 + i * 4];
			for (j = 0; j < font_w; j++, bits <<= 1){
				if((j%8) == 0) {
					bits = font_descriptor.data[font_h * c * 4 + i * 4 + j/8];
				}
				if (bits & 0x80){
					draw_screen(x+j, y+i, color32);
				}
			}
		}
	}else if(selection == 180){
		for (i = 0; i < font_h; i++) {
			bits = font_descriptor.data[font_h * c * 4 + i * 4];
			for (j = 0; j < font_w; j++, bits <<= 1){
				if((j%8) == 0) {
					bits = font_descriptor.data[font_h * c * 4 + i * 4 + j/8];
				}
				if (bits & 0x80){
					draw_screen(x-j, y-i, color32);
				}
			}
            printf("\n");
		}
	}
}

static void put_char_v2(int x, int y, int c, int selection, int font_w,int font_h, unsigned int color32)
{
	int i,j,bits;
	unsigned int *p;

	int xc=x;
	int yc=y;

	if(selection == 0){
		for (i = 0; i < font_h; i++) {
			bits = font_descriptor.data[font_h * c * 2 + i*2];
			for (j = 0; j < font_w; j++, bits <<= 1){
				if(j==8){
					bits = font_descriptor.data[font_h * c * 2 + i*2 +1];
				}
				if (bits & 0x80){
					draw_screen(x+j, y+i, color32);
				}
			}
		}
	}else if(selection == 90){
		for (i = 0; i < font_h; i++) {
			bits = font_descriptor.data[font_h * c * 2 + i*2];
			for (j = 0; j < font_w; j++, bits <<= 1){
				if(j==8){
					bits = font_descriptor.data[font_h * c * 2 + i*2 + 1];
				}
				if (bits & 0x80){
					draw_screen(xc, yc, color32);
				}
				yc--;
			}
			yc=yc+font_w;
			xc++;
		}
	}else if(selection == 180){
		for (i = 0; i < font_h; i++) {
			bits = font_descriptor.data[font_h * c * 2 + i*2];
			for (j = 0; j < font_w; j++, bits <<= 1){
				if(j==8){
					bits = font_descriptor.data[font_h * c * 2 + i*2 +1];
				}
				if (bits & 0x80){
					draw_screen(x-j, y-i, color32);
				}
			}
		}
	}else if(selection == 270){
		for (i = 0; i < font_h; i++) {
			bits = font_descriptor.data[font_h *c * 2 + i*2];
			for (j = 0; j < font_w; j++, bits <<= 1){
				if(j==8){
					bits = font_descriptor.data[font_h * c * 2 + i*2 +1];
				}
				if (bits & 0x80){
					draw_screen(xc, yc, color32);
				}
				yc++;
			}
			yc=yc-font_w;
			xc--;
		}
	}
}

static void put_char_v1(int x, int y, int c, int selection, int font_w,int font_h, unsigned int color32)
{
	int i,j,bits;
	unsigned int *p;
	int xc=x;
	int yc=y;

	if(selection == 0){
		for (i = 0; i < font_h; i++) {
			bits = font_descriptor.data[font_h * c + i];
			for (j = 0; j < font_w; j++, bits <<= 1){
				if (bits & 0x80){
					draw_screen(x+j, y+i, color32);
				}
			}
		}
	}else if(selection == 90){
		for (i = 0; i < font_h; i++) {
			bits = font_descriptor.data[font_h * c + i];
			for (j = 0; j < font_w; j++, bits <<= 1){
				if (bits & 0x80){
					draw_screen(xc, yc, color32);
				}
				yc--;
			}
			yc=yc+8;
			xc++;
		}
	}else if(selection == 180){
		for (i = 0; i < font_h; i++) {
			bits = font_descriptor.data[font_h * c + i];
			for (j = 0; j < font_w; j++, bits <<= 1){
				if (bits & 0x80){
					draw_screen(x-j, y-i, color32);
				}
			}
		}
	}else if(selection == 270){
		for (i = 0; i < font_h; i++) {
			bits = font_descriptor.data[font_h * c + i];
			for (j = 0; j < font_w; j++, bits <<= 1){
				if (bits & 0x80){
					draw_screen(xc, yc, color32);
				}
				yc++;
			}
			yc=yc-8;
			xc--;
		}
	}
}

static void put_string(int x, int y, char *s, int selection, int font_w, int font_h, DRAW_TEXT_FUNC draw_text, unsigned int color32)
{
	int i;
	if(selection == 0){
		for (i = 0; *s; i++, x += font_w, s++)
			draw_text (x, y, *s, selection, font_w,font_h, color32);
	}else if(selection == 90){
		for (i = 0; *s; i++, y -= font_w, s++)
			draw_text (x, y, *s, selection, font_w,font_h, color32);
	}else if(selection == 180){
		for (i = 0; *s; i++, x -= font_w, s++)
			draw_text (x, y, *s, selection, font_w,font_h, color32);
	}else if(selection == 270){
		for (i = 0; *s; i++, y += font_w, s++)
			draw_text (x, y, *s, selection, font_w,font_h, color32);
	}
}

static void put_string_center(int x, int y, char *s, int selection, int font_w, int font_h, 
        DRAW_TEXT_FUNC draw_text, int sl, unsigned int color32)
{
	put_string (x , y - font_w / 2, s, selection, font_w, font_h, draw_text, color32);
}

#define MISC_IOCGPORTRAIT       (0x80046B1F)
int get_direction(void)
{
	int hw, mode, ret;
	if ((hw = open("/dev/hwmisc", O_RDWR)) < 0) {
		perror("open");
		return -1;
	}

	ret= ioctl(hw, MISC_IOCGPORTRAIT, &mode);

	close(hw);
	if(ret == 0)
	{
		if(mode==2 || mode==180)
			mode = 180;
		else if(mode==3 || mode==90)
			mode = 90;
		else if(mode==4 || mode==250)
			mode = 270;
		else
			mode = 0;
	}
	else
		mode=0;
	draw_info.portrait_mode=mode;
	return 0;
}

void disp_open()
{
	int fd, i, j;
	off_t target = ADDR_DISP;
	unsigned width = 8;
	byte *rgb_in, *yuv_out;

	unsigned page_size, offset_in_page;

	/* MAP DISP PHY ADDR*/
	fd = open("/dev/mem", O_RDWR | O_SYNC);

	page_size = getpagesize();
	offset_in_page = (unsigned)target & (page_size - 1);

	disp_mapped_size = DISP_WIDTH * DISP_HEIGHT * 3 / 2; //YUV420

	draw_info.addr = mmap(NULL,
                 disp_mapped_size,
                 PROT_READ | PROT_WRITE,
                 MAP_SHARED,
                 fd,
                 target & ~(off_t)(page_size - 1));

	draw_info.scr_var.bits_per_pixel = 32;
	draw_info.scr_var.xres = 800;
	draw_info.scr_var.yres = 480;
	draw_info.resolution = RES_800x480;
	draw_info.src_color.margin32 = COLOR_GREEN;
	draw_info.src_color.bar32 = COLOR_GREEN;
	draw_info.src_color.text32 = COLOR_CYAN;

	draw_screen = draw_YUV_screen;
	close(fd);
}

void disp_close(void)
{
	munmap(draw_info.addr, disp_mapped_size);
}

void disp_clear(void)
{
	/* clear YUV buffer: assign black color */
	memset(draw_info.addr, 0x10, (DISP_WIDTH * DISP_HEIGHT));
	memset(draw_info.addr + (DISP_WIDTH * DISP_HEIGHT), 0x80, disp_mapped_size - (DISP_WIDTH * DISP_HEIGHT));
}

void fb_open(const char *device_node)
{
	int fd = open (device_node,O_RDWR);

	if (fd < 0){
		printf("Error : can not open framebuffer device\n");
	}

	if (ioctl(fd,FBIOGET_FSCREENINFO,&draw_info.scr_fix)){
		printf("Error : reading fixed information\n");
	}

	if (ioctl(fd,FBIOGET_VSCREENINFO,&draw_info.scr_var)){
		printf("Error : reading variable information\n");
	}

	if((draw_info.scr_var.xres == 800) && (draw_info.scr_var.yres == 480))
		draw_info.resolution = RES_800x480;
	else if ((draw_info.scr_var.xres == 480) && (draw_info.scr_var.yres == 272))
		draw_info.resolution = RES_480x272;
	else
		draw_info.resolution = RES_Default;

	draw_info.src_color.margin32 = COLOR_GREEN;
	draw_info.src_color.bar32 = COLOR_GREEN;
	draw_info.src_color.text32 = COLOR_CYAN;

	DBGMSG(" The mem is %d \n The line_length is %d \n The xres is %d \n The yres is %d \n bits_per_pixel is %d \n",
			draw_info.scr_fix.smem_len,	draw_info.scr_fix.line_length,draw_info.scr_var.xres,
			draw_info.scr_var.yres, draw_info.scr_var.bits_per_pixel);

	// map the device in memory
	draw_info.addr =(char *) mmap (NULL, draw_info.scr_var.yres * draw_info.scr_fix.line_length,
			PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
	if (draw_info.addr == MAP_FAILED ){
		printf ("Error: failed to map framebuffer device to memory.\n");
	}

	/* clear framebuffer */
	memset(draw_info.addr, 0, draw_info.scr_var.yres * draw_info.scr_fix.line_length);

	fb_buffer = malloc(draw_info.scr_var.xres * draw_info.scr_var.yres * sizeof(u32));

	draw_screen = draw_RGB_screen;
	close(fd);
}

void fb_close(void)
{
	free(fb_buffer);
	munmap (draw_info.addr , draw_info.scr_var.yres * draw_info.scr_fix.line_length);
}

void fb_clear(void)
{
	/* clear framebuffer */
	memset(draw_info.addr, 0, draw_info.scr_var.yres * draw_info.scr_fix.line_length);
}

void fb_update(void)
{
	/* clear framebuffer */
	memcpy(draw_info.addr, fb_buffer, draw_info.scr_var.yres * draw_info.scr_fix.line_length);
}

void progressbar(int ratio, unsigned int color32)
{
	int screenx=draw_info.scr_var.xres;
	int screeny=draw_info.scr_var.yres;

	//draw_info.src_color.bar32 = color32;
	if (ratio >100) ratio = 100;

	if (!(draw_info.portrait_mode == 90 || draw_info.portrait_mode == 180 || draw_info.portrait_mode == 270))
	{
		int startx = screenx/8;
		int len = screenx - screenx/8 - startx;
		int starty = screeny - screeny/6;

		if(draw_info.resolution == RES_480x272)
			starty = screeny - screeny/6-5;

		draw_bar_v(startx,starty,len*ratio/100,BAR_WEIGHT,screenx, color32);

	}
	else if (draw_info.portrait_mode == 90)
	{
		int startx = screenx - screenx/8;
		int len = screeny - (screeny/8)*2 ;
		int starty =  screeny /8;
		draw_bar_v(startx,screeny -(len*ratio/100 +screeny/8 ),BAR_WEIGHT,len*ratio/100,screenx, color32);
	}
	else if(draw_info.portrait_mode == 270)
	{
		int startx = screenx/8;
		int starty = screeny/8;
		int len = screeny - screeny/8 - starty;
		draw_bar_v(startx,starty,BAR_WEIGHT,len*ratio/100,screenx, color32);
	}
	else if (draw_info.portrait_mode == 180)
	{
		int startx = screenx/8;
		int len = screenx - (screenx/8 )*2;
		int starty = screeny /8;

		if(draw_info.resolution == RES_480x272)
			starty = screeny /8-25;

		draw_bar_v(screenx - (len*ratio/100 +screenx/8) ,starty,len*ratio/100,BAR_WEIGHT,screenx, color32);
	}
}

void display_margin (void)
{
	int startx;
	int lenx;
	int starty;
	int leny;

	if (!(draw_info.portrait_mode == 90 || draw_info.portrait_mode == 180 || draw_info.portrait_mode == 270)){
		startx = draw_info.scr_var.xres/8;
		lenx = draw_info.scr_var.xres-(draw_info.scr_var.xres/8)-startx;
		leny = draw_info.scr_var.yres-(draw_info.scr_var.yres/2)-(draw_info.scr_var.yres/8);
		starty = draw_info.scr_var.yres-(draw_info.scr_var.yres/2);
		draw_box(startx-BOX_EDAGE*2,starty-BOX_EDAGE*2,startx+lenx+BOX_EDAGE*2,starty+leny+BAR_WEIGHT/2+BOX_EDAGE,draw_info.scr_var.xres);
	}
	else if (draw_info.portrait_mode == 90){
		startx =draw_info.scr_var.xres- draw_info.scr_var.xres/8;
		lenx = draw_info.scr_var.yres-(draw_info.scr_var.yres/8)*2;
		starty = (draw_info.scr_var.yres/8);
		leny = draw_info.scr_var.xres-(draw_info.scr_var.xres/2)-(draw_info.scr_var.xres/4);
		draw_box(startx-BOX_EDAGE*2-leny,starty-BOX_EDAGE*2,startx+BOX_EDAGE*2+BAR_WEIGHT,starty+lenx+BOX_EDAGE,draw_info.scr_var.xres);
	}
	else if (draw_info.portrait_mode == 180){
		startx = draw_info.scr_var.xres/8;
		lenx = draw_info.scr_var.xres-(draw_info.scr_var.xres/8)-startx;
		leny = draw_info.scr_var.yres-(draw_info.scr_var.yres/2)-(draw_info.scr_var.yres/8);
		starty = draw_info.scr_var.yres-(draw_info.scr_var.yres/2)-leny-(BOX_EDAGE*6);
		draw_box(startx-BOX_EDAGE*2,starty-BOX_EDAGE*2,startx+lenx+BOX_EDAGE*2,starty+leny+BAR_WEIGHT/2+BOX_EDAGE,draw_info.scr_var.xres);
	}
	else if (draw_info.portrait_mode == 270){
		lenx = draw_info.scr_var.yres-(draw_info.scr_var.yres/8)*2;
		starty = (draw_info.scr_var.yres/8);
		leny = draw_info.scr_var.xres-(draw_info.scr_var.xres/2)-(draw_info.scr_var.xres/4);
		startx =(draw_info.scr_var.xres/8) + leny;
		draw_box(startx-BOX_EDAGE*2-leny,starty-BOX_EDAGE*2,startx+BOX_EDAGE*2+BAR_WEIGHT,starty+lenx+BOX_EDAGE,draw_info.scr_var.xres);
	}

}

void flicker_display (int font_type, const char *str_input, int position, unsigned int color32)
{
	int displayX,displayY;
	unsigned char display_str[128];
	int strlength=strlen(str_input);
	int i, count = 20;

	strncpy(display_str,str_input,strlen(str_input));
	display_str[strlength]='\0';

	if(draw_info.addr == NULL ){
		printf ("Error: mapping framebuffer memory is NULL.\n");
	}
	switch (draw_info.portrait_mode) {
		case 90:
		case 180:
			if(font_type==VGA8x8_IDX){
				font_descriptor=font_vga_8x8;
				draw_text=put_char_v1;
				displayX=draw_info.scr_var.xres-100;
				displayY=draw_info.scr_var.yres-50;
			}else if(font_type==VGA8x16_IDX){
				font_descriptor=font_vga_8x16;
				draw_text=put_char_v1;
				displayX=draw_info.scr_var.xres-100 ;
				displayY=draw_info.scr_var.yres-70;
			}else if (font_type==VGA10x18_IDX){
				font_descriptor=font_vga_10x18;
				draw_text=put_char_v2;
				if(position==TEXT_UPPER){
					displayX=draw_info.scr_var.xres/2+((strlength/2)*font_descriptor.width);
					displayY=draw_info.scr_var.yres/2+80;
				}else if(position ==TEXT_LOWER){
					//displayX=draw_info.scr_var.xres/2+((strlength/2)*font_descriptor.width);
					displayX=draw_info.scr_var.xres - draw_info.scr_var.xres/8;
					displayY=draw_info.scr_var.yres/2-25;
				}
			}else if (font_type==VGA25x57_IDX){
				font_descriptor=font_vga_25x57;
				draw_text=put_char_v3;
				if(position==TEXT_UPPER){
					displayX=draw_info.scr_var.xres/2+((strlength/2)*font_descriptor.width);
					displayY=draw_info.scr_var.yres/2+80;
				}else if(position ==TEXT_LOWER){
					//displayX=draw_info.scr_var.xres/2+((strlength/2)*font_descriptor.width);
					displayX=draw_info.scr_var.xres - draw_info.scr_var.xres/8;
					displayY=draw_info.scr_var.yres/2-25;
				}
			}

			break;
		case 270:
		default :
			if(font_type==VGA8x8_IDX){
				font_descriptor=font_vga_8x8;
				draw_text=put_char_v1;
				displayX=draw_info.scr_var.xres/3+50;
				displayY=draw_info.scr_var.yres/4;
			}else if(font_type==VGA8x16_IDX){
				font_descriptor=font_vga_8x16;
				draw_text=put_char_v1;
				displayX=draw_info.scr_var.xres/3+50;
				displayY=draw_info.scr_var.yres/4+20;
			}else if (font_type==VGA10x18_IDX){
				font_descriptor=font_vga_10x18;
				draw_text=put_char_v2;
				if(position ==TEXT_UPPER){
					displayX=draw_info.scr_var.xres/2-((strlength/2)*font_descriptor.width);
					displayY=draw_info.scr_var.yres/2-80;
				}else if(position ==TEXT_LOWER){
					//displayX=draw_info.scr_var.xres/2-((strlength/2)*font_descriptor.width);
					displayX=draw_info.scr_var.xres/8;
					displayY=draw_info.scr_var.yres/2+25;
				}
			}else if (font_type==VGA25x57_IDX){
				font_descriptor=font_vga_25x57;
				draw_text=put_char_v3;
				if(position ==TEXT_UPPER){
					displayX=draw_info.scr_var.xres/2-((strlength/2)*font_descriptor.width);
					displayY=draw_info.scr_var.yres/2-80;
				}else if(position ==TEXT_LOWER){
					//displayX=draw_info.scr_var.xres/2-((strlength/2)*font_descriptor.width);
					displayX=draw_info.scr_var.xres/8;
					displayY=draw_info.scr_var.yres/2+25;
				}
			}

			break;
	}

    for (i = 0 ; i < count; i++) {
        if (draw_info.portrait_mode == 180) {
            draw_bar_v(800, displayY-75,
                    draw_info.scr_var.xres,
                    BAR_WEIGHT,
                    draw_info.scr_var.xres, 
                    COLOR_RED); // to clear previous text
        } else {
            draw_bar_v(0, displayY-8,
                    draw_info.scr_var.xres,
                    BAR_WEIGHT,
                    draw_info.scr_var.xres, 
                    COLOR_RED); // to clear previous text
        }

        put_string_center(displayX, displayY,
                display_str,
                draw_info.portrait_mode,
                font_descriptor.width,
                font_descriptor.height,
                draw_text,
                strlength,
                color32);

        usleep(600000);

        if (draw_info.portrait_mode == 180) {
            draw_bar_v(800, displayY-75,
                    draw_info.scr_var.xres,
                    BAR_WEIGHT,
                    draw_info.scr_var.xres, 
                    COLOR_BLACK); // to clear previous text
        } else {
            draw_bar_v(0, displayY-8,
                    draw_info.scr_var.xres,
                    BAR_WEIGHT,
                    draw_info.scr_var.xres, 
                    COLOR_BLACK); // to clear previous text
        }

        usleep(400000);
    }

    if (draw_info.portrait_mode == 180) {
        draw_bar_v(800, displayY-75,
                draw_info.scr_var.xres,
                BAR_WEIGHT,
                draw_info.scr_var.xres, 
                COLOR_BLACK); // to clear previous text
    } else {
        draw_bar_v(0, displayY-8,
                draw_info.scr_var.xres,
                BAR_WEIGHT,
                draw_info.scr_var.xres, 
                COLOR_BLACK); // to clear previous text
    }
}

void marquee_display (int font_type,const char *str_input,int position, int color32)
{
	int displayX,displayY;
	unsigned char display_str[128];
	int strlength=strlen(str_input);
	int i, count = 300;
	int step = 30;

	strncpy(display_str,str_input,strlen(str_input));
	display_str[strlength]='\0';

	if(draw_info.addr == NULL ){
		printf ("Error: mapping framebuffer memory is NULL.\n");
	}
	switch (draw_info.portrait_mode) {
		case 90:
		case 180:
			if(font_type==VGA8x8_IDX){
				font_descriptor=font_vga_8x8;
				draw_text=put_char_v1;
				displayX=draw_info.scr_var.xres-100;
				displayY=draw_info.scr_var.yres-50;
			}else if(font_type==VGA8x16_IDX){
				font_descriptor=font_vga_8x16;
				draw_text=put_char_v1;
				displayX=draw_info.scr_var.xres-100 ;
				displayY=draw_info.scr_var.yres-70;
			}else if (font_type==VGA10x18_IDX){
				font_descriptor=font_vga_10x18;
				draw_text=put_char_v2;
				if(position==TEXT_UPPER){
					displayX=draw_info.scr_var.xres/2+((strlength/2)*font_descriptor.width);
					displayY=draw_info.scr_var.yres/2+80;
				}else if(position ==TEXT_LOWER){
					//displayX=draw_info.scr_var.xres/2+((strlength/2)*font_descriptor.width);
					displayX=draw_info.scr_var.xres - draw_info.scr_var.xres/8;
					displayY=draw_info.scr_var.yres/2-25;
				}
			}else if (font_type==VGA25x57_IDX){
				font_descriptor=font_vga_25x57;
				draw_text=put_char_v3;
				if(position==TEXT_UPPER){
					displayX=draw_info.scr_var.xres/2+((strlength/2)*font_descriptor.width);
					displayY=draw_info.scr_var.yres/2+80;
				}else if(position ==TEXT_LOWER){
					//displayX=draw_info.scr_var.xres/2+((strlength/2)*font_descriptor.width);
					displayX=draw_info.scr_var.xres - draw_info.scr_var.xres/8;
					displayY=draw_info.scr_var.yres/2-25;
				}
			}

			break;
		case 270:
		default :
			if(font_type==VGA8x8_IDX){
				font_descriptor=font_vga_8x8;
				draw_text=put_char_v1;
				displayX=draw_info.scr_var.xres/3+50;
				displayY=draw_info.scr_var.yres/4;
			}else if(font_type==VGA8x16_IDX){
				font_descriptor=font_vga_8x16;
				draw_text=put_char_v1;
				displayX=draw_info.scr_var.xres/3+50;
				displayY=draw_info.scr_var.yres/4+20;
			}else if (font_type==VGA10x18_IDX){
				font_descriptor=font_vga_10x18;
				draw_text=put_char_v2;
				if(position ==TEXT_UPPER){
					displayX=draw_info.scr_var.xres/2-((strlength/2)*font_descriptor.width);
					displayY=draw_info.scr_var.yres/2-80;
				}else if(position ==TEXT_LOWER){
					//displayX=draw_info.scr_var.xres/2-((strlength/2)*font_descriptor.width);
					displayX=draw_info.scr_var.xres/8;
					displayY=draw_info.scr_var.yres/2+25;
				}
			}else if (font_type==VGA25x57_IDX){
				font_descriptor=font_vga_25x57;
				draw_text=put_char_v3;
				if(position ==TEXT_UPPER){
					displayX=draw_info.scr_var.xres/2-((strlength/2)*font_descriptor.width);
					displayY=draw_info.scr_var.yres/2-80;
				}else if(position ==TEXT_LOWER){
					//displayX=draw_info.scr_var.xres/2-((strlength/2)*font_descriptor.width);
					displayX=draw_info.scr_var.xres/8;
					displayY=draw_info.scr_var.yres/2+25;
				}
			}

			break;
	}

	draw_info.src_color.bar32 = COLOR_BLACK;
    for (i = 0 ; i < count; i++) {
        if (draw_info.portrait_mode == 180) {
            draw_bar_v(800, displayY-70,
                    draw_info.scr_var.xres,
                    BAR_WEIGHT,
                    draw_info.scr_var.xres,
                    draw_info.src_color.bar32); // to clear previous text
        } else {
            draw_bar_v(0, displayY-8,
                    draw_info.scr_var.xres,
                    BAR_WEIGHT,
                    draw_info.scr_var.xres,
                    draw_info.src_color.bar32); // to clear previous text
        }

        put_string_center(displayX, displayY,
                display_str, 
                draw_info.portrait_mode,
                font_descriptor.width, 
                font_descriptor.height, 
                draw_text,
                strlength,
                color32);
        
        if (draw_info.portrait_mode == 180) {
            displayX -= step;
            if (displayX < 0)
                displayX = 800;
        } else {
            displayX += step;
            if (displayX > 800)
                displayX = 0;
        }
        usleep(100000);
    } 

	draw_info.src_color.text32 = COLOR_CYAN;
    if (draw_info.portrait_mode == 180) {
        draw_bar_v(800, displayY-60,
                draw_info.scr_var.xres,
                BAR_WEIGHT,
                draw_info.scr_var.xres,
                draw_info.src_color.bar32); // to clear previous text
    } else {
        draw_bar_v(0, displayY-8,
                draw_info.scr_var.xres,
                BAR_WEIGHT,
                draw_info.scr_var.xres,
                draw_info.src_color.bar32); // to clear previous text
    }
}

void display_text (int font_type, const char *str_input, int position, unsigned int color32)
{
	int displayX,displayY;
	unsigned char display_str[128];
	int strlength=strlen(str_input);

	strncpy(display_str, str_input, strlen(str_input));
	display_str[strlength]='\0';

	if(draw_info.addr == NULL ){
		printf ("Error: mapping framebuffer memory is NULL.\n");
	}
	switch (draw_info.portrait_mode) {
		case 90:
			if(font_type==VGA8x8_IDX){
				font_descriptor=font_vga_8x8;
				draw_text=put_char_v1;
				displayX=draw_info.scr_var.xres/3;
				displayY=draw_info.scr_var.yres-50;
				if(draw_info.resolution == RES_480x272) {
					if(position == TEXT_UPPER) {
						displayX=draw_info.scr_var.xres/3;
						displayY=draw_info.scr_var.yres/2+((strlength/2)*font_descriptor.width);
					} else if (position == TEXT_MIDDLE) {
						displayX=draw_info.scr_var.xres/3+20;
						displayY=draw_info.scr_var.yres/2+((strlength/2)*font_descriptor.width)+5;
					} else {
						displayX=draw_info.scr_var.xres/2+70;
						displayY=draw_info.scr_var.yres/2+((strlength/2)*font_descriptor.width);

					}
				}
			}else if (font_type==VGA8x16_IDX){
				font_descriptor=font_vga_8x16;
				draw_text=put_char_v1;
				displayX=draw_info.scr_var.xres/3+20;
				displayY=draw_info.scr_var.yres-50;
				if(draw_info.resolution){
					if(position == TEXT_UPPER){
						displayX=draw_info.scr_var.xres/2;
						displayY=draw_info.scr_var.yres/2+((strlength/2)*font_descriptor.width);
					}else if(position == TEXT_LOWER){
						displayX=draw_info.scr_var.xres/2+150;
						displayY=draw_info.scr_var.yres/2+((strlength/2)*font_descriptor.width);
					}
				}
			}else if (font_type==VGA10x18_IDX){
				font_descriptor=font_vga_10x18;
				draw_text=put_char_v2;
				if(position==TEXT_UPPER){
					displayX=draw_info.scr_var.xres/2;
					displayY=draw_info.scr_var.yres/2+((strlength/2)*font_descriptor.width);
				}else if(position==TEXT_LOWER){
					displayX=draw_info.scr_var.xres/2+150;
					displayY=draw_info.scr_var.yres/2+((strlength/2)*font_descriptor.width);
				}
			}else if (font_type==VGA25x57_IDX){
				font_descriptor=font_vga_25x57;
				draw_text=put_char_v3;
				if(position==TEXT_UPPER){
					displayX=draw_info.scr_var.xres/2;
					displayY=draw_info.scr_var.yres/2+((strlength/2)*font_descriptor.width);
				}else if(position==TEXT_LOWER){
					displayX=draw_info.scr_var.xres/2+150;
					displayY=draw_info.scr_var.yres/2+((strlength/2)*font_descriptor.width);
				}
			}

			break;
		case 180:
			if(font_type==VGA8x8_IDX){
				font_descriptor=font_vga_8x8;
				draw_text=put_char_v1;
				displayX=draw_info.scr_var.xres-100;
				displayY=draw_info.scr_var.yres-50;
			}else if(font_type==VGA8x16_IDX){
				font_descriptor=font_vga_8x16;
				draw_text=put_char_v1;
				displayX=draw_info.scr_var.xres-100 ;
				displayY=draw_info.scr_var.yres-70;
			}else if (font_type==VGA10x18_IDX){
				font_descriptor=font_vga_10x18;
				draw_text=put_char_v2;
				if(position==TEXT_UPPER){
					displayX=draw_info.scr_var.xres/2+((strlength/2)*font_descriptor.width);
					displayY=draw_info.scr_var.yres/2+80;
				}else if(position ==TEXT_LOWER){
					displayX=draw_info.scr_var.xres/2+((strlength/2)*font_descriptor.width);
					displayY=draw_info.scr_var.yres/2-25;
				}
			}else if (font_type==VGA25x57_IDX){
				font_descriptor=font_vga_25x57;
				draw_text=put_char_v3;
				if(position==TEXT_UPPER){
					displayX=draw_info.scr_var.xres/2+((strlength/2)*font_descriptor.width);
					displayY=draw_info.scr_var.yres/2+80;
				}else if(position ==TEXT_LOWER){
					displayX=draw_info.scr_var.xres/2+((strlength/2)*font_descriptor.width);
					displayY=draw_info.scr_var.yres/2-25;
				}
			}

			break;
		case 270:
			if(font_type==VGA8x8_IDX){
				font_descriptor=font_vga_8x8;
				draw_text=put_char_v1;
				displayX=draw_info.scr_var.xres-50;
				displayY=draw_info.scr_var.yres/5;
				if(draw_info.resolution == RES_480x272) {
					if(position == TEXT_UPPER) {
						displayX = draw_info.scr_var.xres-(draw_info.scr_var.xres/3);
						displayY=draw_info.scr_var.yres/2-((strlength/2)*font_descriptor.width);
					} else if (position == TEXT_MIDDLE) { //displayY msut be 12
						displayX= draw_info.scr_var.xres-(draw_info.scr_var.xres/3+20);
						displayY=draw_info.scr_var.yres/2-((strlength/2)*font_descriptor.width)+4;
					} else {
						displayX=draw_info.scr_var.xres/2-50;
						displayY=draw_info.scr_var.yres/2-((strlength/2)*font_descriptor.width);
					}
				}
			}else if(font_type==VGA8x16_IDX){
				font_descriptor=font_vga_8x16;
				draw_text=put_char_v1;
				displayX=draw_info.scr_var.xres-70;
				displayY=draw_info.scr_var.yres/5;
				if(draw_info.resolution){
					if(position==TEXT_UPPER){
						displayX=draw_info.scr_var.xres/2;
						displayY=draw_info.scr_var.yres/2-((strlength/2)*font_descriptor.width);
					}else if(position==TEXT_LOWER){
						displayX=draw_info.scr_var.xres/2-150;
						displayY=draw_info.scr_var.yres/2-((strlength/2)*font_descriptor.width);
					}
				}
			}else if (font_type==VGA10x18_IDX){
				font_descriptor=font_vga_10x18;
				draw_text=put_char_v2;
				if(position ==TEXT_UPPER){
					displayX=draw_info.scr_var.xres/2;
					displayY=draw_info.scr_var.yres/2-((strlength/2)*font_descriptor.width);
				}else if(position ==TEXT_LOWER){
					displayX=draw_info.scr_var.xres/2-150;
					displayY=draw_info.scr_var.yres/2-((strlength/2)*font_descriptor.width);
				}
			}else if (font_type==VGA25x57_IDX){
				font_descriptor=font_vga_25x57;
				draw_text=put_char_v3;
				if(position ==TEXT_UPPER){
					displayX=draw_info.scr_var.xres/2;
					displayY=draw_info.scr_var.yres/2-((strlength/2)*font_descriptor.width);
				}else if(position ==TEXT_LOWER){
					displayX=draw_info.scr_var.xres/2-150;
					displayY=draw_info.scr_var.yres/2-((strlength/2)*font_descriptor.width);
				}
			}

			break;
		default :
			if(font_type==VGA8x8_IDX){
				font_descriptor=font_vga_8x8;
				draw_text=put_char_v1;
				displayX=draw_info.scr_var.xres/3+50;
				displayY=draw_info.scr_var.yres/4;
			}else if(font_type==VGA8x16_IDX){
				font_descriptor=font_vga_8x16;
				draw_text=put_char_v1;
				displayX=draw_info.scr_var.xres/3+50;
				displayY=draw_info.scr_var.yres/4+20;
			}else if (font_type==VGA10x18_IDX){
				font_descriptor=font_vga_10x18;
				draw_text=put_char_v2;
				if(position ==TEXT_UPPER){
					displayX=draw_info.scr_var.xres/2-((strlength/2)*font_descriptor.width);
					displayY=draw_info.scr_var.yres/2-80;
				}else if(position ==TEXT_LOWER){
					displayX=draw_info.scr_var.xres/2-((strlength/2)*font_descriptor.width);
					displayY=draw_info.scr_var.yres/2+25;
				}
			}else if (font_type==VGA25x57_IDX){
				font_descriptor=font_vga_25x57;
				draw_text=put_char_v3;
				if(position ==TEXT_UPPER){
					displayX=draw_info.scr_var.xres/2-((strlength/2)*font_descriptor.width);
					displayY=draw_info.scr_var.yres/2-80;
				}else if(position ==TEXT_LOWER){
					displayX=draw_info.scr_var.xres/2-((strlength/2)*font_descriptor.width);
					displayY=draw_info.scr_var.yres/2+25;
				}
			}

			break;
	}

	draw_info.src_color.bar32=COLOR_BLACK;
    if (draw_info.portrait_mode == 180){
        draw_bar_v(800, displayY-60,
                draw_info.scr_var.xres,
                BAR_WEIGHT,
                draw_info.scr_var.xres,
                draw_info.src_color.bar32); // to clear previous text
    } else {
        draw_bar_v(0, displayY-8,
                draw_info.scr_var.xres,
                BAR_WEIGHT,
                draw_info.scr_var.xres,
                draw_info.src_color.bar32); // to clear previous text
    }

	put_string_center(displayX, displayY, display_str, draw_info.portrait_mode,
			font_descriptor.width, font_descriptor.height, draw_text,strlength, color32);
}

void display_ratio (int ratio, int font_type, unsigned int color32)
{
	int displayX,displayY;
	char str_ratio[8];
	str_ratio[7]='\0';

	if(font_type==VGA8x8_IDX){
		font_descriptor=font_vga_8x8;
		draw_text=put_char_v1;
	}else if(font_type==VGA8x16_IDX){
		font_descriptor=font_vga_8x16;
		draw_text=put_char_v1;
	}else if (font_type==VGA10x18_IDX){
		font_descriptor=font_vga_10x18;
		draw_text=put_char_v2;
	}else if (font_type==VGA25x57_IDX){
		font_descriptor=font_vga_25x57;
		draw_text=put_char_v3;
	}

	if(ratio>100){ratio=100;}

	if(ratio != -1)
		sprintf(str_ratio, "%d%s", ratio,"%");
	else
		str_ratio[0]='\0';

	draw_info.src_color.bar32=COLOR_BLACK;
	switch (draw_info.portrait_mode) {
		case 90:
			if(draw_info.resolution == RES_480x272) {
				displayX=draw_info.scr_var.xres/2+130;
				displayY=draw_info.scr_var.yres/2+10;
			} else {
				displayX=draw_info.scr_var.xres/2+230;
				displayY=draw_info.scr_var.yres/2+30;

			}

			draw_bar_v(displayX-10, displayY-50,
                    BAR_WEIGHT, BAR_WEIGHT+30,draw_info.scr_var.xres, 
                    draw_info.src_color.bar32); // to clear previous text
			put_string_center (displayX, displayY, str_ratio, draw_info.portrait_mode ,
					font_descriptor.width,font_descriptor.height,draw_text,strlen(str_ratio), color32);
		break;
		case 180:
			displayX=draw_info.scr_var.xres/2-10;

			if(draw_info.resolution == RES_480x272)
				displayY=draw_info.scr_var.yres/4;
			else
				displayY=draw_info.scr_var.yres/4+30;

			draw_bar_v(displayX-30, displayY-10,
                    BAR_WEIGHT+30,BAR_WEIGHT,draw_info.scr_var.xres, 
                    draw_info.src_color.bar32); // to clear previous text
			put_string_center (displayX+15, displayY+15, str_ratio, draw_info.portrait_mode,
					font_descriptor.width,font_descriptor.height,draw_text,strlen(str_ratio), color32);
		break;
		case 270:
			if(draw_info.resolution == RES_480x272) {
				displayX=draw_info.scr_var.xres/2-120;
				displayY=draw_info.scr_var.yres/2-10;
			} else {
				displayX=draw_info.scr_var.xres/2-230;
				displayY=draw_info.scr_var.yres/2-20;
			}

			draw_bar_v(displayX-20, displayY-10,
                    BAR_WEIGHT,BAR_WEIGHT+30,draw_info.scr_var.xres, 
                    draw_info.src_color.bar32); // to clear previous text
			put_string_center (displayX, displayY, str_ratio, draw_info.portrait_mode ,
					font_descriptor.width,font_descriptor.height,draw_text,strlen(str_ratio), color32);
		break;
		default:
			displayX=draw_info.scr_var.xres/2-40;

			if(draw_info.resolution == RES_480x272)
				displayY=draw_info.scr_var.yres/2+40;
			else
				displayY=draw_info.scr_var.yres/4+230;

			draw_bar_v(displayX, displayY,
                    BAR_WEIGHT+30,BAR_WEIGHT,draw_info.scr_var.xres, 
                    draw_info.src_color.bar32); // to clear previous text
			put_string_center (displayX+15, displayY+15, str_ratio, draw_info.portrait_mode,
					font_descriptor.width,font_descriptor.height,draw_text,strlen(str_ratio), color32);
		break;
	}
}

void progress_clear(void)
{
	int displayX, displayY;

	draw_info.src_color.bar32=COLOR_BLACK;

	/* clear ratio display */
	switch (draw_info.portrait_mode) {
		case 90:
			displayX=draw_info.scr_var.xres/2+230;
			displayY=draw_info.scr_var.yres/2+30;
			draw_bar_v(displayX-10, displayY-50, 
                    BAR_WEIGHT,BAR_WEIGHT+30,draw_info.scr_var.xres, 
                    draw_info.src_color.bar32); // to clear previous text
		break;
		case 180:
			displayX=draw_info.scr_var.xres/2-10;
			displayY=draw_info.scr_var.yres/4+30;
			draw_bar_v(displayX-30, displayY-10,
                    BAR_WEIGHT+30,BAR_WEIGHT,draw_info.scr_var.xres, 
                    draw_info.src_color.bar32); // to clear previous text
		break;
		case 270:
			displayX=draw_info.scr_var.xres/2-230;
			displayY=draw_info.scr_var.yres/2-20;
			draw_bar_v(displayX-20, displayY-10,
                    BAR_WEIGHT,BAR_WEIGHT+30,draw_info.scr_var.xres, 
                    draw_info.src_color.bar32); // to clear previous text
		break;
		default:
			displayX=draw_info.scr_var.xres/2-40;
			displayY=draw_info.scr_var.yres/4+230;
			draw_bar_v(displayX, displayY,
                    BAR_WEIGHT+30,BAR_WEIGHT,draw_info.scr_var.xres, 
                    draw_info.src_color.bar32); // to clear previous text
		break;
	}
	progressbar(100, COLOR_BLACK);
}


