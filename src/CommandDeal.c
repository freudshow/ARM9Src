/********************************************************************************
 **
 **
 **		用于处理与上位机交互的指令信息
 **
 **
 **
 *******************************************************************************/
#include "includes.h"

sem_t gGPRS_Send_Succ_Sem;

//系统的配置信息通过全局变量结构体来维护，便于应用
System_ConfigType gSystem_Config;

volatile UINT8 gXml_Rcv_Query = 0;

char gGPRS_Send_Succ_Flag = 0;

static UINT32 gCMD_sqMeterIndex;

/*begin:yangfei added 2013-06-05 for update variable*/
struct filetrans gxml_TransInfo_R;  //文件传输
uint16 gVersion = 0x0210;
//static char  UpdatePath[] = "/app/update/V02.01.bin";
//static uint16 MaxPackSize = 512;
uint16 gPacketReceiveFlag[250] = { 0 };
char gUpdateData[5 * 1024] = { 0x00 }; /*升级文件数据*/
int gPack_total = 0;

/*end:yangfei added 2013-06-05*/
// gjt add 06161608 start
static char gFileType[16] = ".none";
// gjt add end

extern uint16 gUpdateBegin;
static FILE *fp;
extern void Mbus_Channel(uint32 channel);
INT32 continuous_data(void);
INT32 report_data(void);
//debug_info(gDebugModule[XML_DEAL], "This is report up test!\n");

static int SQ_Elect_MeterData(void *NotUsed, int argc, char **argv,
		char **azColName) {
	debug_info(gDebugModule[XML_DEAL],
			"Start select data from elect_meterdata table!\n");

	/*begin add by yangmin 2013-08-27 ,for meter[50]*/
	if (gCMD_sqMeterIndex >= 50) {
		printf("[%s][%s][%d]:the  gCMD_sqMeterIndex >= 50\n", FILE_LINE);
		return 1;
	}
	/*end add  by yangmin 2013-08-27 ,for meter[50]*/

	strcpy((char *) gxml_DataInfo_T.meter[gCMD_sqMeterIndex].id,
			argv[gSEG_Meter_id_ElectricMeter]);

	//电能表多功能表 能耗数据项
	//正向有功总电能
	strcpy((char *) gxml_DataInfo_T.meter[gCMD_sqMeterIndex].function[0].fid,
			"9010");
	strcpy((char *) gxml_DataInfo_T.meter[gCMD_sqMeterIndex].function[0].coding,
			argv[gSEG_Subitem_ElectricMeter]);
	strcpy((char *) gxml_DataInfo_T.meter[gCMD_sqMeterIndex].function[0].error,
			"0");
	strcpy(
			(char *) gxml_DataInfo_T.meter[gCMD_sqMeterIndex].function[0].content,
			argv[gSEG_Total_active_power_ElectricMeter]);

//正向有功最大需量
//	strcpy((char *)gxml_DataInfo_T.meter[gCMD_sqMeterIndex].function[1].fid, "");
//	strcpy((char *)gxml_DataInfo_T.meter[gCMD_sqMeterIndex].function[1].coding, argv[gSEG_Subitem_ElectricMeter]);
//	strcpy((char *)gxml_DataInfo_T.meter[gCMD_sqMeterIndex].function[1].error, "0");
//	strcpy((char *)gxml_DataInfo_T.meter[gCMD_sqMeterIndex].function[1].content, argv[gSEG_Maximum_demand_ElectricMeter]);
	gCMD_sqMeterIndex++;
	return 0;
}

static int SQ_Heat_MeterData(void *NotUsed, int argc, char **argv,
		char **azColName) {
	char HeatDataStr[256] = "";

	debug_info(gDebugModule[XML_DEAL],
			"Start select data from heat_meterdata table!\n");
	//可对供热数据进行拼接，也可以类似电能表进行多种数据类型分id 显示，暂时只显示热量数据
	strcat(HeatDataStr, argv[gSEG_Heat_HeatMeter]);
	/*begin add by yangmin 2013-08-27 ,for meter[50]*/
	if (gCMD_sqMeterIndex >= 50) {
		printf("[%s][%s][%d]:the  gCMD_sqMeterIndex >= 50\n", FILE_LINE);
		return 1;
	}
	/*end add  by yangmin 2013-08-27 ,for meter[50]*/
	strcpy((char *) gxml_DataInfo_T.meter[gCMD_sqMeterIndex].id,
			argv[gSEG_Meter_id_HeatMeter]);

	//热计量表能耗数据
	strcpy((char *) gxml_DataInfo_T.meter[gCMD_sqMeterIndex].function[0].fid,
			"9011");
	strcpy((char *) gxml_DataInfo_T.meter[gCMD_sqMeterIndex].function[0].coding,
			argv[gSEG_Subitem_HeatMeter]);
	strcpy((char *) gxml_DataInfo_T.meter[gCMD_sqMeterIndex].function[0].error,
			"0");
	strcpy(
			(char *) gxml_DataInfo_T.meter[gCMD_sqMeterIndex].function[0].content,
			HeatDataStr);
	gCMD_sqMeterIndex++;
	return 0;
}

static int SQ_Water_MeterData(void *NotUsed, int argc, char **argv,
		char **azColName) {
	char WaterDataStr[256] = "";

	debug_info(gDebugModule[XML_DEAL],
			"Start select data from water_meterdata table!\n");
	//可对水表数据进行拼接，也可以类似电能表进行多种数据类型分id 显示，暂时只显示总流量数据
	strcat(WaterDataStr, argv[gSEG_Flow_WaterMeter]);

	/*begin add by yangmin 2013-08-27 ,for meter[50]*/
	if (gCMD_sqMeterIndex >= 50) {
		printf("[%s][%s][%d]:the  gCMD_sqMeterIndex >= 50\n", FILE_LINE);
		return 1;
	}
	/*end add  by yangmin 2013-08-27 ,for meter[50]*/

	strcpy((char *) gxml_DataInfo_T.meter[gCMD_sqMeterIndex].id,
			argv[gSEG_Meter_id_WaterMeter]);

	//热计量表能耗数据
	strcpy((char *) gxml_DataInfo_T.meter[gCMD_sqMeterIndex].function[0].fid,
			"9010");
	strcpy((char *) gxml_DataInfo_T.meter[gCMD_sqMeterIndex].function[0].coding,
			argv[gSEG_Subitem_WaterMeter]);
	strcpy((char *) gxml_DataInfo_T.meter[gCMD_sqMeterIndex].function[0].error,
			"0");
	strcpy(
			(char *) gxml_DataInfo_T.meter[gCMD_sqMeterIndex].function[0].content,
			WaterDataStr);
	gCMD_sqMeterIndex++;
	return 0;
}

static int CMD_CreateMeterData(UINT8 Type, char *pReadtime) {
	INT32 ret = 0;

	char *zErrMsg = 0;
	char SQComd[256] = "";

	strcpy((char *) gxml_DataInfo_T.sequence, "01");
	strcpy((char *) gxml_DataInfo_T.parser, "no");

	gCMD_sqMeterIndex = 0;

	//Electric_Meter_Table
	strcpy(SQComd, "select * from Electric_Meter_Table where Read_time == ");
	strcat(SQComd, "'");
	strcat(SQComd, pReadtime);
	strcat(SQComd, "'");
	ret = sqlite3_exec(sqlitedb, SQComd, SQ_Elect_MeterData, 0, &zErrMsg);
	if (ret != SQLITE_OK) {
		printf("[%s][%s][%d] \n", FILE_LINE);
		fprintf(stderr, "SQL error: %s\n", zErrMsg);
		sqlite3_free(zErrMsg);
	}

	//Heat_Meter_Tbale
	strcpy(SQComd, "select * from Heat_Meter_Table where Read_time == ");
	strcat(SQComd, "'");
	strcat(SQComd, pReadtime);
	strcat(SQComd, "'");
	printf("%s", SQComd);
	ret = sqlite3_exec(sqlitedb, SQComd, SQ_Heat_MeterData, 0, &zErrMsg);
	if (ret != SQLITE_OK) {
		printf("[%s][%s][%d] \n", FILE_LINE);
		fprintf(stderr, "SQL error: %s\n", zErrMsg);
		sqlite3_free(zErrMsg);
	}

	//Water_Meter_Tbale
	strcpy(SQComd, "select * from Water_Meter_Table where Read_time == ");
	strcat(SQComd, "'");
	strcat(SQComd, pReadtime);
	strcat(SQComd, "'");
	printf("%s", SQComd);
	ret = sqlite3_exec(sqlitedb, SQComd, SQ_Water_MeterData, 0, &zErrMsg);
	if (ret != SQLITE_OK) {
		printf("[%s][%s][%d] \n", FILE_LINE);
		fprintf(stderr, "SQL error: %s\n", zErrMsg);
		sqlite3_free(zErrMsg);
	}

	debug_notice(gDebugModule[XML_DEAL], "SQlite Find Data END!!\n");
	return 0;
}

char gXml_TypeDefine = NONE;

INT32 CMD_Sys_Config_SQlite(void) {
	int ret = 0;
	char *zErrMsg = 0;
	char sqStr[256] = "";

	ret = sqlite3_exec(sqlitedb, "drop table System_Config_Table", NULL, 0,
			&zErrMsg);
	if (ret != SQLITE_OK) {
		printf("sqlite delete  Error!\n");
		printf("[%s][%s][%d] \n", FILE_LINE);
		fprintf(stderr, "SQL error: %s\n", zErrMsg);
		sqlite3_free(zErrMsg);
	}
	ret =
			sqlite3_exec(sqlitedb,
					"create table System_Config_Table('Building_id','Gateway_id','Report_mode','Period','Md5_key','Aes_key', 'Ip_addr', 'Port', 'Dns', 'Back_ip_addr','Back_port', 'Back_dns')",
					NULL, 0, &zErrMsg);
	if (ret != SQLITE_OK) {
		printf("%s\n", zErrMsg);
		sqlite3_free(zErrMsg);
	}

	strcpy(sqStr, "insert into System_Config_Table values ('");
	strcat(sqStr, gSystem_Config.Str_Building_id);
	strcat(sqStr, "', '");
	strcat(sqStr, gSystem_Config.Str_Gateway_id);
	strcat(sqStr, "', '");
	strcat(sqStr, gSystem_Config.Str_Report_mode);
	strcat(sqStr, "', '");
	strcat(sqStr, gSystem_Config.Str_Period);
	strcat(sqStr, "', '");
	strcat(sqStr, gSystem_Config.Str_Md5_key);
	strcat(sqStr, "', '");
	strcat(sqStr, gSystem_Config.Str_Aes_key);
	strcat(sqStr, "', '");
	strcat(sqStr, gSystem_Config.Str_Ip_addr);
	strcat(sqStr, "', '");
	strcat(sqStr, gSystem_Config.Str_Port);
	strcat(sqStr, "', '");
	strcat(sqStr, gSystem_Config.Str_Dns);
	strcat(sqStr, "', '");
	strcat(sqStr, gSystem_Config.Str_Back_ip_addr);
	strcat(sqStr, "', '");
	strcat(sqStr, gSystem_Config.Str_Back_port);
	strcat(sqStr, "', '");
	strcat(sqStr, gSystem_Config.Str_Back_dns);
	strcat(sqStr, "')");

	debug_info(gDebugModule[XML_DEAL], "sqlite insert cmd is %s !\n", sqStr);

	ret = sqlite3_exec(sqlitedb, sqStr, NULL, 0, &zErrMsg);
	if (ret != SQLITE_OK) {
		printf("[%s][%s][%d] \n", FILE_LINE);
		fprintf(stderr, "SQL error: %s/n", zErrMsg);
		sqlite3_free(zErrMsg);
	}

	return ret;
}

int32 CMD_TransFile(char* FileType) {
	int ret = 0;
	char *zErrMsg;
	FILE *transfp;
	char SQ_Str[160] = "";
	char Meterinfo_Str[100] = "";
	char filechar = 1;
	UINT32 filecounter = 0;
	printf("[%s][%s][%d] CMD_TransFile = %s\n", FILE_LINE, FileType);
	transfp = fopen("transfile", "r");
	if (transfp == NULL) {
		printf("Open transfile Error!\n");
	}

	if (strcmp(".meteraddress", (char*) FileType) == 0) {
		debug_info(gDebugModule[XML_MODULE],
				"[%s][%s][%d] meteraddress file for meter addr\n", FILE_LINE);
		fseek(transfp, 0, 0);

		ret = sqlite3_exec(sqlitedb, "delete from Meter_Info_Table", NULL, 0,
				&zErrMsg);
		if (ret != SQLITE_OK) {
			printf("[%s][%s][%d] \n", FILE_LINE);
			fprintf(stderr, "SQL error: %s/n", zErrMsg);
			sqlite3_free(zErrMsg);
		}
		printf("[%s][%s][%d] \n", FILE_LINE);
		while (feof(transfp) == 0) {
			filechar = getc(transfp);
			if (filechar == '\n') {
				debug_info(gDebugModule[XML_MODULE],
						"[%s][%s][%d] filecounter = %d\n", FILE_LINE,
						filecounter);
				Meterinfo_Str[filecounter] = '\0';
				filecounter = 0;
				strcpy(SQ_Str, "insert into Meter_Info_Table values (");
				strcat(SQ_Str, Meterinfo_Str);
				strcat(SQ_Str, ")");
				printf("The transfile string is %s\n", SQ_Str);
				ret = sqlite3_exec(sqlitedb, SQ_Str, NULL, 0, &zErrMsg);
				if (ret != SQLITE_OK) {
					printf("[%s][%s][%d] \n", FILE_LINE);
					fprintf(stderr, "SQL error: %s\n", zErrMsg);
					sqlite3_free(zErrMsg);
				}
			} else {
				Meterinfo_Str[filecounter++] = filechar;
				if (filecounter > sizeof(Meterinfo_Str) - 1) {
					printf("filecounter >  sizeof(Meterinfo_Str)-1\n");
					filecounter = 0;
				}
			}
		}
		printf("meterInfo inset into Sqlite3!\n");
		system("rm transfile");/*yangfei added*/
	} else if (strcmp(".tar", (char*) FileType) == 0) {
		debug_info(gDebugModule[XML_MODULE],
				"[%s][%s][%d] Bin file for update the fireware\n", FILE_LINE);
		printf("update file receive ok! begin update ............\n");
		sleep(30);/*延时30秒确保PACK_LOST_ACK 已经回复*/
		system("mv transfile update.tar");/*yangfei added*/
	} else {
		printf("unknow FileType = %s\n", FileType);
	}

	return 0;
}

INT32 CMD_ReviceXMLData( UINT8 Xml_TypeDefine)/*Xml_TypeDefine*/
{
	uint8 xmlTemp[256] = { 0x00 };
	char TimeTemp[20] = "";
	UINT32 xmlTempLen = 0x00;

	switch (Xml_TypeDefine) {
	case SEQUENCE:
		debug_info(gDebugModule[XML_DEAL], "Start Deal SEQUENCE......!\n")
		;
		OSSemPost(SEQUEUE_XML);
#if 0 /*yangfei deleted for ConnectConfirm() will deal*/
		//身份验证数据包，随即序列
		xmlTempLen = StrToBin((char *)gxml_IDInfo_R.sequence, xmlTemp);
		//调用MD5运算函数

		//组建上报帧
		gGPRS_XMLType = MD5_XML;
		PUBLIC_BCDToString((char *)gxml_IDInfo_R.md5, xmlTemp, xmlTempLen);
		sem_post(&ReportUp_Sem);
#endif
		//OS_EXIT_CRITICAL();

		break;

	case RESULT:

		debug_info(gDebugModule[XML_DEAL], "Start Deal RESULT......!\n")
		;
		if (!strcmp((char *) gxml_IDInfo_R.result, "pass")) {
			OSSemPost(RESULT_XML);
		}

		break;

	case TIME:
		debug_info(gDebugModule[XML_DEAL], "Start Deal TIME......!\n")
		;
		xmlTempLen = StrToBin((char *) gxml_BeatInfo_R.time, xmlTemp);
		strcpy(TimeTemp, (char *) gxml_BeatInfo_R.time);
		if (xmlTemp[5] == 0x00)			//min is  0x00
				{
			//set system time
			memset(xmlTemp, 0x00, sizeof(xmlTemp));
			strcpy((char*) xmlTemp, "date -s ");
			memcpy(&xmlTemp[8], TimeTemp, 4);
			xmlTemp[12] = '.';
			memcpy(&xmlTemp[13], &TimeTemp[4], 2);
			xmlTemp[15] = '.';
			memcpy(&xmlTemp[16], &TimeTemp[6], 2);
			xmlTemp[18] = '-';
			memcpy(&xmlTemp[19], &TimeTemp[8], 2);
			xmlTemp[21] = ':';
			memcpy(&xmlTemp[22], &TimeTemp[10], 2);
			xmlTemp[24] = ':';
			memcpy(&xmlTemp[25], &TimeTemp[12], 2);
			xmlTemp[27] = '\0';

			printf("The PC Time is %s", xmlTemp);
			system((char*) xmlTemp);
			system("hwclock -w");
		}

		break;

	case QUERY:

		debug_info(gDebugModule[XML_DEAL], "Start Deal QUERY......!\n")
		;
		g_uiDataDealStatus |= STATU_REPORTUP;
		gXml_Rcv_Query = 1;
		sem_post(&Xml_Deal_Sem);

		break;
	case CONTINUOUS_ACK:
		debug_info(gDebugModule[XML_DEAL], "RCV CMD CONTINUOUS_ACK......!\n")
		;
		sem_post(&Xml_Continuous_Sem);
		break;

	case PERIOD:

		debug_info(gDebugModule[XML_DEAL], "Start Deal PERIOD......!\n")
		;
		strcpy((char *) gSystem_Config.Str_Period,
				(char *) gxml_CfgInfo_R.period);
		sem_wait(&Xml_Send_Sem);
		gGPRS_XMLType = PERIOD_ACK;
		sem_post(&ReportUp_Sem);

		//存入数据库
		CMD_Sys_Config_SQlite();

		break;
	case IPPORT:

		debug_info(gDebugModule[XML_DEAL], "Start Deal IPPORT......!\n")
		;
		strcpy((char *) gSystem_Config.Str_Ip_addr,
				(char *) gxml_IPInfo_R.server[0].ip);
		strcpy((char *) gSystem_Config.Str_Port,
				(char *) gxml_IPInfo_R.server[0].port);
		strcpy((char *) gSystem_Config.Str_Dns,
				(char *) gxml_IPInfo_R.server[0].dns);

		strcpy((char *) gSystem_Config.Str_Back_ip_addr,
				(char *) gxml_IPInfo_R.server[1].ip);
		strcpy((char *) gSystem_Config.Str_Back_port,
				(char *) gxml_IPInfo_R.server[1].port);
		strcpy((char *) gSystem_Config.Str_Back_dns,
				(char *) gxml_IPInfo_R.server[1].dns);
		sem_wait(&Xml_Send_Sem);
		gGPRS_XMLType = IP_PORT_ACK;
		sem_post(&ReportUp_Sem);
		//存入数据库
		CMD_Sys_Config_SQlite();
		sleep(15);
		system("reboot");			//重启系统
		break;

	case FILE_TRANS: {
		int PackSize;
		int pack_sequence;
		int WriteNum = 0;

		debug_info(gDebugModule[XML_MODULE], "[%s][%s][%d] update file\n",
				FILE_LINE);

		gPack_total = atoi((char*) gxml_TransInfo_R.pack_total);
		pack_sequence = atoi((char*) gxml_TransInfo_R.pack_sequence);
		if (gPack_total == 1) {
			PackSize = strlen((char*) gxml_TransInfo_R.file_bidata) / 2;
			printf("gPack_total=1,PackSize is %d\n", PackSize);
		}
		if (pack_sequence < gPack_total) {
			PackSize = strlen((char*) gxml_TransInfo_R.file_bidata) / 2;
			printf("PackSize is %d\n", PackSize);
			if (PackSize != 1000) {
				printf("PackSize is %d\n", PackSize);
				printf("exit......\n");
				//exit(0);
				break;/*长度不对退出*/
			}
		} else if (pack_sequence > gPack_total) {
			perror("pack_sequence > pack_total\n");
			break;
		}
		//gjt test 0620
		StrToBin((char*) gxml_TransInfo_R.file_bidata, (uint8*) gUpdateData);/*将接收到的数据转换成2进制数*/
		//gjt end
		//strcpy((char *)gUpdateData,(char*)gxml_TransInfo_R.file_bidata);
		//printf("The gUpdateData String len is %d", strlen(gUpdateData));

		/*yangfei added 20130904 for creat transfile if donot exit*/

		if (fp == NULL) {
			fp = fopen("transfile", "rb+");
			if (fp == NULL) {
				fp = fopen("transfile", "wb+");
				fclose(fp);
				fp = fopen("transfile", "rb+");
			}
		}
		if (-1 == fseek(fp, 1000 * (pack_sequence - 1), SEEK_SET))/*位置指针到文件写入的地方*/
		{
			printf("Transfile Saved  Fseek Err!\n");
		}
		WriteNum = fwrite(gUpdateData, 1,
				strlen((char*) gxml_TransInfo_R.file_bidata) / 2, fp);
		//printf("strlen(gUpdateData) num is %d\n", strlen((char*)gxml_TransInfo_R.file_bidata)/2);
		printf("file write num is %d\n", WriteNum);
		if (WriteNum == strlen((char*) gxml_TransInfo_R.file_bidata) / 2) {
			gPacketReceiveFlag[pack_sequence] = 0xaa;

		} else {
			debug_err(gDebugModule[XML_MODULE],
					"[%s][%s][%d] [ERR]:transfile file write error num\n",
					FILE_LINE);
		}
		memset(gUpdateData, 0, sizeof(gUpdateData));
		//fclose(fp);
		//gjt add 06161612
		strcpy(gFileType, (char*) gxml_TransInfo_R.file_type);
		//gjt add end
	}
		break;

	case PACK_LOST:/*yangfei*/
	{
		int i = 0;
		uint8 LackPacket = 0;
		//uint8 Pack_lost_index[100] = {0};
		char string[6] = { 0 };
		printf("**********************************");
		printf("PACK_LOST!\n");
		/**/
		memset(gxml_TransInfo_T.pack_lost_index, 0,
				sizeof(gxml_TransInfo_T.pack_lost_index));
		for (i = 1; i <= gPack_total; i++) {
			if (gPacketReceiveFlag[i] != 0xaa) {
				if (LackPacket > 20)/*防止回复的命令太长超过1024*/
				{
					break;
				}
#if 0  /*yangfei modified*/
				Pack_lost_index[LackPacket++] =HexToBcd(i);
#else
				LackPacket++;
				sprintf(string, "%d", i);
				strcat((char*) gxml_TransInfo_T.pack_lost_index, string);
				strcat((char*) gxml_TransInfo_T.pack_lost_index, ",");
#endif
				printf("LackPacket = %d\n", i);
			}
		}
		printf("LackPacket total = %d\n", LackPacket);
		printf("**********************************");
		gUpdateBegin = 0;/*yangfei added 20130707*/
#if 1  /*yangfei modified*/
		sprintf((char*) gxml_TransInfo_T.pack_lost_total, "%d", LackPacket);
#else
		LackPacket = HexToBcd(LackPacket);
		PUBLIC_HexToString(&LackPacket, sizeof(LackPacket),(char*) gxml_TransInfo_T.pack_lost_total,0);

		PUBLIC_HexToString(Pack_lost_index, strlen((char*)Pack_lost_index), (char*)gxml_TransInfo_T.pack_lost_index,',');
#endif
		//strcpy((char*) gxml_TransInfo_T.pack_lost_total,"0");
		sem_wait(&Xml_Send_Sem);/*yangfei added for 只要发送XML必须申请Xml_Send_Sem*/
		gGPRS_XMLType = PACK_LOST_ACK;
		sem_post(&ReportUp_Sem);

		break;
	}
	case QUERY_CONFIG:

		debug_info(gDebugModule[XML_DEAL], "Start Deal QUERY_CONFIG......!\n")
		;
		strcpy((char *) gxml_QueryInfo_T.period,
				(char *) gSystem_Config.Str_Period);
		strcpy((char *) gxml_QueryInfo_T.report_mode,
				(char *) gSystem_Config.Str_Report_mode);
		strcpy((char *) gxml_QueryInfo_T.version, FIREWARE_VER);

		sem_wait(&Xml_Send_Sem);/*yangfei added for 只要发送XML必须申请Xml_Send_Sem*/
		gGPRS_XMLType = REPLY_CONFIG;
		sem_post(&ReportUp_Sem);
		//返回数据帧

		break;

	case ID_CONFIG:

		debug_info(gDebugModule[XML_DEAL], "Start Deal ID_CONFIG......!\n")
		;
		printf("old Str_Building_id = %s Str_Gateway_id=%s\n",
				gSystem_Config.Str_Building_id, gSystem_Config.Str_Gateway_id);
		printf("new Str_Building_id = %s Str_Gateway_id=%s\n",
				gxml_ExInfo_R.new_building_id, gxml_ExInfo_R.new_gateway_id);

		strcpy((char *) gSystem_Config.Str_Building_id,
				(char *) gxml_ExInfo_R.new_building_id);
		strcpy((char *) gSystem_Config.Str_Gateway_id,
				(char *) gxml_ExInfo_R.new_gateway_id);

		CMD_Sys_Config_SQlite();
		sem_wait(&Xml_Send_Sem);
		gGPRS_XMLType = ID_ACK;
		sem_post(&ReportUp_Sem);

		sleep(15);
		system("reboot");				//重启系统

		break;

	case REPORT_MODE:

		debug_info(gDebugModule[XML_DEAL], "Start Deal REPORT_MODE......!\n")
		;
		strcpy((char *) gSystem_Config.Str_Report_mode,
				(char *) gxml_ExInfo_R.report_mode);

		CMD_Sys_Config_SQlite();
		sem_wait(&Xml_Send_Sem);
		gGPRS_XMLType = REPORT_ACK;
		sem_post(&ReportUp_Sem);

		break;

	case MD5_KEY:

		debug_info(gDebugModule[XML_DEAL], "Start Deal MD5_KEY......!\n")
		;
		strcpy((char *) gSystem_Config.Str_Md5_key,
				(char *) gxml_KeyInfo_R.md5_key);

		CMD_Sys_Config_SQlite();
		sem_wait(&Xml_Send_Sem);
		gGPRS_XMLType = KEY_ACK;
		sem_post(&ReportUp_Sem);

		break;

	case AES_KEY:

		debug_info(gDebugModule[XML_DEAL], "Start Deal AES_KEY......!\n")
		;
		strcpy((char *) gSystem_Config.Str_Aes_key,
				(char *) gxml_KeyInfo_R.aes_key);
		CMD_Sys_Config_SQlite();
		sem_wait(&Xml_Send_Sem);
		gGPRS_XMLType = KEY_ACK;
		sem_post(&ReportUp_Sem);

		break;

	case REBOOT:

		debug_info(gDebugModule[XML_DEAL], "System will Reboot......!\n")
		;
		sem_wait(&Xml_Send_Sem);
		gGPRS_XMLType = CONTROL_ACK;
		sem_post(&ReportUp_Sem);
		OSTimeDly(OS_TICKS_PER_SEC * 10);
		system("reboot");				//重启系统
		break;
		/*begin added by yangmin - 20130829*/
	case MODIFY: {
		int i = 0;
		int lReg = 0;
		char *zErrMsg = 0;
		debug_info(gDebugModule[XML_DEAL], "Start modify......!\n");
		for (i = 0; i < gxml_modify_t.num; i++) {
			lReg = sqlite3_exec(sqlitedb,
					(char *) gxml_modify_t.content[i].sqlStr, NULL, 0,
					&zErrMsg);
			if (lReg != SQLITE_OK) {
				printf("sqlite3_exec Err is %d(%s)\n", lReg, zErrMsg);
				sqlite3_free(zErrMsg);
				break;
			}
		}
		if (i < gxml_modify_t.num)
			sprintf(gxml_modify_rsp_t.flag, "%d", i);
		else
			sprintf(gxml_modify_rsp_t.flag, "0");
	}
		sem_wait(&Xml_Send_Sem);
		gGPRS_XMLType = MODIFY_ACK;
		sem_post(&ReportUp_Sem);
		break;
	case REALTIME_QRY: {
		MeterFileType mf;
		struct mdev_meterinfo meterinfo;
		memset(&mf, 0, sizeof(MeterFileType));
		memset(&meterinfo, 0, sizeof(struct mdev_meterinfo));
		//StrToBin((char *)gxml_realtime_req_t.meterAddr, mf.Meter_addr);/*yangfei modified 20140317*/
		mf.Meter_type = gxml_realtime_req_t.type;
		StrToBin_MeterAddress((char *) gxml_realtime_req_t.meterAddr,
				mf.Meter_addr, mf.Meter_type);/*yangfei modified 20140317*/
		mf.Protocol_type = gxml_realtime_req_t.protocolVer;
		mf.ReadMeter_type = RealtimeReadMeter;
		meterinfo.channel = gxml_realtime_req_t.channel;
		Read_Meter(&meterinfo, &mf);
		sem_wait(&Xml_Send_Sem);
		gGPRS_XMLType = REALTIME_ACK;
		sem_post(&ReportUp_Sem);
		break;
	}

	case Query_req: {
		int i, nrow, ncolumn, loop;
		int ret;
		char **DB_result = 0;
		char *zErrMsg = 0;

		printf("execute %s\n", gxml_reportsrv_req_t.query);

		ret = sqlite3_get_table(sqlitedb, gxml_reportsrv_req_t.query,
				&DB_result, &nrow, &ncolumn, &zErrMsg);
		if (ret != SQLITE_OK) {
			printf("[%s][%s][%d] \n", FILE_LINE);
			sqlite3_free(zErrMsg);
			sprintf(gxml_reportsrv_rsp_t.flag, "1");
			goto Query_req_end;
		}

		if (nrow == 0) {
			sprintf(gxml_reportsrv_rsp_t.flag, "0");
			sprintf(gxml_reportsrv_rsp_t.count, "0");
			goto Query_req_end;
		}
		nrow++;  //返回值包含了table头
		sprintf(gxml_reportsrv_rsp_t.flag, "0");
		sprintf(gxml_reportsrv_rsp_t.count, "%d", nrow);
		gxml_reportsrv_rsp_t.result = (char *) malloc(nrow * 512);
		memset(gxml_reportsrv_rsp_t.result, 0, nrow * 512);
		printf("Query_req row = %d,column=%d\n", nrow, ncolumn);
		for (i = 0; i < nrow; i++) {
			for (loop = 0; loop < ncolumn; loop++) {
				strcat(gxml_reportsrv_rsp_t.result + (i * 512),
						DB_result[i * ncolumn + loop]);
				if (loop != (ncolumn - 1))
					strcat(gxml_reportsrv_rsp_t.result + (i * 512), ",");
			}
			printf("%s\n", gxml_reportsrv_rsp_t.result + (i * 512));

		}
		sqlite3_free_table(DB_result);
	}
		Query_req_end: sem_wait(&Xml_Send_Sem);
		gGPRS_XMLType = QUERY_REQ_ACK;
		sem_post(&ReportUp_Sem);
		break;
	case TRANS_REQ: {
		if (strcmp((char *) gxml_TransInfo_R.operation, "Trans_req") == 0
				&& strcmp((char *) gxml_TransInfo_R.trans_req_flag, "start")
						== 0) {
			if (fp != NULL) {
				fclose(fp);
			}

			fp = fopen("transfile", "wb+");
			fclose(fp);
			fp = fopen("transfile", "rb+");
			sem_wait(&Xml_Send_Sem);
			gGPRS_XMLType = TRANS_ACK;
			sem_post(&ReportUp_Sem);
			/*begin:yangfei added 20130903 for 防止主动上报，等文件传输完成后再把连接打开*/
			gGprsRunSta.Connect = FALSE;
			/*end:yangfei added 20130903 for 防止主动上报，等文件传输完成后再把连接打开*/
		} else if (strcmp((char *) gxml_TransInfo_R.operation, "Trans_req") == 0
				&& strcmp((char *) gxml_TransInfo_R.trans_req_flag, "end")
						== 0) {

			memset(gPacketReceiveFlag, 0, sizeof(gPacketReceiveFlag));
			if (fp != NULL) {
				fclose(fp);
				fp = NULL;
			}
			printf("file transfer ok!\n");
			sem_wait(&Xml_Send_Sem);
			gGPRS_XMLType = TRANS_ACK;
			sem_post(&ReportUp_Sem);
			CMD_TransFile(gFileType);
			/*begin:yangfei added 20130903 for 防止主动上报，等文件传输完成后再把连接打开*/
			gGprsRunSta.Connect = TRUE;
			/*end:yangfei added 20130903 for 防止主动上报，等文件传输完成后再把连接打开*/
		} else {
			printf(
					"gxml_TransInfo_R.operation = %s gxml_TransInfo_R.trans_req_flag = %s\n",
					gxml_TransInfo_R.operation,
					gxml_TransInfo_R.trans_req_flag);
		}

		break;
	}
	default: {
		debug_err(gDebugModule[XML_DEAL],
				"The XML Type is Error , the type is %d!\n", Xml_TypeDefine);
		break;
	}

	}

	return 0;
}
/*

 */
INT32 CommandDeal_main_proc(void) {

	while (1) {
		debug_info(gDebugModule[XML_DEAL],
				"wait the Sem to create xml data and report up\n");
		sem_wait(&Xml_Deal_Sem);
		printf("Command Deal Rcv Sem!\n");
		debug_info(gDebugModule[XML_DEAL],
				"Rcv the Sem to create xml data and report up\n");

		if (g_uiDataDealStatus & STATU_REPORTUP) {
			g_uiDataDealStatus &= ~STATU_REPORTUP;

			report_data();

			g_uiDataDealStatus |= STATU_CONTINUOUS;
		}
		//由GPRS管理任务来 触发，在GPRS上线时触发来断点续传
		if (g_uiDataDealStatus & STATU_CONTINUOUS) {
			g_uiDataDealStatus &= ~STATU_CONTINUOUS;

			if (gGprsRunSta.Connect == FALSE) {
				debug_debug(gDebugModule[XML_DEAL], "GPRS Connect False!\n");
				gGPRS_Send_Succ_Flag = FALSE;
			} else {
				continuous_data();
			}

		}
	}

}

INT32 report_data(void) {

	int ret = 0;
	int value = 0;
	INT32 nrow = 0;
	INT32 ncolumn = 0;

	char *zErrMsg = 0;
	char **DB_result;

	char SqCmd_Str[256] = "";
	char lReadingTime[20] = "";
	struct timespec ts;
	struct timeval tv;

	debug_info(gDebugModule[XML_DEAL], "start wait for Xml_Send_sem\n");
	ret = sem_getvalue(&Xml_Send_Sem, &value);
	if (ret != 0) {
		printf("[%s][%s][%d] %s \n", FILE_LINE, strerror(errno));
	}
	debug_debug(gDebugModule[XML_DEAL], "[%s][%s][%d]sem_read=%d \n", FILE_LINE,
			value);

	sem_wait(&Xml_Send_Sem);
	debug_info(gDebugModule[XML_DEAL], "Xml_Send_sem is alive!!\n");

	//get readingtime from sqlite
	strcpy(SqCmd_Str,
			"select * from ReadTime_Queue_Table where Timenode_type == 'REPORT'");
	ret = sqlite3_get_table(sqlitedb, SqCmd_Str, &DB_result, &nrow, &ncolumn,
			&zErrMsg);
	if (ret != SQLITE_OK) {
		printf("[%s][%s][%d] \n", FILE_LINE);
		fprintf(stderr, "SQL error: %s\n", zErrMsg);
		sqlite3_free(zErrMsg);
		sem_post(&Xml_Send_Sem);
		return 1;
	}
	/*modify by yangmin- 2013.08.29 ,handle exception*/
	if (nrow == 0) {
		printf("[%s][%s][%d] ReadTime_Queue_Table no 'REPORT' data\n", FILE_LINE
				);
		sem_post(&Xml_Send_Sem);
		return 2;
	}

	printf("the report time is %s\n", DB_result[ncolumn]);
	strcpy(lReadingTime, DB_result[ncolumn]);
	//ym 2014-02-25 解决时间不变的问题
	strcpy((char *) gxml_DataInfo_T.time, lReadingTime);

	strcpy((char *) gxml_DataInfo_T.total, "1");

	strcpy((char *) gxml_DataInfo_T.current, "1");

	/*begin:yangmin added 2013-08-27 for  release memory */
	sqlite3_free_table(DB_result);
	/*end:yangmin added 2013-08-27 for release memory*/
	/*begin:yangfei added 20130903 for 如果GPRS不在线就不去建立xml文件*/
	printf("GPRS connect sta is %d\n", gGprsRunSta.Connect);
	if (gGprsRunSta.Connect == FALSE) {
		gGPRS_Send_Succ_Flag = FALSE;
		sem_post(&Xml_Send_Sem);
		goto SEND_FALSE;
	}

	/*如果GPRS在线进行下面的操作*/
	ret = CMD_CreateMeterData(REPORT, lReadingTime);
	if (ret) {
		sem_post(&Xml_Send_Sem);
		return 3;
	}

	debug_info(gDebugModule[XML_DEAL], "the gXml_Rcv_Query is %d",
			gXml_Rcv_Query);
	if (gXml_Rcv_Query) {
		gGPRS_XMLType = REPLY;
		gXml_Rcv_Query = 0;
	} else {
		gGPRS_XMLType = REPORT;
	}

	debug_info(gDebugModule[XML_DEAL], "the gGprs_XMLtype is %d\n",
			gGPRS_XMLType);
	sem_post(&ReportUp_Sem);
	debug_info(gDebugModule[XML_DEAL], "the gGprsRunSta.Connect is %d\n",
			gGprsRunSta.Connect);

	gettimeofday(&tv, NULL);
	ts.tv_sec = tv.tv_sec + 60;
	ret = sem_timedwait(&gGPRS_Send_Succ_Sem, &ts);
	if (ret != 0) {
		printf("gGPRS_Send_Succ_Sem is time out!\n");
	} else if (ret == 0) {
		gGPRS_Send_Succ_Flag = TRUE;
		printf("Report up gGPRS_Send_Succ_Flag is True!!\n");
		return 0;
	}

	SEND_FALSE: if (gGPRS_Send_Succ_Flag == FALSE) {
		//该时间点的数据 转存到 断点续传数据库表中
		debug_info(gDebugModule[XML_DEAL],
				" Report Up Xml file By GPRS! Transfer is Failure!!\n");
		/*
		 strcpy(SqCmd_Str, "update ReadTime_Queue_Table SET Timenode_type='CONTINUOUS'");
		 strcat(SqCmd_Str, "where Read_time='");
		 strcat(SqCmd_Str, lReadingTime);
		 strcat(SqCmd_Str, "'");
		 */
		//先判断该时间点是否已经加入到 CONTINUOUS 队列
		strcpy(SqCmd_Str, "insert into ReadTime_Queue_Table values('");
		strcat(SqCmd_Str, lReadingTime);
		strcat(SqCmd_Str, "', ");
		strcat(SqCmd_Str, "'CONTINUOUS');");
		debug_info(gDebugModule[XML_DEAL], "%s\n", SqCmd_Str);
		ret = sqlite3_exec(sqlitedb, SqCmd_Str, NULL, 0, &zErrMsg);
		if (ret != SQLITE_OK) {
			printf("[%s][%s][%d] \n", FILE_LINE);
			fprintf(stderr, "SQL error: %s\n", zErrMsg);
			sqlite3_free(zErrMsg);
		}
	}
	return -1;
}

INT32 continuous_data(void) {
	INT32 i = 0;
	int ret = 0;

	INT32 nrow = 0;
	INT32 ncolumn = 0;

	char *zErrMsg = 0;
	char **DB_result;
	char TempString[50] = "";
	char SqCmd_Str[256] = "";
	char lReadingTime[20] = "";
	struct timespec ts;
	struct timeval tv;
	//get readingtime from sqlite
	strcpy(SqCmd_Str,
			"select * from ReadTime_Queue_Table where Timenode_type == 'CONTINUOUS'");
	ret = sqlite3_get_table(sqlitedb, SqCmd_Str, &DB_result, &nrow, &ncolumn,
			&zErrMsg);
	if (ret != SQLITE_OK) {
		printf("[%s][%s][%d] \n", FILE_LINE);
		fprintf(stderr, "SQL error: %s/n", zErrMsg);
		sqlite3_free(zErrMsg);
		return 1;;
	}

	for (i = 0; i < nrow; i++) {
		printf("The STATU_CONTINUOUS have %d Rows\n", nrow);
		printf("The segmente name is  %s\n", DB_result[ncolumn + i * 2]);
		strcpy(lReadingTime, DB_result[ncolumn + i * 2]);

		sem_wait(&Xml_Send_Sem);

		strcpy((char *) gxml_DataInfo_T.time, lReadingTime);
		sprintf(TempString, "%d", nrow);
		strcpy((char *) gxml_DataInfo_T.total, TempString);
		sprintf(TempString, "%d", i + 1);
		strcpy((char *) gxml_DataInfo_T.current, TempString);

		printf("Start CONTINUOUS SQLite Find Data!\n");
		ret = CMD_CreateMeterData(CONTINUOUS, lReadingTime);
		if (ret) {
			sem_post(&Xml_Send_Sem);
			continue;
		}

		gGPRS_XMLType = CONTINUOUS;
		if (gGprsRunSta.Connect == FALSE) {
			debug_debug(gDebugModule[XML_DEAL], "GPRS Connect False!\n");
			gGPRS_Send_Succ_Flag = FALSE;
			sem_post(&Xml_Send_Sem);
			return 2;/**/
		} else {
			debug_debug(gDebugModule[XML_DEAL],
					"Start Post sem to GPRS Send Data!\n");
			sem_post(&ReportUp_Sem);

			gettimeofday(&tv, NULL);
			ts.tv_sec = tv.tv_sec + 60;
			ret = sem_timedwait(&Xml_Continuous_Sem, &ts);
			if (ret == -1) {
				printf("Xml_Continuous_Sem is time out!\n");
				gGPRS_Send_Succ_Flag = FALSE;
			} else if (ret == 0) {
				gGPRS_Send_Succ_Flag = TRUE;
				printf(" STATU_CONTINUOUS  gGPRS_Send_Succ_Flag is True!!\n");
			}
		}

		if (gGPRS_Send_Succ_Flag == TRUE)/*断点续传成功，删除该时间点*/
		{
			//断点续传成功，删除该时间点
			debug_info(gDebugModule[XML_DEAL],
					" STATU_CONTINUOUS is Successful!\n");
			debug_info(gDebugModule[XML_DEAL],
					" delete the Read_Date %s from ReadTime_Queue_Table!\n",
					lReadingTime);

			strcpy(SqCmd_Str,
					"delete from ReadTime_Queue_Table where Read_time == ");
			strcat(SqCmd_Str, "'");
			strcat(SqCmd_Str, lReadingTime);
			strcat(SqCmd_Str, "'");
			ret = sqlite3_exec(sqlitedb, SqCmd_Str, NULL, 0, &zErrMsg);
			if (ret != SQLITE_OK) {
				printf("[%s][%s][%d] \n", FILE_LINE);
				fprintf(stderr, "SQL error: %s/n", zErrMsg);
				sqlite3_free(zErrMsg);
			}
		}
	}
	/*begin:yangmin added 2013-08-27 for  release memory */
	sqlite3_free_table(DB_result);
	/*end:yangmin added 2013-08-27 for release memory*/
	return 0;
}

