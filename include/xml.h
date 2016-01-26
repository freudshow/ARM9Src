#ifndef _XML_H
#define _XML_H

#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<libxml/xmlmemory.h>
#include<libxml/parser.h>

//�궨��

#define ID_VALIDATE 1
#define HEART_BEAT  2
#define DATA        3
#define CONFIG      4
#define IPADDR      5
#define DATABASE    6

//�ɼ������ͱ�־
#define  REQUEST    0
#define  MD5_XML        1
#define  NOTIFY     2
#define  REPLY      3
#define  REPORT     4
#define  CONTINUOUS    5
#define  PERIOD_ACK    6
#define  IP_PORT_ACK   7
#define  BASIC_DB_REP  8
#define  PACK_LOST_ACK 9
#define  REPLY_CONFIG  10
#define  ID_ACK        11
#define  REPORT_ACK    12
#define  KEY_ACK       13
#define  CONTROL_ACK   14
#define  MODIFY_ACK        15
#define  REALTIME_ACK      16
#define   QUERY_REQ_ACK  17
#define  TRANS_ACK           18

//�ɼ������ձ�־
#define  SEQUENCE        20
#define  RESULT          21
#define  TIME            22
#define  QUERY           23
#define  CONTINUOUS_ACK  24
#define  PERIOD          25
#define  IPPORT          26
#define  BASIC_DB        27
#define  FILE_TRANS      28
#define  PACK_LOST       29
#define  QUERY_CONFIG    30
#define  ID_CONFIG       31
#define  REPORT_MODE     32
#define  MD5_KEY         33
#define  AES_KEY         34
#define  REBOOT          35
#define  MODIFY          36
#define  REALTIME_QRY 37
#define Query_req        38
#define TRANS_REQ         39

#define  NONE            99

#define     MAX_XML_NODE_STRING_LEN              512
#define     MAX_XML_NODE_NUM                          50

typedef struct common {
	xmlChar building_id[20];
	xmlChar gateway_id[20];
	xmlChar type[16];
} common, *commonptr;

typedef struct id_validate {
	xmlChar operation[16];    //����
	xmlChar request;      //�ӽڵ�
	xmlChar sequence[100];     //�ӽڵ�
	xmlChar md5[300];          //�ӽڵ�
	xmlChar result[8];       //�ӽڵ�
} id_validate, *id_validateptr;

typedef struct heart_beat {
	xmlChar operation[16];      //����
	xmlChar notify;        //�ӽڵ�
	xmlChar time[16];           //�ӽڵ�
} heart_beat, *heart_beatptr;

typedef struct meterinfo {
	xmlChar id[36];      //����
	struct {
		xmlChar fid[12];      //����
		xmlChar coding[20];      //����
		xmlChar error[8];    //����
		xmlChar content[60];
	} function[10];           //�ӽڵ�
} meterinfo, *meterinfoptr;

typedef struct data {
	xmlChar operation[20];  //����
	xmlChar sequence[8];   //�ӽڵ�
	xmlChar parser[4];
	xmlChar time[16];
	xmlChar total[8];
	xmlChar current[8];
	meterinfo meter[50];
} data, *dataptr;

typedef struct config {
	xmlChar operation[16];   //����
	xmlChar period[4];
	xmlChar period_ack;
} config, *configptr;

typedef struct ipaddr {
	xmlChar operation[16];
	struct {
		xmlChar ip[20];
		xmlChar dns[20];
		xmlChar port[10];
	} server[2];
} ipaddr, *ipaddrptr;

struct filetrans {
	xmlChar operation[16];
	xmlChar pack_total[4];           //�ļ����ݰ�����Ŀ, ����file_dataʱ��Ч
	xmlChar pack_sequence[4];        //��������иð������, ����file_dataʱ��Ч
	xmlChar file_type[16];            //�ļ�����, ����file_dataʱ��Ч
	xmlChar file_bidata[2 * 1024];    //�ļ�������������, ����file_dataʱ��Ч
	xmlChar pack_lost_total[4];      //�ļ�����ȱ��������������pack_lost_ackʱ��Ч
	xmlChar pack_lost_index[50];     //�ļ�����ȱ���İ����,����pack_lost_ackʱ��Ч
	xmlChar trans_req_flag[12];
};

struct query_config {
	xmlChar operation[16];
	xmlChar period[4];         //���ݲɼ�����
	xmlChar report_mode[4];       //�����ϱ�ģʽ 1:�����ϱ���0:������ѯ
	xmlChar version[20];       //����汾��
};

struct ex_config {
	xmlChar operation[16];
	xmlChar new_building_id[20];    //¥���±�ţ�����Id_config ��Ч
	xmlChar new_gateway_id[20];     //�ɼ����±��, ����Id_config ��Ч
	xmlChar report_mode[4];            //�����ϱ�ģʽ 1:�����ϱ���0:������ѯ������report_modeʱ��Ч
};
//��������

struct update_key {
	xmlChar operation[16];
	xmlChar md5_key[129];         //128λmd5У����Կ������md5_keyʱ��Ч
	xmlChar aes_key[129];         //128λaesУ����Կ������aes_keyʱ��Ч
};
//��Կ����

struct control {
	xmlChar operation[16];
};
//����������ָ��

struct xml_modify_content_t {
	xmlChar sqlStr[MAX_XML_NODE_STRING_LEN];
};
struct xml_modify_req_t {
	int num;
	struct xml_modify_content_t content[MAX_XML_NODE_NUM];
};

struct xml_modify_rsp_t {
	char flag[6];
};

struct xml_realtime_req_t {
	uint8 type;
	uint8 protocolVer;
	uint8 channel;
	uint8 meterAddr[14];
};

struct xml_realtime_rsp_t {
	char error[12];
	char result[256];
};

struct xml_reportsrv_req_t {
	char query[256];
};

struct xml_reportsrv_rsp_t {
	char flag[6];
	char count[6];
	char *result;
};
extern id_validate gxml_IDInfo_R, gxml_IDInfo_T;          //���������֤��Ϣ
extern common gxml_ComInfo_R, gxml_ComInfo_T;         //ͨ�ò���
extern heart_beat gxml_BeatInfo_R, gxml_BeatInfo_T;        //����/Уʱ
extern config gxml_CfgInfo_R, gxml_CfgInfo_T;         //������Ϣ
//data   gxml_DataInfo_R = {"reply","123","yes","20130514","100","20",{"1",{{"8","xx","xx","xx"},{"0","xx","xx","xx"},{"1","xx","xx","xx"},{"2","xx","xx","xx"},}}};
extern data gxml_DataInfo_R, gxml_DataInfo_T;        //�ܺ�Զ������
extern ipaddr gxml_IPInfo_R, gxml_IPInfo_T;          //ip��ַ��Ϣ
extern struct filetrans gxml_TransInfo_R, gxml_TransInfo_T;  //�ļ�����
extern struct query_config gxml_QueryInfo_R, gxml_QueryInfo_T;  //��ѯ���� 
extern struct ex_config gxml_ExInfo_R, gxml_ExInfo_T;       //�������� 
extern struct update_key gxml_KeyInfo_R, gxml_KeyInfo_T;      //��Կ����
extern struct control gxml_CtlInfo_R, gxml_CtlInfo_T;      //����������
extern struct xml_modify_req_t gxml_modify_t;
extern struct xml_modify_rsp_t gxml_modify_rsp_t;
extern struct xml_realtime_req_t gxml_realtime_req_t;
extern struct xml_realtime_rsp_t gxml_realtime_rsp_t;
extern struct xml_reportsrv_req_t gxml_reportsrv_req_t;
extern struct xml_reportsrv_rsp_t gxml_reportsrv_rsp_t;
int XML_main_proc(void *argv);

int makexml(char optype, char*filename);
#endif
