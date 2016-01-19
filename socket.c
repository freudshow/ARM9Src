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

#define CARD_ADDR 0x11                      /* 读卡器的地址 */
#define CARD_CLIENT_IPADDR "192.168.1.66"  /* 本机的IP地址 */
#define CARD_SERVER_IPADDR "192.168.1.68"   /* 读卡器的IP地址 */
#define CARD_CLIENT_PORT 3666           /* 分配给读卡器客户端的端口号 */
#define CARD_SERVER_PORT 3667           /* 分配给读卡器服务端的端口号 */
#define CARD_CMD_TAG_BYTE 4             /* 识别到标签返回标签个数，所在第几个字节 */ 
#define CARD_TIME_OUT   2
#define CARD_TIME_OUT1 0               /* 读卡器接收数据时最长等待时间 */
#define CARD_POWER_BYTE 5
#define CARD_POWER_MAX_SIZE 30
#define CARD_FREQ_MAX_SIZE  2
#define CARD_FREQ_BYTE  6
#define CARD_MAJOR_BYTE 5
#define CARD_MINOR_BYTE 6
#define CARD_ANTENNA_BYTE 5
#define CARD_CMD_HEAD   0x0a
#define CARD_ZERO       0

#define ARM_CONTROL_PORT 3668           /* 控制器端的端口号（与后台通信） */
#define BACKGROUND_SERVER_PROCESS_PORT 3669 /* 后台服务程序的端口号（与控制器通信），可用于升级，配置和数据上传 */

#define READ_CARD_MAX_BUFF_SIZE   112                  /* 读卡器的BUFF大小 */

/* 读卡器的命令结构体 */
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

/* 读卡器的misc结构体 */
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

/* 控制器的misc结构体 */
typedef struct {
	INT32 lControlClientSockfd;
	struct sockaddr_in stControlClientSockaddr;
	struct sockaddr_in stControlServerSockaddr;

} CONTROL_CLIENT_INFO_T;

READ_CARD_CLIENT_INFO_T g_stReadCardClientInfo;

/* 取出指定命令中的校验和   */
/* 参数1:命令结构体指针    */
/* 返回: 校验值           */
INT32 read_card_get_check(READ_CARD_CMD_PT pstCmd, UINT8 *pucCheckNum) {
	UINT8 *pucAddr = NULL;

	/*参数合法性校验*/
	if (NULL == pstCmd || NULL == pucCheckNum) {
		return CARD_RET_FAIL;
	}

	pucAddr = (UINT8 *) &pstCmd->ucLen;

	*pucCheckNum = *(pucAddr + pstCmd->ucLen);

	return CARD_RET_SUC;
}

/* 计算读卡器的校验和   */
/* 参数1:命令结构体指针 */
/* 返回: 校验值        */
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

/*检查读卡器的校验值*/
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
		/* 校验失败后记录LOG，然后重新获取标签 */

		return CARD_RET_FAIL;
	}

	return CARD_RET_SUC;
}

/* 获取读卡器的主次版本号 */
/* 无传参 */
/* 主次版本号以16位返回，高8位主，低8位次 */
INT32 read_card_get_version() {
	INT32 lReg = 0;
	UINT16 usCardVersion = 0;
	UINT8 *pucCardCmd = NULL;
	UINT8 ucCheckNum = 0;
	READ_CARD_CMD_T stReadCardSedCmd;
	READ_CARD_CMD_T stReadCardRevDate;

	memset(&stReadCardSedCmd, 0, sizeof(READ_CARD_CMD_T));
	memset(&stReadCardRevDate, 0, sizeof(READ_CARD_CMD_T));
	/* 填充获取主次版本号的命令 */
	stReadCardSedCmd.ucHead = CARD_CMD_HEAD;
	stReadCardSedCmd.ucAddr = CARD_ADDR;
	stReadCardSedCmd.ucLen = 0x02;
	stReadCardSedCmd.ucCmd = 0x22;
	read_card_count_check(&stReadCardSedCmd, &ucCheckNum);
	stReadCardSedCmd.aucDate[0] = ucCheckNum;

	lReg = read_card_sendmsg(&stReadCardSedCmd);
	if (CARD_RET_FAIL == lReg) {
		printf("read_card_sendmsg is error\n");
		/* 发送消息失败后做记录，做记录，然后退出 */
		return (CARD_RET_FAIL);
	}

	lReg = read_card_select(g_stReadCardClientInfo.lClientSockfd,
			&stReadCardRevDate);
	if ((0 == lReg) || (CARD_RET_FAIL == lReg)) {
		printf("read_card_select is time out (%d)\n", lReg);
		/* 接收消息失败或超时之后 做记录，然后退出 */
		return (CARD_RET_FAIL);
	} else {
		/* 识别到标签之后的处理 */
		/* 计算校验和 */
#if 0
		ucNewCheck = read_card_count_check(&stReadCardRevDate);
		ucOldCheck = read_card_get_check(&stReadCardRevDate);
		/* printf("ucNewCheck=%x   ucOldCheck=%x",ucNewCheck,ucOldCheck); */
		if(ucNewCheck != ucOldCheck)
		{
			printf("[%s][%d][%s]check sum is error \n", __FILE__, __LINE__, __FUNCTION__);
			/* 校验失败后记录LOG，然后重新获取标签 */
			return(CARD_RET_FAIL);
		}
#endif

		lReg = read_card_check_sum(&stReadCardRevDate);
		if (CARD_RET_SUC != lReg) {
			printf("[%s][%d][%s]check sum is error \n", __FILE__, __LINE__,
					__FUNCTION__);
			/* 校验失败后记录LOG，然后重新获取标签 */
			return CARD_RET_FAIL;
		} else if (0x00 != stReadCardRevDate.ucStatus) {
			/* 读卡器返回的状态不是0x00 */
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

/* 查询读写器射频参数 */
/* 无传参 */
/* power&freq以16位返回，高8位power，低8位freq */
INT32 read_card_get_freq(UINT8 *pucPower, UINT8 *pucFreq) {
	INT32 lReg = 0;
	UINT8 ucCheckSum = 0;
	UINT8 *pucCardCmd = NULL;
	UINT16 usCardPowerFreq = 0;
	READ_CARD_CMD_T stReadCardSedCmd;
	READ_CARD_CMD_T stReadCardRevDate;

	memset(&stReadCardSedCmd, 0, sizeof(READ_CARD_CMD_T));
	memset(&stReadCardRevDate, 0, sizeof(READ_CARD_CMD_T));
	/* 填充命令 */
	stReadCardSedCmd.ucHead = CARD_CMD_HEAD;
	stReadCardSedCmd.ucAddr = CARD_ADDR;
	stReadCardSedCmd.ucLen = 0x02;
	stReadCardSedCmd.ucCmd = 0x26;
	read_card_count_check(&stReadCardSedCmd, &ucCheckSum);
	stReadCardSedCmd.aucDate[0] = ucCheckSum;
	lReg = read_card_sendmsg(&stReadCardSedCmd);
	if (CARD_RET_FAIL == lReg) {
		printf("read_card_sendmsg is error\n");
		/* 发送消息失败后做记录，做记录，然后退出 */
		return (lReg);
	}
	lReg = read_card_select(g_stReadCardClientInfo.lClientSockfd,
			&stReadCardRevDate);
	if ((0 == lReg) || (CARD_RET_FAIL == lReg)) {
		printf("read_card_select is time out (%d)\n", lReg);
		/* 接收消息失败或超时之后 做记录，然后退出 */
		return (lReg);
	} else {
		/* 识别到标签之后的处理 */
		/* 计算校验和 */
		lReg = read_card_check_sum(&stReadCardRevDate);
		if (CARD_RET_SUC != lReg) {
			printf("[%s][%d][%s]check sum is error \n", __FILE__, __LINE__,
					__FUNCTION__);
			/* 校验失败后记录LOG，然后重新获取标签 */
			return CARD_RET_FAIL;
		} else if (0x00 != stReadCardRevDate.ucStatus) {
			/* 读卡器返回的状态不是0x00 */
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

/* 设置读写器射频参数    */
/* 参数1：power 0~30     */
/* 参数2：freq  00 01 02 */
/* 返回 0：成功  CARD_RET_FAIL：失败 */
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
	/* 填充命令 */
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
		/* 发送消息失败后做记录，做记录，然后退出 */
		return (lReg);
	}
	lReg = read_card_select(g_stReadCardClientInfo.lClientSockfd,
			&stReadCardRevDate);
	if ((0 == lReg) || (CARD_RET_FAIL == lReg)) {
		printf("read_card_select is time out (%d)\n", lReg);
		/* 接收消息失败或超时之后 做记录，然后退出 */
		return (CARD_RET_FAIL);
	} else {
		/* 识别到标签之后的处理 */
		/* 计算校验和 */
#if 0
		ucNewCheck = read_card_count_check(&stReadCardRevDate);
		ucOldCheck = read_card_get_check(&stReadCardRevDate);
		/* printf("ucNewCheck=%x   ucOldCheck=%x",ucNewCheck,ucOldCheck); */
		if(ucNewCheck != ucOldCheck)
		{
			printf("check sum is error \n");
			/* 校验失败后记录LOG，然后重新获取标签 */
			return(CARD_RET_FAIL);
		}
#endif

		lReg = read_card_check_sum(&stReadCardRevDate);
		if (CARD_RET_SUC != lReg) {
			printf("[%s][%d][%s]check sum is error \n", __FILE__, __LINE__,
					__FUNCTION__);
			/* 校验失败后记录LOG，然后重新获取标签 */
			return CARD_RET_FAIL;
		}

		if (0x00 != stReadCardRevDate.ucStatus) {
			/* 读卡器返回的状态不是0x00 */
			printf("read card return status is error");
			return (CARD_RET_FAIL);
		} else {
			return (0);
		}
	}
	return (CARD_RET_FAIL);
}

/* 查询读写器的工作天线 */
/* 无传参 */
/* 返回天线号 */
INT32 read_card_get_antenna(UINT8 *pucAntenna) {
	INT32 lReg = 0;
	UINT8 ucCheckSum = 0;
	UINT8 *pucCardCmd = NULL;
	UINT8 ucAntenna = 0;
	READ_CARD_CMD_T stReadCardSedCmd;
	READ_CARD_CMD_T stReadCardRevDate;

	memset(&stReadCardSedCmd, 0, sizeof(READ_CARD_CMD_T));
	memset(&stReadCardRevDate, 0, sizeof(READ_CARD_CMD_T));
	/* 填充命令 */
	stReadCardSedCmd.ucHead = CARD_CMD_HEAD;
	stReadCardSedCmd.ucAddr = CARD_ADDR;
	stReadCardSedCmd.ucLen = 0x02;
	stReadCardSedCmd.ucCmd = 0x28;
	read_card_count_check(&stReadCardSedCmd, &ucCheckSum);
	stReadCardSedCmd.aucDate[0] = ucCheckSum;
	lReg = read_card_sendmsg(&stReadCardSedCmd);

	if (CARD_RET_FAIL == lReg) {
		printf("read_card_sendmsg is error\n");
		/* 发送消息失败后做记录，做记录，然后退出 */
		return (lReg);
	}

	lReg = read_card_select(g_stReadCardClientInfo.lClientSockfd,
			&stReadCardRevDate);
	if ((0 == lReg) || (CARD_RET_FAIL == lReg)) {
		printf("read_card_select is time out (%d)\n", lReg);
		/* 接收消息失败或超时之后 做记录，然后退出 */
		return (CARD_RET_FAIL);
	} else {
		/* 识别到标签之后的处理 */
		/* 计算校验和 */
#if 0
		ucNewCheck = read_card_count_check(&stReadCardRevDate);
		ucOldCheck = read_card_get_check(&stReadCardRevDate);
		/* printf("ucNewCheck=%x   ucOldCheck=%x",ucNewCheck,ucOldCheck); */
		if(ucNewCheck != ucOldCheck)
		{
			printf("check sum is error \n");
			/* 校验失败后记录LOG，然后重新获取标签 */
			return(CARD_RET_FAIL);
		}
#endif

		lReg = read_card_check_sum(&stReadCardRevDate);
		if (CARD_RET_SUC != lReg) {
			printf("[%s][%d][%s]check sum is error \n", __FILE__, __LINE__,
					__FUNCTION__);
			/* 校验失败后记录LOG，然后重新获取标签 */
			return CARD_RET_FAIL;
		}

		if (0x00 != stReadCardRevDate.ucStatus) {
			/* 读卡器返回的状态不是0x00 */
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

/* 设置读写器工作天线 */
/* 设置的天线号 */
/* 返回0 成功 CARD_RET_FAIL 失败 */
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
	/* 填充命令 */
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
		/* 发送消息失败后做记录，做记录，然后退出 */
		return (lReg);
	}

	lReg = read_card_select(g_stReadCardClientInfo.lClientSockfd,
			&stReadCardRevDate);
	if ((0 == lReg) || (CARD_RET_FAIL == lReg)) {
		printf("read_card_select is time out (%d)\n", lReg);
		/* 接收消息失败或超时之后 做记录，然后退出 */
		return (CARD_RET_FAIL);
	} else {
		/* 识别到标签之后的处理 */
		/* 计算校验和 */
#if 0
		ucNewCheck = read_card_count_check(&stReadCardRevDate);
		ucOldCheck = read_card_get_check(&stReadCardRevDate);
		/* printf("ucNewCheck=%x   ucOldCheck=%x",ucNewCheck,ucOldCheck); */
		if(ucNewCheck != ucOldCheck)
		{
			printf("check sum is error \n");
			/* 校验失败后记录LOG，然后重新获取标签 */
			return(CARD_RET_FAIL);
		}
#endif
		lReg = read_card_check_sum(&stReadCardRevDate);
		if (CARD_RET_SUC != lReg) {
			printf("[%s][%d][%s]check sum is error \n", __FILE__, __LINE__,
					__FUNCTION__);
			/* 校验失败后记录LOG，然后重新获取标签 */
			return CARD_RET_FAIL;
		}

		if (0x00 != stReadCardRevDate.ucStatus) {
			/* 读卡器返回的状态不是0x00 */
			printf("read card return status is error");
			return (CARD_RET_FAIL);
		} else {
			return (0);
		}
	}
	return (CARD_RET_FAIL);
}

/* 设置读写器RS232通信波特率 */
/* 参数1：读写器的波特率0x00=9600bps；0x01=19200bps；0x02=38400bps；0x03=57600bps；0x04=115200bps */
/* 返回0 成功，CARD_RET_FAIL 失败 */
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

	/* 填充命令 */
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
		/* 发送消息失败后做记录，做记录，然后退出 */
		return (lReg);
	}

	lReg = read_card_select(g_stReadCardClientInfo.lClientSockfd,
			&stReadCardRevDate);
	if ((0 == lReg) || (CARD_RET_FAIL == lReg)) {
		printf("read_card_select is time out (%d)\n", lReg);
		/* 接收消息失败或超时之后 做记录，然后退出 */
		return (CARD_RET_FAIL);
	} else {
		/* 识别到标签之后的处理 */
		/* 计算校验和 */
#if 0
		ucNewCheck = read_card_count_check(&stReadCardRevDate);
		ucOldCheck = read_card_get_check(&stReadCardRevDate);
		/* printf("ucNewCheck=%x   ucOldCheck=%x",ucNewCheck,ucOldCheck); */
		if(ucNewCheck != ucOldCheck)
		{
			printf("check sum is error \n");
			/* 校验失败后记录LOG，然后重新获取标签 */
			return(CARD_RET_FAIL);
		}
#endif

		lReg = read_card_check_sum(&stReadCardRevDate);
		if (CARD_RET_SUC != lReg) {
			printf("[%s][%d][%s]check sum is error \n", __FILE__, __LINE__,
					__FUNCTION__);
			/* 校验失败后记录LOG，然后重新获取标签 */
			return CARD_RET_FAIL;
		}

		if (0x00 != stReadCardRevDate.ucStatus) {
			/* 读卡器返回的状态不是0x00 */
			printf("read card return status is error");
			return (CARD_RET_FAIL);
		} else {
			return (0);
		}
	}

	return (CARD_RET_FAIL);
}

/* 复位读写器 */
INT32 read_card_reset() {
	return 0;
}

/* 读卡器初始化函数 */
void read_card_init() {
	INT32 lReg = 0;

	/* 创建读卡器的客户端socket */
	g_stReadCardClientInfo.lClientSockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (CARD_RET_FAIL == g_stReadCardClientInfo.lClientSockfd) {
		printf("Card_Client Creat socket error\n");
		exit(1);
	}

	/* 设置socket 的card client参数 */
	g_stReadCardClientInfo.stCardClientSockaddr.sin_family = AF_INET;
	g_stReadCardClientInfo.stCardClientSockaddr.sin_port = htons(
			CARD_CLIENT_PORT);
	/* g_stReadCardClientInfo.stCardClientSockaddr.sin_addr.s_addr=INADDR_ANY; */
	g_stReadCardClientInfo.stCardClientSockaddr.sin_addr.s_addr = inet_addr(
			CARD_CLIENT_IPADDR);
	bzero(&(g_stReadCardClientInfo.stCardClientSockaddr), 8);

	/* 设置socket 的card server参数 */
	g_stReadCardClientInfo.stCardServerSockaddr.sin_family = AF_INET;
	g_stReadCardClientInfo.stCardServerSockaddr.sin_port = htons(
			CARD_SERVER_PORT);
	/* g_stReadCardClientInfo.stCardServerSockaddr.sin_addr.s_addr=INADDR_ANY; */
	g_stReadCardClientInfo.stCardServerSockaddr.sin_addr.s_addr = inet_addr(
			CARD_SERVER_IPADDR);
	bzero(&(g_stReadCardClientInfo.stCardClientSockaddr), 8);

	/* 绑定socket ip和端口 */
	lReg = bind(g_stReadCardClientInfo.lClientSockfd,
			(struct sockaddr *) &g_stReadCardClientInfo.stCardClientSockaddr,
			sizeof(struct sockaddr));
	if (CARD_RET_FAIL == lReg) {
		printf("Card_Client Bind socket error\n");
		close(g_stReadCardClientInfo.lClientSockfd);
		exit(1);
	}

	/* 连接读卡器的TCP服务端 */
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

/* 读卡器发送命令                      */
/* 参数1:读卡器命令结构的指针           */
/* 返回 CARD_RET_FAIL 发送失败，大于0，发送的字节数 */
INT32 read_card_sendmsg(READ_CARD_CMD_PT pstReadCardCmd) {
	INT32 lSendLen = 0;

	/* 发送数据到card 的服务端 */
	lSendLen = send(g_stReadCardClientInfo.lClientSockfd,
			(const UINT8 *) pstReadCardCmd, (pstReadCardCmd->ucLen + 3), 0);
	if (CARD_RET_FAIL == lSendLen) {
		printf("Send msg to server error\n");
		return (lSendLen);
	}
	printf("To Read card Send len :%d\n", lSendLen);

	return (lSendLen);
}

/* 设置等待的socket，返回超时或socket值         */
/* 参数1:等待的socket id                       */
/* 参数2:存放接收到数据的结构体指针             */
/* 返回0：超时，-1：出错，大于0：接收到得字节数 */
INT32 read_card_select(INT32 lWaitSocket, READ_CARD_CMD_PT pstRecvDate) {
	INT32 lReg = 0;
	INT32 lSendLen = 0;
	fd_set ReadCardReadFds;
	struct timeval stTimeVal;

	/* 设置等待的超时时间 */
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
/* 读卡器的处理主线程函数 */
void read_card_main_proc() {
	INT32 lReg = 0;
	UINT8 ucCheckSum = 0;
	INT32 lReadCardVersion = 0;
	INT32 lSendLen = 0;
	UINT8 ucTagNumber = 0;
	READ_CARD_CMD_T stReadCardCmd;
	READ_CARD_CMD_T stReadCardDate;

	printf("this is in pthread ...[%s][%s][%d] \n", FILE_LINE);
	/* 初始化读卡器 */
	read_card_init();
	/* 获取读卡器版本失败 进程退出 */
	lReadCardVersion = read_card_get_version();
	if (CARD_RET_FAIL == lReadCardVersion) {
		printf("get read card version is error");
		exit(0);
	}
	g_stReadCardClientInfo.ucMajorVer = (UINT8) (lReadCardVersion >> 8);
	g_stReadCardClientInfo.ucMinorVer = (UINT8) (lReadCardVersion);
	/* 获取读卡器的power和freq */
	lReg = read_card_get_freq(&g_stReadCardClientInfo.ucPower,
			&g_stReadCardClientInfo.ucFreq);
	if (CARD_RET_FAIL == lReg) {
		printf("get read card power/freq  is error");
		exit(0);
	}
	/* 获取读卡器的天线号*/
	lReg = read_card_get_antenna(&g_stReadCardClientInfo.ucAntenna);
	if (CARD_RET_FAIL == lReg) {
		printf("get read card Antenna  is error");
		exit(0);
	}
	/* 获取读卡器的版本号 
	 lTmpNumber = read_card_get_version();
	 if(lTmpNumber)
	 g_stReadCardClientInfo.ucMajorVer = (UINT8)((lTmpNumber>>8)&0xff);
	 g_stReadCardClientInfo.ucMinorVer = (UINT8)(lTmpNumber&0xff);
	 */
	while (1) {
		/* 组装发送读卡器的多标签识别命令 */
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
			/* 发送消息失败后做记录，然后继续发送消息 */
			continue;
		}
		lReg = read_card_select(g_stReadCardClientInfo.lClientSockfd,
				&stReadCardDate);
		if ((0 == lReg) || (CARD_RET_FAIL == lReg)
				|| (0x00 != stReadCardDate.ucStatus)) {
			printf("read_card_select is time out (%d)\n", lReg);
			/* 接收消息失败或超时之后 做记录，然后重走流程重新获取标签 */
			continue;
		} else {
			/* 识别到标签之后的处理 */
			/* 计算校验和 */
#if 0
			ucNewCheck = read_card_count_check(&stReadCardDate);
			ucOldCheck = read_card_get_check(&stReadCardDate);
			/* printf("ucNewCheck=%x   ucOldCheck=%x",ucNewCheck,ucOldCheck); */
			if(ucNewCheck != ucOldCheck)
			{
				printf("check sum is error \n");
				/* 校验失败后记录LOG，然后重新获取标签 */
				continue;
			}
#endif

			lReg = read_card_check_sum(&stReadCardDate);
			if (CARD_RET_SUC != lReg) {
				printf("[%s][%d][%s]check sum is error \n", __FILE__, __LINE__,
						__FUNCTION__);
				/* 校验失败后记录LOG，然后重新获取标签 */
				continue;
			} else {
				ucTagNumber =
						*((UINT8 *) (&stReadCardDate) + CARD_CMD_TAG_BYTE);
			}
		}
		/* 根据获取的标签数量，组装发送读卡器的获取缓冲区数据命令 */
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
			/* 发送消息失败后做记录，然后继续重走流程 */
			continue;
		}
		lReg = read_card_select(g_stReadCardClientInfo.lClientSockfd,
				&stReadCardDate);
		if ((0 == lReg) || (CARD_RET_FAIL == lReg)) {
			printf("read_card_select is time out (%d)\n", lReg);
			/* 接收消息失败或超时之后 做记录，然后重走流程重新获取标签 */
			continue;
		} else {
			/* 识别到标签之后的处理 */
			/* 计算校验和 */
#if 0
			ucNewCheck = read_card_count_check(&stReadCardDate);
			ucOldCheck = read_card_get_check(&stReadCardDate);
			if(ucNewCheck != ucOldCheck)
			{
				printf("check sum is error \n");
				/* 校验失败后记录LOG，然后重新获取标签 */
				continue;
			}
#endif

			lReg = read_card_check_sum(&stReadCardDate);
			if (CARD_RET_SUC != lReg) {
				printf("[%s][%d][%s]check sum is error \n", __FILE__, __LINE__,
						__FUNCTION__);
				/* 校验失败后记录LOG，然后重新获取标签 */
				continue;
			} else {
				printf("it is over ...\n");
			}
		}
	}

	return;
}

/* 主进程入口 */
INT32 main() {
	INT32 lReg = 0;
	INT32 lSendLen = 0;
	pthread_t ReadCardPthreadId;
	pthread_t CamerPthreadId;
	READ_CARD_CMD_T stReadCardCmd;

	printf("this is in main ...[%s][%s][%d] \n", FILE_LINE);
	/* memset(g_stReadCardClientInfo.Client_Ip,0,sizeof(g_stReadCardClientInfo.Client_Ip)); */
	/* 创建读卡器的线程 */
	lReg = pthread_create(&ReadCardPthreadId, NULL,
			(void *) read_card_main_proc, NULL);
	if (0 != lReg) {
		printf("Create read card pthread error!\n");
		exit(1);
	}

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

