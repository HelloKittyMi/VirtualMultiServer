#ifndef _VR_ACS_H
#define _VR_ACS_H
#include "cfg.h"
#include <stdint.h>
#define NET_ACS_EVENT_IDENTITY		(0)			/*�¼���ʶֵ*/
#define NET_ACS_PROTOCOL_IDENTITY	(0)			/*Э���ʶֵ*/
#define NET_ACS_SEND_SLAVE_ADDR		(2)			/*����ʱ�ӻ���ַ*/
#define NET_ACS_RECV_SLAVE_ADDR		(1)			/*����ʱ�ӻ���ַ*/
#define	NET_ACS_START_ADDR			(1240)		/*��ʼ�Ĵ�����ַ*/

#define BACKLOG 10/*���ٵȴ����ӿ���*/


#define C2M_CODE_GET_DATA	0x03
#define C2M_CODE_REBOOT		0x10

#define READ_REGISTER_LEN			(37)		/*���Ĵ���ʱ���ݳ���*/
#define REBOOT_DEV_LEN				(6)			/*��λ�豸ʱ���ݳ���*/

#define READ_REGISTER_START_ADDR	(3000)		/*�����ݼĴ�����ʼ��ַ*/
#define READ_REGISTER_NUM			(16)		/*�����ݼĴ�������*/
#define REBOOT_REGISTER_START_ADDR	(1240)		/*��λ�Ĵ�����ʼ��ַ*/
#define REBOOT_REGISTER_NUM			(0)			/*��λ�Ĵ�������*/

#define VERSION 100

#pragma pack(1)
/*acsm����ͷ�ṹ*/
typedef struct net_cs_head_tag
{
	uint16_t	event_identity;				/*�������ʶ*/
	uint16_t	protocol_identity;			/*Э���ʶ*/
	uint16_t	len;						/*�ֽڳ��ȣ����������ֽڳ��ȣ����������ֶγ���*/
	uint8_t		slave_addr;					/*��Ԫ��ʶ�������豸��ַ*/
	uint8_t		cmd;						/*������*/
}net_cs_head_t;
#pragma pack()

typedef struct acs_msg_handle_tag{
	int cmd;
	int (*msg_func)(char *buf,int ulen,int connectfd);
}acs_msg_handle_t;

int acs_create_random_num();
void *acs_vr_thread(void *arg);
#endif


