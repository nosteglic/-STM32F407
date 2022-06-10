#ifndef __ALARM_H
#define __ALARM_H
#include "sys.h"
#include "rtc.h"
#include "calendar.h"
//----------------------------------------------------------------
//                 alarm������
//----------------------------------------------------------------

//******�궨��*******
#define RTC_Alarm_MAX_NUM  				8				//����������
#define SYSTEM_PARA_SAVE_BASE 		99			//ϵͳ�洢��ʼ��ַ

//*****ϵͳ��ַ����*****
extern u8 SYSTEM_Alarm_SAVE_BASE;								//���Ӵ洢��ʼ��ַ
extern u8 SYSTEM_Alarm_SAVE_END;									//���Ӵ洢������ַ

//*****alarm��ر���*****
extern u8 SYSTEM_Alarm_NUM;											//ϵͳ�е����Ӹ���
extern u8 wait_alarm[RTC_Alarm_MAX_NUM];					//�ȴ����ӵ�˳�����������е����Ϊ������
extern u8 ring_id;																//��ǰ���õ�Alarm_A�����Ӻ�
extern u8 Alarm_FLAG;														//��������ı�־
extern u8 is_first_alarm;												//�Ƿ��ǵ�һ����������
extern u8 alarm_one_id;													//��һ�����õĵ������ӵ�id(=0XFF:��ʾ���Ѿ������)

//******���ӽṹ��******					   
__packed typedef struct  
{			
	u8  hour;	   			//����Сʱ
	u8  min;				//�������		 
	u8  state;				//����״̬,0X80����״̬;������Ϊ�ر�״̬
 	u8  weekmask;			//������������ bit1~bit7,������һ~����.  
	u8  saveflag;			//�����־,0X0A,�������;����,����δ����	   
}_alarm_obj;


extern _alarm_obj Alarm[RTC_Alarm_MAX_NUM];
//******��������******
void alarm_init(void);					//��ʼ�����ӻ�����Ϣ
void alarm_schedule(void);				//���ӵ���
void alarm_sort(void);					//��������
u8   alarm_compare(void);				//���ӱȽ�
void alarm_Set(u8 id,u8 hour,u8 min,u8 state,u8 weekmask,u8 saveflag);			//����������Ϣ(��/��)
_alarm_obj alarm_Get(u8 id);			//��ȡ������Ϣ
void alarm_delete(u8 id);				//����ɾ��
void alarm_read(u16 addr,u8 id);		//��ȡ���Ӵ�AT24CXX��
void alarm_save(u16 addr,u8 id);		//�������ӵ�AT24CXX��
void alarm_ring(void);					//����������
void alarm_system(void);				//��ʼ��ϵͳ�е�������Ϣ
u8   alarm_equal(u8 id,u8 hour,u8 min);	//���ӱȽϵõ��Ѵ��ڵ��������Ƿ�������ͬ�����Ӵ���
u8   alarm_one_delay(void);				//����������������ý�����Ч�����Ƴٸõ������ӵ���һ��

#endif
