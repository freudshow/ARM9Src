#include "includes.h"

#define    HEAD_MAX_LEN    64
#define	   SOCKET_TCP	   0

//����˵����Ats_: AT��������,Ata_: ATӦ������
char Ats_CGMM[] = "AT+CGMM\r";
/*begin:yangfei modified 2013-01-15*/
char Ata_CGMM_xmz_R3[] = "\r\nMC52iR3\r\n\r\nOK\r\n";
char Ata_CGMM_xmz[] = "\r\nMC52i\r\n\r\nOK\r\n";
/*end:yangfei modified 2013-01-15*/
char Ats_SMSO[] = "AT^SMSO\r";
char Ata_SMSO[] = "\r\n^SMSO: MS OFF\r\n";

//����connecting profile:
//at^sics=0,conType,GPRS0
//at^sics=0,apn,cmnet
char Ats_SICS_conType[] = "at^sics=0,conType,GPRS0\r";
char Ats_SICS_apn[256] = "at^sics=0,apn,cmnet\r";
// char Ats_SICS_apn[256]="at^sics=0,apn,uninet\r";

///����tcp service profile
//at^siss=1,srvType,socket
char Ats_SISS_srvType[] = "at^siss=1,srvType,socket\r";

//at^siss=1,conId,0
char Ats_SISS_conId[] = "at^siss=1,conId,0\r";

//at^siss=1,address,"socktcp://117.22.67.183:5100"
char Ats_SISS_address_h[] = "at^siss=1,address,\"socktcp://"; //117.22.67.183:5100\r\"";
char Ats_SISS_address[96];
char IpAddrXmz[64] = "113.132.156.2"; //117.22.0.121  
//char IpAddrXmz[64]="1.85.185.244";
char IpPort[16] = "1147";
char colon[] = ":";

//��TCP����
//AT^SISO=1 
char Ats_SISO[] = "AT^SISO=1\r"; //AT^SISO=1 
char Ats_SISI[] = "AT^SISI=1\r"; //AT^SISI=1 
char Ata_SISI[] = "\r\n^SISI: 1,4,0,0,0,0\r\n\r\nOK\r\n";
char Ata_SISI_1[] = "\r\n^SISI: 1,4,"; //"\r\n^SISI: 1,4,0,0,0,0\r\n\r\nOK\r\n";

char Ats_SISW_h[] = "AT^SISW=1,"; //at^sisw=1,30\r
char Ats_SISW[64];
char Ata_SISW_m[] = "\r\n^SISW: 1, "; //^SISW: 1, 20, 20

char Ata_URC_SISR[] = "\r\n^SISR: 1, 1\r\n"; //^SISW: 1, 1\r\n
char Ats_SISR[] = "AT^SISR=1,1500\r";
char Ata_SISR_m[] = "\r\n^SISR: 1, "; //

char Ats_AT[] = "AT\r";
char Ata_OK[] = "\r\nOK\r\n";
char Ata_OK123[] = "\r\n\r\nOK\r\n";
//char Ata_OK125[]="\r\r\n\r\nOK\r\n";
char Ats_SSYNC[] = "AT^SSYNC=1\r";

char Ats_SSYNC_c[] = "AT^SSYNC?\r";
char Ata_SSYNC_c[] = "\r\n^SSYNC: 1\r\n\r\nOK\r\n";

//char Ats_IPR[]="AT+IPR=9600\r";
//char Ats_IPR[]="AT+IPR=57600\r";
char Ats_IPR[] = "AT+IPR=115200\r";
char Ats_IPR_0[] = "AT+IPR=0\r";

char Ats_IPR_c[] = "AT+IPR?\r";
//char Ata_IPR_c[]="\r\n+IPR: 9600\r\n\r\nOK\r\n";
//char Ata_IPR_c[]="\r\n+IPR: 57600\r\n\r\nOK\r\n";
char Ata_IPR_c[] = "\r\n+IPR: 115200\r\n\r\nOK\r\n";

char Ats_CPIN[] = "AT+CPIN\?\r"; ////��� SIM �����Ƿ�����
char Ata_CPIN[] = "\r\n+CPIN: READY\r\n\r\nOK\r\n";

char Ats_CREG[] = "AT+CREG\?\r"; ////��� GSM ����ע�����
char Ata_CREG[] = "\r\n+CREG: 0,1\r\n\r\nOK\r\n"; //����
char Ata_CREG_Or[] = "\r\n+CREG: 0,5\r\n\r\nOK\r\n"; //����

char Ats_CGREG_1[] = "AT+CGREG=1\r"; //

char Ats_CGATT_1[] = "AT+CGATT\?\r";
char Ata_CGATT_1[] = "+CGATT: 1\r\n\r\nOK\r\n";
char Ats_CGATT[] = "AT+CGATT=1\r";
//char Ats_CGAUTO[]="AT+CGAUTO=1\r";

char Ats_CGREG[] = "AT+CGREG\?\r"; //��� GPRS ����ע�����
char Ata_CGREG[] = "\r\n+CGREG: 0,1\r\n\r\nOK\r\n"; //����
char Ata_CGREG_Or[] = "\r\n+CGREG: 0,5\r\n\r\nOK\r\n"; //����

char Ata_CGREG_yf[] = "\r\n+CGREG: 1,1\r\n\r\nOK\r\n"; //����
char Ata_CGREG_Or_yf[] = "\r\n+CGREG: 1,5\r\n\r\nOK\r\n"; //����

char Ats_CGDCONT[256] = "AT+CGDCONT=1,\"ip\",\"cmnet\"\r"; //���� APN���߽���� //HGGDGS.HB

//char Ats_CGDCONT[256]="AT+CGDCONT=1,\"ip\",\"uninet\"\r";  

//Ata_OK

//char Ats_ETCPIP[]="AT%ETCPIP=\"csdytq\",\"csdytq\"\r";   //���� ppp ����
char Ats_ETCPIP[] = "AT%ETCPIP=\"\",\"\"\r";   //���� ppp ����
//Ata_OK
//============��������
//char At_IPOPEN[]="AT%IPOPEN=\"UDP\",";//AT%IPOPEN="TCP","222.91.64.151",5100
char At_IPOPEN[] = "AT%IPOPEN=\"TCP\",";  //AT%IPOPEN="TCP","222.91.64.151",5100
char comma[] = ",";
char LcIpPort[16];
char over123[] = "\r";
char over124[] = "\"\r";
char Ats_IPOPEN[128];
char Ata_IPOPEN[] = "\r\nCONNECT\r\n";
//=============

char Ats_ATE0[] = "ATE0\r";
//Ata_OK
char Ats_CSQ[] = "AT+CSQ\r"; //��鵱�ص������ź�ǿ��
//char Ata_CSQ[]="\r\n+CSQ: 27,99 \r\n\r\nOK\r\n";
char Ats_IPCLOSE[] = "AT%IPCLOSE?\r";
char Ata_IPCLOSE[] = "\r\n%IPCLOSE: 1,0,0\r\n\r\nOK\r\n";
//========================
char AtaHead_CSQ[] = "\r\n+CSQ:"; //"\r\n+CSQ: 27,99 \r\n\r\nOK\r\n";
char AtaHead_IPSEND[] = "\r\n%IPSEND:"; //"\r\n%IPSEND:15\r\n\r\nOK\r\n" //900c: \r\n%IPSEND:1,15 \r\nOK\r\n
char AtaHead_IPDATA[] = "%IPDATA:";
//%IPDATA:8,"3132333435363636"
//%IPDATA:29,"3234323334323334323334333234323334333234323334323334323334"
//%IPDATA:144,"333234353334353334353334353334353335333435333435333435343335333435333435343335343335333435333435333435333435333435333435333435333435333435333435333234353334353334353334353334353335333435333435333435343335333435333435343335343335333435333435333435333435333435333435333435333435333435333435"
//%IPDATA:25,"68450045006841FFFFFFFFFF84F10000801E3117070006A416"\r\n
//========================
char Ats_IPSEND[] = "AT%IPSEND=\"";
char over_isnd[] = "\"\r";
///r/nERROR: 9 //��ǰû�� TCP�� UDP ���Ӵ���(��� IPSEND)
///r/nERROR: 20//TCP���ͻ�������

char TmpBuf[HEAD_MAX_LEN]; //���Թ۲���
GPRS_RUN_STA gGprsRunSta;

char gIpSendData[MAX_IPSEND_GTM900C_BYTENUM * 2 + 100]; //900Cһ�η���(AT%IPSEND)���ΪMAX_IPSEND_GTM900C_BYTENUM==1024�ֽ�(��������2048�������ַ�,���Գ˶�),�ټ������ŵȾͶ������һЩ

//extern UpCommParaSaveType	gPARA_UpPara;		//GPRS����ͨѶ����

uint16 gFirRand16;
uint8 gStartTime[6];

uint8 test_buff[512];

extern uint8 gGprsFirst;

void GPRS_POW_ON(void) {
	int stat;
	stat = ioctl(g_uiIoControl, GPRS_SWITCH_C28, 1);
	if (stat) {
		printf("[%s][%s][%d] stat = %d \n", FILE_LINE, stat);
	}
}
void GPRS_POW_OFF(void) {
	int stat;
	stat = ioctl(g_uiIoControl, GPRS_SWITCH_C28, 0);
	if (stat) {
		printf("[%s][%s][%d] stat = %d \n", FILE_LINE, stat);
	}
}
void GPRS_RESET_DISABLE(void) {
	int stat;
	stat = ioctl(g_uiIoControl, GPRS_RESET_C29, 0);
	if (stat) {
		printf("[%s][%s][%d] stat = %d \n", FILE_LINE, stat);
	}

}
void GPRS_RESET_ENABLE(void) {
	int stat;
	stat = ioctl(g_uiIoControl, GPRS_RESET_C29, 1);
	if (stat) {
		printf("[%s][%s][%d] stat = %d \n", FILE_LINE, stat);
	}
}
void GprsOff_xmz(void) {

	GPRS_POW_ON();
	OSTimeDly(OS_TICKS_PER_SEC / 2);
	GPRS_POW_OFF();

}

void GprsOn_xmz(void) {
	debug_debug(gDebugModule[GPRS_MODULE], "[%s][%s][%d] \n", FILE_LINE);
	GPRS_POW_OFF();
	//OSTimeDly(OS_TICKS_PER_SEC/2); 
	usleep(2000);
	GPRS_POW_ON();
	usleep(2000);

	GPRS_RESET_DISABLE();
	OSTimeDly(OS_TICKS_PER_SEC);
	GPRS_RESET_ENABLE();
	OSTimeDly(OS_TICKS_PER_SEC);
	GPRS_RESET_DISABLE();
	OSTimeDly(OS_TICKS_PER_SEC / 100);
}

void UGprsWriteStr(char *Str) {
	UpDevSend(UP_COMMU_DEV_GPRS, (uint8*) Str, strlen(Str));
	//UP_COMMU_DEV_232   UP_COMMU_DEV_GPRS
}

uint8 GetGprsRunSta_ModuId(void) {
	uint8 val;

	OS_ENTER_CRITICAL();
	val = gGprsRunSta.ModuId;
	OS_EXIT_CRITICAL();
	return val;
}

//������Ascii�ַ���ʾ��16������ת����1��uint8��HEX����ԭ���ݸ�λ�ַ���ǰ����λ�ַ��ں�
uint8 nAsciiToHex(char* chars) {
	uint8 i;
	uint8 val[2];
	for (i = 0; i < 2; i++) {
		if (chars[i] >= '0' && chars[i] <= '9') {
			val[i] = chars[i] - '0';
		} else if (chars[i] >= 'A' && chars[i] <= 'F') {
			val[i] = chars[i] - 'A' + 0x0A;
		}
		//else{}//�����Ƿ��ַ��ݴ����ⲿ��֤
	}
	return (val[0] * 16 + val[1]);

}

//����AsciiToHex�ķ�����,���ص�ASCII����ĸ����Ϊ��д
void nHexToAscii(uint8 data, char* chars) {
	uint8 i;
	uint8 val[2];
	val[0] = data / 16;
	val[1] = data % 16;
	for (i = 0; i < 2; i++) {
		if (val[i] <= 9) {
			chars[i] = val[i] + '0';
		} else if (val[i] >= 0x0A && val[i] <= 0x0F) {
			chars[i] = val[i] - 0x0A + 'A';
		}
	}
}

//�����4��Ascii��ʾ��10������ת���� uint16��HEX������"256"ת��Ϊ0x100
//�������Ϊ�ַ���
uint8 AsciiDec(char* str, uint16* result) {
	uint8 i, len;
	uint16 val, pow;
	len = strlen(str);

	if (len > 4) {
		return 0xff; //���ȳ���
	}
	val = 0;
	pow = 1;
	for (i = 0; i < len; i++) {
		if (str[len - 1 - i] < 0x30 || str[len - 1 - i] > 0x39) {
			printf("[%s][%s][%d] str[len-1-i]=%d\n", FILE_LINE,
					str[len - 1 - i]);
			return 0xfe; //�ַ��Ƿ�
		}
		val = val + (str[len - 1 - i] - 0x30) * pow;
		pow *= 10;
	}
	*result = val;
	return 0;
}

uint8 ReadCurSType(void) {
	uint8 val;

	OS_ENTER_CRITICAL();
	val = gGprsRunSta.SocketType;
	OS_EXIT_CRITICAL();
	return val;
}
/*
 ��������:�Ƚ��ַ�
 yangfei added note
 */
uint8 IfByte(uint8 data, char* HeadStr, char* OrHeadStr, uint8 p) {
	if (data == (uint8) (HeadStr[p])) {
		return 0; //ƥ����ѡ��Ӧ�ɹ�
	} else if (OrHeadStr != NULL) { //ƥ�䱸ѡ��Ӧ
		if (data == (uint8) (OrHeadStr[p])) {
			return 1; //ƥ�䱸ѡ��Ӧ�ɹ�
		} else {
			return 2; //ƥ��ʧ��
		}
	} else {
		return 2; //ƥ��ʧ��
	}

}
#if 0
uint8 GprsGetHead(uint8 device,char* HeadStr,char* OrHeadStr,uint16 OutTime,uint8 Find)
{

}
#endif
//Find: ���շ�ʽ, TRUE: һֱ������ȷ��HeadStr,ֱ����ʱ������ַ�������ĳ���ܴ��ֵ;
//FALSE: ��OutTime �յ���һ���ַ�����������HeadStr�ĳ��Ⱥ󼴷��سɹ���ʧ��

//���������м��ֽ�ʱ�ĳ�ʱ��ע����Ҫ��������һ��������յ��ĵ�һ���ֽڲ�ƥ��ʱ��C-D������������IPCLOSEʱ���յ�IPDATA����
//��ô�ں����ƥ������ж�IPCLOSE�ĵ�һ���ַ���ƥ�䣬�ͱ���˶��м��ַ���ƥ�䣬���������յ�������IPCLOSE ����ƥ��
//�м��ַ�����ƥ��'P'����������ġ�����ȴ�ʱ��Ӧ�ÿ���GPRSģ���ڷ���IPDATA����ӦIPCLOSEʱ���ܴ��ڵ��������м��ַ�
//֮���ʱ������ʱ��������˴�ֵҪ�ʵ����ô�һ��

//ע��: �˺�����HeadStr��OrHeadStr���ȳ�ʱ����,�����޸�
uint8 GprsGetHead(uint8 device, char* HeadStr, char* OrHeadStr, uint16 OutTime,
		uint8 Find) //������
{
	uint8 err, flag;
	uint8 data, headlen;
	uint16 p;
	uint32 n;

	headlen = strlen(HeadStr);
	if (headlen == 0 || headlen > HEAD_MAX_LEN) { //headlen �������0 ����HeadStr����ǿ�;HeadStr�������HEAD_MAX_LEN���ַ�
		return 0xff;
	}

	err = UpGetch(device, &data, OutTime);
	if (err) { //��ʱ
		debug_err(gDebugModule[MSIC_MODULE], "[%s][%s][%d] \n", FILE_LINE);
		return err;
	}

	n = 5000; ///��������������յ��ַ���
	flag = 0;
	p = 0;
	while (n > 1) {
		n--;
		//if(data!=(uint8)(HeadStr[p])){
		if (IfByte(data, HeadStr, OrHeadStr, p) >= 2) {
			if (Find == TRUE) {
				if (p > 0) { //����ƥ��HeadStr�ĵ�1���ֽ��Ժ���ֽڣ���ƥ��p���ֽ� ��ƥ���p+1���ֽڳ���
					p = 0;     //
					continue;     //���¿�ʼƥ��HeadStr�ĵ�1���ֽ�
				}
			} else {
				flag = 0xfe;     //��ʾʧ�ܣ�������Ҫ������headlen���ַ������Բ�������
				p++;
			}
		} else {
			p++;
		}
		if (p == headlen) {     //ƥ�����
			break;
		}
		err = UpGetch(device, &data, OutTime);     //OS_TICKS_PER_SEC/5
		if (err) {     //
			return 0xfe;//�����м��ַ�ʱ��ʱ  //ncq080819 ����Ҳ����������ƥ���1���ַ����������泬ʱʱ��Ӧ��ô�������ð죬���ڻ�������
		}
	}
	if (n == 1) {	//���Ҵ��������ƣ�Ӧ����ʧ��
		flag = 0xfd;
	}
	return flag;
}

uint8 CMD_AT_1(char* AtComd, char* AtRightAnsw, char* OrAtRightAnsw,
		uint16 OutTime) {
	uint8 err;

	UGprsWriteStr(AtComd);	//ע�⺯�����غ�Ӳ�����Ͳ�û�����
	err = GprsGetHead(UP_COMMU_DEV_AT, AtRightAnsw, OrAtRightAnsw, OutTime,
			TRUE);	//���շ��ؽ����OutTime����������ϲ����ã��еĻ�ϳ����罨����������
	return err;

}

uint8 CMD_AT_RP_1(char* AtComd, char* AtRightAnsw, char* OrAtRightAnsw,
		uint16 OutTime, uint32 rp) {
	uint32 i;
	uint8 err;
	for (i = 0; i < 1 + rp; i++) {	//rp:���Դ�����0 Ϊ�����ԣ�ֻ��һ��

		err = CMD_AT_1(AtComd, AtRightAnsw, OrAtRightAnsw, OutTime);
		if (err == 0) {	//�ɹ�
			break;
		}
		OSTimeDly(2 * OS_TICKS_PER_SEC);	//ʧ������ǰ����ʱ
		//UpQueueFlush(UP_COMMU_DEV_AT);
	}
	return err;

}

//ʹ���豸UP_COMMU_DEV_AT���ʲ������룬����ʹ���豸UP_COMMU_DEV_AT�ĺ����������䲢����
//ATEMode:�Ƿ�����ԣ�TRUE�������ԣ� FLASE����������
uint8 CMD_AT(char* AtComd, char* AtRightAnsw, char* OrAtRightAnsw,
		uint16 OutTime, uint8 ATEMode) {
	uint8 err;

	UGprsWriteStr(AtComd);		//ע�⺯�����غ�Ӳ�����Ͳ�û�����

	OSTimeDly(OS_TICKS_PER_SEC / 8);
#if 0
	if(ATEMode==TRUE) {
		err=GprsGetHead(UP_COMMU_DEV_AT,AtComd,NULL,OS_TICKS_PER_SEC/8,TRUE);//���ջ��ԣ���ʱ���ý�С
		//����������ʱ��OS_TICKS_PER_SEC/20�Ļ����յ�һ�������ֽھͳ�ʱ������Ϊ���͵��ֽڶ࣬����ռȥ�˺ܶ�ʱ��
		//����ע������ĳ�ʱ����Ӳ�����͵�ʱ�����ڡ����⣬�м��ַ��ĳ�ʱҪע��������GprsGetHead�����˵��
		if(err) {
			debug_err(gDebugModule[GPRS_MODULE],"AtComd err=%d ",err);
			printf("[%s][%s][%d] \n",FILE_LINE);
			return 0xff;
		}
	}
#endif
	//err=GprsGetHead(UP_COMMU_DEV_AT,AtRightAnsw,OrAtRightAnsw,OutTime,FALSE);//���շ��ؽ����OutTime����������ϲ����ã��еĻ�ϳ����罨����������
	err = GprsGetHead(UP_COMMU_DEV_AT, AtRightAnsw, OrAtRightAnsw, OutTime,
			TRUE);/*yangfei modified */
	if (err) {
		debug_err(gDebugModule[MSIC_MODULE],
				"[%s][%s][%d] AtRightAnsw err=%d\n", FILE_LINE, err);
	}
	return err;

}

//ʹ���豸UP_COMMU_DEV_AT���ʲ������룬����ʹ���豸UP_COMMU_DEV_AT�ĺ����������䲢����
//ATEMode:�Ƿ�Ĵ����ԣ�TRUE�������ԣ� FLASE����������
uint8 CMD_AT_RP(char* AtComd, char* AtRightAnsw, char* OrAtRightAnsw,
		uint16 OutTime, uint32 rp, uint8 ATEMode) {
	uint32 i;
	uint8 err;
	for (i = 0; i < 1 + rp; i++) {		//rp:���Դ�����0 Ϊ�����ԣ�ֻ��һ��

		err = CMD_AT(AtComd, AtRightAnsw, OrAtRightAnsw, OutTime, ATEMode);
		if (err == 0) {		//�ɹ�
			break;
		} else {
			debug_err(gDebugModule[MSIC_MODULE], "[%s][%s][%d] CMD_AT err\n",
					FILE_LINE);
			OSTimeDly(2 * OS_TICKS_PER_SEC);		//ʧ������ǰ����ʱ
		}
	}
	return err;
}

//============================================================================
//���ڽ����м䲿�ֳ��Ȳ��̶���AT����Ӧ��Ĳ��̶����Ȳ��֣�ƥ��β�ַ����˳�����������С�Ŀ��ܳ��Ⱥ����Ŀ��ܳ��ȷ���err
//�����������ڽ����м䲿�֣������ַ��ĳ�ʱʱ��̶�ΪOS_TICKS_PER_SEC/5
uint16 GprsGetViaTail(uint8 device, char* data, char Tail, char MinLen,
		uint16 MaxLen, uint8* err) {
	uint16 n;
	uint8 tmp;
	n = 0;
	do {
		if (n >= MaxLen) {
			*err = 0xff;
			return n;
		}
		*err = UpGetch(device, &tmp, 4 * OS_TICKS_PER_SEC);		//�����ַ��ĳ�ʱʱ��̶�Ϊ
		if (*err != OS_ERR_NONE) {		//��ʱ
			return n;
		}
		data[n++] = tmp;
		data[n] = '\0';		//���ڵ��Թ۲�

	} while (tmp != Tail);

	if (n < MinLen) {
		*err = 0xfe;
	}
	return n;
}

uint16 GprsGetViaTail_Ext(uint8 device, char* data, char Tail, char MinLen,
		uint16 MaxLen, uint8* err) {
	uint16 n;
	uint8 tmp;
	n = 0;
	do {
		if (n >= MaxLen) {
			*err = 0xff;
			return n;
		}
		*err = UpGetch(device, &tmp, 5 * OS_TICKS_PER_SEC);		//�����ַ��ĳ�ʱʱ��̶�Ϊ
		if (*err != OS_ERR_NONE) {		//��ʱ
			return n;
		}
		data[n++] = tmp;
		data[n] = '\0';		//���ڵ��Թ۲�

	} while (tmp != Tail);
	/*yangfei added*/
	data[n - 1] = '\0';
	if (n < MinLen) {
		*err = 0xfe;
	}
	return n;
}
//char Ata_IPCLOSE[]="\r\n%IPCLOSE: 1,0,0\r\n\r\nOK\r\n";
/*uint8 GprsGetIPCLOSE(uint16 OutTime)
 {
 uint8 err;
 err=GprsGetHead(UP_COMMU_DEV_AT,Ata_IPCLOSE,OutTime,TRUE);
 return err;
 
 }*/
//ʹ���豸UP_COMMU_DEV_AT���ʲ������룬����ʹ���豸UP_COMMU_DEV_AT�ĺ����������䲢����
//�رջ��Է�ʽ��ʹ��
uint8 CMD_IpClose_hw(void) {
	uint8 err;
	UGprsWriteStr(Ats_IPCLOSE);
	err = GprsGetHead(UP_COMMU_DEV_AT, Ata_IPCLOSE, NULL, OS_TICKS_PER_SEC,
			TRUE);
	if (err) {
		return TRUE;
	} else {
		return FALSE;
	}
}
uint8 CMD_IpClose_xmz(void) {
	uint8 err;
	int ret, value = 0;
	ret = sem_getvalue(&GprsXmzSem, &value);
	if (ret) {
		printf("[%s][%s][%d]value=%d \n", FILE_LINE, ret);
	}
	debug_info(gDebugModule[GPRS_MODULE], "[%s][%s][%d]value=%d \n", FILE_LINE,
			value);
	//printf("[%s][%s][%d] sem_wait(&GprsXmzSem)\n",FILE_LINE);
	sem_wait(&GprsXmzSem);
	//printf("[%s][%s][%d] sem_wait(&GprsXmzSem) ok\n",FILE_LINE);
	//OSTimeDly(OS_TICKS_PER_SEC/10);
	err = CMD_AT_RP(Ats_SISI, Ata_SISI_1, NULL, OS_TICKS_PER_SEC * 2, 0, TRUE);
	if (err) {
		sem_post(&GprsXmzSem);
		debug_err(gDebugModule[GPRS_MODULE], "[%s][%s][%d] err=%d !\n",
				FILE_LINE, err);
		return TRUE;
	}

	err = GprsGetHead(UP_COMMU_DEV_AT, Ata_OK, NULL, OS_TICKS_PER_SEC * 2,
			TRUE);
	if (err) {
		sem_post(&GprsXmzSem);
		debug_err(gDebugModule[GPRS_MODULE], "[%s][%s][%d] err=%d !\n",
				FILE_LINE, err);
		return TRUE;
	} else {
		sem_post(&GprsXmzSem);
		return FALSE;
	}

}
uint8 CMD_IpClose(void) {
	if (GetGprsRunSta_ModuId() == MODU_ID_HW) {
		return CMD_IpClose_hw();
	} else {
		return CMD_IpClose_xmz();
	}
}

//char AtaHead_CSQ[]="\r\n+CSQ: ";
//char Ata_CSQ[]="\r\n+CSQ: 27,99 \r\n\r\nOK\r\n";
//ʹ���豸UP_COMMU_DEV_AT���ʲ������룬����ʹ���豸UP_COMMU_DEV_ATIPD�ĺ����������䲢����
//�رջ��Է�ʽ��ʹ��
uint8 GprsGetCSQ(uint16 OutTime, char* pCsq) {
	uint8 err;
	char CsqStr[12];
	uint16 len;
	err = GprsGetHead(UP_COMMU_DEV_AT, AtaHead_CSQ, NULL, OS_TICKS_PER_SEC,
			TRUE);
	if (err == 0) {
		len = GprsGetViaTail(UP_COMMU_DEV_AT, CsqStr, '\n', 7, 9, &err);
		if (err) {
			printf("[%s][%s][%d] err=%d \n", FILE_LINE, err);
			return err;
		}

		//err=GprsGetHead(UP_COMMU_DEV_AT,Ata_OK,NULL,OS_TICKS_PER_SEC/5,FALSE);/*yf modified*/
		err = GprsGetHead(UP_COMMU_DEV_AT, Ata_OK, NULL, OS_TICKS_PER_SEC,
				FALSE);
		if (err) {
			printf("[%s][%s][%d] err=%d \n", FILE_LINE, err);
			return err;
		}
		if (CsqStr[len - 2] != '\r') {
			return 0xff;
		}
		//'\r'����Ŀո�Ͳ��ж���
		CsqStr[len - 3] = '\0';
		strcpy(pCsq, &CsqStr[1]);
	} else {
		return err;
	}
	return 0;
}

//ʹ���豸UP_COMMU_DEV_AT���ʲ������룬����ʹ���豸UP_COMMU_DEV_ATIPD�ĺ����������䲢����
//�رջ��Է�ʽ��ʹ��
uint8 CMD_Csq(uint8* csq)	//������/��Ϊģ�鹲��,����GprsXmzSem�Ի�Ϊģ����˵�ǲ���Ҫ��,������������Ҫ
{
	uint8 err;
	uint16 val;
	char CsqStr[12];

	sem_wait(&GprsXmzSem);
	//printf("[%s][%s][%d] sem_wait(&GprsXmzSem) ok\n",FILE_LINE);
	//OSTimeDly(OS_TICKS_PER_SEC/10);

	UGprsWriteStr(Ats_CSQ);
	err = GprsGetCSQ(OS_TICKS_PER_SEC, CsqStr);
	if (err == 0) {
		if (CsqStr[1] == ',') {
			CsqStr[1] = '\0';
		} else if (CsqStr[2] == ',') {
			CsqStr[2] = '\0';
		} else {
			sem_post(&GprsXmzSem);
			return 0xf0;
		}
		err = AsciiDec(CsqStr, &val);
		*csq = (uint8) val;
	}
	sem_post(&GprsXmzSem);
	return err;

}

uint8 CMD_smso(void) {
	uint8 err;

	err = CMD_AT_RP(Ats_SMSO, Ata_SMSO, NULL, 3 * OS_TICKS_PER_SEC, 0, TRUE);
	if (err) {
		return err;
	}

	err = GprsGetHead(UP_COMMU_DEV_AT, Ata_OK, NULL, OS_TICKS_PER_SEC / 2,
			FALSE);
	return err;

}

//char AtaHead_IPDATA[]="%IPDATA:";
//%IPDATA:25,"68450045006841FFFFFFFFFF84F10000801E3117070006A416"\r\n
//��ռ�豸UP_COMMU_DEV_ATIPD
uint8 GprsGetIPDATA_xmz(char* ipdata, uint16 OutTime, uint16* StrLen) {
	uint8 err;

	ipdata = ipdata;
	StrLen = StrLen;

	err = GprsGetHead(UP_COMMU_DEV_ATIPD, Ata_URC_SISR, NULL, OutTime, TRUE);

	return err;

}

/*
 get data from USART3RecQueue_At UP_COMMU_DEV_AT

 jh:����
 */
uint8 GprsGetIPDATA_jh(char* ipdata, uint16 OutTime, uint16* StrLen) {
	uint8 err;
	char lenstr[8];
	uint16 i, Len;
	OutTime = OutTime;
	sem_wait(&GprsXmzSem);
	/*begin:yangfei added for  QueueFlush 2013-05-27*/
	UpQueueFlush(UP_COMMU_DEV_AT);
	/*end:yangfei added for 2013-05-27*/
	UGprsWriteStr(Ats_SISR);
#ifdef  GPRS_ECHO
	err = GprsGetHead(UP_COMMU_DEV_AT, Ats_SISR, NULL, 2 * OS_TICKS_PER_SEC,
			TRUE);
	if (err) {
		sem_post(&GprsXmzSem);
		debug_err(gDebugModule[GPRS_MODULE], "[%s][%s][%d] Ats_SISR err=%d\n",
				FILE_LINE, err);
		return err;
	}
#endif

	err = GprsGetHead(UP_COMMU_DEV_AT, Ata_SISR_m, NULL, 2 * OS_TICKS_PER_SEC,
			TRUE);
	if (err) {
		sem_post(&GprsXmzSem);
		debug_err(gDebugModule[GPRS_MODULE], "[%s][%d]Ata_SISR_m err=%d\n",
				__FUNCTION__, __LINE__, err);
		return err;
	}

	Len = GprsGetViaTail(UP_COMMU_DEV_AT, lenstr, '\n', 3, 8, &err);
	if (err) {
		sem_post(&GprsXmzSem);
		debug_err(gDebugModule[GPRS_MODULE], "%s %d err=%d\n", __FUNCTION__,
				__LINE__, err);
		return err;
	}
	if (lenstr[Len - 2] != '\r') {
		sem_post(&GprsXmzSem);
		debug_err(gDebugModule[GPRS_MODULE], "%s %d err=%d\n", __FUNCTION__,
				__LINE__, err);
		return 0xff;
	}
	lenstr[Len - 2] = '\0';

	err = AsciiDec(lenstr, &Len);
	if (err) {
		sem_post(&GprsXmzSem);
		debug_err(gDebugModule[GPRS_MODULE], "%s %d err=%d\n", __FUNCTION__,
				__LINE__, err);
		*StrLen = 0;
		return 0xfe;
	}

	if (Len > MAX_REC_MC52i_BYTENUM) {
		return 0xdd;
	}
	for (i = 0; i < Len; i++) {
		//err=UpGetch(UP_COMMU_DEV_AT,(uint8*)(&ipdata[i]),4*OS_TICKS_PER_SEC);/*4s will outtime*/
		err = UpGetch(UP_COMMU_DEV_AT, (uint8*) (&ipdata[i]),
				10 * OS_TICKS_PER_SEC);
		if (err) {
			sem_post(&GprsXmzSem);
			debug_err(gDebugModule[GPRS_MODULE], "[%s][%d] len=%d err=%d\n",
					__FUNCTION__, __LINE__, Len, err);
			return 0xf0;
		}
	}

	*StrLen = Len;
	sem_post(&GprsXmzSem);
	return 0;
}

//extern uint32 gprs_sfai1,gprs_sfai2;
uint8 GprsIPSEND_xmz(uint8* ipdata, uint16 len, uint16* pBufNum) {
	char buf[8];
	uint8 err;
	uint8 RetryCount = 0;
	uint16 cnfWriteLength;
	char lenstr[6];

	if (len > MAX_IPSEND_MC52i_BYTENUM) {
		return 0xdd;
	}
	/*begin:yangfei added 2013-02-28 for ������Ϊ0ʱ����û������*/
	if (len == 0) {
		debug_err(gDebugModule[GPRS_MODULE],
				"[%s][%s][%d] GprsIPSEND_xmz len=0\n", FILE_LINE);
		return 0xd1;
	}
	/*end:yangfei added 2013-02-28  */
	//debug_info(gDebugModule[XML_MODULE],"[%s][%s][%d] GprsXmzSem\n",FILE_LINE);
	sem_wait(&GprsXmzSem);
	//debug_info(gDebugModule[XML_MODULE],"[%s][%s][%d] GprsXmzSem ok\n",FILE_LINE);
	OSTimeDly(OS_TICKS_PER_SEC / 10);
	strcpy(Ats_SISW, Ats_SISW_h);

	gpHex16ToStr(len, buf);
	strcat(Ats_SISW, buf);
	strcat(Ats_SISW, over123);

	START: UGprsWriteStr(Ats_SISW);
#ifdef  GPRS_ECHO
	err = GprsGetHead(UP_COMMU_DEV_AT, Ats_SISW, NULL, 5 * OS_TICKS_PER_SEC,
			TRUE);
	if (err) {
		debug_err(gDebugModule[GPRS_MODULE],
				"[%s][%s][%d]Ats_SISW get head Failure, The err is %d!\n",
				FILE_LINE, err);
		sem_post(&GprsXmzSem);
		return err;
	}
#endif
	err = GprsGetHead(UP_COMMU_DEV_AT, Ata_SISW_m, NULL, 2 * OS_TICKS_PER_SEC,
			TRUE);
	if (err) {
		debug_err(gDebugModule[GPRS_MODULE],
				"[%s][%s][%d]Ats_SISW_m get head Failure, The err is %d!\n",
				FILE_LINE, err);
		sem_post(&GprsXmzSem);
		return err;
	}
	/*begin:yangfei added 2013-05-30 for ^SISW: 1, 0, 20 GPRS can send number lower then expect*/
	GprsGetViaTail_Ext(UP_COMMU_DEV_AT, lenstr, ',', 1, 6, &err);
	if (err) {
		sem_post(&GprsXmzSem);
		debug_err(gDebugModule[GPRS_MODULE], "%s %d err=%d\n", __FUNCTION__,
				__LINE__, err);
		return err;
	}
	err = AsciiDec(lenstr, &cnfWriteLength);
	if (err) {
		sem_post(&GprsXmzSem);
		debug_err(gDebugModule[GPRS_MODULE], "%s %d err=%d\n", __FUNCTION__,
				__LINE__, err);
		return 0xfe;
	}
	if (cnfWriteLength < len) {
		debug_err(gDebugModule[GPRS_MODULE],
				"[%s][%s][%d] cnfWriteLength=%d len=%d\n", FILE_LINE,
				cnfWriteLength, len);
		OSTimeDly(OS_TICKS_PER_SEC);
		if ((RetryCount++) > 3) {
			sem_post(&GprsXmzSem);
			return 0xf0;
		}
		goto START;
	}
	/*end:yangfei added 2013-05-30 for*/
	UpDevSend(UP_COMMU_DEV_GPRS, ipdata, len);

	err = GprsGetHead(UP_COMMU_DEV_AT, Ata_OK, NULL, 2 * OS_TICKS_PER_SEC,
			TRUE);
	if (err) {
		debug_err(gDebugModule[GPRS_MODULE],
				"[%s][%s][%d]Ata_OK get head Failure, The err is %d!\n",
				FILE_LINE, err);
		sem_post(&GprsXmzSem);
		return err;
	}
	*pBufNum = 15;
	debug_debug(gDebugModule[GPRS_MODULE], "[%s][%s][%d] sem_post GprsXmzSem\n",
			FILE_LINE);
	sem_post(&GprsXmzSem);
	return 0;
}

uint8 GprsIPSEND(uint8* ipdata, uint16 len, uint16* pBufNum) {
	if (GetGprsRunSta_ModuId() == MODU_ID_XMZ) {
		return GprsIPSEND_xmz(ipdata, len, pBufNum);
	}
	return 1;
}

void InitGprsRunSta(void) {

	OS_ENTER_CRITICAL();
	///////gGprsRunSta.ModuId=0;
	gGprsRunSta.Ready = FALSE;
	gGprsRunSta.Connect = FALSE;
	gGprsRunSta.SocketType = 0;	//0:TCP;1:UDP  //��ReadyΪTRUE���京�����Ч
	gGprsRunSta.IpCloseNum = 0;
	gGprsRunSta.IpSendFailNum = 0;
	gGprsRunSta.IpSendRemnBufNum = 0;
	gGprsRunSta.Csq = 0;
	//gGprsRunSta.FrmRecTimes=0;
	//gGprsRunSta.FrmSndTimes=0;
	gGprsRunSta.RecDog = 0;
	gGprsRunSta.SndDog = 0;
	OS_EXIT_CRITICAL();
}

//��ȡGPRS����״̬
void ReadGprsRunSta(GPRS_RUN_STA* Sta) {

	OS_ENTER_CRITICAL();
	memcpy((uint8*) Sta, (uint8*) (&gGprsRunSta), sizeof(GPRS_RUN_STA));
	OS_EXIT_CRITICAL();
}
//����GPRS����״̬
void UpdGprsRunSta(GPRS_RUN_STA* Sta) {

	OS_ENTER_CRITICAL();
	memcpy((uint8*) (&gGprsRunSta), (uint8*) Sta, sizeof(GPRS_RUN_STA));
	OS_EXIT_CRITICAL();
}
void UpdGprsRunSta_ModuId(uint8 val) {

	OS_ENTER_CRITICAL();
	gGprsRunSta.ModuId = val;
	OS_EXIT_CRITICAL();
}
void UpdGprsRunSta_Stype(uint8 val) {

	OS_ENTER_CRITICAL();
	gGprsRunSta.SocketType = val;
	OS_EXIT_CRITICAL();
}
void UpdGprsRunSta_Cont(uint8 val) {

	OS_ENTER_CRITICAL();
	gGprsRunSta.Connect = val;
	OS_EXIT_CRITICAL();
}
void UpdGprsRunSta_Ready(uint8 val) {

	OS_ENTER_CRITICAL();
	gGprsRunSta.Ready = val;
	OS_EXIT_CRITICAL();
}
void UpdGprsRunSta_IpCloseNum(uint8 val) {

	OS_ENTER_CRITICAL();
	gGprsRunSta.IpCloseNum = val;
	OS_EXIT_CRITICAL();
}
void UpdGprsRunSta_IpSendRemnBufNum(uint8 val) {

	OS_ENTER_CRITICAL();
	gGprsRunSta.IpSendRemnBufNum = val;
	OS_EXIT_CRITICAL();
}
void UpdGprsRunSta_Csq(uint8 val) {

	OS_ENTER_CRITICAL();
	gGprsRunSta.Csq = val;
	OS_EXIT_CRITICAL();
}

void UpdGprsRunSta_AddIpCloseNum(void) {

	OS_ENTER_CRITICAL();
	gGprsRunSta.IpCloseNum++;
	OS_EXIT_CRITICAL();
}
void UpdGprsRunSta_AddIpSendFailNum(void) {
	OS_ENTER_CRITICAL();
	gGprsRunSta.IpSendFailNum++;
	OS_EXIT_CRITICAL();
}
void UpdGprsRunSta_FeedIpSendFailNum(void) {
	OS_ENTER_CRITICAL();
	gGprsRunSta.IpSendFailNum = 0;
	OS_EXIT_CRITICAL();
}

void UpdGprsRunSta_FeedRecDog(void) {
	OS_ENTER_CRITICAL();
	gGprsRunSta.RecDog = 0;
	OS_EXIT_CRITICAL();
}
void UpdGprsRunSta_AddRecDog(void) {
	OS_ENTER_CRITICAL();
	gGprsRunSta.RecDog++;
	OS_EXIT_CRITICAL();
}

void UpdGprsRunSta_FeedSndDog(void) {
	OS_ENTER_CRITICAL();
	gGprsRunSta.SndDog = 0;
	OS_EXIT_CRITICAL();
}
void UpdGprsRunSta_AddSndDog(void) {
	OS_ENTER_CRITICAL();
	gGprsRunSta.SndDog++;
	OS_EXIT_CRITICAL();
}
void UpdGprsRunSta_AddFrmRecTimes(uint32 n) {
	OS_ENTER_CRITICAL();
	gGprsRunSta.FrmRecTimes++;
	gGprsRunSta.FrmRecBs += n;
	OS_EXIT_CRITICAL();
}
void UpdGprsRunSta_AddFrmSndTimes(uint32 n) {
	OS_ENTER_CRITICAL();
	gGprsRunSta.FrmSndTimes++;
	gGprsRunSta.FrmSndBs += n;
	OS_EXIT_CRITICAL();
}

void GetSocketType(void) {
	UpdGprsRunSta_Stype(SOCKET_TCP);
}

void ProSocketType(void) {
	uint8 SocketType;
	SocketType = ReadCurSType();

	if (SocketType == 0) {	//TCP
		At_IPOPEN[11] = 'T';
		At_IPOPEN[12] = 'C';
		At_IPOPEN[13] = 'P';
	} else {	//UDP
		At_IPOPEN[11] = 'U';
		At_IPOPEN[12] = 'D';
		At_IPOPEN[13] = 'P';
	}
}

//��ȡGPRS�ź�ǿ��  ����ֵ��ΧΪ0(С)--4(��)���ֱ��ʾGPRS��ǿ�ȴ�С
uint8 GetGprsCSQ(void) {
	uint8 GprsSignal;
	GPRS_RUN_STA Sta;

	ReadGprsRunSta(&Sta);
	if ((Sta.Csq > 0) && (Sta.Csq < 32)) {
		GprsSignal = (Sta.Csq / 8) + 1;
	} else {
		GprsSignal = 0;
	}
	return GprsSignal;
}

void ReadCsq(void) {
	uint8 csq;
	if (CMD_Csq(&csq) == 0) {
		UpdGprsRunSta_Csq(csq);
	}
}

uint8 GprsTalk(uint32 rp) {
	uint8 err;
	debug_debug(gDebugModule[GPRS_MODULE], "[%s][%s][%d] \n", FILE_LINE);
	err = CMD_AT_RP(Ats_AT, Ata_OK, NULL, OS_TICKS_PER_SEC, rp, TRUE);
	if (err) {
		debug_err(gDebugModule[GPRS_MODULE],
				"[%s][%s][%d] Ats_AT CMD Err Module donot exist!\n", FILE_LINE);

		OSTimeDly(OS_TICKS_PER_SEC / 2);
	}
	return err;
}

uint8 ModelTalkIn(void) {
	uint8 err;
	uint8 n;

	OSTimeDly(5 * OS_TICKS_PER_SEC);

	err = GprsTalk(5);
	if (err == 0) {
		return 0;
	}

	//�Ựʧ��,��������ģ����п����������»Ự
	n = 3;
	do {
		GprsOff_xmz();
		OSTimeDly(10 * OS_TICKS_PER_SEC);

		GprsOn_xmz();
		OSTimeDly(5 * OS_TICKS_PER_SEC);
		err = GprsTalk(3);
		if (err == 0) {
			return 0;
		}
		if (n-- == 0) {
			break;
		}
	} while (err);

	return 0xff;
}

uint8 ModelIdentify(void) {
	uint8 err;
	uint8 n;
	uint8 nn;
	//return MODU_ID_XMZ;
	printf("[%s][%s][%d] \n", FILE_LINE);
	n = 5;
	gplp: nn = 10;
	while (ModelTalkIn()) {
		nn--;
		if (nn == 0) {
			/*begin:yangfei deleted 2013-08-28 for GPRS Module һֱ�ڷ���AT���ж�ģ���Ƿ���� */
			//return MODU_ID_UNKNOWN;
			/*end:yangfei deleted 2013-08-28 for GPRS Module  */
		}
		OSTimeDly(25 * OS_TICKS_PER_SEC);

	}
	OSTimeDly(OS_TICKS_PER_SEC / 2);
	printf("[%s][%s][%d] \n", FILE_LINE);
	//�Ự�Ѿ��ɹ�,��ȡģ��ID====
	err = CMD_AT_RP(Ats_CGMM, Ata_CGMM_xmz, NULL, OS_TICKS_PER_SEC, 2, TRUE);
	if (err == 0) {
		//GprsCtrInit_Xmz();
		OSTimeDly(5 * OS_TICKS_PER_SEC);

		return MODU_ID_XMZ;
	}

	/*begin:yangfei added 2013-02-23 for GPRS Module  compatible*/
	err = CMD_AT_RP(Ats_CGMM, Ata_CGMM_xmz_R3, NULL, OS_TICKS_PER_SEC, 2, TRUE);
	if (err == 0) {
		//GprsCtrInit_Xmz();
		OSTimeDly(5 * OS_TICKS_PER_SEC);

		return MODU_ID_XMZ;
	}
	/*end:yangfei added 2013-02-23*/

	//====�����ʧ��
	//GprsCtrInit_Xmz();
	OSTimeDly(5 * OS_TICKS_PER_SEC);

	if (n == 0) {
		return MODU_ID_UNKNOWN;
	}
	goto gplp;

}

void GprsRestart_xmz(void) {
	uint8 err;
	err = CMD_AT_RP(Ats_AT, Ata_OK, NULL, OS_TICKS_PER_SEC, 3, TRUE);
	printf("[%s][%s][%d] \n", FILE_LINE);
	debug_debug(gDebugModule[GPRS_MODULE],
			"WARNING: <GprsRestart_xmz> GPRS Modul Shutdown!\n");
	OSTimeDly(OS_TICKS_PER_SEC);
	if (err == 0) {	//��Ϊ����GPRSģ�鴦�ڿ���״̬
		GprsOff_xmz();	//�����ػ�
		printf("WARNING: <GprsRestart_xmz> GPRS Modul Power Off!\n");
		OSTimeDly(15 * OS_TICKS_PER_SEC);
	}
	debug_debug(gDebugModule[GPRS_MODULE],
			"INFO: <GprsRestart_xmz> GPRS Modul Power On, And ReStart!\n");
	GprsOn_xmz();
	OSTimeDly(2 * OS_TICKS_PER_SEC);
}

//ʹ���豸UP_COMMU_DEV_AT���ʲ������룬����ʹ���豸UP_COMMU_DEV_AT�ĺ����������䲢����
//ֱ����UART1 ���ͣ��˺����ɹ�ǰ��������������UART1 ���ͣ���ͨ��ȫ�ֽṹ��gGprsRunStaʵ��
uint8 GprsInit_xmz(void) {
	uint8 err;
	//uint8 len=0;
	//char temp[10] = {0x00};
	uint32 i, n = 0;
	printf("[%s][%s][%d] \n", FILE_LINE);

	//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	while (1) {
		n++;
		if (n > 10) {
			for (i = 0; i < 12; i++) {

				OSTimeDly(10 * OS_TICKS_PER_SEC);
				ReadCsq();
			}
			//n=1;			//for test
		}
		if (gGprsFirst == FALSE) {
			GprsRestart_xmz();
		}
		//�������˵������������һ�β�������GPRSģ��ʱ�����ֺܶ�ʱ�����淢AT��ʧ��1�Σ���ϸ���Է��֣�
		//ģ�鿪���󣨷���^SYSSTART��5�����ң������AT����������Ӧ�����ٷ�AT�����1������Ӧ��
		//��ˣ�����ڶ��ν���AT����Ӧ�ɹ�ʵ��������һ�ε��ͺ��Ӧ�����ڶ��ε�AT����Ӧ�����������
		//����û�й�ϵ���ᱻ�����IPR����Ե������������û������ԵĴ���

		gGprsFirst = FALSE;

		UpQueueFlush(UP_COMMU_DEV_AT); //�豸UP_COMMU_DEV_AT�Ķ��У����

		err = CMD_AT_RP(Ats_AT, Ata_OK, NULL, OS_TICKS_PER_SEC, 3, TRUE);
		OSTimeDly(OS_TICKS_PER_SEC);
		if (err) {

			debug_err(gDebugModule[GPRS_MODULE],
					"WARNING: <GprsInit_xmz> Ats_AT CMD Err!\n");
			continue;
		}
#ifndef GPRS_ECHO
		/*yangfei added �رջ���2013-05-24*/
		err=CMD_AT_RP(Ats_ATE0,Ata_OK,NULL,OS_TICKS_PER_SEC,3,FALSE);
		OSTimeDly(OS_TICKS_PER_SEC);
		if(err) {

			debug_err(gDebugModule[GPRS_MODULE], "WARNING: <GprsInit_xmz> Ats_AT CMD Err!\n");
			continue;
		}
#endif

		debug_info(gDebugModule[GPRS_MODULE],
				"INFO: <GprsInit_xmz> Ats_AT CMD OK!\n");

		err = CMD_AT_RP(Ats_IPR_0, Ata_OK, NULL, OS_TICKS_PER_SEC, 2, FALSE);
		OSTimeDly(OS_TICKS_PER_SEC / 2);
		if (err) {
			debug_err(gDebugModule[GPRS_MODULE],
					"WARNING: <GprsInit_xmz> Ats_IPR_0 CMD Err!\n");
			OSTimeDly(OS_TICKS_PER_SEC / 2);
		}

		err = CMD_AT_RP(Ats_SSYNC_c, Ata_SSYNC_c, NULL, OS_TICKS_PER_SEC, 2,
				FALSE);
		OSTimeDly(OS_TICKS_PER_SEC / 2);
		if (err) {
			debug_info(gDebugModule[GPRS_MODULE],
					"WARNING: <GprsInit_xmz> Ats_SSYNC_c CMD Err!\n");
			err = CMD_AT_RP(Ats_SSYNC, Ata_OK, NULL, OS_TICKS_PER_SEC * 2, 2,
					FALSE);
			debug_info(gDebugModule[GPRS_MODULE],
					"WARNING: <GprsInit_xmz> Try CMD Ats_SSYNC CMD!\n");
			OSTimeDly(OS_TICKS_PER_SEC / 2);
		}
		debug_info(gDebugModule[GPRS_MODULE],
				"INFO: <GprsInit_xmz> Ats_SSYNC_c CMD OK!\n");

		err = CMD_AT_RP(Ats_CPIN, Ata_CPIN, NULL, OS_TICKS_PER_SEC * 4, 20,
				FALSE);
		OSTimeDly(OS_TICKS_PER_SEC);
		if (err) {
			debug_info(gDebugModule[GPRS_MODULE],
					"WARNING: <GprsInit_xmz> Ats_CPIN CMD Err!\n");
			continue;
		}
		debug_info(gDebugModule[GPRS_MODULE],
				"INFO: <GprsInit_xmz> Ats_CPIN CMD OK!\n");

		ReadCsq();
		OSTimeDly(2 * OS_TICKS_PER_SEC);
		ReadCsq();
		OSTimeDly(2 * OS_TICKS_PER_SEC);
		ReadCsq();
		OSTimeDly(2 * OS_TICKS_PER_SEC);

		err = CMD_AT_RP(Ats_CREG, Ata_CREG, Ata_CREG_Or, OS_TICKS_PER_SEC * 2,
				30, FALSE);
		OSTimeDly(OS_TICKS_PER_SEC);
		if (err) {
			debug_info(gDebugModule[GPRS_MODULE],
					"WARNING: <GprsInit_xmz> Ats_CREG CMD Err!\n");
			continue;
		}
		debug_info(gDebugModule[GPRS_MODULE],
				"INFO: <GprsInit_xmz> Ats_CREG CMD OK!\n");

		ReadCsq();
		OSTimeDly(OS_TICKS_PER_SEC);

		err = CMD_AT_RP(Ats_CGDCONT, Ata_OK, NULL, 2 * OS_TICKS_PER_SEC, 15,
				FALSE);
		OSTimeDly(OS_TICKS_PER_SEC);
		if (err) {
			debug_info(gDebugModule[GPRS_MODULE],
					"WARNING: <GprsInit_xmz> Ats_CGDCONT CMD Err!\n");
			continue;
		}
		debug_info(gDebugModule[GPRS_MODULE],
				"INFO: <GprsInit_xmz> Ats_CGDCONT CMD OK!\n");

		err = CMD_AT_RP(Ats_CGATT, Ata_OK, NULL, 10 * OS_TICKS_PER_SEC, 3,
				FALSE);
		if (err) {
			debug_err(gDebugModule[GPRS_MODULE], "[%s][%s][%d] \n", FILE_LINE);
		}
		debug_info(gDebugModule[GPRS_MODULE],
				"INFO: <GprsInit_xmz> Ats_CGATT CMD OK!\n");

		err = CMD_AT_RP(Ats_CGREG, Ata_CGREG, Ata_CGREG_Or,
				OS_TICKS_PER_SEC * 2, 5, FALSE);
		OSTimeDly(OS_TICKS_PER_SEC);
		if (err) {
			debug_info(gDebugModule[GPRS_MODULE],
					"WARNING: <GprsInit_xmz> Ats_CGREG CMD Err!\n");
			continue;
		}
		debug_info(gDebugModule[GPRS_MODULE],
				"INFO: <GprsInit_xmz> Ats_CGREG CMD OK!\n");
		/*begin:yangfei added 2013-02-26 for*/
		err = CMD_AT_RP(Ats_CGREG_1, Ata_OK, NULL, OS_TICKS_PER_SEC * 2, 3,
				FALSE);
		OSTimeDly(OS_TICKS_PER_SEC);
		/*end:yangfei added 2013-02-26*/
		ReadCsq();
		OSTimeDly(OS_TICKS_PER_SEC);

		GetSocketType();
		//ProSocketType();
		err = CMD_AT_RP(Ats_SICS_conType, Ata_OK123, NULL, OS_TICKS_PER_SEC, 3,
				FALSE);
		OSTimeDly(OS_TICKS_PER_SEC);
		if (err) {
			debug_info(gDebugModule[GPRS_MODULE],
					"WARNING: <GprsInit_xmz> Ats_SICS_conType CMD Err!\n");
			continue;
		}
		debug_info(gDebugModule[GPRS_MODULE],
				"INFO: <GprsInit_xmz> Ats_SICS_conType CMD OK!\n");

		err = CMD_AT_RP(Ats_SICS_apn, Ata_OK123, NULL, OS_TICKS_PER_SEC, 3,
				FALSE);
		OSTimeDly(OS_TICKS_PER_SEC);
		if (err) {
			debug_info(gDebugModule[GPRS_MODULE],
					"WARNING: <GprsInit_xmz> Ats_SICS_apn CMD Err!\n");
			continue;
		}
		debug_info(gDebugModule[GPRS_MODULE],
				"INFO: <GprsInit_xmz> Ats_SICS_apn CMD OK!\n");

		err = CMD_AT_RP(Ats_SISS_srvType, Ata_OK123, NULL, OS_TICKS_PER_SEC, 3,
				FALSE);
		OSTimeDly(OS_TICKS_PER_SEC);
		if (err) {
			debug_info(gDebugModule[GPRS_MODULE],
					"WARNING: <GprsInit_xmz> Ats_SISS_srvType CMD Err!\n");
			continue;
		}
		debug_info(gDebugModule[GPRS_MODULE],
				"INFO: <GprsInit_xmz> Ats_SISS_srvType CMD OK!\n");

		err = CMD_AT_RP(Ats_SISS_conId, Ata_OK123, NULL, OS_TICKS_PER_SEC, 3,
				FALSE);
		OSTimeDly(OS_TICKS_PER_SEC);
		if (err) {
			debug_info(gDebugModule[GPRS_MODULE],
					"WARNING: <GprsInit_xmz> Ats_SISS_conId CMD Err!\n");
			continue;
		}
		debug_info(gDebugModule[GPRS_MODULE],
				"INFO: <GprsInit_xmz> Ats_SISS_conId CMD OK!\n");

#if 1
		strcpy(Ats_SISS_address, Ats_SISS_address_h);
		strcat(Ats_SISS_address, gSystem_Config.Str_Ip_addr);
		strcat(Ats_SISS_address, colon);
		strcat(Ats_SISS_address, gSystem_Config.Str_Port);
		strcat(Ats_SISS_address, over124);
		/*begin:yangfei added 2013-08-29 for ������½*/
#else 
		strcpy(Ats_SISS_address,Ats_SISS_address_h);
		strcat(Ats_SISS_address,"513xy.eicp.net");
		strcat(Ats_SISS_address,colon);
		strcat(Ats_SISS_address,"1147");
		strcat(Ats_SISS_address,over124);
#endif

		/*begin:yangfei modified 2013-02-26 for return timeout err*/
#if 0
		err=CMD_AT_RP(Ats_SISS_address,Ata_OK123,NULL,OS_TICKS_PER_SEC,3,TRUE);
#else
		err = CMD_AT_RP(Ats_SISS_address, Ata_OK123, NULL, OS_TICKS_PER_SEC * 3,
				3, FALSE);
#endif
		/*end:yangfei modified 2013-02-26 for */
		OSTimeDly(OS_TICKS_PER_SEC);
		if (err) {
			debug_err(gDebugModule[GPRS_MODULE],
					"<GprsInit_xmz> Ats_SISS_address CMD Err!");
			continue;
		}
		debug_info(gDebugModule[GPRS_MODULE],
				"INFO: <GprsInit_xmz> Ats_SISS_address CMD OK!\n");

		UpQueueFlush(UP_COMMU_DEV_ATIPD);
		//������

		//err=CMD_AT_RP(Ats_SISO,Ata_OK123,NULL,OS_TICKS_PER_SEC*30,0,TRUE);
		err = CMD_AT_RP(Ats_SISO, Ata_OK, NULL, OS_TICKS_PER_SEC * 20, 0,
				FALSE);
		OSTimeDly(OS_TICKS_PER_SEC);
		if (err) {
			debug_err(gDebugModule[GPRS_MODULE],
					"<GprsInit_xmz> Ats_SISO CMD Err=%d!\n", err);
			continue; /*yangfei:needed use */
		}
		debug_info(gDebugModule[GPRS_MODULE],
				"INFO: <GprsInit_xmz> Ats_SISO CMD OK!\n");
		OSTimeDly(OS_TICKS_PER_SEC);
		//err=CMD_AT_RP(Ats_SISI,Ata_SISI,NULL,OS_TICKS_PER_SEC*5,4,TRUE);
		////OSTimeDly(OS_TICKS_PER_SEC);

		OSTimeDly(OS_TICKS_PER_SEC);
		break;
	}
	return 0;
}
