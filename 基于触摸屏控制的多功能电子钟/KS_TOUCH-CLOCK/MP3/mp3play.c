#include "mp3play.h"
#include "includes.h"
#include "malloc.h" 
#include "mp3dec.h"
#include "stdio.h"

/* �Ƽ�ʹ�����¸�ʽmp3�ļ���
 * �����ʣ�44100Hz
 * ��  ����2
 * �����ʣ�320kbps
 */

/* ������������Ƶ����ʱ�������������Ҫ������СΪ2304*16/8�ֽ�(16ΪPCM����Ϊ16λ)��
 * �������Ƕ���MP3BUFFER_SIZEΪ2304��ʵ�����������ΪMP3BUFFER_SIZE*2���ֽ�
 */
#define MP3BUFFER_SIZE  2304		
#define INPUTBUF_SIZE   3000	

static HMP3Decoder		Mp3Decoder;			/* mp3������ָ��	*/
static MP3FrameInfo		Mp3FrameInfo;		/* mP3֡��Ϣ  */
static MP3_TYPE mp3player;         /* mp3�����豸 */
static uint8_t Isread=0;           /* DMA������ɱ�־ */
static uint8_t bufflag=0;          /* ���ݻ�����ѡ���־ */

__mp3ctrl * mp3ctrl;	//mp3���ƽṹ�� 
__audiodev audiodev;	//���ֲ��ſ�����

uint8_t inputbuf[INPUTBUF_SIZE]={0};        /* �������뻺������1940�ֽ�Ϊ���MP3֡��С  */
static short outbuffer[2][MP3BUFFER_SIZE];  /* ���������������Ҳ��I2S�������ݣ�ʵ��ռ���ֽ�����RECBUFFER_SIZE*2 */

FIL file_1;											/* file objects */
FRESULT result; 
UINT bw1;            					/* File R/W count */

/* �������ļ��ڵ��õĺ������� */
void MP3Player_I2S_DMA_TX_Callback(void);



//����ID3V1 
//buf:�������ݻ�����(��С�̶���128�ֽ�)
//pctrl:MP3������
//����ֵ:0,��ȡ����
//    ����,��ȡʧ��
u8 mp3_id3v1_decode(u8* buf,__mp3ctrl *pctrl)
{
	ID3V1_Tag *id3v1tag;
	id3v1tag=(ID3V1_Tag*)buf;
	if (strncmp("TAG",(char*)id3v1tag->id,3)==0)//��MP3 ID3V1 TAG
	{
		if(id3v1tag->title[0])strncpy((char*)pctrl->title,(char*)id3v1tag->title,30);
		if(id3v1tag->artist[0])strncpy((char*)pctrl->artist,(char*)id3v1tag->artist,30); 
	}else return 1;
	return 0;
}
//����ID3V2 
//buf:�������ݻ�����
//size:���ݴ�С
//pctrl:MP3������
//����ֵ:0,��ȡ����
//    ����,��ȡʧ��
u8 mp3_id3v2_decode(u8* buf,u32 size,__mp3ctrl *pctrl)
{
	ID3V2_TagHead *taghead;
	ID3V23_FrameHead *framehead; 
	u32 t;
	u32 tagsize;	//tag��С
	u32 frame_size;	//֡��С 
	taghead=(ID3V2_TagHead*)buf; 
	if(strncmp("ID3",(const char*)taghead->id,3)==0)//����ID3?
	{
		tagsize=((u32)taghead->size[0]<<21)|((u32)taghead->size[1]<<14)|((u16)taghead->size[2]<<7)|taghead->size[3];//�õ�tag ��С
		pctrl->datastart=tagsize;		//�õ�mp3���ݿ�ʼ��ƫ����
		if(tagsize>size)tagsize=size;	//tagsize��������bufsize��ʱ��,ֻ��������size��С������
		if(taghead->mversion<3)
		{
			printf("not supported mversion!\r\n");
			return 1;
		}
		t=10;
		while(t<tagsize)
		{
			framehead=(ID3V23_FrameHead*)(buf+t);
			frame_size=((u32)framehead->size[0]<<24)|((u32)framehead->size[1]<<16)|((u32)framehead->size[2]<<8)|framehead->size[3];//�õ�֡��С
 			if (strncmp("TT2",(char*)framehead->id,3)==0||strncmp("TIT2",(char*)framehead->id,4)==0)//�ҵ���������֡,��֧��unicode��ʽ!!
			{
				strncpy((char*)pctrl->title,(char*)(buf+t+sizeof(ID3V23_FrameHead)+1),AUDIO_MIN(frame_size-1,MP3_TITSIZE_MAX-1));
			}
 			if (strncmp("TP1",(char*)framehead->id,3)==0||strncmp("TPE1",(char*)framehead->id,4)==0)//�ҵ�����������֡
			{
				strncpy((char*)pctrl->artist,(char*)(buf+t+sizeof(ID3V23_FrameHead)+1),AUDIO_MIN(frame_size-1,MP3_ARTSIZE_MAX-1));
			}
			t+=frame_size+sizeof(ID3V23_FrameHead);
		} 
	}else pctrl->datastart=0;//������ID3,mp3�����Ǵ�0��ʼ
	return 0;
} 

//��ȡMP3������Ϣ
//pname:MP3�ļ�·��
//pctrl:MP3������Ϣ�ṹ�� 
//����ֵ:0,�ɹ�
//    ����,ʧ��
u8 mp3_get_info(u8 *pname,__mp3ctrl* pctrl)
{
    HMP3Decoder decoder;
    MP3FrameInfo frame_info;
	MP3_FrameXing* fxing;
	MP3_FrameVBRI* fvbri;
	FIL*fmp3;
	u8 *buf;
	u32 br;
	u8 res;
	int offset=0;
	u32 p;
	short samples_per_frame;	//һ֡�Ĳ�������
	u32 totframes;				//��֡��
	
	fmp3=mymalloc(SRAMIN,sizeof(FIL)); 
	buf=mymalloc(SRAMIN,5*1024);		//����5K�ڴ� 
	if(fmp3&&buf)//�ڴ�����ɹ�
	{ 		
		f_open(fmp3,(const TCHAR*)pname,FA_READ);//���ļ�
		res=f_read(fmp3,(char*)buf,5*1024,&br);
		if(res==0)//��ȡ�ļ��ɹ�,��ʼ����ID3V2/ID3V1�Լ���ȡMP3��Ϣ
		{  
			mp3_id3v2_decode(buf,br,pctrl);	//����ID3V2����
			f_lseek(fmp3,fmp3->fsize-128);	//ƫ�Ƶ�����128��λ��
			f_read(fmp3,(char*)buf,128,&br);//��ȡ128�ֽ�
			mp3_id3v1_decode(buf,pctrl);	//����ID3V1����  
			decoder=MP3InitDecoder(); 		//MP3���������ڴ�
			f_lseek(fmp3,pctrl->datastart);	//ƫ�Ƶ����ݿ�ʼ�ĵط�
			f_read(fmp3,(char*)buf,5*1024,&br);	//��ȡ5K�ֽ�mp3����
 			offset=MP3FindSyncWord(buf,br);	//����֡ͬ����Ϣ
			if(offset>=0&&MP3GetNextFrameInfo(decoder,&frame_info,&buf[offset])==0)//�ҵ�֡ͬ����Ϣ��,����һ����Ϣ��ȡ����	
			{ 
				p=offset+4+32;
				fvbri=(MP3_FrameVBRI*)(buf+p);
				if(strncmp("VBRI",(char*)fvbri->id,4)==0)//����VBRI֡(VBR��ʽ)
				{
					if (frame_info.version==MPEG1)samples_per_frame=1152;//MPEG1,layer3ÿ֡����������1152
					else samples_per_frame=576;//MPEG2/MPEG2.5,layer3ÿ֡����������576 
 					totframes=((u32)fvbri->frames[0]<<24)|((u32)fvbri->frames[1]<<16)|((u16)fvbri->frames[2]<<8)|fvbri->frames[3];//�õ���֡��
					pctrl->totsec=totframes*samples_per_frame/frame_info.samprate;//�õ��ļ��ܳ���
				}else	//����VBRI֡,�����ǲ���Xing֡(VBR��ʽ)
				{  
					if (frame_info.version==MPEG1)	//MPEG1 
					{
						p=frame_info.nChans==2?32:17;
						samples_per_frame = 1152;	//MPEG1,layer3ÿ֡����������1152
					}else
					{
						p=frame_info.nChans==2?17:9;
						samples_per_frame=576;		//MPEG2/MPEG2.5,layer3ÿ֡����������576
					}
					p+=offset+4;
					fxing=(MP3_FrameXing*)(buf+p);
					if(strncmp("Xing",(char*)fxing->id,4)==0||strncmp("Info",(char*)fxing->id,4)==0)//��Xng֡
					{
						if(fxing->flags[3]&0X01)//������frame�ֶ�
						{
							totframes=((u32)fxing->frames[0]<<24)|((u32)fxing->frames[1]<<16)|((u16)fxing->frames[2]<<8)|fxing->frames[3];//�õ���֡��
							pctrl->totsec=totframes*samples_per_frame/frame_info.samprate;//�õ��ļ��ܳ���,�������ʱ�䣬��mp3_get_curtime��
						}else	//��������frames�ֶ�
						{
							pctrl->totsec=fmp3->fsize/(frame_info.bitrate/8);
						} 
					}else 		//CBR��ʽ,ֱ�Ӽ����ܲ���ʱ��
					{
						pctrl->totsec=fmp3->fsize/(frame_info.bitrate/8);	//�������ʱ�䣬��mp3_get_curtime��
					}
				} 
				pctrl->bitrate=frame_info.bitrate;			//�õ���ǰ֡������
				mp3ctrl->samplerate=frame_info.samprate; 	//�õ�������. 
				if(frame_info.nChans==2)mp3ctrl->outsamples=frame_info.outputSamps; //���PCM��������С 
				else mp3ctrl->outsamples=frame_info.outputSamps*2; //���PCM��������С,���ڵ�����MP3,ֱ��*2,����Ϊ˫�������
			}else res=0XFE;//δ�ҵ�ͬ��֡	
			MP3FreeDecoder(decoder);//�ͷ��ڴ�		
		} 
		f_close(fmp3);
	}else res=0XFF;
	myfree(SRAMIN,fmp3);
	myfree(SRAMIN,buf);	
	return res;	
}  
//�õ���ǰ����ʱ��
//fx:�ļ�ָ��
//mp3x:mp3���ſ�����
void mp3_get_curtime(FIL*fx,__mp3ctrl *mp3x)	//mp3x->totsec��mp3x->datastart��p3_get_info�������Ѿ���ȡ
{
	u32 fpos=0;  	 
	if(fx->fptr>mp3x->datastart)fpos=fx->fptr-mp3x->datastart;	//�õ���ǰ�ļ����ŵ��ĵط� 
	mp3x->cursec=fpos*mp3x->totsec/(fx->fsize-mp3x->datastart);	//��ǰ���ŵ��ڶ�������?	
}
//mp3�ļ�������˺���
//pos:��Ҫ��λ�����ļ�λ��
//����ֵ:��ǰ�ļ�λ��(����λ��Ľ��)
u32 mp3_file_seek(u32 pos)
{
	if(pos>audiodev.file->fsize)
	{
		pos=audiodev.file->fsize;
	}
	f_lseek(audiodev.file,pos);
	return audiodev.file->fptr;
}


/**
  * @brief   MP3��ʽ��Ƶ����������
  * @param  ��
  * @retval ��
  */
uint8_t mp3PlayerDemo(u8*const mp3file)
{
	uint8_t res,key,t;
	uint8_t *read_ptr=inputbuf;
	uint32_t frames=0;
	int err=0, i=0, outputSamps=0;	
	int	read_offset = 0;				/* ��ƫ��ָ�� */
	int	bytes_left = 0;					/* ʣ���ֽ��� */	
	mp3player.ucFreq=I2S_AudioFreq_Default;		//���ò���Ƶ��
	mp3player.ucStatus=STA_IDLE;							//״̬
	mp3player.ucVolume=40;										//����
	mp3ctrl=mymalloc(SRAMIN,sizeof(__mp3ctrl)); //ΪMP3���ƽṹ�����ռ�
	result=f_open(&file_1,(const TCHAR*)mp3file,FA_READ);		//��ȡMP3�ļ�
	//����ȡ�ļ�ʧ��ʱ���򴮿����������Ϣ
	if(result!=FR_OK)
	{
		printf("Open mp3file :%s fail!!!->%d\r\n",mp3file,result);
		result = f_close (&file_1);
		return 0xf0;	/* ֹͣ���� */
	}
	printf("��ǰ�����ļ� -> %s\n",mp3file);
	res=mp3_get_info((u8 *)mp3file,mp3ctrl); //��ȡ���ŵ�mp3�ļ�����Ϣ 
	if(res!=0)return 0xf4;
	//��ʼ��MP3������
	Mp3Decoder = MP3InitDecoder();	
	if(Mp3Decoder==0)
	{
		printf("��ʼ��helix������豸\n");
		return 0xf1;	/* ֹͣ���� */
	}
	printf("��ʼ����...\n");
	delay_ms(10);	/* �ӳ�һ��ʱ�䣬�ȴ�I2S�жϽ��� */
	//�������ǲ������
	WM8978_ADDA_Cfg(1,0);	//����DAC
	WM8978_Input_Cfg(0,0,0);//�ر�����ͨ��
	WM8978_Output_Cfg(1,0);	//����DAC��� 	
	WM8978_I2S_Cfg(2,0);	//�����ֱ�׼,16λ���ݳ���
	/*  ��ʼ��������I2S  */
	I2S_Play_Stop();
	//	I2S_GPIO_Config();
	I2Sx_Mode_Config(I2S_Standard_Phillips,I2S_DataFormat_16b,mp3player.ucFreq);	
	i2s_tx_callback=MP3Player_I2S_DMA_TX_Callback;					//�ص�����
	I2Sx_TX_DMA_Init((uint16_t *)outbuffer[0],(uint16_t *)outbuffer[1],MP3BUFFER_SIZE);	//I2S,DMA��ʼ��
	bufflag=0;
	Isread=0;
	mp3player.ucStatus = STA_PLAYING;		/* ����״̬ */
  result=f_read(&file_1,inputbuf,INPUTBUF_SIZE,&bw1);
	//��ȡʧ��
	if(result!=FR_OK)
	{
		printf("��ȡ%sʧ�� -> %d\r\n",mp3file,result);
		MP3FreeDecoder(Mp3Decoder);//�ͷ��ڴ�		
		return 0xf2;
	}
	read_ptr=inputbuf;
	bytes_left=bw1;
	audiodev.status=3;//��ʼ����+����ͣ
	/* ����������ѭ���� */
	while(mp3player.ucStatus == STA_PLAYING)
	{
		//Ѱ��֡ͬ�������ص�һ��ͬ���ֵ�λ��
		read_offset = MP3FindSyncWord(read_ptr, bytes_left);			
		//û���ҵ�ͬ����
		if(read_offset < 0)																		
		{
			result=f_read(&file_1,inputbuf,INPUTBUF_SIZE,&bw1);//��ȡ
			if(result!=FR_OK)
			{
				printf("��ȡ%sʧ�� -> %d\r\n",mp3file,result);
				break;
			}
			read_ptr=inputbuf;
			bytes_left=bw1;//ʣ��δ���ֽ���
			continue;
		}
		
		read_ptr += read_offset;					//ƫ����ͬ���ֵ�λ��
		bytes_left -= read_offset;				//ͬ����֮������ݴ�С	
		if(bytes_left < 1024)							//��������
		{
			/* ע������ط���Ϊ���õ���DMA��ȡ������һ��Ҫ4�ֽڶ���  */
			i=(uint32_t)(bytes_left)&3;									//�ж϶�����ֽ�
			if(i) i=4-i;														//��Ҫ������ֽ�
			memcpy(inputbuf+i, read_ptr, bytes_left);	//�Ӷ���λ�ÿ�ʼ����
			read_ptr = inputbuf+i;										//ָ�����ݶ���λ��
			//��������
			result = f_read(&file_1, inputbuf+bytes_left+i, INPUTBUF_SIZE-bytes_left-i, &bw1);
			bytes_left += bw1;										//��Ч��������С
		}
		//��ʼ���� ������mp3����ṹ�塢������ָ�롢��������С�������ָ�롢���ݸ�ʽ
		err = MP3Decode(Mp3Decoder, &read_ptr, &bytes_left, outbuffer[bufflag], 0);					
		frames++;	
		//������
		if (err != ERR_MP3_NONE)									
		{
			switch (err)
			{
				case ERR_MP3_INDATA_UNDERFLOW:
					printf("ERR_MP3_INDATA_UNDERFLOW\r\n");
					result = f_read(&file_1, inputbuf, INPUTBUF_SIZE, &bw1);
					read_ptr = inputbuf;
					bytes_left = bw1;
					break;		
				case ERR_MP3_MAINDATA_UNDERFLOW:
					/* do nothing - next call to decode will provide more mainData */
					printf("ERR_MP3_MAINDATA_UNDERFLOW\r\n");
					break;		
				default:
					printf("UNKNOWN ERROR:%d\r\n", err);		
					// ������֡
					if (bytes_left > 0)
					{
						bytes_left --;
						read_ptr ++;
					}	
					break;
			}
			Isread=1;
		}
		else		//�����޴���׼�������������PCM
		{
			MP3GetLastFrameInfo(Mp3Decoder, &Mp3FrameInfo);		//��ȡ������Ϣ				
			/* �����DAC */
			outputSamps = Mp3FrameInfo.outputSamps;							//PCM���ݸ���
			if (outputSamps > 0)
			{
				if (Mp3FrameInfo.nChans == 1)	//������
				{
					//������������Ҫ����һ�ݵ���һ������
					for (i = outputSamps - 1; i >= 0; i--)
					{
						outbuffer[bufflag][i * 2] = outbuffer[bufflag][i];
						outbuffer[bufflag][i * 2 + 1] = outbuffer[bufflag][i];
					}
					outputSamps *= 2;
				}//if (Mp3FrameInfo.nChans == 1)	//������
			}//if (outputSamps > 0)
			
			/* ���ݽ�����Ϣ���ò����� */
			if (Mp3FrameInfo.samprate != mp3player.ucFreq)	//������ 
			{
				mp3player.ucFreq = Mp3FrameInfo.samprate;
				
				printf(" \r\n Bitrate       %dKbps", Mp3FrameInfo.bitrate/1000);
				printf(" \r\n Samprate      %dHz", mp3player.ucFreq);
				printf(" \r\n BitsPerSample %db", Mp3FrameInfo.bitsPerSample);
				printf(" \r\n nChans        %d", Mp3FrameInfo.nChans);
				printf(" \r\n Layer         %d", Mp3FrameInfo.layer);
				printf(" \r\n Version       %d", Mp3FrameInfo.version);
				printf(" \r\n OutputSamps   %d", Mp3FrameInfo.outputSamps);
				printf("\r\n");
				//I2S_AudioFreq_Default = 2��������֡��ÿ�ζ�Ҫ������
				if(mp3player.ucFreq >= I2S_AudioFreq_Default)	
				{
					//���ݲ������޸�I2S����
					I2Sx_Mode_Config(I2S_Standard_Phillips,I2S_DataFormat_16b,mp3player.ucFreq);
					I2Sx_TX_DMA_Init((uint16_t *)outbuffer[0],(uint16_t *)outbuffer[1],outputSamps);
				}
		//		audio_start();	//������				
				I2S_Play_Start();

			}
		}//else ��������
		
		if(file_1.fptr==file_1.fsize) 		//mp3�ļ���ȡ��ɣ��˳�
		{
			printf("END\r\n");
			res=KEY0_PRES;
			break;
		}	
		
		while(Isread==0)	//�ȴ�������
		{
		}
		Isread=0;		
		while(1)	//�������
		{
			key=KEY_Scan(0);
			if(key==WKUP_PRES)//��ͣ
			{
				if(audiodev.status&0X01)audiodev.status&=~(1<<0);
				else audiodev.status|=0X01;  
			}	
			if(key==KEY2_PRES||key==KEY0_PRES)//�ر�
			{
				res=key;
				mp3player.ucStatus = STA_IDLE;
				break; 
			}
			mp3_get_curtime(&file_1,mp3ctrl); //��ȡ������Ϣ	
			t++;
			if(t==20)
			{
				t=0;
				LED0=!LED0;   //LED0��˸
			}
			if((audiodev.status&0X01)==0)delay_ms(10);
			else break;
		}
	}
	I2S_Play_Stop();	//����ֹͣ
	MP3FreeDecoder(Mp3Decoder);//�ͷ��ڴ�		
	f_close(&file_1);	//�ر��ļ�
	return res;
}

/* DMA��������жϻص����� */
/* �����������Ѿ�������ɣ���Ҫ�л��������������»��������ݲ��� 
   ͬʱ��ȡWAV�ļ�������䵽�Ѳ�������  */
void MP3Player_I2S_DMA_TX_Callback(void)
{
	u16 i;
	if(DMA1_Stream4->CR&(1<<19)) //��ǰʹ��Memory1����
	{
		bufflag=0;                       //���Խ����ݶ�ȡ��������0
		if((audiodev.status&0X01)==0)
		{
			for(i=0;i<MP3BUFFER_SIZE;i++)//��ͣ
			{
				outbuffer[0][i]=0;//���0
			}
		}
	}
	else                               //��ǰʹ��Memory0����
	{
		bufflag=1;                       //���Խ����ݶ�ȡ��������1
		if((audiodev.status&0X01)==0)
		{
			for(i=0;i<MP3BUFFER_SIZE;i++)//��ͣ
			{
				outbuffer[1][i]=0;//���0
			}
		}
	}
	Isread=1;                          // DMA������ɱ�־
}







