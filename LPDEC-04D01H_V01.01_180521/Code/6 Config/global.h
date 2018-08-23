/******************************************************************
** ��Ȩ:       �������ĵ��Ƽ����޹�˾
** �ļ���:     global.h
** �汾��
** ��������:   IAR 1.42
** ����:       li
** ��������:   2017.8.18
** ����:       ���ļ���Ҫ���ڽṹ�嶨�壬������ĵĺ궨���Լ��ļ�����
** ����ļ�:
** �޸���־:
*******************************************************************/
#ifndef __GLOBAL_H
#define __GLOBAL_H

#include "main.h"


/********************  Task Layer  *******************/
extern    u8        g_Maintask;        //ϵͳ������
extern    u8        ga_Subtask[8];     //ϵͳ������

#define Var_Fill(var,data)      for(u16 ii = 0; ii < sizeof(var); ++ii)\
                                *(((u8*)(&var))+ii) = data              //�����var�����data


#define NumberOfElements(x) (sizeof(x)/sizeof((x)[0]))
#define TmrSecondsToMilliseconds( n )   ( (U32) ((n) * 1000) )

                                  
#define SET_BIT(REG, BIT)     ((REG) |= (BIT))

#define CLEAR_BIT(REG, BIT)   ((REG) &= ~(BIT))

#define READ_BIT(REG, BIT)    ((REG) & (BIT))

#define CLEAR_REG(REG)        ((REG) = (0x0))

#define WRITE_REG(REG, VAL)   ((REG) = (VAL))

#define READ_REG(REG)         ((REG))


#define SYS_SoftVerMessLen   GPRS_CommPacket.MessRead.softwareVerLen
#define SYS_SoftVerMess      GPRS_CommPacket.MessRead.softwareVer
#define SYS_HardVerMessLen   GPRS_CommPacket.MessRead.hardwareVerLen
#define SYS_HardVerMess      GPRS_CommPacket.MessRead.hardwareVer

typedef struct
{
  u8  writeRom_F        :1;    //��EEPROMдʧ�ܣ�������ñ�־
  u8  writeDynamic_F    :1;
  u8  atReday_F         :1;    //��GPRS ģ��AT��׼��OK,��ô����ñ�־
  u8  closeStaProc      :1;    //��ģ�鸴λ�����������ޣ���ô��ʱ�ر�GPRS״̬�������һ��ʱ����ȥִ��
  u8  reserve           :4;
  
}Task_Flag_Type;


extern Task_Flag_Type Task_Flag_Struct;


#endif



