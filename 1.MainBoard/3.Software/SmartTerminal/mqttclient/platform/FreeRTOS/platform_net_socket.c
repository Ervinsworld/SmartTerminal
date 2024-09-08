/*
 * @Author: jiejie
 * @Github: https://github.com/jiejieTop
 * @Date: 2020-01-10 23:45:59
 * @LastEditTime: 2020-04-25 17:50:58
 * @Description: the code belongs to jiejie, please keep the author information and source code according to the license.
 */
#include "platform_net_socket.h"
#include "platform_timer.h"
#include "usart.h"
#include "stdio.h"


int platform_net_socket_connect(const char *host, const char *port, int proto)
{
	char cmd[100];
//	char *ssid = "W201314H";
//	char *pwd  = "heguobao";
	char *ssid = "ErvinsiPhone";
	char *pwd  = "88888888";
	unsigned int rc;

	rc = ESP8266_SendCommand("AT+RST", "OK", 2000);
	if (rc)
		return -1;
	
	HAL_Delay(800);

	rc = ESP8266_SendCommand("AT+CWMODE=1", "OK", 500);
	if (rc)
		return -1;

	sprintf(cmd, "AT+CWJAP=\"%s\",\"%s\"", ssid, pwd);
	rc = ESP8266_SendCommand(cmd, "OK", 5000);
	if (rc)
		return -1;
	
	rc = ESP8266_SendCommand("AT+CIPCLOSE", "OK", 500);
	
	sprintf(cmd, "AT+CIPSTART=\"TCP\",\"%s\",%s,0", host, port);
	rc = ESP8266_SendCommand(cmd, "OK", 5000);
	if (rc)
		return -1;

	ESP8266_GetIP();

	return 0;
}

int platform_net_socket_recv(int fd, void *buf, size_t len, int flags)
{
	/* 在mbedtls_net_recv里才会调用platform_net_socket_recv函数
	 * 我们没有使用mbedtls_net_recv
	 * 所以不用实现platform_net_socket_recv
	 */
    return 0;
}

int platform_net_socket_recv_timeout(int fd, unsigned char *buf, int len, int timeout)
{
	extern int ESP8266ReadPacket(unsigned char *buf, int len);
	int time = platform_timer_now();
	int rlen = 0;
	int ret = len;

	timeout += time;
	
	while ((platform_timer_now() < timeout) && len)
	{
		rlen = ESP8266ReadPacket(buf, len);
		buf += rlen;
		len -= rlen;
	}

	return (ret - len);
}

int platform_net_socket_write(int fd, void *buf, size_t len)
{
	/* 在mbedtls_net_send里才会调用platform_net_socket_write函数
	 * 我们没有使用mbedtls_net_recv
	 * 所以不用实现platform_net_socket_write
	 */
	return 0;
}

int platform_net_socket_write_timeout(int fd, unsigned char *buf, int len, int timeout)
{
	extern int ESP8266_Send_Packet(unsigned char *data, int len);
	int i;
	if (len == 2)
		printf("datas: 0x%x, 0x%x\r\n", buf[0], buf[1]);
	printf("Send Packet: ");
	for (i = 0; i < len; i++)
		printf("%02x ", buf[i]);
	printf("\r\n");
	return ESP8266_Send_Packet(buf, len);
}

int platform_net_socket_close(int fd)
{
	ESP8266_SendCommand("AT+CIPCLOSE", "OK", 500);
	return 0;
}

