#ifndef _VR_APD_H
#define _VR_APD_H
#include "cfg.h"

#define D2M_CMD_SYN_PARA				0x01
#define D2M_CMD_START_SAMPLE			0x02
#define D2M_CMD_COLLECT_DATA			0x03
#define D2M_CMD_BEEP					0x04
#define D2M_CMD_SYN_CLOCK				0x05
#define D2M_CMD_RAW_DATA_EX				0x06
#define D2M_CMD_REFINE_DATA				0x11			// �����������(��һ��)
#define D2M_CMD_RAW_DATA				0x12
#define D2M_CMD_REBOOT					0x07
#define D2M_CMD_REMOTE_UPDATE			0x08
#define D2M_CMD_SYN_STATE				0x09
#define	D2M_CMD_REQUEST_PHARSE			0x13			// �����ʼ��λ 
#define D2M_CMD_REQUEST_AMP				0x14			// �������� 
#define D2M_CMD_REQUEST_SAMPLE_STATUS	0x15			// �������״̬ 
#define D2M_CMD_REFINED					0x16			// ���ó������� 
#define D2M_CMD_CIRCLE_SAMPLE 			0x19			// ѭ������ 
#define D2M_CMD_STOP_SAMPLE   			0x20			// ֹͣѭ���ɼ� 
#define D2M_CMD_SYN_S_PARA				0x21			// ͬ���Ӷ˲���	
#define D2M_CMD_DEBUG_BUILD				0xd0			// ����apd_build.log������
#define D2M_CMD_DEBUG_MD5				0xd1			// ����apd_md5sum.log������
#define D2M_CMD_DEBUG_EXE				0xd2			// ����Ӧ�ó�����̺Ͷ�̬���ļ��Ĵ�С
#define D2M_CMD_DEBUG_CFG_INI			0xd3			// ����config.ini�е�����
#define D2M_CMD_DEBUG_CFG_POS			0xd4			// ����pos.conf������
#define D2M_CMD_DEBUG_AMP				0xd5			// ����ǰ��������������
#define D2M_CMD_DEBUG_FPGA				0xd6			// ����fpga��hard_id�����ںͰ汾��
#define D2M_CMD_DEBUG_POWERFREQ 		0xd7			// ����ǰ�Ĺ�Ƶ״̬�͹�Ƶ��λ
#define D2M_CMD_DEBUG_PTP				0xd8			// ����ǰ��PTP״̬
#define D2M_CMD_DEBUG_SAVE_RAW			0xd9			// �ݴ�ԭʼ����
#define D2M_CMD_DEBUG_PTP_PING			0xda			// ping�Ӷ�eth1��, �����PTP����
#define D2M_CMD_UPDATE_APP				0xdb			// ����Ӧ�ó���
#define D2M_CMD_SOFT_VERSION    		0xdc			// ��ȡ�汾��Ϣ
#define D2M_CMD_DEBUG_SW_INFO			0xdd			// ��������汾��ص���Ϣ
#define D2M_CMD_SET_PC2MV_SCALE			0xde      	   	// �����ŵ���
#define D2M_CMD_GET_PC2MV_SCALE			0xdf         	// ��ȡ�ŵ�������ϵ��
#define D2M_CMD_INVALID					0xfe

#define BACKLOG 10/*���ٵȴ����ӿ���*/

#define MSG_START 0xAA
#define MSG_END 0xAA
#define ESCAPE_BB 0xBB
#define ESCAPE_0xAA 0xAA
#define ESCAPE_A 0x0A
#define ESCAPE_B 0x0B

#pragma pack(1)
typedef struct fault_tag{
	unsigned char type;
	unsigned char status;			//0ΪOK��1Ϊ����
	unsigned char cause;			
}fault_t;
#pragma pack()

typedef struct apd_msg_handle_tag{
	int cmd;
	int (*msg_func)(char *buf,int ulen,int connectfd);
}apd_msg_handle_t;
int apd_create_random_num();
void *apd_vr_thread(void *arg);

#endif
