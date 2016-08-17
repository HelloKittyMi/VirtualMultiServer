#ifndef _VR_APD_H
#define _VR_APD_H
#include "cfg.h"

#define D2M_CMD_SYN_PARA				0x01
#define D2M_CMD_START_SAMPLE			0x02
#define D2M_CMD_COLLECT_DATA			0x03
#define D2M_CMD_BEEP					0x04
#define D2M_CMD_SYN_CLOCK				0x05
#define D2M_CMD_RAW_DATA_EX				0x06
#define D2M_CMD_REFINE_DATA				0x11			// 请求抽样数据(归一化)
#define D2M_CMD_RAW_DATA				0x12
#define D2M_CMD_REBOOT					0x07
#define D2M_CMD_REMOTE_UPDATE			0x08
#define D2M_CMD_SYN_STATE				0x09
#define	D2M_CMD_REQUEST_PHARSE			0x13			// 请求初始相位 
#define D2M_CMD_REQUEST_AMP				0x14			// 请求增益 
#define D2M_CMD_REQUEST_SAMPLE_STATUS	0x15			// 请求采样状态 
#define D2M_CMD_REFINED					0x16			// 设置抽样参数 
#define D2M_CMD_CIRCLE_SAMPLE 			0x19			// 循环采样 
#define D2M_CMD_STOP_SAMPLE   			0x20			// 停止循环采集 
#define D2M_CMD_SYN_S_PARA				0x21			// 同步从端参数	
#define D2M_CMD_DEBUG_BUILD				0xd0			// 请求apd_build.log的内容
#define D2M_CMD_DEBUG_MD5				0xd1			// 请求apd_md5sum.log的内容
#define D2M_CMD_DEBUG_EXE				0xd2			// 请求应用程序进程和动态库文件的大小
#define D2M_CMD_DEBUG_CFG_INI			0xd3			// 请求config.ini中的配置
#define D2M_CMD_DEBUG_CFG_POS			0xd4			// 请求pos.conf的配置
#define D2M_CMD_DEBUG_AMP				0xd5			// 请求当前的三相主从增益
#define D2M_CMD_DEBUG_FPGA				0xd6			// 请求fpga的hard_id，日期和版本号
#define D2M_CMD_DEBUG_POWERFREQ 		0xd7			// 请求当前的工频状态和工频相位
#define D2M_CMD_DEBUG_PTP				0xd8			// 请求当前的PTP状态
#define D2M_CMD_DEBUG_SAVE_RAW			0xd9			// 暂存原始数据
#define D2M_CMD_DEBUG_PTP_PING			0xda			// ping从端eth1口, 即检查PTP连线
#define D2M_CMD_UPDATE_APP				0xdb			// 更新应用程序
#define D2M_CMD_SOFT_VERSION    		0xdc			// 获取版本信息
#define D2M_CMD_DEBUG_SW_INFO			0xdd			// 请求软件版本相关的信息
#define D2M_CMD_SET_PC2MV_SCALE			0xde      	   	// 纠正放电量
#define D2M_CMD_GET_PC2MV_SCALE			0xdf         	// 获取放电量纠正系数
#define D2M_CMD_INVALID					0xfe

#define BACKLOG 10/*多少等待连接控制*/

#define MSG_START 0xAA
#define MSG_END 0xAA
#define ESCAPE_BB 0xBB
#define ESCAPE_0xAA 0xAA
#define ESCAPE_A 0x0A
#define ESCAPE_B 0x0B

#pragma pack(1)
typedef struct fault_tag{
	unsigned char type;
	unsigned char status;			//0为OK，1为故障
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
