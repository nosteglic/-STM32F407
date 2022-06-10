#include "alarm.h"
#include "includes.h"
#include "file.h"
#include "mp3play.h"
#include "mynotice.h"
//----------------------------------------------------------------
//                 alarm������
//----------------------------------------------------------------
//**********************************************����***********************************************************
//*****����calendar�ı���*****
extern RTC_TimeTypeDef RTC_TimeStruct;		//ʱ��ṹ�壺ʱ���֡��롢ampm
extern RTC_DateTypeDef RTC_DateStruct;		//���ڽṹ�壺�ꡢ�¡��ա�����
//*****ϵͳ��ַ����*****
u8 SYSTEM_Alarm_SAVE_BASE;					//���Ӵ洢��ʼ��ַ
u8 SYSTEM_Alarm_SAVE_END;					//���Ӵ洢������ַ

//*****alarm��ر���*****
_alarm_obj Alarm[RTC_Alarm_MAX_NUM];		//������ӽṹ��
u8 SYSTEM_Alarm_NUM;						//ϵͳ�е����Ӹ���
u8 wait_alarm[RTC_Alarm_MAX_NUM];			//�ȴ����ӵ�˳�����������е����Ϊ������
u8 ring_id;									//��ǰ���õ�Alarm_A�����Ӻ�
u8 Alarm_FLAG;								//��������ı�־
u8 alarm_one_id;							//��һ�����õĵ������ӵ�id(=0XFF:��ʾ���Ѿ������)
u8 is_first_alarm;							//�Ƿ��ǵ�һ����������
u8 is_sort;									//�Ƿ���Ҫ��������
//**********************************************����***********************************************************
//*******************************************************************
//��ʼ�����ӣ���ȡϵͳ�е�������Ϣ	
//�������ͣ�void
//ʹ�ò�������
//*******************************************************************
void alarm_init(void) 
{	
	u8 i;				//ѭ������
	u16 addr;		//ĳ��������ϵͳ�е���ʼ��ַ
	//��ʼ��ϵͳAT24CXX����
	alarm_system();
	//��ʼ����ȡ���ӵĵ�ַ/���Ӹ���
	SYSTEM_Alarm_SAVE_BASE = SYSTEM_PARA_SAVE_BASE +sizeof(SYSTEM_Alarm_SAVE_END);				//���Ӵ洢��ʼ��ַ
	SYSTEM_Alarm_SAVE_END = AT24CXX_ReadOneByte(SYSTEM_PARA_SAVE_BASE);//��ȡϵͳ�洢������ַ	
	//��ʼ�����ӵȴ���������
	for(i=0;i<RTC_Alarm_MAX_NUM;i++)
		wait_alarm[i] = i;
	//�����Ӵ洢�Ľ�����ַ����Ҫ��ʱ
	printf("\r\nAlarm_endaddr: %d\r\n",SYSTEM_Alarm_SAVE_END);
	if(SYSTEM_Alarm_SAVE_END >= 0X64)
	{
		SYSTEM_Alarm_NUM = (SYSTEM_Alarm_SAVE_END - SYSTEM_Alarm_SAVE_BASE) / sizeof(_alarm_obj);		//��ȡϵͳ�����е����Ӹ���
		//��ϵͳ���Ӹ����ȣ�������Ӹ�������ʱ
		if(SYSTEM_Alarm_NUM>RTC_Alarm_MAX_NUM) 
		{
			SYSTEM_Alarm_NUM = RTC_Alarm_MAX_NUM;		//�޸�ϵͳ���Ӹ���Ϊ������Ӹ���
			SYSTEM_Alarm_SAVE_END = RTC_Alarm_MAX_NUM * sizeof(_alarm_obj);	//�޸����Ӵ洢������ַ
			AT24CXX_WriteOneByte(SYSTEM_PARA_SAVE_BASE,SYSTEM_Alarm_SAVE_END);//����AT24CXX
		}
		//�����ʼ�������������ڲ���
//		alarm_Set(0,23,59,0X80,0XFE,0);				//����һ�����ӣ���״̬
//		alarm_Set(1,17,20,0X80,0XFE,0);		   	//����һ�����ӣ���״̬
//		alarm_Set(2,12,50,0X80,0XFE,0);	
//		alarm_Set(3,11,58,0X80,0XFE,0);	
//		alarm_Set(4,19,10,0X80,0XFE,0);	
		printf("\r\nAlarm_num: %d\r\n",SYSTEM_Alarm_NUM);//����������Ӹ���
		//��ϵͳ��������ʱ
		if (SYSTEM_Alarm_NUM > 0) 
		{
			//��ȡ������Ϣ��������
			for(i=0;i<SYSTEM_Alarm_NUM;i++)
			{
				addr=SYSTEM_Alarm_SAVE_BASE + i * sizeof(_alarm_obj);	//������ȡ�����ӵ���ʼ��ַ
				alarm_read(addr,i);		//��ȡ����
				//���ԣ����ϵͳ�����е�����
				printf("\r\nAlarm_AT24CXX[%d]:%d,%d,%d,%d,%d,\r\n",i,Alarm[i].hour,Alarm[i].min,Alarm[i].state,Alarm[i].weekmask,Alarm[i].saveflag);//���Դ�ϵͳ�ж�������������Ϣ
			}
			//���ӵ���
			alarm_schedule();
		}
		else
			printf("\r\n----------------------Ŀǰϵͳ�в�û���κ�������Ϣ�������������ý������ã�-----------------------\r\n");
	}
	else
		printf("\r\nAlarm��ʼ��ʧ��\r\n");
} 
//*******************************************************************
//���ӵ��ȣ����������������õ�RTC��Alarm_A��(ǰ�����Ѿ���������Ϣ)
//          ÿ��Ӧ���ڸ�����������֮����øú���
//�������ͣ�void
//ʹ�ò�������
//*******************************************************************
void alarm_schedule(void)
{
	u8 id;					//id��
	//��������:���� ����ɾ���� ����ʱ����
	if(is_sort == 1)
	{
		alarm_sort();
		is_sort =0;		//�޸ı�־λ
	}
	//�ж����������������һ����Ҫ������ӵ�id��
	id = alarm_compare();
	//���ǵ�һ������ʱ
	if((SYSTEM_Alarm_NUM==1)&(is_first_alarm==0))
	{
		if(Alarm[id].state == 0X80)			//����״̬
		{
			printf("\r\n����Alarm_A�����ã�Ҫ���õ������ǣ�[%d],ʱ��Ϊ��%d:%d\r\n",id,Alarm[id].hour,Alarm[id].min);
			RTC_Set_AlarmA(Alarm[id].weekmask,Alarm[id].hour,Alarm[id].min,0);
			alarm_one_id = 0XFF;					//����һ�����������Ѿ�����ˣ����ù���
		}
		else if(Alarm[id].state == 0X50)			//�������ǡ���������ʱ��
		{
			alarm_one_id = id;					//��¼�������ӵ�id��
			RTC_Set_AlarmA(Alarm[id].weekmask,Alarm[id].hour,Alarm[id].min,0);//��������ʱ�䵽RTC����������A��
			Alarm[id].state = 0X00;				//����һ��֮�����ӹر�
			Alarm[id].weekmask = 0X00;		//��������Ϊ 0 
			alarm_save(SYSTEM_Alarm_SAVE_BASE + id * sizeof(_alarm_obj),id);		//���浽ϵͳ�ڴ���
			printf("\r\n����Alarm_A�����ã�Ҫ���õ������ǣ�[%d],ʱ��Ϊ��%d:%d\r\n",id,Alarm[id].hour,Alarm[id].min);
		}
		is_first_alarm = 1;//��ִ�й�һ�Σ���ֹ�ظ�����
	}
	//���ǵ�һ������ʱ
	//����Ҫ���õ��������Ѿ���Alarm_A�е������ظ�ʱ������Ҫ��������
	if(id != ring_id)
	{
		//�������ǿ���״̬ʱ���������õ�Alarm_A�У��ȴ�����
		//printf("\r\nAlarm[%d]: %d\r\n",id,Alarm[id].state);//�����������״̬ 
		if(Alarm[id].state == 0X80)
		{	
			printf("\r\n����Alarm_A�����ã�Ҫ���õ������ǣ�[%d],ʱ��Ϊ��%d:%d\r\n",id,Alarm[id].hour,Alarm[id].min);
			RTC_Set_AlarmA(Alarm[id].weekmask,Alarm[id].hour,Alarm[id].min,0);
			alarm_one_id = 0XFF;					//����һ�����������Ѿ�����ˣ����ù���
		}
		else if(Alarm[id].state == 0X50)			//�������ǡ���������ʱ��
		{
			alarm_one_id = id;					//��¼�������ӵ�id��
			RTC_Set_AlarmA(Alarm[id].weekmask,Alarm[id].hour,Alarm[id].min,0);//��������ʱ�䵽RTC����������A��
			Alarm[id].state = 0X00;				//����һ��֮�����ӹر�
			Alarm[id].weekmask = 0X00;		//��������Ϊ 0    
			alarm_save(SYSTEM_Alarm_SAVE_BASE + id * sizeof(_alarm_obj),id);		//���浽ϵͳ�ڴ���
			printf("\r\n����Alarm_A�����ã�Ҫ���õ������ǣ�[%d],ʱ��Ϊ��%d:%d\r\n",id,Alarm[id].hour,Alarm[id].min);
		}
		ring_id = id;	//���µ�ǰ�����ӱ��
	}
}

//*******************************************************************
//�������򣺽����е����Ӱ�ʱ��������絽��
//�������ͣ�void
//ʹ�ò�������
//*******************************************************************
void alarm_sort(void)
{
	u8 i,j,temp;								//��ʱѭ������
	//����min
	for(i=0;i<SYSTEM_Alarm_NUM-1;i++)
	{
		for(j=0;j<SYSTEM_Alarm_NUM-1-i;j++)
		{
			if(Alarm[wait_alarm[j]].min > Alarm[wait_alarm[j+1]].min)
			{
				temp = wait_alarm[j];
				wait_alarm[j] = wait_alarm[j+1];
				wait_alarm[j+1] = temp;
			}	
		}
	}
	//����hour
	for(i=0;i<SYSTEM_Alarm_NUM-1;i++)
	{
		for(j=0;j<SYSTEM_Alarm_NUM-1-i;j++)
		{
			if(Alarm[wait_alarm[j]].hour > Alarm[wait_alarm[j+1]].hour)
			{
				temp = wait_alarm[j];
				wait_alarm[j] = wait_alarm[j+1];
				wait_alarm[j+1] = temp;
			}	
		}
	}
	//���ԣ��������Ľ��
	printf("\r\nϵͳ���Ѵ��ڵ����ӵ�˳��wait_alarm[%d]:\r",SYSTEM_Alarm_NUM);
	for(i=0;i<SYSTEM_Alarm_NUM;i++)
		printf("\r%d,\r",wait_alarm[i]+1);//�����������˳��
	printf("\r\n");
}

//*******************************************************************
//���ӱȽϣ���ǰTime�����ź����������Ϣ�Ƚϣ��õ���һ��Ҫ���������
//�������ͣ�u8:������һ�������õ����ӵ�id��
//ʹ�ò�������
//*******************************************************************
u8   alarm_compare(void)
{
	u8 i;			//��ʱ����
	//ѭ��������õ������뵱ǰʱ��Ƚϣ�
	for(i=0;i<SYSTEM_Alarm_NUM;i++)
	{
		if(RTC_TimeStruct.RTC_Hours == Alarm[wait_alarm[i]].hour)//ͬСʱ
		{
			if(RTC_TimeStruct.RTC_Minutes < Alarm[wait_alarm[i]].min)//�ȽϷ���
				return wait_alarm[i];
		}
		else if(RTC_TimeStruct.RTC_Hours < Alarm[wait_alarm[i]].hour)
			return wait_alarm[i];	
	}
	//ѭ��������û�У�˵��������û��������Ҫ����
	//����һ������Ӧ��Ϊ�ڶ����ʱ,����Ϊ��һ������
	return wait_alarm[0];
}
//*******************************************************************
//���ӳ�ʼ��������������Ϣ(����add/modify)
//�������ͣ�void
//ʹ�ò�����id����������ı��
//          hour,min,state,weekmask,saveflag:���ӽṹ�����Ϣ
//*******************************************************************
void alarm_Set(u8 id,u8 hour,u8 min,u8 state,u8 weekmask,u8 saveflag)
{
	//��ʾ����Ϊ����
	if( weekmask == 0)
	{
		state = 0X50;		//����Ϊ�������ӵ�״̬
	    weekmask = 1<<RTC_DateStruct.RTC_WeekDay;		//�޸���������
	}
	is_sort = 1;		//��Ҫ��������������
	//���浽������
	Alarm[id].hour = hour;
	Alarm[id].min = min;
	Alarm[id].state = state;
	Alarm[id].weekmask = weekmask;
	Alarm[id].saveflag = saveflag;
	printf("\r\nAlarm[%d]:%d,%d,%d,%d,%d,\r\n",id,Alarm[id].hour,Alarm[id].min,Alarm[id].state,Alarm[id].weekmask,Alarm[id].saveflag);//������õ�������Ϣ
	//���浽AT24CXX��
	alarm_save(SYSTEM_Alarm_SAVE_BASE + id * sizeof(_alarm_obj),id);
	//������µ�����ʱ���޸������Ϣ
	if(id == SYSTEM_Alarm_NUM)
	{		
		SYSTEM_Alarm_NUM+=1;			//�������������Ӹ���
		SYSTEM_Alarm_SAVE_END += sizeof(_alarm_obj);		//����ϵͳ���ӽ�����ַ
		AT24CXX_WriteOneByte( SYSTEM_PARA_SAVE_BASE,SYSTEM_Alarm_SAVE_END);//����AT24CXX
	}
}	
//*******************************************************************
//��ȡ������Ϣ:���ڴ��䵽�������ý�����ʾ
//�������ͣ�_alarm_obj *   (�ṹ��ָ��)
//ʹ�ò�����id����������ı��
//*******************************************************************
_alarm_obj alarm_Get(u8 id)
{
	return Alarm[id];
}
//*******************************************************************
//����ɾ��������ALarm[],AT24CXX,���Ӹ�������Ϣ
//�������ͣ�void
//ʹ�ò�����id����������ı��
//*******************************************************************
void alarm_delete(u8 id)
{
	u8 i;				//��ʱ����
	is_sort = 1;		//��Ҫ��������������
	//�޸�����
	for(i = id;i < SYSTEM_Alarm_NUM-1;i++)
	{	
//		Alarm[i].hour = Alarm[i+1].hour;
//		Alarm[i].min = Alarm[i+1].min;
//		Alarm[i].state = Alarm[i+1].state;
//		Alarm[i].weekmask = Alarm[i+1].weekmask;
//		Alarm[i].saveflag = Alarm[i+1].saveflag;	
		Alarm[i] = Alarm[i+1];
	}
	//�޸�ϵͳ�洢
	for(i = id;i< SYSTEM_Alarm_NUM-1;i++)
		alarm_save(SYSTEM_Alarm_SAVE_BASE + i * sizeof(_alarm_obj),i);  //�޸�ϵͳ�����ӵ���Ϣ
	//�޸����Ӵ洢������ַ/���Ӹ���
	SYSTEM_Alarm_NUM -= 1;			//�������������Ӹ���
	SYSTEM_Alarm_SAVE_END -= sizeof(_alarm_obj);		//����ϵͳ���ӽ�����ַ
	AT24CXX_WriteOneByte( SYSTEM_PARA_SAVE_BASE,SYSTEM_Alarm_SAVE_END);//����AT24CXX
	//���������Ϣ
	printf("\r\nɾ�����ӣ�Alarm[%d]---------\r\n",id);
	printf("\r\n-----------------��ʱϵͳ�л�ʣ��  %d  ���������£�----------------\r\n",SYSTEM_Alarm_NUM);
	for(i =0 ;i <SYSTEM_Alarm_NUM;i++)
		printf("\r\nAlarm[%d]:%d,%d,%d,%d,%d,\r\n",i,Alarm[i].hour,Alarm[i].min,Alarm[i].state,Alarm[i].weekmask,Alarm[i].saveflag);//������õ�������Ϣ;
}
//*******************************************************************
//����������
//�������ͣ�void
//ʹ�ò�������
//*******************************************************************
void alarm_ring(void)
{
	if(Alarm_FLAG == 1)
	{
		printf("\r\n���� �� �졿---------------------------------------------------------\r\n");
		mp3PlayerDemo(APP_MUSIC);
		Alarm_FLAG=0;
//		rval=gui_notice(0,0,1,"�ر�����","ȷ��");
//		if(rval==SWITCH_CLOCK_SETTING){
//			Alarm_FLAG =0;
//			BACK_COLOR=BLACK;			//����ɫΪ��ɫ
//			LCD_Clear(BLACK);			//��������
//			mp3PlayerDemo(APP_MUSIC,rval);
//		}
	}
}
//*******************************************************************
//��ȡAT24CXX���������Ϣ��������
//�������ͣ�void
//ʹ�ò�����id����������ı��
//          addr��д������ӵ�ϵͳ��ʼ��ַ
//*******************************************************************
void alarm_read(u16 addr,u8 id)
{
	AT24CXX_Read(addr,&Alarm[id].hour,1);//��ȡ������Ϣ��������
	AT24CXX_Read(addr+1,&Alarm[id].min,1);
	AT24CXX_Read(addr+2,&Alarm[id].state,1);
	AT24CXX_Read(addr+3,&Alarm[id].weekmask,1);
	AT24CXX_Read(addr+4,&Alarm[id].saveflag,1);
}
//*******************************************************************
//д��������Ϣ��AT24CXX��
//�������ͣ�void
//ʹ�ò�����id����������ı��
//          addr��д������ӵ�ϵͳ��ʼ��ַ
//*******************************************************************
void alarm_save(u16 addr,u8 id)
{
	Alarm[id].saveflag&=0X0A;								//��������Ϊ�ѱ���״̬
	AT24CXX_Write(addr,&Alarm[id].hour,1);	//д��������Ϣ��AT24CXX
	AT24CXX_Write(addr+1,&Alarm[id].min,1);
	AT24CXX_Write(addr+2,&Alarm[id].state,1);
	AT24CXX_Write(addr+3,&Alarm[id].weekmask,1);
	AT24CXX_Write(addr+4,&Alarm[id].saveflag,1);
} 
//*******************************************************************
//��ʼ��ϵͳ�е�������Ϣ(����AT24CXX�ĵ�254���ֽ�Ϊ��ʼ����־)
//�������ͣ�void
//ʹ�ò�������
//*******************************************************************
void alarm_system(void)
{
	u8 temp;			//��ʱ����
	u8 i;					//ѭ������
	u8 set_value=0X55;		//��254�ֽ�����ֵ
	temp=AT24CXX_ReadOneByte(254);//����ÿ�ο�����д���ӵĵ�ַ  
	if(temp==set_value) ;		   
	else//�ų���һ�γ�ʼ�������
	{
		for(i=99;i<250;i++)
			AT24CXX_WriteOneByte(i,0X00);			//���
		AT24CXX_WriteOneByte(254,set_value);	//��ʼ����־
		AT24CXX_WriteOneByte(99,0X64);	//��ʼ����Ľ�����ַ����ʼ��ַ��ͬ100
	  temp=AT24CXX_ReadOneByte(254);	  
	}
}

//*******************************************************************
//���ӱȽϣ����ӱȽϵõ��Ѵ��ڵ��������Ƿ�������ͬ�����Ӵ���
//�������ͣ�u8:����ֵΪ 1 ˵���Ѵ�����ͬ���������ظ����ø�����
//ʹ�ò�����hour:Сʱ��min:���ӡ�id����ǰ���õ�����ʱ��
//*******************************************************************
u8   alarm_equal(u8 id,u8 hour,u8 min)
{
	u8 i;			//��ʱ����
	for(i=0;i<SYSTEM_Alarm_NUM;i++)
	{
	  if(i != id)
	  {
		 if(Alarm[i].hour == hour)		//ͬСʱ
		 {
			if(Alarm[i].min == min)		//ͬ����
				return 1;								//������ͬ����
		 }
	  }
	}
	//ѭ�����е����Ӳ�������ͬ������
	return 0;
}

//*******************************************************************
//����������������ý�����Ч�����Ƴٸõ������ӵ���һ��
//�������ͣ�u8:����ֵΪ 1 ˵���Ѵ�����ͬ���������ظ����ø�����
//ʹ�ò�������
//*******************************************************************
u8   alarm_one_delay(void)
{
	//����һ�����������Ѿ������
	if(alarm_one_id == 0XFF)//�Ѿ�����һ��������
		return 0;
	//�жϸõ���������û�й���
	if(RTC_TimeStruct.RTC_Hours > Alarm[alarm_one_id].hour)
	{
		return 1;						//���ӹ�����
	}
	else if(RTC_TimeStruct.RTC_Hours == Alarm[alarm_one_id].hour)
	{
		if(RTC_TimeStruct.RTC_Minutes > Alarm[alarm_one_id].min)
			return 1;
		else if(RTC_TimeStruct.RTC_Minutes == Alarm[alarm_one_id].min)
			if(RTC_TimeStruct.RTC_Seconds >= 0)
				return 1;
	}
	//����û�й��ڣ����� 0
	return 0;
}
