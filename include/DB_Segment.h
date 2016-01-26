#ifndef __DB_SEGMENT_H
#define __DB_SEGMENT_H

/********************************************************************************
 **			数据库读取时，callback 回调函数参数定义
 **	callback(void *NotUsed, int argc, char **argv, char **azColName)	
 **  int argc --- 共有多少个字段
 **	char **argv --- 指向各个字段指针数组的指针，*argv 则为某字段数据开始地址
 **	char **azColName --- 指向各个字段名称指针数组的指针，
 **        *azColName 则为某字段名称的开始地址
 **
 ********************************************************************************/

/********************************************************************************
 **							System_Config_Table
 ** Building_id | Gateway_id | Report_mode | Period | Md5_key | Aes_key | Ip_addr |
 **
 ** Port | Dns | Back_ip_addr | Back_port | Back_dns
 **
 ** 	给每个字段定义枚举数据，在数据库读取时根据枚举数据直接取该字段数据
 **
 ********************************************************************************/

enum DB_System_Config {
	gSEG_Building_id,
	gSEG_Gateway_id,
	gSEG_Report_mode,
	gSEG_Period,
	gSEG_Md5_key,
	gSEG_Aes_key,
	gSEG_Ip_addr,
	gSEG_Port,
	gSEG_Dns,
	gSEG_Back_ip_addr,
	gSEG_Back_port,
	gSEG_Back_Dns
};

/********************************************************************************
 **							Meter_Info_Table
 ** Meter_addr | Meter_type | Protocol_type | Comm_port | Subitem | Building_id
 **
 ** 	给每个字段定义枚举数据，在数据库读取时根据枚举数据直接取该字段数据
 **
 ********************************************************************************/

enum DB_Meter_Info {
	gSEG_User_id_MeterInfo,
	gSEG_Meter_addr_MeterInfo,
	gSEG_Meter_type_MeterInfo,
	gSEG_Protocol_type_MeterInfo,
	gSEG_Comm_port_MeterInfo,
	gSEG_Subitem_MeterInfo,
	gSEG_Building_id_MeterInfo,
	gSEG_Meter_id_MeterInfo,
	gSEG_Function_id
};

/********************************************************************************
 **							Electric_Meter_Table
 ** Meter_addr | Read_time | Subitem | Total_active_power | Maximum_demand | 
 **
 ** Overall_power_factor
 **
 ** 	给每个字段定义枚举数据，在数据库读取时根据枚举数据直接取该字段数据
 **
 ********************************************************************************/

enum DB_Electric_Meter {
	gSEG_Meter_addr_ElectricMeter,
	gSEG_Meter_id_ElectricMeter,
	gSEG_Read_time_ElectricMeter,
	gSEG_Function_id_ElectricMeter,
	gSEG_Subitem_ElectricMeter,
	gSEG_Error_ElectricMeter,
	gSEG_Total_active_power_ElectricMeter,
	gSEG_Maximum_demand_ElectricMeter,
	gSEG_Overall_power_factor_ElectricMeter
};

/********************************************************************************
 **							Heat_Meter_Table
 ** Meter_addr | Read_time | Subitem |  |  | 
 **
 ** 
 **
 ** 	给每个字段定义枚举数据，在数据库读取时根据枚举数据直接取该字段数据
 **
 ********************************************************************************/

enum DB_Heat_Meter {
	gSEG_Meter_addr_HeatMeter,
	gSEG_Meter_id_HeatMeter,
	gSEG_Read_time_HeatMeter,
	gSEG_Function_id_HeatMeter,
	gSEG_Subitem_HeatMeter,
	gSEG_Error_HeatMeter,
	gSEG_Heat_HeatMeter
};

/********************************************************************************
 **							 Water_Meter_Table
 ** Meter_addr | Read_time | Subitem |  |  | 
 **
 ** 
 **
 ** 	给每个字段定义枚举数据，在数据库读取时根据枚举数据直接取该字段数据
 **
 ********************************************************************************/

enum DB_Water_Meter {
	gSEG_Meter_addr_WaterMeter,
	gSEG_Meter_id_WaterMeter,
	gSEG_Read_time_WaterMeter,
	gSEG_Function_id_WaterMeter,
	gSEG_Subitem_WaterMeter,
	gSEG_Error_WaterMeter,
	gSEG_Flow_WaterMeter
};

typedef struct {
	char Str_Building_id[20];
	char Str_Gateway_id[20];
	char Str_Report_mode[8];
	char Str_Period[8];
	char Str_Md5_key[40];
	char Str_Aes_key[40];
	char Str_Ip_addr[24];
	char Str_Port[16];
	char Str_Dns[100];
	char Str_Back_ip_addr[24];
	char Str_Back_port[16];
	char Str_Back_dns[100];

} System_ConfigType;

#endif
