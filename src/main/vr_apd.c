#include <cfg.h>

#include <stdio.h>  
#include <stdlib.h>  
#include <string.h>  
#include <unistd.h>  
#include <sys/types.h>  
#include <sys/socket.h>  
#include <netinet/in.h>  
#include <arpa/inet.h>  
#include <sys/time.h>
#include <time.h>
#include <unistd.h>
#include <math.h>

#include "vr_apd.h"


#define N 2000

#define START_PHASE_1		(30*60)
#define START_PHASE_2		(210*60)

#define START_Q_1		(600)
#define START_Q_2		(200)

static double g_phase_rd[N];
static double g_Q_rd[N];
static double g_L_rd[N];

static int rd_distribution(double *rd, int num, float start, float end)
{	
	double x1,x2,y1,y2;	
	double pi2=6.28318530728,mx,mi,ave=0;	
	int i;	

	srand(time(NULL));	
	for (i=0;i<=num-2;i=i+2)	
	{		
		x1=1.0*rand()/RAND_MAX;		
		x2=1.0*rand()/RAND_MAX;		
		y1= sqrt((-2.0*log(x1))) * cos(pi2*x2);		
		y2= sqrt((-2.0*log(x1))) * sin(pi2*x2);		
		rd[i]=y1;		
		rd[i+1]=y2;	
	}	
	mx=rd[0];mi=rd[0];	
	for (i=0;i<num;i++)	
	{		
		if (rd[i]>mx)
			mx=rd[i];		
		if (rd[i]<mi)
			mi=rd[i];	
	}	

	for (i=0;i<num;i++) 		
		rd[i] = (rd[i]-mi)/(mx-mi+0.001) * (end-start) + start;	
      
	return 0;
}


/*校验*/
static int pack_msg_add_checksum(char *dat,unsigned int len) 
{
	unsigned int i;
	unsigned char check_sum = 0;
	
	for(i = 0;i < len;i++) {
		check_sum += (unsigned char)*dat++;
	}
	*dat = (char)check_sum;
	return len+1;
}

/*转义*/
static int special_char_split(char *dat,char *out_dat) {
	if((unsigned char)*dat == (unsigned char)ESCAPE_BB) {
		*out_dat++ = ESCAPE_BB;
		*out_dat = ESCAPE_B;
		return 2;
	}
	if((unsigned char)*dat == (unsigned char)ESCAPE_0xAA) {
		*out_dat++ = ESCAPE_BB;
		*out_dat = ESCAPE_A;
		return 2;
	}
	*out_dat = *dat;
	return 0;

}

/*遍历查找*/
static int pack_msg_add_escape(char *dat,unsigned int len) {
	char *p = dat;
	unsigned int  i = 0;
	int j;
	int ret;
	char tmp_buf[2];
	int ret_len;
	
	ret_len = len;
	for(i = 0;i < len;i++ ) {
	
		ret = special_char_split(p,tmp_buf);
		if(ret == 2) {
//			printf("i:%d\n\n", i);
			*p = tmp_buf[0];
			for (j=len-i; j>=2; --j)
			{
				*(p+j) = *(p+j-1);
			}
			*(++p) = tmp_buf[1];
			
			ret_len++;
		}
		p++;
	}
	return ret_len;

}


/*0x01*/
static int handle_syn_para(char *buf,int ulen, int connectfd)
{
	unsigned char data[8] = {0xaa,0x00,0x00,0x00,0x05,0x01,0x06,0xaa};
	send(connectfd, data, sizeof(data), 0);
	return 1;
}

/* 0x02 下发采集命令，返回是否同步过参数*/
static int handle_start_sample(char *buf,int ulen, int connectfd)
{
	char sync_flag = 1;
	unsigned char data[9] = {0xaa,0x00,0x00,0x00,0x06,0x02,0x01,0x09,0xaa};
	pack_msg_add_checksum((char *)data+1, 6);
	
	send(connectfd, &data, sizeof(data), 0);
	
	return 1;
}

/*0x03*/
static int handle_collect_data(char *buf, int ulen, int connectfd)
{	
	fault_t fault[8] = {{0x01, 0,0},{0x02, 0,0},{0x12, 0,0},{0x22, 0,0},
		{0x03, 0,0},{0x04, 0,0},{0x05, 0,0},{0x06, 0,0}};
	char data[30000] = {0xaa,0x00,0x00,0x00,0x06,0x03};
	char *ptr = NULL;
	unsigned int len = 0;
	unsigned short int pulse_num[3] = {0};
	unsigned short int tmp = 0;
	int i = 0, j = 3, datalen = 0;

	struct timeval tv;
	
	len += 6;
	unsigned char busy_flag = 0;
	ptr = data +len;
	
	memcpy(ptr, (char *)&busy_flag, 1);
	len += sizeof(busy_flag);
	ptr = data + len;
	
	memcpy(ptr, (unsigned char*)&fault, sizeof(fault));
	len += sizeof(fault);
	ptr = data +len;
	
	gettimeofday(&tv,NULL);  
	srand(tv.tv_usec);
	if(busy_flag == 0)
	{
		while(j--)
		{
			pulse_num[j] = 1024;
			tmp = htons(pulse_num[j]);
			memcpy(ptr , &tmp, 2);
			len += sizeof(pulse_num[j]);
			ptr = data + len;
		}
		j = 3;
		while(j--)
		{
			for(i=0; i< pulse_num[j]; i++)
			{
				tmp = htons((unsigned short int )150);
				memcpy(ptr, &tmp, 2);	//L
				ptr = ptr + 2;
				
				if(rand()%N < (N/2))
				{
					tmp = htons((unsigned short)START_Q_1 + (unsigned short)g_Q_rd[(unsigned short)rand()%N]);
					memcpy(ptr, &tmp, 2);	//Q
					ptr = ptr + 2;
					
					tmp = htons((unsigned short)START_PHASE_1 + (unsigned short)g_phase_rd[(unsigned short)rand()%N]);
					memcpy(ptr, &tmp, 2);	//PH
					ptr = ptr + 2;
				}
				else
				{
					tmp = htons((unsigned short)START_Q_2+ (unsigned short)g_Q_rd[(unsigned short)rand()%N]);
					memcpy(ptr, &tmp, 2);	//Q
					ptr = ptr + 2;
					
					tmp = htons((unsigned short)START_PHASE_2+ (unsigned short)g_phase_rd[(unsigned short)rand()%N]);
					memcpy(ptr, &tmp, 2);	//PH
					ptr = ptr + 2;
				}
				len = len + 6;
			}
		}
	}
	datalen = htonl(len -1);
	memcpy(data+1, &datalen, 4);
	
	
	pack_msg_add_checksum((char *)data+1, len -1);
	len ++;
	len = pack_msg_add_escape(data + 6, len -6) + 6;

	*(data+len) = 0xaa;
	len++;
	printf("send collect_data\n");
//	for(i = 0; i< len; i++)
//	{
//		printf("%02x ", (unsigned char)data[i]);
//	}
//	printf("\n\n\n");
	send(connectfd, data, len, 0);
	
	return 0;
}

/*0x04*/
static int handle_beep(char *buf,int ulen, int connectfd)
{
	unsigned char data[8] = {0xaa,0x00,0x00,0x00,0x05,0x04,0x09,0xaa};
	send(connectfd, data, sizeof(data), 0);

	return 1;
}

/*0x05*/
static int handle_syn_clock(char *buf,int ulen, int connectfd)
{
	unsigned char data[8] = {0xaa,0x00,0x00,0x00,0x05,0x05,0x0a,0xaa};
	send(connectfd, data, sizeof(data), 0);

	return 1;
}

/*0x06*/
static int handle_request_raw_data(char *buf,int ulen, int connectfd)
{
	unsigned char data[8] = {0xaa,0x00,0x00,0x00,0x05,0x06,0x0b,0xaa};
	send(connectfd, data, sizeof(data), 0);
	return 1;
}

/*0x11*/
static int handle_request_refine_data(char *buf,int ulen, int connectfd)
{
	unsigned char data[8] = {0xaa,0x00,0x00,0x00,0x05,0x11,0x16,0xaa};
	send(connectfd, data, sizeof(data), 0);

	return 1;
}

/*0x07*/
static int handle_reboot(char *buf,int ulen, int connectfd)
{
	char reboot_flag = 1;
	unsigned char data[9] = {0xaa,0x00,0x00,0x00,0x06,0x07,0x01,0x0e,0xaa};
	send(connectfd, data, sizeof(data), 0);

	return 1;
}

/*0x08*/
static int handle_remote_update(char *buf,int ulen, int connectfd)
{
	char result = 1;
	unsigned char data[9] = {0xaa,0x00,0x00,0x00,0x06,0x08,0x01,0x0f,0xaa};
	send(connectfd, data, sizeof(data), 0);

	return 1;
}

/*0x09*/
static int handle_syn_state(char *buf,int ulen, int connectfd)
{
	static char result = 1;

	unsigned char data[9] = {0xaa,0x00,0x00,0x00,0x06,0x09,0x01,0x10,0xaa};
	send(connectfd, data, sizeof(data), 0);


	return 1;
}

/*0xd0*/
static int handle_debug_build(char *buf,int ulen, int connectfd)
{
	char *version = "virtual_apd20160510 00:00:00";
	unsigned char data[256] = {0xaa,0x00,0x00,0x00,0x06,0xd0};
	unsigned int len = 0;
	int datalen = 0;
	len += 6;
	memcpy(data+len, version, strlen(version));
	len += strlen(version);
	datalen = htonl(len -1);
	memcpy(data+1, &datalen, sizeof(datalen));
	pack_msg_add_checksum((char *)data+1, len -1);
	len ++;
	*(data+len) = 0xaa;
	len ++;
	
	send(connectfd, data, len, 0);
	return 1;
}

/*0xd1*/
static int handle_debug_md5(char *buf,int ulen, int connectfd)
{
	char *data = "virtual apdmd5";
	send(connectfd, &data, strlen(data), 0);
	return 1;
}

/*0xd5*/
static int handle_debug_amp(char *buf,int ulen, int connectfd)
{
	char result = 5;
	send(connectfd, &result, sizeof(result), 0);
	return 1;
}

/*0xdc*/
static int handle_soft_version(char *buf,int ulen, int connectfd)
{
	char *version = "virtual_apd20160510 00:00:00";
	unsigned char data[1024] = {0xaa,0x00,0x00,0x00,0x06,0xdc};
	unsigned int len = 0;
	int datalen = 0;
	int i = 0;
	
	len += 6;
	memcpy(data+len, version, strlen(version));
	len += strlen(version);
	datalen = htonl(len -1);
	memcpy(data+1, &datalen, sizeof(datalen));
	pack_msg_add_checksum((char *)data+1, len -1);
	len ++;
	*(data+len) = 0xaa;
	len ++;
	
	send(connectfd, data, len, 0);

	return 1;
}


apd_msg_handle_t d2m_handler[] = {
	{D2M_CMD_SYN_PARA,			handle_syn_para},
	{D2M_CMD_START_SAMPLE,		handle_start_sample},
	{D2M_CMD_COLLECT_DATA,		handle_collect_data},
	{D2M_CMD_BEEP,				handle_beep},
	{D2M_CMD_SYN_CLOCK,			handle_syn_clock},
	{D2M_CMD_RAW_DATA_EX,		handle_request_raw_data},
	{D2M_CMD_REFINE_DATA,		handle_request_refine_data},
	//{D2M_CMD_RAW_DATA,			handle_raw_data},
	{D2M_CMD_REBOOT,			handle_reboot},
	{D2M_CMD_REMOTE_UPDATE,		handle_remote_update},
	{D2M_CMD_SYN_STATE, 		handle_syn_state},
	{D2M_CMD_DEBUG_BUILD,		handle_debug_build},
	{D2M_CMD_DEBUG_MD5,			handle_debug_md5},
//	{D2M_CMD_DEBUG_EXE,			handle_debug_exe},
//	{D2M_CMD_DEBUG_CFG_INI,		handle_debug_cfg_ini},
//	{D2M_CMD_DEBUG_CFG_POS,		handle_debug_cfg_pos},
	{D2M_CMD_DEBUG_AMP, 		handle_debug_amp},
//	{D2M_CMD_DEBUG_FPGA, 		handle_debug_fpga},
//	{D2M_CMD_DEBUG_POWERFREQ,	handle_debug_powerfreq},
//	{D2M_CMD_DEBUG_PTP,			handle_debug_ptp},
//	{D2M_CMD_DEBUG_PTP_PING,	handle_debug_ptp_ping},
	{D2M_CMD_SOFT_VERSION,		handle_soft_version},
/*	{D2M_CMD_DEBUG_SAVE_RAW,	handle_debug_save_raw},
	{D2M_CMD_DEBUG_SW_INFO, 	handle_debug_sw_info},
	{D2M_CMD_REQUEST_AMP,		handle_set_amp},
	{D2M_CMD_REQUEST_SAMPLE_STATUS,	handle_req_sample_status},
	{D2M_CMD_REFINED,			handle_set_refine},
	{D2M_CMD_CIRCLE_SAMPLE,		handle_start_circle_sample},
	{D2M_CMD_STOP_SAMPLE,		handle_stop_circle_sample},
	{D2M_CMD_SYN_S_PARA,		handle_syn_para_s},
	{D2M_CMD_SET_PC2MV_SCALE,	handle_set_pc2mv_sacle},
	{D2M_CMD_GET_PC2MV_SCALE,	handle_get_pc2mv_sacle},
	{D2M_CMD_INVALID, 			handle_invalid_cmd}*/
};


static int handle_apd_msg(char *buf, int len, int cmd, int connectfd)
{
	if(buf == NULL || len <= 0)
	{
		printf("data err\n");
		return -1;
	}
	int i = 0;
	for(i = 0; i< sizeof(d2m_handler)/sizeof(apd_msg_handle_t); i++)
	{
		if((char)cmd == (char)d2m_handler[i].cmd)
		{
			d2m_handler[i].msg_func(buf, len, connectfd);
			return 1;
		}
	}
	return 0;
}

#define TZ_DEL_TIME			(0)

struct tm *apd_convert_localtime(const time_t *timep,struct tm *result)
{
	time_t cur;
	
	if(timep == NULL || result == NULL)
	{
		return NULL;
	}

	cur = *timep + TZ_DEL_TIME;
	return localtime_r(&cur,result);
}

int apd_create_random_num()
{	
	rd_distribution(g_phase_rd, N, 1, 30*60);
	rd_distribution(g_Q_rd, N, 1, 200);
	return 0;
}

void *apd_vr_thread(void *arg)
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
	printf("apd,create success, port:%d\n", port);
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
			apd_convert_localtime(&timep, &tmcur);
			printf("time:[%04d-%02d-%02d %02d:%02d:%02d],cmd:0x%02x\n", tmcur.tm_year + 1900,tmcur.tm_mon + 1, tmcur.tm_mday,
				tmcur.tm_hour, tmcur.tm_min,tmcur.tm_sec, (char)buf[5]);
			handle_apd_msg(buf, len, buf[5], connectfd);
		}
		shutdown(connectfd, SHUT_RDWR);
		close(connectfd);
		sleep(1);
	}
}
