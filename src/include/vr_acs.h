#ifndef _VR_ACS_H
#define _VR_ACS_H
#include "cfg.h"
#include <stdint.h>
#define NET_ACS_EVENT_IDENTITY		(0)			/*事件标识值*/
#define NET_ACS_PROTOCOL_IDENTITY	(0)			/*协议标识值*/
#define NET_ACS_SEND_SLAVE_ADDR		(2)			/*发送时从机地址*/
#define NET_ACS_RECV_SLAVE_ADDR		(1)			/*接收时从机地址*/
#define	NET_ACS_START_ADDR			(1240)		/*起始寄存器地址*/

#define BACKLOG 10/*多少等待连接控制*/


#define C2M_CODE_GET_DATA	0x03
#define C2M_CODE_REBOOT		0x10

#define READ_REGISTER_LEN			(37)		/*读寄存器时数据长度*/
#define REBOOT_DEV_LEN				(6)			/*复位设备时数据长度*/

#define READ_REGISTER_START_ADDR	(3000)		/*读数据寄存器起始地址*/
#define READ_REGISTER_NUM			(16)		/*读数据寄存器数量*/
#define REBOOT_REGISTER_START_ADDR	(1240)		/*复位寄存器起始地址*/
#define REBOOT_REGISTER_NUM			(0)			/*复位寄存器数量*/

#define VERSION 100

#pragma pack(1)
/*acsm请求头结构*/
typedef struct net_cs_head_tag
{
	uint16_t	event_identity;				/*事务处理标识*/
	uint16_t	protocol_identity;			/*协议标识*/
	uint16_t	len;						/*字节长度，后面所有字节长度，不包括该字段长度*/
	uint8_t		slave_addr;					/*单元标识，即从设备地址*/
	uint8_t		cmd;						/*功能码*/
}net_cs_head_t;
#pragma pack()

typedef struct acs_msg_handle_tag{
	int cmd;
	int (*msg_func)(char *buf,int ulen,int connectfd);
}acs_msg_handle_t;

int acs_create_random_num();
void *acs_vr_thread(void *arg);
#endif


