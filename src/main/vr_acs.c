#include <cfg.h>
#include <stdio.h>  
#include <stdlib.h>  
#include <string.h> 
#include <stdint.h>
#include <unistd.h>  
#include <sys/types.h>  
#include <sys/socket.h>  
#include <netinet/in.h>  
#include <arpa/inet.h>  
#include <sys/time.h>
#include <time.h>
#include <unistd.h>
#include <math.h>

#include "vr_acs.h"

static int pack_header(uint8_t cmd,char *data, int *len)
{
	net_cs_head_t net_cs_head;

	net_cs_head.event_identity 		= htons(NET_ACS_EVENT_IDENTITY);
	net_cs_head.protocol_identity	= htons(NET_ACS_PROTOCOL_IDENTITY);
	net_cs_head.len					= 0;
	net_cs_head.slave_addr			= NET_ACS_SEND_SLAVE_ADDR;
	net_cs_head.cmd					= cmd;
	switch(cmd)
	{
	case C2M_CODE_GET_DATA:
		net_cs_head.len = htons(READ_REGISTER_LEN);
		break;
	case C2M_CODE_REBOOT:
		net_cs_head.len = htons(REBOOT_DEV_LEN);
		break;
	default:
		break;
	}
	memcpy(data, (char *)&net_cs_head, sizeof(net_cs_head_t));
	(*len) += sizeof(net_cs_head_t);

	return 0;
}

static int handle_acs_get_data(char *buf,int ulen, int connectfd)
{
	char data[1024] = {0};
	int data_len = 0;
	int tmp = 0;
	
	pack_header(C2M_CODE_GET_DATA, data, &data_len);
	data[data_len]=(unsigned char)(READ_REGISTER_LEN-3);
	data_len += 1;
	tmp = htons((unsigned short)VERSION);
	memcpy(&data[data_len], (char *)&tmp, 2);
	data_len += 2;
	/*TODO*/
	data_len += 32;
	
	send(connectfd, data, data_len, 0);
	
	return 0;
}

static int handle_acs_reboot(char *buf,int ulen, int connectfd)
{
	char data[1024] = {0};
	int data_len = 0;
	int tmp = 0;
	
	pack_header(C2M_CODE_REBOOT, data, &data_len);
	tmp = htons((unsigned short)REBOOT_REGISTER_START_ADDR);
	memcpy(&data[data_len], (char*)&tmp, 2);
	
	data_len += 2;
	/*TODO*/
	data_len += 2;
	
	send(connectfd, data, data_len, 0);
	
	return 0;

}


acs_msg_handle_t c2m_handler[] = {
	{C2M_CODE_GET_DATA,			handle_acs_get_data},
	{C2M_CODE_REBOOT,			handle_acs_reboot}
};


static int handle_acs_msg(char *buf, int len, int cmd, int connectfd)
{
	if(buf == NULL || len <= 0)
	{
		printf("data err\n");
		return -1;
	}
	int i = 0;
	for(i = 0; i< sizeof(c2m_handler)/sizeof(acs_msg_handle_t); i++)
	{
		if((char)cmd == (char)c2m_handler[i].cmd)
		{
			c2m_handler[i].msg_func(buf, len, connectfd);
			return 1;
		}
	}
	return 0;
}

int acs_create_random_num()
{	

	return 0;
}

#define TZ_DEL_TIME			(0)

struct tm *acs_convert_localtime(const time_t *timep,struct tm *result)
{
	time_t cur;
	
	if(timep == NULL || result == NULL)
	{
		return NULL;
	}

	cur = *timep + TZ_DEL_TIME;
	return localtime_r(&cur,result);
}


void *acs_vr_thread(void *arg)
{
	if(arg == NULL){
		printf("the argument for vr_thread is err\n");
	}

	int port = *(int *)arg;
	char buf[1024] = {0};
	int len = 0;
	int i = 0;
	time_t timep;
	struct tm tmcur;
	
	int listenfd = 0, connectfd = 0;
	struct sockaddr_in server;
	struct sockaddr_in client;
	socklen_t addrlen;
	printf("acs,create success, port:%d\n", port);
	if((listenfd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
	{
		printf("create socket faild\n");
		exit(1);
	}
	
	int opt = SO_REUSEADDR;
	setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
	bzero(&server, sizeof(server));
	server.sin_family 	= AF_INET;
	server.sin_port		= htons(port);
	server.sin_addr.s_addr = htonl(INADDR_ANY);
	if(bind(listenfd, (struct sockaddr *)&server, sizeof(server)) == -1)
	{
		printf("bind err\n");
		exit(1);
	}
	if(listen(listenfd, BACKLOG) == -1)
	{
		printf("listen err\n");
		exit(1);
	}
	addrlen = sizeof(client);
	while(1)
	{	
		if((connectfd = accept(listenfd, (struct sockaddr *)&client, &addrlen)) == -1)
		{
			printf("accept err\n");
			exit(1);
		}
		printf("\n******a connection from client's ip is %s, prot is %d, server port is%d******\n",inet_ntoa(client.sin_addr),htons(client.sin_port), port); 
		memset(buf, 0, sizeof(buf));
		if((len = recv(connectfd, buf, sizeof(buf), 0)) <= 0)
		{
			printf("have no data\n");
		}
		else
		{
			printf("recv data:");
			for(i = 0; i< len; i++)
			{
				printf("%02x ", (unsigned char)buf[i]);
			}
			printf("\n");
			time(&timep);
			acs_convert_localtime(&timep, &tmcur);
			printf("time:[%04d-%02d-%02d %02d:%02d:%02d],cmd:0x%02x\n", tmcur.tm_year + 1900,tmcur.tm_mon + 1, tmcur.tm_mday,
				tmcur.tm_hour, tmcur.tm_min,tmcur.tm_sec, (char)buf[5]);
			handle_acs_msg(buf, len, buf[7], connectfd);
		}
		shutdown(connectfd, SHUT_RDWR);
		close(connectfd);
		sleep(1);
	}
}

