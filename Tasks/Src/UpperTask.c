/**
  ******************************************************************************
  * File Name          : UpperTask.c
  * Description        : 上位机处理任务，进行串口调试
  ******************************************************************************
  *
  * Copyright (c) 2018 Team TPP-Shanghai Jiao Tong University
  * All rights reserved.
  *
  ******************************************************************************
  */
#include "includes.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

int16_t times=0;
#ifdef DEBUG_MODE
//--------------------底层接收驱动部分-------------------//
uint8_t data;
uint8_t buf[REC_LEN];
uint16_t RX_STA=0;
void zykReceiveData(uint8_t data);
void ctrlUartRxCpltCallback()
{
	zykReceiveData(data);
	HAL_UART_AbortReceive((&CTRL_UART));
	if(HAL_UART_Receive_DMA(&CTRL_UART, &data, 1) != HAL_OK)
	{
		Error_Handler();
		printf( "CtrlUart error" );
	} 
}

void ctrlUartInit(){
	if(HAL_UART_Receive_DMA(&CTRL_UART, &data, 1) != HAL_OK){
		Error_Handler();
		printf( "InitCtrlUart error" );
	} 
}

void zykReceiveData(uint8_t data)
{
		if((RX_STA&0x8000)==0)
		{
			if(RX_STA&0x4000)
			{
				if(data!=0x0a)
				{
						RX_STA=0;
				}
				else 
				{
					RX_STA|=0x8000;	
					buf[RX_LEN]='\0';   
				}
			}
			else 
			{	
				if(data==0x0d)RX_STA|=0x4000;
				else
				{
					buf[RX_STA&0X3FFF]=data ;
					RX_STA++;
					if(RX_STA>(REC_LEN-1))RX_STA=0; 
				}		 
			}
		}
}

//--------------------数据解析协议部分-------------------//
uint8_t ComProtocal(char*rxbuf,char*head,char*end,char* separater,char dataout[][15])
{
    uint8_t headlength,endlength,datalength,totallength;
    uint8_t i=0;
    char temp[50]="";
    char*splitchar;
    headlength=strlen(head);
    endlength=strlen(end);
    totallength=strlen(rxbuf);
		datalength=totallength-headlength-endlength;
    strncpy(temp,rxbuf,headlength);
    temp[headlength]='\0';
    if(strcmp(temp,head))
    {
        return 0;
    }
    strncpy(temp,rxbuf+totallength-endlength,endlength);
    temp[endlength]='\0';
    if(strcmp(temp,end))
    {
        return 0;
    }
    strncpy(temp,rxbuf+headlength,datalength);
    temp[datalength]='\0';

    splitchar=strtok((char*)temp,separater);
    while(splitchar!=NULL)
    {
        sprintf(dataout[i++],"%s",splitchar);
        splitchar=strtok(NULL,separater);
    }
    return i;
}

//--------------------任务循环部分-------------------//
//debug监测变量
extern int16_t channel0,channel1,channel2,channel3;
extern int16_t GMYAWIntensity,GMPITCHIntensity,AMFBIntensity,AMUD1Intensity,AMUD2Intensity;
extern int16_t CMBRIntensity,CMBLIntensity,CMFRIntensity,CMFLIntensity;
extern double GMYAWRealAngle,GMPITCHRealAngle,AMFBRealAngle,AMUD1RealAngle,AMUD2RealAngle;
extern int16_t times;
void dataCallBack()
{
	static uint16_t pcnt = 0;
	if(pcnt>100)
		{
			//printf("GMYAW:\t Intensity %d,RealAngle %f,TargetAngle %f\r\n", GMYAWIntensity,GMYAWRealAngle,GMYAWAngleTarget);
			printf("GMPITCH:\t Intensity %d,RealAngle %f,TargetAngle %f\r\n", GMPITCHIntensity,GMPITCHRealAngle,GMPITCHAngleTarget);
			
			//printf("AMFB:\t Intensity %d,RealAngle %f,TargetAngle %f\r\n", AMFBIntensity,AMFBRealAngle,AMFBAngleTarget);
			//printf("AMUD1:\t Intensity %d,RealAngle %f,TargetAngle %f\r\n", AMUD1Intensity,AMUD1RealAngle,AMUD1AngleTarget);
			//printf("AMUD2:\t Intensity %d,RealAngle %f,TargetAngle %f\r\n", AMUD2Intensity,AMUD2RealAngle,AMUD2AngleTarget);
			
			
			//printf("CMIntensity %d %d %d %d \n",CMBRIntensity,CMBLIntensity,CMFRIntensity,CMFLIntensity);
			//printf("CMrx angle %d %d %d %d \n",CMFRRx.angle,CMFLRx.angle,CMBRRx.angle,CMBLRx.angle);
			printf("CMrx speed %d %d %d %d \n",CMFRRx.RotateSpeed,CMFLRx.RotateSpeed,CMBRRx.RotateSpeed,CMBLRx.RotateSpeed);
			//printf("AMrx angle SIDE%d UD%d %d FB%d GMPITCH%d \n",GMYAWRx.angle,AMUD1Rx.angle,AMUD2Rx.angle,AMFBRx.angle,GMPITCHRx.angle);
			
			//printf("can1 update%d can2 update%d type%d\n",can1_update,can2_update,can_type);
			//printf("Channel %d %d %d %d \n",channel0,channel1,channel2,channel3);
			
			//printf("times %d \n",times);
			
			pcnt = 0;
		}
		else pcnt++;
}
#endif
