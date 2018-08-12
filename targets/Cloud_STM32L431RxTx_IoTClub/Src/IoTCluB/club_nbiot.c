/**
 *Copyright: Copyright (c) 2018
 *Created on 2018-8-6
 *Author: IoTCluB
 *Version 0.1
 *Title: Club nbiot.h
 *Addr: www.iot-club.cn
**/
#if defined(WITH_AT_FRAMEWORK) && defined(USE_NB_NEUL95)

#include "club_nbiot.h"

UINT32 				g_nbiotQueue = 0;	//nbiot��Ϣ���
platformAddrType 	platformAddr = {0};
NBIOT_Queue_Type	nbiot_queue;
UINT32 				nbiot_data_pool[NBIOT_MAX_MEM/4];
//��ַ��ֵ

extern int32_t nb_cmd_data_ioctl(void* arg,int8_t * buf, int32_t len);
void club_nbiot_init(char *ipAddr, char *port){
	
	UINT32 uwRet;
	platformAddr.IPAddr 	= 	(UINT8*)ipAddr;
	platformAddr.Port 		=	(UINT8*)port;


	
	//����һ��NBIOT���ݶ���
	uwRet = LOS_QueueCreate("NBIOT", NBIOT_MAX_QUEUE, &g_nbiotQueue, 0, sizeof(NBIOT_Queue_Type));
	if(uwRet != LOS_OK){
		CLUB_DEBUG("NBIOT QUEUE %d create ERROR:%d\r\n", g_nbiotQueue, uwRet);
	}else{
		CLUB_DEBUG("NBIOT QUEUE %d create OK:%d\r\n", g_nbiotQueue, uwRet);
	}
	
	//����һ�δ洢��ʱ�������ݵ��ڴ�
	uwRet  = LOS_MemInit(nbiot_data_pool, NBIOT_MAX_MEM);
	if(uwRet != LOS_OK){
		CLUB_DEBUG("NBIOT data pool is error:%d\r\n", uwRet);
	}else{
		CLUB_DEBUG("NBIOT data pool is ok\r\n");
	}
	
	
	//������ƽ̨
	club_nbiot_connect();
	
	//ע��ƽ̨���ݻص�
	club_nbiot_URC_call("+NNMI", strlen("+NNMI:"), nb_cmd_data_ioctl);

}

//���ӵ�ƽ̨
//NB_STAT_OK
//NB_STAT_ERR_SIM
//NB_STAT_ERR_NET

NBIOT_CON_STAT club_nbiot_connect(){
	los_nb_init((const int8_t* )(platformAddr.IPAddr), (const int8_t*)(platformAddr.Port), NULL);
	return NB_STAT_OK;
}

//ע��URC�ϱ���Ϣ��һ���յ�ע����Ϣ���ûص�����
void club_nbiot_URC_call(char *URCStr, UINT16 dataLen, oob_callback callback){
	los_nb_notify(URCStr, dataLen, callback);
}

//����NBIOT
int club_nbiot_reset(){
	return 0;
}

//������Ϣ��OCƽ̨
int club_nbiot_send(char *data, UINT32 dataLen){

	UINT32 uwRet;
	UINT32 *dataBufTemp 					= 	NULL;
	
	NBIOT_Queue_Type	nbiotSendTemp 	= 	{NB_MSG_SEND_DATA, 0, 0};
	dataBufTemp 						= 	LOS_MemAlloc(nbiot_data_pool, dataLen);	//����һ���ڴ�
	nbiotSendTemp.msgParam1 			=	(UINT32*)dataBufTemp;					//����ָ���ַ
	nbiotSendTemp.msgParam2				= 	(UINT32*)dataLen;						//���ݳ���
	
	if(dataBufTemp != NULL){
		memcpy(dataBufTemp,data,dataLen);
		//�ڴ�����ɹ�������Ϣ��ʽ���͸�NB����
		uwRet = LOS_QueueWriteCopy(g_nbiotQueue, &nbiotSendTemp, sizeof(nbiotSendTemp), LOS_NO_WAIT);
		if(uwRet != LOS_OK){
			CLUB_DEBUG("Send MSG Data error:%d\r\n", uwRet);
		}	
		
	}
	else{
		
		//�ڴ�����ʧ��
		CLUB_DEBUG("Mem is error\r\n");
		
	}

	return uwRet;
}

//nbiot����
void proc_nbiot_task(UINT32 uwArg){
	
	UINT32 uwRet, dataLen;
	int ret;
	NBIOT_Queue_Type	nbiotRecv;
	UINT32 *pSendData = NULL;
	UINT32 SendDataLen = 0;
	
	printf("This is nbiot task\r\n");
	for(;;){
		uwRet = LOS_QueueReadCopy(g_nbiotQueue, &nbiotRecv, &dataLen, LOS_WAIT_FOREVER);
		if(uwRet != LOS_OK){
			//��ȡ��Ϣʧ��
			CLUB_DEBUG("Com Recv error:%d\r\n", uwRet);
		}	
		
		switch(nbiotRecv.msgType){
			case NB_MSG_REBOOT:		//�յ�����NB����
			{
				break;
			}
			case NB_MSG_SEND_DATA:	//�յ�����NB��Ϣ����
			{
				pSendData = (UINT32 *)(nbiotRecv.msgParam1);
				SendDataLen = (UINT32)(nbiotRecv.msgParam2);
				ret = los_nb_report((const char*)pSendData, SendDataLen);				
				if(ret >= 0)
				printf("ocean_send_data OK!\n");                                                //�����ɹ�
		    else                                                                            //����ʧ��
				printf("ocean_send_data Fail!\n"); 			
				LOS_MemFree(nbiot_data_pool, pSendData);	//�ͷ��ڴ�
				break;
			}
			default:
				break;
			
		}

	}	
	
}
#endif

