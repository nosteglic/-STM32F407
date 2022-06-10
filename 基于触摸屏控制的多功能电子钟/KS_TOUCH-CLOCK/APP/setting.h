#ifndef __SETTING_H
#define __SETTING_H
#include "sys.h"
#include "includes.h" 	   	 

#define SYSSET_BTN1_WIDTH			100			//һ�ఴ�����(�Ӽ���ť)
#define SYSSET_BTN1_HEIGHT			80			//һ�ఴ���߶�(�Ӽ���ť)
#define SYSSET_BTN2_WIDTH			120			//���ఴ�����(ȷ��/ȡ����ť)
#define SYSSET_BTN2_HEIGHT			60			//���ఴ���߶�(ȷ��/ȡ����ť)
#define SYSSET_BTN3_WIDTH			100			//���ఴ�����(������ť�Ŀ��)
#define SYSSET_EDIT_WIDTH			100			//���ֱ༭���Ŀ��
#define SYSSET_EDIT_HEIGHT			50			//���ֱ༭���ĸ߶�
#define SYSSET_EDIT_BACK_COLOR		GBLUE		//��ʾ����������ɫ

//��������ѡ����Ŀ��������Ϣ
#define APP_ITEM_BTN1_WIDTH		60	  		//��2������ʱ�Ŀ��
#define APP_ITEM_BTN2_WIDTH		100			//ֻ��1������ʱ�Ŀ��
#define APP_ITEM_BTN_HEIGHT		30			//�����߶�
#define APP_ITEM_ICO_SIZE		32			//ICOͼ��ĳߴ�

#define APP_ITEM_SEL_BKCOLOR	0X0EC3		//ѡ��ʱ�ı���ɫ
#define APP_WIN_BACK_COLOR	 	0XC618		//���屳��ɫ

//��ֵ����
#define	app_pi	3.1415926535897932384626433832795

u8 sysset_time_set(u16 x,u16 y,u8 *hour,u8 *min,u8*caption);
u8 sysset_is_leap_year(u16 year);
u8 sysset_date_set(u16 x,u16 y,u16 *year,u8 *month,u8 *date,u8*caption);
void app_show_nummid(u16 x,u16 y,u16 width,u16 height,u32 num,u8 len,u8 size,u16 ptcolor,u16 bkcolor);
u8 app_listbox_select(u8 *sel,u8 *top,u8 * caption,u8 *items[],u8 itemsize);
void app_filebrower(u8 *topname,u8 mode);
void app_gui_tcbar(u16 x,u16 y,u16 width,u16 height,u8 mode);
u8 * set_search_caption(const u8 *mcaption);
u8 app_items_sel(u16 x,u16 y,u16 width,u16 height,u8 *items[],u8 itemsize,u8 *hour,u8 *min,u8 *selx,u8 mode,u8*caption,u8 *is_open80);
#endif
