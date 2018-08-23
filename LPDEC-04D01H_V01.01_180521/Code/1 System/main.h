/******************************************************************
** ��Ȩ:       �������ĵ��Ƽ����޹�˾
** �ļ���:     main.h
** �汾��
** ��������:   IAR 1.42
** ����:       li
** ��������:   2017.8.18
** ����:       �ļ�����
** ����ļ�:
** �޸���־:
*******************************************************************/

#ifndef  __MAIN_H
#define  __MAIN_H


//#include "global.h"

#include "stm8l15x.h"
#include "config.h"

#include "1 task_safeguard.h"
#include "2 task_control.h"
#include "3 task_mem_read.h"
#include "4 task_mem_write.h"
#include "5 task_measure.h"
#include "6 task_communicate.h"
#include "7 task_net_connect.h"
#include "8 task_interaction.h"
#include "task_common.h"

#include "app_clock.h"
#include "app_comm_g510.h"
#include "app_comm_pc.h"
#include "app_common.h"

#include "bsp_gpio.h"
#include "bsp_gprs.h"
#include "bsp_iwdg.h"
#include "bsp_led.h"
#include "bsp_tim.h"
#include "bsp_uart.h"
#include "bsp_rcc.h"
#include "bsp_mem.h"
#include "bsp_var.h"
#include "bsp_common.h"
#include "bsp_key.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include <math.h>
#include <time.h>

#endif