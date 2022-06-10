#include "setting.h"
#include "spb.h"

/*****************************��ͼ��*******************************/
//��1:����ʾflash
//��0������ʾsd��
//��2������ʾɶ������
u8*const APP_OK_PIC="1:/SYSTEM/APP/COMMON/ok.bmp";				//ȷ��ͼ��
u8*const APP_CANCEL_PIC="1:/SYSTEM/APP/COMMON/cancel.bmp";		//ȡ��ͼ��
u8*const APP_UNSELECT_PIC="1:/SYSTEM/APP/COMMON/unselect.bmp";	//δѡ��ͼ��
u8*const APP_SELECT_PIC="1:/SYSTEM/APP/COMMON/select.bmp";		//ѡ��ͼ��
///////////////////////////////////////////////////////////////////////

/**********************************************************************
 * �������ƣ�app_show_nummid
 * ���룺	x,y,width,height:����	 
			num:Ҫ��ʾ������
			len:λ��
			size:����ߴ�
			ptcolor,bkcolor:������ɫ�Լ�����ɫ
 * �����void
 * ���ܣ���һ�������м���ʾ����
 *********************************************************************/
void app_show_nummid(u16 x,u16 y,u16 width,u16 height,u32 num,u8 len,u8 size,u16 ptcolor,u16 bkcolor){
	u16 numlen;
	u8 xoff,yoff;
	numlen=(size/2)*len;							//���ֳ���
	if(numlen>width||size>height)return;
	xoff=(width-numlen)/2;
	yoff=(height-size)/2;
	POINT_COLOR=ptcolor;
	BACK_COLOR=bkcolor;
	LCD_ShowxNum(x+xoff,y+yoff,num,len,size,0X80);	//��ʾ�������
}

/**********************************************************************
 * �������ƣ�sysset_time_set
 * ���룺	x,y:��������(���ڳߴ��Ѿ��̶��˵�)	 
			hour,min:ʱ��
			caption:��������
 * �����u8 rval
 * ���ܣ�ʱ��/��������
 *********************************************************************/
u8 sysset_time_set(u16 x,u16 y,u8 *hour,u8 *min,u8*caption){
	u8 rval=HOLD_CLOCK_SETTING;
	u8 res;
	u8 i;
	u8 flag=0;
	///////////////////////////////////////////////////////////////////
	_window_obj* twin=0;	//����
 	_btn_obj * tbtn[6];		//�ܹ�������ť:0,ʱ�ӼӰ�ť;1,ʱ�Ӽ���ť;2,���ӼӰ�ť;3,���Ӽ���ť;4,ȷ�ϰ�ť;5,ȡ����ť.		  
 	twin=window_creat(x,y,300,400,0,1|1<<5,24);											//��������
 	tbtn[0]=btn_creat(x+35,y+84,SYSSET_BTN1_WIDTH,SYSSET_BTN1_HEIGHT,0,0x02);			//������ť
	tbtn[1]=btn_creat(x+35,y+84+134,SYSSET_BTN1_WIDTH,SYSSET_BTN1_HEIGHT,0,0x02);		//������ť
	tbtn[2]=btn_creat(x+35+130,y+84,SYSSET_BTN1_WIDTH,SYSSET_BTN1_HEIGHT,0,0x02);		//������ť
	tbtn[3]=btn_creat(x+35+130,y+84+134,SYSSET_BTN1_WIDTH,SYSSET_BTN1_HEIGHT,0,0x02);	//������ť
	tbtn[4]=btn_creat(x+25,y+84+234,SYSSET_BTN2_WIDTH,SYSSET_BTN2_HEIGHT,0,0x02);		//������ť
	tbtn[5]=btn_creat(x+25+125,y+84+234,SYSSET_BTN2_WIDTH,SYSSET_BTN2_HEIGHT,0,0x02);	//������ť
	///////////////////////////////////////////////////////////////////
	/*��������*/
	if(twin==NULL)rval=ERROR_GUI;		//����
	else{
		twin->caption=caption;			//�������
		twin->captionheight=60;			//����߶�
		twin->captionbkcu=YELLOW;		//�����Ͻ���ɫ
		twin->captionbkcd=YELLOW;		//�����½���ɫ
		twin->captioncolor=BLACK;		//������ɫ
		twin->windowbkc=WHITE;			//���屳����ɫ
	}
	
	/*��ť����*/
	for(i=0;i<6;i++){					
		if(tbtn[i]==NULL){				//����
			rval=ERROR_GUI;
			break;
		}
		tbtn[i]->font=24;				//��ť�����С
		if(i<4){						//�Ӽ�����
			tbtn[i]->bcfucolor=WHITE;	//�ɿ�ʱΪ��ɫ
			tbtn[i]->bcfdcolor=WHITE;	//����ʱΪ��ɫ			
			tbtn[i]->bkctbl[0]=WHITE;	//�߿���ɫ
			tbtn[i]->bkctbl[1]=GRAY;	//��һ�е���ɫ			
			tbtn[i]->bkctbl[2]=GRAY;	//�ϰ벿����ɫ
			tbtn[i]->bkctbl[3]=GRAY;	//�°벿����ɫ
		}else if(i==4){					//ȷ�ϰ���
			tbtn[i]->bcfucolor=BLACK;	//�ɿ�ʱΪ��ɫ
			tbtn[i]->bcfdcolor=BLACK;	//����ʱΪ��ɫ
			tbtn[i]->bkctbl[0]=WHITE;	//�߿���ɫ
			tbtn[i]->bkctbl[1]=GRAY;	//��һ�е���ɫ				
			tbtn[i]->bkctbl[2]=GREEN;	//�ϰ벿����ɫ
			tbtn[i]->bkctbl[3]=GREEN;	//�°벿����ɫ
		}
		else{							//ȡ������
			tbtn[i]->bcfucolor=WHITE;	//�ɿ�ʱΪ��ɫ
			tbtn[i]->bcfdcolor=WHITE;	//����ʱΪ��ɫ
			tbtn[i]->bkctbl[0]=WHITE;	//�߿���ɫ
			tbtn[i]->bkctbl[1]=GRAY;	//��һ�е���ɫ				
			tbtn[i]->bkctbl[2]=RED;		//�ϰ벿����ɫ
			tbtn[i]->bkctbl[3]=RED;		//�°벿����ɫ
		}
		/*��ť����*/
		if(i==0||i==2)tbtn[i]->caption="+";
		if(i==1||i==3)tbtn[i]->caption="-";
		if(i==4)tbtn[i]->caption="ȷ��";
		if(i==5)tbtn[i]->caption="ȡ��";				
	}
	//////////////////////////////////////////////////////////////////
	if(rval!=ERROR_GUI){//�޴���
		window_draw(twin);					//��������
		for(i=0;i<6;i++)btn_draw(tbtn[i]);	//����ť
		
		/*�����õ�ʱ/������*/
		gui_fill_rectangle(x+35+1,y+84+82,SYSSET_EDIT_WIDTH-2,SYSSET_EDIT_HEIGHT,SYSSET_EDIT_BACK_COLOR);  		//���ʱ�ӱ���
		gui_fill_rectangle(x+35+130+1,y+84+82,SYSSET_EDIT_WIDTH-2,SYSSET_EDIT_HEIGHT,SYSSET_EDIT_BACK_COLOR);	//�����ӱ���
 		app_show_nummid(x+35,y+84+82,SYSSET_EDIT_WIDTH,SYSSET_EDIT_HEIGHT,*hour,2,24,BLACK,SYSSET_EDIT_BACK_COLOR);
		app_show_nummid(x+35+130,y+84+82,SYSSET_EDIT_WIDTH,SYSSET_EDIT_HEIGHT,*min,2,24,BLACK,SYSSET_EDIT_BACK_COLOR);  
		
		while(flag==0){
			tp_dev.scan(0);							//������
			in_obj.get_key(&tp_dev,IN_TYPE_TOUCH);	//�õ�������ֵ   
			
			/*��ťɨ��*/
			for(i=0;i<6;i++){
				res=btn_check(tbtn[i],&in_obj);		//ȷ�ϰ�ť���
				if(res){
					if((tbtn[i]->sta&0X80)==0){		//����Ч����
						switch(i){
							case 0:					//ʱ�����Ӱ�ť������
								(*hour)++;
								if(*hour>23)*hour=0;
								break;
							case 1:					//ʱ�Ӽ��ٰ�ť������	  
								if(*hour)(*hour)--;
								else *hour=23;
								break;
							case 2:					//�������Ӱ�ť������
								(*min)++;
								if(*min>59)(*min)=0;
								break;
							case 3:					//���Ӽ��ٰ�ť������	  
								if(*min)(*min)--;
								else *min=59;
								break;
							case 4:					//ȷ�ϰ�ť����
								rval=SWITCH_CLOCK_SETTING;
								flag=1;
								break;	  
							case 5:					//ȡ����ť����	  
								rval=HOLD_CLOCK_SETTING;
								flag=1;
								break;
 						}//switch(i)
					}//if((tbtn[i]->sta&0X80)==0)
					
					/*��ʾ������ʱ/��*/
					app_show_nummid(x+35,y+84+82,SYSSET_EDIT_WIDTH,SYSSET_EDIT_HEIGHT,*hour,2,24,BLACK,SYSSET_EDIT_BACK_COLOR);
					app_show_nummid(x+35+130,y+84+82,SYSSET_EDIT_WIDTH,SYSSET_EDIT_HEIGHT,*min,2,24,BLACK,SYSSET_EDIT_BACK_COLOR);  
				}//if(res)
			}//for(i=0;i<6;i++)
		}//while(flag==0)
 	}//if(rval!=ERROR_GUI)
	////////////////////////////////////////////////////////////////////////////////////////////////////////
	window_delete(twin);					//ɾ������
	for(i=0;i<6;i++)btn_delete(tbtn[i]);	//ɾ����ť
	printf("sysset_time_set:%d",rval);
	return rval;
}


/**********************************************************************
 * �������ƣ�sysset_is_leap_year
 * ���룺	year
 * �����	������ǲ�������.1,��.0,����
 * ���ܣ��ж��Ƿ������꺯��
//�·�   1  2  3  4  5  6  7  8  9  10 11 12
//����   31 29 31 30 31 30 31 31 30 31 30 31
//������ 31 28 31 30 31 30 31 31 30 31 30 31
 *********************************************************************/
u8 sysset_is_leap_year(u16 year){			  
	if(year%4==0){ 							//�����ܱ�4����
		if(year%100==0){ 
			if(year%400==0)return 1;		//�����00��β,��Ҫ�ܱ�400���� 	   
			else return 0;   
		}else return 1;   
	}else return 0;	
}	

/***********************************************************************
 * �������ƣ�sysset_date_set
 * ���룺	x,y:��������(���ڳߴ��Ѿ��̶��˵�)	 
			year,month,date:������
			caption:��������
 * �����	u8 rval
 * ���ܣ���������
 ***********************************************************************/
u8 sysset_date_set(u16 x,u16 y,u16 *year,u8 *month,u8 *date,u8*caption) 
{
	u8 rval=0,res;
	u8 i;
	u8 maxdate=31;			//2�·���������
	u8 flag=0;
	//////////////////////////////////////////////////////////////////////////
	_window_obj* twin=0;	//����
 	_btn_obj * tbtn[8];		//�ܹ��˸���ť:0,��ݼӰ�ť;1,��ݼ���ť;2,�·ݼӰ�ť;3�·ݼ���ť;4,���ڼӰ�ť;5,���ڼ���ť;6,ȷ�ϰ�ť;7,ȡ����ť		  
 	
	twin=window_creat(x,y,380,400,0,1|1<<5,24);											//��������
 	tbtn[0]=btn_creat(x+20,y+84,SYSSET_BTN1_WIDTH,SYSSET_BTN1_HEIGHT,0,0x02);			//������ť
	tbtn[1]=btn_creat(x+20,y+84+134,SYSSET_BTN1_WIDTH,SYSSET_BTN1_HEIGHT,0,0x02);		//������ť
	tbtn[2]=btn_creat(x+20+120,y+84,SYSSET_BTN1_WIDTH,SYSSET_BTN1_HEIGHT,0,0x02);		//������ť
	tbtn[3]=btn_creat(x+20+120,y+84+134,SYSSET_BTN1_WIDTH,SYSSET_BTN1_HEIGHT,0,0x02);	//������ť
	tbtn[4]=btn_creat(x+20+240,y+84,SYSSET_BTN1_WIDTH,SYSSET_BTN1_HEIGHT,0,0x02);		//������ť
	tbtn[5]=btn_creat(x+20+240,y+84+134,SYSSET_BTN1_WIDTH,SYSSET_BTN1_HEIGHT,0,0x02);	//������ť

	tbtn[6]=btn_creat(x+40,y+84+234,SYSSET_BTN2_WIDTH,SYSSET_BTN2_HEIGHT,0,0x02);		//������ť
	tbtn[7]=btn_creat(x+40+180,y+84+234,SYSSET_BTN2_WIDTH,SYSSET_BTN2_HEIGHT,0,0x02);	//������ť
	
	////////////////////////////////////////////////////////////////////////////////
	/*��������*/
 	if(twin==NULL)rval=ERROR_GUI;
	else{
		twin->caption=caption;
		twin->captionheight=60;
		twin->captionbkcu=YELLOW;
		twin->captionbkcd=YELLOW;
		twin->captioncolor=BLACK;
		twin->windowbkc=WHITE;
	}
	/*��ť����*/
	for(i=0;i<8;i++)
	{
		tbtn[i]->font=24;
		if(tbtn[i]==NULL)
		{
			rval=ERROR_GUI;
			break;
		}
		if(i<6)			//�Ӽ�����
		{
			tbtn[i]->bcfucolor=WHITE;	//�ɿ�ʱΪ��ɫ
			tbtn[i]->bcfdcolor=WHITE;	//����ʱΪ��ɫ			
			tbtn[i]->bkctbl[0]=WHITE;	//�߿���ɫ
			tbtn[i]->bkctbl[1]=GRAY;	//��һ�е���ɫ			
			tbtn[i]->bkctbl[2]=GRAY;	//�ϰ벿����ɫ
			tbtn[i]->bkctbl[3]=GRAY;	//�°벿����ɫ
		}else if(i==6)	//ȷ�ϰ���
		{
			tbtn[i]->bcfucolor=BLACK;	//�ɿ�ʱΪ��ɫ
			tbtn[i]->bcfdcolor=BLACK;	//����ʱΪ��
			tbtn[i]->bkctbl[0]=WHITE;	//�߿���ɫ
			tbtn[i]->bkctbl[1]=GRAY;	//��һ�е���ɫ				
			tbtn[i]->bkctbl[2]=GREEN;	//�ϰ벿����ɫ
			tbtn[i]->bkctbl[3]=GREEN;	//�°벿����ɫ
		}
		else{			//ȡ������
			tbtn[i]->bcfucolor=WHITE;	//�ɿ�ʱΪ��
			tbtn[i]->bcfdcolor=WHITE;	//����ʱΪ��ɫ
			tbtn[i]->bkctbl[0]=WHITE;	//�߿���ɫ
			tbtn[i]->bkctbl[1]=GRAY;	//��һ�е���ɫ				
			tbtn[i]->bkctbl[2]=RED;		//�ϰ벿����ɫ
			tbtn[i]->bkctbl[3]=RED;		//�°벿����ɫ
		}
		if(i==0||i==2||i==4)tbtn[i]->caption="+";
		if(i==1||i==3||i==5)tbtn[i]->caption="-";
		if(i==6)tbtn[i]->caption="ȷ��";
		if(i==7)tbtn[i]->caption="ȡ��";				
	}
	
	if(rval!=ERROR_GUI)//�޴���
	{
		window_draw(twin);					//��������
		for(i=0;i<8;i++)btn_draw(tbtn[i]);	//����ť
		
		/*��������*/
		gui_fill_rectangle(x+20+1,y+84+82,SYSSET_EDIT_WIDTH-2,SYSSET_EDIT_HEIGHT,SYSSET_EDIT_BACK_COLOR);  		//�����ݱ���
		gui_fill_rectangle(x+20+120+1,y+84+82,SYSSET_EDIT_WIDTH-2,SYSSET_EDIT_HEIGHT,SYSSET_EDIT_BACK_COLOR);	//����·ݱ���
 		gui_fill_rectangle(x+20+240+1,y+84+82,SYSSET_EDIT_WIDTH-2,SYSSET_EDIT_HEIGHT,SYSSET_EDIT_BACK_COLOR);	//������ڱ���

 		app_show_nummid(x+20,y+84+82,SYSSET_EDIT_WIDTH,SYSSET_EDIT_HEIGHT,*year,4,24,BLACK,SYSSET_EDIT_BACK_COLOR);
		app_show_nummid(x+20+120,y+84+82,SYSSET_EDIT_WIDTH,SYSSET_EDIT_HEIGHT,*month,2,24,BLACK,SYSSET_EDIT_BACK_COLOR);  
		app_show_nummid(x+20+240,y+84+82,SYSSET_EDIT_WIDTH,SYSSET_EDIT_HEIGHT,*date,2,24,BLACK,SYSSET_EDIT_BACK_COLOR);  
		
		while(flag==0){
			tp_dev.scan(0);    						//���������
			in_obj.get_key(&tp_dev,IN_TYPE_TOUCH);	//�õ�������ֵ
			
			/*����*/
			for(i=0;i<8;i++){
				res=btn_check(tbtn[i],&in_obj);		//ȷ�ϰ�ť���
				if(res){
					if((tbtn[i]->sta&0X80)==0){		//����Ч����
						if(*month==2){
							if(sysset_is_leap_year(*year))maxdate=29;//�������2�·�
	 						else maxdate=28;	  
						}
						else if((*month==1)||(*month==3)||(*month==5)||(*month==7)||(*month==8)||(*month==10)||(*month==12))maxdate=31;
						else maxdate=30; 
						switch(i){
							case 0://������Ӱ�ť������
								(*year)++;
								if(*year>2100)*year=2000;
								break;
							case 1://��ݼ��ٰ�ť������	  
								if(*year>2000)(*year)--;
								else *year=2100;
								break;
							case 2://�·����Ӱ�ť������
								(*month)++;
								if(*month>12)(*month)=1;
								break;
							case 3://�·ݼ��ٰ�ť������	  
								if(*month>1)(*month)--;
								else *month=12;
								break;
							case 4://�������Ӱ�ť������
								(*date)++;
								if(*date>maxdate)(*date)=1;
								break;
							case 5://���ڼ��ٰ�ť������	  
								if(*date>1)(*date)--;
								else *date=maxdate;
								break;
							case 6://ȷ�ϰ�ť����  
								rval=SWITCH_CLOCK_SETTING;
								flag=1;
								break;	  
							case 7://ȡ����ť����  
								rval=HOLD_CLOCK_SETTING;
								flag=1;
								break;
 						}
					}   
			 		app_show_nummid(x+20,y+84+82,SYSSET_EDIT_WIDTH,SYSSET_EDIT_HEIGHT,*year,4,24,BLACK,SYSSET_EDIT_BACK_COLOR);
					app_show_nummid(x+20+120,y+84+82,SYSSET_EDIT_WIDTH,SYSSET_EDIT_HEIGHT,*month,2,24,BLACK,SYSSET_EDIT_BACK_COLOR);  
					app_show_nummid(x+20+240,y+84+82,SYSSET_EDIT_WIDTH,SYSSET_EDIT_HEIGHT,*date,2,24,BLACK,SYSSET_EDIT_BACK_COLOR);  
				}//if(res)
			}//for(i=0;i<8;i++)
		}//while(rval==0)
 	}
	///////////////////////////////////////////////////////
	window_delete(twin);				//ɾ������
	for(i=0;i<8;i++)btn_delete(tbtn[i]);//ɾ����ť
	return rval;
}

/***********************************************************************
 * �������ƣ�app_listbox_select
 * ���룺	//sel:��ǰѡ�е���Ŀ
			//top:��ǰ�������Ŀ
			//caption:ѡ�������
			//items[]:��Ŀ���ּ�
			//itemsize:����Ŀ��
 * �����	[7]:0,���µ��Ƿ��ذ���,���߷����˴���;1,������ȷ�ϰ���,����˫����ѡ�е���Ŀ.
     		[6:0]:0,��ʾ�޴���;����,�������.
 * ���ܣ��½�ѡ���
 ***********************************************************************/
u8 app_listbox_select(u8 *sel,u8 *top,u8 * caption,u8 *items[],u8 itemsize){
	u8  res;
	u8 rval=0;					//����ֵ	  
  	u16 i;	    						   
	u8 flag=0;
 	_btn_obj* rbtn=0;			//���ذ�ť�ؼ�
 	_btn_obj* okbtn=0;			//ȷ�ϰ�ť�ؼ�
	_listbox_obj * tlistbox;	//listbox 
	
	if(*sel>=itemsize||*top>=itemsize)return ERROR_ITEM_SIZE;	//��������/�����Ƿ�
 	app_filebrower(caption,0X07);								//��ʾ����
   	tlistbox=listbox_creat(0,gui_phy.tbheight,lcddev.width,lcddev.height-gui_phy.tbheight*2,1,gui_phy.tbfsize);//����һ��filelistbox
	if(tlistbox==NULL)rval=ERROR_MALLOC;						//�����ڴ�ʧ��.
	else{														//�����Ŀ
		for(i=0;i<itemsize;i++){
			res=listbox_addlist(tlistbox,items[i]);
			if(res){
				rval=ERROR_MALLOC;
				break;
			}
		}
	}
	if(rval==0){//�ɹ��������Ŀ
		tlistbox->scbv->topitem=*top;
		tlistbox->selindex=*sel;
		listbox_draw_listbox(tlistbox);
		rbtn=btn_creat(lcddev.width-2*gui_phy.tbfsize-8-1,lcddev.height-gui_phy.tbheight,2*gui_phy.tbfsize+8,gui_phy.tbheight-1,0,0x03);//�������ְ�ť
 		okbtn=btn_creat(0,lcddev.height-gui_phy.tbheight,2*gui_phy.tbfsize+8,gui_phy.tbheight-1,0,0x03);//����ȷ�����ְ�ť
		if(rbtn==NULL||okbtn==NULL)rval=ERROR_GUI;		//û���㹻�ڴ湻����
		else{
		 	rbtn->caption="ȡ��";			//����
			rbtn->font=gui_phy.tbfsize;		//�����µ������С	 	 
			rbtn->bcfdcolor=BLACK;			//����ʱ����ɫ
			rbtn->bcfucolor=BLACK;			//�ɿ�ʱ����ɫ
			btn_draw(rbtn);					//����ť
			
		 	okbtn->caption="���";			//����
		 	okbtn->font=gui_phy.tbfsize;	//�����µ������С	 
			okbtn->bcfdcolor=BLACK;			//����ʱ����ɫ
			okbtn->bcfucolor=BLACK;			//�ɿ�ʱ����ɫ
			btn_draw(okbtn);				//����ť
		}	   
 	}  
   	while(flag==0){
		tp_dev.scan(0);    
		in_obj.get_key(&tp_dev,IN_TYPE_TOUCH);	//�õ�������ֵ 
		/*ȡ����ť���*/
		res=btn_check(rbtn,&in_obj);		    
		if(res)if(((rbtn->sta&0X80)==0)){		//��ť״̬�ı���
			rval=HOLD_CLOCK_SETTING;
			flag=1;
		}
		/*��Ӱ�ť���*/
		res=btn_check(okbtn,&in_obj);
		if(res){
			if(((okbtn->sta&0X80)==0)){ 
				rval=SWITCH_CLOCK_SETTING;
				flag=1;				 				   			   
  			}	 
		}  
		
		listbox_check(tlistbox,&in_obj);		//ɨ�� 
		if(tlistbox->dbclick==0X80){			//˫����
			*top=tlistbox->scbv->topitem;		//��¼�˳�ʱѡ�����Ŀ
			*sel=tlistbox->selindex;
			rval|=1<<7;							//���˫��������
			flag=1;			
		}
		
	}
	////////////////////////////////////////////////////////////////////////
	listbox_delete(tlistbox);					//ɾ��listbox
	btn_delete(okbtn);							//ɾ����ť	  	 
	btn_delete(rbtn);							//ɾ����ť 
	printf("app_listbox_select:%d\n\n",rval);
	return rval; 
}

/***********************************************************************
 * �������ƣ�app_filebrower
 * ���룺	//topname:�����ʱ��Ҫ��ʾ������	 
			//mode:
				//[0]:0,����ʾ�Ϸ�ɫ��;1,��ʾ�Ϸ�ɫ��
				//[1]:0,����ʾ�·�ɫ��;1,��ʾ�·�ɫ��
				//[2]:0,����ʾ����;1,��ʾ����
				//[3~7]:����
 * �����	void
 * ���ܣ��ļ����������ʾ
 ***********************************************************************/
void app_filebrower(u8 *topname,u8 mode)
{		
  	if(mode&0X01)app_gui_tcbar(0,0,lcddev.width,gui_phy.tbheight,0x02);								//�·ֽ���
	if(mode&0X02)app_gui_tcbar(0,lcddev.height-gui_phy.tbheight,lcddev.width,gui_phy.tbheight,0x01);//�Ϸֽ���
	if(mode&0X04)gui_show_strmid(0,0,lcddev.width,gui_phy.tbheight,BLACK,gui_phy.tbfsize,topname);	  
} 

/***********************************************************************
 * �������ƣ�app_gui_tcbar
 * ���룺	//x,y,width,height:���꼰�ߴ�.
			//mode:	���÷ֽ���
				//	    [3]:�ұ߷ֽ���
				//		[2]:��߷ֽ���
				//		[1]:�±߷ֽ���
				//		[0]:�ϱ߷ֽ���
 * �����	void
 * ���ܣ�2ɫ��
 ***********************************************************************/
void app_gui_tcbar(u16 x,u16 y,u16 width,u16 height,u8 mode)
{
 	u16 halfheight=height/2;
 	gui_fill_rectangle(x,y,width,halfheight,YELLOW);  			//���ײ���ɫ	
 	gui_fill_rectangle(x,y+halfheight,width,halfheight,YELLOW); //���ײ���ɫ
	if(mode&0x01)gui_draw_hline(x,y,width,GRAY);
	if(mode&0x02)gui_draw_hline(x,y+height-1,width,GRAY);
	if(mode&0x04)gui_draw_vline(x,y,height,GRAY);
	if(mode&0x08)gui_draw_vline(x+width-1,y,width,GRAY);
} 

/***********************************************************************
 * �������ƣ�set_search_caption
 * ���룺	mcaption:��Ŀ¼�µ�Ŀ¼��(һ��Ҫ��'.'�ַ�������)
 * �����	(u8*)(++mcaption)
 * ���ܣ�	������Ŀ����
 ***********************************************************************/
u8 * set_search_caption(const u8 *mcaption)
{
	while(*mcaption!='.')mcaption++;
	return (u8*)(++mcaption);
}

/**************************************
 * �������ƣ�
 * ���룺	//x,y,width,height:����ߴ�(width��СΪ150,height��СΪ72)
			//items[]:��Ŀ���ּ�
			//itemsize:����Ŀ��(��󲻳���8��)
			//selx:���.��ѡģʽʱ,��Ӧ�����ѡ�����.��ѡģʽʱ,��Ӧѡ�����Ŀ.
			//mode:
				//[7]:0,��OK��ť;1,��OK��ť
				//[6]:0,����ȡ����ɫ;1,��ȡ����ɫ
				//[5]:0,��ѡģʽ;1,��ѡģʽ
				//[4]:0,������ͼ��;1,����ͼ��
				//[3:0]:����
				//caption:��������	  
 * �����u8 rval
 * ���ܣ����ӽ���
 **************************************/
u8 app_items_sel(u16 x,u16 y,u16 width,u16 height,u8 *items[],u8 itemsize,u8 *hour,u8 *min,u8 *selx,u8 mode,u8*caption,u8 *is_open80) 
{
	u8 rval=0,res;
	u8 selsta=0;	//ѡ��״̬Ϊ0,
					//[7]:����Ƿ��Ѿ���¼��һ�ΰ��µ���Ŀ;
					//[6:4]:����
	                //[3:0]:��һ�ΰ��µ���Ŀ
	u16 i;

	u8 temp;
	u16 itemheight=0;		//ÿ����Ŀ�ĸ߶�
	u16 itemwidth=0;		//ÿ����Ŀ�Ŀ��
	u8* unselpath=0;		//δѡ�е�ͼ���·��
	u8* selpath=0;			//ѡ��ͼ���·��
	u8* icopath=0;
	u8 flag=0;
	
///////////////////////////////////////////////////////////////////////////////////////////
 	_window_obj* twin=0;	//����
	_btn_obj * tbtn[7];		//ʱ�����õ�6����ť
							//tbtn[0-3]���Ӽ�
							//tbtn[4]:����/�޸�
							//tbtn[5]:ȡ��
							//tbtn[6]:�ر�
					
							
	if(itemsize>8||itemsize<1)return ERROR_ITEM_SIZE;	//��Ŀ������
	if(width<150||height<72)return ERROR_ITEM_SIZE; 	//�ߴ����
	
	itemheight=(height-450)/itemsize-1;					//�õ�ÿ����Ŀ�ĸ߶�
	itemwidth=width;									//ÿ����Ŀ�Ŀ��
	
 	twin=window_creat(0,0,lcddev.width,lcddev.height,0,1|(1<<5)|((1<<6)&mode),24);		//��������
	
	tbtn[0]=btn_creat(x+120,y+114,SYSSET_BTN1_WIDTH,SYSSET_BTN1_HEIGHT,0,0x02);			//������ť
	tbtn[1]=btn_creat(x+120,y+114+134,SYSSET_BTN1_WIDTH,SYSSET_BTN1_HEIGHT,0,0x02);		//������ť
	tbtn[2]=btn_creat(x+120+130,y+114,SYSSET_BTN1_WIDTH,SYSSET_BTN1_HEIGHT,0,0x02);		//������ť
	tbtn[3]=btn_creat(x+120+130,y+114+134,SYSSET_BTN1_WIDTH,SYSSET_BTN1_HEIGHT,0,0x02);	//������ť
	
	/*����*/
	if(twin==NULL){
		spb_delete();						//�ͷ�SPBռ�õ��ڴ�
		twin=window_creat(0,0,lcddev.width,lcddev.height,0,1|(1<<5)|((1<<6)&mode),24);//���´�������
		rval=ERROR_GUI;
 	}
	/*��ť*/
	for(i=0;i<4;i++){
		if(tbtn[i]==NULL){
			rval=ERROR_GUI;
			break;
		}
		tbtn[i]->font=24;
		tbtn[i]->bcfucolor=WHITE;			//�ɿ�ʱΪ��ɫ
		tbtn[i]->bcfdcolor=WHITE;			//����ʱΪ��ɫ			
		tbtn[i]->bkctbl[0]=WHITE;			//�߿���ɫ
		tbtn[i]->bkctbl[1]=GRAY;			//��һ�е���ɫ			
		tbtn[i]->bkctbl[2]=GRAY;			//�ϰ벿����ɫ
		tbtn[i]->bkctbl[3]=GRAY;			//�°벿����ɫ
		
		if(i==0||i==2)tbtn[i]->caption="+";
		if(i==1||i==3)tbtn[i]->caption="-";				
	}
  	if(mode&(1<<7)){						//��OK��ť
		tbtn[4]=btn_creat(0,lcddev.height-y-SYSSET_BTN2_HEIGHT,SYSSET_BTN2_WIDTH,SYSSET_BTN2_HEIGHT,0,0x02);										//��������/�޸İ�ť
		tbtn[5]=btn_creat(lcddev.width-SYSSET_BTN2_WIDTH,lcddev.height-y-SYSSET_BTN2_HEIGHT,SYSSET_BTN2_WIDTH,SYSSET_BTN2_HEIGHT,0,0x02);			//����ȡ����ť
		tbtn[6]=btn_creat((lcddev.width-SYSSET_BTN2_WIDTH)/2,lcddev.height-y-SYSSET_BTN2_HEIGHT,SYSSET_BTN2_WIDTH,SYSSET_BTN2_HEIGHT,0,0x02);	//�����رհ�ť
		if(tbtn[6]==NULL)rval=ERROR_GUI;
		else{
			if(((*is_open80)==0X80)|((*is_open80)==0X50))tbtn[6]->caption="�ر�";
			else tbtn[6]->caption="ɾ��";
			tbtn[6]->font=24;
			tbtn[6]->bcfucolor=WHITE;	//�ɿ�ʱΪ��ɫ
			tbtn[6]->bcfdcolor=WHITE;	//����ʱΪ��ɫ
			tbtn[6]->bkctbl[0]=WHITE;	//�߿���ɫ
			tbtn[6]->bkctbl[1]=GRAY;	//��һ�е���ɫ				
			tbtn[6]->bkctbl[2]=BLACK;	//�ϰ벿����ɫ
			tbtn[6]->bkctbl[3]=BLACK;	//�°벿����ɫ
		}
		if(twin==NULL||tbtn[4]==NULL)rval=ERROR_GUI;
		else{
			tbtn[4]->font=24;
			tbtn[4]->bcfucolor=BLACK;		//�ɿ�ʱΪ��ɫ
			tbtn[4]->bcfdcolor=BLACK;		//����ʱΪ��ɫ
			tbtn[4]->bkctbl[0]=WHITE;		//�߿���ɫ
			tbtn[4]->bkctbl[1]=GRAY;		//��һ�е���ɫ				
			tbtn[4]->bkctbl[2]=GREEN;		//�ϰ벿����ɫ
			tbtn[4]->bkctbl[3]=GREEN;		//�°벿����ɫ
			if(((*is_open80)==0X80)|((*is_open80)==0X50)) tbtn[4]->caption="�޸�";
			else tbtn[4]->caption="����";
			
		}
		
	}else{ 										//û��ok��ť
		tbtn[5]=btn_creat(lcddev.width-SYSSET_BTN2_WIDTH,lcddev.height-y-SYSSET_BTN2_HEIGHT,SYSSET_BTN2_WIDTH,SYSSET_BTN2_HEIGHT,0,0x02);	//����ȡ����ť
		if(twin==NULL||tbtn[5]==NULL)rval=ERROR_GUI;
	}
 	if(rval!=ERROR_GUI||rval!=ERROR_ITEM_SIZE){//֮ǰ�Ĳ�������
		/*����*/
		twin->font=24;
 		twin->caption=caption;
		twin->captionheight=60;
		twin->captionbkcu=YELLOW;
		twin->captionbkcd=YELLOW;
		twin->captioncolor=BLACK;
		twin->windowbkc=WHITE;
		
		/*��ť*/
		tbtn[5]->font=24;
 		tbtn[5]->caption="ȡ��";		//ȡ��
		tbtn[5]->bcfucolor=WHITE;		//�ɿ�ʱΪ��ɫ
		tbtn[5]->bcfdcolor=WHITE;		//����ʱΪ��ɫ
 		tbtn[5]->bkctbl[0]=WHITE;		//�߿���ɫ
		tbtn[5]->bkctbl[1]=GRAY;		//��һ�е���ɫ				
		tbtn[5]->bkctbl[2]=RED;			//�ϰ벿����ɫ
		tbtn[5]->bkctbl[3]=RED;			//�°벿����ɫ

		if(mode&(1<<4))					//��Ҫ����ͼ��
		{
  			if(mode&(1<<5))				//��ѡģʽ
			{
				unselpath=(u8*)APP_CANCEL_PIC;		//δѡ�е�ͼ���·��
				selpath=(u8*)APP_OK_PIC;			//ѡ��ͼ���·��
			}else		   				//��ѡģʽ
			{
				unselpath=(u8*)APP_UNSELECT_PIC;	//δѡ�е�ͼ���·��
				selpath=(u8*)APP_SELECT_PIC;		//ѡ��ͼ���·��
			}
		}
		
		window_draw(twin);					//��������
		btn_draw(tbtn[5]);					//����ť
	    if(mode&(1<<7)){
			if((*is_open80)!=0X02)
				btn_draw(tbtn[6]);
			btn_draw(tbtn[4]);
		}
		for(i=0;i<4;i++)btn_draw(tbtn[i]);	//����ť
		
		gui_fill_rectangle(x+120+1,y+114+82,SYSSET_EDIT_WIDTH-2,SYSSET_EDIT_HEIGHT,SYSSET_EDIT_BACK_COLOR);  		//���ʱ�ӱ���
		gui_fill_rectangle(x+120+130+1,y+114+82,SYSSET_EDIT_WIDTH-2,SYSSET_EDIT_HEIGHT,SYSSET_EDIT_BACK_COLOR);		//�����ӱ���
 		app_show_nummid(x+120,y+114+82,SYSSET_EDIT_WIDTH,SYSSET_EDIT_HEIGHT,*hour,2,24,BLACK,SYSSET_EDIT_BACK_COLOR);
		app_show_nummid(x+120+130,y+114+82,SYSSET_EDIT_WIDTH,SYSSET_EDIT_HEIGHT,*min,2,24,BLACK,SYSSET_EDIT_BACK_COLOR);
		
		/*����*/
		for(i=0;i<itemsize;i++){
			icopath=app_get_icopath(mode&(1<<5),selpath,unselpath,*selx,i); 										//�õ�ͼ��·��
			app_show_items(x+5,y+375+i*(itemheight+1),itemwidth,itemheight,items[i],icopath,BLACK,twin->windowbkc);	//��ʾ���е���Ŀ
			if((i+1)!=itemsize)app_draw_smooth_line(x+5,y+375+(i+1)*(itemheight+1)-1,itemwidth,1,0Xb1ffc4,0X1600b1);//������
 		}
		while(flag==0){
			tp_dev.scan(0);    
			in_obj.get_key(&tp_dev,IN_TYPE_TOUCH);	//�õ�������ֵ   
			
			for(i=0;i<7;i++){
				res=btn_check(tbtn[i],&in_obj);		//ȷ�ϰ�ť���
				if(res){
					if((tbtn[i]->sta&0X80)==0){		//����Ч����
						switch(i){
							case 0://ʱ�����Ӱ�ť������
								(*hour)++;
								if(*hour>23)*hour=0;
								break;
							case 1://ʱ�Ӽ��ٰ�ť������	  
								if(*hour)(*hour)--;
								else *hour=23;
								break;
							case 2://�������Ӱ�ť������
								(*min)++;
								if(*min>59)(*min)=0;
								break;
							case 3://���Ӽ��ٰ�ť������	  
								if(*min)(*min)--;
								else *min=59;
								break;
							case 4://ȷ�ϰ�ť����
								if(mode&(1<<7)){
									(*is_open80)=0X80;
									rval=SWITCH_CLOCK_SETTING;
									flag=1;
								}
								break;	  
							case 5://ȡ����ť����	  
								rval=HOLD_CLOCK_SETTING;
								flag=1;
								break;
							case 6:
								if(mode&(1<<7)){
									if(((*is_open80)==0X80)|((*is_open80)==0X50)){
										(*is_open80)=0X00;//�ر�
										rval=CLOSE_CLOCK;
									}
									else{
										(*is_open80)=0X01;//ɾ��
										rval=SWITCH_CLOCK_SETTING;
									}
									flag=1;
								}
								break;
 						}
					}
					app_show_nummid(x+120,y+114+82,SYSSET_EDIT_WIDTH,SYSSET_EDIT_HEIGHT,*hour,2,24,BLACK,SYSSET_EDIT_BACK_COLOR);
					app_show_nummid(x+120+130,y+114+82,SYSSET_EDIT_WIDTH,SYSSET_EDIT_HEIGHT,*min,2,24,BLACK,SYSSET_EDIT_BACK_COLOR);  
				}//if(res)
			}//for(i=0;i<6;i++)
			
			temp=0XFF;//�����,���Ϊ0XFF,���ɿ���ʱ��,˵���ǲ�����Ч�����ڵ�.�����0XFF,���ʾTP�ɿ���ʱ��,������Ч������.
			for(i=0;i<itemsize;i++){
				if(tp_dev.sta&TP_PRES_DOWN){//������������
				 	if(app_tp_is_in_area(&tp_dev,x+5,y+375+i*(itemheight+1),itemwidth,itemheight)){//�ж�ĳ��ʱ��,��������ֵ�ǲ�����ĳ��������
						if((selsta&0X80)==0){//��û�а��¹�
							icopath=app_get_icopath(mode&(1<<5),selpath,unselpath,*selx,i); //�õ�ͼ��·��
							app_show_items(x+5,y+375+i*(itemheight+1),itemwidth,itemheight,items[i],icopath,BLACK,APP_ITEM_SEL_BKCOLOR);//��ѡ��Ŀ
							selsta=i;		//��¼��һ�ΰ��µ���Ŀ
							selsta|=0X80;	//����Ѿ����¹���
						}
						break;		
					}
				}else{ //���������ɿ���
				 	if(app_tp_is_in_area(&tp_dev,x+5,y+375+i*(itemheight+1),itemwidth,itemheight)){//�ж�ĳ��ʱ��,��������ֵ�ǲ�����ĳ��������
						temp=i;	   
						break;
					}
				}
			}
			if((selsta&0X80)&&(tp_dev.sta&TP_PRES_DOWN)==0){//�а��¹�,�Ұ����ɿ���
				if((selsta&0X0F)==temp){//�ɿ�֮ǰ������Ҳ���ڰ���ʱ��������.
					if(mode&(1<<5)){//��ѡģʽ,ִ��ȡ������
						if((*selx)&(1<<temp))*selx&=~(1<<temp);
						else *selx|=1<<temp;
					}else{//��ѡģʽ																				  
						app_show_items(x+5,y+375+(*selx)*(itemheight+1),itemwidth,itemheight,items[*selx],unselpath,BLACK,twin->windowbkc);//ȡ��֮ǰѡ�����Ŀ
						*selx=temp;
					}
				}else temp=selsta&0X0F;//�õ���ʱ���µ���Ŀ��
 				icopath=app_get_icopath(mode&(1<<5),selpath,unselpath,*selx,temp); //�õ�ͼ��·��
				app_show_items(x+5,y+375+temp*(itemheight+1),itemwidth,itemheight,items[temp],icopath,BLACK,twin->windowbkc);//��ѡ��Ŀ
				selsta=0;//ȡ��
			}
 		}
 	}
	/////////////////////////////////////////////////////////////////
	window_delete(twin);				//ɾ������
	for(i=0;i<6;i++)btn_delete(tbtn[i]);//ɾ����ť
	printf("app_items_sel:%d\n\n",rval);
	return rval;
} 

