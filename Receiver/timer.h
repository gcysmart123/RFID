#ifndef __TIMER_H_
#define __TIMER_H_

#include "STC15F2K60S2.H"
#define TIMER50MS 50000

extern void Init_Timer0(void);
extern void Init_Timer1(void);


extern unsigned char return_await_number_table(void);
extern void set_await_number_table(unsigned char temp);

extern void set_main_press_time_table(unsigned char temp);
extern unsigned char return_main_press_time(void);
extern void clear_main_press_time(void);\
extern void set_main_press_time(unsigned char temp);

extern void set_logout_cycle_table(unsigned char temp);
extern unsigned char return_logout_cycle_table(void);

extern void clear_return_standby_time(void);

extern unsigned char return_fd_table(void);
extern void set_fd_table(unsigned char temp);

extern unsigned char return_filter_main(void);
extern void set_filter_main(unsigned char temp);

extern unsigned char return_filter_other(void);
extern void set_filter_other(unsigned char temp);

extern void clear_again_and_again_time(void);
extern void clear_second_filter_time(void);
extern void set_second_filter_table(unsigned char temp); //设置second_filter_table变量的值
extern unsigned char return_second_filter_table(void);

#endif