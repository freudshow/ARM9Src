#ifndef _GPRS_H
#define _GPRS_H

#define MAX_IPSEND_GTM900C_BYTENUM 	1024  //GTM900Cһ�η���(AT%IPSEND)���Ϊ1024�ֽ�(��������2048�������ַ�)
#define MAX_IPDATA_GTM900C_BYTENUM	2048  
//�������Ƿ���900C���յ�IPDATA�󷢹�����������1072�ֽ�(���˻ᰴ1072��ɶ��IPDATA������),�������ַ��ֻ���ִ���1072�����
//�������ֵ�㲻��,�ֽڶ�����һ����ֵ,ԭ��900B�����ǰ�1400�����
//
#define MAX_IPSEND_MC52i_BYTENUM	1500  //MC52iһ�η���(AT^SISW)���Ϊ1500�ֽ�,�����Ƹ�ʽ����
#define MAX_REC_MC52i_BYTENUM	1500  //MC52iһ�����ɶ��������������Ҳ��1500�ֽ�,�����������Լ�һ����1500��Ķ�û�������,��CPUȥҪ

typedef struct {
	uint8 ModuId; //MODU_ID_HW or MODU_ID_XMZ
	uint8 Ready; //TRUE or FALSE
	uint8 Connect; ////TRUE or FALSE
	uint8 SocketType; //0:TCP;1:UDP
	uint8 IpCloseNum; //TRUE or FALSE 
	uint8 IpSendFailNum;
	uint8 IpSendRemnBufNum; //��һ��IPSENDʱGPRSģ���ʣ�໺��������
	uint8 Csq; //�ź�ǿ��

	uint32 FrmRecTimes; //����֡���� //���Թ۲���
	uint32 FrmSndTimes; //����֡���� //���Թ۲���

	uint32 FrmRecBs; //�����ֽ���
	uint32 FrmSndBs; //�����ֽ���

	uint32 RecDog; //����GPRS���յĿ��Ź�
	uint32 SndDog; //����GPRS���͵Ŀ��Ź�
} GPRS_RUN_STA;

extern GPRS_RUN_STA gGprsRunSta;

#define MODU_ID_UNKNOWN		0x55
#define MODU_ID_HW			0x12
#define MODU_ID_XMZ			0x21

#define LC_PORT_MIN		10000
#define LC_PORT_RANGE  	10000

extern uint8 gStartTime[];
extern uint16 RandFirRand16(void);
extern uint8 nAsciiToHex(char* str);
extern uint8 ModelIdentify(void);
extern uint8 GprsInit_hw(void);
extern uint8 GprsInit_xmz(void);

extern uint8 GprsGetIPDATA_xmz(char* ipdata, uint16 OutTime, uint16* StrLen);
extern uint8 GprsGetIPDATA_jh(char* ipdata, uint16 OutTime, uint16* StrLen);
extern uint8 GprsIPSEND_hw(uint8* ipdata, uint16 len, uint16* pBufNum);
extern uint8 GprsIPSEND_xmz(uint8* ipdata, uint16 len, uint16* pBufNum);
extern uint8 CMD_IpClose_hw(void);
extern uint8 CMD_IpClose_xmz(void);
extern uint8 GprsGetIPDATA(char* ipdata, uint16 OutTime, uint16* StrLen);
extern uint8 GprsIPSEND(uint8* ipdata, uint16 len, uint16* pBufNum);
extern uint8 CMD_IpClose(void);
extern uint8 CMD_Csq(uint8* csq);

extern void InitGprsRunSta(void);
extern void ReadGprsRunSta(GPRS_RUN_STA* Sta);
extern uint8 ReadCurSType(void);
extern void UpdGprsRunSta(GPRS_RUN_STA* Sta);
extern void UpdGprsRunSta_Ready(uint8 val);
extern void UpdGprsRunSta_IpCloseNum(uint8 val);
extern void UpdGprsRunSta_IpSendRemnBufNum(uint8 val);
extern void UpdGprsRunSta_Csq(uint8 val);
extern void UpdGprsRunSta_AddIpCloseNum(void);
extern void UpdGprsRunSta_AddIpSendFailNum(void);
extern void UpdGprsRunSta_FeedIpSendFailNum(void);
extern void UpdGprsRunSta_FeedRecDog(void);
extern void UpdGprsRunSta_AddRecDog(void);
extern void UpdGprsRunSta_FeedSndDog(void);
extern void UpdGprsRunSta_AddSndDog(void);
extern void UpdGprsRunSta_AddFrmRecTimes(uint32 n);
extern void UpdGprsRunSta_AddFrmSndTimes(uint32 n);
extern void UpdGprsRunSta_Stype(uint8 val);
extern void UpdGprsRunSta_Cont(uint8 val);
extern void UpdGprsRunSta_ModuId(uint8 val);
extern uint8 GetGprsRunSta_ModuId(void);
void GprsRestart_xmz(void);
void GprsOn_xmz(void);
//#if TEST_SELF_CHECK > 0
//extern uint8 SetXmzBsp(void);
//#endif

extern uint8 GetGprsCSQ(void); //��ȡGPRS�ź�ǿ��  ����ֵ��ΧΪ0(С)--4(��)���ֱ��ʾGPRS��ǿ�ȴ�С

//=============����ͨ���豸ID ����Щ������ֱֵ����Ϊȫ��������±�ʹ�ã���˲�������޸�
#define  UP_COMMU_DEV_ZIGBEE	0
#define	 UP_COMMU_DEV_485		1
#define	 UP_COMMU_DEV_232		2
#define	 UP_COMMU_DEV_GPRS		3
#define  UP_COMMU_DEV_AT		4
#define  UP_COMMU_DEV_ATIPD		5

#endif

