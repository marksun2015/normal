#ifndef PROGRESS_H
#define PROGRESS_H

#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>
#include <linux/fb.h>
#include <sys/mman.h>

#include "font_type.h"

#define BAR_WEIGHT	70
#define BOX_EDAGE	4

typedef unsigned char 	u8;
typedef unsigned short 	u16;
typedef unsigned long 	u32;

enum {
	RES_Default = 0,
	RES_800x480 = 1,
	RES_480x272 = 2
};


struct fbcon_font_desc {
    int idx;
    char *name;
    int width,height;
    char *data;
    int pref;
};

struct draw_color{
	unsigned short margin16;  //RGB565
	unsigned short bar16;
	unsigned short text16;
	unsigned int margin32;  //ARGB
	unsigned int bar32;
	unsigned int text32;
};

struct fb_info {
	unsigned char *addr;
	const char *image_filename;
	struct fb_var_screeninfo scr_var;
	struct fb_fix_screeninfo scr_fix;
	struct draw_color src_color;
	unsigned int portrait_mode;
	unsigned int resolution;
};

typedef void (* DRAW_TEXT_FUNC) (int x, int y, int c, int selection, int font_w,int font_h,unsigned int color32);

struct fb_info draw_info;
struct fbcon_font_desc font_descriptor;

DRAW_TEXT_FUNC draw_text;

#endif
