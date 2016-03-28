#ifndef __TM1629_H_
#define __TM1629_H_


/*************************************************
this file will define all  TM1629 driver function
************************************************/
#include "STC15F2K60S2.h"
#include "intrins.h"
#include "global.h"
#ifdef COPY_TWO_MACHINE
#include "tpyedef.h"
#endif
sbit TM1629_STB = P0 ^ 2;
sbit TM1629_CLK = P0 ^ 0;
sbit TM1629_DIO = P0 ^ 1;
sbit TM1629_STB2 = P0 ^ 3;
sbit TM1629_STB3 = P5 ^ 4;

extern unsigned char display_ram[800];
extern unsigned char multiple_key[16];
extern unsigned char single_key[16];


#define nop _nop_();_nop_();_nop_();_nop_();_nop_();_nop_();

extern void writeDataTo1629(unsigned char p);
extern void writeDataTo1629_2(unsigned char p);
extern void writeDataTo1629_3(unsigned char p);
extern void send_command(unsigned char word);
extern void send_command_2(unsigned char word);
extern void send_command_3(unsigned char word);
extern void display(void);
extern void display_light(unsigned char temp); //3个TM1629显示函数


extern void tm1629_init(void);
extern void tm1629_await(void);
extern void tm1629_clear(void);
extern void tm1629_display_buff_clear(void);
extern void tm1629_load(void);
extern void mcuram_to_displayram(unsigned char a[48], unsigned char* b);
extern void decoder_temp_to_mcuram(unsigned char* a, unsigned char* index);
extern void mcuram_to_mcuram_down(unsigned char* a);
extern void mcuram_to_mcuram_up(unsigned char* a);

extern void tm1629_f(unsigned char f_number);
extern void tm1629_E(unsigned char f_number);
extern void Show_on(void);
extern void Show_off(void);
extern void Show_one_number(unsigned char f_number);
extern void Show_two_number(unsigned char f_number);
extern void Show_three_number(unsigned int f_number);
extern void Show_four_number(unsigned char* temp);

extern void fun0(void);
extern void fun1(void);
extern void fun2(void);
extern void fun3(void);
extern void fun4(void);
extern void fun5(void);
extern void fun6(void);
extern void fun7(void);
extern void fun8(void);
extern void fun9(void);
extern void fun10(void);
extern void fun11(void);
extern void fun12(void);
extern void fun13(void);
extern void fun14(void);
extern void fun15(void);
extern void fun16(void);
extern void fun17(void);
extern void fun18(void);
extern void fun19(void);
extern void fun20(void);
extern void fun21(void);
extern void fun22(void);
extern void fun23(void);
extern void fun24(void);
extern void fun25(void);
extern void fun26(void);
extern void fun27(void);
extern void fun28(void);
extern void fun29(void);
extern void fun30(void);
extern void fun31(void);
extern void fun32(void);
extern void fun33(void);
extern void fun34(void);
extern void fun35(void);
extern void fun36(void);
extern void fun37(void);
extern void fun38(void);
extern void fun39(void);
extern void fun40(void);
extern void fun41(void);
extern void fun42(void);
extern void fun43(void);
extern void fun44(void);
extern void fun45(void);
extern void fun46(void);
extern void fun47(void);
extern void fun48(void);
extern void fun49(void);
extern void fun50(void);
extern void fun51(void);
extern void fun52(void);
extern void fun53(void);
extern void fun54(void);
extern void fun55(void);
extern void fun56(void);
extern void fun57(void);
extern void fun58(void);
extern void fun59(void);
extern void fun60(void);
extern void fun61(void);
extern void fun62(void);
extern void fun63(void);
extern void fun64(void);
extern void fun65(void);
extern void fun66(void);
extern void fun67(void);
extern void fun68(void);
extern void fun69(void);
extern void fun70(void);
extern void fun71(void);
extern void fun72(void);
extern void fun73(void);
extern void fun74(void);
extern void fun75(void);
extern void fun76(void);
extern void fun77(void);
extern void fun78(void);
extern void fun79(void);
extern void fun80(void);
extern void fun81(void);
extern void fun82(void);
extern void fun83(void);
extern void fun84(void);
extern void fun85(void);
extern void fun86(void);
extern void fun87(void);
extern void fun88(void);
extern void fun89(void);
extern void fun90(void);
extern void fun91(void);
extern void fun92(void);
extern void fun93(void);
extern void fun94(void);
extern void fun95(void);
extern void fun96(void);
extern void fun97(void);
extern void fun98(void);
extern void fun99(void);
extern void fun100(void);
extern void fun101(void);
#ifdef COPY_TWO_MACHINE
extern void fun102(void);
extern void fun103(void);
extern void fun104(void);
extern void fun105(void);
extern void fun106(void);
#endif
extern void LogoutProcess(void);
extern void CycleProcess(void);
extern void Logout(void);
extern void CycleUp(void);
extern void CycleDown(void);
extern void Cancel_funtion(unsigned char* id_number, unsigned char* buff);
void Search_funtion(unsigned char* id_number, unsigned char* buff);//id_number为解码的temp数组，buff为显示缓存


void Tm1629_delay(unsigned char k); //延时函数
#ifdef COPY_TWO_MACHINE
extern uint8_t  Two_Menu_FE_status;
extern uint8_t  Two_Menu_FE_E2_table;
extern uint8_t  Two_Menu_FE_E2_data[10];
#endif
#endif

