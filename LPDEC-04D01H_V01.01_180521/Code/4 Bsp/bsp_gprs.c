#include "bsp_gprs.h"


void G510_Buck_EN(void)
{
    BSP_GPRS_BUCKEN_SET;
}

/*************************
    G510 Power on���̣�
    ___         ___
       |_______|
         800ms
  ��·ת�� ��->�ͣ���->��
**************************/
void G510_Power_ON(void)
{
    BSP_GPRS_POWERON_SET;
    Bsp_Tim3_Delay_Ms(850);
    BSP_GPRS_POWERON_RESET;
}

void Bsp_G510_Init(void)
{
    G510_Buck_EN();
    G510_Power_ON();
}

/*************************
    G510 Power off���̣�
    �ر�����Ӧ�ýӿں�ע������
    ___         ___
       |_______|
          3s
  ��·ת�� ��->�ͣ���->��
**************************/
void G510_Power_OFF(void)
{
    GPIO_SetBits(GPRS_POWER_ON_PORT,GPRS_POWER_ON_PIN);
    Bsp_Tim3_Delay_Ms(3500);
    GPIO_ResetBits(GPRS_POWER_ON_PORT,GPRS_POWER_ON_PIN);
}





