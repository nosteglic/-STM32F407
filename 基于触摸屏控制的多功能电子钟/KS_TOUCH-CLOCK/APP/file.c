#include "file.h"
//----------------------------------------------------------------
//                 file��SPI Flash�ļ���ȡ
//----------------------------------------------------------------
//**********************************************����***********************************************************
//*****�ļ�·������*****
//PC2LCD2002����ȡģ����:����ʽ,˳��(��λ��ǰ),����.C51��ʽ.																		    
//��������:
//���������:ASCII��+��(' '+95)
//��ͨ����:ASCII��
u8*const APP_ASCII_S6030="1:/SYSTEM/APP/COMMON/fonts60.fon";	//���������60*30����������·�� 
u8*const APP_ASCII_5427="1:/SYSTEM/APP/COMMON/font54.fon";		//��ͨ����54*27����������·�� 
u8*const APP_ASCII_3618="1:/SYSTEM/APP/COMMON/font36.fon";		//��ͨ����36*18����������·��
u8*const APP_ASCII_2814="1:/SYSTEM/APP/COMMON/font28.fon";		//��ͨ����28*14����������·�� 

u8*const APP_MUSIC="1:/�������ļ�/������.mp3";								//����·��

//*****��������������*****
u8* asc2_s6030=0;	//���������60*30���������
u8* asc2_5427=0;	//��ͨ����54*27���������
u8* asc2_3618=0;	//��ͨ����36*18���������
u8* asc2_2814=0;	//��ͨ����28*14���������

//**********************************************����***********************************************************
//*******************************************************************
//��ȡϵͳ�е����嵽������
//�������ͣ�u8
//ʹ�ò�������
//*******************************************************************
u8 font_set(void)
{
	u8 res;
	u8 rval=0;			//���÷���ֵ
	FIL* f_rec=0;		//�����ڴ�����
	f_rec=(FIL *)gui_memin_malloc(sizeof(FIL));			//����FIL�ֽڵ��ڴ�����  
	//������������
	res=f_open(f_rec,(const TCHAR*)APP_ASCII_S6030,FA_READ);//���ļ���:APP_ASCII_S6030
	if(res==FR_OK)					//����������ɹ�
	{
		asc2_s6030=(u8*)gui_memex_malloc(f_rec->fsize);	//Ϊ�����忪�ٻ����ַ
		if(asc2_s6030==0)								//���������Ϊ��
			rval=1;
		else 
			res=f_read(f_rec,asc2_s6030,f_rec->fsize,(UINT*)&br);	//һ�ζ�ȡ�����ļ�
	}
	res=f_open(f_rec,(const TCHAR*)APP_ASCII_2814,FA_READ);//���ļ�:APP_ASCII_2814
	if(res==FR_OK)					//����������ɹ�
	{
			asc2_2814=(u8*)gui_memex_malloc(f_rec->fsize);	//Ϊ�����忪�ٻ����ַ
			if(asc2_2814==0)//���������Ϊ��
				rval=1;
			else 
				res=f_read(f_rec,asc2_2814,f_rec->fsize,(UINT*)&br);	//һ�ζ�ȡ�����ļ�
	}
	res=f_open(f_rec,(const TCHAR*)APP_ASCII_3618,FA_READ);//���ļ�:APP_ASCII_3618
	if(res==FR_OK)					//����������ɹ�
	{
			asc2_3618=(u8*)gui_memex_malloc(f_rec->fsize);	//Ϊ�����忪�ٻ����ַ
			if(asc2_3618==0)//���������Ϊ��
				rval=1;
			else 
				res=f_read(f_rec,asc2_3618,f_rec->fsize,(UINT*)&br);	//һ�ζ�ȡ�����ļ�
	} 
	res=f_open(f_rec,(const TCHAR*)APP_ASCII_5427,FA_READ);//���ļ�:APP_ASCII_5427
	if(res==FR_OK)					//����������ɹ�
	{
			asc2_5427=(u8*)gui_memex_malloc(f_rec->fsize);	//Ϊ�����忪�ٻ����ַ
			if(asc2_5427==0)//���������Ϊ��
				rval=1;
			else 
				res=f_read(f_rec,asc2_5427,f_rec->fsize,(UINT*)&br);	//һ�ζ�ȡ�����ļ�
	}  
	rval=0;
	f_close(f_rec);	//�ر��ļ���
	return rval;
}

