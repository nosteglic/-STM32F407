#ifndef __CALENDAR_H
#define __CALENDAR_H
#include "sys.h"
#include "includes.h"
//----------------------------------------------------------------
//                 calendar��ʱ��
//---------------------------------------------------------------- 

//******�궨��*******   
#define	PI	3.1415926535897932384626433832795      //��ֵ����

//******���λ�ñ���******
static u16 TIME_TOPX=100;  	//ʱ��x����λ��
static u16 TIME_TOPY=500;		//ʱ��y����λ��
static u16 DATE_TOPX=50;  	//����x����λ��
static u16 DATE_TOPY=10;		//����y����λ��

//******�ӱ��̱���******
static u16 center_x=240;		//��������x����
static u16 center_y=300;		//��������y����
static u16 size=340;				//����ֱ����С
static u16 d=12;						//������Ȧ���

//******�¶ȱ���******
static u16 TEMP_TOPX=70;  	//�¶�x����λ��
static u16 TEMP_TOPY=600;		//�¶�y����λ��

//******��������******
void calendar_date_refresh(void);																											//������ʾ���ϵ�ʱ��
void calendar_circle_clock_drawpanel(u16 x,u16 y,u16 size,u16 d);											//������
void calendar_circle_clock_showtime(u16 x,u16 y,u16 size,u16 d,u8 hour,u8 min,u8 sec);//��ʾʱ��
u8 calendar_play(u8 tag);																															//����ʱ��
void temperature_show(void);																													//��ʾ�¶�		
void calendar_ring(u8 type);																													//���㱨ʱ

#endif












