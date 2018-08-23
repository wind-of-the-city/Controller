/*************************************************
** ��Ȩ:       ����������Ƽ����޹�˾
** �ļ���:     main.h
** �汾��      
** ��������:   
** ����:     
** ��������:   
** ����:       �ṹ�嶨��  ϵͳȫ�ֱ������弰����  
** ����ļ�:
** �޸���־:   ��
*************************************************/
#ifndef __VARIABLE__H__
#define __VARIABLE__H__

#include "Main.h"

typedef unsigned char           INT8U;  // ��������
typedef signed char             INT8S;
typedef unsigned int            INT16U;
typedef signed int              INT16S;
typedef unsigned long           INT32U;
typedef signed long             INT32S;
typedef unsigned long long      INT64U;
typedef signed long long        INT64S;
typedef float                   FP32;
typedef double                  FP64;



#define CONST        const
#define STATIC       static
#define IMPORT       extern                   //���� 
#define IMPORT       extern                   //����   

#define EXPORT       extern                   //���  
 
#define EXTERN       extern
#define INLINE       inline                   //����
#define NO_INIT      __no_init                //���ʼ��
#define __O          volatile                 //ֻд 
#define __IO         volatile                 //��д  

//////////////////////////////  ϵͳ  /////////////////////////////////////////

////////////////  ϵͳ��Ӳ��  //////////////////
#define ON                          1                   
#define OFF                         0 
#define INVALID                     0xFF 

#define SOFT_VERSION                0x01                //�̼��汾x.x BCD��

////////////////  ����  ///////////////////////         //���Բ��ֵĺ궨������ʽ�汾�ж���������Ϊ0
#define COMM_SUM_CHECK_DEBUG_EN   0                   //Э�����ݺ�У��  ��λ��ʾ��ֹУ��


////////////////  ʵʱʱ��  ///////////////////////
#define SYS_RTC_EN                      1        //ʵʱʱ�ӱ��뱣֤ÿ����ιһ�ι�
#define SYS_RTC_MAX_CNT                 8        //�ж� RTC_MAX_CNT ����Ϊ1��      RTC_MAX_CNT ������Ϊ 2��4 ������2�ı��� ��ΪLCD_Cnt ��& RTC_MAX_CNT

#define SYS_RTC_HARDWARE_EN             1                   //ʹ��Ӳ��ʱ��+������ʱ��  ÿ�����ϵͳʱ��


////////////////  UART  ///////////////////////
#define UART_EN                     1

//////////////////////////////  ͨѶ  ////////////////////////////////////////
#define COMM_EN                     1
#define COMM_RETURN_BUFF_LENGTH     70        //
#define COMM_RX_BUFF_LENGTH         40        //

//////////////// protocol  ///////////////////////
#define COMM_EN                   1
#define COMM_METER_TYPE           0x00C0   //�豸����                        

////////////////////////////////�洢///////////////////////////////////////////

#define MEM_EN                      1
#define MEM_HISTORY_LENGTH          60                  //�ɼ�¼ǰN�µ���ʷ����
#define MEM_FIRST_POR_DATA          SOFT_VERSION        //����������ʶ��ϵͳ�Ƿ�����ϵ�
                                                        //�����ϵ�����EEPROM�и����洢λ��д���ʼ���� �Ժ�ÿ���ϵ����ָ���Щ����
////////////////  �洢��ַ  ///////////////////////
//wang STM8S207R8��DATAFlash��ʼ��ַ0x004000h

#define MEM_FIRST_POR_DATA_ADDR         0x004002    //�����ϵ��ʶ

#define MEM_PAR_INFO_ADDR               0x004006    //�̶������洢�׵�ַ
#define MEM_SYS_VALVE_STATE_ADDR        0x004010    //ϵͳ����״̬�����ݴ洢��ַ
#define MEM_SYS_RUN_STATE_ADDR          0x004020    //ϵͳ����״̬���ݴ洢��ַ
#define MEM_SYS_TEMP_AND_HUD_INFO_ADDR  0x004030    //ϵͳ��ʪ�����ݴ洢��ַ
#define MEM_SYS_FAULT_STATE_ADDR        0x004040    //ϵͳ�������ݴ洢��ַ
#define MEM_RTC_ADDR                    0x004050    //ϵͳʱ��


////////////////  FLASH  ///////////////////////
#define FLASH_EN                    0




/////////////////////////////////  ͨ���Ӻ���  /////////////////////////////////////////
#define CLKS_PER_MS                  (F_MCLK*1000)
#define CLKS_PER_US                   F_MCLK

/////////////////////////////////  Task  //////////////////////////////////////////////
#define TASK0                         SG_Task
#define TASK1                         COMM_Task
#define TASK2                         MEM_Read_Task
#define TASK3                         MEM_Write_Task
#define TASK4                         Measure_Task
#define TASK5                         Reserve_Task
#define TASK6                         Contrl_Task
#define TASK7                         Other_Task

#define SG                            0      //wangw140928������-ϵͳ��ȫ����
#define SG_SYS_SCAN                   1
#define SG_LOW_VOLT_DETECT            2

#define COMM                          1      //wangw140928������-ͨ������
#define COMM_DECODE                   0
#define COMM_ACTION                   1
#define COMM_RETURN_MTASK             COMM   //Э�����ݷ��� ������ΪCOMM��LCD   //wang2014-6-20ֻ�漰�����ȼ������⣬֮ǰ�ȱ���LCD����Ϊ�˿���������ʵʱ�Ժ�
#define COMM_RETURN                   7

#define MEM_READ                      2      //wangw140928������-�洢���ݶ�����
#define MEM_FIRST_POR_DATA_RECOVER    0
#define MEM_SYS_PAR_INFO_RECOVER      1
#define MEM_VALVE_STATE_RECOVER       2
#define MEM_RUN_STATE_RECOVER         3
#define MEM_SYS_TEMP_AND_HUD_RECOVER  4
#define MEM_SYS_FAULT_STATE_RECOVER   5
#define MEM_SYS_RTC_RECOVER           6


#define MEM_WRITE                     3      //wangw140928������-�洢����д����
#define MEM_FIRST_POR_DATA_SAVE       0
#define MEM_SYS_PAR_INFO_SAVE         1
#define MEM_VALVE_STATE_SAVE          2
#define MEM_RUN_STATE_SAVE            3
#define MEM_SYS_TEMP_AND_HUD_SAVE     4
#define MEM_SYS_FAULT_STATE_SAVE      5
#define MEM_SYS_RTC_SAVE              6


#define MEASURE                       4       //wangw140928������-��������

#define MEASURE_RETURN_AIR_TEMP       1       //�ط��¶Ȳ���
#define MEASURE_HOME_HUMIDITY         2       //����ʪ�Ȳ���
#define MEASURE_FAN_COIL_TEMP         3       //����ʪ�Ȳ���



#define RESERVE                       5       //wangw140928������-Ԥ������


#define CONTRL                        6
#define MOTO                          6
#define SYS_MODE_AND_STATE_CTL        0       //����ģʽ��״̬��������
#define COLD_VALVE_CTL                1       //�䷧����������
#define HEAT_VALVE_CTL                2       //�ȷ�����������
#define FAN_SPEED_CTL                 3       //����ٶȿ���������
#define HUMDIFICA_CTL                 4       //������ʪ����������
#define HEATING_CTL                   5       //�������ȿ���������




#define OTHER                       7       //wangw140928������-��������
#define BEEP_TIPS                   1       //����������������
#define LED_TIPS                    2       //LEDָʾ�ƿ���������



/* ���� */
typedef struct
{
    void (*go_fun)(INT8U prio);                                     //�������ĺ���ָ�� ����ִ��������
}SYS_TaskStruct;


#ifdef  GLOBAL
#define EXT 
///////////////////////////////////////////////////////////////////////////////
//////////////////////////////  ���¶������ݱ��  /////////////////////////////
///////////////////////////////////////////////////////////////////////////////

const INT8U    SYS_TaskMapTbl[256] = { 
    0, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
    4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
    5, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
    4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
    6, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
    4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
    5, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
    4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
    7, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0, 
    4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
    5, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
    4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
    6, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
    4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
    5, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
    4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0  
};



void                            SG_Task(INT8U prio);
void                            COMM_Task(INT8U prio);
void                            MEM_Read_Task(INT8U prio);
void                            MEM_Write_Task(INT8U prio);
void                            Measure_Task(INT8U prio);
void                            Reserve_Task(INT8U prio);
void                            Contrl_Task(INT8U prio);
void                            Other_Task(INT8U prio);

const SYS_TaskStruct  SYS_TaskTbl[8] = {       //�����б� TASK0���ȼ����
    TASK0,
    TASK1,
    TASK2,
    TASK3,
    TASK4,
    TASK5,
    TASK6,
    TASK7
};



#else
#define EXT  extern
EXT const    INT8U              SYS_TaskMapTbl[256];
#ifdef WIN32
EXT          SYS_TaskStruct     SYS_TaskTbl[8];
#else
EXT const    SYS_TaskStruct     SYS_TaskTbl[8]; 
#endif

#endif

//wang140820����̹ܿ�����
//wangϵͳ������Ϣ
typedef union
{
   //INT8U SYS_Valve_Info;	             //wangϵͳ������Ϣ
   struct
   { 
      INT8U  Heating_State;              //wang���ص����״̬   �ָ�        
      INT8U  Humidification_State;       //wang���ؼ�ʪ��״̬   �ָ�
      INT8U  Cold_Valve_State;            //wang�䷧״̬        �ָ�
      INT8U  Heat_Valve_State;	          //wang�ȷ�״̬        �ָ�
      INT8U  Num_Of_Valves_State;        //wang����������0-1�����ţ�1-2�����ţ�  �ָ�
 
   };
}SYS_Valve_State_Struct;

//wang141017ϵͳ����״̬  0-�� 1-��
typedef union
{
   //INT8U SYS_Run_Info;	           //wangϵͳģʽ��Ϣ
   struct
   { 
      INT8U  Power_State;                 //wang�ܿ���״̬ �����ػ���0-�أ�1-����     
      INT8U  Energy_Saving_Mode_State;    //wang���ؽ���ģʽ״̬                 �ָ� 
      INT8U  Work_Mode;                   //wang0-���䣬1-���ȣ�2-ͨ�磬3-��ʪ   �ָ�      
      INT8U  Fan_Coil_Speed_Mode;         //wang0-�ߣ�1-�У�2-�ͣ�3-�Զ�         �ָ�
      INT8U  Fan_Coil_Speed_State;        //wang0-�ߣ�1-�У�2-��  5-��           �ָ�
      INT8U  Auto_Power_On_State;         //wang������ʱ״̬  00���ش˹��� 01һ����Ч 02һֱ��Ч
      INT8U  Auto_Power_On_Time;          //wangʱ���   
      INT8U  Auto_Power_Off_State;        //wang�ػ���ʱ״̬  00���ش˹��� 01һ����Ч 02һֱ��Ч
      INT8U  Auto_Power_Off_Time;         //wangʱ���      
  
   };
}SYS_Run_State_Struct;

/* ϵͳ���еı�־ ����洢 */
typedef union
{
  INT32U SYS_Run_Flag;			   //ϵͳ���λ1����
  struct{ 
            INT8U  Broadcast_Comm_Flag:1;  //wang�㲥ͨѶ���λ
            INT8U  Beep_Operate_Flag:1;    //wang������������־λ��0�������� 1������һ��
            INT8U  Beep_Ring_Fre:2;        //wang��������Ĵ���00������ 01��һֱ�� 02����һ�� 03��������
            INT8U  Led_Blanik_Fre:3;       //wang�Ƶ���˸Ƶ�� 00:���� 01:���� 02:250MS 03��1S 04:2S
            INT8U  SYS_State_Change_Flag:1; //wang����ָ���Ƿ񷵻����� 0��ԭ֡���� 1�������ݷ���         
  };
}SYS_Run_Flag_Struct;

//wang141017ϵͳ��ʪ����Ϣ
typedef struct
{     
    INT16U Cooling_Set_Temp;                //wang����ģʽ�����¶�           �ָ�
    INT16U Heating_Set_Temp;                //wang����ģʽ�����¶�           �ָ�
    INT8U  Ctl_Temp_Accuracy;               //wang���µĲ����¶�             �ָ�
    INT16U Fan_Coil_Temp;                   //wang��ǰ����̹��¶�           �ָ�
    INT16S Return_Air_Temp;                 //wang��ǰ�ط��¶�,ʵ�ʲ����¶�  �ָ�
    INT8U  Home_Humidity;                   //wang����ʪ��                   �ָ�
    INT8S  Home_Humidity_Compensation;      //wang����ʪ�Ȳ���ֵ             �ָ�
    INT16S Home_Temp;                       //wang�����¶ȣ������˲���ֵ��   �ָ�
    INT8S  Home_Temp_Compensation;          //wang�����¶Ȳ���ֵ             �ָ�
    INT8U  Temp_Protect_State;              //wang�¶ȱ���״̬               �ָ�
    INT8U  High_Temp_Protect_Value;          //wang���±���ֵ   ����ʱ��Ҫ����100  �ָ�
    INT8U  Low_Temp_Protect_Value;           //wang���±���ֵ   ����ʱ��Ҫ����100  �ָ� 
    
}SYS_TempAndHumidity_Info_Struct;

//wang140820ϵͳ������Ϣ
typedef union
{
   INT8U SYS_Fault_Info;	              //wangϵͳ������Ϣ
   struct
   { 
      INT8U  Home_Temp_Too_High_Fault: 1;     //wang ���¹��߹���     
      INT8U  Home_Temp_Too_Low_Fault: 1;      //wang ���¹��͹���     
      INT8U  Return_Air_Temp_Sensor_Fault: 1; //wang �ط��¶ȴ��������ϣ�0-û���ϣ�1-���ϣ�
      INT8U  Humidity_Sensor_Fault: 1;        //wang ʪ�ȴ��������ϣ�0-û���ϣ�1-���ϣ�
      INT8U  Fan_Fault: 1;                    //wang ������ϣ�0-û���ϣ�1-���ϣ�            
      INT8U  Memory_Storage_Fault: 1;         //wang �洢�����ϣ�0-û���ϣ�1-���ϣ�            
      INT8U  Fan_Coil_Temp_Sensor_Fault: 1;   //wang �̹��¶ȴ��������ϣ�0-û���ϣ�1-���ϣ�

   };
}SYS_Fault_State_Struct;

//wang140820ϵͳ�̶���Ϣ
typedef struct
{       
    INT8U TubeQuantity;                     //wang������                  
    INT8U Fan_Coil_Controller_ID;           //wang����̹ܿ�������ַ 
    INT8U Firmware_Version;                 //wang����̼��汾

}SYS_PAR_Info_Struct;


/*ʵʱʱ��  6�ֽ�*/
typedef struct
{
    INT8U       Second;		                                    //��
    INT8U       Minute;    		                            //��
    INT8U       Hour;	                                            //ʱ
    INT8U       Day;                                                //��
    INT8U       Month;                                              //��
    INT8U       Year;                                               //wang�ܱ�ʾ�ķ�Χ2000~2225��
//   INT8U       YearL;		                                    //���λ    2000~2255
//   INT8U       YearH;		                            //���λ    

}SYS_RTCStruct;                                   

//***********************************************************
//*******************End wang2013-11-11**************************
//////////////////////////////  ���¶��弰����ȫ�ֱ���  ////////////////////////

/////////////////////////////////  ϵͳ  ///////////////////////////////////////
//wang����̹ܽṹ�嶨�壬����EXT�൱�ڿ�
EXT NO_INIT  SYS_Valve_State_Struct             SYS_Valve_State;   //���ڴ洢ϵͳ��������,�ýṹ�屣�浽EEPROM

EXT NO_INIT  SYS_Run_State_Struct               SYS_Run_State; 

EXT NO_INIT  SYS_Run_Flag_Struct                SYS_Run_Flag;

EXT NO_INIT  SYS_TempAndHumidity_Info_Struct    SYS_TempAndHumidity_Info; //wang

EXT NO_INIT  SYS_Fault_State_Struct             SYS_Fault_State;  //wang

EXT NO_INIT  SYS_PAR_Info_Struct                SYS_PAR_Info;  //wang

EXT NO_INIT  SYS_RTCStruct                      SYS_RTC;         //ʵʱʱ��


EXT NO_INIT  INT8U              SYS_SubTask[8];      //ϵͳ�������֧   SYS_SubTask[0]��Ӧ�������Ȩ  
EXT NO_INIT  INT8U              SYS_MainTask;        //ϵͳ�������־

/////////////  ʵʱʱ��  ///////////////
EXT NO_INIT  INT8U              SYS_RTC_Cnt;                           //���� ÿ2�����Ϊ1S

////////////  Ӧ��ʱ��  ////////////////
#define LAST_GLOBAL_ADR         (&SYS_RTC_Cnt + sizeof(SYS_RTC_Cnt)-1)




#endif
