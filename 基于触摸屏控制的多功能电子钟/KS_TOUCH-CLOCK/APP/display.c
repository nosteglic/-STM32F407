#include "display.h"
#include "calendar.h"
#include "setting.h"
#include "alarm.h"

extern RTC_TimeTypeDef RTC_TimeStruct;     //ʱ��ṹ�壺ʱ���֡��롢ampm
extern RTC_DateTypeDef RTC_DateStruct;			//���ڽṹ�壺�ꡢ�¡��ա�����
extern _alarm_obj Alarm[RTC_Alarm_MAX_NUM];
extern u8 SYSTEM_Alarm_NUM;

u8 alarm_num_open=0X00;

u8*const calendar_week_table[GUI_LANGUAGE_NUM][7]=
{
{(u8 *)"������",(u8 *)"����һ",(u8 *)"���ڶ�",(u8 *)"������",(u8 *)"������",(u8 *)"������",(u8 *)"������"},
};

u8*const alarm_tbl[GUI_LANGUAGE_NUM][RTC_Alarm_MAX_NUM]=
{
	{
		(u8 *)"���� 1",
		(u8 *)"���� 2",
		(u8 *)"���� 3",
		(u8 *)"���� 4",
		(u8 *)"���� 5",
		(u8 *)"���� 6",
		(u8 *)"���� 7",
		(u8 *)"���� 8"
	},
};


/**********************************************************************
 * �������ƣ�setting_play
 * ���룺void
 * �����u8 rval
 * ���ܣ���ʾ���ܲ˵��������������ܲ���
 **********************************************************************/
u8 setting_play(void){
	int i;
	u8 rval=HOLD_CLOCK_SETTING,res;
	u8 **items;
	/*****ʱ��/�����ݴ�************/
	u16 temp1;
	u8 temp2,temp3;	
	u8 is_open;
	_window_obj *menu;			//���ܲ˵���
	_btn_obj *menu_btn[3];		/********���ܼ����߽ǰ�ť0X02********
									menu_btn[0]��ʱ������
									menu_btn[1]����������
									menu_btn[2]����������
								**************************************/
	items=(u8**)alarm_tbl[gui_phy.language];
	while(1){
		//////////////////////////////////////////////////////////////////////////////////////////////////
		/***********�������ܲ˵���********/
		menu=window_creat(menu_left,menu_top,menu_width,menu_height,menu_id,1<<5,24);//�˵�������
		/***********�������ܼ�************/
		menu_btn[0]=btn_creat(3,menu_button_top,menu_button_width,menu_button_height,menu_button_id,0x02);
		menu_btn[1]=btn_creat(6+menu_button_width,menu_button_top,menu_button_width,menu_button_height,menu_button_id,0x02);
		menu_btn[2]=btn_creat(6+2*menu_button_width,menu_button_top,menu_button_width,menu_button_height,menu_button_id,0x02);
		/////////////////////////////////////////////////////////////////////////////////////////////////
		/*********************************************����**********************************************/
		
		/*****************��������*******************/
		//�˵�����
		if(menu==NULL){
			rval=ERROR_GUI;
			myerror_notice(rval);
		}
		else{
			menu->caption="���ò˵�";
			menu->captionheight=40;
			menu->captionbkcu=YELLOW;
			menu->captionbkcd=YELLOW;
			menu->captioncolor=BLACK;
			menu->windowbkc=WHITE;
		}
		//���ܼ�����
		for(i=0;i<3;i++){
			if(menu_btn[i]==NULL){
				rval=ERROR_GUI;
				myerror_notice(rval);
				break;
			}
			menu_btn[i]->font=24;
			menu_btn[i]->bcfucolor=WHITE;
			menu_btn[i]->bcfucolor=WHITE;
			menu_btn[i]->bkctbl[0]=WHITE;
			menu_btn[i]->bkctbl[1]=GRAY;
			menu_btn[i]->bkctbl[2]=BLACK;
			menu_btn[i]->bkctbl[3]=BLACK;
			if(i==0)menu_btn[i]->caption="ʱ������";
			if(i==1)menu_btn[i]->caption="��������";
			if(i==2)menu_btn[i]->caption="��������";
			
		}
		///////////////////////////////////////////
		if(rval!=ERROR_GUI){							//GUI�����ɹ��Ļ�
			/*�����*/
			window_draw(menu);							//�����ܲ˵���
			for(i=0;i<3;i++)btn_draw(menu_btn[i]);		//�����ܼ�
			//////////
			rval=1;
			while(rval!=HOLD_CLOCK_SETTING&&rval!=SWITCH_CLOCK_SETTING){
				tp_dev.scan(0);							//�жϴ��������޴���
				in_obj.get_key(&tp_dev,IN_TYPE_TOUCH);	//�õ�������ֵ
				if(TPAD_Scan(0)){						//����TPAD������ʱ����ʾ����
					rval=SWITCH_CLOCK_SETTING;
					break;
				}
				for(i=0;i<3;i++){						//�ж��������ܼ�
					res=btn_check(menu_btn[i],&in_obj);	//menu_btn[i]�Ƿ񱻰��¼��
					if(res){							//����й��ܼ�������
						if((menu_btn[i]->sta&0X80)==0){	//����Ч����
							
							/*���ݹ��ܼ�������Ӧ����ʹ��*/
							switch(i){
								case 0:/*ʱ������*/
									temp1=RTC_TimeStruct.RTC_Hours;
									temp2=RTC_TimeStruct.RTC_Minutes;
									rval=sysset_time_set((lcddev.width-300)/2,(lcddev.height-400)/2,(u8*)&temp1,(u8*)&temp2,"ʱ�����ý���");
									if(rval==SWITCH_CLOCK_SETTING) {
										RTC_Set_Time(temp1,temp2,0,0);	//�����õ�ʱ�䱣��
									}
									break;
								case 1:/*��������*/
									temp1=RTC_DateStruct.RTC_Year+2000;//����RTC_Set_Date��yearֻ�к��������֣�����Ҫ����2000
									temp2=RTC_DateStruct.RTC_Month;
									temp3=RTC_DateStruct.RTC_Date;
									rval=sysset_date_set((lcddev.width-380)/2,(lcddev.height-400)/2,&temp1,(u8*)&temp2,(u8*)&temp3,"�������ý���");
									if(rval==SWITCH_CLOCK_SETTING){
										RTC_Set_Date(temp1-2000,temp2,temp3,RTC_Get_Week(temp1,temp2,temp3));//��������
									}
									break;
								case 2:/*��������*/
									if(SYSTEM_Alarm_NUM==0){						//��ʱû������
										temp1=RTC_TimeStruct.RTC_Hours;
										temp2=RTC_TimeStruct.RTC_Minutes;
										temp3=0;
										is_open=Alarm[0].saveflag;					//�ж��Ƿ��ǵ�һ����Ӹ�����
										rval=gui_notice((lcddev.width-300)/2,(lcddev.height-400)/2,0,"δ�������!","���");
										if(rval==SWITCH_CLOCK_SETTING){				//����������ӡ�
											if(is_open==0) is_open=0X02;			//����ǵ�һ�����
											rval=app_items_sel(0,0,lcddev.width,lcddev.height,(u8**)calendar_week_table[gui_phy.language],7,(u8 *)&temp1,(u8 *)&temp2,(u8 *)&temp3,0XB0,"����1",(u8 *)&is_open);//������ӽ���
											if(rval==SWITCH_CLOCK_SETTING){			//�������
												if(alarm_equal(SYSTEM_Alarm_NUM,(u8)temp1,temp2)){//����ͬ���õ�����
													rval=gui_notice((lcddev.width-300)/2,(lcddev.height-400)/2,1,"��ͬ�����Ѵ���!","ȷ��");
												}
												else{
													alarm_Set(SYSTEM_Alarm_NUM,(u8)temp1,temp2,is_open,temp3,0);//������������
												}
											}
										}
									}
									else rval=SWITCH_CLOCK_SETTING;					//���򣬽��������б�
									
									while(rval==SWITCH_CLOCK_SETTING){
										printf("���Ӳ˵����� %d\n",rval);
										rval=alarm_play(SYSTEM_Alarm_NUM);			//��ʾ�����б�
										if(rval==SWITCH_CLOCK_SETTING){				//���������б��������
											if(SYSTEM_Alarm_NUM>=RTC_Alarm_MAX_NUM){//�����涨������ӵ�������
												rval=gui_notice((lcddev.width-300)/2,(lcddev.height-400)/2,1,"��������!","ȷ��");
												SYSTEM_Alarm_NUM=RTC_Alarm_MAX_NUM;
											}
											else{									//δ����������
												temp1=RTC_TimeStruct.RTC_Hours;
												temp2=RTC_TimeStruct.RTC_Minutes;
												temp3=0;
												is_open=Alarm[SYSTEM_Alarm_NUM].saveflag;
												if(is_open==0) is_open=0X02;		//��һ�����
												rval=app_items_sel(0,0,lcddev.width,lcddev.height,(u8**)calendar_week_table[gui_phy.language],7,(u8 *)&temp1,(u8 *)&temp2,(u8 *)&temp3,0XB0,items[SYSTEM_Alarm_NUM],(u8 *)&is_open);
												if(rval==SWITCH_CLOCK_SETTING){		//���ˡ�������
													if(alarm_equal(SYSTEM_Alarm_NUM,(u8)temp1,temp2)){//����ͬ���õ�����
														rval=gui_notice((lcddev.width-300)/2,(lcddev.height-400)/2,1,"��ͬ�����Ѵ���!","ȷ��");
													}
													else{
														alarm_Set(SYSTEM_Alarm_NUM,(u8)temp1,temp2,is_open,temp3,0);//������������
													}
												}
											}
										}//if(rval==SWITCH_CLOCK_SETTING)
										printf("alarm_num_open:%d\n\n\r\n\n",SYSTEM_Alarm_NUM);
									}
									break;
							}//switch(i)���ж�����ʲô
						}//if((menu_btn[i]->sta&0X80)==0)����i���˵���ť����Ч����
					}//if(flag_ok)���в˵���ť����
				}//for(i=0;i<3;i++)
			
			}//rval!=SWITCH_CLOCK_SETTING||rval!=HOLD_CLOCK_SETTING
		}//if(!flag_break)��һ��ʼ���޳���
		///////////////////////////////////////////////////////////////////////////////////////////////////////
		window_delete(menu);							//ɾ�����ܲ˵���
		for(i=0;i<3;i++)btn_delete(menu_btn[i]);		//ɾ�����ܼ�
		printf("setting_play:%d\n",rval);
		return rval;
	}
}


/***********************************************************************
 * �������ƣ�alarm_play
 * ���룺count����ǰ������
 * �����u8 rval
 * ���ܣ���ʾ�����б����
 **********************************************************************/
u8 alarm_play(u8 count){	//count:������������
	////////////////////////////////////////
	u8 temp1,temp2,temp3,is_open;
	u8 rval=0,res;
	u8 rval_temp;
	u8 flag=0;
	u8 flag_temp=0;
	//[7]:0,���µ��Ƿ��ذ���,���߷����˴���;1,������ȷ�ϰ���,����˫����ѡ�е���Ŀ.
	//[6:0]:0,��ʾ�޴���;����,�������.
	u8 selitem=0;			//selitem:��ǰѡ�е���Ŀ
	u8 topitem=0;			//topitem:��ǰ�������Ŀ
	u8 **items;				//items[]:��Ŀ���ּ�
	/////////////////////////////////////////////////////////////
	items=(u8**)alarm_tbl[gui_phy.language];//���������
	
	while(flag==0){
		res=app_listbox_select(&selitem,&topitem,"��������",items,count);
		if(res&SWITCH_CLOCK_SETTING){		//������ˡ���ӡ�
			rval=SWITCH_CLOCK_SETTING;
			flag=1;
		}
		else if(res&HOLD_CLOCK_SETTING){	//��������ˡ�ȡ����
			rval=HOLD_CLOCK_SETTING;
			flag=1;
		}
		else if(res&0X80){					//˫����selitem������
			flag_temp=0;
			temp1=Alarm[selitem].hour;
			temp2=Alarm[selitem].min;
			temp3=Alarm[selitem].weekmask;
			is_open=Alarm[selitem].state;	//�ж����ӿ�����
			if(is_open==0X50)temp3=0;		//���ǵ�������ʱ��������ʾΪ��
			while(flag_temp==0){
				rval=app_items_sel(0,0,lcddev.width,lcddev.height,(u8**)calendar_week_table[gui_phy.language],7,(u8*)&temp1,(u8*)&temp2,(u8*)&temp3,0XB0,items[selitem],(u8 *)&is_open);//��ѡ
				if(rval==SWITCH_CLOCK_SETTING){	//������ˡ�����/�޸�/ɾ����
					if(is_open==0X01){	//ɾ��
						rval_temp=gui_notice((lcddev.width-300)/2,(lcddev.height-400)/2,0,"ȷ��ɾ��������?","ȷ��");
						if(rval_temp==SWITCH_CLOCK_SETTING) {//ȷ��ɾ��
							alarm_delete(selitem);		//ɾ����ǰ������
							selitem=0;
							count=SYSTEM_Alarm_NUM;
							//rval=SWITCH_CLOCK_SETTING;		//�ص�����������
							flag_temp=1;
						}
						else{is_open==0X00;}		//�ر�����
					}
					else{				//����/�޸�
						if(alarm_equal(selitem,temp1,temp2)){//����ͬ���õ�����
							rval=gui_notice((lcddev.width-300)/2,(lcddev.height-400)/2,1,"��ͬ�����Ѵ���!","ȷ��");
						}
						else{
							alarm_Set(selitem,temp1,temp2,is_open,temp3,0);//������������
							flag_temp=1;
						}
					}
				}//if(rval==SWITCH_CLOCK_SETTING)
				else if(rval==HOLD_CLOCK_SETTING){
					flag_temp=1;
				}
			}//while(flag_temp==0)
		}//else if(res&0X80)
		else flag=1;						//����			
	}//while
	printf("alarm_play:%d\n",rval);
	return rval;
}
