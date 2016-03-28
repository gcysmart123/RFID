#ifndef __AT24C256_H_
#define __AT24C256_H_

#include "STC15F2K60S2.H"
#include "intrins.h"
#include "tpyedef.h"
#include "string.h"

#define  _Nop()  _nop_()        //定义空指令

sbit SDA = P3 ^ 4;            //模拟I2C数据传送位
sbit SCL = P3 ^ 5;            //模拟I2C时钟控制位

typedef struct Env_struct{
	unsigned char Two_Menu_F3_E1;     //及时模式或者排队模式
	unsigned char Two_Menu_F3_E2;     //呼叫存储数量
	unsigned char Two_Menu_F4_E1;     //销号时间
	unsigned char Two_Menu_F5_E1;     //循环时间
	unsigned char Two_Menu_F6_E1;    //语音模式
	unsigned char Two_Menu_F6_E2;      //语音报读次数
	unsigned char Two_Menu_F6_E3;   //循环时候是否报读
	unsigned char Two_Menu_F6_E4;  //音量大小调整
	unsigned char Two_Menu_F6_E5;  //显示屏LED亮度调整
	unsigned char Two_Menu_F6_E6;    //语音导航
	unsigned char Two_Menu_F6_E7;     //中英文
	unsigned char Two_Menu_F6_E8;     //闪烁
	unsigned char Two_Menu_F7_E1;     //防区设置
	unsigned char Two_Menu_F8_E1;     //单键位还是多键位
	unsigned char Two_Menu_Fb_E1;     //键盘销号
	unsigned char Two_Menu_FC_E1;     //时间切换

}Env_t;

//RF存储在EE中的位置为0X0000~0X1C00
typedef struct {
	uint8_t region[4];      //区号0~9~A~Z; //存放的ASCII码
	uint32_t rf;            //RF地址码，高8位为0表示此块已经被使用。其它为未使用 
}RF_def;


#define  _Nop()  _nop_()        //定义空指令
#define  I2C_ADDRESS 0xa0

#define  CALL_NUMBER   512	
#define  ALARM_NUMBER  64
#define  CANCEL_NUMBER 64
#define  HOST_NUMBER   32
#define  CALL_DATA_START   0X0020
#define  ALARM_DATA_START  0X1020
#define  CANCEL_DATA_START 0X1220
#define  HOST_DATA_START   0X1420
#define  SIN_KEY           0X1520
#define  MUL_KEY           0X1530
#define  BACK              0X1540

extern Env_t	EEPROM;

extern void DelayUs2x(unsigned char t);//函数声明 
extern void DelayMs(unsigned char t);
extern void Start_I2c();
extern void Stop_I2c();
extern void SendByte(unsigned char c);
extern unsigned char RcvByte();
extern bit ISendStr(unsigned char sla, unsigned int suba, unsigned char *s, unsigned char no);
extern bit IRcvStr(unsigned char sla, unsigned int suba, unsigned char *s, unsigned char no);
extern void Ack_I2c(void);
extern void NoAck_I2c(void);

extern bit register_call_function(RF_def *pRF);
extern bit delete_call_function(unsigned char *buf);
extern bit register_host_function(RF_def *pRF);
extern bit delete_host_function(unsigned char *buf);
extern bit register_alarm_function(RF_def *pRF);
extern bit delete_alarm_function(unsigned char *buf);
extern bit register_cancel_function(RF_def *pRF);
extern bit delete_cancel_function(unsigned char *buf);
extern void Delete_all_data(void);
extern int8_t Find_RF_EEPROM(RF_def *p, uint32_t dat);
extern int8_t Find_RF_EEPROM_Call(RF_def *p, uint32_t dat);
extern int8_t Find_RF_EEPROM_Host(RF_def *p, uint32_t dat);
extern int8_t Find_RF_EEPROM_Alarm(RF_def *p, uint32_t dat);
extern int8_t Find_RF_EEPROM_Cancel(RF_def *p, uint32_t dat);

#endif