#include <stdio.h>
#include "mqttclient.h"
#include "platform_mutex.h"
#include "cmsis_os.h"
#include "FreeRTOS.h"
#include "queue.h"
#include "event_groups.h"               // ARM.FreeRTOS::RTOS:Event Groups
#include "semphr.h"    
#include "page.h"

extern EventGroupHandle_t MqttNotifyEvent;
mqtt_client_t *client = NULL;

//subscribe switch
#if 1
static void smarthome_msg_handler(void* client, message_data_t* msg)
{
	printf("Get msg: %s\r\n", (char *)msg->message->payload);
	//if (strstr((char *)msg->message->payload, "\"dev\":\"led\",\"status\":\"0\""))
	if (strstr((char *)msg->message->payload, "hello")) 
	{
	   printf("led off\r\n");
	   //HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_SET);
	}
	else if (strstr((char *)msg->message->payload, "\"dev\":\"led\",\"status\":\"1\""))
	{
	   printf("led on\r\n");
	   //HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_RESET);
	}
	else if (strstr((char *)msg->message->payload, "\"dev\":\"led\",\"status\":\"2\""))
	{
		printf("led switch\r\n");
		//HAL_GPIO_TogglePin(LED_GPIO_Port, LED_Pin);
	}
}
#endif

int makeMsg(PageID id, mqtt_message_t* msg, char* buf);


void MqttTask(void *argument)
{
	mqtt_message_t msg;
    memset(&msg, 0, sizeof(msg));
	EventBits_t MqttNotifyEventbit;
//	uint8_t rx_data[200] = {0};        
    int err;
	int cnt = 0;
	char topicName[40];
	char buf[40];

	mqtt_log_init();

	client = mqtt_lease();

	mqtt_set_port(client, "1883");
	//mqtt_set_host(client, "192.168.0.130");
	mqtt_set_host(client, "47.114.187.247"); /* iot.100ask.net: 47.114.187.247 */
	mqtt_set_client_id(client, random_string(10));
	mqtt_set_user_name(client, random_string(10));
	mqtt_set_password(client, random_string(10));
	mqtt_set_clean_session(client, 1);

	do {
		err = mqtt_connect(client);
	} while (err);

	printf("mqtt_connect err = %d\r\n", err);

	/*
    msg.payload = (void *) buf;
    msg.qos = QOS0;
	msg.payloadlen = strlen(buf);
	mqtt_publish(client, "homex", &msg);

	err = mqtt_subscribe(client, "homex", QOS0, smarthome_msg_handler);	  
	printf("subscribe err = %d\r\n", err);
	*/
  while (1)
  {

	  MqttNotifyEventbit = xEventGroupWaitBits(MqttNotifyEvent, 1<<0|1<<1|1<<2, 
			pdTRUE, pdFALSE, portMAX_DELAY);
	  if(MqttNotifyEventbit&(1<<0)){
		  makeMsg(Light1, &msg, buf);
		  sprintf(topicName, "lightdemo/switch");
	  }
	  else if(MqttNotifyEventbit&(1<<1)){
		  makeMsg(Light2, &msg, buf);
		  sprintf(topicName, "lightdemo/multi");
	  }
	  else if(MqttNotifyEventbit&(1<<2)){
		  makeMsg(LightBar, &msg, buf);
		  sprintf(topicName, "lightdemo/cont");
	  }

	  //makeMsg(Light1, &msg, buf);
	  //sprintf(topicName, "lightdemo/switch");
	  
	  msg.payload = (void *) buf;
	  msg.qos = QOS0;
	  msg.payloadlen = strlen(buf);

	  err = mqtt_publish(client, topicName, &msg);	  
	  
	  //printf("Publish msg: %s, err = %d\r\n", buf, err);

	  //mqtt_list_subscribe_topic(client); 	
  }
}


int makeMsg(PageID id, mqtt_message_t* msg, char* buf){

	int payloadNum;
	payloadNum = getPagedata(id);
	switch(id){
		case Light1:
			if(payloadNum == 1)
				sprintf(buf, "Lightoff");
			else if(payloadNum == 2)
				sprintf(buf, "Lighton");
			break;
		
		case Light2:
			if(payloadNum == 1)
				sprintf(buf, "Level1");
			else if(payloadNum == 2)
				sprintf(buf, "Level2");
			else if(payloadNum == 3)
				sprintf(buf, "Level3");
			break;
		
		case LightBar:
			if(payloadNum>=0 && payloadNum<=100)
				sprintf(buf, "Brightness%d", payloadNum);
			break;
		default:break;
	}
}