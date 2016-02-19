#include "Centre_Controller.h"
#include "DELU_Heat_Meter.h"
#include "WaterMeter.h"
#include "Elect97.h"
#include "includes.h"
#include "time.h"
#include "CommandDeal.h"
#include "DB_Segment.h"

extern System_ConfigType gSystem_Config;

/* 全局的各个通道的文件描述符 */

INT32 g_uiGprsFd = 0;
//IO控制管教
volatile INT32 g_uiIoControl;
//数据库句柄
sqlite3 *sqlitedb;

struct {
	INT32 mbusFd;
	INT32 m485_1_Fd;
	INT32 m485_2_Fd;
} gDeviceFd = { 0 };

struct {
	UINT8 meterType;
	UINT8 functionId[3];
	UINT8 location;
} gParaType[100] = { { 0x10, { 0x90, 0x10 }, gSEG_Subitem_WaterMeter }, { 0x20,
		{ 0x90, 0x11 }, gSEG_Subitem_HeatMeter }, { 0x21, { 0x90, 0x10 },
		gSEG_Total_active_power_ElectricMeter } };

COM_INFO_T g_GprsDeviceInfo;

char gReadingDate[20] = "";

/* 初始化抄表流程	1:抄485总线, 2 :抄MBUS	总线*/
volatile INT32 g_uiReadMeterStatus = STATU_IDEL;
volatile INT32 g_uiDataDealStatus = STATU_IDEL;

//static uint8 testHeartFrm_xmz[]={"您好!这里是CZ513-6型集中器自检程序!GPRS数据通道已经成功建立!呵呵.请试着向集中器发送一些文字或数据.看看自检程序界面上是否收到.---您好!这里是晨泰集中器自检程序!GPRS数据通道已经成功建立!呵呵.请试着向集中器发送一些文字或数据.看看自检程序界面上是否收到.---您好!这里是晨泰集中器自检程序!GPRS数据通道已经成功建立!呵呵.请试着向集中器发送一些文字或数据.看看自检程序界面上是否收到.---您好!这里是晨泰集中器自检程序!GPRS数据通道已经成功建立!呵呵.请试着向集中器发送一些文字或数据.看看自检程序界面上是否收到."};

extern void GPRS_Rec_proc(void *pdata);
extern void GPRS_Mana_main_proc(void *pdata);
extern void GPRS_Upsend_main_proc(void *pdata);
extern int32 CMD_TransFile(void);
extern int32 COM_Para_Set(uint32 fd, int32 baud, int32 parity);

void Mbus_Channel(uint32 channel) {
	uint8 i = 0;
	for (i = 0; i < 6; i++) {
		ioctl(g_uiIoControl, MBUS_C0 + i, 1);
	}
	OSTimeDly(OS_TICKS_PER_SEC / 4);
	ioctl(g_uiIoControl, MBUS_C0 + channel - 1, 1);
	OSTimeDly(OS_TICKS_PER_SEC / 2);
}
/*根据通道号和设备类型去读表*/
UINT32 Read_Meter(void *meterinfo, MeterFileType *mf) {
	struct mdev_meterinfo *pMeterinfo = (struct mdev_meterinfo *) meterinfo;

	if (pMeterinfo->channel > 0 && pMeterinfo->channel < 7) {
		debug_info(gDebugModule[SEM],
				"[%s][%s][%d] wait mbus_Sem,channel = %d\n", FILE_LINE,
				pMeterinfo->channel);
		sem_wait(&mbus_Sem);
		debug_info(gDebugModule[SEM], "[%s][%s][%d] mbus_Sem is alive\n",
				FILE_LINE);

		if (RealtimeReadMeter == mf->ReadMeter_type) {
			pMeterinfo->fd = gDeviceFd.mbusFd;
		}
		Mbus_Channel(pMeterinfo->channel);
	} else if (pMeterinfo->channel == 7) {
		sem_wait(&m485_Sem);
		if (RealtimeReadMeter == mf->ReadMeter_type) {
			pMeterinfo->fd = gDeviceFd.m485_2_Fd;
		}
	} else {
		debug_info(gDebugModule[TIMING_METER],
				"[%s][%s][%d]  channel = %d is error!\n", FILE_LINE,
				pMeterinfo->channel);
		return -1;
	}
	switch (mf->Meter_type)			//此处也可以调用不同的回调函数或函数指针
	{
	case ELECTR_METER:
		COM_Para_Set(gDeviceFd.m485_2_Fd, 1200, PAR_EVEN);
		debug_info(gDebugModule[TIMING_METER],
				"[%s][%s][%d]  Start Send Data to ElectrMeter By RS485!\n",
				FILE_LINE)
		;
		elect97_meter_com(pMeterinfo->fd, mf);
		break;
	case HEAT_METER:
		COM_Para_Set(gDeviceFd.mbusFd, 2400, PAR_EVEN);
		debug_info(gDebugModule[TIMING_METER],
				"[%s][%s][%d]  This is HeatMeter Send Data!\n", FILE_LINE)
		;
		Heat_meter_com(pMeterinfo->fd, mf);
		break;
	case WATER_METER:
		COM_Para_Set(gDeviceFd.mbusFd, 4800, PAR_NONE);
		debug_info(gDebugModule[TIMING_METER],
				"[%s][%s][%d]  This is WaterMeter Send Data!\n", FILE_LINE)
		;
		Water_meter_com(pMeterinfo->fd, mf);
		break;
	default:
		debug_info(gDebugModule[TIMING_METER],
				"[%s][%s][%d] Send Command MeterType is  ERROR!\n", FILE_LINE)
		;
		break;
	}
	if (pMeterinfo->channel > 0 && pMeterinfo->channel < 7) {
		sem_post(&mbus_Sem);
	} else if (pMeterinfo->channel == 7 || pMeterinfo->channel == 8) {
		sem_post(&m485_Sem);
	}
	return 0;
}

/* 通过Mbus线性抄表流程 */
/*yangfei modified 20130831 *Fd == device fd*/
static int Read_Meter_Timer(void *meterinfo, int argc, char **argv,
		char **azColName) {
	UINT16 j = 0x00;
	MeterFileType mf;

	for (j = 0; j < argc; j++) {
		debug_info(gDebugModule[METER], "%s", argv[j]);
		debug_info(gDebugModule[METER], "    %s\n", azColName[j]);
	}
	memset(&mf, 0x00, sizeof(mf));
	//以下字符串转换函数有错误，需纠正
	//printf("\n mf.Meter_addr is ----");

	//StrToBin(argv[gSEG_Meter_addr_MeterInfo], mf.Meter_addr);/*yangfei modified 20140317*/

	//printf("\n mf.Comm_port is ---- ");
	StrToBin(argv[gSEG_Comm_port_MeterInfo], &mf.Comm_port);

	//printf("\n mf.Meter_type is ----");
	StrToBin(argv[gSEG_Meter_type_MeterInfo], &mf.Meter_type);

	StrToBin_MeterAddress(argv[gSEG_Meter_addr_MeterInfo], mf.Meter_addr,
			mf.Meter_type);/*yangfei modified 20140317*/

	//printf("\n mf.Protocol_type is ---- ");
	StrToBin(argv[gSEG_Protocol_type_MeterInfo], &mf.Protocol_type);

	//printf("\n mf.Subitem is ----");
	StrToBin(argv[gSEG_Subitem_MeterInfo], &mf.Subitem);

	strcpy((char *) mf.Meter_id, argv[gSEG_Meter_id_MeterInfo]);

	debug_info(gDebugModule[METER], "The Comm_port is 0x%x\n", mf.Comm_port);
	debug_info(gDebugModule[METER], "The Meter_type is 0x%x\n", mf.Meter_type);
	debug_info(gDebugModule[METER], "The Protocol_type is 0x%x\n",
			mf.Protocol_type);
	debug_info(gDebugModule[METER], "The Subitem is 0x%4x\n", mf.Subitem);
	debug_info(gDebugModule[METER], "\n mf.Meter_type is ---- %d \n",
			mf.Meter_type);

	mf.ReadMeter_type = PeriodReadMeter;
	Read_Meter(meterinfo, &mf);

	return 0;
}

int32 COM_Para_Set(uint32 fd, int32 baud, int32 parity) {
	struct termios ComAttr;
	/* 配置Mbus接口信息*/
	COM_INFO_T ComDeviceInfo;
	int speed_arr[] = { B1200, B2400, B4800, B9600 };
	int name_arr[] = { 1200, 2400, 4800, 9600 };
	int i;
	for (i = 0; i < sizeof(name_arr) / sizeof(int); i++) {
		if (baud == name_arr[i]) {
			ComDeviceInfo.Speed = speed_arr[i];
			//printf("baud = %d\n",baud);
			break;
		}
	}
	if (i >= sizeof(name_arr) / sizeof(int)) {
		printf("bad baud = %d\n", baud);
		return 1;
	}

	//ComDeviceInfo.Speed = B2400;/*热表2400NB*/
	ComDeviceInfo.ByteBits = CS8;
	memcpy(ComDeviceInfo.Name, DEVICEMBUS, strlen(DEVICEMBUS));
	if (fcntl(fd, F_SETFL, O_NONBLOCK) < 0) {
		printf("Mbus fcntl error ....[%s][%s][%d] \n", FILE_LINE);
		return (CONTROL_RET_FAIL);
	}
	memset(&ComAttr, 0, sizeof(struct termios));
	ComAttr.c_iflag = IGNPAR;
	if (parity == PAR_NONE) {
		ComAttr.c_cflag = ComDeviceInfo.Speed | HUPCL | ComDeviceInfo.ByteBits
				| CREAD | CLOCAL;
	} else if (parity == PAR_EVEN) {
		ComAttr.c_cflag = ComDeviceInfo.Speed | HUPCL | ComDeviceInfo.ByteBits
				| PARENB | CREAD | CLOCAL;
	}

	ComAttr.c_cc[VMIN] = 1;

	if (tcsetattr(fd, TCSANOW, &ComAttr) < 0) {
		printf("Mbus fcntl error ....[%s][%s][%d] \n", FILE_LINE);
		return (CONTROL_RET_FAIL);
	}
	return 0;
}

/* 通过Mbus抄表的处理主线程函数 */
INT32 mbus_main_proc() {
	uint8 i = 0;
	char SQ_Str[100] = "";
	char channleStr[10] = "";

	struct mdev_meterinfo meterinfo = { 0 };

	INT32 rec = 0;
	char *zErrMsg = 0;
	INT32 MbusFd = 0;
	MbusFd = open(DEVICEMBUS, O_RDWR, 0);
	if (MbusFd < 0) {
		if (g_uiReadMeterStatus == STATU_READ_MBUS) {
			g_uiReadMeterStatus = STATU_IDEL;
		}
		printf("Mbus open error ....[%s][%s][%d] \n", FILE_LINE);
		return (CONTROL_RET_FAIL);
	} else {
		gDeviceFd.mbusFd = MbusFd;
	}

	while (1) {
		if (g_uiReadMeterStatus == STATU_READ_MBUS) {
			for (i = 1; i < 7; i++) {
				debug_notice(gDebugModule[TIMING_METER],
						"The MBUS channle is %d \n", i);
				sprintf(channleStr, "%d", i);

				COM_Para_Set(MbusFd, 2400, PAR_EVEN);

				strcpy(SQ_Str,
						"select * from Meter_Info_Table where Comm_port == ");
				strcat(SQ_Str, "'");
				strcat(SQ_Str, channleStr);
				strcat(SQ_Str, "' and Meter_type == '20'");

				debug_info(gDebugModule[METER],
						"This is Mbus_Proc  for HeatMeter!\n");
				meterinfo.channel = i;
				meterinfo.fd = MbusFd;
				rec = sqlite3_exec(sqlitedb, SQ_Str, Read_Meter_Timer,
						(void *) &meterinfo, &zErrMsg);
				if (rec != SQLITE_OK) {
					debug_err(gDebugModule[METER], "[%s][%s][%d] \n", FILE_LINE);
					fprintf(stderr, "SQL error: %s\n", zErrMsg);
					sqlite3_free(zErrMsg);
				}

				OSTimeDly(OS_TICKS_PER_SEC);

				COM_Para_Set(MbusFd, 4800, PAR_NONE);

				strcpy(SQ_Str,
						"select * from Meter_Info_Table where Comm_port == ");
				strcat(SQ_Str, "'");
				strcat(SQ_Str, channleStr);
				strcat(SQ_Str, "' and Meter_type == '10'");
				OSTimeDly(OS_TICKS_PER_SEC);

				debug_info(gDebugModule[METER],
						"This is Mbus_Proc  for WaterMeter!\n");
				rec = sqlite3_exec(sqlitedb, SQ_Str, Read_Meter_Timer,
						(void *) &meterinfo, &zErrMsg);
				if (rec != SQLITE_OK) {
					debug_err(gDebugModule[METER], "[%s][%s][%d] \n", FILE_LINE);
					fprintf(stderr, "SQL error: %s\n", zErrMsg);
					sqlite3_free(zErrMsg);
				}
			}
			g_uiReadMeterStatus = STATU_IDEL;
			/*begin:yangfei added 20130902 for 抄表结束，通知上报处理*/
			g_uiDataDealStatus |= STATU_REPORTUP;
			sem_post(&Xml_Deal_Sem);
			/*end:yangfei added 20130902 for 抄表结束，通知上报处理*/
		}
		sleep(2);
	}
	return (CONTROL_RET_SUC);
}

/*****************************************************************************************
 通过485抄表的处理主线程函数
 参数一:通过
 ******************************************************************************************/
INT32 com485_main_proc() {
	INT32 rec = 0;
	char *zErrMsg = 0;
	INT32 Com485_1_Fd = 0;
	INT32 Com485_2_Fd = 0;
	struct mdev_meterinfo meterinfo = { 0 };
	/*1st 485*/
	Com485_1_Fd = open(DEVICE4851, O_RDWR, 0);
	if (Com485_1_Fd < 0) {
		printf("Com485 open error ....[%s][%s][%d] \n", FILE_LINE);
		return (CONTROL_RET_FAIL);
	} else {
		gDeviceFd.m485_1_Fd = Com485_1_Fd;
	}
	/*2st 485*/
	Com485_2_Fd = open(DEVICE4852, O_RDWR, 0);
	if (Com485_2_Fd < 0) {
		printf("Com485 open error ....[%s][%s][%d] \n", FILE_LINE);
		return (CONTROL_RET_FAIL);
	} else {
		gDeviceFd.m485_2_Fd = Com485_2_Fd;
	}

	printf("This is RS485 proc!\n");
	while (1) {
		if (g_uiReadMeterStatus == STATU_READ_RS485) {
			printf("This is RS485 proc!\n");

#if 0
			//表类型确定走的通道2==rs485    1=mbus
			rec = sqlite3_get_table(sqlitedb,"select * from Meter_Info_Table where Comm_port == '2'", &result, &nrow, &ncolumn, &zErrMsg);
			printf("get table ********** Msg Sum %d row\n", nrow);

			for(i=ncolumn; i<ncolumn*2; i++)
			{
				printf("%s\n", result[i]);
			}
#endif
			COM_Para_Set(Com485_2_Fd, 1200, PAR_EVEN);
			meterinfo.channel = 7;
			meterinfo.fd = Com485_2_Fd;
			//rec = sqlite3_exec(sqlitedb, "select * from Meter_Info_Table where Comm_port == '7' and Meter_type == '21'", Read_Meter_Timer, (void *)&meterinfo, &zErrMsg);
			rec = sqlite3_exec(sqlitedb,
					"select * from Meter_Info_Table where Comm_port == '7'",
					Read_Meter_Timer, (void *) &meterinfo, &zErrMsg);

			if (rec != SQLITE_OK) {
				printf("[%s][%s][%d] \n", FILE_LINE);
				fprintf(stderr, "SQL error: %s/n", zErrMsg);
				sqlite3_free(zErrMsg);
			}

			g_uiReadMeterStatus = STATU_READ_MBUS;
		}
		sleep(2);
	}
	return (CONTROL_RET_SUC);
}

void updateDatabase(const int year, const int month, const int day) {
	int rec = 0;
	char strSql[256] = { 0 };
	char *zErrMsg = NULL;
	sprintf((char *) strSql,
			"delete from Water_Meter_Table where Read_time < '%04d%02d%02d000000'",
			year, month, day);
	rec = sqlite3_exec(sqlitedb, strSql, NULL, NULL, &zErrMsg);
	if (rec != SQLITE_OK) {
		debug_err(gDebugModule[METER], "[%s][%s][%d] \n", FILE_LINE);
		fprintf(stderr, "execute %s error: %s\n", strSql, zErrMsg);
		sqlite3_free(zErrMsg);
	} else {
		printf("execute %s ok \n", strSql);
	}
	sprintf((char *) strSql,
			"delete from Heat_Meter_Table where Read_time < '%04d%02d%02d000000'",
			year, month, day);
	rec = sqlite3_exec(sqlitedb, strSql, NULL, NULL, &zErrMsg);
	if (rec != SQLITE_OK) {
		debug_err(gDebugModule[METER], "[%s][%s][%d] \n", FILE_LINE);
		fprintf(stderr, "execute %s error: %s\n", strSql, zErrMsg);
		sqlite3_free(zErrMsg);
	} else {
		printf("execute %s ok \n", strSql);
	}

	sprintf((char *) strSql,
			"delete from Electric_Meter_Table where Read_time < '%04d%02d%02d000000'",
			year, month, day);
	rec = sqlite3_exec(sqlitedb, strSql, NULL, NULL, &zErrMsg);
	if (rec != SQLITE_OK) {
		debug_err(gDebugModule[METER], "[%s][%s][%d] \n", FILE_LINE);
		fprintf(stderr, "execute %s error: %s\n", strSql, zErrMsg);
		sqlite3_free(zErrMsg);
	} else {
		printf("execute %s ok \n", strSql);
	}
	return;
}

/* 定时器*/
void ReadMeter_timer_proc() {
	UINT8 i = 0, ret = 0;
	char lDatetemp[20] = "";
	char lYeartemp[10] = "";
	UINT8 lTimetemp[10] = { 0x00 };
	char SqCmd_Str[256] = "";
	char *zErrMsg = 0;

	time_t timep;
	struct tm *p;

	time(&timep);
	debug_debug(gDebugModule[TIMING_METER], "[%s][%s][%d]  %s\n", FILE_LINE,
			ctime(&timep));
	//strcpy(lReadDate, ctime(&timep));
	//p = gmtime(&timep);/*UTC时间  yangfei modified*/
	p = localtime(&timep);/*当地时间*/

	debug_debug(gDebugModule[TIMING_METER],
			"[%s][%s][%d] One minute function!\n", FILE_LINE);
	debug_debug(gDebugModule[TIMING_METER], "g_uiReadMeterStatus is %d\n",
			g_uiReadMeterStatus);

	if ((p->tm_min % INIT_SAMPLE_PERIOD) == 0) /*采集时间到*/
	{
		sprintf(lYeartemp, "%d", 1900 + p->tm_year);

		lTimetemp[4] = p->tm_sec;
		lTimetemp[3] = p->tm_min;
#if 0 /*yf modified 20130625*/
		lTimetemp[2] = 8+p->tm_hour;
#else
		lTimetemp[2] = p->tm_hour;
#endif
		lTimetemp[1] = p->tm_mday;
		lTimetemp[0] = 1 + p->tm_mon;

		for (i = 0; i < 5; i++) {
			lTimetemp[i] = HexToBcd(lTimetemp[i]);
		}

		PUBLIC_BCDToString(lDatetemp, lTimetemp, 5);

		memset(gReadingDate, 0x00, sizeof(gReadingDate));
		strcat(gReadingDate, lYeartemp);
		strcat(gReadingDate, lDatetemp);

		updateDatabase(1900 + p->tm_year - 1, 1 + p->tm_mon, p->tm_mday);
		printf("ReadMeter time is %s\n", gReadingDate);

#if 1
		//judge  meter is reading
		//start read meter;
		//正常情况下，2次抄表的间隔，必须保证表计轮询抄读完成
		//if(g_uiReadMeterStatus == STATU_IDEL)
		g_uiReadMeterStatus = STATU_READ_RS485;
		//g_uiReadMeterStatus = STATU_IDEL;

		ret =
				sqlite3_exec(sqlitedb,
						"delete from ReadTime_Queue_Table where Timenode_type == 'REPORT'",
						NULL, 0, &zErrMsg);
		if (ret != SQLITE_OK) {
			printf("sqlite delete  Error!\n");
			printf("[%s][%s][%d] \n", FILE_LINE);
			fprintf(stderr, "SQL error: %s\n", zErrMsg);
			sqlite3_free(zErrMsg);
		}

		/*
		 strcpy(SqCmd_Str, "update ReadTime_Queue_Table SET Read_time='");
		 strcat(SqCmd_Str, gReadingDate);
		 strcat(SqCmd_Str, "', ");
		 strcat(SqCmd_Str, "where Timenode_type='REPORT'");
		 */
		strcpy(SqCmd_Str, "insert into ReadTime_Queue_Table values('");
		strcat(SqCmd_Str, gReadingDate);
		strcat(SqCmd_Str, "', ");
		strcat(SqCmd_Str, "'REPORT");
		strcat(SqCmd_Str, "')");
		debug_info(gDebugModule[TIMING_METER], "%s", SqCmd_Str);
		ret = sqlite3_exec(sqlitedb, SqCmd_Str, NULL, 0, &zErrMsg);
		if (ret != SQLITE_OK) {
			/*
			 if(ret == 1)
			 {
			 
			 debug_info(gDebugModule[TIMING_METER], "%s", SqCmd_Str);
			 ret = sqlite3_exec(sqlitedb, SqCmd_Str, NULL, 0, &zErrMsg);
			 if( ret!=SQLITE_OK )
			 {
			 printf("[%s][%s][%d] \n",FILE_LINE);
			 fprintf(stderr, "SQL error: %s\n", zErrMsg);
			 }
			 }
			 */
			printf("the return error is %d", ret);
			printf("[%s][%s][%d] \n", FILE_LINE);
			fprintf(stderr, "SQL error: %s\n", zErrMsg);
			sqlite3_free(zErrMsg);
		}
#endif

	}

#if 0    /*yangfei deleted 20130902 for alway is Report_mode*/
	if(0x00 == strcmp(gSystem_Config.Str_Report_mode, "1"))/*1:主动上报*/
#endif 

#if 0
	StrToBin(gSystem_Config.Str_Period, &lReport);
	if((p->tm_min%lReport) == 0)
	{
		if(g_uiReadMeterStatus == STATU_IDEL)
		{
			g_uiDataDealStatus |= STATU_REPORTUP;
			debug_info(gDebugModule[TIMING_METER], "Report xml data!!! and gucReportUpWait = %d\n", gucReportUpWait);
			sem_post(&Xml_Deal_Sem);
		}
		else
		{
			gucReportUpWait = TRUE;
			debug_info(gDebugModule[TIMING_METER], "\nBUS is Busy!!! and gucReportUpWait = %d\n", gucReportUpWait);
		}
	}

	if((gucReportUpWait == TRUE)&&(g_uiReadMeterStatus==STATU_IDEL))
	{
		gucReportUpWait = FALSE;
		//report up
		g_uiDataDealStatus |= STATU_REPORTUP;
		debug_info(gDebugModule[TIMING_METER], "Report xml data  wait readed meter!!! and gucReportUpWait = %d\n", gucReportUpWait);
		sem_post(&Xml_Deal_Sem);
	}
#endif

}
/* 定时器函数，定时时间到了之后就设置工作状态*/
void ReadMeter_timer() {
	struct itimerval tick;
	signal(SIGALRM, ReadMeter_timer_proc);
	memset(&tick, 0, sizeof(tick));
	tick.it_value.tv_sec = 60;
	tick.it_value.tv_usec = 0;
	tick.it_interval.tv_sec = 60;
	tick.it_interval.tv_usec = 0;
	if (setitimer(ITIMER_REAL, &tick, NULL) < 0)
		printf("ReadMeter_timer set itimer is error ~\n");
	while (1)
		sleep(1);
}
/*打开并初始化设备*/
int open_device(INT32 *fd, char*file, COM_INFO_T DeviceInfo) {
	struct termios stComAttr;
	*fd = open(file, O_RDWR, 0);
	if (*fd < 0) {
		printf("Com open error ....[%s][%s][%d] \n", FILE_LINE);
		return (CONTROL_RET_FAIL);
	}
	printf("test[%d][%s] \n", *fd, DeviceInfo.Name);
	if (fcntl(*fd, F_SETFL, O_NONBLOCK) < 0) {
		printf("Com fcntl error ....[%s][%s][%d] \n", FILE_LINE);
		return (CONTROL_RET_FAIL);
	}
	memset(&stComAttr, 0, sizeof(struct termios));
	stComAttr.c_iflag = IGNPAR;
	stComAttr.c_cflag = DeviceInfo.Speed | HUPCL | DeviceInfo.ByteBits | CREAD
			| CLOCAL;
	stComAttr.c_cc[VMIN] = 1;

	if (tcsetattr(*fd, TCSANOW, &stComAttr) < 0) {
		printf("Com tcsetattr error ....[%s][%s][%d] \n", FILE_LINE);
		return (CONTROL_RET_FAIL);
	}
	return (CONTROL_RET_SUC);
}
/*
 GPRS接收数据主进程，
 */
int GPRS_Rec_main_proc() {
	int ret;
	debug_debug(gDebugModule[GPRS_MODULE], "[%s][%s][%d] \n", FILE_LINE);
	ret = open_device(&g_uiGprsFd, DEVICEGPRS, g_GprsDeviceInfo);
	if (ret != 0) {
		debug_err(gDebugModule[GPRS_MODULE], "[%s][%s][%d] ret=%d \n",
				FILE_LINE, ret);
		return (CONTROL_RET_FAIL);
	}

	while (1) {
		server_com_rev(g_uiGprsFd);
		sleep(2);
	}
	return (CONTROL_RET_SUC);
}

static int Init_SysCfg(void *NotUsed, int argc, char **argv, char **azColName) {
	strcpy((char *) gSystem_Config.Str_Building_id, argv[0]);
	strcpy((char *) gSystem_Config.Str_Gateway_id, argv[1]);
	strcpy((char *) gSystem_Config.Str_Report_mode, argv[2]);
	strcpy((char *) gSystem_Config.Str_Period, argv[3]);
	strcpy((char *) gSystem_Config.Str_Md5_key, argv[4]);
	strcpy((char *) gSystem_Config.Str_Aes_key, argv[5]);
	strcpy((char *) gSystem_Config.Str_Ip_addr, argv[6]);
	strcpy((char *) gSystem_Config.Str_Port, argv[7]);
	strcpy((char *) gSystem_Config.Str_Dns, argv[8]);
	strcpy((char *) gSystem_Config.Str_Back_ip_addr, argv[9]);
	strcpy((char *) gSystem_Config.Str_Back_port, argv[10]);
	strcpy((char *) gSystem_Config.Str_Back_dns, argv[11]);

	return 0;
}

INT32 Init_Sqlite(void) {
	INT32 lReg = 0;
	INT32 nrow = 0;
	INT32 ncolumn = 0;

	char *zErrMsg = 0;
	char **DB_result;

	lReg = sqlite3_open(SQLITE_NAME, &sqlitedb);
	if (lReg) {
		fprintf(stderr, "Can't open database: %s/n", sqlite3_errmsg(sqlitedb));
		sqlite3_close(sqlitedb);
	}
#if 0
	lReg = sqlite3_exec(sqlitedb, ".import a.txt Meter_Info_Table;", NULL, 0, &zErrMsg);
	if( lReg!=SQLITE_OK )
	{
		printf("yangfei added %s\n", zErrMsg);
		printf("sqlite Err is %d\n", lReg);
		sqlite3_free(zErrMsg);
	}
#endif
	//判断是否已经创建Meter_Info_Table 表，若未创建过，则新创建
	lReg = sqlite3_exec(sqlitedb, "select meter_id from Meter_Info_Table", NULL,
			0, &zErrMsg);
	if (lReg != SQLITE_OK) {
		printf("%s\n", zErrMsg);
		printf("sqlite Err is %d\n", lReg);
		sqlite3_free(zErrMsg);
		/*begin:yangfei modified 2013-08-28 for add User_id 并设为主键*/
#if  0
		lReg = sqlite3_exec(sqlitedb, "create table Meter_Info_Table('Meter_addr','Meter_type',	'Protocol_type','Comm_port','Subitem','Building_id')",NULL, 0, &zErrMsg);
#else
		lReg = sqlite3_exec(sqlitedb, "drop table Meter_Info_Table", NULL, 0,
				NULL);
		lReg =
				sqlite3_exec(sqlitedb,
						"create table Meter_Info_Table('User_id' int PRIMARY KEY,'Meter_addr','Meter_type','Pocol_type','Comm_port','Subitem','Building_id','meter_id','function_id')",
						NULL, 0, &zErrMsg);
#endif
		/*end:yangfei modified 2013-08-28 for add User_id 并设为主键*/
		if (lReg != SQLITE_OK) {
			printf("%s\n", zErrMsg);
			sqlite3_free(zErrMsg);
			printf("Create table Meter_Info_Table is Error! %d\n", lReg);
		}
	} else {
		printf("Meter_Info_Table has aready created!\n");
	}

	//判断是否已经创建System_Config_Table 表，若未创建过，则新创建
	lReg = sqlite3_exec(sqlitedb, "select *from System_Config_Table",
			Init_SysCfg, 0, &zErrMsg);
	if (lReg != SQLITE_OK) {
		printf("%s\n", zErrMsg);
		printf("sqlite Err is %d\n", lReg);
		sqlite3_free(zErrMsg);
		lReg =
				sqlite3_exec(sqlitedb,
						"create table System_Config_Table('Building_id','Gateway_id','Report_mode','Period','Md5_key','Aes_key', 'Ip_addr', 'Port', 'Dns', 'Back_ip_addr','Back_port', 'Back_dns')",
						NULL, 0, &zErrMsg);
		if (lReg != SQLITE_OK) {
			printf("%s\n", zErrMsg);
			sqlite3_free(zErrMsg);
		} else {
			printf("Create table System_Config_Table!\n");
			strcpy((char *) gSystem_Config.Str_Building_id, "111111A001");
			strcpy((char *) gSystem_Config.Str_Gateway_id, "111111A00101");
			strcpy((char *) gSystem_Config.Str_Report_mode, "1");
			strcpy((char *) gSystem_Config.Str_Period, CFG_PERIOD);
			strcpy((char *) gSystem_Config.Str_Md5_key, "513");
			strcpy((char *) gSystem_Config.Str_Aes_key, "513");
			strcpy((char *) gSystem_Config.Str_Ip_addr, CFG_IPADDR);
			strcpy((char *) gSystem_Config.Str_Port, CFG_PORT);
			strcpy((char *) gSystem_Config.Str_Dns, "");
			strcpy((char *) gSystem_Config.Str_Back_ip_addr, "122.5.18.174");
			strcpy((char *) gSystem_Config.Str_Back_port, "1147");
			strcpy((char *) gSystem_Config.Str_Back_dns, "");

			CMD_Sys_Config_SQlite();
		}

	} else {
		printf("System_Config_Table has aready created!\n");
	}

	//判断是否已经创建Electric_Meter_Table 表，若未创建过，则新创建
	lReg = sqlite3_get_table(sqlitedb,
			"select Meter_id from Electric_Meter_Table", &DB_result, &nrow,
			&ncolumn, &zErrMsg);
	if (lReg != SQLITE_OK) {
		printf("%s\n", zErrMsg);
		printf("sqlite Err is %d\n", lReg);
		sqlite3_free(zErrMsg);
		lReg = sqlite3_exec(sqlitedb, "drop table Electric_Meter_Table", NULL,
				0, NULL);
		lReg =
				sqlite3_exec(sqlitedb,
						"create table Electric_Meter_Table('Meter_addr','Meter_id','Read_time','functionid','Subitem','error','Total_active_power','Maximum_demand','Overall_power_factor')",
						NULL, 0, &zErrMsg);
		if (lReg != SQLITE_OK) {
			printf("%s\n", zErrMsg);
			sqlite3_free(zErrMsg);
			printf("Create table Electric_Meter_Table is Error! %d\n", lReg);
		} else {
			printf("Electric_Meter_Table has aready created!\n");
		}
	} else {
		printf("Electric_Meter_Table has aready created!\n");
	}

	//判断是否已经创建Heat_Meter_Table 表，若未创建过，则新创建
	lReg = sqlite3_get_table(sqlitedb, "select Meter_id from Heat_Meter_Table",
			&DB_result, &nrow, &ncolumn, &zErrMsg);
	//lReg = sqlite3_exec(sqlitedb, "select *from Heat_Meter_Table", NULL, 0, &zErrMsg);
	if (lReg != SQLITE_OK) {
		printf("%s\n", zErrMsg);
		printf("sqlite Err is %d\n", lReg);
		sqlite3_free(zErrMsg);
		lReg = sqlite3_exec(sqlitedb, "drop table Heat_Meter_Table", NULL, 0,
				NULL);
		lReg =
				sqlite3_exec(sqlitedb,
						"create table Heat_Meter_Table('Meter_addr','Meter_id','Read_time','functionid','Subitem','error','Acc_heat')",
						NULL, 0, &zErrMsg);
		if (lReg != SQLITE_OK) {
			printf("%s\n", zErrMsg);
			sqlite3_free(zErrMsg);
			printf("Create table Heat_Meter_Table is Error! %d\n", lReg);
		} else {
			printf("Heat_Meter_Table has aready created!\n");
		}
	} else {
		printf("Heat_Meter_Table has aready created!\n");
	}

	//判断是否已经创建Water_Meter_Table 表，若未创建过，则新创建
	lReg = sqlite3_get_table(sqlitedb, "select Meter_id from Water_Meter_Table",
			&DB_result, &nrow, &ncolumn, &zErrMsg);
	if (lReg != SQLITE_OK) {
		printf("%s\n", zErrMsg);
		printf("sqlite Err is %d\n", lReg);
		sqlite3_free(zErrMsg);
		lReg = sqlite3_exec(sqlitedb, "drop table Water_Meter_Table", NULL, 0,
				NULL);
		lReg =
				sqlite3_exec(sqlitedb,
						"create table Water_Meter_Table('Meter_addr','Meter_id','Read_time','functionid','Subitem','error','Acc_flow')",
						NULL, 0, &zErrMsg);
		if (lReg != SQLITE_OK) {
			printf("%s\n", zErrMsg);
			sqlite3_free(zErrMsg);
			printf("Create table Water_Meter_Table is Error! %d\n", lReg);
		} else {
			printf("Water_Meter_Table has aready created!\n");
		}
	} else {
		printf("Water_Meter_Table has aready created!\n");
	}

	//判断是否已经创建ReadTime_Queue_Table 表，若未创建过，则新创建
	lReg = sqlite3_exec(sqlitedb, "select *from ReadTime_Queue_Table", NULL, 0,
			&zErrMsg);
	if (lReg != SQLITE_OK) {
		printf("%s\n", zErrMsg);
		printf("sqlite Err is %d\n", lReg);
		sqlite3_free(zErrMsg);
		lReg =
				sqlite3_exec(sqlitedb,
						"create table ReadTime_Queue_Table('Read_time','Timenode_type')",
						NULL, 0, &zErrMsg);
		if (lReg != SQLITE_OK) {
			printf("%s\n", zErrMsg);

			sqlite3_free(zErrMsg);
			printf("Create table ReadTime_Queue_Table is Error! %d\n", lReg);
		}

	} else {
		printf("ReadTime_Queue_Table has aready created!\n");
	}

#if 0
	// gjt for test 06141512
	lReg = sqlite3_exec(sqlitedb, ".import a.txt Meter_Info_Table", NULL, 0, &zErrMsg);
	if( lReg!=SQLITE_OK )
	{
		printf("%s\n", zErrMsg);
		printf("sqlite Err is %d\n", lReg);
		printf("Import file a.txt Error!!\n", lReg);
	}
#endif

	//CMD_TransFile();

	return 0;
}

/* 主进程入口 */
INT32 main() {
	INT32 lReg = 0;

	pthread_t MbusPthreadId;
	pthread_t Com485PthreadId;
	pthread_t GPRS_Rec_Main_PthreadId;
	pthread_t GPRS_Rec_PthreadId;
	pthread_t TaskUpRecGprs_PthreadId;
	pthread_t GPRS_Mana_PthreadId;
	pthread_t GPRS_Upsend_PthreadId;
	pthread_t ReportUp_PthreadId;
	pthread_t XML_main_PthreadId;
	pthread_t TimerPthreadId;
	pthread_t CommandDealPthreadId;
	printf("**********************************************\n");
	printf("       Compile time:("__TIME__","__DATE__")\n");
	printf("**********************************************\n");

    Misc_Init();
	/* 打开IO控制驱动*/
	g_uiIoControl = open(IO_CONTROL, O_RDWR, 0);
	if (g_uiIoControl < 0) {
		printf("CONTROL_IO open error ....[%s][%s][%d] \n", FILE_LINE);
		return (CONTROL_RET_FAIL);
	}

	//初始化数据库，并做系统需求的数据库建表
	Init_Sqlite();

	g_GprsDeviceInfo.Speed = B115200;
	g_GprsDeviceInfo.ByteBits = CS8;
	memcpy(g_GprsDeviceInfo.Name, DEVICEGPRS, strlen(DEVICEGPRS));

	/* memset(g_stReadCardClientInfo.Client_Ip,0,sizeof(g_stReadCardClientInfo.Client_Ip)); */
	/* 创建Mbus抄表线程的线程 */
	lReg = pthread_create(&MbusPthreadId, NULL, (void *) mbus_main_proc, NULL);
	if (0 != lReg) {
		printf("Create Mbus pthread error!\n");
		exit(1);
	}
	/* 创建485抄表线程的线程 */

	lReg = pthread_create(&Com485PthreadId, NULL, (void *) com485_main_proc,
			NULL);
	if (0 != lReg) {
		printf("Create Com485 pthread error!\n");
		exit(1);
	}

	/*begin:yangfei added 2013-5-06*/
	lReg = pthread_create(&GPRS_Rec_Main_PthreadId, NULL,
			(void *) GPRS_Rec_main_proc, NULL);
	if (0 != lReg) {
		printf("Create GPRS_Rec_PthreadId!\n");
		exit(1);
	}

	lReg = pthread_create(&GPRS_Rec_PthreadId, NULL, (void *) GPRS_Rec_proc,
			NULL);
	if (0 != lReg) {
		printf("Create GPRS_Rec_PthreadId!\n");
		exit(1);
	}
	lReg = pthread_create(&TaskUpRecGprs_PthreadId, NULL,
			(void *) TaskUpRecGprs, NULL);
	if (0 != lReg) {
		printf("Create GPRS_Rec_PthreadId!\n");
		exit(1);
	}

	lReg = pthread_create(&GPRS_Mana_PthreadId, NULL,
			(void *) GPRS_Mana_main_proc, NULL);
	if (0 != lReg) {
		printf("Create GPRS_Mana_PthreadId error!\n");
		exit(1);
	}

	lReg = pthread_create(&GPRS_Upsend_PthreadId, NULL,
			(void *) GPRS_Upsend_main_proc, NULL);
	if (0 != lReg) {
		printf("Create GPRS_Upsend_PthreadId error!\n");
		exit(1);
	}

	lReg = pthread_create(&ReportUp_PthreadId, NULL,
			(void *) ReportUp_main_proc, NULL);
	if (0 != lReg) {
		printf("Create GPRS_Upsend_PthreadId error!\n");
		exit(1);
	}

	lReg = pthread_create(&XML_main_PthreadId, NULL, (void *) XML_main_proc,
			NULL);
	if (0 != lReg) {
		printf("Create GPRS_Upsend_PthreadId error!\n");
		exit(1);
	}

	lReg = pthread_create(&TimerPthreadId, NULL, (void *) ReadMeter_timer,
			NULL);
	if (0 != lReg) {
		printf("Create Timer pthread error!\n");
		exit(1);
	}
	lReg = pthread_create(&CommandDealPthreadId, NULL,
			(void *) CommandDeal_main_proc, NULL);
	if (0 != lReg) {
		printf("Create CommandDeal pthread error!\n");
		exit(1);
	}
	pthread_join(MbusPthreadId, NULL);
	pthread_join(Com485PthreadId, NULL);
	pthread_join(GPRS_Rec_Main_PthreadId, NULL);
	pthread_join(GPRS_Rec_PthreadId, NULL);
	pthread_join(TaskUpRecGprs_PthreadId, NULL);
	pthread_join(GPRS_Mana_PthreadId, NULL);
	pthread_join(GPRS_Upsend_PthreadId, NULL);
	pthread_join(ReportUp_PthreadId, NULL);
	pthread_join(XML_main_PthreadId, NULL);
	pthread_join(TimerPthreadId, NULL);
	pthread_join(CommandDealPthreadId, NULL);
	//while (1) {

	//}
	return (0);
}

void test() {
//printf("[%s][%s][%d]OSSemPend test \n",FILE_LINE);
//OSSemPend(&USART3RecQueSem_At, 20000, &ret);
//printf("[%s][%s][%d]OSSemPend test ok\n",FILE_LINE);

#if 0
	ShowTime();
	{
		char ch[50]="11220033";
		char array[4]= {0};
		printf("strlen(ch) %d\n",strlen(ch));

		StrToBin(ch,array);
		printf("[%x][%x][%x][%x] \n",array[0],array[1],array[2],array[3]);
		printf("strlen(ch) %d\n",strlen(array));

	}

	{
		int number=123456;
		int number1=0x123456;
		char string[20];
		//itoa(number,string,10);
		sprintf(string,"%d",number);

		printf("string=%s\n",string);

		sprintf(string,"%d",number1);

		printf("string=%s\n",string);
	}

#endif
	/*
	 list_insert_node(&pstList485Head,(void *)&a);
	 list_insert_node(&pstList485Head,(void *)&b);
	 list_insert_node(&pstList485Head,(void *)&c);
	 list_insert_node(&pstList485Head,(void *)&d);

	 list_del_node(&pstList485Head,(void *)&a);

	 list_del_node(&pstList485Head,(void *)&c);

	 list_del_node(&pstList485Head,(void *)&d);
	 */
#if 0
	sleep(2);
	/* 组装命令BUFF */
	stReadCardCmd.ucHead='r';
	stReadCardCmd.ucAddr='u';
	stReadCardCmd.ucLen=0x0b;
	stReadCardCmd.ucCmd=0x20;
	strcpy(stReadCardCmd.aucDate,"hahahaha");

	/* 发送数据到card 的服务端 */
	lSendLen = send(g_stReadCardClientInfo.lClientSockfd,(const UINT8 *)&stReadCardCmd,sizeof(READ_CARD_CMD_T),0);
	if(CONTROL_RET_FAIL == lSendLen)
	{
		printf("Send msg to server error\n");
		close(g_stReadCardClientInfo.lClientSockfd);
		exit(1);
	}
	printf("Send len :%d\n",lSendLen);

	lSendLen = recv(g_stReadCardClientInfo.lClientSockfd,(void *)&pstRead_Card_Cmd,sizeof(pstRead_Card_Cmd),0);
	if(CONTROL_RET_FAIL == lSendLen)
	{
		printf("recv error\n");
		exit(1);
	}

	printf("recv len :%d\n",lSendLen);
	printf("ucHead=%d  ucAddr=%d  ucLen=%d ucCmd=%d \n",pstRead_Card_Cmd.ucHead,pstRead_Card_Cmd.ucAddr,pstRead_Card_Cmd.ucLen,pstRead_Card_Cmd.ucCmd);
#endif
//com_test_main_proc();
	{
#if  0
		const unsigned char*buf = "yangfei";
		unsigned char testbuf[300]= {0};
		unsigned char *md = testbuf;
		char str[16];
		char array[4]= {0};
		char password[]="password";
		int i;
		MD5(password,strlen(password),str);

		/* 字符串转换为二进制数*/
		//str_to_data(str, array);
		//printf("[%x][%x][%x][%x] \n",array[0],array[1],array[2],array[3]);
		PUBLIC_HexToString(str, sizeof(str), testbuf,0);
		printf("[%s][%s][%d]%s \n",FILE_LINE,testbuf);

		strcat(gMD5_Key,"word");
		MD5_Password(gMD5_Key);
		//printf("[%s][%s][%d]%s \n",FILE_LINE,md);
		ShowTime(FILE_LINE);
		memset(testbuf,0,sizeof(testbuf));
		sprintf(testbuf,"openssl aes-128-cbc -salt -in test.txt -out test.txt.aes -k %s",password);
		system(testbuf);
		//system("openssl aes-128-cbc -salt -in test.txt -out test.txt.aes -k 123");
		memset(testbuf,0,sizeof(testbuf));
		sprintf(testbuf,"openssl aes-128-cbc -d -salt -in test.txt.aes -out test.txt.out -k %s",password);
		//system("openssl aes-128-cbc -d -salt -in test.txt.aes -out test.txt.out -k 123");
		system(testbuf);
		ShowTime(FILE_LINE);

		{
			uint8 Water_MeterAddr[7]= {11,22,33,44,55,66,77};
			uint8 Water_MeterAddrBCD[7];
			char TempString[17];
			int i;
			PUBLIC_BCDToString(TempString, Water_MeterAddr, 7);
			printf("     %s   \n",TempString);
			for(i=0; i<7; i++)
			{
				Water_MeterAddrBCD[i] = HexToBcd(Water_MeterAddr[i]);
				printf("%x ",Water_MeterAddrBCD[i]);
			}
			printf("\n");
			PUBLIC_HexToString( Water_MeterAddrBCD, 7,TempString,0);
			printf("     %s   \n",TempString);
		}
#endif
	}

	while (1) {
		char c;
		int value;
		c = getchar();
		if (c == '0') {
			if ((gXML_Rcv_File.writepos + 1) % XML_BUF_FILE_NUM
					== gXML_Rcv_File.readpos) {
				printf(
						"[%s][%s][%d] gXML_Rcv_File is full writepos=%d readpos=%d\n",
						FILE_LINE, gXML_Rcv_File.writepos,
						gXML_Rcv_File.readpos);
			}
			sem_wait(&gXML_Rcv_File.sem_write);

			sem_getvalue(&gXML_Rcv_File.sem_write, &value);
			printf("[%s][%s][%d]gXML_Rcv_File.sem_write=%d \n", FILE_LINE,
					value);
			gXML_Rcv_File.writepos = (gXML_Rcv_File.writepos + 1)
					% XML_BUF_FILE_NUM;
			sem_getvalue(&gXML_Rcv_File.sem_read, &value);
			printf("[%s][%s][%d]gXML_Rcv_File.sem_write=%d \n", FILE_LINE,
					value);

			sem_post(&gXML_Rcv_File.sem_read);
			printf("[%s][%s][%d]gXML_Rcv_File.writepos = %d \n", FILE_LINE,
					gXML_Rcv_File.writepos);
#if 0
			if((gXML_Send_File.writepos + 1)%XML_BUF_FILE_NUM == gXML_Send_File.readpos)
			{
				printf("[%s][%s][%d] XML_BUF_FILE is full \n",FILE_LINE);
			}
			else
			{
				gXML_Send_File.writepos=(gXML_Send_File.writepos + 1)%XML_BUF_FILE_NUM;
				printf("[%s][%s][%d]gXML_Send_File.writepos = %d \n",FILE_LINE,gXML_Send_File.writepos);
			}
			ret=makexml(PERIOD_ACK,gXML_Send_File.XMLFile[gXML_Send_File.writepos]);
			if(ret == 0)
			{
				sem_post(&UpSend_Sem);
				sem_post(&XMLSendFile_Sem);
			}
			else
			{
				printf("[%s][%s][%d]makexml err \n",FILE_LINE);
				gXML_Send_File.writepos--;
			}
#endif

			gGPRS_XMLType = NOTIFY;
			sem_post(&ReportUp_Sem);
		}

	}
}

