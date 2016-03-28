#include "gd5800.h"
#include "usart.h"
#include "menu.h"
#include "key.h"

void GD5800_initialization(void)
{
	/***********************/
	Usart2_SendData(0x7E);       /***frame command ****/
	Usart2_SendData(0X03);
	Usart2_SendData(0X33);
	Usart2_SendData(0x04);
	Usart2_SendData(0XEF);
	/***********************/
}

void GD5800_select_chapter(unsigned int chapter_number)
{
	Usart2_SendData(0x7E);       /***frame command ****/
	Usart2_SendData(0X04);
	Usart2_SendData(0X41);
	Usart2_SendData(chapter_number >> 8);
	Usart2_SendData(chapter_number);
	Usart2_SendData(0XEF);

}

void GD5800_sound_volume(unsigned char volume)
{
	Usart2_SendData(0x7E);       /***frame command ****/
	Usart2_SendData(0X03);
	Usart2_SendData(0X31);
	Usart2_SendData(volume);
	Usart2_SendData(0XEF);

}

void GD5800_stop_music(void)
{
	Usart2_SendData(0x7E);       /***frame command ****/
	Usart2_SendData(0X02);
	Usart2_SendData(0x0E);
	Usart2_SendData(0XEF);
}

void GD5800_play_music(void)
{
	Usart2_SendData(0x7E);       /***frame command ****/
	Usart2_SendData(0X02);
	Usart2_SendData(0x01);
	Usart2_SendData(0XEF);
}
#ifdef OLD_FUNTION
void submenuf6_1_voiceselect(unsigned char report_mode, unsigned char *report_number)
{
	unsigned char *number;
	number = report_number;
	if (report_mode <= NOT_REPORT_C)
	{
		if (report_mode <= NOT_REPORT)
		{
			GD5800_select_chapter(*(number + 2) + ZERO);/*呼叫号数百位*/
		}
		else
		{
			if (*(number + 2) != 0)
			{
				GD5800_select_chapter(*(number + 2) + ZERO);
				GD5800_select_chapter(HUNDRED);/*hundred 的声音*/
				GD5800_select_chapter(AND);/*and的声音*/
			}
		}

		if (report_mode <= NOT_REPORT)
		{
			GD5800_select_chapter(*(number + 3) + ZERO);
		}
		else
		{
			if (*(number + 3) != 0)
			{
				if (*(number + 3) == 1)
				{
					if (*(number + 4) == 0) //10
					{
						GD5800_select_chapter(TEN);
					}
					else//11-19
					{
						GD5800_select_chapter(*(number + 4) + ELEVEN - 1);
					}
				}
				else//20-90
				{
					GD5800_select_chapter(*(number + 3) + TWENTY - 2);
				}
			}
		}

		if (report_mode <= NOT_REPORT)
		{
			GD5800_select_chapter(*(number + 4) + ZERO);
		}
		else
		{
			if (*(number + 4) != 0 && *(number + 3) != 1)
			{
				GD5800_select_chapter(*(number + 4) + ZERO);

			}
		}
	}

	else
	{
		switch (report_mode)
		{
		case DINGDONG_type:
			GD5800_select_chapter(DINGDONG);
			break;
		case YINYUEYI_type:
			GD5800_select_chapter(YINGYUEYI);
			break;
		case YINYUEER_type:
			GD5800_select_chapter(YINGYUEER);
			break;
		case EMERGENCY_type:
			GD5800_select_chapter(BAOJING);
			break;
		default:break;
		}
	}
}

/*sjz********************************************************************************8****
void submenuf6_1(unsigned char report_mode,unsigned char *report_number,unsigned int call_type)
parameters:
report_mode: 0----simple report   1----complicated report  2-----do not need report type
3----"dingdong" voice  4-----"dingdang" voice  5 ----emergency voice
the pointer of report_number:   has three digital number,the range is from 0 to 999
and the pointer is from the array[1] to array[3]
call_type:   1----restaurant mode  2-----hospital mode
3----bank mode  etc
key_value:
case 0:cancel;
case 1:temp_buff[6]=0x0b;break;
case 2:temp_buff[6]=0x0d;break;
case 3:temp_buff[6]=0x0c;break;
case 5:temp_buff[6]=0x13;break;
case 6:temp_buff[6]=0x0e;break;
case 7:temp_buff[6]=0x0f;break;
*****************************************************************************************/

void submenuf6_1(unsigned char report_mode, unsigned char *report_number, unsigned int call_type, unsigned char key_value)
{
	unsigned int mode;
	unsigned char *number;
	unsigned int  type;
	unsigned char key;
	unsigned char Two_Menu_F7_E1_temp = 0;
	unsigned char Two_Menu_F6_E7_temp = 0;
	Two_Menu_F7_E1_temp = return_Two_Menu_F7_E1();
	Two_Menu_F6_E7_temp = return_Two_Menu_F6_E7();
	mode = report_mode;
	number = report_number;
	type = call_type;
	key = key_value;
	if (mode <= NOT_REPORT_C )
	{
		GD5800_select_chapter(DINGDONG);

		if (key == 15)//防区号是否当前缀
		{
			GD5800_select_chapter(QING);
		}

		if (Two_Menu_F6_E7_temp == 0 || (Two_Menu_F6_E7_temp == 1 && mode >= COMPLICATED_REPORT))
		{
			GD5800_select_chapter(HAO);
		}


		if (key != 15)//防区号是否当前缀
		{

			if (*(number + 1) <= 9)
			{
				if (report_mode <= NOT_REPORT)
				{
					GD5800_select_chapter(*(number + 1) + ZERO);
				}
				else
				{
					if (*(number + 1) != 0)
					{
						GD5800_select_chapter(*(number + 1) + ZERO);
						GD5800_select_chapter(THOUSAND);
					}
				}

			}
			else if (*(number + 1) <= 15 && *(number + 1) >= 10)
			{
				GD5800_select_chapter(*(number + 1) + ZIMU_A - 10);
			}
			else
			{
				switch (*(number + 1))
				{
				case 16:GD5800_select_chapter(ZIMU_H); break;//H
				case 17:GD5800_select_chapter(ZIMU_L); break;//L
				case 18:GD5800_select_chapter(ZIMU_O); break;//O
				case 19:GD5800_select_chapter(ZIMU_P); break;//P
				case 20:GD5800_select_chapter(ZIMU_U); break;//U
				default:break;
				}
			}
		}
					submenuf6_1_voiceselect(mode, number);

		if (Two_Menu_F6_E7_temp == 1 && report_mode <= NOT_REPORT)
		{
			GD5800_select_chapter(HAO);
		}

		if (key == 15)
		{
			//GD5800_select_chapter(DAO);
			GD5800_select_chapter(GUITAI);
		}
		else if (mode != NOT_REPORT && mode != NOT_REPORT_C)
		{
			if (type < 50)
			{
				GD5800_select_chapter(type + QUXIAO);
			}
			else if (type == BAOJING_1)
			{
				GD5800_select_chapter(BAOJING);
			}			
		}
	}
	else
	{
		if (type == BAOJING_1 || type == BAOJING-QUXIAO)
		{
			GD5800_select_chapter(BAOJING);
		}
		else
		{
			submenuf6_1_voiceselect(mode, number);
		}
	}

}
#endif

#ifdef  PRODUCT_6_32INCH
void submenuf6_1_voiceselect(unsigned char report_mode, unsigned char *report_number)
{
	unsigned char *number;
	number = report_number;
	if (report_mode <= NOT_REPORT_C)
	{
	/*
		if (report_mode <= NOT_REPORT)
		{
			GD5800_select_chapter(*(number + 2) + ZERO);//呼叫号数百位
		}
		else
		{
			if (*(number + 2) != 0)
			{
				GD5800_select_chapter(*(number + 2) + ZERO);
				GD5800_select_chapter(HUNDRED);//hundred 的声音
				GD5800_select_chapter(AND);//and的声音
			}
		}
*/
		if (report_mode <= NOT_REPORT)
		{
			GD5800_select_chapter(*(number + 3) + ZERO);
		}
		else
		{
			if (*(number + 3) != 0)
			{
				if (*(number + 3) == 1)
				{
					if (*(number + 4) == 0) //10
					{
						GD5800_select_chapter(TEN);
					}
					else//11-19
					{
						GD5800_select_chapter(*(number + 4) + ELEVEN - 1);
					}
				}
				else//20-90
				{
					GD5800_select_chapter(*(number + 3) + TWENTY - 2);
				}
			}
		}

		if (report_mode <= NOT_REPORT)
		{
			GD5800_select_chapter(*(number + 4) + ZERO);
		}
		else
		{
			if (*(number + 4) != 0 && *(number + 3) != 1)
			{
				GD5800_select_chapter(*(number + 4) + ZERO);

			}
		}
	}

	else
	{
		switch (report_mode)
		{
		case DINGDONG_type:
			GD5800_select_chapter(DINGDONG);
			break;
		case YINYUEYI_type:
			GD5800_select_chapter(YINGYUEYI);
			break;
		case YINYUEER_type:
			GD5800_select_chapter(YINGYUEER);
			break;
		case EMERGENCY_type:
			GD5800_select_chapter(BAOJING);
			break;
		default:break;
		}
	}
}
#endif

#ifdef  PRODUCT_6_32INCH
void submenuf6_1(unsigned char report_mode, unsigned char *report_number, unsigned int call_type, unsigned char key_value)
{
	unsigned int mode;
	unsigned char *number;
	unsigned int  type;
	unsigned char key;
	unsigned char Two_Menu_F7_E1_temp = 0;
	unsigned char Two_Menu_F6_E7_temp = 0;
	Two_Menu_F7_E1_temp = return_Two_Menu_F7_E1();
	Two_Menu_F6_E7_temp = return_Two_Menu_F6_E7();
	mode = report_mode;
	number = report_number;
	type = call_type;
	key = key_value;
	if (mode <= NOT_REPORT_C )
	{
		GD5800_select_chapter(DINGDONG);
//#ifdef  AWAIT_VOICE_PATCH
		if (key == 15)//防区号是否当前缀
		{
			GD5800_select_chapter(QING);
		}
//#endif
		if (Two_Menu_F6_E7_temp == 0 || (Two_Menu_F6_E7_temp == 1 && mode >= COMPLICATED_REPORT))
		{
			GD5800_select_chapter(HAO);
		}

//#ifdef  AWAIT_VOICE_PATCH
		if (key != 15)//防区号是否当前缀
		{
//#endif
			if (*(number + 2) <= 9)
			{
				if (report_mode <= NOT_REPORT)
				{
					GD5800_select_chapter(*(number + 2) + ZERO);
				}
				else
				{
					if (*(number + 2) != 0)
					{
						GD5800_select_chapter(*(number + 2) + ZERO);
						GD5800_select_chapter(HUNDRED);/*hundred 的声音*/
						GD5800_select_chapter(AND);/*and的声音*/
					}
				}

			}
			else if (*(number + 2) <= 15 && *(number + 2) >= 10)
			{
				GD5800_select_chapter(*(number + 2) + ZIMU_A - 10);
			}
			else
			{
				switch (*(number + 2))
				{
				case 16:GD5800_select_chapter(ZIMU_H); break;//H
				case 17:GD5800_select_chapter(ZIMU_L); break;//L
				case 18:GD5800_select_chapter(ZIMU_O); break;//O
				case 19:GD5800_select_chapter(ZIMU_P); break;//P
				case 20:GD5800_select_chapter(ZIMU_U); break;//U
				default:break;
				}
			}
//#ifdef  AWAIT_VOICE_PATCH
		}
//#endif

		submenuf6_1_voiceselect(mode, number);

		if (Two_Menu_F6_E7_temp == 1 && report_mode <= NOT_REPORT)
		{
			GD5800_select_chapter(HAO);
		}

		if (key == 15)
		{
			//GD5800_select_chapter(DAO);
			GD5800_select_chapter(GUITAI);
		}
		else if (mode != NOT_REPORT && mode != NOT_REPORT_C)
		{
			if (type < 50)
			{
				GD5800_select_chapter(type + QUXIAO);
			}
			else if (type == BAOJING_1)
			{
				GD5800_select_chapter(BAOJING);
			}		
		}
	}

	else
	{
		if (type == BAOJING_1 || type == BAOJING-QUXIAO)
		{
			GD5800_select_chapter(BAOJING);
		}
		else
		{
			submenuf6_1_voiceselect(mode, number);
		}
	}
	
}
#endif

void GD5800_select_chapter_new(unsigned int chapter_number)
{
tofun94:
	usart2_enable = 1;
	usart2_num = 0;
	usart2_right = 0;
	GD5800_select_chapter(chapter_number);
	delay10ms();
	delay10ms();
	delay10ms();
	delay10ms();
	if (usart2_right != 1)
		goto tofun94;
	set_sound_table(0);
}
