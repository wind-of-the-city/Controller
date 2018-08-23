#include "bsp_iwdg.h"

void IWDG_Init(void)
{
  IWDG_SetReload(0xff);      //���ؼĴ���д��255
                            //��д��ֵ ����д0XCC , ��д0X55
  IWDG_Enable();  //CC
  IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable);  //55
  
  IWDG_SetPrescaler(IWDG_Prescaler_256);   //256*256/38khz=1.72s
  Feed_The_IWDog();
}

void Feed_The_IWDog(void)
{
  IWDG_ReloadCounter();
}