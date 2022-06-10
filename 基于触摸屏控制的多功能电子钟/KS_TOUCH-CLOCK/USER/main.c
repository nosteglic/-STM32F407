#include "includes.h"
#include "display.h"
#include "file.h"
#include "calendar.h"
#include "alarm.h"

vu8 system_task_return;		//����ǿ�Ʒ��ر�־.

extern RTC_TimeTypeDef RTC_TimeStruct;     //ʱ��ṹ�壺ʱ���֡��롢ampm
extern RTC_DateTypeDef RTC_DateStruct;			//���ڽṹ�壺�ꡢ�¡��ա�����

void system_init(){
	/******MALLOC*****/		    
	FSMC_SRAM_Init();				//��ʼ���ⲿSRAM  
	my_mem_init(SRAMIN);		//��ʼ���ڲ��ڴ��
	my_mem_init(SRAMEX);		//��ʼ���ⲿ�ڴ��
	my_mem_init(SRAMCCM);		//��ʼ��CCM�ڴ��
	/*******GUI******/
	piclib_init();					//piclib��ʼ��	  
	exfuns_init();					//FATFS �����ڴ�
	/******************************��ʼ��*********************************/
	delay_init(168);     		//��ʼ����ʱ����
	uart_init(115200);	 		//��ʼ�����ڲ�����Ϊ115200
	LED_Init();					 		//��ʼ��LED
	TPAD_Init(8);						//��ʼ����������,��84/4=21MhzƵ�ʼ���
	LCD_Init();							//LCD��ʼ��
	LCD_Clear(BLACK);				//��������
	gui_init();							//GUI�����ʼ��  
	tp_dev.init();					//��������ʼ��
	My_RTC_Init();		 			//��ʼ��RTC
	usmart_dev.init(84); 		//��ʼ��USMART
	Adc_Init();							//ADC��ʼ��
	BEEP_Init();						//��������ʼ��
	AT24CXX_Init(); 				//EEPROM��ʼ��
	WM8978_Init();					//WM8978��ʼ��
	WM8978_HPvol_Set(40,40);//������������
	WM8978_SPKvol_Set(50);	//������������;
	/**********************************************************************/
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//����ϵͳ�ж����ȼ�����2
	RTC_Set_WakeUp(RTC_WakeUpClock_CK_SPRE_16bits,0);		//����WAKE UP�ж�,1�����ж�һ��
	/*******************************����***********************************/
	font_init();						//���������ʼ��	
	W25QXX_Init();					//W25QXX��ʼ��
	f_mount(fs[1],"1:",1);	//����SPI FLASH
	/*******************************���������*****************************/
	font_set();							//������������
	/********************************AT24CXX���**********************************/
	AT24CXX_Check();				//���IIC�����Ƿ�����
	/*****************************����*****************************************/
	alarm_init();						//���ӳ�ʼ��
}

//������
int main(void){
	/**********����***********/
	u8 rval=HOLD_CLOCK_SETTING;
	u8 temp=0;
	/**********����***********/
	system_init();							//ϵͳ���ֳ�ʼ��
	while(1){
		rval=calendar_play(0);				//��ʾʱ�ӽ���
		printf("calendar_play:%d\n",rval);	//һ��printf��䶼��Ϊ�˵��Զ���
		if(rval==SWITCH_CLOCK_SETTING){		//�������ý���
			do{
				rval=setting_play();		//�˵�������
				printf("setting_play:%d\n",rval);
				BACK_COLOR=BLACK;			//����ɫΪ��ɫ
				LCD_Clear(BLACK);			//��������
				temp=rval;
				rval=calendar_play(2);//����ʱ��
				rval=temp;
			}while(rval==HOLD_CLOCK_SETTING);
			//����ʱ�����Ӳ���
			Alarm_FLAG=0;
			//�жϵ��������Ƿ���ڡ������������øõ������ӵ���һ��
			if(alarm_one_delay())
			{
				printf("\r\n------------------��һ�����õĵ������ӹ��ڣ����õ�������˳�ӵ���һ�죡-----------------\r\n");
				Alarm[alarm_one_id].state = 0X50;
				Alarm[alarm_one_id].weekmask = 1<<((RTC_DateStruct.RTC_WeekDay+1)%7);
				alarm_save(SYSTEM_Alarm_SAVE_BASE + alarm_one_id * sizeof(_alarm_obj),alarm_one_id);		//���浽ϵͳ�ڴ���
			}
		}
		alarm_ring();						//�ж������Ƿ�Ҫ��
	}
}

