#include "lte.h"
#include <string.h>

extern UART_Emul_HandleTypeDef UartEmulHandle;
extern uint8_t g_recv_byte;
extern uint8_t g_isDone, g_count_temp, g_count, g_check_ok, g_check_end;
extern uint8_t g_recv_buff[MAX_SIZE_BUFF], g_buff_temp[MAX_SIZE_BUFF];
extern uint8_t g_buffer_log[30];
extern uint8_t g_id_msg, g_size_sms;
extern uint32_t g_timeNow;


/*Hard control Module LTE*/

/**
  * @brief  Hard Reset Module
  * @param  none
  * @retval none
  */
void Reset_LTE(void)
{
		HAL_GPIO_WritePin(RESET_CTRL_PORT, RESET_CTRL_PIN, GPIO_PIN_RESET);
		HAL_Delay(100);
		HAL_GPIO_WritePin(RESET_CTRL_PORT, RESET_CTRL_PIN, GPIO_PIN_SET);
		HAL_Delay(350);
		HAL_GPIO_WritePin(RESET_CTRL_PORT, RESET_CTRL_PIN, GPIO_PIN_RESET);
}

/**
  * @brief  Hard Power On Module
  * @param  none
  * @retval none
  */
void PWRCRL_ON_LTE(void)
{
		HAL_GPIO_WritePin(PWRKEY_CTRL_PORT, PWRKEY_CTRL_PIN, GPIO_PIN_RESET);
		HAL_Delay(100);
		HAL_GPIO_WritePin(PWRKEY_CTRL_PORT, PWRKEY_CTRL_PIN, GPIO_PIN_SET);
		HAL_Delay(550);
		HAL_GPIO_WritePin(PWRKEY_CTRL_PORT, PWRKEY_CTRL_PIN, GPIO_PIN_RESET);
}

/**
  * @brief	Power Off use AT command
  * @param  none
  * @retval none
  */
void PWRCRL_OFF_LTE(void)
{
		//keep Power Key at high level
		HAL_GPIO_WritePin(PWRKEY_CTRL_PORT, PWRKEY_CTRL_PIN, GPIO_PIN_RESET);
		HAL_Delay(1000);			
		Trans_Data(&UartEmulHandle, (uint8_t*)"0AT+QPOWD\r", 10);
		if(Recv_Response(&UartEmulHandle, 300) == RESPONSE_OK) Log_Info((uint8_t*)"RES_OK\n", 7);
		else Log_Info((uint8_t*)"RES_ERR\n", 8);	
}

/**
  * @brief  Enable Power supply
  * @param  none
  * @retval none
  */
void Enable_LTE(void)
{
		HAL_GPIO_WritePin(PWR_EN_PORT, PWR_EN_PIN, GPIO_PIN_RESET);
}

/**
  * @brief  Disable Power supply. This function have to use after PWRCRL_OFF_LTE()
  * @param  none
  * @retval none
  */
void Disable_LTE(void)
{
		HAL_GPIO_WritePin(PWR_EN_PORT, PWR_EN_PIN, GPIO_PIN_SET);
}

/**
  * @brief  Blynk 2 leds once
  * @param  none
  * @retval none
  */
void Blynk(void)
{
		HAL_GPIO_TogglePin(LED_PORT, LED_PWR|LED_GPS);	//on led
		HAL_Delay(200);
		HAL_GPIO_TogglePin(LED_PORT, LED_PWR|LED_GPS);	//off led
		HAL_Delay(200);
}

/*Control Module LTE*/

/**
  * @brief  Check detection sim enable
  * @param  none
  * @retval true(1) or false(0)
  */
uint8_t Check_Detect_Sim(void)
{
		/*Check enable detect sim*/
		HAL_Delay(1000);		
		Trans_Data(&UartEmulHandle, (uint8_t*)"0AT+QSIMDET?\r", 13);
		if(Recv_Response(&UartEmulHandle, 350) == RESPONSE_OK) 
		{
				Log_Info((uint8_t*)"RES_OK\n", 7);
				Get_Paragraph(g_buff_temp, g_recv_buff, 24 , 24);	//get enable status
				Log_Info(g_buff_temp, 1);
		}
		else Log_Info((uint8_t*)"RES_ERR\n", 8);
		/*if enabled detect sim, return 1, else return 0*/
		return Compare_Str(g_buff_temp, (uint8_t*)"1", 1);
}

/**
  * @brief  Enable sim
  * @param  none
  * @retval none
  */
void Enable_SIM(void)
{
//		/*Check enable detect sim*/
//		HAL_Delay(1000);		
//		Trans_Data(&UartEmulHandle, (uint8_t*)"0AT+QSIMDET?\r", 13);
//		if(Recv_Response(&UartEmulHandle, 350) == RESPONSE_OK) 
//		{
//				Log_Info((uint8_t*)"RES_OK\n", 7);
//				Get_Paragraph(g_buff_temp, g_recv_buff, 24 , 24);	//get enable status
//				Log_Info(g_buff_temp, 1);
//		}
//		else Log_Info((uint8_t*)"RES_ERR\n", 8);
		
		/*if disable, enable detect sim*/
		if(Check_Detect_Sim() == 0U)
		{
				HAL_Delay(1000);			
				Trans_Data(&UartEmulHandle, (uint8_t*)"0AT+QSIMDET=1,1\r", 16);
				Recv_Response(&UartEmulHandle, 350);
		}
}

/**
  * @brief  Set Baudrate LTE is 4800
  * @param  none
  * @retval none
  */
void Set_Baud_LTE_4800(void)
{
		HAL_Delay(1000);			
		Trans_Data_115200(&UartEmulHandle, (uint8_t*)"AT+IPR=4800;&W\r", 15);
		if(Recv_Response_Continue(&UartEmulHandle, 350) == RESPONSE_OK) Log_Info((uint8_t*)"RES_OK\n", 7);
				else Log_Info((uint8_t*)"RES_ERR\n", 8);	
}

/**
  * @brief  Check present baudrate of LTE module and log
  * @param  none
  * @retval none
  */
void Check_Baud_LTE(void)
{
		HAL_Delay(1000);			
		Trans_Data(&UartEmulHandle, (uint8_t*)"0AT+IPR?\r", 9);
		if(Recv_Response(&UartEmulHandle, 350) == RESPONSE_OK) Log_Info((uint8_t*)"RES_OK\n", 7);
				else Log_Info((uint8_t*)"RES_ERR\n", 8);	
}

/**
  * @brief  Check CPIN of LTE module and log
  * @param  none
  * @retval none
  */
void Check_CPIN_LTE(void)
{
		HAL_Delay(1000);			
		Trans_Data(&UartEmulHandle, (uint8_t*)"0AT+CPIN?\r", 10);
		if(Recv_Response(&UartEmulHandle, 5000) == RESPONSE_OK) Log_Info((uint8_t*)"RES_OK\n", 7);
				else Log_Info((uint8_t*)"RES_ERR\n", 8);
}

/*SMS Functions relate*/

/**
  * @brief  Select Text mode for SMS
  * @param  none
  * @retval OK or ERR
  */
response_t Select_Text_Mode(void)
{
		HAL_Delay(1000);			
		Trans_Data(&UartEmulHandle, (uint8_t*)"0AT+CMGF=1\r", 11);	//SMS mode : Text mode
		if(Recv_Response(&UartEmulHandle, 350) == RESPONSE_OK) 
		{
				Log_Info((uint8_t*)"RES_OK\n", 7);
				return RESPONSE_OK;
		}
		else Log_Info((uint8_t*)"RES_ERR\n", 8);
		return RESPONSE_ERR;
}

/**
  * @brief  Select ME to storage Message
  * @param  none
  * @retval OK or ERR
  */
response_t Select_ME_Memory(void)
{
		HAL_Delay(1000);			
		Trans_Data(&UartEmulHandle, (uint8_t*)"0AT+CPMS=ME,ME,ME\r", 18);
		if(Recv_Response(&UartEmulHandle, 350) == RESPONSE_OK) 
		{
				Log_Info((uint8_t*)"RES_OK\n", 7);
				return RESPONSE_OK;
		}
		else Log_Info((uint8_t*)"RES_ERR\n", 8);
		return RESPONSE_ERR;
}

/**
  * @brief  Delete memory storage SMS
  * @param  none
  * @retval OK or ERR
  */
response_t Delete_Memory_SMS(void)
{
		HAL_Delay(1000);			
		Trans_Data(&UartEmulHandle, (uint8_t*)"0AT+CMGD=1,4\r", 13);	//SMS mode : Text mode
		if(Recv_Response(&UartEmulHandle, 350) == RESPONSE_OK) 
		{
				Log_Info((uint8_t*)"RES_OK\n", 7);
				return RESPONSE_OK;
		}
		else Log_Info((uint8_t*)"RES_ERR\n", 8);
		return RESPONSE_ERR;
}

/****************MQTT Relate functions***************/

/**
  * @brief  Select Recv Mode
	* @param  clientIndex: MQTT client identifier (0-5)
	* @param  msgRecvMode: Configure the MQTT message receiving mode (0 or 1)
	* @param  msgLenEnabe:  Whether length of MQTT message received from server will be contained in URC (0 or 1)
  * @retval OK or ERR
  */
response_t MQTT_Recv_Mode(uint8_t clientIndex,
													uint8_t msgRecvMode, uint8_t msgLenEnabe)
{
		HAL_Delay(1000);
		sprintf((char*) g_buff_temp, "0AT+QMTCFG=recv/mode,%d,%d,%d\r", clientIndex, msgRecvMode, msgLenEnabe);
		Trans_Data(&UartEmulHandle, (uint8_t*)g_buff_temp, 27);
		if(Recv_Response(&UartEmulHandle, 350) == RESPONSE_OK) 
		{
				Log_Info((uint8_t*)"RES_OK\n", 7);
				return RESPONSE_OK;
		}
		else Log_Info((uint8_t*)"RES_ERR\n", 8);
		return RESPONSE_ERR;
}

/**
  * @brief  Configure the session type
	* @param  clientIndex: MQTT client identifier (0-5)
	* @param  sessionMode: Configure the session type (0 or 1)
  * @retval OK or ERR
  */
response_t MQTT_Session(uint8_t clientIndex,
												uint8_t sessionMode)
{
		HAL_Delay(1000);
		sprintf((char*) g_buff_temp, "0AT+QMTCFG=session,%d,%d\r", clientIndex, sessionMode);
		Trans_Data(&UartEmulHandle, (uint8_t*)g_buff_temp, 23);
		if(Recv_Response(&UartEmulHandle, 350) == RESPONSE_OK) 
		{
				Log_Info((uint8_t*)"RES_OK\n", 7);
				return RESPONSE_OK;
		}
		else Log_Info((uint8_t*)"RES_ERR\n", 8);
		return RESPONSE_ERR;
}

/**
  * @brief  Open a Network for MQTT Client
	* @param  clientIndex: MQTT client identifier (0-5)
	* @param  hostName: pointer to host name
	* @param  port: port id (4 digits)
  * @retval OK or ERR
  */
response_t MQTT_Open(uint8_t clientIndex,
										 uint8_t* hostName, uint16_t port)
{
		HAL_Delay(1000);
//		uint8_t lenBuffTrans = 20+lenHostName;	
		uint8_t lenBuffTrans = 20 + strlen((char*)hostName);
		sprintf((char*) g_buff_temp, "0AT+QMTOPEN=%d,%s,%d\r", clientIndex, hostName, port);
		Trans_Data(&UartEmulHandle, (uint8_t*)g_buff_temp, lenBuffTrans);
		/*wait response of +QMTOPEN*/
		Recv_Response(&UartEmulHandle, WAIT_OPEN);
		Get_Paragraph(g_buff_temp, g_recv_buff, g_count_temp - 3, g_count_temp - 3);
		if(Compare_Str(g_buff_temp, (uint8_t*)"0", 1))
		{
				Log_Info((uint8_t*)"Open Success!\n", 14);
				HAL_Delay(MAX_WAIT_TIME);
				return RESPONSE_OK;
		}
		if(Compare_Str(g_buff_temp, (uint8_t*)"2", 1))
		{
				Log_Info((uint8_t*)"Open Before!\n", 13);
				HAL_Delay(MAX_WAIT_TIME);
				return RESPONSE_MQTT_EXIST;
		}
		HAL_Delay(MAX_WAIT_TIME);
		return RESPONSE_ERR;
}

/**
  * @brief  Connect a Client to MQTT Server
	* @param  clientIndex: MQTT client identifier (0-5)
	* @param  clientID: The client identifier
	* @param  userName: user name of client
	* @param  passWord: password corresponding to the user name of the client
  * @retval OK or ERR
  */
response_t MQTT_Connect(uint8_t clientIndex,
												uint8_t* clientID, uint8_t* userName, uint8_t* passWord)
{
		HAL_Delay(MAX_WAIT_TIME);
//		uint8_t lenBuffTrans = 18 + lenUser + lenPass;
		uint8_t lenBuffTrans = 17 + strlen((char*)clientID) + strlen((char*)userName) + strlen((char*)passWord);
		sprintf((char*) g_buff_temp, "0AT+QMTCONN=%d,%s,%s,%s\r", clientIndex, clientID, userName, passWord);
		Trans_Data(&UartEmulHandle, (uint8_t*)g_buff_temp, lenBuffTrans);
		/*wait response of +QMTCONN*/
		if(Recv_Response(&UartEmulHandle, WAIT_CONNECT) == RESPONSE_MQTT_CLOSE) 
		{
				Log_Info((uint8_t*)"CONNECT-CLOSE\n", 14);
				HAL_Delay(MAX_WAIT_TIME);
				return RESPONSE_MQTT_CLOSE;
		}
		Get_Paragraph(g_buff_temp, g_recv_buff, g_count_temp - 3, g_count_temp - 3);
		if(Compare_Str(g_buff_temp, (uint8_t*)"0", 1))
		{
				Log_Info((uint8_t*)"Connect Success!\n", 17);
				HAL_Delay(MAX_WAIT_TIME);
				return RESPONSE_OK;
		}
		HAL_Delay(MAX_WAIT_TIME);
		return RESPONSE_ERR;
}
/**
  * @brief  Check MQTT Connect
  * @retval OK or ERR
  */
response_t MQTT_Check_Connect(void)
{
		Trans_Data(&UartEmulHandle, (uint8_t*)"0AT+QMTCONN?\r", 13);
		Recv_Response(&UartEmulHandle, 300);
		if(g_count_temp < 20)
		{
				Log_Info((uint8_t*)"Not connect!\n", 13);
				HAL_Delay(MAX_WAIT_TIME);
				return RESPONSE_ERR; //not connect
		}
		Get_Paragraph(g_buff_temp, g_recv_buff, 26, 26);
		if(Compare_Str(g_buff_temp, (uint8_t*)"3", 1))
		{
				Log_Info((uint8_t*)"Connect Exist!\n", 15);
				HAL_Delay(MAX_WAIT_TIME);
				return RESPONSE_OK;
		}
		return RESPONSE_ERR; //not connect
}
/**
  * @brief  Publish data to MQTT Topic
	* @param  msgId: Message identifier of packet. Range: 0–65535. It will be 0 only when QoS = 0
	* @param  QoS: The QoS level at which the client wants to publish the messages (0-2)
	* @param  retain: retain the message after it has been delivered to the current subscribers (0 or 1)
	* @param  topic: topic to publish
	* @param  lenData: length of data to publish
	* @param  lenTopic: length of topic
	* @param  pData: pointer to data buffer
  * @retval OK or ERR
  */
response_t MQTT_Publish(uint8_t clientIndex,
												uint8_t msgId, uint8_t QoS, uint8_t retain, 
												uint8_t* topic, uint8_t lenData, uint8_t* pData)
{
		HAL_Delay(MAX_WAIT_TIME);
		uint8_t lenOfLenData;
		if(lenData <= 9 ) lenOfLenData = 1;
		else if(lenData > 9 && lenData < 100) lenOfLenData = 2;
		else if(lenData >= 100) lenOfLenData = 3;
		uint8_t lenBuffTrans = 23 + strlen((char*)topic) + lenOfLenData;
		sprintf((char*) g_buff_temp, "0AT+QMTPUBEX=%d,%d,%d,%d,%s,%d\r", clientIndex, msgId, QoS, retain, topic, lenData);
		Trans_Data(&UartEmulHandle, (uint8_t*)g_buff_temp, lenBuffTrans);
		Recv_Response(&UartEmulHandle, 200);
	
		/*transmit data buffer*/
		Trans_Data(&UartEmulHandle, pData, lenData);
		/*wait response of +QMTCONN*/
		Recv_Response(&UartEmulHandle, MAX_WAIT_TIME);
		Get_Paragraph(g_buff_temp, g_recv_buff, g_count_temp - 3, g_count_temp - 3);
		if(Compare_Str(g_buff_temp, (uint8_t*)"0", 1))
		{
				Log_Info((uint8_t*)"Publish Success!\n", 17);
				HAL_Delay(MAX_WAIT_TIME);
				return RESPONSE_OK;
		}
		HAL_Delay(MAX_WAIT_TIME);
		return RESPONSE_ERR;
}

/**
  * @brief  Select SSL Mode
	* @param  clientIndex: MQTT client identifier (0-5)
	* @param  mode: 0(disable SSL) or 1 (enable SSL)
  * @param  sslIndex: SSL context index (0-5)
  * @retval OK or ERR
  */
response_t MQTT_SSL_Mode(uint8_t clientIndex,
												 uint8_t mode, uint8_t sslIndex)
{
		HAL_Delay(1000);
		sprintf((char*) g_buff_temp, "0AT+QMTCFG=ssl,%d,%d,%d\r", clientIndex, mode, sslIndex);
		Trans_Data(&UartEmulHandle, (uint8_t*)g_buff_temp, 21);
		if(Recv_Response(&UartEmulHandle, 350) == RESPONSE_OK) 
		{
				Log_Info((uint8_t*)"RES_OK\n", 7);
				return RESPONSE_OK;
		}
		else Log_Info((uint8_t*)"RES_ERR\n", 8);
		return RESPONSE_ERR;
}

/**
  * @brief  Config Certificates of SSL from file in UFS
	* @param  sslIndex: SSL context index (0-5)
  * @retval OK or ERR
  */
response_t MQTT_SSL_Certificate(uint8_t sslIndex)
{
		HAL_Delay(1000);
		/*Send CA*/
		sprintf((char*) g_buff_temp, "0AT+QSSLCFG=cacert,%d,cacert.pem\r", sslIndex);
		Trans_Data(&UartEmulHandle, (uint8_t*)g_buff_temp, 33);
		if(Recv_Response(&UartEmulHandle, 350) != RESPONSE_OK) 
		{
				Log_Info((uint8_t*)"CA_ERR\n", 7);
				return RESPONSE_ERR;
		}
		
		/*Send CC*/
		sprintf((char*) g_buff_temp, "0AT+QSSLCFG=clientcert,%d,client.pem\r", sslIndex);
		Trans_Data(&UartEmulHandle, (uint8_t*)g_buff_temp, 36);
		if(Recv_Response(&UartEmulHandle, 350) != RESPONSE_OK) 
		{
				Log_Info((uint8_t*)"CC_ERR\n", 7);
				return RESPONSE_ERR;
		}
		
		/*Send CK*/
		sprintf((char*) g_buff_temp, "0AT+QSSLCFG=clientkey,%d,user_key.pem\r", sslIndex);
		Trans_Data(&UartEmulHandle, (uint8_t*)g_buff_temp, 37);
		if(Recv_Response(&UartEmulHandle, 350) != RESPONSE_OK)
		{
				Log_Info((uint8_t*)"CK_ERR\n", 7);
				return RESPONSE_ERR;
		}
		else Log_Info((uint8_t*)"RES_OK\n", 7);
		return RESPONSE_ERR;
}

/**
  * @brief  Select SSL level
  * @param  sslIndex: SSL context index (0-5)
  * @param  level: SSL level (0-2)
  * @retval OK or ERR
  */
response_t MQTT_SSL_Level(uint8_t sslIndex,
												  uint8_t level)
{
		HAL_Delay(1000);
		sprintf((char*) g_buff_temp, "0AT+QSSLCFG=seclevel,%d,%d\r", sslIndex, level);
		Trans_Data(&UartEmulHandle, (uint8_t*)g_buff_temp, 25);
		if(Recv_Response(&UartEmulHandle, 350) == RESPONSE_OK) 
		{
				Log_Info((uint8_t*)"RES_OK\n", 7);
				return RESPONSE_OK;
		}
		else Log_Info((uint8_t*)"RES_ERR\n", 8);
		return RESPONSE_ERR;
}

/**
  * @brief  Select SSL version
  * @param  sslIndex: SSL context index (0-5)
  * @param  version: SSL version (0-4)
  * @retval OK or ERR
  */
response_t MQTT_SSL_Version(uint8_t sslIndex,
												    uint8_t version)
{
		HAL_Delay(1000);
		sprintf((char*) g_buff_temp, "0AT+QSSLCFG=sslversion,%d,%d\r", sslIndex, version);
		Trans_Data(&UartEmulHandle, (uint8_t*)g_buff_temp, 27);
		if(Recv_Response(&UartEmulHandle, 350) == RESPONSE_OK) 
		{
				Log_Info((uint8_t*)"RES_OK\n", 7);
				return RESPONSE_OK;
		}
		else Log_Info((uint8_t*)"RES_ERR\n", 8);
		return RESPONSE_ERR;
}

/**
  * @brief  Config ignore validity check for certification
  * @param  sslIndex: SSL context index (0-5)
  * @param  cipherSuite: string type format 0xYYYY
  * @retval OK or ERR
  */
response_t MQTT_SSL_Ciphersuite(uint8_t sslIndex,
																uint8_t* cipherSuite)
{
		HAL_Delay(1000);
		sprintf((char*) g_buff_temp, "0AT+QSSLCFG=ciphersuite,%d,%s\r", sslIndex, cipherSuite);
		Trans_Data(&UartEmulHandle, (uint8_t*)g_buff_temp, 33);
		if(Recv_Response(&UartEmulHandle, 350) == RESPONSE_OK) 
		{
				Log_Info((uint8_t*)"RES_OK\n", 7);
				return RESPONSE_OK;
		}
		else Log_Info((uint8_t*)"RES_ERR\n", 8);
		return RESPONSE_ERR;
}

/**
  * @brief  Config ignore validity check for certification
  * @param  sslIndex: SSL context index (0-5)
  * @param  ignoreltime: 0 (care about validity check for certificate) or 1 (ignore)
  * @retval OK or ERR
  */
response_t MQTT_SSL_Ignore(uint8_t sslIndex,
													 uint8_t ignoreltime)
{
		HAL_Delay(1000);
		sprintf((char*) g_buff_temp, "0AT+QSSLCFG=ignorelocaltime,%d,%d\r", sslIndex, ignoreltime);
		Trans_Data(&UartEmulHandle, (uint8_t*)g_buff_temp, 32);
		if(Recv_Response(&UartEmulHandle, 350) == RESPONSE_OK) 
		{
				Log_Info((uint8_t*)"RES_OK\n", 7);
				return RESPONSE_OK;
		}
		else Log_Info((uint8_t*)"RES_ERR\n", 8);
		return RESPONSE_ERR;
}

/***************HTTP Relate functions****************/

/**
  * @brief  Config contextID
	* @param  contextID: PDP context ID, range: 1-16
  * @retval OK or ERR
  */
response_t HTTP_Config_ID(uint8_t contextID)
{
		HAL_Delay(MAX_WAIT_TIME);
		sprintf((char*)g_buff_temp, "0AT+QHTTPCFG=contextid,%d\r", contextID);
		if(contextID <= 9) Trans_Data(&UartEmulHandle, (uint8_t*)g_buff_temp, 25);
		else Trans_Data(&UartEmulHandle, (uint8_t*)g_buff_temp, 26);
		if(Recv_Response(&UartEmulHandle, 500) == RESPONSE_OK)
		{
				return RESPONSE_OK;
		}
		else return RESPONSE_ERR;
}

/**
  * @brief  Enable request header
	* @param  en_or_dis: 1 (enable) or 0 (disable)
  * @retval OK or ERR
  */
response_t HTTP_Enable_Request(uint8_t en_or_dis)
{
		HAL_Delay(MAX_WAIT_TIME);
		sprintf((char*)g_buff_temp, "0AT+QHTTPCFG=requestheader,%d\r", en_or_dis);
		Trans_Data(&UartEmulHandle, (uint8_t*)g_buff_temp, 29);
		if(Recv_Response(&UartEmulHandle, 500) == RESPONSE_OK)
		{
				return RESPONSE_OK;
		}
		else return RESPONSE_ERR;
}

/**
  * @brief  Enable response header
	* @param  en_or_dis: 1 (enable) or 0 (disable)
  * @retval OK or ERR
  */
response_t HTTP_Enable_Response(uint8_t en_or_dis)
{
		HAL_Delay(MAX_WAIT_TIME);
		sprintf((char*)g_buff_temp, "0AT+QHTTPCFG=responseheader,%d\r", en_or_dis);
		Trans_Data(&UartEmulHandle, (uint8_t*)g_buff_temp, 30);
		if(Recv_Response(&UartEmulHandle, 500) == RESPONSE_OK)
		{
				return RESPONSE_OK;
		}
		else return RESPONSE_ERR;
}

/**
  * @brief  Configure Parameters of a TCP/IP Context
	* @param  contextID: The context ID. The range is 1-16
	* @param  contextType: 1 (IPV4) or 2 (IPV4V6)
	* @param  APN:  The access point name
	* @param  userName:  The user Name
	* @param  passWord:  The password
	* @param  authen:  The authentication methods (0-3)
  * @retval OK or ERR
  */
response_t HTTP_Config_TCP_IP(uint8_t contextID,
															uint8_t contextType, uint8_t* APN, uint8_t* userName, 
															uint8_t* passWord, uint8_t authen)
{
		HAL_Delay(MAX_WAIT_TIME);
//		uint8_t lenBuffTrans = 20 + lenOfAPN + lenOfUser + lenOfPass;
		uint8_t lenBuffTrans = 20 + strlen((char*)APN) + strlen((char*)userName) + strlen((char*)passWord);
		if(contextID > 9) lenBuffTrans++; 
		sprintf((char*)g_buff_temp, "0AT+QICSGP=%d,%d,%s,%s,%s,%d\r", contextID, contextType, APN, userName, passWord, authen );
		Trans_Data(&UartEmulHandle, (uint8_t*)g_buff_temp, lenBuffTrans);
		if(Recv_Response(&UartEmulHandle, 500) == RESPONSE_OK)
		{
				return RESPONSE_OK;
		}
		else return RESPONSE_ERR;
}

/**
  * @brief  Deactivate a PDP Context
	* @param  contextID: The context ID. The range is 1-16.
  * @retval OK or ERR
  */
response_t HTTP_Deactive_PDP(uint8_t contextID)
{
		HAL_Delay(MAX_WAIT_TIME);
		sprintf((char*)g_buff_temp, "0AT+QIDEACT=%d\r", contextID);
		if (contextID <= 9) Trans_Data(&UartEmulHandle, (uint8_t*)g_buff_temp, 14);
	  else Trans_Data(&UartEmulHandle, (uint8_t*)g_buff_temp, 15);
		if(Recv_Response(&UartEmulHandle, 500) == RESPONSE_OK)
		{
				return RESPONSE_OK;
		}
		else return RESPONSE_ERR;
}

/**
  * @brief  Activate a PDP Context
	* @param  contextID: The context ID. The range is 1-16.
  * @retval OK or ERR
  */
response_t HTTP_Active_PDP(uint8_t contextID)
{
		HAL_Delay(MAX_WAIT_TIME);
		sprintf((char*)g_buff_temp, "0AT+QIACT=%d\r", contextID);
		if (contextID <= 9) Trans_Data(&UartEmulHandle, (uint8_t*)g_buff_temp, 12);
	  else Trans_Data(&UartEmulHandle, (uint8_t*)g_buff_temp, 13);
		if(Recv_Response(&UartEmulHandle, 500) == RESPONSE_OK)
		{
				return RESPONSE_OK;
		}
		else return RESPONSE_ERR;
}

/**
  * @brief  Set URL for http server
	* @param  lenOfURL: len of URL
	* @param  timeOut: time out for module to connect to server
	* @param  URL: URL string
  * @retval OK or ERR
  */
response_t HTTP_Set_URL(uint8_t lenOfURL, uint8_t timeOut, uint8_t* URL)
{
		HAL_Delay(MAX_WAIT_TIME);
		sprintf((char*)g_buff_temp, "0AT+QHTTPURL=%d,%d\r", lenOfURL, timeOut);
		Trans_Data(&UartEmulHandle, (uint8_t*)g_buff_temp, 19);
		Recv_Response(&UartEmulHandle, 200);
		Trans_Data(&UartEmulHandle, URL, lenOfURL);
		if(Recv_Response(&UartEmulHandle, 500) == RESPONSE_OK)
		{
				return RESPONSE_OK;
		}
		else return RESPONSE_ERR;
}

/**
  * @brief  Get a range data request
	* @param  rspTime: timeout for the HTTP get response
	* @param  startPosition: start position of data string
	* @param  rangeL: length of data client wanna get
  * @retval OK or ERR
  */
response_t HTTP_Get_Range(uint8_t* rspTime, 
													uint8_t* startPosition, uint32_t range, uint8_t lenOfRange)
{
		HAL_Delay(MAX_WAIT_TIME);
		uint8_t lenBuffTrans = 18 + strlen((char*)rspTime) + strlen((char*)startPosition) + lenOfRange;
		sprintf((char*)g_buff_temp, "0AT+QHTTPGETEX=%s,%s,%d\r", rspTime, startPosition, range);
		Trans_Data(&UartEmulHandle, (uint8_t*)g_buff_temp, lenBuffTrans);
		Recv_Response(&UartEmulHandle, 5000);
		Get_Paragraph(g_buff_temp, g_recv_buff, lenBuffTrans + 18, lenBuffTrans + 8);
//		Log_Info(g_buff_temp, 1);
		if(Compare_Str(g_buff_temp, (uint8_t*)"0", 1))
		{
				Log_Info((uint8_t*)"Get range request!\n", 19);
				return RESPONSE_OK;
		}
		return RESPONSE_ERR;
}

/**
  * @brief  Ping to server and get IP
	* @param  contextID:  The context ID. The range is 1-16.
	* @param  hostName: host name
	* @param  outputStr: pointer to output buffer
  * @retval 0(fail) or size of IP
  */
uint8_t HTTP_Ping_IP(uint8_t contextID, 
											  uint8_t* hostName, uint8_t* outputStr)
{
		HAL_Delay(MAX_WAIT_TIME);
		uint8_t i = 0, startOfIP = 0, sizeIP = 0;
		uint8_t lenBuffTrans = 13 + strlen((char*)hostName);
		if(contextID > 9) lenBuffTrans++;
		sprintf((char*)g_buff_temp, "0AT+QPING=%d,%s\r", contextID, hostName);
		Trans_Data(&UartEmulHandle, (uint8_t*)g_buff_temp, lenBuffTrans);
		Recv_Response(&UartEmulHandle, 5000);
		while(*(g_recv_buff+i) != ' ')
    {
        i++;
    }
		if(*(g_recv_buff+i+1) != '0') return 0;
		startOfIP = i + 4;
		while(*(g_recv_buff+startOfIP+sizeIP) != '"')
    {
        *(outputStr + sizeIP) = *(g_recv_buff + startOfIP + sizeIP);
        sizeIP++;
    }
		return sizeIP;
}

/*End of file*/
