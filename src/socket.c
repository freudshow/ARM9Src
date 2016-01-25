#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>

#define INT8    char
#define UINT8   unsigned char
#define INT16   short
#define UINT16  unsigned short
#define INT32   int
#define UINT32  unsigned int

#define CARD_RET_SUC  0
#define CARD_RET_FAIL -1

#define CARD_ADDR 0x11                      /* �������ĵ�ַ */
#define CARD_CLIENT_IPADDR "192.168.1.66"  /* ������IP��ַ */
#define CARD_SERVER_IPADDR "192.168.1.68"   /* ��������IP��ַ */
#define CARD_CLIENT_PORT 3666           /* ������������ͻ��˵Ķ˿ں� */
#define CARD_SERVER_PORT 3667           /* ���������������˵Ķ˿ں� */
#define CARD_CMD_TAG_BYTE 4             /* ʶ�𵽱�ǩ���ر�ǩ���������ڵڼ����ֽ� */ 
#define CARD_TIME_OUT   2
#define CARD_TIME_OUT1 0               /* ��������������ʱ��ȴ�ʱ�� */
#define CARD_POWER_BYTE 5
#define CARD_POWER_MAX_SIZE 30
#define CARD_FREQ_MAX_SIZE  2
#define CARD_FREQ_BYTE  6
#define CARD_MAJOR_BYTE 5
#define CARD_MINOR_BYTE 6
#define CARD_ANTENNA_BYTE 5
#define CARD_CMD_HEAD   0x0a
#define CARD_ZERO       0

#define ARM_CONTROL_PORT 3668           /* �������˵Ķ˿ںţ����̨ͨ�ţ� */
#define BACKGROUND_SERVER_PROCESS_PORT 3669 /* ��̨�������Ķ˿ںţ��������ͨ�ţ������������������ú������ϴ� */

#define READ_CARD_MAX_BUFF_SIZE   112                  /* ��������BUFF��С */

/* ������������ṹ�� */
typedef struct {
	UINT8 ucHead;
	UINT8 ucAddr;
	UINT8 ucLen;
	union {
		UINT8 ucStatus;
		UINT8 ucCmd;
	};
	UINT8 aucDate[READ_CARD_MAX_BUFF_SIZE];
} READ_CARD_CMD_T, *READ_CARD_CMD_PT;

/* ��������misc�ṹ�� */
typedef struct {
	INT32 lClientSockfd;
	struct sockaddr_in stCardClientSockaddr;
	struct sockaddr_in stCardServerSockaddr;
	UINT8 ucMajorVer;
	UINT8 ucMinorVer;
	UINT8 ucPower;
	UINT8 ucFreq;
	UINT8 ucAntenna;

} READ_CARD_CLIENT_INFO_T;

/* ��������misc�ṹ�� */
typedef struct {
	INT32 lControlClientSockfd;
	struct sockaddr_in stControlClientSockaddr;
	struct sockaddr_in stControlServerSockaddr;

} CONTROL_CLIENT_INFO_T;

READ_CARD_CLIENT_INFO_T g_stReadCardClientInfo;

/* ȡ��ָ�������е�У���   */
/* ����1:����ṹ��ָ��    */
/* ����: У��ֵ           */
INT32 read_card_get_check(READ_CARD_CMD_PT pstCmd, UINT8 *pucCheckNum) {
	UINT8 *pucAddr = NULL;

	/*�����Ϸ���У��*/
	if (NULL == pstCmd || NULL == pucCheckNum) {
		return CARD_RET_FAIL;
	}

	pucAddr = (UINT8 *) &pstCmd->ucLen;

	*pucCheckNum = *(pucAddr + pstCmd->ucLen);

	return CARD_RET_SUC;
}

/* �����������У���   */
/* ����1:����ṹ��ָ�� */
/* ����: У��ֵ        */
INT32 read_card_count_check(READ_CARD_CMD_PT pstCmd, UINT8 *pucCheckNum) {
	UINT32 ulCheckNumber = 0;
	UINT8 ucLoop = 0;
	UINT8 *pucAddr = NULL;

	if (NULL == pstCmd || NULL == pucCheckNum) {
		return CARD_RET_FAIL;
	}

	pucAddr = (UINT8 *) &pstCmd->ucLen;
	ulCheckNumber = pstCmd->ucHead + pstCmd->ucAddr;

	for (ucLoop = 0; ucLoop < pstCmd->ucLen; ucLoop++) {
		ulCheckNumber = ulCheckNumber + (*(pucAddr));
		pucAddr++;
	}

	ulCheckNumber = ~ulCheckNumber;
	ulCheckNumber++;
	/* printf("head=%x   addr=%x   len=%x   cmd=%x  cc=%x  \n",pstCmd->ucHead,pstCmd->ucAddr,pstCmd->ucLen,pstCmd->ucCmd,ulCheckNumber); */

	*pucCheckNum = (UINT8) ulCheckNumber;

	return CARD_RET_SUC;
}

/*����������У��ֵ*/
INT32 read_card_check_sum(READ_CARD_CMD_PT pstCmd) {
	UINT8 ucNewCheck = 0;
	UINT8 ucOldCheck = 0;
	INT32 lRet = 0;

	if (NULL == pstCmd) {
		return CARD_RET_FAIL;
	}

	lRet = read_card_count_check(pstCmd, &ucNewCheck);
	lRet = read_card_get_check(pstCmd, &ucOldCheck);
	if (CARD_RET_SUC != lRet) {
		return CARD_RET_FAIL;
	}

	/* printf("ucNewCheck=%x   ucOldCheck=%x",ucNewCheck,ucOldCheck); */
	if (ucNewCheck != ucOldCheck) {
		printf("[%s][%d][%s]check sum is error \n", __FILE__, __LINE__,
				__FUNCTION__);
		/* У��ʧ�ܺ��¼LOG��Ȼ�����»�ȡ��ǩ */

		return CARD_RET_FAIL;
	}

	return CARD_RET_SUC;
}

/* ��ȡ�����������ΰ汾�� */
/* �޴��� */
/* ���ΰ汾����16λ���أ���8λ������8λ�� */
INT32 read_card_get_version() {
	INT32 lReg = 0;
	UINT16 usCardVersion = 0;
	UINT8 *pucCardCmd = NULL;
	UINT8 ucCheckNum = 0;
	READ_CARD_CMD_T stReadCardSedCmd;
	READ_CARD_CMD_T stReadCardRevDate;

	memset(&stReadCardSedCmd, 0, sizeof(READ_CARD_CMD_T));
	memset(&stReadCardRevDate, 0, sizeof(READ_CARD_CMD_T));
	/* ����ȡ���ΰ汾�ŵ����� */
	stReadCardSedCmd.ucHead = CARD_CMD_HEAD;
	stReadCardSedCmd.ucAddr = CARD_ADDR;
	stReadCardSedCmd.ucLen = 0x02;
	stReadCardSedCmd.ucCmd = 0x22;
	read_card_count_check(&stReadCardSedCmd, &ucCheckNum);
	stReadCardSedCmd.aucDate[0] = ucCheckNum;

	lReg = read_card_sendmsg(&stReadCardSedCmd);
	if (CARD_RET_FAIL == lReg) {
		printf("read_card_sendmsg is error\n");
		/* ������Ϣʧ�ܺ�����¼������¼��Ȼ���˳� */
		return (CARD_RET_FAIL);
	}

	lReg = read_card_select(g_stReadCardClientInfo.lClientSockfd,
			&stReadCardRevDate);
	if ((0 == lReg) || (CARD_RET_FAIL == lReg)) {
		printf("read_card_select is time out (%d)\n", lReg);
		/* ������Ϣʧ�ܻ�ʱ֮�� ����¼��Ȼ���˳� */
		return (CARD_RET_FAIL);
	} else {
		/* ʶ�𵽱�ǩ֮��Ĵ��� */
		/* ����У��� */
#if 0
		ucNewCheck = read_card_count_check(&stReadCardRevDate);
		ucOldCheck = read_card_get_check(&stReadCardRevDate);
		/* printf("ucNewCheck=%x   ucOldCheck=%x",ucNewCheck,ucOldCheck); */
		if(ucNewCheck != ucOldCheck)
		{
			printf("[%s][%d][%s]check sum is error \n", __FILE__, __LINE__, __FUNCTION__);
			/* У��ʧ�ܺ��¼LOG��Ȼ�����»�ȡ��ǩ */
			return(CARD_RET_FAIL);
		}
#endif

		lReg = read_card_check_sum(&stReadCardRevDate);
		if (CARD_RET_SUC != lReg) {
			printf("[%s][%d][%s]check sum is error \n", __FILE__, __LINE__,
					__FUNCTION__);
			/* У��ʧ�ܺ��¼LOG��Ȼ�����»�ȡ��ǩ */
			return CARD_RET_FAIL;
		} else if (0x00 != stReadCardRevDate.ucStatus) {
			/* ���������ص�״̬����0x00 */
			printf("read card return status is error");
			return (CARD_RET_FAIL);
		} else {
			pucCardCmd = (UINT8 *) (&stReadCardRevDate);
			usCardVersion = (*(pucCardCmd + CARD_MAJOR_BYTE));
			usCardVersion = ((usCardVersion << 8)
					+ (*(pucCardCmd + CARD_MINOR_BYTE)));
			return (usCardVersion);
		}
	}

	return (CARD_RET_FAIL);
}

/* ��ѯ��д����Ƶ���� */
/* �޴��� */
/* power&freq��16λ���أ���8λpower����8λfreq */
INT32 read_card_get_freq(UINT8 *pucPower, UINT8 *pucFreq) {
	INT32 lReg = 0;
	UINT8 ucCheckSum = 0;
	UINT8 *pucCardCmd = NULL;
	UINT16 usCardPowerFreq = 0;
	READ_CARD_CMD_T stReadCardSedCmd;
	READ_CARD_CMD_T stReadCardRevDate;

	memset(&stReadCardSedCmd, 0, sizeof(READ_CARD_CMD_T));
	memset(&stReadCardRevDate, 0, sizeof(READ_CARD_CMD_T));
	/* ������� */
	stReadCardSedCmd.ucHead = CARD_CMD_HEAD;
	stReadCardSedCmd.ucAddr = CARD_ADDR;
	stReadCardSedCmd.ucLen = 0x02;
	stReadCardSedCmd.ucCmd = 0x26;
	read_card_count_check(&stReadCardSedCmd, &ucCheckSum);
	stReadCardSedCmd.aucDate[0] = ucCheckSum;
	lReg = read_card_sendmsg(&stReadCardSedCmd);
	if (CARD_RET_FAIL == lReg) {
		printf("read_card_sendmsg is error\n");
		/* ������Ϣʧ�ܺ�����¼������¼��Ȼ���˳� */
		return (lReg);
	}
	lReg = read_card_select(g_stReadCardClientInfo.lClientSockfd,
			&stReadCardRevDate);
	if ((0 == lReg) || (CARD_RET_FAIL == lReg)) {
		printf("read_card_select is time out (%d)\n", lReg);
		/* ������Ϣʧ�ܻ�ʱ֮�� ����¼��Ȼ���˳� */
		return (lReg);
	} else {
		/* ʶ�𵽱�ǩ֮��Ĵ��� */
		/* ����У��� */
		lReg = read_card_check_sum(&stReadCardRevDate);
		if (CARD_RET_SUC != lReg) {
			printf("[%s][%d][%s]check sum is error \n", __FILE__, __LINE__,
					__FUNCTION__);
			/* У��ʧ�ܺ��¼LOG��Ȼ�����»�ȡ��ǩ */
			return CARD_RET_FAIL;
		} else if (0x00 != stReadCardRevDate.ucStatus) {
			/* ���������ص�״̬����0x00 */
			printf("read card return status is error");
			return (CARD_RET_FAIL);
		} else {
			*pucPower = (*(pucCardCmd + CARD_POWER_BYTE));
			*pucFreq = (*(pucCardCmd + CARD_FREQ_BYTE));
			/* pucCardCmd = (UINT8 *)(&stReadCardRevDate);
			 usCardPowerFreq = (*(pucCardCmd+CARD_POWER_BYTE));
			 usCardPowerFreq = ((usCardPowerFreq << 8) + (*(pucCardCmd+CARD_FREQ_BYTE)));
			 */
			return (CARD_RET_SUC);
		}
	}

	return (CARD_RET_FAIL);
}

/* ���ö�д����Ƶ����    */
/* ����1��power 0~30     */
/* ����2��freq  00 01 02 */
/* ���� 0���ɹ�  CARD_RET_FAIL��ʧ�� */
INT32 read_card_set_freq(UINT8 ucCardPower, UINT8 ucCardFreq) {
	INT32 lReg = 0;
	UINT8 ucCheckSum = 0;
	UINT8 *pucCardCmd = NULL;
	READ_CARD_CMD_T stReadCardSedCmd;
	READ_CARD_CMD_T stReadCardRevDate;

	if ((ucCardPower < CARD_ZERO) || (ucCardPower > CARD_POWER_MAX_SIZE)) {
		printf("para ucCardPower is not 0 ~ 30 ");
		return (CARD_RET_FAIL);
	}
	if ((ucCardFreq < CARD_ZERO) || (ucCardFreq > CARD_FREQ_MAX_SIZE)) {
		printf("para ucCardFreq is not 0 ~ 2 ");
		return (CARD_RET_FAIL);
	}
	memset(&stReadCardSedCmd, 0, sizeof(READ_CARD_CMD_T));
	memset(&stReadCardRevDate, 0, sizeof(READ_CARD_CMD_T));
	/* ������� */
	stReadCardSedCmd.ucHead = CARD_CMD_HEAD;
	stReadCardSedCmd.ucAddr = CARD_ADDR;
	stReadCardSedCmd.ucLen = 0x04;
	stReadCardSedCmd.ucCmd = 0x25;
	stReadCardSedCmd.aucDate[0] = ucCardPower;
	stReadCardSedCmd.aucDate[1] = ucCardFreq;
	read_card_count_check(&stReadCardSedCmd, &ucCheckSum);
	stReadCardSedCmd.aucDate[2] = ucCheckSum;
	lReg = read_card_sendmsg(&stReadCardSedCmd);

	if (CARD_RET_FAIL == lReg) {
		printf("read_card_sendmsg is error\n");
		/* ������Ϣʧ�ܺ�����¼������¼��Ȼ���˳� */
		return (lReg);
	}
	lReg = read_card_select(g_stReadCardClientInfo.lClientSockfd,
			&stReadCardRevDate);
	if ((0 == lReg) || (CARD_RET_FAIL == lReg)) {
		printf("read_card_select is time out (%d)\n", lReg);
		/* ������Ϣʧ�ܻ�ʱ֮�� ����¼��Ȼ���˳� */
		return (CARD_RET_FAIL);
	} else {
		/* ʶ�𵽱�ǩ֮��Ĵ��� */
		/* ����У��� */
#if 0
		ucNewCheck = read_card_count_check(&stReadCardRevDate);
		ucOldCheck = read_card_get_check(&stReadCardRevDate);
		/* printf("ucNewCheck=%x   ucOldCheck=%x",ucNewCheck,ucOldCheck); */
		if(ucNewCheck != ucOldCheck)
		{
			printf("check sum is error \n");
			/* У��ʧ�ܺ��¼LOG��Ȼ�����»�ȡ��ǩ */
			return(CARD_RET_FAIL);
		}
#endif

		lReg = read_card_check_sum(&stReadCardRevDate);
		if (CARD_RET_SUC != lReg) {
			printf("[%s][%d][%s]check sum is error \n", __FILE__, __LINE__,
					__FUNCTION__);
			/* У��ʧ�ܺ��¼LOG��Ȼ�����»�ȡ��ǩ */
			return CARD_RET_FAIL;
		}

		if (0x00 != stReadCardRevDate.ucStatus) {
			/* ���������ص�״̬����0x00 */
			printf("read card return status is error");
			return (CARD_RET_FAIL);
		} else {
			return (0);
		}
	}
	return (CARD_RET_FAIL);
}

/* ��ѯ��д���Ĺ������� */
/* �޴��� */
/* �������ߺ� */
INT32 read_card_get_antenna(UINT8 *pucAntenna) {
	INT32 lReg = 0;
	UINT8 ucCheckSum = 0;
	UINT8 *pucCardCmd = NULL;
	UINT8 ucAntenna = 0;
	READ_CARD_CMD_T stReadCardSedCmd;
	READ_CARD_CMD_T stReadCardRevDate;

	memset(&stReadCardSedCmd, 0, sizeof(READ_CARD_CMD_T));
	memset(&stReadCardRevDate, 0, sizeof(READ_CARD_CMD_T));
	/* ������� */
	stReadCardSedCmd.ucHead = CARD_CMD_HEAD;
	stReadCardSedCmd.ucAddr = CARD_ADDR;
	stReadCardSedCmd.ucLen = 0x02;
	stReadCardSedCmd.ucCmd = 0x28;
	read_card_count_check(&stReadCardSedCmd, &ucCheckSum);
	stReadCardSedCmd.aucDate[0] = ucCheckSum;
	lReg = read_card_sendmsg(&stReadCardSedCmd);

	if (CARD_RET_FAIL == lReg) {
		printf("read_card_sendmsg is error\n");
		/* ������Ϣʧ�ܺ�����¼������¼��Ȼ���˳� */
		return (lReg);
	}

	lReg = read_card_select(g_stReadCardClientInfo.lClientSockfd,
			&stReadCardRevDate);
	if ((0 == lReg) || (CARD_RET_FAIL == lReg)) {
		printf("read_card_select is time out (%d)\n", lReg);
		/* ������Ϣʧ�ܻ�ʱ֮�� ����¼��Ȼ���˳� */
		return (CARD_RET_FAIL);
	} else {
		/* ʶ�𵽱�ǩ֮��Ĵ��� */
		/* ����У��� */
#if 0
		ucNewCheck = read_card_count_check(&stReadCardRevDate);
		ucOldCheck = read_card_get_check(&stReadCardRevDate);
		/* printf("ucNewCheck=%x   ucOldCheck=%x",ucNewCheck,ucOldCheck); */
		if(ucNewCheck != ucOldCheck)
		{
			printf("check sum is error \n");
			/* У��ʧ�ܺ��¼LOG��Ȼ�����»�ȡ��ǩ */
			return(CARD_RET_FAIL);
		}
#endif

		lReg = read_card_check_sum(&stReadCardRevDate);
		if (CARD_RET_SUC != lReg) {
			printf("[%s][%d][%s]check sum is error \n", __FILE__, __LINE__,
					__FUNCTION__);
			/* У��ʧ�ܺ��¼LOG��Ȼ�����»�ȡ��ǩ */
			return CARD_RET_FAIL;
		}

		if (0x00 != stReadCardRevDate.ucStatus) {
			/* ���������ص�״̬����0x00 */
			printf("read card return status is error");
			return (CARD_RET_FAIL);
		} else {
			pucCardCmd = (UINT8 *) (&stReadCardRevDate);
			*pucAntenna = (*(pucCardCmd + CARD_ANTENNA_BYTE));
			return (CARD_RET_SUC);
		}
	}

	return (CARD_RET_FAIL);
}

/* ���ö�д���������� */
/* ���õ����ߺ� */
/* ����0 �ɹ� CARD_RET_FAIL ʧ�� */
INT32 read_card_set_antenna(UINT8 ucAntenna) {
	INT32 lReg = 0;
	UINT8 ucCheckSum = 0;
	UINT8 *pucCardCmd = NULL;
	UINT8 ucTmpAntenna = 0;
	READ_CARD_CMD_T stReadCardSedCmd;
	READ_CARD_CMD_T stReadCardRevDate;

	if ((ucAntenna < 0) || (ucAntenna > 8)) {
		printf("para set ucAntenna is error \n");
		return (CARD_RET_FAIL);
	}
	memset(&stReadCardSedCmd, 0, sizeof(READ_CARD_CMD_T));
	memset(&stReadCardRevDate, 0, sizeof(READ_CARD_CMD_T));
	/* ������� */
	stReadCardSedCmd.ucHead = CARD_CMD_HEAD;
	stReadCardSedCmd.ucAddr = CARD_ADDR;
	stReadCardSedCmd.ucLen = 0x03;
	stReadCardSedCmd.ucCmd = 0x27;
	stReadCardSedCmd.aucDate[0] = (ucTmpAntenna << ucAntenna);
	read_card_count_check(&stReadCardSedCmd, &ucCheckSum);
	stReadCardSedCmd.aucDate[1] = ucCheckSum;
	lReg = read_card_sendmsg(&stReadCardSedCmd);

	if (CARD_RET_FAIL == lReg) {
		printf("read_card_sendmsg is error\n");
		/* ������Ϣʧ�ܺ�����¼������¼��Ȼ���˳� */
		return (lReg);
	}

	lReg = read_card_select(g_stReadCardClientInfo.lClientSockfd,
			&stReadCardRevDate);
	if ((0 == lReg) || (CARD_RET_FAIL == lReg)) {
		printf("read_card_select is time out (%d)\n", lReg);
		/* ������Ϣʧ�ܻ�ʱ֮�� ����¼��Ȼ���˳� */
		return (CARD_RET_FAIL);
	} else {
		/* ʶ�𵽱�ǩ֮��Ĵ��� */
		/* ����У��� */
#if 0
		ucNewCheck = read_card_count_check(&stReadCardRevDate);
		ucOldCheck = read_card_get_check(&stReadCardRevDate);
		/* printf("ucNewCheck=%x   ucOldCheck=%x",ucNewCheck,ucOldCheck); */
		if(ucNewCheck != ucOldCheck)
		{
			printf("check sum is error \n");
			/* У��ʧ�ܺ��¼LOG��Ȼ�����»�ȡ��ǩ */
			return(CARD_RET_FAIL);
		}
#endif
		lReg = read_card_check_sum(&stReadCardRevDate);
		if (CARD_RET_SUC != lReg) {
			printf("[%s][%d][%s]check sum is error \n", __FILE__, __LINE__,
					__FUNCTION__);
			/* У��ʧ�ܺ��¼LOG��Ȼ�����»�ȡ��ǩ */
			return CARD_RET_FAIL;
		}

		if (0x00 != stReadCardRevDate.ucStatus) {
			/* ���������ص�״̬����0x00 */
			printf("read card return status is error");
			return (CARD_RET_FAIL);
		} else {
			return (0);
		}
	}
	return (CARD_RET_FAIL);
}

/* ���ö�д��RS232ͨ�Ų����� */
/* ����1����д���Ĳ�����0x00=9600bps��0x01=19200bps��0x02=38400bps��0x03=57600bps��0x04=115200bps */
/* ����0 �ɹ���CARD_RET_FAIL ʧ�� */
INT32 read_card_set_baudrate(UINT8 ucCardBaudrate) {
	INT32 lReg = 0;
	UINT8 *pucCardCmd = NULL;
	UINT8 ucCheckNum = 0;
	READ_CARD_CMD_T stReadCardSedCmd;
	READ_CARD_CMD_T stReadCardRevDate;

	if ((ucCardBaudrate < 0) || (ucCardBaudrate > 4)) {
		printf("para set ucCardBaudrate is error \n");
		return (CARD_RET_FAIL);
	}

	memset(&stReadCardSedCmd, 0, sizeof(READ_CARD_CMD_T));
	memset(&stReadCardRevDate, 0, sizeof(READ_CARD_CMD_T));

	/* ������� */
	stReadCardSedCmd.ucHead = CARD_CMD_HEAD;
	stReadCardSedCmd.ucAddr = CARD_ADDR;
	stReadCardSedCmd.ucLen = 0x03;
	stReadCardSedCmd.ucCmd = 0x20;
	stReadCardSedCmd.aucDate[0] = ucCardBaudrate;

	read_card_count_check(&stReadCardSedCmd, &ucCheckNum);
	stReadCardSedCmd.aucDate[1] = ucCheckNum;

	lReg = read_card_sendmsg(&stReadCardSedCmd);
	if (CARD_RET_FAIL == lReg) {
		printf("read_card_sendmsg is error\n");
		/* ������Ϣʧ�ܺ�����¼������¼��Ȼ���˳� */
		return (lReg);
	}

	lReg = read_card_select(g_stReadCardClientInfo.lClientSockfd,
			&stReadCardRevDate);
	if ((0 == lReg) || (CARD_RET_FAIL == lReg)) {
		printf("read_card_select is time out (%d)\n", lReg);
		/* ������Ϣʧ�ܻ�ʱ֮�� ����¼��Ȼ���˳� */
		return (CARD_RET_FAIL);
	} else {
		/* ʶ�𵽱�ǩ֮��Ĵ��� */
		/* ����У��� */
#if 0
		ucNewCheck = read_card_count_check(&stReadCardRevDate);
		ucOldCheck = read_card_get_check(&stReadCardRevDate);
		/* printf("ucNewCheck=%x   ucOldCheck=%x",ucNewCheck,ucOldCheck); */
		if(ucNewCheck != ucOldCheck)
		{
			printf("check sum is error \n");
			/* У��ʧ�ܺ��¼LOG��Ȼ�����»�ȡ��ǩ */
			return(CARD_RET_FAIL);
		}
#endif

		lReg = read_card_check_sum(&stReadCardRevDate);
		if (CARD_RET_SUC != lReg) {
			printf("[%s][%d][%s]check sum is error \n", __FILE__, __LINE__,
					__FUNCTION__);
			/* У��ʧ�ܺ��¼LOG��Ȼ�����»�ȡ��ǩ */
			return CARD_RET_FAIL;
		}

		if (0x00 != stReadCardRevDate.ucStatus) {
			/* ���������ص�״̬����0x00 */
			printf("read card return status is error");
			return (CARD_RET_FAIL);
		} else {
			return (0);
		}
	}

	return (CARD_RET_FAIL);
}

/* ��λ��д�� */
INT32 read_card_reset() {
	return 0;
}

/* ��������ʼ������ */
void read_card_init() {
	INT32 lReg = 0;

	/* �����������Ŀͻ���socket */
	g_stReadCardClientInfo.lClientSockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (CARD_RET_FAIL == g_stReadCardClientInfo.lClientSockfd) {
		printf("Card_Client Creat socket error\n");
		exit(1);
	}

	/* ����socket ��card client���� */
	g_stReadCardClientInfo.stCardClientSockaddr.sin_family = AF_INET;
	g_stReadCardClientInfo.stCardClientSockaddr.sin_port = htons(
			CARD_CLIENT_PORT);
	/* g_stReadCardClientInfo.stCardClientSockaddr.sin_addr.s_addr=INADDR_ANY; */
	g_stReadCardClientInfo.stCardClientSockaddr.sin_addr.s_addr = inet_addr(
			CARD_CLIENT_IPADDR);
	bzero(&(g_stReadCardClientInfo.stCardClientSockaddr), 8);

	/* ����socket ��card server���� */
	g_stReadCardClientInfo.stCardServerSockaddr.sin_family = AF_INET;
	g_stReadCardClientInfo.stCardServerSockaddr.sin_port = htons(
			CARD_SERVER_PORT);
	/* g_stReadCardClientInfo.stCardServerSockaddr.sin_addr.s_addr=INADDR_ANY; */
	g_stReadCardClientInfo.stCardServerSockaddr.sin_addr.s_addr = inet_addr(
			CARD_SERVER_IPADDR);
	bzero(&(g_stReadCardClientInfo.stCardClientSockaddr), 8);

	/* ��socket ip�Ͷ˿� */
	lReg = bind(g_stReadCardClientInfo.lClientSockfd,
			(struct sockaddr *) &g_stReadCardClientInfo.stCardClientSockaddr,
			sizeof(struct sockaddr));
	if (CARD_RET_FAIL == lReg) {
		printf("Card_Client Bind socket error\n");
		close(g_stReadCardClientInfo.lClientSockfd);
		exit(1);
	}

	/* ���Ӷ�������TCP����� */
	lReg = connect(g_stReadCardClientInfo.lClientSockfd,
			(struct sockaddr *) &g_stReadCardClientInfo.stCardServerSockaddr,
			sizeof(struct sockaddr));
	if (CARD_RET_FAIL == lReg) {
		printf("Connect server error\n");
		close(g_stReadCardClientInfo.lClientSockfd);
		exit(1);
	}

	return;
}

/* ��������������                      */
/* ����1:����������ṹ��ָ��           */
/* ���� CARD_RET_FAIL ����ʧ�ܣ�����0�����͵��ֽ��� */
INT32 read_card_sendmsg(READ_CARD_CMD_PT pstReadCardCmd) {
	INT32 lSendLen = 0;

	/* �������ݵ�card �ķ���� */
	lSendLen = send(g_stReadCardClientInfo.lClientSockfd,
			(const UINT8 *) pstReadCardCmd, (pstReadCardCmd->ucLen + 3), 0);
	if (CARD_RET_FAIL == lSendLen) {
		printf("Send msg to server error\n");
		return (lSendLen);
	}
	printf("To Read card Send len :%d\n", lSendLen);

	return (lSendLen);
}

/* ���õȴ���socket�����س�ʱ��socketֵ         */
/* ����1:�ȴ���socket id                       */
/* ����2:��Ž��յ����ݵĽṹ��ָ��             */
/* ����0����ʱ��-1����������0�����յ����ֽ��� */
INT32 read_card_select(INT32 lWaitSocket, READ_CARD_CMD_PT pstRecvDate) {
	INT32 lReg = 0;
	INT32 lSendLen = 0;
	fd_set ReadCardReadFds;
	struct timeval stTimeVal;

	/* ���õȴ��ĳ�ʱʱ�� */
	stTimeVal.tv_sec = CARD_TIME_OUT;
	stTimeVal.tv_usec = CARD_TIME_OUT1;
	FD_ZERO(&ReadCardReadFds);
	FD_SET(lWaitSocket, &ReadCardReadFds);

	lReg = select(lWaitSocket + 1, &ReadCardReadFds, NULL, NULL, &stTimeVal);
	if ((0 == lReg) || (-1 == lReg)) {
		return (lReg);
	} else {
		if (FD_ISSET(lWaitSocket, &ReadCardReadFds)) {
			lSendLen = recv(lWaitSocket, (char*) pstRecvDate,
					sizeof(READ_CARD_CMD_T), 0);
			if (lSendLen < 0) {
				printf("secv is error\n");
				return CARD_RET_FAIL;
			}
		} else {
			printf("it is not WaitSocket\n");
			return (CARD_RET_FAIL);
		}
	}

	return (lSendLen);
}
/* �������Ĵ������̺߳��� */
void read_card_main_proc() {
	INT32 lReg = 0;
	UINT8 ucCheckSum = 0;
	INT32 lReadCardVersion = 0;
	INT32 lSendLen = 0;
	UINT8 ucTagNumber = 0;
	READ_CARD_CMD_T stReadCardCmd;
	READ_CARD_CMD_T stReadCardDate;

	printf("this is in pthread ...[%s][%s][%d] \n", FILE_LINE);
	/* ��ʼ�������� */
	read_card_init();
	/* ��ȡ�������汾ʧ�� �����˳� */
	lReadCardVersion = read_card_get_version();
	if (CARD_RET_FAIL == lReadCardVersion) {
		printf("get read card version is error");
		exit(0);
	}
	g_stReadCardClientInfo.ucMajorVer = (UINT8) (lReadCardVersion >> 8);
	g_stReadCardClientInfo.ucMinorVer = (UINT8) (lReadCardVersion);
	/* ��ȡ��������power��freq */
	lReg = read_card_get_freq(&g_stReadCardClientInfo.ucPower,
			&g_stReadCardClientInfo.ucFreq);
	if (CARD_RET_FAIL == lReg) {
		printf("get read card power/freq  is error");
		exit(0);
	}
	/* ��ȡ�����������ߺ�*/
	lReg = read_card_get_antenna(&g_stReadCardClientInfo.ucAntenna);
	if (CARD_RET_FAIL == lReg) {
		printf("get read card Antenna  is error");
		exit(0);
	}
	/* ��ȡ�������İ汾�� 
	 lTmpNumber = read_card_get_version();
	 if(lTmpNumber)
	 g_stReadCardClientInfo.ucMajorVer = (UINT8)((lTmpNumber>>8)&0xff);
	 g_stReadCardClientInfo.ucMinorVer = (UINT8)(lTmpNumber&0xff);
	 */
	while (1) {
		/* ��װ���Ͷ������Ķ��ǩʶ������ */
		memset(&stReadCardCmd, 0, sizeof(READ_CARD_CMD_T));
		memset(&stReadCardDate, 0, sizeof(READ_CARD_CMD_T));
		stReadCardCmd.ucHead = CARD_CMD_HEAD;
		stReadCardCmd.ucAddr = CARD_ADDR;
		stReadCardCmd.ucLen = 0x02;
		stReadCardCmd.ucCmd = 0x80;
		read_card_count_check(&stReadCardCmd, &ucCheckSum);
		stReadCardCmd.aucDate[0] = ucCheckSum;
		lReg = read_card_sendmsg(&stReadCardCmd);
		if (CARD_RET_FAIL == lReg) {
			printf("read_card_sendmsg is error\n");
			/* ������Ϣʧ�ܺ�����¼��Ȼ�����������Ϣ */
			continue;
		}
		lReg = read_card_select(g_stReadCardClientInfo.lClientSockfd,
				&stReadCardDate);
		if ((0 == lReg) || (CARD_RET_FAIL == lReg)
				|| (0x00 != stReadCardDate.ucStatus)) {
			printf("read_card_select is time out (%d)\n", lReg);
			/* ������Ϣʧ�ܻ�ʱ֮�� ����¼��Ȼ�������������»�ȡ��ǩ */
			continue;
		} else {
			/* ʶ�𵽱�ǩ֮��Ĵ��� */
			/* ����У��� */
#if 0
			ucNewCheck = read_card_count_check(&stReadCardDate);
			ucOldCheck = read_card_get_check(&stReadCardDate);
			/* printf("ucNewCheck=%x   ucOldCheck=%x",ucNewCheck,ucOldCheck); */
			if(ucNewCheck != ucOldCheck)
			{
				printf("check sum is error \n");
				/* У��ʧ�ܺ��¼LOG��Ȼ�����»�ȡ��ǩ */
				continue;
			}
#endif

			lReg = read_card_check_sum(&stReadCardDate);
			if (CARD_RET_SUC != lReg) {
				printf("[%s][%d][%s]check sum is error \n", __FILE__, __LINE__,
						__FUNCTION__);
				/* У��ʧ�ܺ��¼LOG��Ȼ�����»�ȡ��ǩ */
				continue;
			} else {
				ucTagNumber =
						*((UINT8 *) (&stReadCardDate) + CARD_CMD_TAG_BYTE);
			}
		}
		/* ���ݻ�ȡ�ı�ǩ��������װ���Ͷ������Ļ�ȡ�������������� */
		memset(&stReadCardCmd, 0, sizeof(READ_CARD_CMD_T));
		memset(&stReadCardDate, 0, sizeof(READ_CARD_CMD_T));
		stReadCardCmd.ucHead = CARD_CMD_HEAD;
		stReadCardCmd.ucAddr = CARD_ADDR;
		stReadCardCmd.ucLen = 0x03;
		stReadCardCmd.ucCmd = 0x40;
		stReadCardCmd.aucDate[0] = ucTagNumber;
		read_card_count_check(&stReadCardCmd, &ucCheckSum);
		stReadCardCmd.aucDate[1] = ucCheckSum;
		lReg = read_card_sendmsg(&stReadCardCmd);
		if (CARD_RET_FAIL == lReg) {
			printf("read_card_sendmsg is error \n");
			/* ������Ϣʧ�ܺ�����¼��Ȼ������������� */
			continue;
		}
		lReg = read_card_select(g_stReadCardClientInfo.lClientSockfd,
				&stReadCardDate);
		if ((0 == lReg) || (CARD_RET_FAIL == lReg)) {
			printf("read_card_select is time out (%d)\n", lReg);
			/* ������Ϣʧ�ܻ�ʱ֮�� ����¼��Ȼ�������������»�ȡ��ǩ */
			continue;
		} else {
			/* ʶ�𵽱�ǩ֮��Ĵ��� */
			/* ����У��� */
#if 0
			ucNewCheck = read_card_count_check(&stReadCardDate);
			ucOldCheck = read_card_get_check(&stReadCardDate);
			if(ucNewCheck != ucOldCheck)
			{
				printf("check sum is error \n");
				/* У��ʧ�ܺ��¼LOG��Ȼ�����»�ȡ��ǩ */
				continue;
			}
#endif

			lReg = read_card_check_sum(&stReadCardDate);
			if (CARD_RET_SUC != lReg) {
				printf("[%s][%d][%s]check sum is error \n", __FILE__, __LINE__,
						__FUNCTION__);
				/* У��ʧ�ܺ��¼LOG��Ȼ�����»�ȡ��ǩ */
				continue;
			} else {
				printf("it is over ...\n");
			}
		}
	}

	return;
}

/* ��������� */
INT32 main() {
	INT32 lReg = 0;
	INT32 lSendLen = 0;
	pthread_t ReadCardPthreadId;
	pthread_t CamerPthreadId;
	READ_CARD_CMD_T stReadCardCmd;

	printf("this is in main ...[%s][%s][%d] \n", FILE_LINE);
	/* memset(g_stReadCardClientInfo.Client_Ip,0,sizeof(g_stReadCardClientInfo.Client_Ip)); */
	/* �������������߳� */
	lReg = pthread_create(&ReadCardPthreadId, NULL,
			(void *) read_card_main_proc, NULL);
	if (0 != lReg) {
		printf("Create read card pthread error!\n");
		exit(1);
	}

#if 0
	sleep(2);
	/* ��װ����BUFF */
	stReadCardCmd.ucHead='r';
	stReadCardCmd.ucAddr='u';
	stReadCardCmd.ucLen=0x0b;
	stReadCardCmd.ucCmd=0x20;
	strcpy(stReadCardCmd.aucDate,"hahahaha");

	/* �������ݵ�card �ķ���� */
	lSendLen = send(g_stReadCardClientInfo.lClientSockfd,(const UINT8 *)&stReadCardCmd,sizeof(READ_CARD_CMD_T),0);
	if(CARD_RET_FAIL == lSendLen)
	{
		printf("Send msg to server error\n");
		close(g_stReadCardClientInfo.lClientSockfd);
		exit(1);
	}
	printf("Send len :%d\n",lSendLen);

	lSendLen = recv(g_stReadCardClientInfo.lClientSockfd,(void *)&pstRead_Card_Cmd,sizeof(pstRead_Card_Cmd),0);
	if(CARD_RET_FAIL == lSendLen)
	{
		printf("recv error\n");
		exit(1);
	}

	printf("recv len :%d\n",lSendLen);
	printf("ucHead=%d  ucAddr=%d  ucLen=%d ucCmd=%d \n",pstRead_Card_Cmd.ucHead,pstRead_Card_Cmd.ucAddr,pstRead_Card_Cmd.ucLen,pstRead_Card_Cmd.ucCmd);
#endif
	sleep(20);

	return (0);
}

