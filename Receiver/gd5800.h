#ifndef __GD5800_H_
#define __GD5800_H_

#include "STC15F2K60S2.H"

#define	SIMPLE_REPORT			0
#define	NOT_REPORT				1
#define	COMPLICATED_REPORT		2
#define	NOT_REPORT_C			3
#define	DINGDONG_type			4
#define	YINYUEYI_type			5
#define	YINYUEER_type			6
#define	EMERGENCY_type			7


extern void GD5800_initialization(void);
extern void GD5800_select_chapter(unsigned int chapter_number);
extern void GD5800_sound_volume(unsigned char volume);
extern void GD5800_stop_music(void);
extern void GD5800_play_music(void);
extern void submenuf6_1(unsigned char report_mode, unsigned char *report_number, unsigned int call_type, unsigned char key_value);
extern void submenuf6_1_voiceselect(unsigned char report_mode, unsigned char *report_number);
extern void GD5800_select_chapter_new(unsigned int chapter_number);
#endif