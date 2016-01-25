/********************************************************************************
 **
 **
 **		用于处理与上位机交互的指令信息
 **
 **
 **
 *******************************************************************************/
#ifndef __COMMANDDEAL_H
#define __COMMANDDEAL_H

#include "DB_Segment.h"

#if 0
#define ELECTR_METER	33
#define HEAT_METER		32
#define WATER_METER		16
#endif
#define ELECTR_METER	    0x21
#define HEAT_METER		    0x20
#define WATER_METER		0x10

enum readMeterType {
	PeriodReadMeter = 0, RealtimeReadMeter
};

typedef struct {
	UINT8 Meter_addr[7];
	UINT8 Meter_type;
	UINT8 Protocol_type;
	UINT8 Comm_port;
	UINT8 Subitem;
	UINT8 Building_id;
	UINT8 ReadMeter_type;
	UINT8 Meter_id[26];
} MeterFileType;

extern char gGPRS_Send_Succ_Flag;
extern System_ConfigType gSystem_Config;
extern INT32 g_uiMbusFd;
extern INT32 g_uiCom485Fd;
extern INT32 CMD_Sys_Config_SQlite(void);
extern INT32 CMD_ReviceXMLData(UINT8 Xml_TypeDefine);
extern INT32 CommandDeal_main_proc(void);
extern UINT32 Read_Meter(void *meterinfo, MeterFileType *mf);

#endif

