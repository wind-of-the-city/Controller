/******************************************************************
** ��Ȩ:       �������ĵ��Ƽ����޹�˾
** �ļ���:     main.c
** �汾��
** ��������:   IAR FOR STM8 1.42
** ����:       XXX
** ��������:   2018.1.8
** ����:       ������  ���ϵͳ��ʼ�����������
** ����ļ�:
** �޸���־:
*******************************************************************/
#include "global.h"

void main(void)
{
  if(Com_Bsp_Init())
  {
     for(;;)
     {
        while(g_Maintask)
        {
           u8 m_temp=ga_TaskMapTable[g_Maintask];
           Task_Struct[m_temp].function(m_temp);
        }
        Feed_The_IWDog();
     }
  }
}




















#ifdef  USE_FULL_ASSERT

/**
  * @brief  Reports the name of the source file and the source line number
  *   where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t* file, uint32_t line)
{
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1)
  {
  }
}
#endif

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
