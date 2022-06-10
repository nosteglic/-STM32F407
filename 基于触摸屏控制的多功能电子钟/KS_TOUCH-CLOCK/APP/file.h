#ifndef __FILE_H
#define __FILE_H
#include "sys.h"
#include "includes.h"
//----------------------------------------------------------------
//                 file��SPI Flash�ļ���ȡ
//----------------------------------------------------------------
//*****����*****
extern u8*const APP_ASCII_S6030;	//����ܴ�����·��
extern u8*const APP_ASCII_5427;		//��ͨ������·��
extern u8*const APP_ASCII_3618;		//��ͨ������·��
extern u8*const APP_ASCII_2814;		//��ͨ������·��

extern u8* asc2_s6030;				//���������60*30���������
extern u8* asc2_5427;				//��ͨ����54*27���������
extern u8* asc2_3618;				//��ͨ����36*18���������
extern u8* asc2_2814;				//��ͨ����28*14���������

extern u8*const APP_MUSIC;		//�����ļ���·��

//*****��������*****
u8 font_set(void);				//��ȡ��������
				    				   
#endif

