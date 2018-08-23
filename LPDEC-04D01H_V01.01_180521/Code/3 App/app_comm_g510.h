#ifndef __APP_COMM_G510_H
#define __APP_COMM_G510_H

#include "global.h"

#define TRAVERSE_AND_ADDEVT 1
#define TRAVERSE            0   

/* Bit defined */
#define BIT0			(0x01)
#define BIT1			(0x02)
#define BIT2			(0x04)
#define BIT3			(0x08)
#define BIT4			(0x10)
#define BIT5			(0x20)
#define BIT6			(0x40)
#define BIT7			(0x80)
/*************֡ͷ/֡β*************/
#define FRAME_HEAD       0x7f
#define FRAME_END        0xf7

#define TRANSFER_FLAG       0x1b
#define TRANSFER_CHAR1      0x1a
#define TRANSFER_CHAR1_FLAG 0x01
#define TRANSFER_CHAR2      0x1b
#define TRANSFER_CHAR2_FLAG 0x02

/***********************************************************************/
/*********************�������·�������************************/
/*�������7bit ������    0:�����->�ն�,1:�ն�->�����*/

#define WRITE_CTROL_CODE              0x20 
#define READ_CTROL_CODE               0x21
#define CONTROL_CTROL_CODE            0x22
#define UPLOAD_CTROL_CODE             0x23    

//#define RETURN_WRITE_CTROL_CODE       0xa0   
//#define RETURN_READ_CTROL_CODE        0xa1
//#define RETURN_CONTROL_CTROL_CODE     0xa2
//#define RETURN_UPLOAD_CTROL_CODE      0xa3

/*****************************************************************/

/************************�������Ϣ**********************************/
//��¼�������Ϣ
#define AUTHENTICA_ID                   0xb010
#define AUTH_CTROL_CODE                 0xa3
#define AUTH_LEN                        0x09

//�˹����а������Ϣ
#define CALL_ID                         0xb080
#define CALL_CTROL_CODE                 0xa3
#define CALL_LEN                        0x03

//�������÷��ذ�
#define SET_PARMTE_ID                   0xb020
#define SET_PARMTE_CTROL_CODE           0xa0
#define SET_PARMTE_LEN                  0x03

//��ѯ������
#define QUERY_PARMTE_ID                 0xb021
#define QUERY_PARMTE_CTROL_CODE         0xa1
#define QUERY_PARMTE_LEN                0x1d

//����Ϣ��
#define READ_MESS_ID                    0xb022
#define READ_MESS_CTROL_CODE            0xa1
#define READ_MESS_LEN                   0x10

//���ư�����
#define CONTROL_ID                      0xb023
#define CTROL_CODE                      0xa2
#define CTROL_LEN                       0x03

//д��ַ���ذ�
#define WRITE_ADDR_ID                   0xb024
#define WRITE_ADDR_CTROL_CODE           0xa0
#define WRITE_ADDR_LEN                  0x03

//����ַ���ذ�
#define READ_ADDR_ID                    0xb025
#define READ_ADDR_CTROL_CODE            0xa1
#define READ_ADDR_LEN                   0x07
/*****************************************************************/


/***********�������������������*************************/
#define SUB_TASK_PROCCTR_PER          (10U) /*10ms*/


/*******Ĭ�������������*********/	 
#define NET_PROTOCOL_DEFAULT        NET_PROTOCOL_TCP
#define SERVER_APN_DEFAULT          "cmiot"
#define SERVER_IP_DEFAULT           "118.178.227.84"
#define SERVER_PORT_DEFAULT         (8282U)
#define REPORT_PERIOD_DEFAULT       (30U)

#define SERVER_APN_LEN       (7)
#define SERVER_IP_LEN        (17)
//#define SERVER_PORT_LEN      (6)

/*********************BUFF LEN***********************/
#define GPRS_RTY_MESS_LEN         (40)
#define GPRS_RX_BUFF_LEN          (100)

#define GPRS_SendATCmdStr(pTxBuff, usLen)	        BSP_UART_Transmit(pTxBuff, usLen)


/*switch state*/
#define GPRS_IsSwitch_State()                       !READ_REG(g_stGPRS_Handler.ucStateEvent)
#define GPRS_State_Event_Get(Event)                 READ_BIT(g_stGPRS_Handler.ucStateEvent, Event)
#define GPRS_State_Event_Set(Event)                 SET_BIT(g_stGPRS_Handler.ucStateEvent, Event)
#define GPRS_State_Event_Clr(Event)                 CLEAR_BIT(g_stGPRS_Handler.ucStateEvent, Event)
#if 0
/*Comm Event*/
#define GPRS_Comm_Event_IsEmpty()                  !READ_REG(g_stGPRS_Handler.ucCommEvent)
#define GPRS_Comm_Event_Get(Event)                 READ_BIT(g_stGPRS_Handler.ucCommEvent, Event)
#define GPRS_Comm_Event_Set(Event)                 SET_BIT(g_stGPRS_Handler.ucCommEvent, Event)
#define GPRS_Comm_Event_Clr(Event)                 CLEAR_BIT(g_stGPRS_Handler.ucCommEvent, Event)

/*Comm Return Event*/
#define GPRS_Comm_Return_Event_IsEmpty()                  !READ_REG(g_stGPRS_Handler.ucReturnEvent)
#define GPRS_Comm_Return_Event_Get(Event)                 READ_BIT(g_stGPRS_Handler.ucReturnEvent, Event)
#define GPRS_Comm_Return_Event_Set(Event)                 SET_BIT(g_stGPRS_Handler.ucReturnEvent, Event)
#define GPRS_Comm_Return_Event_Clr(Event)                 CLEAR_BIT(g_stGPRS_Handler.ucReturnEvent, Event)


/*Comm Retry  Event*/
#define GPRS_Retry_Event_IsEmpty()                  !READ_REG(g_stGPRS_Handler.RetryCtrol.rtyEvent)
#define GPRS_Retry_Event_Get(Event)                 READ_BIT(g_stGPRS_Handler.RetryCtrol.rtyEvent, Event)
#define GPRS_Retry_Event_Set(Event)                 SET_BIT(g_stGPRS_Handler.RetryCtrol.rtyEvent, Event)
#define GPRS_Retry_Event_Clr(Event)                 CLEAR_BIT(g_stGPRS_Handler.RetryCtrol.rtyEvent, Event)
#endif

#define GPRS_Event_IsEmpty(Event)                   !READ_REG(Event)
#define GPRS_Event_Get(Event,Bit)                   READ_BIT(Event, Bit)
#define GPRS_Event_Set(Event,Bit)                   SET_BIT(Event, Bit)
#define GPRS_Event_Clr(Event,Bit)                   CLEAR_BIT(Event, Bit)



#define Get_CommEvent(Event)                        CntLeadZeros(Event)
#define Get_NextState(Event)                        CntLeadZeros(Event)


/*********GPRSģ��Ĺ���״̬****************/
typedef enum 
{
    GPRS_STATE_POWER_ON = 0,
    GPRS_STATE_RESET,
    GPRS_STATE_INIT,      /*����AT��ʼ������*/
	GPRS_STATE_CONNECT_NET,/*TCP/IP������·��������*/
	GPRS_STATE_READ_MESS,
	GPRS_STATE_COMM,
	GPRS_STATE_IDLE,
	GPRS_STATE_WAIT_ACK,
	GPRS_STATE_MAX,
   
}GPRS_STATE;

/*********GPRSģ��ͨѶ������ID**********/
typedef enum 
{
	UPLOAD_ID_AUTH = 0,   /*���͵�¼��*/
	UPLOAD_ID_CALL,       /*�����˹����а�*/
	UPLOAD_ID_MAX,
	UPLOAD_ID_INVALID = 0xFF,

}GPRS_COMM_ID;

typedef enum 
{
	RETURN_ID_SET_PARMTE=0,
	RETURN_ID_QUERY_PARMTE,
	RETURN_ID_READ_MESS,
	RETURN_ID_CTROL,
	RETURN_ID_WRITE_ADDR,
	RETURN_ID_READ_ADDR,
	RETURN_ID_MAX,
	RETURN_ID_INVALID = 0xFF,

}GPRS_COMM_RETURN_ID;


typedef enum 
{
	COMM_Event_AUTH         = (0x01 << UPLOAD_ID_AUTH),                  /*���͵�¼��*/
	COMM_Event_CALL         = (0x01 << UPLOAD_ID_CALL),                  /*�����˹����а�*/
	
    COMM_Event_SET_PARMTE   = (0x01 << RETURN_ID_SET_PARMTE),            
    COMM_Event_QUERY_PARMTE = (0x01 << RETURN_ID_QUERY_PARMTE),
    COMM_Event_READ_MESS    = (0x01 << RETURN_ID_READ_MESS),
    COMM_Event_CTROL        = (0x01 << RETURN_ID_CTROL),
    COMM_Event_WRITE_ADDR   = (0x01 << RETURN_ID_WRITE_ADDR),
    COMM_Event_READ_ADDR    = (0x01 << RETURN_ID_READ_ADDR),
	
}GPRS_COMM_Event;

/*State switch Event*/
typedef enum 
{
	STATE_Event_COMM   = BIT0,     /*go to Comm*/
    STATE_Event_MAX,
	
}GPRS_STATE_Event;


/*******GPRSӦ������*************/
typedef enum 
{
    RSP_TYPE_CMD = 0, /*Ӧ��ATָ��*/
    RSP_TYPE_DATA,    /*ͨ������*/
	RSP_TYPE_MAX,

}RSP_TYPE;


/*********GPRSģ�����ѡ��***********/
typedef enum
{
	GPRS_OPER_POWER_ON = 0,
    GPRS_OPER_RESET,
	GPRS_OPER_MAX,
	
}ENUM_GPRS_OPERATION;

/*********ATִ�н��***********/
typedef enum 
{
    NO_BACK = 0,
    BACK_TIMEOUT,           /*ATָ��Ӧ��ʱ*/
    BACK_ERROR,             /*ATָ��Ӧ�����*/
	BACK_PERTIMES_OUT,      /*ATָ���ط���������*/
	BACK_TRUE,              /*ATָ��Ӧ��������ȷ*/
	
}AT_RESULT_TYPE;


/**********GPRSָ�*****************/
typedef enum 
{

    GPRS_AT=0,/************************* ��ʼ������***************************/
	GPRS_AT_ATE0,
//	GPRS_AT_AT_CPIN1,
    GPRS_AT_CGREG1,
    GPRS_AT_KEEPCONF,
    GPRS_AT_GTSET,
    GPRS_AT_MIPCALL_1,
	GPRS_AT_MIPCALL1,
    GPRS_AT_MIPOPEN_1,
	GPRS_AT_MIPOPEN1,
	
	GPRS_AT_MIPSEND_1,
	GPRS_AT_DATA,
	
	GPRS_AT_CSQ,
	GPRS_AT_CCID,
	GPRS_AT_CGSN,

	GPRS_AT_MIPRTCP,

}AT_CMD_Type;

typedef enum 
{
    RESPOND_TYPE=1,
	RTY_TYPE,
    UPLOAD_TYPE,    
	MAX_TYPE,
	
}SEND_TYPE;



/*******��������*************/
typedef enum 
{
    NET_NO_NET=0,
    NET_GPRS,
	NET_TYPE_MAX,
	
}NET_TYPE;

/*******����Э��*************/
typedef enum 
{
    NET_PROTOCOL_UDP=0,
    NET_PROTOCOL_TCP,
    NET_PROTOCOL_COAP,
    NET_PROTOCOL_MQTT,
	NET_PROTOCOL_MAX,
}NET_PROTOCOL;

typedef struct
{
	const S8* CmdStr;                /*ָ���*/
	U16 usTimeOut;                   /*���ճ�ʱ*/
	const S8* ExpResultStr;          /*�������*/
	U8 ucResendTimes;                /*�ط�����*/
		
}GPRS_Cmd_Data_Type; 

/*�������*/
typedef struct
{
	NET_TYPE NetType;
	NET_PROTOCOL NetProtocol;
	U8 ServerApn[SERVER_APN_LEN];
	U8 ServerIp[SERVER_IP_LEN];
    U16 ServerPort;
    U16 ulReportPeriod; 
}Net_Parameters;

#define ADDR_REGION_LEN   7
typedef struct
{
//    U8 productType;                                //��Ʒ����
    U8 AckMeageFlowNum;              //Ӧ����Ϣ��ˮ��
    U8 TerminalMeageFlowNum;         //��ˮ�� 
    U8 addrRegion[ADDR_REGION_LEN];  //��ַ��
    U8 controlCode;                  //������
    U8 dataLen;                      //���ݳ���
    U8 *bodyData;                    //������
    
}PACKET_HEAD_Type;

#define IMEI_LEN    6
#define ICCID_LEN   10
typedef struct
{
   // U8 imei[IMEI_LEN];
   U8 iccid[ICCID_LEN];
    
}AUTH_DATA_Type;

#define POWER_ON   1
#define POWER_OFF  0

typedef union
{
    U16 state;
    struct
    {
        U16 staOfEle        :1;
        U16 reserve         :15;
    };
    
}DEVICE_STATE_Type;


typedef struct
{
    DEVICE_STATE_Type DeviceState;   //�豸״̬
    U8 CSQ;                          //GPRS�ź�ǿ��
    U8 hardwareVerLen;               //Ӳ���汾�ų���
    U8 hardwareVer[4];               //Ӳ���汾��
    U8 softwareVerLen;               //����汾�ų���
    U8 softwareVer[4];               //����汾��
    
}MESS_READ_Type;



/*********ͨѶ�����ݰ�����**************/
typedef struct
{
	AUTH_DATA_Type AuthData;
	MESS_READ_Type MessRead;
		
}COMM_Packet_Type;

#define RTY_TIME_INIT      (30000UL)  /*30*1000 ms ��30s*/
#define RTY_TIMES_MAX      (3)
typedef struct
{
    U8  rtycnt;                                         //�ش�����
    U8  rtycntEn;                                       //�ش�����ʹ��
#if CONFIG_RETRY_COPY
    U8  rtyBuff[GPRS_RTY_MESS_LEN];                     //�ش���Ϣ����
    U8  rtyLen;                                         //�ش���Ϣ����
#endif
    U16 rtyTimCount;                                    //�ش���ʱ������
    U16 rtyTime;                                        //�ش�ʱ��
}Retry_Ctr_Type;

typedef struct
{
    U8 ucRespondEvt;
    U8 ucDelayEvt;
    U8 ucRetryEvt;
    U8 ucUploadEvt;
    
}Event_Type;

/*******ATָ��ͨѶ���ƿ�**********/
typedef struct
{
    U8 SendATCmdEn;                 /*enable send ATCmd*/
	U8 ATCmdPt;                     /*��ǰAT��������*/
	U8 ucResendCnt;                 /*�ط�����*/	
	U8 ucResendTimes;               /*�ط�����*/
	U32 ulTimeOut;                  /*���ճ�ʱ*/
	U8 ATCmdResult;                 /*��ǰATָ��ͨѶ���*/
	BOOL Busy;                      /*MCU��ģ��ͨѶæµ��־*/
	U8 CmdStr[GPRS_RX_BUFF_LEN];    /*��ǰ���͵�ָ���*/
	U8 ucCmdStrLen;                 /*��ǰָ���*/
	S8 ExpResultStr[20];               /*��ǰ���͵�ָ������������*/
	U16 ucByteTimeOut;               /*�ֽڳ�ʱ*/
	U8 RxBuf[GPRS_RX_BUFF_LEN];     /*�������ݻ���*/
	U8 ucRxCnt;                     /*�������ݼ���*/
	U8 ucRxLen;                     /*�������ݳ���*/
	U8 RxFrameOk;                   /*ָʾһ֡��������*/
    U8 RspType;                     /*NBģ��Ӧ������*/

}AT_CMD_CB; 

/*******GPRSģ����ƿ�**********/
typedef struct
{
	U8 State;                       /*ģ�鵱ǰ״̬*/
	U8 StateOld;                    /*ģ����һ��״̬*/
	U8 StepPt;                      /*step ponit*/
	U16 ulDelayCnt;                 /*��ʱ����*/
	U16 ulProcCtrTaskPer;           /*״̬�����������*/
	U8  ucCommEvent;                /*�����Ҫ���͵����ݰ�*/
	U8  ucSendType;                 /*��Ƿ�������*/
	U8 ucStateEvent;                /*��ǰ���¼�״̬*/
	U8 ucDataID;                    /*������ڷ��͵���ϢID*/
	U8 ucCommBusy;                  /*ͨ��æµ���*/
	U8 RxBuf[GPRS_RX_BUFF_LEN];     /*Ӧ�ò���յ�������*/
	U8 TxBuf[GPRS_RX_BUFF_LEN];     /*Ӧ�ò���Ҫ���͵�����*/
	U8 ucTxLen;                     /*Ӧ�ò㷢�����ݳ���*/              
	Net_Parameters   NetPar;        /*ģ������������Ϣ*/
    COMM_Packet_Type CommPacket;    /*ͨѶ���ݰ�*/
    Retry_Ctr_Type   RetryCtrol;    /*�ط�����*/
    Event_Type       EvtProc;       /*�¼�����*/
    AT_CMD_CB        ATCmdCB;       /*******ATָ��ͨѶ���ƿ�**********/
	
}GPRS_Handler;

#define GPRS_NetPar          (g_stGPRS_Handler.NetPar)
#define GPRS_RetryCtrol      (g_stGPRS_Handler.RetryCtrol)
#define GPRS_ATCmdCB         (g_stGPRS_Handler.ATCmdCB)
#define GPRS_CommPacket      (g_stGPRS_Handler.CommPacket)
#define GPRS_EvtProc         (g_stGPRS_Handler.EvtProc)

/*************����������̵�step*******************/
#define INIT_STEP_NUM        NumberOfElements(s_ATCmdStep_Init)
#define CONNECT_STEP_NUM     NumberOfElements(s_ATCmdStep_Connnect)
#define MESS_STEP_NUM        NumberOfElements(s_ATCmdStep_Mess)
#define COMM_STEP_NUM        NumberOfElements(s_ATCmdStep_Comm)

extern PACKET_HEAD_Type      PacketHead;
extern GPRS_Handler          g_stGPRS_Handler;

void APP_GPRS_TimeProc(U16 usPeriod);
void APP_GPRS_NetParConfig(void);
void APP_COMM_Init(void);


void APP_SubTask_CmdProc(void);
void APP_SubTask_StateProc(void);
void APP_SuTask_DecodeProc(void);







#endif
