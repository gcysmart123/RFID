#ifndef __DECODER_H_
#define __DECODER_H_

#include "STC15F2K60S2.H"
#include "global.h"
#include "tpyedef.h"

#ifdef REPLAY_PATCH
/*sjz add 2015 11 30*/
typedef struct  repeat_mode_saved_parameters{
unsigned char report_mode;
unsigned char report_number[8];
unsigned int call_type;
unsigned char key_value;
}re_save_paras;
/*sjz add 2015 11 30 end*/
extern re_save_paras rep_saved_parameter_when_dec;
extern unsigned char  repeat_times;
extern void replay_process(void);
#endif
extern uint8_t  decoder_num;

extern void DecoderProcess(void);

extern void set_Two_menu_set_success(unsigned char temp);
extern unsigned char return_Two_menu_set_success(void);
#endif
