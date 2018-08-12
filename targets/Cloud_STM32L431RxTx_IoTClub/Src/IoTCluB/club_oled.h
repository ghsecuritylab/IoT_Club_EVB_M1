/**
 *Copyright: Copyright (c) 2018
 *Created on 2018-8-6
 *Author: IoTCluB
 *Version 0.1
 *Title: Club oled.h
 *Addr: www.iot-club.cn
**/

#ifndef __CLUB_OLED__
#define __CLUB_OLED__

#include "sys_init.h"
#include "club_func.h"
#include "oled.h"

#define OLED_MAX_QUEUE 5


typedef struct{
	char 	*data;		//���ݵ�ַָ��
	UINT32	dataLen;	//���ݳ���
	UINT8	x;			//x ��ַ
	UINT8 	y;			//y ��ַ
	UINT8	size;		//�����С
	UINT8 	font;		//ѡ������
	UINT8	autoEnter;	//�Ƿ��Զ�����
}OLED_Data_Type;


void club_oled_init();
void proc_oled_task(UINT32 uwArg);
void club_oled_send_data(char *data, UINT8 dataLen, UINT8 x, UINT8 y, UINT8 size);

extern UINT32 g_oledQueue;



#endif
