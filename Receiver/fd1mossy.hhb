#include "tm1629.h"
#include "timer.h"
#include "usart.h"
#include "ds1302.h"
#include "key.h"

unsigned char const CODE[] = { 0x3F, 0x06, 0x5B, 0x4F, 0x66, 0x6D, 0x7D, 0x07, 0x7F, 0x6F, 0x77, 0x7C, 0x39, 0x5E, 0x79, 0x71, 0x76, 0x38, 0x5c, 0x73, 0x3e };//0-9 abcdef 显示器码数组
unsigned char const INIT_CODE[] = { 0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80 };//逐段点亮数码管数组
unsigned char const SHANGSHUO[] = { 0x40, 0x00 }; // 点亮数码管中间段以及灭
unsigned char buf_display[6][8] = { 0 }; //3个TM1629显存数组
unsigned char display_ram[240] = { 0 }; //程序运行时记录显示数据的内存 
unsigned char await_time_table= 0 ;//用于记录待机显示横杠数码管次数 

void writeDataTo1629(unsigned char p) //写数据给第一个TM1629
{
	unsigned int i;
	TM1629_STB = 0;
	for (i = 0; i<8; i++)
	{
		TM1629_CLK = 0;
		if (p & 0x01)
			TM1629_DIO = 1;
		else
			TM1629_DIO = 0;
		_nop_();
		_nop_();
		_nop_();
		_nop_();
		TM1629_CLK = 1;
		p = p >> 1;
	}
	TM1629_CLK = 0;
	TM1629_DIO = 0;
}

void writeDataTo1629_2(unsigned char p) //写数据给第二个TM1629
{
	unsigned int i;
	TM1629_STB2 = 0;
	for (i = 0; i<8; i++)
	{
		TM1629_CLK = 0;
		if (p & 0x01)
			TM1629_DIO = 1;
		else
			TM1629_DIO = 0;
		_nop_();
		_nop_();
		_nop_();
		_nop_();
		TM1629_CLK = 1;
		p = p >> 1;
	}
	TM1629_CLK = 0;
	TM1629_DIO = 0;
}

void writeDataTo1629_3(unsigned char p) //写数据给第三个TM1629
{
	unsigned int i;
	TM1629_STB3 = 0;
	for (i = 0; i<8; i++)
	{
		TM1629_CLK = 0;
		if (p & 0x01)
			TM1629_DIO = 1;
		else
			TM1629_DIO = 0;
		_nop_();
		_nop_();
		_nop_();
		_nop_();
		TM1629_CLK = 1;
		p = p >> 1;
	}
	TM1629_CLK = 0;
	TM1629_DIO = 0;
}

void send_command(unsigned char word) //写命令给第一个TM1629
{
	TM1629_STB = 1;
	nop;
	TM1629_STB = 0;
	writeDataTo1629(word);
}

void send_command_2(unsigned char word) //写命令给第二个TM1629
{
	TM1629_STB2 = 1;
	nop;
	TM1629_STB2 = 0;
	writeDataTo1629_2(word);
}

void send_command_3(unsigned char word) //写命令给第三个TM1629
{
	TM1629_STB3 = 1;
	nop;
	TM1629_STB3 = 0;
	writeDataTo1629_3(word);
}

void display(void) //3个TM1629显示函数
{
	unsigned char i;
	send_command(0x40);	//设置数据命令:普通模式、地址自增1，写数据到显存
	send_command(0xc0);	//设置显示地址命令：从00H开始
	for (i = 0; i<8; i++)	//发送16字节的显存数据
	{
		writeDataTo1629(buf_display[0][i]);
		writeDataTo1629(buf_display[1][i]);
	}
	send_command(0x8C);	//设置显示控制命令：打开显示，并设置为11/16.
	TM1629_STB = 1;

	send_command_2(0x40);	//设置数据命令:普通模式、地址自增1，写数据到显存
	send_command_2(0xc0);	//设置显示地址命令：从00H开始
	for (i = 0; i<8; i++)	//发送16字节的显存数据
	{
		writeDataTo1629_2(buf_display[2][i]);
		writeDataTo1629_2(buf_display[3][i]);
	}
	send_command_2(0x8C);	//设置显示控制命令：打开显示，并设置为11/16.
	TM1629_STB2 = 1;

	send_command_3(0x40);	//设置数据命令:普通模式、地址自增1，写数据到显存
	send_command_3(0xc0);	//设置显示地址命令：从00H开始
	for (i = 0; i<8; i++)	//发送16字节的显存数据
	{
		writeDataTo1629_3(buf_display[4][i]);
		writeDataTo1629_3(buf_display[5][i]);
	}
	send_command_3(0x8C);	//设置显示控制命令：打开显示，并设置为11/16.
	TM1629_STB3 = 1;
}

void tm1629_init(void) //TM1629开机初始化函数
{
	unsigned char i, j, k;		//k控制显示的具体数字，i和j控制buf_display的刷新
	for (k = 0; k<8; k++)
	{
		for (i = 0; i<6; i++)
		{
			for (j = 0; j<8; j++)
			{
				buf_display[i][j] = INIT_CODE[k];
			}
		}
		display();
		Tm1629_delay(30);
	}
}


void Tm1629_delay(unsigned char k) //延时函数
{
	unsigned char i, j;
	for (; k>0; k--)
	{
		for (i = 255; i>0; i--)
		{
			for (j = 255; j>0; j--)
			{
				;

			}
		}
	}
}

void tm1629_clear(void)//全部归零
{
	unsigned char i, j;
	for (i = 0; i<6; i++)
	{
		for (j = 0; j<8; j++)
		{
			buf_display[i][j] = 0x00;
		}
	}
}

void tm1629_await(void)
{
	unsigned char i;	//k控制显示的具体数字，i和j控制buf_display的刷新
	unsigned char await_number_table_temp = 0;
	await_number_table_temp = return_await_number_table();
	if (await_number_table_temp == 1)
	{
		//GD5800_select_chapter(0x0008)	;
		tm1629_clear();
		i = await_time_table & 0x03;
		buf_display[0][i] = 0x40;
		display();
		set_await_number_table(0);
		await_time_table++;
		if (await_time_table == 4)
			await_time_table = 0;
	}
}

void Display_time(void)
{
	tm1629_clear();
	Ds1302_Read_Time(); 
	buf_display[1][7] = CODE[2];				//年
	buf_display[1][6] = CODE[0];
	buf_display[1][5] = CODE[time_buf1[1] / 10];
	buf_display[1][4] = CODE[time_buf1[1] % 10];

	buf_display[1][3] = 0x40;					 // -
	
	buf_display[1][2] = CODE[time_buf1[2] / 10]; //月
	buf_display[1][1] = CODE[time_buf1[2] % 10];

	buf_display[1][0] = 0x40;					 // -

	buf_display[0][7] = CODE[time_buf1[3] / 10];
	buf_display[0][6] = CODE[time_buf1[3] % 10];//日

	buf_display[0][5] = CODE[time_buf1[7]];		//星期

	buf_display[0][3] = CODE[time_buf1[4] / 10]; //小时
	buf_display[0][2] = CODE[time_buf1[4] % 10];
	buf_display[0][1] = CODE[time_buf1[5] / 10]; //小时
	buf_display[0][0] = CODE[time_buf1[5] % 10];

	display();

}

void tm1629_f(unsigned char f_number) //第一行倒数二个显示F,倒数第一个显示f_number
{
	tm1629_clear();
	buf_display[0][1] = CODE[0x0f];
	buf_display[0][0] = CODE[f_number];
	display();
}

void tm1629_E(unsigned char f_number) //第一行倒数二个显示F,倒数第一个显示f_number
{
	tm1629_clear();
	buf_display[0][1] = CODE[0x0E];
	buf_display[0][0] = CODE[f_number];
	display();
}

void Show_one_number(unsigned char f_number) //显示一个数字
{
	tm1629_clear();
	//buf_display[0][1] = CODE[0x0E];
	buf_display[0][0] = CODE[f_number];
	display();
}

void Show_two_number(unsigned char f_number) //显示两个数字
{
	tm1629_clear();
	buf_display[0][1] = CODE[f_number / 10];
	buf_display[0][0] = CODE[f_number % 10];
	display();
}

void Show_four_number(unsigned char* temp)
{
	tm1629_clear();
	buf_display[0][3] = CODE[*(temp)];
	buf_display[0][2] = CODE[*(temp+1)];
	buf_display[0][1] = CODE[*(temp+2)];
	buf_display[0][0] = CODE[*(temp+3)];
	display();
}

void tm1629_load(void)
{
	tm1629_clear();
	if (*(display_ram + 66) != 0)
		mcuram_to_displayram(buf_display[2] + 15, display_ram + 66);
	if (*(display_ram + 60) != 0)
		mcuram_to_displayram(buf_display[2] + 11, display_ram + 60);
	if (*(display_ram + 54) != 0)
		mcuram_to_displayram(buf_display[2] + 7, display_ram + 54);
	if (*(display_ram + 48) != 0)
		mcuram_to_displayram(buf_display[2] + 3, display_ram + 48);
	if (*(display_ram + 42) != 0)
		mcuram_to_displayram(buf_display[1] + 15, display_ram + 42);
	if (*(display_ram + 36) != 0)
		mcuram_to_displayram(buf_display[1] + 11, display_ram + 36);
	if (*(display_ram + 30) != 0)
		mcuram_to_displayram(buf_display[1] + 7, display_ram + 30);
	if (*(display_ram + 24) != 0)
		mcuram_to_displayram(buf_display[1] + 3, display_ram + 24);
	if (*(display_ram + 18) != 0)
		mcuram_to_displayram(buf_display[0] + 15, display_ram + 18);
	if (*(display_ram + 12) != 0)
		mcuram_to_displayram(buf_display[0] + 11, display_ram + 12);
	if (*(display_ram + 6) != 0)
		mcuram_to_displayram(buf_display[0] + 7, display_ram + 6);
	if (*(display_ram + 0) != 0)
		mcuram_to_displayram(buf_display[0] + 3, display_ram);
}

void mcuram_to_displayram(unsigned char a[48], unsigned char* b)  //从RAM区域移到显存区域,顺带翻译成数码管显示的码
{
	/*
	A// 类别、区号、接收号
	B// 类别、区号、接收号
	*/
	*(a) = CODE[*(b + 1)];
	*(a - 1) = CODE[*(b + 2)];
	*(a - 2) = CODE[*(b + 3)];
	*(a - 3) = CODE[*(b + 4)];
}

void decoder_temp_to_mcuram(unsigned char* a, unsigned char* index)//a为MCU缓存区   index为解码后取出的8字节临时数组
{
	*(a + 0) = *(index);
	*(a + 1) = *(index + 1);
	*(a + 2) = *(index + 2);
	*(a + 3) = *(index + 3);
	*(a + 4) = *(index + 4);
	*(a + 5) = *(index + 5);
}

void mcuram_to_mcuram_down(unsigned char* a) //a为MCURAM缓存区区   向下压一组数据
{
	*(a + 6) = *(a);		//ram区元素下移6个
	*(a + 7) = *(a + 1);
	*(a + 8) = *(a + 2);
	*(a + 9) = *(a + 3);
	*(a + 10) = *(a + 4);
	*(a + 11) = *(a + 5);

}

void mcuram_to_mcuram_up(unsigned char* a) //a为MCURAM缓存区区   向上压一组数据
{
	*(a) = *(a + 6);   //ram区元素上移6个
	*(a + 1) = *(a + 7);
	*(a + 2) = *(a + 8);
	*(a + 3) = *(a + 9);
	*(a + 4) = *(a + 10);
	*(a + 5) = *(a + 11);
}



void fun0(void) //待机显示函数
{
	//tm1629_await();
	Display_time();
}

void fun1(void) //一级菜单F0
{
	tm1629_f(0x00);
}

void fun2(void) //一级菜单F1
{
	tm1629_f(0x01);
}

void fun3(void) //一级菜单F2
{
	tm1629_f(0x02);
}

void fun4(void) //一级菜单F3
{
	tm1629_f(0x03);
}

void fun5(void) //一级菜单F4
{
	tm1629_f(0x04);
}

void fun6(void) //一级菜单F5
{
	tm1629_f(0x05);
}

void fun7(void) //一级菜单F6
{
	tm1629_f(0x06);
}

void fun8(void) //一级菜单F7
{
	tm1629_f(0x07);
}

void fun9(void) //一级菜单F8
{
	tm1629_f(0x08);
}

void fun10(void) //一级菜单F9
{
	tm1629_f(0x09);
}

void fun11(void) //一级菜单FA
{
	tm1629_f(0x0a);
}

void fun12(void) //一级菜单Fb
{
	tm1629_f(0x0b);
}

void fun13(void) //一级菜单FC
{
	tm1629_f(0x0c);
}

void fun14(void) //一级菜单Fd
{
	tm1629_f(0x0d);
}

void fun15(void) //设置年份
{
	if (return_await_number_table() == 1)
	{
		Display_time();
		set_await_number_table(2);
	}
	if (return_await_number_table() == 3)
	{
		buf_display[1][7] = 0;
		buf_display[1][6] = 0;
		buf_display[1][5] = 0;
		buf_display[1][4] = 0;
		display();
		set_await_number_table(0);
	}
	Ds1302_Write_Time();
}

void fun16(void) //设置月份
{
	if (return_await_number_table() == 1)
	{
		Display_time();
		set_await_number_table(2);
	}
	if (return_await_number_table() == 3)
	{
		buf_display[1][2] = 0;
		buf_display[1][1] = 0;
		display();
		set_await_number_table(0);
	}
	Ds1302_Write_Time();
}

void fun17(void) //设置日期
{
	if (return_await_number_table() == 1)
	{
		Display_time();
		set_await_number_table(2);
	}
	if (return_await_number_table() == 3)
	{
		buf_display[0][7] = 0;
		buf_display[0][6] = 0;
		display();
		set_await_number_table(0);
	}
	Ds1302_Write_Time();
}

void fun18(void) //设置星期
{
	if (return_await_number_table() == 1)
	{
		Display_time();
		set_await_number_table(2);
	}
	if (return_await_number_table() == 3)
	{
		buf_display[0][5] = 0;
		display();
		set_await_number_table(0);
	}
	Ds1302_Write_Time();
}

void fun19(void) //设置小时
{
	if (return_await_number_table() == 1)
	{
		Display_time();
		set_await_number_table(2);
	}
	if (return_await_number_table() == 3)
	{
		buf_display[0][3] = 0;
		buf_display[0][2] = 0;
		display();
		set_await_number_table(0);
	}
	Ds1302_Write_Time();
}

void fun20(void) //设置分钟
{
	if (return_await_number_table() == 1)
	{
		Display_time();
		set_await_number_table(2);
	}
	if (return_await_number_table() == 3)
	{
		buf_display[0][1] = 0;
		buf_display[0][0] = 0;
		display();
		set_await_number_table(0);
	}
	Ds1302_Write_Time();
}

void fun21(void) //二级菜单F1-E1
{
	tm1629_E(0x01);
	Two_Menu_F1_E1[0] = 0;
	Two_Menu_F1_E1[1] = 0;
	Two_Menu_F1_E1[2] = 0;
	Two_Menu_F1_E1[3] = 1;
}

void fun22(void) //二级菜单F1-E2
{
	tm1629_E(0x02);
	Two_Menu_F1_E2[0] = 0;
	Two_Menu_F1_E2[1] = 0;
	Two_Menu_F1_E2[2] = 0;
	Two_Menu_F1_E2[3] = 1;
}

void fun23(void) //二级菜单F1-E3
{
	tm1629_E(0x03);
	Two_Menu_F1_E3[0] = 0;
	Two_Menu_F1_E3[1] = 0;
	Two_Menu_F1_E3[2] = 0;
	Two_Menu_F1_E3[3] = 1;
}

void fun24(void) //二级菜单F1-E4
{
	tm1629_E(0x04);
	Two_Menu_F1_E4[0] = 0;
	Two_Menu_F1_E4[1] = 0;
	Two_Menu_F1_E4[2] = 0;
	Two_Menu_F1_E4[3] = 1;
}

void fun25(void) //二级菜单F2-E1
{
	tm1629_E(0x01);
	Two_Menu_F2_E1[0] = 0;
	Two_Menu_F2_E1[1] = 0;
	Two_Menu_F2_E1[2] = 0;
	Two_Menu_F2_E1[3] = 1;
}

void fun26(void) //二级菜单F2-E2
{
	tm1629_E(0x02);
	Two_Menu_F2_E2[0] = 0;
	Two_Menu_F2_E2[1] = 0;
	Two_Menu_F2_E2[2] = 0;
	Two_Menu_F2_E2[3] = 1;
}

void fun27(void) //二级菜单F2-E3
{
	tm1629_E(0x03);
	Two_Menu_F2_E3[0] = 0;
	Two_Menu_F2_E3[1] = 0;
	Two_Menu_F2_E3[2] = 0;
	Two_Menu_F2_E3[3] = 1;
}

void fun28(void) //二级菜单F2-E4
{
	tm1629_E(0x04);
	Two_Menu_F2_E4[0] = 0;
	Two_Menu_F2_E4[1] = 0;
	Two_Menu_F2_E4[2] = 0;
	Two_Menu_F2_E4[3] = 1;
}

void fun29(void) //二级菜单F3-E1
{
	tm1629_E(0x01);
}

void fun30(void) //二级菜单F3-E2
{
	tm1629_E(0x02);
}

void fun31(void) //二级菜单F4
{
	unsigned char temp = 0;
	temp = return_Two_Menu_F4_E1();
	Show_two_number(temp);
}

void fun32(void) //二级菜单F5
{
	unsigned char temp = 0;
	temp = return_Two_Menu_F5_E1();
	Show_two_number(temp);
}

void fun33(void) //二级菜单F6-E1
{
	tm1629_E(0x01);
}

void fun34(void) //二级菜单F6-E2
{
	tm1629_E(0x02);
}

void fun35(void) //二级菜单F6-E3
{
	tm1629_E(0x03);
}

void fun36(void) //二级菜单F6-E4
{
	tm1629_E(0x04);
}

void fun37(void) //二级菜单F6-E5
{
	tm1629_E(0x05);
}

void fun38(void) //二级菜单F7-E1
{
	tm1629_E(0x01);
}

void fun39(void) //二级菜单F7-E2
{
	tm1629_E(0x02);
}

void fun40(void) //二级菜单F7-E3
{
	tm1629_E(0x03);
}

void fun41(void) //二级菜单F7-E4
{
	tm1629_E(0x04);
}

void fun42(void) //二级菜单F8-E1
{
	tm1629_E(0x01);
}

void fun43(void) //二级菜单F8-E2
{
	tm1629_E(0x02);
}

void fun44(void) //二级菜单F9-E1
{
	tm1629_E(0x01);
}

void fun45(void) //二级菜单F9-E2
{
	tm1629_E(0x02);
}

void fun46(void) //二级菜单FA
{
	tm1629_f(0x0f);
}


void fun47(void) //二级菜单Fb
{
	tm1629_f(0x0f);
}

void fun48(void) //二级菜单FC
{
	tm1629_f(0x0f);
}

void fun49(void) //二级菜单Fd
{
	tm1629_f(0x0f);
}

void fun50(void) //F1_E1 千位闪烁
{
	if (return_await_number_table() == 1)
	{
		Show_four_number(Two_Menu_F1_E1);
		set_await_number_table(2);
	}
	if (return_await_number_table() == 3)
	{
		buf_display[0][3] = 0;
		display();
		set_await_number_table(0);
	}
}

void fun51(void) //F1_E1 百位闪烁
{
	if (return_await_number_table() == 1)
	{
		Show_four_number(Two_Menu_F1_E1);
		set_await_number_table(2);
	}
	if (return_await_number_table() == 3)
	{
		buf_display[0][2] = 0;
		display();
		set_await_number_table(0);
	}
}

void fun52(void)  //F1_E1 十位闪烁
{
	if (return_await_number_table() == 1)
	{
		Show_four_number(Two_Menu_F1_E1);
		set_await_number_table(2);
	}
	if (return_await_number_table() == 3)
	{
		buf_display[0][1] = 0;
		display();
		set_await_number_table(0);
	}
}

void fun53(void) //F1_E1 个位闪烁
{
	if (return_await_number_table() == 1)
	{
		Show_four_number(Two_Menu_F1_E1);
		set_await_number_table(2);
	}
	if (return_await_number_table() == 3)
	{
		buf_display[0][0] = 0;
		display();
		set_await_number_table(0);
	}
}

void fun54(void) //F1_E2 千位闪烁
{
	if (return_await_number_table() == 1)
	{
		Show_four_number(Two_Menu_F1_E2);
		set_await_number_table(2);
	}
	if (return_await_number_table() == 3)
	{
		buf_display[0][3] = 0;
		display();
		set_await_number_table(0);
	}
}

void fun55(void) //F1_E2 百位闪烁
{
	if (return_await_number_table() == 1)
	{
		Show_four_number(Two_Menu_F1_E2);
		set_await_number_table(2);
	}
	if (return_await_number_table() == 3)
	{
		buf_display[0][2] = 0;
		display();
		set_await_number_table(0);
	}
}

void fun56(void) //F1_E2 十位闪烁
{
	if (return_await_number_table() == 1)
	{
		Show_four_number(Two_Menu_F1_E2);
		set_await_number_table(2);
	}
	if (return_await_number_table() == 3)
	{
		buf_display[0][1] = 0;
		display();
		set_await_number_table(0);
	}
}

void fun57(void) //F1_E2 个位闪烁
{
	if (return_await_number_table() == 1)
	{
		Show_four_number(Two_Menu_F1_E2);
		set_await_number_table(2);
	}
	if (return_await_number_table() == 3)
	{
		buf_display[0][0] = 0;
		display();
		set_await_number_table(0);
	}
}

void fun58(void) //F1_E3 千位闪烁
{
	if (return_await_number_table() == 1)
	{
		Show_four_number(Two_Menu_F1_E3);
		set_await_number_table(2);
	}
	if (return_await_number_table() == 3)
	{
		buf_display[0][3] = 0;
		display();
		set_await_number_table(0);
	}
}

void fun59(void) //F1_E3 百位闪烁
{
	if (return_await_number_table() == 1)
	{
		Show_four_number(Two_Menu_F1_E3);
		set_await_number_table(2);
	}
	if (return_await_number_table() == 3)
	{
		buf_display[0][2] = 0;
		display();
		set_await_number_table(0);
	}
}

void fun60(void) //F1_E3 十位闪烁
{
	if (return_await_number_table() == 1)
	{
		Show_four_number(Two_Menu_F1_E3);
		set_await_number_table(2);
	}
	if (return_await_number_table() == 3)
	{
		buf_display[0][1] = 0;
		display();
		set_await_number_table(0);
	}
}

void fun61(void) //F1_E3 个位闪烁
{
	if (return_await_number_table() == 1)
	{
		Show_four_number(Two_Menu_F1_E3);
		set_await_number_table(2);
	}
	if (return_await_number_table() == 3)
	{
		buf_display[0][0] = 0;
		display();
		set_await_number_table(0);
	}
}

void fun62(void) //F1_E4 千位闪烁
{
	if (return_await_number_table() == 1)
	{
		Show_four_number(Two_Menu_F1_E4);
		set_await_number_table(2);
	}
	if (return_await_number_table() == 3)
	{
		buf_display[0][3] = 0;
		display();
		set_await_number_table(0);
	}
}

void fun63(void) //F1_E4 百位闪烁
{
	if (return_await_number_table() == 1)
	{
		Show_four_number(Two_Menu_F1_E4);
		set_await_number_table(2);
	}
	if (return_await_number_table() == 3)
	{
		buf_display[0][2] = 0;
		display();
		set_await_number_table(0);
	}
}

void fun64(void) //F1_E4 十位闪烁
{
	if (return_await_number_table() == 1)
	{
		Show_four_number(Two_Menu_F1_E4);
		set_await_number_table(2);
	}
	if (return_await_number_table() == 3)
	{
		buf_display[0][1] = 0;
		display();
		set_await_number_table(0);
	}
}

void fun65(void) //F1_E4 个位闪烁
{
	if (return_await_number_table() == 1)
	{
		Show_four_number(Two_Menu_F1_E4);
		set_await_number_table(2);
	}
	if (return_await_number_table() == 3)
	{
		buf_display[0][0] = 0;
		display();
		set_await_number_table(0);
	}
}

void fun66(void) //F2_E1 千位闪烁
{
	if (return_await_number_table() == 1)
	{
		Show_four_number(Two_Menu_F2_E1);
		set_await_number_table(2);
	}
	if (return_await_number_table() == 3)
	{
		buf_display[0][3] = 0;
		display();
		set_await_number_table(0);
	}
}

void fun67(void) //F2_E1 百位闪烁
{
	if (return_await_number_table() == 1)
	{
		Show_four_number(Two_Menu_F2_E1);
		set_await_number_table(2);
	}
	if (return_await_number_table() == 3)
	{
		buf_display[0][2] = 0;
		display();
		set_await_number_table(0);
	}
}

void fun68(void) //F2_E1 十位闪烁
{
	if (return_await_number_table() == 1)
	{
		Show_four_number(Two_Menu_F2_E1);
		set_await_number_table(2);
	}
	if (return_await_number_table() == 3)
	{
		buf_display[0][1] = 0;
		display();
		set_await_number_table(0);
	}
}

void fun69(void) //F2_E1 个位闪烁
{
	if (return_await_number_table() == 1)
	{
		Show_four_number(Two_Menu_F2_E1);
		set_await_number_table(2);
	}
	if (return_await_number_table() == 3)
	{
		buf_display[0][0] = 0;
		display();
		set_await_number_table(0);
	}
}

void fun70(void) //F2_E2 千位闪烁
{
	if (return_await_number_table() == 1)
	{
		Show_four_number(Two_Menu_F2_E2);
		set_await_number_table(2);
	}
	if (return_await_number_table() == 3)
	{
		buf_display[0][3] = 0;
		display();
		set_await_number_table(0);
	}
}

void fun71(void) //F2_E2 百位闪烁
{
	if (return_await_number_table() == 1)
	{
		Show_four_number(Two_Menu_F2_E2);
		set_await_number_table(2);
	}
	if (return_await_number_table() == 3)
	{
		buf_display[0][2] = 0;
		display();
		set_await_number_table(0);
	}
}

void fun72(void) //F2_E2 十位闪烁
{
	if (return_await_number_table() == 1)
	{
		Show_four_number(Two_Menu_F2_E2);
		set_await_number_table(2);
	}
	if (return_await_number_table() == 3)
	{
		buf_display[0][1] = 0;
		display();
		set_await_number_table(0);
	}
}

void fun73(void) //F2_E2 个位闪烁
{
	if (return_await_number_table() == 1)
	{
		Show_four_number(Two_Menu_F2_E2);
		set_await_number_table(2);
	}
	if (return_await_number_table() == 3)
	{
		buf_display[0][0] = 0;
		display();
		set_await_number_table(0);
	}
}

void fun74(void) //F2_E3 千位闪烁
{
	if (return_await_number_table() == 1)
	{
		Show_four_number(Two_Menu_F2_E3);
		set_await_number_table(2);
	}
	if (return_await_number_table() == 3)
	{
		buf_display[0][3] = 0;
		display();
		set_await_number_table(0);
	}
}

void fun75(void) //F2_E3 百位闪烁
{
	if (return_await_number_table() == 1)
	{
		Show_four_number(Two_Menu_F2_E3);
		set_await_number_table(2);
	}
	if (return_await_number_table() == 3)
	{
		buf_display[0][2] = 0;
		display();
		set_await_number_table(0);
	}
}

void fun76(void) //F2_E3 十位闪烁
{
	if (return_await_number_table() == 1)
	{
		Show_four_number(Two_Menu_F2_E3);
		set_await_number_table(2);
	}
	if (return_await_number_table() == 3)
	{
		buf_display[0][1] = 0;
		display();
		set_await_number_table(0);
	}
}

void fun77(void) //F2_E3 个位闪烁
{
	if (return_await_number_table() == 1)
	{
		Show_four_number(Two_Menu_F2_E3);
		set_await_number_table(2);
	}
	if (return_await_number_table() == 3)
	{
		buf_display[0][0] = 0;
		display();
		set_await_number_table(0);
	}
}

void fun78(void) //F2_E4 千位闪烁
{
	if (return_await_number_table() == 1)
	{
		Show_four_number(Two_Menu_F2_E4);
		set_await_number_table(2);
	}
	if (return_await_number_table() == 3)
	{
		buf_display[0][3] = 0;
		display();
		set_await_number_table(0);
	}
}

void fun79(void) //F2_E4 百位闪烁
{
	if (return_await_number_table() == 1)
	{
		Show_four_number(Two_Menu_F2_E4);
		set_await_number_table(2);
	}
	if (return_await_number_table() == 3)
	{
		buf_display[0][2] = 0;
		display();
		set_await_number_table(0);
	}
}

void fun80(void) //F2_E4 十位闪烁
{
	if (return_await_number_table() == 1)
	{
		Show_four_number(Two_Menu_F2_E4);
		set_await_number_table(2);
	}
	if (return_await_number_table() == 3)
	{
		buf_display[0][1] = 0;
		display();
		set_await_number_table(0);
	}
}

void fun81(void) //F2_E4 个位闪烁
{
	if (return_await_number_table() == 1)
	{
		Show_four_number(Two_Menu_F2_E4);
		set_await_number_table(2);
	}
	if (return_await_number_table() == 3)
	{
		buf_display[0][0] = 0;
		display();
		set_await_number_table(0);
	}
}

void fun82(void) //显示即时显示 还是循环显示
{
	unsigned char temp=0;
	temp = return_Two_Menu_F3_E1();
	Show_one_number(temp);
}

void fun83(void) //队列存储个数
{
	unsigned char temp = 0;
	temp = return_Two_Menu_F3_E2();
	Show_two_number(temp);
}

void fun84(void) //F6_E1语音选择
{
	unsigned char temp = 0;
	temp = return_Two_Menu_F6_E1();
	Show_one_number(temp);
}

void fun85(void) //F6_E2语音报读次数
{
	unsigned char temp = 0;
	temp = return_Two_Menu_F6_E2();
	Show_one_number(temp);
}

void fun86(void) //F6_E3循环时候是否报读
{
	unsigned char temp = 0;
	temp = return_Two_Menu_F6_E3();
	Show_one_number(temp);
}

void fun87(void) //F6_E4音量大小调整
{
	unsigned char temp = 0;
	temp = return_Two_Menu_F6_E4();
	Show_one_number(temp);
}

void fun88(void) //F6_E5屏幕亮度调整
{
	unsigned char temp = 0;
	temp = return_Two_Menu_F6_E5();
	Show_one_number(temp);
}

void fun89(void) //F7_E1 999*9
{
	unsigned char temp = 0;
	temp = return_Two_Menu_F7_E1();
	Show_one_number(temp);
}

void fun90(void) //F7_E2 9999*9
{
	unsigned char temp = 0;
	temp = return_Two_Menu_F7_E2();
	Show_one_number(temp);
}

void fun91(void) //F7_E3 999*99
{
	unsigned char temp = 0;
	temp = return_Two_Menu_F7_E3();
	Show_one_number(temp);
}

void fun92(void) //F7_E4 9999*99
{
	unsigned char temp = 0;
	temp = return_Two_Menu_F7_E4();
	Show_one_number(temp);
}

void fun93(void) //F8_E1单键位跟多键位切换
{
	unsigned char temp = 0;
	temp = return_Two_Menu_F8_E1();
	Show_one_number(temp);
}

void fun94(void) //F8_E2键位设置
{
	unsigned char temp = 0;
	temp = return_Two_Menu_F8_E2();
	Show_two_number(temp);
}

void fun95(void) //解码菜单
{
	tm1629_load();
	display();;
}