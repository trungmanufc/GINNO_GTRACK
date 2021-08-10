#ifndef __LTE_H
#define __LTE_H

#include "soft_uart.h"
#include <stdio.h>

#define MAX_WAIT_TIME			 	2000
#define WAIT_OPEN						10000

#define PWR_EN_PIN 					GPIO_PIN_9
#define PWR_EN_PORT					GPIOC
#define WAKEUP_CTRL_PIN 		GPIO_PIN_3
#define WAKEUP_CTRL_PORT 		GPIOB
#define RESET_CTRL_PIN 			GPIO_PIN_4
#define RESET_CTRL_PORT 		GPIOB
#define PWRKEY_CTRL_PIN 		GPIO_PIN_5
#define PWRKEY_CTRL_PORT 		GPIOB
#define LED_PORT						GPIOC
#define LED_PWR							GPIO_PIN_7
#define LED_GPS							GPIO_PIN_8

/*Hard control Module LTE*/
void Reset_LTE(void);
void PWRCRL_ON_LTE(void);
void PWRCRL_OFF_LTE(void);
void Enable_LTE(void);
void Blynk(void);

/*Control Module LTE*/

/**
  * @brief  Check detection sim enable
  * @param  none
  * @retval true(1) or false(0)
  */
uint8_t Check_Detect_Sim(void);

/**
  * @brief  Enable sim
  * @param  none
  * @retval none
  */
void Enable_SIM(void);

/**
  * @brief  Check present baudrate of LTE module and log
  * @param  none
  * @retval none
  */
void Check_Baud_LTE(void);

/**
  * @brief  Check CPIN of LTE module and log
  * @param  none
  * @retval none
  */
void Check_CPIN_LTE(void);

/*SMS Functions relate*/

/**
  * @brief  Select Text mode for SMS
  * @param  none
  * @retval OK or ERR
  */
response_t Select_Text_Mode(void);

/**
  * @brief  Select ME to storage Message
  * @param  none
  * @retval OK or ERR
  */
response_t Select_ME_Memory(void);

/**
  * @brief  Delete memory storage SMS
  * @param  none
  * @retval OK or ERR
  */
response_t Delete_Memory_SMS(void);

/****************MQTT Relate functions***************/

/**
  * @brief  Select Recv Mode
	* @param  clientIndex: MQTT client identifier (0-5)
	* @param  msgRecvMode: Configure the MQTT message receiving mode (0 or 1)
	* @param  msgLenEnabe:  Whether length of MQTT message received from server will be contained in URC (0 or 1)
  * @retval OK or ERR
  */
response_t MQTT_Recv_Mode(uint8_t clientIndex,
													uint8_t msgRecvMode, uint8_t msgLenEnabe);

/**
  * @brief  Configure the session type
	* @param  clientIndex: MQTT client identifier (0-5)
	* @param  sessionMode: Configure the session type (0 or 1)
  * @retval OK or ERR
  */
response_t MQTT_Session(uint8_t clientIndex,
												uint8_t sessionMode);

/**
  * @brief  Open a Network for MQTT Client
	* @param  clientIndex: MQTT client identifier (0-5)
	* @param  hostName: pointer to host name
	* @param  port: port id (4 digits)
	* @param  lenHostName: len of host name
  * @retval OK or ERR
  */
response_t MQTT_Open(uint8_t clientIndex,
										 uint8_t* hostName, uint16_t port);

/**
  * @brief  Connect a Client to MQTT Server
	* @param  clientIndex: MQTT client identifier (0-5)
	* @param  clientID: The client identifier
	* @param  userName: user name of client
	* @param  passWord: password corresponding to the user name of the client
  * @retval OK or ERR
  */
response_t MQTT_Connect(uint8_t clientIndex,
												uint8_t clientID, uint8_t* userName, uint8_t* passWord);
												
/**
  * @brief  Connect a Client to MQTT Server
	* @param  msgId: Message identifier of packet. Range: 0–65535. It will be 0 only when QoS = 0
	* @param  QoS: The QoS level at which the client wants to publish the messages (0-2)
	* @param  retain: retain the message after it has been delivered to the current subscribers (0 or 1)
	* @param  topic: topic to publish
	* @param  lenData: length of data to publish
	* @param  pData: pointer to data buffer
	* @param  lenOfLenData: numbers of digit of lendata (1,2,3,4...)
  * @retval OK or ERR
  */
response_t MQTT_Publish(uint8_t clientIndex,
												uint8_t msgId, uint8_t QoS, uint8_t retain, 
												uint8_t* topic, uint8_t lenData, uint8_t* pData, uint8_t lenOfLenData);

/***************HTTP Relate functions****************/

/**
  * @brief  Config contextID
	* @param  contextID: PDP context ID, range: 1-16
  * @retval OK or ERR
  */
response_t HTTP_Config_ID(uint8_t contextID);

/**
  * @brief  Enable request header
	* @param  en_or_dis: 1 (enable) or 0 (disable)
  * @retval OK or ERR
  */
response_t HTTP_Enable_Request(uint8_t en_or_dis);

/**
  * @brief  Enable response header
	* @param  en_or_dis: 1 (enable) or 0 (disable)
  * @retval OK or ERR
  */
response_t HTTP_Enable_Response(uint8_t en_or_dis);

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
															uint8_t contextType, uint8_t* APN, uint8_t* userName, uint8_t* passWord, uint8_t authen);
															
/**
  * @brief  Deactivate a PDP Context
	* @param  contextID: The context ID. The range is 1-16.
  * @retval OK or ERR
  */
response_t HTTP_Deactive_PDP(uint8_t contextID);

/**
  * @brief  Activate a PDP Context
	* @param  contextID: The context ID. The range is 1-16.
  * @retval OK or ERR
  */
response_t HTTP_Active_PDP(uint8_t contextID);

/**
  * @brief  Set URL for http server
	* @param  lenOfURL: len of URL
	* @param  timeOut: time out for module to connect to server
	* @param  URL: URL string
  * @retval OK or ERR
  */
response_t HTTP_Set_URL(uint8_t lenOfURL, uint8_t timeOut, uint8_t* URL);

/**
  * @brief  Get a range data request
	* @param  rspTime: timeout for the HTTP get response
	* @param  startPosition: start position of data string
	* @param  rangeL: length of data client wanna get
  * @retval OK or ERR
  */
response_t HTTP_Get_Range(uint8_t* rspTime, 
													uint8_t* startPosition, uint32_t range, uint8_t lenOfRange);

/**
  * @brief  Ping to server and get IP
	* @param  contextID:  The context ID. The range is 1-16.
	* @param  hostName: host name
	* @param  outputStr: pointer to output buffer
  * @retval 0(fail) or size of IP
  */
uint8_t HTTP_Ping_IP(uint8_t contextID, 
											  uint8_t* hostName, uint8_t* outputStr);
												
#endif

/*End of file*/
