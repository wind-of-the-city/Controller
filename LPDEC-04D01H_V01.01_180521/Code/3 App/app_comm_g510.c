#include "app_comm_g510.h"

GPRS_Handler        g_stGPRS_Handler;
PACKET_HEAD_Type    PacketHead;

typedef void(*GPRS_State_Fun)(U8 *RxBuf); /* ����״̬������*/
//typedef void(*GPRS_State_Fun)(void);
static void APP_GPRS_RefreshRetryMess(void);
static U8   APP_GPRS_EvtTraverse(U8 mode);




static void APP_GPRS_State_Init_Proc   (U8 *RxBuf);
static void APP_GPRS_State_Connect_Proc(U8 *RxBuf);
static void APP_GPRS_State_Mess_Proc(U8 *RxBuf);
static void APP_GPRS_State_Comm_Proc   (U8 *RxBuf);

static const GPRS_State_Fun APP_GPRS_StateProcTbl[] = 
{
    NULL,     //POWER_ON
    NULL,     //RESET
    APP_GPRS_State_Init_Proc,
	APP_GPRS_State_Connect_Proc,
	APP_GPRS_State_Mess_Proc,
	APP_GPRS_State_Comm_Proc,
	NULL,
    NULL,
    NULL,
    NULL,
};


/******************GPRSָ�**********************************/
const GPRS_Cmd_Data_Type AT_CmdTbl[] = 
{
    /*------cmdstr-----------------------------------revtimeout(s)--cmdtruebackstr----------retry_times */
    /************************* GPRS  network ***************************/
      {"AT\r\n",                                         4,       "OK",                     2},
      {"ATE0\r\n",                                       3,       "OK",                     3},
   //   {"AT+CPIN?\r\n",                                   3,       "READY",                  3},
      {"AT+CGREG?\r\n",   			                     3,	      "+CGREG: 0,1",         	15},

      {"AT+MIPKEEPCONF=1,240,240,5\r\n",   			     3,	      "OK",         	        3},
/*������Ϊ0ʱ��ԭʼ�����ʽ(1�ֽ�ת2�ֽ�)*/
/*������Ϊ1ʱ����ת��ֱ��������Ҳ�����ʶͷ*/
/*������Ϊ2ʱ����ת��ֱ�����������ʶͷ*/
      {"AT+GTSET=\"IPRFMT\",2\r\n",   			         3,	      "OK",         	        2},   //��ѡ��

/*������������*/
      {"AT+MIPCALL=1,\"CMNET\"\r\n",                     30,      "+MIPCALL:",            3},
      {"AT+MIPCALL?\r\n",                                3,       "+MIPCALL: 1",            2},
      {"AT+MIPOPEN=1,,\"118.178.227.84\",9091,0\r\n",    20,        "+MIPOPEN: 1,1",        3},
      {"AT+MIPOPEN?\r\n",                                3,       "+MIPOPEN: 1",            2},
/* ���ݷ���*/
      {"AT+MIPSEND=1\r\n",                               10,      ">",                      2},
      {"DATA",                                           6,       "OK",                     1},  //��Ҫ���͵����ݣ����Ż�
      
      
      {"AT+CSQ\r\n",   			                         3,	      "+CSQ",         	3},  //���Ż�
      {"AT+CCID\r\n",                                    4,       "898",                    2},
      {"AT+CGSN\r\n",                                    4,       "OK",                     2},

      {NULL,                                          NULL,       "+MIPRTCP",            NULL},
      
};

/*************************��ʼ������***************************/
static const U8 s_ATCmdStep_Init[] = 
{
    NULL,
    GPRS_AT,       GPRS_AT_ATE0,    /*GPRS_AT_AT_CPIN1, */  GPRS_AT_CGREG1, GPRS_AT_KEEPCONF,  
	GPRS_AT_GTSET,                                
};

/************************* TCP/IP������·��������***************************/
static const U8 s_ATCmdStep_Connnect[] = 
{
    GPRS_AT_MIPCALL_1, GPRS_AT_MIPCALL1, GPRS_AT_MIPOPEN_1, //GPRS_AT_MIPOPEN1,                               
};

/*************************���ݷ�������***************************/
static const U8 s_ATCmdStep_Comm[] = 
{
    GPRS_AT_MIPSEND_1,GPRS_AT_DATA,                             
};

static const U8 s_ATCmdStep_Mess[] = 
{
    GPRS_AT_CSQ,GPRS_AT_CCID,
    //GPRS_AT_CGSN,       /*IMEI���ݲ���ȡ*/                         
};


/*У��ATָ��Ӧ�����Ƿ���ȷ*/
static U8 APP_GPRS_ATCmdAckCheck(U8 *RxBuf)
{   

    /*�ж��Ƿ��Ƿ�������������*/
    if(Check_Strstr((S8*)RxBuf, (S8*)(S8*)AT_CmdTbl[GPRS_AT_MIPRTCP].ExpResultStr, GPRS_ATCmdCB.ucRxLen) == TRUE)  
    {
        GPRS_ATCmdCB.RspType = RSP_TYPE_DATA;  /*ͨ������*/
        return BACK_TRUE;   /*���յ���ȷӦ��*/

    }
    if(Check_Strstr((S8*)RxBuf, "ready", GPRS_ATCmdCB.ucRxLen) == TRUE) //GPRSģ��׼����������
    {
        Task_Flag_Struct.atReday_F = ENABLE;
        GPRS_ATCmdCB.RspType = RSP_TYPE_CMD;
	    return BACK_TRUE;
    }
    if(GPRS_ATCmdCB.ATCmdPt != GPRS_AT_MIPOPEN_1)/*socket����ʱ,Ҳ�п��ܻظ�MIPSTAT: 1,1*/
    {
        if(Check_Strstr((S8*)RxBuf, "MIPSTAT: 1,1", GPRS_ATCmdCB.ucRxLen) == TRUE)
        {
            g_stGPRS_Handler.State = GPRS_STATE_CONNECT_NET;    //��������socket
		    g_stGPRS_Handler.StepPt = 2;
		    GPRS_Event_Set(GPRS_EvtProc.ucUploadEvt,COMM_Event_AUTH);
        }
    }
    if(GPRS_ATCmdCB.ATCmdPt != GPRS_AT_MIPCALL1) /*����IP��ȡ����Ҳ��ȥ��ѯ����IP*/
    {
        if(Check_Strstr((S8*)RxBuf, "MIPCALL: 0", GPRS_ATCmdCB.ucRxLen) == TRUE)
        
        { 
		    g_stGPRS_Handler.State = GPRS_STATE_CONNECT_NET;    //������������
		    g_stGPRS_Handler.StepPt = 0;
		    GPRS_Event_Set(GPRS_EvtProc.ucUploadEvt,COMM_Event_AUTH);
        }
    }

    
     /*�жϽ��յ������Ƿ�������Ӧ��*/
	if(Check_Strstr((S8*)RxBuf, GPRS_ATCmdCB.ExpResultStr, GPRS_ATCmdCB.ucRxLen) == TRUE)
	{
		GPRS_ATCmdCB.RspType = RSP_TYPE_CMD;/*Ӧ��ATָ��*/
	    return BACK_TRUE;                 /*���յ���ȷӦ��*/
	} 
    else
	{   
	    /*���ڻ�ȡ������ַ�ͽ���soctet���Ӷ��Ƿ��������صģ���һ����Ϊ"OK"*/
	    if((GPRS_ATCmdCB.ATCmdPt == GPRS_AT_MIPCALL_1) || GPRS_ATCmdCB.ATCmdPt == GPRS_AT_MIPOPEN_1)
	    {
  	        if(Check_Strstr((S8*)RxBuf, "OK", 4) == TRUE)   //ȥ����һ������
  	        {
  	            return NO_BACK;   /*��һ��"OK"������ȷ�����Լ����ȴ�*/
  	        }
	    }
	     /*��ѯ�Ƿ�פ���ɹ����粻�ɹ�����ô�ȴ�3S�������Ƿ��ش��󣬸���ָ������*/
        if(GPRS_ATCmdCB.ATCmdPt == GPRS_AT_CGREG1) 
        {
            return NO_BACK;
        }
		return BACK_ERROR;   		
	}	
}

static U8 APP_GPRS_Encrypt(u8 *hex_data_buf,u8 len_hex,u8 *new_hex_data_buf,u8 *new_len_hex)
{
  u8 len=0;
  for(u8 i=0;i<len_hex;i++,len++)
  {
    switch(hex_data_buf[i])
    {
      case TRANSFER_CHAR1:
              new_hex_data_buf[len]=TRANSFER_FLAG;
              len++;
              new_hex_data_buf[len]=TRANSFER_CHAR1_FLAG;
              break;
      case TRANSFER_CHAR2:
              new_hex_data_buf[len]=TRANSFER_FLAG;
              len++;
              new_hex_data_buf[len]=TRANSFER_CHAR2_FLAG;
              break;
      default:
              new_hex_data_buf[len]=hex_data_buf[i];
              break;
     }
  }
  *new_len_hex=len;
  if(*new_len_hex<=GPRS_RX_BUFF_LEN){
    return TRUE;
  }
  else{
    return FALSE;
  }
}

static void APP_GPRS_Data_Trope(u8 *hex_data_buf,u8 len_hex,u8 *new_hex_data_buf)
{
  u16 len_hex_cnt=0;
  u16 len_temp=0;
				
  for(len_temp=0;len_hex_cnt<len_hex;len_hex_cnt++,len_temp++)      
  {
    if(hex_data_buf[len_hex_cnt]==TRANSFER_FLAG)
    {
      switch(hex_data_buf[len_hex_cnt+1])
      {
        case TRANSFER_CHAR1_FLAG:
              new_hex_data_buf[len_temp]=TRANSFER_CHAR1;
              len_hex_cnt++;
              break;
        case TRANSFER_CHAR2_FLAG:
              new_hex_data_buf[len_temp]=TRANSFER_CHAR2;
              len_hex_cnt++;
              break;
        default:
        #if DEBUG_CONTROLLER
              BSP_LOG("0x1b error\r\n");
        #endif
              break;
      }
    }
    else
    {
       new_hex_data_buf[len_temp]=hex_data_buf[len_hex_cnt];
    }
  } 
}

static U16 APP_GPRS_GetPacket(u16 messID,U8 ctrolcode,U8 datalen,U8* Packet)
{
    U8 i=0,j=0;
    U8 check=0;
    U8 ipLen=0;
    U8 apnLen=0;
    U8 fillZeroLen=0;
    U8 newLen=0;
    U8 TxBuf[GPRS_RX_BUFF_LEN]={0};
#if DEBUG_RTY_FUN
    static U8 rtyTestCnt=0;
#endif
   
    TxBuf[i++] = FRAME_HEAD;
    TxBuf[i++] = 0x01;    //��Ʒ����
    for(j=0;j<ADDR_REGION_LEN;j++)
    {
       TxBuf[i++] = PacketHead.addrRegion[j]&0xff;
    }
    TxBuf[i++] = ctrolcode&0xff;
    TxBuf[i++] = datalen&0xff;
    TxBuf[i++] = (messID>>8)&0xff;
    TxBuf[i++] = messID&0xff;
    switch(messID)
    {
       case AUTHENTICA_ID:
       {
           TxBuf[i++] = PacketHead.TerminalMeageFlowNum;
           for(j=0;j<6;j++)
           {
               TxBuf[i++] = GPRS_CommPacket.AuthData.iccid[j+4];
           }
           
       }
       break;
       
       case CALL_ID:
       {
           TxBuf[i++] = PacketHead.TerminalMeageFlowNum;
           //��������Ϊ��
       }
       
       break;
       case SET_PARMTE_ID:
       {
           TxBuf[i++] = PacketHead.AckMeageFlowNum;
           //������������Ϊ��
       }
       break;
       case QUERY_PARMTE_ID:
       {
            TxBuf[i++] = PacketHead.AckMeageFlowNum;
            ipLen  = strlen((char *)GPRS_NetPar.ServerIp);
            apnLen = strlen((char *)GPRS_NetPar.ServerApn);
            for(j=0;j<ipLen;j++)
            {
                TxBuf[i++] = (GPRS_NetPar.ServerIp[j]) & 0xff;
            }
            fillZeroLen = SERVER_IP_LEN-ipLen;
            for(j=0;j<fillZeroLen;j++)        //Ϊ������SERVER_IP_LEN(17)���ȵ��ֽ�
            {
                TxBuf[i++] = 0;
            }  
            
            TxBuf[i++] = (GPRS_NetPar.ServerPort>>8) & 0xff;
            TxBuf[i++] = (GPRS_NetPar.ServerPort) & 0xff;
            for(j=0;j<apnLen;j++)
            {
                TxBuf[i++] = GPRS_NetPar.ServerApn[j] & 0xff;
            }
            fillZeroLen = SERVER_APN_LEN - apnLen;
            for(j=0;j<fillZeroLen;j++)
            {
                TxBuf[i++] = 0;
            } 
       }
       break;
       case READ_MESS_ID:
       {
            TxBuf[i++] = PacketHead.AckMeageFlowNum;
            TxBuf[i++] = (GPRS_CommPacket.MessRead.DeviceState.state>>8) & 0xff;
            TxBuf[i++] = (GPRS_CommPacket.MessRead.DeviceState.state) & 0xff;
            TxBuf[i++] = (GPRS_CommPacket.MessRead.CSQ) & 0xff;
            TxBuf[i++] = (GPRS_CommPacket.MessRead.hardwareVerLen) & 0xff;
            for(j=0;j<GPRS_CommPacket.MessRead.hardwareVerLen;j++)
            {
                TxBuf[i++] = (GPRS_CommPacket.MessRead.hardwareVer[j]) & 0xff;
            }
            TxBuf[i++] = (GPRS_CommPacket.MessRead.softwareVerLen) & 0xff;
            for(j=0;j<GPRS_CommPacket.MessRead.softwareVerLen;j++)
            {
                TxBuf[i++] = (GPRS_CommPacket.MessRead.softwareVer[j]) & 0xff;
            }
       }
       break;
       case CONTROL_ID:
       {
           TxBuf[i++] = PacketHead.AckMeageFlowNum;
            //������������Ϊ��
       }
       break;
       case WRITE_ADDR_ID:
       {
          TxBuf[i++] = PacketHead.AckMeageFlowNum;
           //������������Ϊ��
       }
       break;
       case READ_ADDR_ID:
       {
          TxBuf[i++] = PacketHead.AckMeageFlowNum;
          for(j=0;j<4;j++)
          {
               TxBuf[i++] = (PacketHead.addrRegion[j]) & 0xff;
          }
       }
       break;
    }
    check = Get_Xor(TxBuf,i); //����У���
    TxBuf[i++] = check&0xff;
#if DEBUG_RTY_FUN
    rtyTestCnt++;
    if(rtyTestCnt <=2)
    {
        for(j=0;j<3;j++)
        {
            TxBuf[i++]=j;
        }
    } 
    if((rtyTestCnt <=8) && (rtyTestCnt >=4))
    {
         for(j=0;j<3;j++)
         {
              TxBuf[i++]=j;
         }
    }
#endif     
    TxBuf[i++] = FRAME_END;
    APP_GPRS_Encrypt(TxBuf,i,Packet,&newLen);
    return   (U16)newLen;
    
}

static void APP_GPRS_RetryMessInit(void)
{
   GPRS_RetryCtrol.rtycnt = 0;
   GPRS_RetryCtrol.rtycntEn = DISABLE;
   
   GPRS_RetryCtrol.rtyTimCount =0;
   GPRS_RetryCtrol.rtyTime = RTY_TIME_INIT;
#if CONFIG_RETRY_COPY
   GPRS_RetryCtrol.rtyLen = 0;
   memset(GPRS_RetryCtrol.rtyBuff,0,GPRS_RTY_MESS_LEN);
#endif
}

static void APP_GPRS_RefreshRetryMess(void)
{
   GPRS_RetryCtrol.rtycnt++;
   GPRS_RetryCtrol.rtycntEn = ENABLE;
   GPRS_RetryCtrol.rtyTimCount =0;
   GPRS_RetryCtrol.rtyTime = RTY_TIME_INIT;   
}

#if CONFIG_RETRY_COPY
static U16 APP_GPRS_WriteRetryIDPacket(U8* Packet)
{
/*�ش�ʱҲ�����������ֹ��һ�����ݳ�����ô������������Ҳ�����*/
/*�����ش��漰��ʱ�����ݣ���ô������ݱ��棬�������ʱ�ٷ���*/
/*���ش�ʱ�������������Ѻ�CONFIG_RETRY_COPY��λ*/
    U16 usPacketLen=0;
	#if DEBUG_CONTROLLER
            BSP_LOG("retry send mess\r\n");
    #endif
    memcpy(Packet,GPRS_RetryCtrol.rtyBuff,GPRS_RetryCtrol.rtyLen);    //�ش���Ϣcopy�����ͻ���
    usPacketLen = GPRS_RetryCtrol.rtyLen;                             
    return usPacketLen;    
}

#endif

static U16 APP_GPRS_WriteReturnIDPacket(U8 DataID, U8* Packet)
{
    U16 usPacketLen=0;
	switch(DataID)
	{

		case RETURN_ID_SET_PARMTE:
		{
			usPacketLen = APP_GPRS_GetPacket(SET_PARMTE_ID,SET_PARMTE_CTROL_CODE,SET_PARMTE_LEN,Packet); 
		    #if DEBUG_CONTROLLER
                BSP_LOG("send set parte mess\r\n");
            #endif
		}break;	
		case RETURN_ID_QUERY_PARMTE:
		{
			usPacketLen = APP_GPRS_GetPacket(QUERY_PARMTE_ID,QUERY_PARMTE_CTROL_CODE,QUERY_PARMTE_LEN,Packet); 
		    #if DEBUG_CONTROLLER
                BSP_LOG("send query parte mess\r\n");
            #endif
		}break;
		case RETURN_ID_READ_MESS:
		{
			usPacketLen = APP_GPRS_GetPacket(READ_MESS_ID,READ_MESS_CTROL_CODE,READ_MESS_LEN,Packet); 
		    #if DEBUG_CONTROLLER
                BSP_LOG("send read mess\r\n");
            #endif
		}break;
		case RETURN_ID_CTROL:
		{
			usPacketLen = APP_GPRS_GetPacket(CONTROL_ID,CTROL_CODE,CTROL_LEN,Packet); 
		    #if DEBUG_CONTROLLER
                BSP_LOG("send ctrol mess\r\n");
            #endif
		}break;
		case RETURN_ID_WRITE_ADDR:
		{
			usPacketLen = APP_GPRS_GetPacket(WRITE_ADDR_ID,WRITE_ADDR_CTROL_CODE,WRITE_ADDR_LEN,Packet); 
		    #if DEBUG_CONTROLLER
                BSP_LOG("send write addr mess\r\n");
            #endif
		}break;
		case RETURN_ID_READ_ADDR:
		{
			usPacketLen = APP_GPRS_GetPacket(READ_ADDR_ID,READ_ADDR_CTROL_CODE,READ_ADDR_LEN,Packet); 
		    #if DEBUG_CONTROLLER
                BSP_LOG("send read addr mess\r\n");
            #endif
		}break;
	    default:
			break;
	}
	return usPacketLen;

}
/*���ݲ�ͬ������ID,������֡����װЭ����֡����*/
static U16 APP_GPRS_WriteDataIDPacket(U8 DataID, U8* Packet)
{
    U16 usPacketLen=0;
	switch(DataID)
	{    
		case UPLOAD_ID_AUTH:
		{

			usPacketLen = APP_GPRS_GetPacket(AUTHENTICA_ID,AUTH_CTROL_CODE,AUTH_LEN,Packet);
#if CONFIG_RETRY_COPY            
            GPRS_RetryCtrol.rtyLen = usPacketLen;                   //��Ϣ���ȱ���
            memcpy(GPRS_RetryCtrol.rtyBuff,Packet,usPacketLen);     //��Ϣ����   
#endif
		    #if DEBUG_CONTROLLER
                BSP_LOG("send auth mess\r\n");
            #endif
		}break;
		
		case UPLOAD_ID_CALL:
		{
			usPacketLen = APP_GPRS_GetPacket(CALL_ID,CALL_CTROL_CODE,CALL_LEN,Packet);
#if CONFIG_RETRY_COPY
			GPRS_RetryCtrol.rtyLen = usPacketLen; 
			memcpy(GPRS_RetryCtrol.rtyBuff,Packet,usPacketLen);
#endif
		    #if DEBUG_CONTROLLER
                BSP_LOG("send call mess\r\n");
            #endif
		}break;	

	    default:
			break;
	}
	return usPacketLen;
}

static void APP_GPRS_SetIP(U8 *pFrameDes)
{
    U8 i=0;
    U8 ipLen=0;
    U8 portLen=0;
    U8 data[6]={0};
    U8 *ip = GPRS_NetPar.ServerIp;
    U8 *port=NULL;
    ipLen = strlen((char*)ip);
    port = DecimalToASCII(GPRS_NetPar.ServerPort,data); //�˿ں�ת��֧��1--65535
    if(port != NULL)
    {
        portLen=strlen((char*)port);
        pFrameDes[0] = '"';
        for(i=0;i<ipLen;i++)
        {
           pFrameDes[i+1]=ip[i];
        }
        pFrameDes[ipLen+1] = '"';
        pFrameDes[ipLen+2] = ',';
        for(i=0;i<portLen;i++)
        {
           pFrameDes[ipLen+3+i]=port[i];
        }
        pFrameDes[ipLen+portLen+3]=',';
        pFrameDes[ipLen+portLen+4]='0';
        pFrameDes[ipLen+portLen+5]='\r';
        pFrameDes[ipLen+portLen+6]='\n';
        pFrameDes[ipLen+portLen+7]='\0';
        return ;
    }
    //�˿ں�ת��������ӡ��Ϣ
    
}

static U8 APP_GPRS_WriteATPacket(U8 ATCmdIndex, U8 *pFrameDes)
{  
    U8 ucSendLen;
    U8 dataNum;
    char *endStr="\x1A\r\n";
	switch(ATCmdIndex)
	{
		case GPRS_AT_MIPOPEN_1:/*����socket����*/
		{
            strcpy((S8*)pFrameDes, AT_CmdTbl[ATCmdIndex].CmdStr);  
		    dataNum = Get_Comma(2,(u8*)pFrameDes);
		    APP_GPRS_SetIP(&pFrameDes[dataNum]);			    
		 	ucSendLen = strlen((char*)pFrameDes);
			
		}break;
		
		case GPRS_AT_DATA:/*�����������*/
		{
		     memcpy(pFrameDes, g_stGPRS_Handler.TxBuf, g_stGPRS_Handler.ucTxLen);/*copy Ӧ�ò�����*/
             ucSendLen = strlen(endStr);
             memcpy(pFrameDes+g_stGPRS_Handler.ucTxLen,endStr,ucSendLen);        /*copy ��β��*/
             ucSendLen += g_stGPRS_Handler.ucTxLen;
			
		}break;
		
		default:
		{
			strcpy((S8*)pFrameDes, AT_CmdTbl[ATCmdIndex].CmdStr);
			ucSendLen = strlen(AT_CmdTbl[ATCmdIndex].CmdStr);/*Ĭ�ϵ�ָ�������*/
			
		}break;
	}
	return ucSendLen;    
}


static void APP_GPRS_SendATCmd(AT_CMD_CB* pNBATCmdCB)
{
	U8 ucResendCnt;
	U8 ATCmdIndex;
	AT_CMD_CB *pAtCmd = pNBATCmdCB;
	ATCmdIndex = pAtCmd->ATCmdPt;
	if(1 == pAtCmd->Busy)
	{
		return ;/*��ǰæµ״̬*/	
	}
	/*���ݲ�ͬ��ATӦ����,�����ط�����*/
	if((BACK_TRUE == pAtCmd->ATCmdResult) || (BACK_PERTIMES_OUT == pAtCmd->ATCmdResult))
	{
		ucResendCnt = 0;/*�ط�����*/	
	}
	else
	{
	    ucResendCnt = GPRS_ATCmdCB.ucResendCnt;/*�����ط�����*/;	
	}

	memset((U8*)pAtCmd, 0, sizeof(GPRS_ATCmdCB));
	
	pAtCmd->Busy         = 1;          /*����æ��״̬*/
	pAtCmd->ATCmdPt      = ATCmdIndex; /*����AT������*/
	pAtCmd->ATCmdResult  = NO_BACK;
	pAtCmd->ucResendCnt  = ucResendCnt;  
	pAtCmd->ucResendTimes = AT_CmdTbl[ATCmdIndex].ucResendTimes;
	pAtCmd->ulTimeOut    = TmrSecondsToMilliseconds(AT_CmdTbl[ATCmdIndex].usTimeOut);/*ˢ�³�ʱʱ��*/

	/*****t���ָ�����ݳ���******/
	pAtCmd->ucCmdStrLen = APP_GPRS_WriteATPacket(ATCmdIndex, pAtCmd->CmdStr);
	/*���ָ����������*/	
	strcpy(pAtCmd->ExpResultStr, AT_CmdTbl[ATCmdIndex].ExpResultStr);
	/*����ATָ��*/	
	GPRS_SendATCmdStr(pAtCmd->CmdStr, pAtCmd->ucCmdStrLen);
	/*���ʹ���+1*/	
    pAtCmd->ucResendCnt++;
}

static void APP_GPRS_State_Init_Proc(U8 *RxBuf)
{
	U8 ATCmdIndex;
	if(g_stGPRS_Handler.StepPt >= INIT_STEP_NUM)
	{
		return;	
	}	 
	ATCmdIndex = s_ATCmdStep_Init[g_stGPRS_Handler.StepPt];
	/************************ATָ��Ӧ����ȷ��**************************/	
	if(BACK_TRUE == GPRS_ATCmdCB.ATCmdResult)/*ATָ��Ӧ��������ȷ*/
	{
		/*next Step*/
		g_stGPRS_Handler.StepPt++;
		if(g_stGPRS_Handler.StepPt >= INIT_STEP_NUM)
		{
			/******GPRSģ��״̬�л�***************/
			g_stGPRS_Handler.State = GPRS_STATE_CONNECT_NET;/*�л�����һ������*/
			g_stGPRS_Handler.StepPt = 0;     /**clr step*/
			GPRS_NetPar.NetType = NET_GPRS;/*����������״̬*/
		}
		else
		{
		    g_stGPRS_Handler.State = GPRS_STATE_INIT;/*������ǰ����*/		
		}	
	}
/************************AT ָ��ﵽ�ط�����**************************/	
	else if(BACK_PERTIMES_OUT == GPRS_ATCmdCB.ATCmdResult)/*ATָ���ط���������*/
	{
		switch(ATCmdIndex)
		{
		    case GPRS_AT:   //ֱ�ӿ�ʼ��һ��ָ��
			{
			    g_stGPRS_Handler.StepPt++;
			    g_stGPRS_Handler.State = GPRS_STATE_INIT; /*������ǰ����*/	
			    
			}break;
			default:
			{
			    #if DEBUG_CONTROLLER
                      BSP_LOG("Init Reset\r\n");
                #endif
			    g_stGPRS_Handler.State = GPRS_STATE_RESET;       //���и�λ
		        g_stGPRS_Handler.StepPt = 0;/**clr step*/
		        GPRS_NetPar.NetType = NET_NO_NET;
			}
				break;	
		}
	}
/************************ATָ��Ӧ������ʱ*************************/	
	else
    {
		/*û�е�ATָ���ط������������ٴη���ATָ��*/
		g_stGPRS_Handler.State = GPRS_STATE_WAIT_ACK;/*ֱ���ٴη���ATָ�Ȼ��ȴ�Ӧ��*/
		GPRS_ATCmdCB.ATCmdPt = s_ATCmdStep_Init[g_stGPRS_Handler.StepPt];
        GPRS_ATCmdCB.SendATCmdEn = 1;/*enable send ATCmd*/	 
    }	
}

static void APP_GPRS_State_Connect_Proc(U8 *RxBuf)
{
    U8 ATCmdIndex;
	
	if(g_stGPRS_Handler.StepPt >= CONNECT_STEP_NUM)
	{
		return;	
	}
	
	ATCmdIndex = s_ATCmdStep_Connnect[g_stGPRS_Handler.StepPt];
/************************ATָ��Ӧ����ȷ��**************************/	
	if(BACK_TRUE == GPRS_ATCmdCB.ATCmdResult)/*ATָ��Ӧ��������ȷ*/
	{	           
         /*next Step*/
		g_stGPRS_Handler.StepPt++;
		if(g_stGPRS_Handler.StepPt >= CONNECT_STEP_NUM)
		{
			/******GPRSģ��״̬�л�***************/
			g_stGPRS_Handler.State = GPRS_STATE_READ_MESS;/*�л�����һ������*/
			g_stGPRS_Handler.StepPt = 0;/**clr step*/
		}
		else
		{
		    g_stGPRS_Handler.State = GPRS_STATE_CONNECT_NET;/*������ǰ����*/				
		}			
	}
/************************AT ָ��ﵽ�ط�����**************************/	
	else if(BACK_PERTIMES_OUT == GPRS_ATCmdCB.ATCmdResult)
	{
	    switch(ATCmdIndex)
		{
		    case GPRS_AT_MIPCALL_1:   //ֱ�ӿ�ʼ��һ��ָ��
			{
			    g_stGPRS_Handler.StepPt++;
			    g_stGPRS_Handler.State = GPRS_STATE_CONNECT_NET; /*������ǰ����*/	
			    
			}break;
			default:
			{
			    #if DEBUG_CONTROLLER
                      BSP_LOG("Connect Reset\r\n");
                #endif
			    g_stGPRS_Handler.State = GPRS_STATE_RESET;       //���и�λ
		        g_stGPRS_Handler.StepPt = 0;/**clr step*/
		        GPRS_NetPar.NetType = NET_NO_NET;
			}
				break;	
		}	
	}
/************************ATָ��Ӧ������ʱ*************************/	
	else
	{
		/*û�е�ATָ���ط������������ٴη���ATָ��*/
		g_stGPRS_Handler.State = GPRS_STATE_WAIT_ACK;/*ֱ���ٴη���ATָ�Ȼ��ȴ�Ӧ��*/
        GPRS_ATCmdCB.SendATCmdEn = 1;/*enable send ATCmd*/	
		GPRS_ATCmdCB.ATCmdPt = s_ATCmdStep_Connnect[g_stGPRS_Handler.StepPt];
	}	

}

static void APP_GPRS_State_Mess_Proc(U8 *RxBuf)
{
        U8 ATCmdIndex;
        U8 asi_len=0;
        char *str = "898";
        char *p   = NULL;
        if(g_stGPRS_Handler.StepPt >= MESS_STEP_NUM)
        {
            return; 
        }   
        ATCmdIndex = s_ATCmdStep_Mess[g_stGPRS_Handler.StepPt];
        /************************ATָ��Ӧ����ȷ��**************************/    
        if(BACK_TRUE == GPRS_ATCmdCB.ATCmdResult)/*ATָ��Ӧ��������ȷ*/
        {
            switch(ATCmdIndex)
            {
                  case GPRS_AT_CSQ:
                  {
				        GPRS_CommPacket.MessRead.CSQ = Get_Uint_Data((S8*)&RxBuf[Get_Comma(1, RxBuf)] - 3);
		
			      }
                  break;	
			      case GPRS_AT_CCID:
                  {
				        //��ȡICCID
				        p=strstr((char*)RxBuf,str);
				        asi_len = strlen(p);
				        if(asi_len > 20) asi_len = 20;    
				        Ascii_To_Hex(p,GPRS_CommPacket.AuthData.iccid,(U16)asi_len); 
				        
			      }
                  break;	
			      default:
				    break;
            }
            /*next Step*/
            g_stGPRS_Handler.StepPt++;
            if(g_stGPRS_Handler.StepPt >= MESS_STEP_NUM)
            {
                /******GPRSģ��״̬�л�***************/
                g_stGPRS_Handler.State = GPRS_STATE_IDLE;/*�л�����һ������*/
                g_stGPRS_Handler.StepPt = 0;     /**clr step*/
            }
            else
            {
                g_stGPRS_Handler.State = GPRS_STATE_READ_MESS;/*������ǰ����*/       
            }   
        }
    /************************AT ָ��ﵽ�ط�����**************************/ 
        else if(BACK_PERTIMES_OUT == GPRS_ATCmdCB.ATCmdResult)/*ATָ���ط���������*/
        {
            #if DEBUG_CONTROLLER
                  BSP_LOG("Mess Reset\r\n");
            #endif
            g_stGPRS_Handler.State = GPRS_STATE_RESET;  //���и�λ
            g_stGPRS_Handler.StepPt = 0;/**clr step*/
            GPRS_NetPar.NetType = NET_NO_NET;
        }
    /************************ATָ��Ӧ������ʱ*************************/ 
        else
        {
            /*û�е�ATָ���ط������������ٴη���ATָ��*/
            g_stGPRS_Handler.State = GPRS_STATE_WAIT_ACK;/*ֱ���ٴη���ATָ�Ȼ��ȴ�Ӧ��*/
            GPRS_ATCmdCB.ATCmdPt = s_ATCmdStep_Mess[g_stGPRS_Handler.StepPt];
            GPRS_ATCmdCB.SendATCmdEn = 1;/*enable send ATCmd*/  
        }

}

static void APP_GPRS_State_Comm_Proc   (U8 *RxBuf)
{
    U8 ATCmdIndex;  
    static U8 errCnt=0;
	if(g_stGPRS_Handler.StepPt >= COMM_STEP_NUM)
	{
		return;	
	} 
	ATCmdIndex = s_ATCmdStep_Comm[g_stGPRS_Handler.StepPt];
/************************ATָ��Ӧ����ȷ��**************************/	
	if(BACK_TRUE == GPRS_ATCmdCB.ATCmdResult)/*ATָ��Ӧ��������ȷ*/
	{
		switch(ATCmdIndex)
		{
			case GPRS_AT_DATA:
            {
				
				if(RESPOND_TYPE == g_stGPRS_Handler.ucSendType)
				{
				    GPRS_Event_Clr(GPRS_EvtProc.ucRespondEvt,(0x01 << g_stGPRS_Handler.ucDataID));
				}
				else if(RTY_TYPE == g_stGPRS_Handler.ucSendType)
				{
				    GPRS_Event_Clr(GPRS_EvtProc.ucRetryEvt,(0x01 << g_stGPRS_Handler.ucDataID));
		            APP_GPRS_RefreshRetryMess();
				}
				else if(UPLOAD_TYPE == g_stGPRS_Handler.ucSendType)
				{
				    /*����ͨѶ���,�����Ӧ���¼�*/
				    GPRS_Event_Clr(GPRS_EvtProc.ucUploadEvt,(0x01 << g_stGPRS_Handler.ucDataID));
				    //�������������¼�������ȵ�Ӧ����ܷ�����һ�������ϴ���Ϣ            
				    GPRS_Event_Set(GPRS_EvtProc.ucDelayEvt,(0x01 << g_stGPRS_Handler.ucDataID));     
                    APP_GPRS_RefreshRetryMess();
				}
				GPRS_Event_Clr(g_stGPRS_Handler.ucCommEvent,(0x01 << g_stGPRS_Handler.ucDataID));
		        errCnt=0;           /*ATָ��ɹ�������������*/
			}break;	
			
			default:
				break;	
		}
		/*next Step*/
		g_stGPRS_Handler.StepPt++;
		if(g_stGPRS_Handler.StepPt >= COMM_STEP_NUM)
		{
			/******GPRSģ��״̬�л�***************/
			/*���ݱ�ǩ���ж��Ƿ�������������Ҫ���ͣ���������ٴν���comm��û�����˳�ͨѶ״̬*/
			if(APP_GPRS_EvtTraverse(TRAVERSE) == FALSE)
			{
                g_stGPRS_Handler.State = GPRS_STATE_IDLE;/*�л�����һ������*/
				g_stGPRS_Handler.ucCommBusy= 0;/*Comm Complete*/
			}
			else
			{
				g_stGPRS_Handler.State = GPRS_STATE_COMM;/*������ǰ����*/		
			}
			
			g_stGPRS_Handler.StepPt = 0;/**clr step*/
		}
		else
		{
		    g_stGPRS_Handler.State = GPRS_STATE_COMM;/*������ǰ����*/		
		}		
	}
/************************AT ָ��ﵽ�ط�����**************************/	
	else if(BACK_PERTIMES_OUT == GPRS_ATCmdCB.ATCmdResult)
	{
	    if(GPRS_AT_DATA == ATCmdIndex)
	    {
	        g_stGPRS_Handler.State = GPRS_STATE_COMM;
	        errCnt++;
	    }
	    else if((GPRS_AT_MIPSEND_1 == ATCmdIndex) || (errCnt > 3))
	    {
            #if DEBUG_CONTROLLER
                  BSP_LOG("Comm Reset\r\n");
            #endif
            errCnt=0;
	        g_stGPRS_Handler.State = GPRS_STATE_RESET;
	        GPRS_NetPar.NetType = NET_NO_NET;
	    }
	    
		g_stGPRS_Handler.StepPt = 0;/**clr step*/
		g_stGPRS_Handler.ucCommBusy= 0;	

	}
/************************ATָ��Ӧ������ʱ*************************/	
	else
	{
		/*û�е�ATָ���ط������������ٴη���ATָ��*/
		g_stGPRS_Handler.State = GPRS_STATE_WAIT_ACK;/*ֱ���ٴη���ATָ�Ȼ��ȴ�Ӧ��*/
        GPRS_ATCmdCB.SendATCmdEn = 1;/*enable send ATCmd*/
		GPRS_ATCmdCB.ATCmdPt = s_ATCmdStep_Comm[g_stGPRS_Handler.StepPt];
	} 
}
/*����������·����ݲ��ӱ�ʶͷ���˺����ɷ���ATָ�������*/

static U8 APP_GPRS_RxDataCheck(U8 *rxBuff)   
{
    U8 i=0;
    U8 rxLen=rxBuff[10]+13;
    if(rxBuff[0] != FRAME_HEAD)       return FALSE;  //֡ͷУ��
    if(rxBuff[1] != 0x01)       return FALSE;        //��Ʒ����У��
    for(i=0;i<4;i++)
    {
        if(rxBuff[i+2] != PacketHead.addrRegion[i])  //��ַ��У��
            return FALSE;
    }
    if(rxBuff[rxLen-1] != FRAME_END)   return FALSE;
    if(FALSE == Xor_Check(rxBuff,rxLen))             //��У��
    {
        return FALSE;
    }
    #if DEBUG_CONTROLLER
    BSP_LOG("DECODE TRUE\r\n");
    #endif
    return TRUE;
}
static void APP_GPRS_RxDataDecode(U8 *rxBuff)
{
    U8 i=0;
    U8 ctrolCmd=0;
    U8 *p_buf=rxBuff+14;
    U8 ctrCode  = rxBuff[9];
  //  U8 dataLen  = rxBuff[10];
    U16 command = ((U16)(rxBuff[11])<<8) + rxBuff[12]; 
    U8 flowNum= rxBuff[13];
    switch(ctrCode)
    {
         case WRITE_CTROL_CODE:             /*�������·�д������*/
         {
             PacketHead.AckMeageFlowNum = flowNum;
             switch(command)
             {
                 case SET_PARMTE_ID:       //���ò���
                 {
                      for(i=0;i<SERVER_IP_LEN;i++)
                      {
                           GPRS_NetPar.ServerIp[i] = p_buf[i];
                      }
                      GPRS_NetPar.ServerPort = ((U16)(p_buf[SERVER_IP_LEN]<<8)+p_buf[SERVER_IP_LEN+1]);
                      for(i=0;i<SERVER_APN_LEN;i++)
                      {
                           GPRS_NetPar.ServerApn[i] = p_buf[SERVER_IP_LEN+2+i];
                      }
                      GPRS_Event_Set(GPRS_EvtProc.ucRespondEvt,COMM_Event_SET_PARMTE);
                      Set_Task(MEM_WRITE, MEM_STORE_SOLID_ROMDATA);//����洢����
                 }
                 break;
                 case WRITE_ADDR_ID:      //д��ַ
                 {
                      for(i=0;i<4;i++)
                      {
                            PacketHead.addrRegion[i] = p_buf[i];
                      }
                      GPRS_Event_Set(GPRS_EvtProc.ucRespondEvt,COMM_Event_WRITE_ADDR);
                      Set_Task(MEM_WRITE, MEM_STORE_SOLID_ROMDATA);//����洢����
                 }
                 break;
                 default:
                    break;
             }
         }
         break;
         case READ_CTROL_CODE:              /*�������·���������*/
         {
             PacketHead.AckMeageFlowNum = flowNum;
             switch(command)
             {
                 case QUERY_PARMTE_ID:      //��ѯ��������
                 {
                       GPRS_Event_Set(GPRS_EvtProc.ucRespondEvt,COMM_Event_QUERY_PARMTE);
                 }
                 break;
                 case READ_MESS_ID:         //����Ϣ
                 {
                       if(g_stGPRS_Handler.ucCommBusy == 0)
                       {
                            //�����ٶ�һ���ź�ǿ��
                            g_stGPRS_Handler.StateOld = g_stGPRS_Handler.State; 
		                    g_stGPRS_Handler.State = GPRS_STATE_READ_MESS;        //���¶�ȡ��Ϣ
		                    g_stGPRS_Handler.StepPt = 0;
		               }
		               GPRS_Event_Set(GPRS_EvtProc.ucRespondEvt,COMM_Event_READ_MESS);
                       if(BSP_RELAY_CTROL_READ ==0)
                       {
                            GPRS_CommPacket.MessRead.DeviceState.staOfEle = POWER_OFF;
                       }
                       else
                       {
                            GPRS_CommPacket.MessRead.DeviceState.staOfEle = POWER_ON; 
                       }
                 }
                 break;
                 case READ_ADDR_ID:         //����ַ
                 {
                       GPRS_Event_Set(GPRS_EvtProc.ucRespondEvt,COMM_Event_READ_ADDR);
                 }
                 break;
                 default:
                    break;
             }
         }
         break;
         case CONTROL_CTROL_CODE:           /*�������·�������*/
         {
             PacketHead.AckMeageFlowNum = flowNum;
             if(command == CONTROL_ID)      //���ư�
             {
                 ctrolCmd =  p_buf[0];
                 if(ctrolCmd == ENABLE)   
                 {
                      //�ϵ� ����IO��
                      BSP_RELAY_CTROL_SET;
                      GPRS_CommPacket.MessRead.DeviceState.staOfEle = POWER_ON;            
                 }
                 else
                 {
                      //�ϵ� ����IO��
                      BSP_RELAY_CTROL_RESET;
                      GPRS_CommPacket.MessRead.DeviceState.staOfEle = POWER_OFF;           
                 }
                 Set_Task(MEM_WRITE,MEM_STORE_DYNAMIC_DATA);
                 GPRS_Event_Set(GPRS_EvtProc.ucRespondEvt,COMM_Event_CTROL);
             }
         }
         break;
         case UPLOAD_CTROL_CODE:            /*���������������ϴ�������*/
         {
             PacketHead.TerminalMeageFlowNum++;
             switch(command)
             {
                 case AUTHENTICA_ID:       //��¼������
                 {
                    //��¼�ɹ�
                    APP_GPRS_RetryMessInit();
                    GPRS_Event_Clr(GPRS_EvtProc.ucDelayEvt,COMM_Event_AUTH);
                 }
                 break;
                 case CALL_ID:             //�˹����а�����
                 {
                    //���гɹ�
                    APP_GPRS_RetryMessInit();
                    GPRS_Event_Clr(GPRS_EvtProc.ucDelayEvt,COMM_Event_CALL);
                 }
                 break;
                 default:
                    break;
             }
         }
         break;
         default:
                 break;
    }
}

static void APP_GPRS_EventProc(U8 surEvt,U8* objEvt)
{
    U8 ucID=0;
    if(GPRS_Event_IsEmpty(surEvt)== FALSE) 
    {
          ucID = Get_CommEvent(surEvt);
          GPRS_Event_Set(*objEvt,(1<<ucID));
    }

}
static U8 APP_GPRS_EvtTraverse(U8 mode)
{    
    if(GPRS_Event_IsEmpty(GPRS_EvtProc.ucRespondEvt)== FALSE)
    {
        if(mode == TRAVERSE_AND_ADDEVT)  APP_GPRS_EventProc(GPRS_EvtProc.ucRespondEvt,&g_stGPRS_Handler.ucCommEvent);
        return RESPOND_TYPE;
    }
    else if(GPRS_Event_IsEmpty(GPRS_EvtProc.ucDelayEvt)== FALSE)
    {        
        if(GPRS_Event_IsEmpty(GPRS_EvtProc.ucRetryEvt)== FALSE)
        {
            if(mode == TRAVERSE_AND_ADDEVT)  APP_GPRS_EventProc(GPRS_EvtProc.ucRetryEvt,&g_stGPRS_Handler.ucCommEvent);
            return RTY_TYPE;
        }
        return 0;
    }
    else if(GPRS_Event_IsEmpty(GPRS_EvtProc.ucUploadEvt)== FALSE)
    {
        if(mode == TRAVERSE_AND_ADDEVT)  APP_GPRS_EventProc(GPRS_EvtProc.ucUploadEvt,&g_stGPRS_Handler.ucCommEvent);
        return UPLOAD_TYPE;
    }
    else if(GPRS_Event_IsEmpty(g_stGPRS_Handler.ucCommEvent)== FALSE)
    {
        /*�����ϳ����޷����е�����*/
        g_stGPRS_Handler.ucCommEvent = 0;
        #if DEBUG_CONTROLLER
             BSP_LOG("Event is Error \r\n");
        #endif
        return 0;
    }
    else
    {
        g_stGPRS_Handler.State = GPRS_STATE_IDLE;  /*�л���IDLE̬*/
        return 0;
    }
}
static void APP_GPRS_Reset_Init(void)
{                
    
    /*��ʱ�¼�����������ϴ��¼���*/
    /*�����ʱ�¼�ȫ������������ϴ��¼���*/ 
    APP_GPRS_EventProc(GPRS_EvtProc.ucDelayEvt,&GPRS_EvtProc.ucUploadEvt);                   
    GPRS_Event_Set(GPRS_EvtProc.ucUploadEvt,COMM_Event_AUTH);
    GPRS_EvtProc.ucRespondEvt =0;
    GPRS_EvtProc.ucRetryEvt =0;
    GPRS_EvtProc.ucDelayEvt =0;
    g_stGPRS_Handler.ucCommEvent = 0;
    APP_GPRS_RetryMessInit();                   /*�ش���ر�����ʼ��*/
}

void APP_COMM_Init(void)
{    
#if DEBUG_TROPE_FUN
    PacketHead.TerminalMeageFlowNum = 0x19;
#endif
	g_stGPRS_Handler.State = GPRS_STATE_POWER_ON; 
	g_stGPRS_Handler.StateOld = GPRS_STATE_POWER_ON;
	g_stGPRS_Handler.StepPt = 0;
	GPRS_Event_Set(GPRS_EvtProc.ucUploadEvt,COMM_Event_AUTH);/*�ϵ��跢��һ����¼��*/
	Set_Task(COMM, COMM_STATE_PROC);     /*����GPRS״̬��������*/
}



void APP_GPRS_TimeProc(U16 usPeriod)
{

//    if(GPRS_ATCmdCB.Busy == 1)   //�����ݽ���ʱ��ATָ��Ҳæµ����߿��ܻ��г�ͻ���ѽ���ʱ��æµ��־ȥ��
//	{
		/*********ATָ��Ӧ��ʱ**************/
		if(GPRS_ATCmdCB.ulTimeOut >= usPeriod)
        {
            GPRS_ATCmdCB.ulTimeOut-=usPeriod;
            if(GPRS_ATCmdCB.ulTimeOut < usPeriod)
            {
				GPRS_ATCmdCB.ATCmdResult = BACK_TIMEOUT;/*��ʱ*/ 
                			
            }
        }
//	}
	if(GPRS_ATCmdCB.ucByteTimeOut >= usPeriod)
	{
	    GPRS_ATCmdCB.ucByteTimeOut -= usPeriod;
	    if(GPRS_ATCmdCB.ucByteTimeOut < usPeriod)
	    {
				GPRS_ATCmdCB.RxFrameOk = 1;
                Set_Task(COMM, COMM_CMD_PROC);
	    }
	}
	/*****************GPRSģ����ʱ����*************************/
	if(g_stGPRS_Handler.ulDelayCnt >= usPeriod)    
	{
	    g_stGPRS_Handler.ulDelayCnt -= usPeriod;
	}
    else
    {
        g_stGPRS_Handler.ulDelayCnt = 0;
    }
	
   /*****************CPRS ģ��״̬������*************************/	
	g_stGPRS_Handler.ulProcCtrTaskPer += usPeriod;
	if(g_stGPRS_Handler.ulProcCtrTaskPer >= SUB_TASK_PROCCTR_PER)
	{
	    g_stGPRS_Handler.ulProcCtrTaskPer -= SUB_TASK_PROCCTR_PER;
		Set_Task(COMM, COMM_STATE_PROC);
	}	
	/*****************CPRS ģ���ش�����*************************/
    if(GPRS_RetryCtrol.rtycntEn != DISABLE)
    {
        GPRS_RetryCtrol.rtyTimCount+=usPeriod;
        if(GPRS_RetryCtrol.rtyTimCount >= GPRS_RetryCtrol.rtyTime)
        {
              GPRS_RetryCtrol.rtyTimCount=0;
              GPRS_RetryCtrol.rtycntEn = DISABLE;
              APP_GPRS_EventProc(GPRS_EvtProc.ucDelayEvt,&GPRS_EvtProc.ucRetryEvt);
        }
    }

}


/*ģ��ATָ���������*/
void APP_SubTask_CmdProc(void)
{
    disableInterrupts();
    GPRS_ATCmdCB.Busy = 0;   
    GPRS_ATCmdCB.ucRxLen = GPRS_ATCmdCB.ucRxCnt;
    GPRS_ATCmdCB.ucRxCnt = 0;
    GPRS_ATCmdCB.RxFrameOk = 0;
    memcpy(g_stGPRS_Handler.RxBuf,GPRS_ATCmdCB.RxBuf,GPRS_ATCmdCB.ucRxLen);
    enableInterrupts();

    GPRS_ATCmdCB.ATCmdResult = APP_GPRS_ATCmdAckCheck(g_stGPRS_Handler.RxBuf); 
    /**********Ӧ�����ֱ�ӷ���***********************/ 
    if(BACK_ERROR == GPRS_ATCmdCB.ATCmdResult) 
    {
        return;
    }
    /**********Ӧ��ATָ��ֱ�ӷ���,ͨ��APP_SubTask_StateProc ����***/
    if(RSP_TYPE_CMD == GPRS_ATCmdCB.RspType)
    {
        return;
    }
    /***********�Է��������������ݽ���Э���������Ӧ����****************/
    if(RSP_TYPE_DATA == GPRS_ATCmdCB.RspType)
    {
        Set_Task(COMM,COMM_DECODE);
        #if DEBUG_CONTROLLER
        BSP_LOG("START DECODE\r\n");
        #endif
    }  
    GPRS_ATCmdCB.ATCmdResult = NO_BACK;


}


void APP_SubTask_StateProc(void)
{
    static U8 ResetTimesCount=0;
    switch(g_stGPRS_Handler.State)
    {
         case GPRS_STATE_WAIT_ACK:
         {
              //���û�н����ֱ�ӷ���
 			  if(NO_BACK == GPRS_ATCmdCB.ATCmdResult)
			  {
				    return;	
			  }  
			  //ATӦ��ʱ������ATָ��Ӧ������������ж��Ƿ���Ҫ�ط�
			  if((GPRS_ATCmdCB.ATCmdResult == BACK_ERROR) || (GPRS_ATCmdCB.ATCmdResult == BACK_TIMEOUT))  
			  {
                    if(GPRS_ATCmdCB.ucResendCnt >= GPRS_ATCmdCB.ucResendTimes)
	        	    {	
	        	        GPRS_ATCmdCB.ATCmdResult = BACK_PERTIMES_OUT;/*�ط���������*/
	        	    }			
			  }
			  //���ݲ�ͬ�Ľ��������Ӧ����
			  if(APP_GPRS_StateProcTbl[g_stGPRS_Handler.StateOld] != NULL)
		      {
		            APP_GPRS_StateProcTbl[g_stGPRS_Handler.StateOld](g_stGPRS_Handler.RxBuf);		
		      }
              if((BACK_TRUE == GPRS_ATCmdCB.ATCmdResult) || (BACK_PERTIMES_OUT == GPRS_ATCmdCB.ATCmdResult))/*ATָ��Ӧ��������ȷ*/
              {
                    /*����StateOld*/
                    g_stGPRS_Handler.StateOld = GPRS_STATE_WAIT_ACK;
              }
              GPRS_ATCmdCB.Busy = 0;              
         }
         break;
         case GPRS_STATE_POWER_ON:
         {
              if(g_stGPRS_Handler.ulDelayCnt > 0)
              {
                    return;
              }
              if(0 == g_stGPRS_Handler.StepPt)
			  {
               /*************************
                                G510 Power on���̣�
                                   ___        ___
                                     |_______|
                                         800ms
                        ��·ת�� ��->�ͣ���->��
                             **************************/
				    BSP_GPRS_POWERON_SET;   
				    g_stGPRS_Handler.StepPt++;
				    g_stGPRS_Handler.ulDelayCnt = 900;   //����850ms
				    LED_Status = CONN_INDI;              /*��������LEDָʾ*/
			  }
			  else if(1 == g_stGPRS_Handler.StepPt)
			  {
			        BSP_GPRS_POWERON_RESET;
				    g_stGPRS_Handler.StepPt++;
				    g_stGPRS_Handler.ulDelayCnt = 1000; //�ȴ�1S���ٲ���GPRSģ��
			  }
			  else/*next state*/
			  {
				    /*һЩ������0*/
                    GPRS_NetPar.NetType = NET_NO_NET;/*������״̬����*/
				    g_stGPRS_Handler.StateOld = GPRS_STATE_POWER_ON;	
				    g_stGPRS_Handler.State = GPRS_STATE_INIT;/*��ʼ��һ������*/
				    g_stGPRS_Handler.StepPt = 0;	    
			  }
         }
         break;
         /************��״̬ΪGPRSģ��POWER_OFF������ִ��������̺���POWER_ON ************/
         /**�������ֱ�����ģ��ĸ�λ����ôִ��������̺�,�ɿ�ʼATָ������**/
         case GPRS_STATE_RESET:  
         {
              if(g_stGPRS_Handler.ulDelayCnt > 0)
              {
                    return;
              }
              if(0 == g_stGPRS_Handler.StepPt)
			  {
				    //���Ų���
				    BSP_GPRS_BUCKEN_RESET;
				    g_stGPRS_Handler.StepPt++;
				    g_stGPRS_Handler.ulDelayCnt = 3500; //����3.5S
				    LED_Status = SYS_INDI;     /*ϵͳLEDָʾ*/
			  }
			  else if(1 == g_stGPRS_Handler.StepPt)
			  {
			        //���Ų���
			        BSP_GPRS_BUCKEN_SET;
				    g_stGPRS_Handler.StepPt++;
				    g_stGPRS_Handler.ulDelayCnt = 3000;
			  }
			  else/*next state*/
			  {
				    /*һЩ������0*/
                    GPRS_NetPar.NetType = NET_NO_NET;               /*������״̬����*/
				    g_stGPRS_Handler.StateOld = GPRS_STATE_RESET;	
				    g_stGPRS_Handler.State = GPRS_STATE_POWER_ON;   /*��ʼ��һ������*/
				    g_stGPRS_Handler.StepPt = 0;	
				    APP_GPRS_Reset_Init();
				    ResetTimesCount++;
				    if(ResetTimesCount >=3)
				    {
				        ResetTimesCount = 0;
				        Task_Flag_Struct.closeStaProc = ENABLE;
                        #if DEBUG_CONTROLLER
                        BSP_LOG("Close State Proc\r\n");
                        #endif
				    }
              /**�����Ӹ�λ�����������糬���趨��λ��������ô��ʱ�����ж�GPRS�Ĳ���**/
              /****����ʵ�ֿɹرն�ʱ�����������״̬������*****/
			  }
         }
         break;
         case GPRS_STATE_INIT:
         {
            if((g_stGPRS_Handler.ulDelayCnt > 0) && (Task_Flag_Struct.atReday_F == DISABLE))
            {
                    return;
            }
            if(0 == g_stGPRS_Handler.StepPt)
			{
				    g_stGPRS_Handler.StepPt++;
				    g_stGPRS_Handler.ulDelayCnt = 20000; //20S GPRS AT׼��������ʱʱ��
			}
			else
			{
			        g_stGPRS_Handler.ulDelayCnt = 0;  //������0
			        Task_Flag_Struct.atReday_F = DISABLE; //AT����ʧ��
			        g_stGPRS_Handler.StateOld = g_stGPRS_Handler.State;
			        g_stGPRS_Handler.State = GPRS_STATE_WAIT_ACK;
			        GPRS_ATCmdCB.SendATCmdEn = 1; /*enable send ATCmd*/
			        GPRS_ATCmdCB.ATCmdPt = s_ATCmdStep_Init[g_stGPRS_Handler.StepPt]; 
			}
         }
         break;
         case GPRS_STATE_CONNECT_NET:
		 {				
			g_stGPRS_Handler.StateOld = g_stGPRS_Handler.State; 
			g_stGPRS_Handler.State = GPRS_STATE_WAIT_ACK;
			GPRS_ATCmdCB.SendATCmdEn = 1;/*enable send ATCmd*/
			GPRS_ATCmdCB.ATCmdPt = s_ATCmdStep_Connnect[g_stGPRS_Handler.StepPt];
			
         }
         break;	
         case GPRS_STATE_READ_MESS:
         {
            g_stGPRS_Handler.StateOld = g_stGPRS_Handler.State; 
			g_stGPRS_Handler.State = GPRS_STATE_WAIT_ACK;
			GPRS_ATCmdCB.SendATCmdEn = 1;/*enable send ATCmd*/
			GPRS_ATCmdCB.ATCmdPt = s_ATCmdStep_Mess[g_stGPRS_Handler.StepPt];
         }
         break;
         case GPRS_STATE_COMM:
         {
            g_stGPRS_Handler.ucSendType = APP_GPRS_EvtTraverse(TRAVERSE_AND_ADDEVT);
            if((FALSE == g_stGPRS_Handler.ucSendType) || (GPRS_Event_IsEmpty(g_stGPRS_Handler.ucCommEvent)))
            {
                break;
            }

			if(0 == g_stGPRS_Handler.StepPt)
			{
				g_stGPRS_Handler.ucCommBusy = 1;/*������ںͷ�����ͨѶ*/
				LED_Status = DATA_SEND_INDI;    /*����ͨѶLEDָʾ*/
			}
            
			if(GPRS_AT_DATA == s_ATCmdStep_Comm[g_stGPRS_Handler.StepPt]) 
			{   /*APP_GPRS_WriteReturnIDPacket��APP_GPRS_WriteDataIDPacket�������Ժ϶�Ϊһ*/
			    /*�����ǵ�������ĿЭ��������࣬���ջ��ǵð������ϴ���Ӧ����Ϣ�ֿ�*/
				switch(g_stGPRS_Handler.ucSendType)
				{
				    case RESPOND_TYPE:
				    {
				        g_stGPRS_Handler.ucDataID = Get_CommEvent(GPRS_EvtProc.ucRespondEvt);
				        g_stGPRS_Handler.ucTxLen = (U8)APP_GPRS_WriteReturnIDPacket(g_stGPRS_Handler.ucDataID, g_stGPRS_Handler.TxBuf);
				    }
				    break;
				    case RTY_TYPE:
				    {
				        g_stGPRS_Handler.ucDataID = Get_CommEvent(GPRS_EvtProc.ucRetryEvt);
				    #if CONFIG_RETRY_COPY
				        g_stGPRS_Handler.ucTxLen = (U8)APP_GPRS_WriteRetryIDPacket(g_stGPRS_Handler.TxBuf);
				    #else
				        g_stGPRS_Handler.ucTxLen = (U8)APP_GPRS_WriteDataIDPacket(g_stGPRS_Handler.ucDataID, g_stGPRS_Handler.TxBuf);
				    #endif
				    }
				    break;
				    case UPLOAD_TYPE:
				    {
				        g_stGPRS_Handler.ucDataID = Get_CommEvent(GPRS_EvtProc.ucUploadEvt);
				        g_stGPRS_Handler.ucTxLen = (U8)APP_GPRS_WriteDataIDPacket(g_stGPRS_Handler.ucDataID, g_stGPRS_Handler.TxBuf);
				    }
				    break;
				}
			}
            //��ʱ�ط�����Ϊ3������Ϊ��һ��ָ���Ϊ�ڶ���ָ�����ش�������1
			if((GPRS_RetryCtrol.rtycnt >= RTY_TIMES_MAX) && (g_stGPRS_Handler.StepPt ==0))   
			{
			    g_stGPRS_Handler.StateOld = GPRS_STATE_RESET;   //��λGPRSģ��
			    g_stGPRS_Handler.State = GPRS_STATE_RESET;
			    g_stGPRS_Handler.StepPt = 0;			    
                #if DEBUG_CONTROLLER
                      BSP_LOG("Rty Reset\r\n");
                #endif
			    return;
			}
			g_stGPRS_Handler.StateOld = g_stGPRS_Handler.State; 
			g_stGPRS_Handler.State = GPRS_STATE_WAIT_ACK;
			GPRS_ATCmdCB.SendATCmdEn = 1;/*enable send ATCmd*/
			GPRS_ATCmdCB.ATCmdPt = s_ATCmdStep_Comm[g_stGPRS_Handler.StepPt];
         }
         break;
         case GPRS_STATE_IDLE:
		 {
			U8 NextStateOffset;
             /*�ж��Ƿ���������ҪͨѶ*/
			if(APP_GPRS_EvtTraverse(TRAVERSE) != FALSE)
			{
			    /*������ҪͨѶ�������л���COM״̬���¼�*/
                GPRS_State_Event_Set(STATE_Event_COMM);
			}
            
			if(GPRS_IsSwitch_State())  //�ֲ������������������͹���
			{
                break;
			}
			
			/*��ȡ��һ�����ȼ��ߵ�״̬*/
            NextStateOffset = Get_NextState(g_stGPRS_Handler.ucStateEvent);
			g_stGPRS_Handler.State = GPRS_STATE_COMM + NextStateOffset;
			GPRS_State_Event_Clr(0x01 << NextStateOffset);
	        g_stGPRS_Handler.StateOld = GPRS_STATE_IDLE; 
			g_stGPRS_Handler.StepPt = 0;		

		 }
         break;	
         default:
		 {
		    g_stGPRS_Handler.State = GPRS_STATE_INIT;
			g_stGPRS_Handler.StepPt = 0;
		 }
         break;
    }
    if(1 == GPRS_ATCmdCB.SendATCmdEn)
    {
          GPRS_ATCmdCB.SendATCmdEn = 0;
          APP_GPRS_SendATCmd(&GPRS_ATCmdCB);
    }

}


void APP_SuTask_DecodeProc(void)
{   
    U8  TropeBuf[GPRS_RX_BUFF_LEN]={0};
    U8  rxLen = GPRS_ATCmdCB.ucRxLen;
    U8 *rxBuff = &g_stGPRS_Handler.RxBuf[Get_Comma(2,g_stGPRS_Handler.RxBuf)];
    APP_GPRS_Data_Trope(rxBuff,rxLen,TropeBuf);
    if(APP_GPRS_RxDataCheck(TropeBuf) == FALSE)  return;   //��Ǵ����룬�����
    APP_GPRS_RxDataDecode(TropeBuf);
}






































