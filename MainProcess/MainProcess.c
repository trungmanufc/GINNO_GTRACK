#include "MainProcess.h"
#include <string.h>
#include <stdlib.h>

/*global variables for LTE module*/
extern uint8_t g_recv_byte;
extern uint8_t g_isDone, g_check_ok, g_check_end;
extern uint16_t	g_count, g_count_temp;
extern uint8_t g_recv_buff[MAX_SIZE_BUFF], g_buff_temp[MAX_SIZE_BUFF];
extern uint8_t g_buffer_log[30];
extern uint8_t g_id_msg, g_size_sms;
extern uint32_t g_timeNow;
extern uint32_t g_len_data_http, g_len_read_http;
/*global variables for Flash module*/
extern uint8_t 		g_data_read;

/*global variables for Bootloader*/
extern uint32_t 		g_user_address;
extern uint8_t 		  g_buff_hex_line[50];
extern uint32_t 		g_buff_data_boot[10000];
extern uint32_t 		g_count_data_boot;
extern uint32_t 		g_start_line_addr;
extern HexTypedef_t aLineHex;
uint8_t g_current_ver = 0x00;

void Process_NewFirmware(void)
{
		uint16_t number_of_downloads = 0, len_of_last = 0;
		uint32_t index_of_last = 0;
		uint8_t buff_index[10] = {0};
		uint8_t buff_len[10] = {0};
		uint8_t number_of_write = 0;
		uint16_t id_page_write = 0;
		uint8_t rest_data_last = 0;
		
		W25Q16_Erase_Chip();
		HTTP_Set_URL(76, 80, (uint8_t*)"https://raw.githubusercontent.com/NHQuang58/Test-Merge/master/Blynk_F401.hex");
		g_len_data_http = HTTP_Get(80);
		
		number_of_downloads = g_len_data_http / 4096;
		len_of_last = g_len_data_http % 4096;
		number_of_write = 4096/256;
		
		//download 4096bytes/time
		for(uint16_t i = 0; i < number_of_downloads; i++)
		{
				//index of download
				memset(buff_index, '\0', 10);
				u16_to_String((char*)buff_index, i*4096);
				HTTP_Get_Range((uint8_t*)"20", buff_index, (uint8_t*)"4096");
				g_len_read_http = HTTP_Read(80, 4096);
				Log_Info((uint8_t*)"Downloaded!\r\n", 13);
				//write to pages of external flash 
				if(g_len_read_http == 4096)
				{
						for(uint8_t i = 0; i < number_of_write; i++)
						{
								W25Q16_WritePage(&g_buff_temp[256*i], id_page_write, 0x00, 256);
								id_page_write++;
						}
						Log_Info((uint8_t*)"Write!\r\n", 8);
				}
				else Log_Info((uint8_t*)"Download not enough!\r\n", 22);	
		}
		
		//download rest data in the last time
		if(len_of_last > 0)
		{
				//index of last download
				index_of_last = number_of_downloads * 4096;
				memset(buff_index, '\0', 10);
				u16_to_String((char*)buff_index, index_of_last);
				//len of last download
				memset(buff_len, '\0', 10);
				u16_to_String((char*)buff_len, len_of_last);
				HTTP_Get_Range((uint8_t*)"20", buff_index, buff_len);
				g_len_read_http = HTTP_Read(80, len_of_last);
				Log_Info((uint8_t*)"Downloaded!\r\n", 13);
			
				//write to external flash
				number_of_write = len_of_last/256;
				if(g_len_read_http == len_of_last)
				{
						for(uint8_t i = 0; i < number_of_write; i++)
						{
								W25Q16_WritePage(&g_buff_temp[256*i], id_page_write, 0x00, 256);
								id_page_write++;
						}
						rest_data_last = len_of_last % 256;
						if(rest_data_last > 0)
						{
								for(uint8_t i = 0; i < rest_data_last; i++)
								{
										W25Q16_WriteByte(g_buff_temp[number_of_write * 256 + i], (id_page_write*256) + i);
								}
						}
						Log_Info((uint8_t*)"Write last!\r\n", 13);
				}
				else Log_Info((uint8_t*)"Download last not enough!\r\n", 27);				
		}
		
		while(g_start_line_addr < g_len_data_http)
		{
				memset(g_buff_hex_line, '\0', 50);
				g_start_line_addr = Get_Hex_Line(g_buff_hex_line, g_start_line_addr);
				Process_Hex_Line(g_buff_hex_line);	
		}
		Write_Flash();
		W25Q16_WriteByte(g_current_ver, 0x1FFFFF); //save new version
		
		Boot();
}
void Process_Main(void)
{
		W25Q16_Init();
		W25Q16_ReadByte(&g_current_ver, 0x1FFFFF);
		printf("Current Ver: %d\r\n", g_current_ver);
		HAL_Delay(1000);
		HTTP_Config_ID(1);
		HTTP_Enable_Response(1);
		HTTP_Config_TCP_IP(1, 1, (uint8_t*)"m3-world",(uint8_t*)"mms", (uint8_t*)"mms", 1);
		HTTP_Deactive_PDP(1);
		HTTP_Active_PDP(1);
		HTTP_Set_URL(73, 80, (uint8_t*)"https://raw.githubusercontent.com/NHQuang58/Test-Merge/master/version.txt");
		g_len_data_http = HTTP_Get(80);
		g_len_read_http = HTTP_Read(80, g_len_data_http);
	  printf("Read Ver: %d\r\n", atoi((char*)g_buff_temp));
		
		if(g_current_ver != atoi((char*)g_buff_temp))	//have new version
		{
				g_current_ver = atoi((char*)g_buff_temp);
				printf("New Ver: %d\r\n", g_current_ver);
				Process_NewFirmware();
		}
		else
		{
				printf("Old ver\r\n");
				Boot();
		}
}

