#include "bsp_tim.h"

static U16 s_MsCount=0;;

Tim_Type Tim_Struct;

void Bsp_Tim5_Init(void)
{
 	CLK_PeripheralClockConfig(CLK_Peripheral_TIM5 , ENABLE);              //ʹ�ܶ�ʱ��5ʱ��
  	TIM5_TimeBaseInit(TIM5_Prescaler_128 , TIM5_CounterMode_Up , 16000);    //���ö�ʱ��5Ϊ128��Ƶ�����ϼ���������ֵΪ16000��Ϊ7.8125��ļ���ֵ
  	TIM5_ITConfig(TIM5_IT_Update , ENABLE);     //ʹ�����ϼ�������ж�
  	TIM5_ARRPreloadConfig(ENABLE);  //ʹ�ܶ�ʱ��5�Զ����ع���
  	TIM5_Cmd(ENABLE);               //������ʱ��5��ʼ����
}

void Bsp_Tim2_Init(void)
{
    CLK_PeripheralClockConfig(CLK_Peripheral_TIM2 , ENABLE);              //ʹ�ܶ�ʱ��2ʱ��
    TIM2_TimeBaseInit(TIM2_Prescaler_16 , TIM2_CounterMode_Up , 10000);    //���ö�ʱ��2Ϊ16��Ƶ�����ϼ���������ֵΪ10000��Ϊ10����ļ���ֵ
    TIM2_ITConfig(TIM2_IT_Update , ENABLE);     //ʹ�����ϼ�������ж�
    TIM2_ARRPreloadConfig(ENABLE);  //ʹ�ܶ�ʱ��2�Զ����ع���
    TIM2_Cmd(ENABLE);               //������ʱ��2��ʼ����
}


void Bsp_Tim3_Delay_Init(void)
{
    CLK_PeripheralClockConfig(CLK_Peripheral_TIM3 , ENABLE);              //ʹ�ܶ�ʱ��3ʱ��
    TIM3_TimeBaseInit(TIM3_Prescaler_1 , TIM3_CounterMode_Up , 16000);    //���ö�ʱ��3Ϊ1��Ƶ�����ϼ���������ֵΪ16000��Ϊ1����ļ���ֵ
}

void Bsp_Tim3_Delay_Ms(u16 ms)
{
    TIM3_ARRPreloadConfig(ENABLE);  //ʹ�ܶ�ʱ��2�Զ����ع���
    TIM3_Cmd(ENABLE);               //������ʱ��2��ʼ����
    while(ms--)
    {
        while( TIM3_GetFlagStatus(TIM3_FLAG_Update) == RESET); //�ȴ������Ƿ�ﵽ1����
        TIM3_ClearFlag(TIM3_FLAG_Update);  //�������1���룬�����Ӧ�ı�־
    }
    TIM3_Cmd(DISABLE);                   //��ʱȫ���������رն�ʱ��2
}


void Bsp_Tim_Update(void)   //10MSִ������
{
    s_MsCount++;
    if(Task_Flag_Struct.closeStaProc == DISABLE)
    {
        APP_GPRS_TimeProc(10);
    }
    Bsp_Led_Tim(10);
    Bsp_Key_Tim(10);
    if(s_MsCount >= MS_TO_S)     // 1Sʱ�䵽
    {
         s_MsCount =0;
        // BSP_LED_FLICKER;         
         if(Task_Flag_Struct.writeRom_F != DISABLE || Task_Flag_Struct.writeDynamic_F != DISABLE)
         {
              Tim_Struct.rewriteCount++;
              if(Tim_Struct.rewriteCount >= 180)   //��дδ�ɹ�����ô���3MIN����д
              {
                   Tim_Struct.rewriteCount = 0; 
                   if(Task_Flag_Struct.writeRom_F != DISABLE)
                   {
                        Set_Task(MEM_WRITE,MEM_STORE_SOLID_ROMDATA);
                   }
                   if(Task_Flag_Struct.writeDynamic_F != DISABLE)
                   {
                        Set_Task(MEM_WRITE,MEM_STORE_DYNAMIC_DATA);
                   }
              
              }
         }
         if(Task_Flag_Struct.closeStaProc != DISABLE) //���´�GPRS״̬�������
         {
              Tim_Struct.reOpenStaProcCount++;
              if(Tim_Struct.reOpenStaProcCount >= RE_OPEN_STA_PROC_TIME)
              {
                   Tim_Struct.reOpenStaProcCount = 0;
                   Task_Flag_Struct.closeStaProc = DISABLE;
              }
         }
    }
}















