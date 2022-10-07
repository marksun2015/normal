#ifndef TEXT_H
#define TEXT_H

#ifdef __cplusplus
extern "C" {
#endif
extern void fb_open(const char *device_node);
extern void progressbar(int ratio, unsigned int color);
extern void display_margin (void);
extern void display_text (int font_type,const char *str_input,int posision);
extern void flicker_display (int font_type,const char *str_input,int position);
extern void marquee_display (int font_type,const char *str_input,int position);
extern void display_ratio (int ratio,int font_type) ;
extern void progress_clear(void);
extern int get_direction(void) ;
extern void fb_close(void);
extern void fb_clear(void);

extern void disp_open();
extern void disp_close();
extern void disp_clear();
#ifdef __cplusplus
}
#endif

#endif
