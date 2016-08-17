#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#include "vr_acs.h"
#include "vr_apd.h"


#include "thread.h"

/* 命令传入参数，argv[1]--采集器个数， argv[2]--起始端口值 */
int main(int argc, char *argv[])
{
	int apd_num = 0;
	int server_port = 0;
	int i = 0;
	int *port_para =NULL;
	if(argc < 2)
	{
		printf("vrapd:no input parameters\n");
	}
	if(argc != 3)
	{
		printf("the number of parameters is err\n");
	}
	if(argc == 3)
	{
		if((apd_num = atoi(argv[1])) == 0)
		{
			printf("the second argument is err\n");
		}
		if((server_port = atoi(argv[2])) == 0)
		{
			printf("the third argument is err\n");
		}
	}
	port_para = calloc(apd_num, sizeof(server_port));
	/*预设随机种子*/
#if SYS_DEVICE_TYPE == DEV_TYPE_ACS
	acs_create_random_num();
	for(i = 0; i< apd_num; i++)
	{
		*(port_para + i) = server_port + i;
 		thread(acs_vr_thread, port_para+i);
	}
#elif SYS_DEVICE_TYPE == DEV_TYPE_APD
	apd_create_random_num();
	for(i = 0; i< apd_num; i++)
	{
		*(port_para + i) = server_port + i;
 		thread(apd_vr_thread, port_para+i);
	}
#endif

	while(1)
	{
		sleep(1);
	}
}
