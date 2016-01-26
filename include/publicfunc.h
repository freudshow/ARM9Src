#ifndef				_PUBLICFUNC_H
#define				_PUBLICFUNC_H	

#define MERGETWOBYTE(byt_h,byt_l)  (((uint16)(byt_h)<<8)+byt_l)
#define LOWBYTEOF(dbyte)   ((uint8)(dbyte))
#define HIGHBYTEOF(dbyte)   ((uint8)((dbyte)>>8))

#define	HEX_TO_ASCII(x)				((x<=0x09)?(x+0x30):(x+0x37))
#define UINT8_TO_ASCII_H(x)			HEX_TO_ASCII((x&0x0F))							
#define UINT8_TO_ASCII_L(x)			HEX_TO_ASCII((x&0x0F))

#define  NO_ERR 0 
#define  ERR_1  1 
#define  ERR_2  2 
#define  ERR_3  3 
#define  ERR_4  4 
#define  ERR_5  5 

/*===============================================*/
extern void CopyNHword(uint16* p1, uint16* p2, uint32 n);
extern uint8 CmpNByte(uint8* No1, uint8* No2, uint32 n);
//===============================================
extern void SetMeterNo(uint8* No1, uint8 data);
extern void CopyMeterNo(uint8* No1, uint8* No2);
extern uint8 CmpMeterNo(uint8* No1, uint8* No2);
extern int CmpMeterNoHL(uint8* No1, uint8* No2);
extern void CopyEle(uint8* Ele1, uint8* Ele2);
extern uint8 CmpEle(uint8* Ele1, uint8* Ele2);
//===============================================
extern void CopyMeterNoFrm(uint8* No1, uint8* No2);
extern uint8 CmpMeterNoFrm(uint8* No1, uint8* No2);
extern uint8 CmpEleFrm(uint8* Ele1, uint8* Ele2);
//===============================================
extern uint8 HexToBcd(uint8 hexData);
extern uint8 BcdToHex(uint8 bcdData);
extern uint16 BcdToHex_16bit(uint16 hexData); //ZXL 08.06.09���
//===============================================
extern uint8 BcdCheck(uint8 Data);
extern uint8 BcdCheck_n(uint8 *pTemp, uint8 n);
extern uint8 MeterNoBcdCheck(uint8* No1);
//===============================================
extern uint8 IsLeap(uint8 year);
extern void CountDate(uint8* TheDate, uint8 AddDay);
extern void CountMinute(uint8 *TheDate, uint8 AddMinute); //�����ӽ������    ע�����ӷ��Ӳ��ܳ���1Сʱ
extern void CountMonth(uint8* TheDate);  //�� �� ���¼�1  //zxl 08.06.08
extern void CountHour(uint8* TheDate); //ʱ �� �� �� �е�ʱ+1//zxl 08.06.25
extern void SubDate(uint8* TheDate); //�� �� �� �� �е��� �� 1 
extern void SubMonth(uint8* TheDate); //��  �� �� �е��� �� 1 
extern uint8 GetLastDayofMonth(uint8 MonthBcd, uint8 YearBcd);
extern void SubDate1(uint8* StartDate, uint8* LastDate, uint8* ReDate); //��������������õ�ʱ���
//================================================zxl 08.04.20���
extern uint8 AsciiToHex(uint8 Asciicode);
extern uint8 HexToAscii(uint8 Hexcode);
extern uint8 Hex_BcdToAscii(uint16 Hex_bcd_code, uint8* AscBuf); //16λ��HEX����BCD��ת��ΪASCII�룬Ȼ�󷴷�������AscBuf�У�����Ϊnum
extern uint8 Hex_BcdToAscii_8bit(uint8 Hex_bcd_code, uint8* AscBuf); //8λ��HEX����BCD��ת��ΪASCII�룬Ȼ������������AscBuf�У�����Ϊ2   yb add 101101

extern uint32 AsciiToBcd_16bit(uint8* pBuf); //����Ϊ5���ֽڵ�ASC�����飬ת��Ϊһ��16λ��2�ֽڣ���BCD���֣�����{30��38��30��30��31}��ת��Ϊ8001
extern int8 Reversalmemcmp(uint8* p1, uint8* p2, uint8 len); //��ת�Ƚϴ�С
extern void Reversalmemcpy(uint8 *destin, uint8 *source, uint16 n); //��Դ��ַ����N���ֽڷ���浽Ŀ�ĵ�ַ 
extern uint8 TimeCheck(uint8* pTime); //���ں�ʱ���麯����˳��Ϊ  ��-��-ʱ-��-��-�꣬��ȷ����0�����ڴ��󷵻�1��ʱ����󷵻�2
extern uint8 ASCIPToHex(uint8* pStr, uint16 *buf); //�����ֽڵ�ASC���IPת��Ϊһ���ֽڵ�HEX
extern uint8 ConverHexToASCIP(uint8* pTemp1, uint8* pTemp2); //16����ת��ΪASCII��IP������C0 A8 01 08 ת��Ϊ192.168.1.8,����ֵΪת����IP�ĳ���
extern uint8 ConverASCIPToHex(uint8* pTemp1, uint8* pTemp2); //��ASCII���IP��ַת��Ϊ4�ֽ�16���Ƶ�����
extern uint8 Bcd645DataToAscii(uint8* pAscii, uint8* pBcdData, uint8 n,
		uint8 m); //BCD����ת��ΪASCII�ַ��������ְ���645Э��ı�׼����,n:m��������ʽ����ʾ����������n,С������
extern uint8 Bcd645DataToAsciiTest(uint8* pAscii, uint8* pBcdData, uint8 len);
extern uint16 DecToHex_16bit(uint16 Dec); //2�ֽڵ�10����ת��Ϊ16����,���뷶Χ��0~65535
//=================================================yb 09.01.14 add
extern uint8 CmpData(uint8 *Target, uint8 *Current); //�Ƚ������յĴ�С      �������Ŀ�������򷵻�2��С���򷵻�1�������򷵻�0
extern uint8 CmpDate_H(uint8 *Target, uint8 *Current); //�Ƚ�ʱ������Ĵ�С      �������Ŀ�������򷵻�2��С���򷵻�1�������򷵻�0   2000-2099
extern uint16 abs_value(uint16 a, uint16 b);
extern uint16 x_n_value(uint16 x, uint8 n);
extern void Remove_R4(uint8 *a, uint8 mm);
extern void Remove_L4(uint8 *a, uint8 mm);
extern uint8 JudgeAvail(uint8*pdata, uint8 bit);           //�ж�λ�Ƿ���Ч(bin format)
extern void gpHex16ToStr(uint16 x, char* str);
extern uint32 Uint32HexToBCD(uint32 mm);
extern uint32 Uint32BCDToHex(uint32 mm);
extern uint8 AddBcdByte(uint8 byte1, uint8 byte2, uint8* pCarry); //����pCarryΪָ���λ��ָ��
extern uint8 AddBcdStr(uint8* str1, uint8* str2, uint8 len); //����BCD����ӣ�������ڵ�һ������
//����Bcd�ֽڼ�����byte2-byte1,����pCarryΪָ���λ��ָ��
extern uint8 SubBcdByte(uint8 byte1, uint8 byte2, uint8* pCarry);
extern uint8 SubBcdStr(uint8* str1, uint8* str2, uint8 len); //����BCD�������������ڵ�һ������
//����BCD����ӣ�������ڵ�һ������,�������λΪ����λ
extern uint8 AddBcdStrWithSign(uint8* str1, uint8* str2, uint8 len);
//����BCD�������str2-str1 ������ڵ�һ������,�������λΪ����λ
extern uint8 SubBcdStrWithSign(uint8* str1, uint8* str2, uint8 len);
extern uint8 CmpStr(uint8 *str1, uint8 *str2, uint8 len);

extern int8 CmpStrAbsolute(uint8 *str1, uint8 *str2, uint8 len);

extern uint8 CmpStr1(uint8 *str1, uint8 *str2, uint8 len);
/////////////////////////////////////////////////////////////////////////////////
//���ݸ�ʽ����
extern uint8 CountFormatA2(uint8 pData[4], uint8 decNum);
void CountV_645_97(uint8* pData);                       //����97���ѹΪA.7
void CountI_645_97(uint8* pI1, uint8* pI2);      //��97����������pI1����Ϊ376��1���ݸ�ʽ25 pI2

//==================================================================================================================================
//===================================================����ն�Խ�޲������ĺ���=======================================================
extern int8 CmpMeVolata(uint8 *data, int32 Vol);                    //  �õ���ѹ�ıȽ�ֵ
extern int8 CmpMeElcurrent(uint8 *data, int32 Elc);                  // �õ������ıȽ�ֵ

extern uint8 CheckDT(uint8* DTBcd);
extern uint8 Zeller(uint8 year, uint8 month, uint8 day);
extern void Zeller_DTBcd(uint8* DTBcd);
extern uint8 CmpDateTime(uint8* pTarget, uint8* pCurrent, uint8 CmpFlag);
extern uint8 GetChannelType(void);                       //�õ���ǰͨ������
extern uint8 GetChannelTypeFromFlash(void); //��flash�ж�ȡ��ǰͨ�����ͣ���Ϊ��ȫ�ֱ�����ʼ��֮ǰ���޷�ʹ�ú���GetChannelType
extern int CmpCurTime(uint8* pTime);
extern uint8 JudgeSameDayMonthAndYear(uint8 *CurrentDay, uint8* AskDate); //�ж����ǲ���ͬһ��
extern uint8 JudgeSameMonth(uint8 *CurrentMonth, uint8* AskDate);
extern uint8 JudgeSameMonthAndYear(uint8 *CurrentMonth, uint8* AskDate); //�ж����ǲ���ͬһ��
extern uint8 JudgeSameMonthAndYear_1(uint8 *CurrentValue, uint8 *AskValue);
extern uint8 JudgeSameDayMonthAndYear2(uint8 *CurrentDay, uint8* AskDate);
extern uint8 CmpData_M(uint8 *Target, uint8 *Current);
extern void GotoNextTime(uint8 time[5], uint8 FreezeInt);
extern void AdjectStartTime(uint8 time[5], uint8 FreezeInt);
extern uint8 JudgeMeterZero(uint8 *pdata, uint8 flag);
extern uint8 PUBLIC_CountCS(uint8* _data, uint16 _len);
extern char* PUBLIC_HexToString(uint8 *pIn, uint16 InLen, char *pOut,
		char AddFlag);
extern char* PUBLIC_MeterAddrToString(uint8 *pMeterAddrIn, char *pMeterOut);

extern uint16 HexToBcd2bit(uint8 hexData);
//extern uint16 HexToBcd1(uint8 hexData);
extern uint32 HexToBcd4bit(uint16 hexData);

extern uint32 BcdToHex_16bit1(uint32 mm); //duan kou hao 
extern uint8 Uint16BCDToHex1(uint16 mm);

extern char* PUBLIC_BCDToString(char *pMeterOut, const uint8 *pMeterAddrIn,
		uint8 len);
extern uint32 StrToBin(char *str, uint8 *array);
extern int StrToBin_MeterAddress(char *str, uint8 *array, uint8 Meter_type);

extern void stringCopy(char *dest, char *original, char omit);
#endif
