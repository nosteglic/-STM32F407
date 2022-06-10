#include "mynotice.h"
#include "setting.h"

/**********************************************************************
 * �������ƣ�myerror_notice
 * ���룺	u8 rval������
 * �����u8 rval
 * ���ܣ�������ʾ
 *********************************************************************/
void myerror_notice(u8 rval){
	switch(rval){
		case 1:
			gui_notice((lcddev.width-300)/2,(lcddev.height-400)/2,1,"GUI���ʧ��!","ȷ��");
			break;
		case 2:
			gui_notice((lcddev.width-300)/2,(lcddev.height-400)/2,1,"˫��ʧ��!","ȷ��");
			break;
		case 3:
			gui_notice((lcddev.width-300)/2,(lcddev.height-400)/2,1,"��Ŀ����ʧ��!","ȷ��");
			break;
		case 4:
			gui_notice((lcddev.width-300)/2,(lcddev.height-400)/2,1,"�ڴ����ʧ��!","ȷ��");
			break;
		default:
			break;
	}
}

/**********************************************************************
 * �������ƣ�gui_notice
 * ���룺	x,y:��������(���ڳߴ��Ѿ��̶��˵�)	 
			mode��	0��������ť�����/ȡ��
					1��һ����ť��ȷ��
			caption:��ʾ��
			bcaption����ť��
 * �����u8 rval
 * ���ܣ���ʾ��
 *********************************************************************/
u8 gui_notice(u16 x,u16 y,u8 mode,u8 *caption,u8 *bcaption){
	u8 rval=0,res;
	u8 flag=0;
	_window_obj *twin=0;
	_btn_obj *Abtn;
	_btn_obj *Cbtn;
	_btn_obj *Obtn;
	_btn_obj *label;
	
	/////////////////////////////////////////////////////////////////////////
	twin=window_creat(x,y,300,400,0,1|1<<5,24);											//��������
 	
	label=btn_creat(x+25+30,y+84+82,SYSSET_EDIT_WIDTH*2,SYSSET_EDIT_HEIGHT,0,0x04);		//������ʾ��
	
	/*����*/
	if(twin==NULL)rval=ERROR_GUI;
	else{
		twin->caption="��ʾ";
		twin->captionheight=60;
		twin->captionbkcu=YELLOW;
		twin->captionbkcd=YELLOW;
		twin->captioncolor=BLACK;
		twin->windowbkc=WHITE;
	}
	
	if(mode==0){//������ť
		Abtn=btn_creat(x+25,y+84+234,SYSSET_BTN2_WIDTH,SYSSET_BTN2_HEIGHT,0,0x02);		//������ť
		Cbtn=btn_creat(x+25+125,y+84+234,SYSSET_BTN2_WIDTH,SYSSET_BTN2_HEIGHT,0,0x02);	//������ť
		
		if(Abtn==NULL||Cbtn==NULL)rval=ERROR_GUI;
		
		Abtn->font=24;
		Abtn->bcfucolor=BLACK;		//�ɿ�ʱΪ��ɫ
		Abtn->bcfdcolor=BLACK;		//����ʱΪ��ɫ
		Abtn->bkctbl[0]=WHITE;		//�߿���ɫ
		Abtn->bkctbl[1]=GRAY;		//��һ�е���ɫ				
		Abtn->bkctbl[2]=GREEN;		//�ϰ벿����ɫ
		Abtn->bkctbl[3]=GREEN;		//�°벿����ɫ
		Abtn->caption=bcaption;
		
		Cbtn->font=24;
		Cbtn->bcfucolor=WHITE;		//�ɿ�ʱΪ��ɫ
		Cbtn->bcfdcolor=WHITE;		//����ʱΪ��ɫ
		Cbtn->bkctbl[0]=WHITE;		//�߿���ɫ
		Cbtn->bkctbl[1]=GRAY;		//��һ�е���ɫ				
		Cbtn->bkctbl[2]=RED;		//�ϰ벿����ɫ
		Cbtn->bkctbl[3]=RED;		//�°벿����ɫ
		Cbtn->caption="ȡ��";
	}
	else{//һ����ť
		Obtn=btn_creat(x+25+SYSSET_BTN2_WIDTH/2+1,y+84+234,SYSSET_BTN2_WIDTH,SYSSET_BTN2_HEIGHT,0,0x02);		//������ť
		
		if(Obtn==NULL)rval=ERROR_GUI;
		
		Obtn->font=24;
		Obtn->bcfucolor=WHITE;		//�ɿ�ʱΪ��ɫ
		Obtn->bcfdcolor=WHITE;		//����ʱΪ��ɫ
		Obtn->bkctbl[0]=WHITE;		//�߿���ɫ
		Obtn->bkctbl[1]=GRAY;		//��һ�е���ɫ				
		Obtn->bkctbl[2]=RED;		//�ϰ벿����ɫ
		Obtn->bkctbl[3]=RED;		//�°벿����ɫ
		Obtn->caption=bcaption;
	}
	
	/*��ʾ��ʾ��Ϣ*/
	label->caption=caption;
	label->font=24;
	
	if(rval!=ERROR_GUI){
		window_draw(twin);					//������
		
		/*����ť*/
		if(mode==0){
			btn_draw(Abtn);					
			btn_draw(Cbtn);
		}
		else{
			btn_draw(Obtn);
		}
		
		btn_draw(label);					//����ǩ
		
		while(flag==0){
			tp_dev.scan(0);    						//������
			in_obj.get_key(&tp_dev,IN_TYPE_TOUCH);	//�õ�������ֵ
			
			if(mode==0){
				/*ȡ����ť���*/
				res=btn_check(Cbtn,&in_obj);
				if(res){
					if(((Cbtn->sta&0X80)==0)){//����Ч����
						rval=HOLD_CLOCK_SETTING;
						flag=1;
						break;	//�˳�
					}
				}
				/*bcaption��ť���*/
				res=btn_check(Abtn,&in_obj);
				if(res){
					if((Abtn->sta&0X80)==0){//����Ч����
						rval=SWITCH_CLOCK_SETTING;
						flag=1;
						break;
					}
				}
			}//if(mode==0)
			else{
				/*ȷ�ϰ�ť���*/
				res=btn_check(Obtn,&in_obj);
				if(res){
					if(((Obtn->sta&0X80)==0)){
						rval=SWITCH_CLOCK_SETTING;
						flag=1;
						break;
					}
				}
			}
			
		}//while(rval==0)
	}//if(rval==0)
	///////////////////////////////////////////////////////////////
	window_delete(twin);				//ɾ������
	
	/*ɾ����ť*/
	if(mode==0){
		btn_delete(Abtn);
		btn_delete(Cbtn);
	}
	btn_delete(label);					//ɾ����ʾ��
	printf("gui_notice:%d\n",rval);
	return rval;
}
