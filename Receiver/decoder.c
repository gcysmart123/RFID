#include "decoder.h"
#include "ev1527.h"
#include "usart.h"
#include "at24c256.h"
#include "key.h"
#include "menu.h"
#include "tm1629.h"
#include "timer.h"
#include "gd5800.h"
#include "com.h"
#include "global.h"

unsigned char buf_eeprom[8] = { 0 };//д��EEPROM_buf
unsigned char Two_menu_set_success = 0;
uint32_t last_dat = 0;
uint8_t  decoder_num = 0;

#ifdef REPLAY_PATCH
/*sjz add 2015 11 30*/
re_save_paras rep_saved_parameter_when_dec;
unsigned char  repeat_times=0;
unsigned char  P24_status_saved=0;
/*sjz add 2015 11 30*/
#endif




void DecoderProcess(void)
{	
	RF_def tmp;
	RF_def RFtmp;
	uint32_t dat;
	int state = 0;
	unsigned char l, k ,index=0;
	unsigned char temp_buff[8];//��������ʱ����
#ifdef DEBUG
	unsigned char i=0;

#endif

	unsigned char func_index_temp = 0;		//������ʱ���������ڷŻ������ⲿ����
	unsigned char Two_Menu_F8_E1_temp = 0;	//������ʱ���������ڷŻ������ⲿ����
	unsigned char Two_Menu_F8_E2_temp = 0;	//������ʱ���������ڷŻ������ⲿ����
	unsigned char Two_Menu_F3_E1_temp = 0;	//������ʱ���������ڷŻ������ⲿ����	
	unsigned char Two_Menu_F3_E2_temp = 0;	//������ʱ���������ڷŻ������ⲿ����
	unsigned char Two_Menu_F7_E1_temp = 0;	//������ʱ���������ڷŻ������ⲿ����
	unsigned char Two_Menu_F6_E1_temp = 0;	//������ʱ���������ڷŻ������ⲿ����
	unsigned char Two_Menu_F6_E2_temp = 0;	//������ʱ���������ڷŻ������ⲿ����

	func_index_temp = return_func_index();	//��������Ҫ���ⲿ����
	Two_Menu_F8_E1_temp = return_Two_Menu_F8_E1();	//��������Ҫ���ⲿ����
	Two_Menu_F8_E2_temp = return_Two_Menu_F8_E2();	//��������Ҫ���ⲿ����
	Two_Menu_F3_E1_temp = return_Two_Menu_F3_E1();	//��������Ҫ���ⲿ����
	Two_Menu_F3_E2_temp = return_Two_Menu_F3_E2();	//��������Ҫ���ⲿ����
	index			    = return_Two_Menu_F3_E2();
	Two_Menu_F7_E1_temp = return_Two_Menu_F7_E1();	//��������Ҫ���ⲿ����
	Two_Menu_F6_E1_temp = return_Two_Menu_F6_E1();	//��������Ҫ���ⲿ����
	Two_Menu_F6_E2_temp = return_Two_Menu_F6_E2();	//��������Ҫ���ⲿ����

	receive_rf_decoder();	//���뺯��

	if (return_again_receive_rf_decoder_finished() == 1) //��־λ����1 ˵����2�μ�����ͨ��,���յ���Ч��
	{
#ifdef DEBUG1
		uart_printf("decoder_val is %02x %02x %02x .\r\n", (unsigned int)old2_RF_RECE_REG[0], (unsigned int)old2_RF_RECE_REG[1], (unsigned int)old2_RF_RECE_REG[2]); //���԰�����ֵ
#endif
		dat = ((uint32_t)old2_RF_RECE_REG[0]) << 16 | ((uint32_t)old2_RF_RECE_REG[1]) << 8 | ((uint32_t)old2_RF_RECE_REG[2]);
		switch (func_index_temp)
		{
		case MENU_STANDBY://����״̬��
		{
			set_logout_cycle_table(0);//ѭ�����������¼���
			//���̹���
		if ((old2_RF_RECE_REG[2] & 0xf0) == 0x00 && ((old2_RF_RECE_REG[0] >> 4) == Two_Menu_F7_E1_temp) && (old2_RF_RECE_REG[0] >> 4) < 10)//���̹��򣬳���Ĭ�ϵ�����
		{
				if (Two_Menu_F8_E1_temp == 1)  //Ϊ����ֵ
				{
					temp_buff[0] = single_key[old2_RF_RECE_REG[2] & 0x0f] ;
				}
				else
				{
					temp_buff[0] = multiple_key[old2_RF_RECE_REG[2] & 0x0f];
				}   
				temp_buff[1] = old2_RF_RECE_REG[0] >> 4;	//Ϊ������
				temp_buff[2] = old2_RF_RECE_REG[0] & 0x0f;	//Ϊ3λ�����һλ
				temp_buff[3] = old2_RF_RECE_REG[1] >> 4;	//Ϊ3λ����ڶ�λ
				temp_buff[4] = old2_RF_RECE_REG[1] & 0x0f;	//Ϊ3λ�������λ
				temp_buff[5] = old2_RF_RECE_REG[0];			//������,���Ҵ��1λID��
				temp_buff[6] = old2_RF_RECE_REG[1];			//������,���Ҵ��2λID��
				temp_buff[7] = old2_RF_RECE_REG[2];			//������,���Ҵ��3λID��

				if (temp_buff[0] != QUXIAO_1 && temp_buff[0])
				{

#ifdef REPLAY_PATCH/*sjz add 2015 11 30*/
//                              repeat_times = Two_Menu_F6_E2_temp;//��������Ҫ���ⲿ����
                              if(Two_Menu_F6_E2_temp > 1)  /*repeat mode*/
                             {
                             	 repeat_times = Two_Menu_F6_E2_temp;//��������Ҫ���ⲿ����
	                          rep_saved_parameter_when_dec.call_type = temp_buff[0];
	                          rep_saved_parameter_when_dec.key_value = old2_RF_RECE_REG[2] & 0x0f;
	                          rep_saved_parameter_when_dec.report_mode= Two_Menu_F6_E1_temp;
	                          for(l = 0; l < 8; l++)
	                          {
	                               rep_saved_parameter_when_dec.report_number[l]=*(temp_buff+l);
	                          }
                             }
                             else   /*single play mode*/
                             {
                                 submenuf6_1(Two_Menu_F6_E1_temp, temp_buff, temp_buff[0], old2_RF_RECE_REG[2] & 0x0f); 
                             }
/*sjz add end*/
#else
					for (l = 0; l < Two_Menu_F6_E2_temp; l++)
					{
					      /*�������������߼��̺�������������ս�־*/
						submenuf6_1(Two_Menu_F6_E1_temp, temp_buff, temp_buff[0], old2_RF_RECE_REG[2] & 0x0f); 
					}


#endif
				}
				else
				{
					Cancel_funtion(temp_buff,display_ram);//ȡ������
					mcu_to_computer(0x92, temp_buff, old2_RF_RECE_REG[2] & 0x0f);//��λ��
					tm1629_load();
					display();
					break;
				}		
				if (decoder_num < Two_Menu_F3_E2_temp)
					decoder_num++;
				tm1629_clear();//����
				mcu_to_computer(0x91, temp_buff, old2_RF_RECE_REG[2] & 0x0f);//��λ��
				decoder_temp_to_mcuram(display_ram, temp_buff);//����ʱ������������뵥Ƭ���ݴ����� 8�ֽ�ת6�ֽ�
				tm1629_load();//��Ƭ����������������������Դ�������
				display();//��ʾ�����
				set_func_index(DECODER_MENU);//��ʱ�������˵���Ϊ��һ�ν�����׼��
				//clear_again_receive_rf_decoder_finished();//���������ɱ�־λ

				break;
			}
			//������ע��,��������Ҫ�ĺ�����
#ifdef DEBUG
			uart_printf("dat is : %x %x \r\n", (unsigned int)(dat >> 16), (unsigned int)dat);
#endif
			if (state = Find_RF_EEPROM(&RFtmp, dat))
			{
				if (last_dat == dat)
				{
					if (return_second_filter_table() != 0)
					{
						goto standby;
					}
				
				}
				else
				{
					last_dat = dat;
					clear_second_filter_time();
					set_second_filter_table(1);
				}

#ifdef DEBUG
				uart_printf("Find_RF_EEPROM \r\n");
				uart_printf("state is : %x \r\n", (unsigned int)state);
#endif
				if (state == 1)
				{
					temp_buff[0] = QUXIAO_1;
				}
				else if (state == 2)
				{
					temp_buff[0] = BAOJING_1;
				}
				else
				{
					temp_buff[0] = old2_RF_RECE_REG[2] & 0x0f;
				}

#ifdef DEBUG
				uart_printf("temp[0] is : %x \r\n", (unsigned int)temp_buff[0]);
#endif
				memcpy(temp_buff+1, RFtmp.region, 4);
				temp_buff[7] = old2_RF_RECE_REG[2];

				if (temp_buff[0] < 50)
				{
#ifdef DEBUG
					uart_printf("temp_buff[0] < 50 \r\n");
#endif
					if (Two_Menu_F8_E1_temp == 1)  //Ϊ����ֵ
					{

						temp_buff[0] = single_key[old2_RF_RECE_REG[2] & 0x0f];
					}
					else
					{

						temp_buff[0] = multiple_key[old2_RF_RECE_REG[2] & 0x0f];
					}
				}

#ifdef DEBUG
				uart_printf("temp[0] is : %x \r\n", (unsigned int)temp_buff[0]);
#endif


#ifdef DEBUG
				uart_printf("cancen funtion \r\n");
#endif
				//��������
				if (temp_buff[0] != QUXIAO_1 && temp_buff[0])
				{
#ifdef DEBUG
					uart_printf("cancel funtion fault \r\n");
#endif


#ifdef REPLAY_PATCH/*sjz add 2015 11 30*/
//                              repeat_times = Two_Menu_F6_E2_temp;//��������Ҫ���ⲿ����
                              if(Two_Menu_F6_E2_temp > 1)  /*repeat mode*/
                             {
                             	repeat_times = Two_Menu_F6_E2_temp;//��������Ҫ���ⲿ����
	                          rep_saved_parameter_when_dec.call_type = temp_buff[0];
	                          rep_saved_parameter_when_dec.key_value = old2_RF_RECE_REG[2] & 0x0f;
	                          rep_saved_parameter_when_dec.report_mode= Two_Menu_F6_E1_temp;
	                          for(l = 0; l < 8; l++)
	                          {
	                               rep_saved_parameter_when_dec.report_number[l]=*(temp_buff+l);
	                          }
                             }
                             else   /*single play mode*/
                             {
                                 submenuf6_1(Two_Menu_F6_E1_temp, temp_buff, temp_buff[0], old2_RF_RECE_REG[2] & 0x0f); 
                             }
/*sjz add end*/
#else
					for (l = 0; l < Two_Menu_F6_E2_temp; l++)
					{
						submenuf6_1(Two_Menu_F6_E1_temp, temp_buff, temp_buff[0], old2_RF_RECE_REG[2] & 0x0f);
					}

#endif
				}
				else
				{
#ifdef DEBUG
					uart_printf("cancen funtion success \r\n");
#endif
					if (decoder_num < Two_Menu_F3_E2_temp)
						decoder_num++;
					mcu_to_computer(0x92, temp_buff, old2_RF_RECE_REG[2] & 0x0f);//��λ��
					Cancel_funtion(temp_buff, display_ram);//ȡ������
					tm1629_load();
					display();
					goto standby;
				}

				tm1629_clear();//����
				mcu_to_computer(0x91, temp_buff, old2_RF_RECE_REG[2] & 0x0f);//��λ��
				decoder_temp_to_mcuram(display_ram, temp_buff);//������ϵĻ�  ����ʱ������������뵥Ƭ���ݴ����� 8�ֽ�ת6�ֽ�
				tm1629_load();//��Ƭ����������������������Դ�������
				display();//��ʾ�����
#ifdef DEBUG
				uart_printf("decoder success!");
#endif

				set_func_index(DECODER_MENU);;//��ʱ�������˵���Ϊ��һ�ν�����׼��
				clear_again_receive_rf_decoder_finished();//���������ɱ�־λ
				goto standby;

			}
standby:
			 break;
		}

		case DECODER_MENU: //����˵���
		{
			set_logout_cycle_table(0);//ѭ�����������¼���
			 //���̹���
			if ((old2_RF_RECE_REG[2] & 0xf0) == 0x00 && ((old2_RF_RECE_REG[0] >> 4) == Two_Menu_F7_E1_temp) && (old2_RF_RECE_REG[0] >> 4) < 10)//���̹��򣬳���Ĭ�ϵ�����
			{
				if (Two_Menu_F8_E1_temp == 1)  //Ϊ����ֵ
				{
					temp_buff[0] = single_key[old2_RF_RECE_REG[2] & 0x0f];
				}
				else
				{
					temp_buff[0] = multiple_key[old2_RF_RECE_REG[2] & 0x0f];
				}
				temp_buff[1] = old2_RF_RECE_REG[0] >> 4;	//Ϊ������
				temp_buff[2] = old2_RF_RECE_REG[0] & 0x0f;	//Ϊ3λ�����һλ
				temp_buff[3] = old2_RF_RECE_REG[1] >> 4;	//Ϊ3λ����ڶ�λ
				temp_buff[4] = old2_RF_RECE_REG[1] & 0x0f;	//Ϊ3λ�������λ
				temp_buff[5] = old2_RF_RECE_REG[0];			//������,���Ҵ��һλID��
				temp_buff[6] = old2_RF_RECE_REG[1];			//������,���Ҵ�ڶ�λID��
				temp_buff[7] = old2_RF_RECE_REG[2];			//������,���Ҵ����λID��
				if (Two_Menu_F3_E1_temp == 1)				//Ϊ��ʱģʽ
				{	
					if (temp_buff[0] != QUXIAO_1 && temp_buff[0])
					{

#ifdef REPLAY_PATCH/*sjz add 2015 11 30*/
//                              repeat_times = Two_Menu_F6_E2_temp;//��������Ҫ���ⲿ����
                              if(Two_Menu_F6_E2_temp > 1)  /*repeat mode*/
                             {	
                             	repeat_times = Two_Menu_F6_E2_temp;//��������Ҫ���ⲿ����
	                          rep_saved_parameter_when_dec.call_type = temp_buff[0];
	                          rep_saved_parameter_when_dec.key_value = old2_RF_RECE_REG[2] & 0x0f;
	                          rep_saved_parameter_when_dec.report_mode= Two_Menu_F6_E1_temp;
	                          for(l = 0; l < 8; l++)
	                          {
	                               rep_saved_parameter_when_dec.report_number[l]=*(temp_buff+l);
	                          }
                             }
                             else   /*single play mode*/
                             {
                                 submenuf6_1(Two_Menu_F6_E1_temp, temp_buff, temp_buff[0], old2_RF_RECE_REG[2] & 0x0f); 
                             }
/*sjz add end*/
#else
						for (l = 0; l < Two_Menu_F6_E2_temp; l++)
						{
							submenuf6_1(Two_Menu_F6_E1_temp, temp_buff, temp_buff[0], old2_RF_RECE_REG[2] & 0x0f);
						}
#endif


					}
					else
					{
						mcu_to_computer(0x92, temp_buff, old2_RF_RECE_REG[2] & 0x0f);//��λ��
						Cancel_funtion(temp_buff, display_ram);//ȡ������
						tm1629_load();
						display();
						break;
					}
					if (decoder_num < Two_Menu_F3_E2_temp)
						decoder_num++;
					mcu_to_computer(0x91, temp_buff, old2_RF_RECE_REG[2] & 0x0f);//��λ��

/*sjz add a new number to the first postion and re display the all number by timely mode*/
					Search_funtion(temp_buff, display_ram);

					for (l = Two_Menu_F3_E2_temp; l>1; l--) //����������һ������
					{
						mcuram_to_mcuram_down(display_ram + ((l - 2)<<3));
					}
					for (l = 0; l<6; l++) //����һ��������0
					{
						display_ram[l] = 0;
					}
						
					decoder_temp_to_mcuram(display_ram, temp_buff); //������ϵĻ�  ����ʱ������������뵥Ƭ���ݴ����� 8�ֽ�ת6�ֽ�			
					tm1629_load();//��Ƭ����������������������Դ�������
					display();//��ʾ�����
/*sjz add a new number to the first postion and re display the all number by timely mode   end 20151221*/

				}
				else if (Two_Menu_F3_E1_temp == 2)//Ϊѭ��ģʽ
				{
					if (temp_buff[0] == QUXIAO_1 || temp_buff[0] == 0)
					{
						mcu_to_computer(0x92, temp_buff, old2_RF_RECE_REG[2] & 0x0f);//��λ��
						Cancel_funtion(temp_buff, display_ram);//ȡ������
						tm1629_load();
						display();
						break;
					}
					for (k = 0; k < Two_Menu_F3_E2_temp; k++)
					{
						if (display_ram[(k << 3) + 1] == temp_buff[1] && display_ram[(k << 3) + 2] == temp_buff[2] && display_ram[(k << 3) + 3] == temp_buff[3] && display_ram[(k << 3) + 4] == temp_buff[4])
						{
							index = k;
							decoder_temp_to_mcuram(display_ram + (k << 3), temp_buff);
							tm1629_load();//��Ƭ����������������������Դ�������
							display();//��ʾ�����
							break;
						}
					}

					if (index == Two_Menu_F3_E2_temp)
					{
						for (k = 0; k < Two_Menu_F3_E2_temp; k++)
						{
							if (display_ram[(k << 3)] == 0)//�ҳ�λ�ڶ��������Ǹ���
							{
								if (decoder_num < Two_Menu_F3_E2_temp)
									decoder_num++;
								mcu_to_computer(0x91, temp_buff, old2_RF_RECE_REG[2] & 0x0f);//��λ��
								decoder_temp_to_mcuram(display_ram + (k << 3), temp_buff);
								tm1629_load();//��Ƭ����������������������Դ�������
								display();//��ʾ�����
								break;

							}
						}
					}
				}
				//clear_again_receive_rf_decoder_finished();//���������ɱ�־λ
				break;
			}
			//������ע��,��������Ҫ�ĺ�����
			if (state = Find_RF_EEPROM(&RFtmp, dat))
			{
				if (last_dat == dat)
				{
					if (return_second_filter_table() != 0)
					{
						goto decoder;
					}

				}
				else
				{
					last_dat = dat;
					clear_second_filter_time();
					set_second_filter_table(1);
				}
#ifdef DEBUG
				uart_printf("Find_RF_EEPROM \r\n");
#endif
				if (state == 1)
				{
					temp_buff[0] = QUXIAO_1;
				}
				else if (state == 2)
				{
					temp_buff[0] = BAOJING_1;
				}
				else
				{
					temp_buff[0] = old2_RF_RECE_REG[2] & 0x0f;
				}

				memcpy(temp_buff + 1, RFtmp.region, 4);
				temp_buff[7] = old2_RF_RECE_REG[2];

				if (temp_buff[0] < 50)
				{
					if (Two_Menu_F8_E1_temp == 1)  //Ϊ����ֵ
					{

						temp_buff[0] = single_key[old2_RF_RECE_REG[2] & 0x0f];
					}
					else
					{

						temp_buff[0] = multiple_key[old2_RF_RECE_REG[2] & 0x0f];
					}
				}

				if (Two_Menu_F3_E1_temp == 1)//Ϊ��ʱģʽ
				{

					if (temp_buff[0] != QUXIAO_1 && temp_buff[0])
					{
#ifdef DEBUG
						uart_printf("cancen funtion fault \r\n");
#endif


#ifdef REPLAY_PATCH/*sjz add 2015 11 30*/
//                              repeat_times = Two_Menu_F6_E2_temp;//��������Ҫ���ⲿ����
                              if(Two_Menu_F6_E2_temp > 1)  /*repeat mode*/
                             {	repeat_times = Two_Menu_F6_E2_temp;//��������Ҫ���ⲿ����
	                          rep_saved_parameter_when_dec.call_type = temp_buff[0];
	                          rep_saved_parameter_when_dec.key_value = old2_RF_RECE_REG[2] & 0x0f;
	                          rep_saved_parameter_when_dec.report_mode= Two_Menu_F6_E1_temp;
	                          for(l = 0; l < 8; l++)
	                          {
	                               rep_saved_parameter_when_dec.report_number[l]=*(temp_buff+l);
	                          }
                             }
                             else   /*single play mode*/
                             {
                                 submenuf6_1(Two_Menu_F6_E1_temp, temp_buff, temp_buff[0], old2_RF_RECE_REG[2] & 0x0f); 
                             }
/*sjz add end*/
#else
						for (l = 0; l < Two_Menu_F6_E2_temp; l++)
						{
							submenuf6_1(Two_Menu_F6_E1_temp, temp_buff, temp_buff[0], old2_RF_RECE_REG[2] & 0x0f);
						}
#endif
					}
					else
					{
#ifdef DEBUG
						uart_printf("cancen funtion success \r\n");
#endif
						mcu_to_computer(0x92, temp_buff, old2_RF_RECE_REG[2] & 0x0f);//��λ��
						Cancel_funtion(temp_buff, display_ram);//ȡ������
						tm1629_load();
						display();
						goto decoder;
					}
					if (decoder_num < Two_Menu_F3_E2_temp)
						decoder_num++;
					mcu_to_computer(0x91, temp_buff, old2_RF_RECE_REG[2] & 0x0f);//��λ��
					Search_funtion(temp_buff, display_ram);

					for (l = Two_Menu_F3_E2_temp; l > 1; l--) //����������һ������
					{
						mcuram_to_mcuram_down(display_ram + ((l - 2) << 3));
					}
					for (l = 0; l < 6; l++) //����һ��������0
					{
						display_ram[l] = 0;
					}

					decoder_temp_to_mcuram(display_ram, temp_buff); //������ϵĻ�  ����ʱ������������뵥Ƭ���ݴ����� 8�ֽ�ת6�ֽ�
					tm1629_load();//��Ƭ����������������������Դ�������
					display();//��ʾ�����
				}
				else if (Two_Menu_F3_E1_temp == 2)//Ϊѭ��ģʽ
				{
					if (temp_buff[0] == QUXIAO_1 || temp_buff[0] == 0)
					{
						mcu_to_computer(0x92, temp_buff, old2_RF_RECE_REG[2] & 0x0f);//��λ��
						Cancel_funtion(temp_buff, display_ram);//ȡ������
						tm1629_load();
						display();
						break;
					}

					for (k = 0; k < Two_Menu_F3_E2_temp; k++)
					{
						if (display_ram[(k << 3) + 1] == temp_buff[1] && display_ram[(k << 3) + 2] == temp_buff[2] && display_ram[(k << 3) + 3] == temp_buff[3] && display_ram[(k << 3) + 4] == temp_buff[4])
						{
							index = k;
							decoder_temp_to_mcuram(display_ram + (k << 3), temp_buff);
							tm1629_load();//��Ƭ����������������������Դ�������
							display();//��ʾ�����
							break;
						}
					}

					if (index == Two_Menu_F3_E2_temp)
					{
						for (k = 0; k < Two_Menu_F3_E2_temp; k++)
						{
							if (display_ram[(k << 3)] == 0)//�ҳ�λ�ڶ��������Ǹ���
							{
								if (decoder_num < Two_Menu_F3_E2_temp)
									decoder_num++;
								mcu_to_computer(0x91, temp_buff, old2_RF_RECE_REG[2] & 0x0f);//��λ��
								decoder_temp_to_mcuram(display_ram + (k << 3), temp_buff);
								tm1629_load();//��Ƭ����������������������Դ�������
								display();//��ʾ�����
								break;

							}
						}
					}
				}
#ifdef DEBUG
				uart_printf("decoder success!");
#endif
				clear_again_receive_rf_decoder_finished();
				goto decoder;

			}
				
			
decoder:
			break;
		}

		case TWO_MENU_F1_E1_D1:
		case TWO_MENU_F1_E1_D2:
		case TWO_MENU_F1_E1_D3:
		case TWO_MENU_F1_E1_D4:
		{
			set_func_index(TWO_MENU_F1_E1_D4);

			memcpy(tmp.region, Two_Menu_F1_E1, 4);
			tmp.rf = ((uint32_t)old2_RF_RECE_REG[0]) << 16 | ((uint32_t)old2_RF_RECE_REG[1]) << 8 | ((uint32_t)old2_RF_RECE_REG[2]);
			if (!(register_call_function(&tmp)))//��ֵ�Զ���1
			{
#ifdef DEBUG
				uart_printf("dat is : %x %x \r\n", (unsigned int)(tmp.rf >> 16), (unsigned int)tmp.rf);
				uart_printf("call is %02x %02x %02x %02x.\r\n", (unsigned int)Two_Menu_F1_E1[0], (unsigned int)Two_Menu_F1_E1[1], (unsigned int)Two_Menu_F1_E1[2], (unsigned int)Two_Menu_F1_E1[3]);
				uart_printf("caller is rigister success \r\n");
#endif					
				delay10ms();
				if (Two_Menu_F1_E1[3] == 9)
				{
					if (Two_Menu_F1_E1[2] == 9)
					{
						if (Two_Menu_F1_E1[1] == 9)
						{
							Two_Menu_F1_E1[0]++;
							Two_Menu_F1_E1[1] = 0;
							Two_Menu_F1_E1[2] = 0;
							if (Two_Menu_F1_E1[0] == 21)
							{
								Two_Menu_F1_E1[0] = 0;
								Two_Menu_F1_E1[3] = 1;
							}
							else
							{
								Two_Menu_F1_E1[3] = 1;
							}
						}
						else
						{
							Two_Menu_F1_E1[1]++;
							Two_Menu_F1_E1[2] = 0;
							Two_Menu_F1_E1[3] = 0;
						}
					}
					else
					{
						Two_Menu_F1_E1[2]++;
						Two_Menu_F1_E1[3] = 0;
					}
				}
				else
					Two_Menu_F1_E1[3]++;
			}
			break;
		}

		case TWO_MENU_F1_E2_D1:
		case TWO_MENU_F1_E2_D2:
		case TWO_MENU_F1_E2_D3:
		case TWO_MENU_F1_E2_D4:
		{
								  set_func_index(TWO_MENU_F1_E2_D4);
								  memcpy(tmp.region, Two_Menu_F1_E2, 4);
								  tmp.rf = ((uint32_t)old2_RF_RECE_REG[0]) << 16 | ((uint32_t)old2_RF_RECE_REG[1]) << 8 | ((uint32_t)old2_RF_RECE_REG[2]);

								  if (!register_host_function(&tmp))//��ֵ�Զ���1
								  {
#ifdef DEBUG
									  uart_printf("host is %02x %02x %02x .\r\n", (unsigned int)buf_eeprom[5], (unsigned int)buf_eeprom[6], (unsigned int)buf_eeprom[7]); //���԰�����ֵ
									  uart_printf("host is %02x %02x %02x %02x.\r\n", (unsigned int)Two_Menu_F1_E2[0], (unsigned int)Two_Menu_F1_E2[1], (unsigned int)Two_Menu_F1_E2[2], (unsigned int)Two_Menu_F1_E2[3]);
									  uart_printf("host is rigister success \r\n");
#endif					
									  delay10ms();
									  if (Two_Menu_F1_E2[3] == 9)
									  {
										  if (Two_Menu_F1_E2[2] == 9)
										  {
											  if (Two_Menu_F1_E2[1] == 9)
											  {
												  Two_Menu_F1_E2[0]++;
												  Two_Menu_F1_E2[1] = 0;
												  Two_Menu_F1_E2[2] = 0;
												  if (Two_Menu_F1_E2[0] == 21)
												  {
													  Two_Menu_F1_E2[0] = 0;
													  Two_Menu_F1_E2[3] = 1;
												  }
												  else
												  {
													  Two_Menu_F1_E2[3] = 1;
												  }


											  }
											  else
											  {
												  Two_Menu_F1_E2[1]++;
												  Two_Menu_F1_E2[2] = 0;
												  Two_Menu_F1_E2[3] = 0;
											  }
										  }
										  else
										  {
											  Two_Menu_F1_E2[2]++;
											  Two_Menu_F1_E2[3] = 0;
										  }
									  }
									  else
										  Two_Menu_F1_E2[3]++;
								  }
								  break;
		}

		case TWO_MENU_F1_E3_D1:
		case TWO_MENU_F1_E3_D2:
		case TWO_MENU_F1_E3_D3:
		case TWO_MENU_F1_E3_D4:
		{
			set_func_index(TWO_MENU_F1_E3_D4);

			memcpy(tmp.region, Two_Menu_F1_E3, 4);
			tmp.rf = ((uint32_t)old2_RF_RECE_REG[0]) << 16 | ((uint32_t)old2_RF_RECE_REG[1]) << 8 | ((uint32_t)old2_RF_RECE_REG[2]);

			if (!register_alarm_function(&tmp))//��ֵ�Զ���1
			{
#ifdef DEBUG
				uart_printf("alarm is %02x %02x %02x .\r\n", (unsigned int)buf_eeprom[5], (unsigned int)buf_eeprom[6], (unsigned int)buf_eeprom[7]); //���԰�����ֵ
				uart_printf("alarm is %02x %02x %02x %02x.\r\n", (unsigned int)Two_Menu_F1_E3[0], (unsigned int)Two_Menu_F1_E3[1], (unsigned int)Two_Menu_F1_E3[2], (unsigned int)Two_Menu_F1_E3[3]);
				uart_printf("alarm is rigister success \r\n");
#endif					
				delay10ms();
				if (Two_Menu_F1_E3[3] == 9)
				{
					if (Two_Menu_F1_E3[2] == 9)
					{
						if (Two_Menu_F1_E3[1] == 9)
						{
							Two_Menu_F1_E3[0]++;
							Two_Menu_F1_E3[1] = 0;
							Two_Menu_F1_E3[2] = 0;
							if (Two_Menu_F1_E3[0] == 21)
							{
								Two_Menu_F1_E3[0] = 0;
								Two_Menu_F1_E3[3] = 1;
							}
							else
							{
								Two_Menu_F1_E3[3] = 1;
							}
						}
						else
						{
							Two_Menu_F1_E3[1]++;
							Two_Menu_F1_E3[2] = 0;
							Two_Menu_F1_E3[3] = 0;
						}
					}
					else
					{
						Two_Menu_F1_E3[2]++;
						Two_Menu_F1_E3[3] = 0;
					}
				}
				else
					Two_Menu_F1_E3[3]++;
			}
			break;
		}
		case TWO_MENU_F1_E4_D1:
		case TWO_MENU_F1_E4_D2:
		case TWO_MENU_F1_E4_D3:
		case TWO_MENU_F1_E4_D4:
		{
			set_func_index(TWO_MENU_F1_E4_D4);

			memcpy(tmp.region, Two_Menu_F1_E4, 4);
			tmp.rf = ((uint32_t)old2_RF_RECE_REG[0]) << 16 | ((uint32_t)old2_RF_RECE_REG[1]) << 8 | ((uint32_t)old2_RF_RECE_REG[2]);


			if (!register_cancel_function(&tmp))//��ֵ�Զ���1
			{
#ifdef DEBUG
				uart_printf("cancel is %02x %02x %02x .\r\n", (unsigned int)buf_eeprom[5], (unsigned int)buf_eeprom[6], (unsigned int)buf_eeprom[7]); //���԰�����ֵ
				uart_printf("cancel is %02x %02x %02x %02x.\r\n", (unsigned int)Two_Menu_F1_E4[0], (unsigned int)Two_Menu_F1_E4[1], (unsigned int)Two_Menu_F1_E4[2], (unsigned int)Two_Menu_F1_E4[3]);
				uart_printf("cancel is rigister success \r\n");
#endif					
				delay10ms();
				if (Two_Menu_F1_E4[3] == 9)
				{
					if (Two_Menu_F1_E4[2] == 9)
					{
						if (Two_Menu_F1_E4[1] == 9)
						{
							Two_Menu_F1_E4[0]++;
							Two_Menu_F1_E4[1] = 0;
							Two_Menu_F1_E4[2] = 0;
							if (Two_Menu_F1_E4[0] == 21)
							{
								Two_Menu_F1_E4[0] = 0;
								Two_Menu_F1_E4[3] = 1;
							}
							else
							{
								Two_Menu_F1_E4[3] = 1;
							}
						}
						else
						{
							Two_Menu_F1_E4[1]++;
							Two_Menu_F1_E4[2] = 0;
							Two_Menu_F1_E4[3] = 0;
						}
					}
					else
					{
						Two_Menu_F1_E4[2]++;
						Two_Menu_F1_E4[3] = 0;
					}
				}
				else
					Two_Menu_F1_E4[3]++;
			}
			break;
		}
		case TWO_MENU_F8_E2_SET :
			set_sound_table(0);
			Two_menu_set_success = 1;
			if (Two_Menu_F8_E1_temp == 1)
			{
				single_key[old2_RF_RECE_REG[2] & 0x0f] = Two_Menu_F8_E2_temp;
				ISendStr(I2C_ADDRESS, SIN_KEY, single_key, 16);
			}
			else
			{
				multiple_key[old2_RF_RECE_REG[2] & 0x0f] = Two_Menu_F8_E2_temp;
				ISendStr(I2C_ADDRESS, MUL_KEY, multiple_key, 16);
			}
#ifdef DEBUG
			uart_printf("single_key��  ");
			for (i = 0; i < 16; i++)
			{
				
				uart_printf("%02d ", (unsigned int)single_key[i]);
			}
			uart_printf(" \r\n");

			uart_printf("multiple_key��");
			for (i = 0; i < 16; i++)
			{

				uart_printf("%2d ", (unsigned int)multiple_key[i]);
			}
			uart_printf(" \r\n");
#endif	

			break;


		default:break;//Ĭ�ϵ�break
		}
		clear_again_receive_rf_decoder_finished();//�����־λ
	}

	
    LogoutProcess();
    CycleProcess();
}

unsigned char return_Two_menu_set_success(void)
{
	unsigned char temp = 0;
	temp = Two_menu_set_success;
	return temp;
}

void set_Two_menu_set_success(unsigned char temp)
{
	Two_menu_set_success = temp;
}

#ifdef REPLAY_PATCH
void replay_process(void)
{
    unsigned char report_mode;
    unsigned char *report_number;
    unsigned int call_type;
    unsigned char key_value;
     report_mode= rep_saved_parameter_when_dec.report_mode;
     report_number=rep_saved_parameter_when_dec.report_number;
     call_type = rep_saved_parameter_when_dec.call_type;
     key_value = rep_saved_parameter_when_dec.key_value;

         if(P24 == 1) //5800 idle mode can play music
         {
             if(repeat_times>0)
 	     {
                submenuf6_1(report_mode, report_number, call_type,key_value); 
		  delaymulti10ms(10);
		  repeat_times--;
            }
    	 }
}
#endif
