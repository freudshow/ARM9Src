#include "includes.h"

/*************************************************
 * xx_xx_R：本地接收上位机的xml数据
 * xx_xx_T：本地发送给上位机的xml数据
 *************************************************/
id_validate gxml_IDInfo_R, gxml_IDInfo_T;          //接收身份验证信息
common gxml_ComInfo_R, gxml_ComInfo_T;         //通用部分
heart_beat gxml_BeatInfo_R, gxml_BeatInfo_T;        //心跳/校时
config gxml_CfgInfo_R, gxml_CfgInfo_T;         //配置信息
//data   gxml_DataInfo_R = {"reply","123","yes","20130514","100","20",{"1",{{"8","xx","xx","xx"},{"0","xx","xx","xx"},{"1","xx","xx","xx"},{"2","xx","xx","xx"},}}};
data gxml_DataInfo_R, gxml_DataInfo_T;        //能耗远传数据
ipaddr gxml_IPInfo_R, gxml_IPInfo_T;          //ip地址信息
struct filetrans gxml_TransInfo_R, gxml_TransInfo_T;  //文件传输
struct query_config gxml_QueryInfo_R, gxml_QueryInfo_T;  //查询配置 
struct ex_config gxml_ExInfo_R, gxml_ExInfo_T;       //额外配置 
struct update_key gxml_KeyInfo_R, gxml_KeyInfo_T;      //密钥更新
struct control gxml_CtlInfo_R, gxml_CtlInfo_T;      //服务器控制
struct xml_modify_req_t gxml_modify_t;
struct xml_modify_rsp_t gxml_modify_rsp_t;
struct xml_realtime_req_t gxml_realtime_req_t;
struct xml_realtime_rsp_t gxml_realtime_rsp_t;
struct xml_reportsrv_req_t gxml_reportsrv_req_t;
struct xml_reportsrv_rsp_t gxml_reportsrv_rsp_t;

/*yangfei added 默认密钥*/
extern System_ConfigType gSystem_Config;
//static uint8 gTestbuf[2000];

/*
 */
static int parserxml(xmlNodePtr rootNode) {
	int i;
	int contentId = 0;

	xmlChar *propvalue = NULL;  //属性值
	xmlNodePtr curNode = rootNode->children;   //common 节点
	xmlChar *tmp;
	xmlNodePtr sonNode, propNodePtr, grandsonNode;

	while (NULL != curNode) {
		printf("%s\n", curNode->name);
		sonNode = curNode->children;
		if (!xmlStrcmp(curNode->name, BAD_CAST "common")) {//通用部分
			while (NULL != sonNode) {
				if (!xmlStrcmp(sonNode->name, BAD_CAST "building_id")) {
					tmp = xmlNodeGetContent(sonNode);  //取出节点中的内容
					strcpy((char*) gxml_ComInfo_R.building_id, (char*) tmp);
					debug_debug(gDebugModule[XML_MODULE], "building_id=%s\n",
							gxml_ComInfo_R.building_id);
				}
				if (!xmlStrcmp(sonNode->name, BAD_CAST "gateway_id")) {
					tmp = xmlNodeGetContent(sonNode);  //取出节点中的内容
					strcpy((char*) gxml_ComInfo_R.gateway_id, (char*) tmp);
					debug_debug(gDebugModule[XML_MODULE], "gateway_id=%s\n",
							gxml_ComInfo_R.gateway_id);
				}
				if (!xmlStrcmp(sonNode->name, BAD_CAST "type")) {
					tmp = xmlNodeGetContent(sonNode);  //取出节点中的内容
					strcpy((char*) gxml_ComInfo_R.type, (char*) tmp);
					printf("type=%s\n", gxml_ComInfo_R.type);
					debug_debug(gDebugModule[XML_MODULE], "gateway_id=%s\n",
							gxml_ComInfo_R.gateway_id);
				}
				sonNode = sonNode->next;
			}
		}
		if (xmlHasProp(curNode, BAD_CAST "operation")) {//查找带有属性operation的节点
			propNodePtr = curNode;
			if (!xmlStrcmp(propNodePtr->name, BAD_CAST "id_validate")) //身份验证数据包
					{
				propvalue = xmlGetProp(propNodePtr, BAD_CAST "operation"); //获取属性值
				strcpy((char*) gxml_IDInfo_R.operation, (char*) propvalue);
				printf("operation is %s\n", gxml_IDInfo_R.operation);
				sonNode = propNodePtr->children;
				while (NULL != sonNode) {
					if (!xmlStrcmp(sonNode->name, BAD_CAST "sequence")) {
						printf("Rcv  XML sequence  !\n");
						tmp = xmlNodeGetContent(sonNode);  //取出节点中的内容
						strcpy((char*) gxml_IDInfo_R.sequence, (char*) tmp);
						printf("sequence=%s\n", gxml_IDInfo_R.sequence);
						printf("sequence num =%d\n",
								strlen((char*) gxml_IDInfo_R.sequence));
					} else if (!xmlStrcmp(sonNode->name, BAD_CAST "result")) {
						tmp = xmlNodeGetContent(sonNode);   //取出节点中的内容
						strcpy((char*) gxml_IDInfo_R.result, (char*) tmp);
						printf("result=%s\n", gxml_IDInfo_R.result);
					}
					sonNode = sonNode->next;                     //获取兄弟节点 
				}
			} else if (!xmlStrcmp(propNodePtr->name, BAD_CAST "heart_beat")) {//心跳校时数据包
				propvalue = xmlGetProp(propNodePtr, BAD_CAST "operation"); //获取属性值
				strcpy((char*) gxml_BeatInfo_R.operation, (char*) propvalue);
				sonNode = propNodePtr->children;
				while (NULL != sonNode) {
					if (!xmlStrcmp(sonNode->name, BAD_CAST "time")) {
						tmp = xmlNodeGetContent(sonNode);  //取出节点中的内容
						strcpy((char*) gxml_BeatInfo_R.time, (char*) tmp);
						printf("time is %s\n", gxml_BeatInfo_R.time);
					}
					sonNode = sonNode->next;
				}
			} else if (!xmlStrcmp(propNodePtr->name, BAD_CAST "data")) //能耗远传数据包
					{
				propvalue = xmlGetProp(propNodePtr, BAD_CAST "operation"); //获取属性值
				strcpy((char*) gxml_DataInfo_R.operation, (char*) propvalue);
				printf("data operation is %s\n", gxml_DataInfo_R.operation);
			} else if (!xmlStrcmp(propNodePtr->name, BAD_CAST "config"))  //配置信息
					{
				propvalue = xmlGetProp(propNodePtr, BAD_CAST "operation"); //获取属性值	
				strcpy((char*) gxml_IPInfo_R.operation, (char*) propvalue);
				printf("IP addr operation is %s\n", gxml_IPInfo_R.operation);
				sonNode = propNodePtr->children;
				if (!xmlStrcmp(propvalue, BAD_CAST "period")) {
					tmp = xmlNodeGetContent(sonNode);
					strcpy((char*) gxml_CfgInfo_R.period, (char*) tmp);
					debug_debug(gDebugModule[XML_MODULE],
							"config period is %s\n", gxml_CfgInfo_R.period);
				} else if (!xmlStrcmp(propvalue, BAD_CAST "ip_port")) {
					printf("node is %s\n", sonNode->name);
					i = 0;
					while (NULL != sonNode) {
						grandsonNode = sonNode->children;
						debug_debug(gDebugModule[XML_MODULE],
								"grandsonNode is %s\n", grandsonNode->name);
						tmp = xmlNodeGetContent(grandsonNode);
						strcpy((char*) gxml_IPInfo_R.server[i].ip, (char*) tmp);
						debug_debug(gDebugModule[XML_MODULE],
								"ipconfig ip is %s\n",
								gxml_IPInfo_R.server[i].ip);

						grandsonNode = grandsonNode->next;
						debug_debug(gDebugModule[XML_MODULE],
								"grandsonNode is %s\n", grandsonNode->name);
						tmp = xmlNodeGetContent(grandsonNode);
						strcpy((char*) gxml_IPInfo_R.server[i].dns,
								(char*) tmp);
						debug_debug(gDebugModule[XML_MODULE],
								"ipconfig dns is %s\n",
								gxml_IPInfo_R.server[i].dns);

						grandsonNode = grandsonNode->next;
						debug_debug(gDebugModule[XML_MODULE],
								"grandsonNode is %s\n", grandsonNode->name);
						tmp = xmlNodeGetContent(grandsonNode);
						strcpy((char*) gxml_IPInfo_R.server[i].port,
								(char*) tmp);
						debug_debug(gDebugModule[XML_MODULE],
								"ipconfig port is %s\n",
								gxml_IPInfo_R.server[i].port);
						sonNode = sonNode->next;
						i++;
					}
				}
			} else if (!xmlStrcmp(propNodePtr->name, BAD_CAST "filetransfer")) {//文件传输数据包
				propvalue = xmlGetProp(propNodePtr, BAD_CAST "operation");
				strcpy((char*) gxml_TransInfo_R.operation, (char*) propvalue);
				printf("file transfer operation is %s\n",
						gxml_TransInfo_R.operation);
				sonNode = propNodePtr->children;
				if (!xmlStrcmp(propvalue, BAD_CAST "file_data")) {
					while (NULL != sonNode) {
						if (!xmlStrcmp(sonNode->name, BAD_CAST "pack_total")) {
							tmp = xmlNodeGetContent(sonNode);  //取出节点中的内容
							strcpy((char*) gxml_TransInfo_R.pack_total,
									(char*) tmp);
							printf("pack total is %s\n",
									gxml_TransInfo_R.pack_total);
						} else if (!xmlStrcmp(sonNode->name,
								BAD_CAST "pack_sequence")) {
							tmp = xmlNodeGetContent(sonNode);  //取出节点中的内容
							strcpy((char*) gxml_TransInfo_R.pack_sequence,
									(char*) tmp);
							printf("pack sequence is %s\n",
									gxml_TransInfo_R.pack_sequence);
						} else if (!xmlStrcmp(sonNode->name,
								BAD_CAST "file_type")) {
							tmp = xmlNodeGetContent(sonNode);  //取出节点中的内容
							strcpy((char*) gxml_TransInfo_R.file_type,
									(char*) tmp);
							printf("file type is %s\n",
									gxml_TransInfo_R.file_type);
						} else if (!xmlStrcmp(sonNode->name,
								BAD_CAST "file_data")) {
							tmp = xmlNodeGetContent(sonNode);  //取出节点中的内容
							strncpy((char*) gxml_TransInfo_R.file_bidata,
									(char*) tmp,
									sizeof(gxml_TransInfo_R.file_bidata) - 1);
							debug_debug(gDebugModule[XML_MODULE],
									"file data is %s\n",
									gxml_TransInfo_R.file_bidata);
						}

						sonNode = sonNode->next;
					}
				} else if (!xmlStrcmp(propvalue, BAD_CAST "pack_lost")) {
					//do noting
				} else if (!xmlStrcmp(propvalue, BAD_CAST "Trans_req")) {
					while (NULL != sonNode) {
						if (!xmlStrcmp(sonNode->name,
								BAD_CAST "Trans_req_flag")) {
							tmp = xmlNodeGetContent(sonNode);  //取出节点中的内容
							strncpy((char*) gxml_TransInfo_R.trans_req_flag,
									(char*) tmp,
									sizeof(gxml_TransInfo_R.trans_req_flag)
											- 1);
							printf("Trans_req is %s\n",
									gxml_TransInfo_R.trans_req_flag);
						}
						sonNode = sonNode->next;
					}
				}
			}
			else if (!xmlStrcmp(propNodePtr->name, BAD_CAST "query")) {  //查询配置
					
				propvalue = xmlGetProp(propNodePtr, BAD_CAST "operation");
				strcpy((char*) gxml_QueryInfo_R.operation, (char*) propvalue);
				printf("query operation is %s\n", gxml_QueryInfo_R.operation);
			}
			else if (!xmlStrcmp(propNodePtr->name, BAD_CAST "ex_config")) {//额外配置
				propvalue = xmlGetProp(propNodePtr, BAD_CAST "operation");
				strcpy((char*) gxml_ExInfo_R.operation, (char*) propvalue);
				printf("ex config operation is %s\n", gxml_ExInfo_R.operation);
				sonNode = propNodePtr->children;
				if (!xmlStrcmp(propvalue, BAD_CAST "id_config")) {
					while (NULL != sonNode) {
						if (!xmlStrcmp(sonNode->name,
								BAD_CAST "new_building_id")) {
							tmp = xmlNodeGetContent(sonNode);  //取出节点中的内容
							strcpy((char*) gxml_ExInfo_R.new_building_id,
									(char*) tmp);
							printf("new building id is %s\n",
									gxml_ExInfo_R.new_building_id);
						} else if (!xmlStrcmp(sonNode->name,
								BAD_CAST "new_gateway_id")) {
							tmp = xmlNodeGetContent(sonNode);  //取出节点中的内容
							strcpy((char*) gxml_ExInfo_R.new_gateway_id,
									(char*) tmp);
							printf("new gateway id is %s\n",
									gxml_ExInfo_R.new_gateway_id);
						}
						sonNode = sonNode->next;
					}
				}
				else if (!xmlStrcmp(propvalue, BAD_CAST "report_mode")) {
					while (NULL != sonNode) {
						if (!xmlStrcmp(sonNode->name, BAD_CAST "report_mode")) {
							tmp = xmlNodeGetContent(sonNode);  //取出节点中的内容
							strcpy((char*) gxml_ExInfo_R.report_mode,
									(char*) tmp);
							printf("report mode is %s\n",
									gxml_ExInfo_R.report_mode);
						}
						sonNode = sonNode->next;
					}
				}
			}
			else if (!xmlStrcmp(propNodePtr->name, BAD_CAST "update")) {//密钥更新
				propvalue = xmlGetProp(propNodePtr, BAD_CAST "operation");
				strcpy((char*) gxml_KeyInfo_R.operation, (char*) propvalue);
				printf("update key operation is %s\n",
						gxml_KeyInfo_R.operation);
				sonNode = propNodePtr->children;
				if (!xmlStrcmp(propvalue, BAD_CAST "md5_key")) {
					while (NULL != sonNode) {
						if (!xmlStrcmp(sonNode->name, BAD_CAST "md5_keydata")) {
							tmp = xmlNodeGetContent(sonNode);  //取出节点中的内容
							strcpy((char*) gxml_KeyInfo_R.md5_key, (char*) tmp);
							printf("md5 key is %s\n", gxml_KeyInfo_R.md5_key);
						}
						sonNode = sonNode->next;
					}
				}
				else if (!xmlStrcmp(propvalue, BAD_CAST "aes_key")) {
					while (NULL != sonNode) {
						if (!xmlStrcmp(sonNode->name, BAD_CAST "aes_keydata")) {
							tmp = xmlNodeGetContent(sonNode);  //取出节点中的内容
							strcpy((char*) gxml_KeyInfo_R.aes_key, (char*) tmp);
							printf("aes key is %s\n", gxml_KeyInfo_R.aes_key);
						}
						sonNode = sonNode->next;
					}
				}
			} 
			else if (!xmlStrcmp(propNodePtr->name, BAD_CAST "control")) {//服务器控制
				propvalue = xmlGetProp(propNodePtr, BAD_CAST "operation");
				strcpy((char*) gxml_CtlInfo_R.operation, (char*) propvalue);
				printf("control operation is %s\n", gxml_CtlInfo_R.operation);
			}
			/*begin add by yangmin - 20130829*/
			else if (!xmlStrcmp(propNodePtr->name, BAD_CAST "modify")) {
				propvalue = xmlGetProp(propNodePtr, BAD_CAST "num");
				gxml_modify_t.num = atoi((const char *) propvalue);
				if (gxml_modify_t.num
						<= 0|| gxml_modify_t.num > MAX_XML_NODE_NUM) {
					debug_err(gDebugModule[XML_MODULE],
							"[%s][%s][%d]  the num = %d error!\n", FILE_LINE,
							gxml_modify_t.num);
				}
				sonNode = propNodePtr->children;
				while (sonNode != NULL) {
					if (!xmlStrcmp(sonNode->name, BAD_CAST "content")) {
						propvalue = xmlGetProp(sonNode, BAD_CAST "id");
						contentId = atoi((const char *) propvalue);
						if (contentId <= 0 || contentId > gxml_modify_t.num) {
							debug_err(gDebugModule[XML_MODULE],
									"[%s][%s][%d]  the contend id = %d error!\n",
									FILE_LINE, contentId);
						}
						tmp = xmlNodeGetContent(sonNode);  //取出节点中的内容
						strcpy(
								(char*) gxml_modify_t.content[contentId - 1].sqlStr,
								(char*) tmp);
						debug_info(gDebugModule[XML_MODULE],
								"the %d sql =  %s\n", contentId,
								gxml_modify_t.content[contentId - 1].sqlStr);
					}
					sonNode = sonNode->next;
				}
			}

			else if (!xmlStrcmp(propNodePtr->name, BAD_CAST "report_srv")) {
				sonNode = propNodePtr->children;
				if (sonNode != NULL) {
					if (!xmlStrcmp(sonNode->name, BAD_CAST "condition")) {

						tmp = xmlNodeGetContent(sonNode);  //取出节点中的内容
						strcpy((char*) gxml_reportsrv_req_t.query, (char*) tmp);
						debug_info(gDebugModule[XML_MODULE], "the sql =  %s\n",
								gxml_reportsrv_req_t.query);
					}
					sonNode = sonNode->next;
				}
			}

			else if (!xmlStrcmp(propNodePtr->name, BAD_CAST "realtime_qry")) {
				sonNode = propNodePtr->children;
				while (sonNode != NULL) {
					if (!xmlStrcmp(sonNode->name, BAD_CAST "item")) {
						propvalue = xmlGetProp(sonNode, BAD_CAST "type");
						StrToBin((char *) propvalue, &gxml_realtime_req_t.type);
						if (gxml_realtime_req_t.type != (uint8) 0x10
								&& gxml_realtime_req_t.type != (uint8) 0x20
								&& gxml_realtime_req_t.type != (uint8) 0x21
								&& gxml_realtime_req_t.type != (uint8) 0x25) {
							debug_err(gDebugModule[XML_MODULE],
									"[%s][%s][%d]  the type = %d error!\n",
									FILE_LINE, gxml_realtime_req_t.type);
						}
						propvalue = xmlGetProp(sonNode, BAD_CAST "ProtocolVer");
						gxml_realtime_req_t.protocolVer = atoi(
								(const char *) propvalue);
						if (gxml_realtime_req_t.protocolVer < 0
								|| gxml_realtime_req_t.protocolVer > 1) {
							debug_err(gDebugModule[XML_MODULE],
									"[%s][%s][%d]  the protocolVer = %d error!\n",
									FILE_LINE, gxml_realtime_req_t.protocolVer);
						}
						propvalue = xmlGetProp(sonNode, BAD_CAST "channel");
						gxml_realtime_req_t.channel = atoi(
								(const char *) propvalue);
						if (gxml_realtime_req_t.channel < 1
								|| gxml_realtime_req_t.channel > 8) {
							debug_err(gDebugModule[XML_MODULE],
									"[%s][%s][%d]  the channel = %d error!\n",
									FILE_LINE, gxml_realtime_req_t.channel);
						}
						tmp = xmlNodeGetContent(sonNode);  //取出节点中的内容
						strcpy((char*) gxml_realtime_req_t.meterAddr,
								(char*) tmp);
						debug_info(gDebugModule[XML_MODULE],
								"the type =%x,the protocolVer=%d,the channel=%d,the meterAddr =  %s\n",
								gxml_realtime_req_t.type,
								gxml_realtime_req_t.protocolVer,
								gxml_realtime_req_t.channel,
								gxml_realtime_req_t.meterAddr);
					}
					sonNode = sonNode->next;
				}
			}
			/*end add by yangmin - 20130829*/

		}
		curNode = curNode->next;
	}
	if (!xmlStrcmp(gxml_ComInfo_R.type, BAD_CAST "sequence"))
		return SEQUENCE;
	else if (!xmlStrcmp(gxml_ComInfo_R.type, BAD_CAST "result"))
		return RESULT;
	else if (!xmlStrcmp(gxml_ComInfo_R.type, BAD_CAST "time"))
		return TIME;
	else if (!xmlStrcmp(gxml_ComInfo_R.type, BAD_CAST "query"))
		return QUERY;
	else if (!xmlStrcmp(gxml_ComInfo_R.type, BAD_CAST "continuous_ack"))
		return CONTINUOUS_ACK;
	else if (!xmlStrcmp(gxml_ComInfo_R.type, BAD_CAST "period"))
		return PERIOD;
	else if (!xmlStrcmp(gxml_ComInfo_R.type, BAD_CAST "ip_port"))
		return IPPORT;
	else if (!xmlStrcmp(gxml_ComInfo_R.type, BAD_CAST "basic_db"))
		return BASIC_DB;
	else if (!xmlStrcmp(gxml_ComInfo_R.type, BAD_CAST "file_data"))
		return FILE_TRANS;
	else if (!xmlStrcmp(gxml_ComInfo_R.type, BAD_CAST "Trans_req"))
		return TRANS_REQ;
	else if (!xmlStrcmp(gxml_ComInfo_R.type, BAD_CAST "pack_lost"))
		return PACK_LOST;
	else if (!xmlStrcmp(gxml_ComInfo_R.type, BAD_CAST "query_config"))
		return QUERY_CONFIG;
	else if (!xmlStrcmp(gxml_ComInfo_R.type, BAD_CAST "id_config"))
		return ID_CONFIG;
	else if (!xmlStrcmp(gxml_ComInfo_R.type, BAD_CAST "report_mode"))
		return REPORT_MODE;
	else if (!xmlStrcmp(gxml_ComInfo_R.type, BAD_CAST "md5_key"))
		return MD5_KEY;
	else if (!xmlStrcmp(gxml_ComInfo_R.type, BAD_CAST "aes_key"))
		return AES_KEY;
	else if (!xmlStrcmp(gxml_ComInfo_R.type, BAD_CAST "reboot"))
		return REBOOT;
	else if (!xmlStrcmp(gxml_ComInfo_R.type, BAD_CAST "modify_req"))
		return MODIFY;
	else if (!xmlStrcmp(gxml_ComInfo_R.type, BAD_CAST "realtime_qry_req"))
		return REALTIME_QRY;
	else if (!xmlStrcmp(gxml_ComInfo_R.type, BAD_CAST "Query_req"))
		return Query_req;
	else
		return NONE;
}

/*
 *功能：生成xml文件
 *参数：xmltype--文件类型
 *      optype--操作类型
 *
 *
 *
 */
int makexml(char optype, char*filename) {
	int nRel;
	char *str;
	uint8 string[300] = "";
	uint8 MD5_Result_String[300] = "";
	//定义文档和指针
	xmlDocPtr doc = xmlNewDoc(BAD_CAST "1.0");
	//新建节点
	xmlNodePtr root_node = xmlNewNode(NULL, BAD_CAST "root");
	//设置新节点为根节点
	xmlDocSetRootElement(doc, root_node);
	//创建一个新节点common,添加到根节点
	xmlNodePtr node = xmlNewNode(NULL, BAD_CAST "common");

	xmlNodePtr childnode = NULL;
	xmlAddChild(root_node, node);

	strcpy((char *) gxml_ComInfo_T.building_id, gSystem_Config.Str_Building_id);
	strcpy((char *) gxml_ComInfo_T.gateway_id, gSystem_Config.Str_Gateway_id);

	//在common节点直接创建文本节点
	xmlNewTextChild(node, NULL, BAD_CAST "building_id",
			gxml_ComInfo_T.building_id);
	xmlNewTextChild(node, NULL, BAD_CAST "gateway_id",
			gxml_ComInfo_T.gateway_id);

	debug_info(gDebugModule[XML_MODULE], "[%s][%s][%d] optype is %d\n",
			FILE_LINE, optype);

	switch (optype) {
	case REQUEST:
		//str = "request";
		printf("send REQUEST xml\n");
		strcpy((char*) gxml_ComInfo_T.type, "request");
		xmlNewTextChild(node, NULL, BAD_CAST "type", gxml_ComInfo_T.type);
		node = xmlNewNode(NULL, BAD_CAST "id_validate");
		xmlAddChild(root_node, node);
		xmlNewProp(node, BAD_CAST "operation", BAD_CAST "request");
		nRel = xmlSaveFileEnc(filename, doc, "utf-8");
		if (nRel != -1) {
			xmlFreeDoc(doc);
			return 0;
		}
		break;
	case MD5_XML:
		//str = "md5";
		strcpy((char*) gxml_ComInfo_T.type, "md5");
		xmlNewTextChild(node, NULL, BAD_CAST "type", gxml_ComInfo_T.type);

		node = xmlNewNode(NULL, BAD_CAST "id_validate");
		xmlAddChild(root_node, node);
		xmlNewProp(node, BAD_CAST "operation", BAD_CAST "md5");
		/*begin:yangfei added 2013-06-06 for creat MD5 value*/
		//strcpy((char*)string,(char*)gSystem_Config.Str_Md5_key);
		//strcat((char*)string,(char*)gxml_IDInfo_R.sequence);
		strcpy((char*) string, (char*) gxml_IDInfo_R.sequence);
		strcat((char*) string, (char*) gSystem_Config.Str_Md5_key);

		debug_info(gDebugModule[XML_MODULE], "gxml_IDInfo_R.sequence %s, \n",
				gxml_IDInfo_R.sequence)
		;
		MD5(string, strlen((char*) string), MD5_Result_String);

		/*end:yangfei added 2013-06-06 */
#if 0
		PUBLIC_HexToString(MD5_Result_String,strlen((char*)MD5_Result_String),(char*)gxml_IDInfo_T.md5, 0);
#else
		PUBLIC_HexToString(MD5_Result_String, 16, (char*) gxml_IDInfo_T.md5, 0);
#endif
		printf("MD5 result is ************\n");
		debug_info(gDebugModule[XML_MODULE],
				"the md5 string is %s,    The string len is %d\n",
				gxml_IDInfo_T.md5, strlen((char * )gxml_IDInfo_T.md5))
		;
		xmlNewTextChild(node, NULL, BAD_CAST "md5", gxml_IDInfo_T.md5);
		nRel = xmlSaveFileEnc(filename, doc, "utf-8");
		if (nRel != -1) {
			xmlFreeDoc(doc);
			return 0;
		}
		break;
	case NOTIFY:
		//str = "notify";
		strcpy((char*) gxml_ComInfo_T.type, "notify");
		xmlNewTextChild(node, NULL, BAD_CAST "type", gxml_ComInfo_T.type);
		/*yangfei added for test*/
#if 0
		//memset(gTestbuf,'a',sizeof(gTestbuf));
		strcpy(gTestbuf, "live");
		gTestbuf[sizeof(gTestbuf)-1]='\0';
		xmlNewTextChild(node,NULL,BAD_CAST"yangfei_test",gTestbuf);
		debug_info(gDebugModule[XML_MODULE], "[%s][%s][%d] yangfei test OK\n",FILE_LINE);
#endif
		/*yangfei added for test*/
		node = xmlNewNode(NULL, BAD_CAST "heart_beat");
		xmlAddChild(root_node, node);
		xmlNewProp(node, BAD_CAST "operation", BAD_CAST "notify");
		nRel = xmlSaveFileEnc(filename, doc, "utf-8");
		if (nRel != -1) {
			xmlFreeDoc(doc);
			return 0;
		}
		break;
		// gjt add test 0612
	case REPORT:
	case CONTINUOUS:
		//gjt add test end
	case REPLY:

		if (optype == REPORT) {
			str = "report";
		} else if (optype == REPLY) {
			str = "reply";
		} else if (optype == CONTINUOUS) {
			str = "continuous";
		}

		strcpy((char*) gxml_ComInfo_T.type, str);
		xmlNewTextChild(node, NULL, BAD_CAST "type", gxml_ComInfo_T.type);

		node = xmlNewNode(NULL, BAD_CAST "data");
		xmlAddChild(root_node, node);
		xmlNewProp(node, BAD_CAST "operation", BAD_CAST str);

		xmlNewTextChild(node, NULL, BAD_CAST "sequence",
				gxml_DataInfo_T.sequence);
		xmlNewTextChild(node, NULL, BAD_CAST "parser", gxml_DataInfo_T.parser);
		xmlNewTextChild(node, NULL, BAD_CAST "time", gxml_DataInfo_T.time);
		xmlNewTextChild(node, NULL, BAD_CAST "total", gxml_DataInfo_T.total);
		xmlNewTextChild(node, NULL, BAD_CAST "current",
				gxml_DataInfo_T.current);

		int j = 0;
		while ('\0' != gxml_DataInfo_T.meter[j].id[0]) {
			xmlNodePtr son_node = xmlNewNode(NULL, BAD_CAST "meter");
			xmlAddChild(node, son_node);
			xmlNewProp(son_node, BAD_CAST "id", gxml_DataInfo_T.meter[j].id);

			int i = 0;
			while ('\0' != gxml_DataInfo_T.meter[j].function[i].fid[0]) {
				printf("function id is %s\n",
						gxml_DataInfo_T.meter[j].function[i].fid);
				xmlNodePtr grandson_node_water = xmlNewNode(NULL,
						BAD_CAST "function");
				xmlAddChild(son_node, grandson_node_water);
				xmlNewProp(grandson_node_water, BAD_CAST "id",
						gxml_DataInfo_T.meter[j].function[i].fid);  //水
				xmlNewProp(grandson_node_water, BAD_CAST "coding",
						gxml_DataInfo_T.meter[j].function[i].coding);
				xmlNewProp(grandson_node_water, BAD_CAST "error",
						gxml_DataInfo_T.meter[j].function[i].error);
				xmlNodeSetContent(grandson_node_water,
						gxml_DataInfo_T.meter[j].function[i].content);
				i++;
			}
			j++;
		}
#if 0        
		xmlNodePtr grandson_node_current = xmlNewNode(NULL,BAD_CAST"function");
		xmlAddChild(son_node,grandson_node_current);
		xmlNewProp(grandson_node_current,BAD_CAST"id",BAD_CAST"2");  //电
		xmlNewProp(grandson_node_current,BAD_CAST"coding",gxml_DataInfo_T.meter[0].function[1].coding);
		xmlNewProp(grandson_node_current,BAD_CAST"error",gxml_DataInfo_T.meter[0].function[1].error);
		xmlNodeSetContent(grandson_node_current,gxml_DataInfo_T.meter[0].function[1].content);

		xmlNodePtr grandson_node_gas = xmlNewNode(NULL,BAD_CAST"function");
		xmlAddChild(son_node,grandson_node_gas);
		xmlNewProp(grandson_node_gas,BAD_CAST"id",BAD_CAST"3");//气
		xmlNewProp(grandson_node_gas,BAD_CAST"coding",gxml_DataInfo_T.meter[0].function[2].coding);
		xmlNewProp(grandson_node_gas,BAD_CAST"error",gxml_DataInfo_T.meter[0].function[2].error);
		xmlNodeSetContent(grandson_node_gas,gxml_DataInfo_T.meter[0].function[2].content);
#endif        
		nRel = xmlSaveFileEnc(filename, doc, "utf-8");
		if (nRel != -1) {
			xmlFreeDoc(doc);
			return 0;
		}
		break;
#if 0
		case REPORT:
		debug_debug(gDebugModule[XML_MODULE], "Start Creat REPORT XML file \n");
		memset(gxml_ComInfo_T.type,0,sizeof(gxml_ComInfo_T.type));
		break;

		case CONTINUOUS:
		memset(gxml_ComInfo_T.type,0,sizeof(gxml_ComInfo_T.type));
		break;
#endif
	case PERIOD:/*yangfei added */
	case PERIOD_ACK:
		str = "period_ack";
		strcpy((char*) gxml_ComInfo_T.type, str);
		xmlNewTextChild(node, NULL, BAD_CAST "type", gxml_ComInfo_T.type);

		node = xmlNewNode(NULL, BAD_CAST "config");
		xmlAddChild(root_node, node);
		xmlNewProp(node, BAD_CAST "operation", BAD_CAST "period_ack");
		// nRel=xmlSaveFile("period_ack.xml",doc);
		nRel = xmlSaveFileEnc(filename, doc, "utf-8");
		printf("[%s][%s][%d] filename = %s\n", FILE_LINE, filename);
		if (nRel != -1) {
			xmlFreeDoc(doc);
			return 0;
		}
		printf("[%s][%s][%d] creat period_ack.xml ok\n", FILE_LINE);
		break;
	case IPPORT:/*yangfei added */
	case IP_PORT_ACK:
		str = "ip_port_ack";
		strcpy((char*) gxml_ComInfo_T.type, str);
		xmlNewTextChild(node, NULL, BAD_CAST "type", gxml_ComInfo_T.type);

		node = xmlNewNode(NULL, BAD_CAST "config");
		xmlAddChild(root_node, node);
		xmlNewProp(node, BAD_CAST "operation", BAD_CAST "ip_port_ack");
		//nRel=xmlSaveFile("ip_port_ack.xml",doc);
		nRel = xmlSaveFileEnc(filename, doc, "utf-8");
		if (nRel != -1) {
			xmlFreeDoc(doc);
			return 0;
		}
		break;
	case BASIC_DB:/*yangfei added */
	case BASIC_DB_REP:
		str = "basic_db_reply";
		strcpy((char*) gxml_ComInfo_T.type, str);
		xmlNewTextChild(node, NULL, BAD_CAST "type", gxml_ComInfo_T.type);

		node = xmlNewNode(NULL, BAD_CAST "data");
		xmlAddChild(root_node, node);
		xmlNewProp(node, BAD_CAST "operation", BAD_CAST "basic_db_reply");
		nRel = xmlSaveFileEnc(filename, doc, "utf-8");
		if (nRel != -1) {
			xmlFreeDoc(doc);
			return 0;
		}
		break;
	case PACK_LOST_ACK:
		str = "pack_lost_ack";
		strcpy((char*) gxml_ComInfo_T.type, str);
		xmlNewTextChild(node, NULL, BAD_CAST "type", gxml_ComInfo_T.type);

		node = xmlNewNode(NULL, BAD_CAST "filetransfer");
		xmlAddChild(root_node, node);
		xmlNewProp(node, BAD_CAST "operation", BAD_CAST "pack_lost_ack");

		xmlNewTextChild(node, NULL, BAD_CAST "pack_lost_total",
				gxml_TransInfo_T.pack_lost_total);
		xmlNewTextChild(node, NULL, BAD_CAST "pack_lost_index",
				gxml_TransInfo_T.pack_lost_index);
		nRel = xmlSaveFileEnc(filename, doc, "utf-8");
		if (nRel != -1) {
			xmlFreeDoc(doc);
			return 0;
		}
		break;
	case REPLY_CONFIG:
		str = "reply_config";
		strcpy((char*) gxml_ComInfo_T.type, str);
		xmlNewTextChild(node, NULL, BAD_CAST "type", gxml_ComInfo_T.type);

		node = xmlNewNode(NULL, BAD_CAST "query");
		xmlAddChild(root_node, node);
		xmlNewProp(node, BAD_CAST "operation", BAD_CAST "reply_config");

		xmlNewTextChild(node, NULL, BAD_CAST "period", gxml_QueryInfo_T.period);
		xmlNewTextChild(node, NULL, BAD_CAST "report_mode",
				gxml_QueryInfo_T.report_mode);
		xmlNewTextChild(node, NULL, BAD_CAST "soft_ver",
				gxml_QueryInfo_T.version);
		nRel = xmlSaveFileEnc(filename, doc, "utf-8");
		if (nRel != -1) {
			xmlFreeDoc(doc);
			return 0;
		}
		break;
	case ID_ACK:
		str = "id_ack";
		strcpy((char*) gxml_ComInfo_T.type, str);
		xmlNewTextChild(node, NULL, BAD_CAST "type", gxml_ComInfo_T.type);

		node = xmlNewNode(NULL, BAD_CAST "ex_config");
		xmlAddChild(root_node, node);
		xmlNewProp(node, BAD_CAST "operation", BAD_CAST "id_ack");
		nRel = xmlSaveFileEnc(filename, doc, "utf-8");
		if (nRel != -1) {
			xmlFreeDoc(doc);
			return 0;
		}
		break;
	case REPORT_ACK:
		str = "report_ack";
		strcpy((char*) gxml_ComInfo_T.type, str);
		xmlNewTextChild(node, NULL, BAD_CAST "type", gxml_ComInfo_T.type);

		node = xmlNewNode(NULL, BAD_CAST "ex_config");
		xmlAddChild(root_node, node);
		xmlNewProp(node, BAD_CAST "operation", BAD_CAST "report_ack");
		nRel = xmlSaveFileEnc(filename, doc, "utf-8");
		if (nRel != -1) {
			xmlFreeDoc(doc);
			return 0;
		}
		break;
	case KEY_ACK:
		str = "key_ack";
		strcpy((char*) gxml_ComInfo_T.type, str);
		xmlNewTextChild(node, NULL, BAD_CAST "type", gxml_ComInfo_T.type);

		node = xmlNewNode(NULL, BAD_CAST "update");
		xmlAddChild(root_node, node);
		xmlNewProp(node, BAD_CAST "operation", BAD_CAST "key_ack");
		nRel = xmlSaveFileEnc(filename, doc, "utf-8");
		if (nRel != -1) {
			xmlFreeDoc(doc);
			return 0;
		}
		break;
	case CONTROL_ACK:
		str = "control_ack";
		strcpy((char*) gxml_ComInfo_T.type, str);
		xmlNewTextChild(node, NULL, BAD_CAST "type", gxml_ComInfo_T.type);

		node = xmlNewNode(NULL, BAD_CAST "control");
		xmlAddChild(root_node, node);
		xmlNewProp(node, BAD_CAST "operation", BAD_CAST "control_ack");
		nRel = xmlSaveFileEnc(filename, doc, "utf-8");
		if (nRel != -1) {
			xmlFreeDoc(doc);
			return 0;
		}
		break;
		/*begin add by yangmin - 20130829*/
	case MODIFY_ACK:
		str = "modify_rsp";
		strcpy((char*) gxml_ComInfo_T.type, str);
		xmlNewTextChild(node, NULL, BAD_CAST "type", gxml_ComInfo_T.type);

		node = xmlNewNode(NULL, BAD_CAST "modify");
		xmlAddChild(root_node, node);
		xmlNewProp(node, BAD_CAST "operation", BAD_CAST "modify_rsp");
		xmlNewTextChild(node, NULL, BAD_CAST "flag",
				BAD_CAST gxml_modify_rsp_t.flag);
		nRel = xmlSaveFileEnc(filename, doc, "utf-8");
		if (nRel != -1) {
			xmlFreeDoc(doc);
			return 0;
		}
		break;
	case REALTIME_ACK:
		str = "realtime_qry_rsp";
		strcpy((char*) gxml_ComInfo_T.type, str);
		xmlNewTextChild(node, NULL, BAD_CAST "type", gxml_ComInfo_T.type);

		node = xmlNewNode(NULL, BAD_CAST "realtime_qry");
		xmlAddChild(root_node, node);
		xmlNewProp(node, BAD_CAST "operation", BAD_CAST "realtime_qry_rsp");
		xmlNewTextChild(node, NULL, BAD_CAST "result",
				BAD_CAST gxml_realtime_rsp_t.result);
		xmlNewProp(node->children, BAD_CAST "error",
				BAD_CAST gxml_realtime_rsp_t.error);
		nRel = xmlSaveFileEnc(filename, doc, "utf-8");
		if (nRel != -1) {
			xmlFreeDoc(doc);
			return 0;
		}
		break;
	case QUERY_REQ_ACK:
		str = "Query_rsp";
		strcpy((char*) gxml_ComInfo_T.type, str);
		xmlNewTextChild(node, NULL, BAD_CAST "type", gxml_ComInfo_T.type);

		node = xmlNewNode(NULL, BAD_CAST "report_srv");
		xmlAddChild(root_node, node);
		xmlNewProp(node, BAD_CAST "operation", BAD_CAST "Query_rsp");
		xmlNewTextChild(node, NULL, BAD_CAST "flag",
				BAD_CAST gxml_reportsrv_rsp_t.flag);

		childnode = xmlNewNode(NULL, BAD_CAST "result");
		xmlAddChild(node, childnode);
		xmlNewProp(childnode, BAD_CAST "count",
				BAD_CAST gxml_reportsrv_rsp_t.count);
		{
			int count = atoi(gxml_reportsrv_rsp_t.count);
			int k;
			for (k = 0; k < count; k++) {
				xmlNewTextChild(node, NULL, BAD_CAST "table_item",
						BAD_CAST gxml_reportsrv_rsp_t.result + (k * 512));
			}
		}
		nRel = xmlSaveFileEnc(filename, doc, "utf-8");
		if (nRel != -1) {
			xmlFreeDoc(doc);
			free(gxml_reportsrv_rsp_t.result);
			return 0;
		}
		free(gxml_reportsrv_rsp_t.result);
		break;
	case TRANS_ACK: {
		str = "Trans_ack";
		strcpy((char*) gxml_ComInfo_T.type, str);
		xmlNewTextChild(node, NULL, BAD_CAST "type", gxml_ComInfo_T.type);

		node = xmlNewNode(NULL, BAD_CAST "filetransfer");
		xmlAddChild(root_node, node);
		xmlNewProp(node, BAD_CAST "operation", BAD_CAST "Trans_ack");
		xmlNewTextChild(node, NULL, BAD_CAST "Trans_ack_flag",
				BAD_CAST "success");
		nRel = xmlSaveFileEnc(filename, doc, "utf-8");
		if (nRel != -1) {
			xmlFreeDoc(doc);
			return 0;
		}
		break;
	}
		/*end add by yangmin - 20130829*/
	default:
		printf("[%s][%s][%d] default type\n", FILE_LINE);
		return 2;
		break;
	}
	return 2;
}

int XML_main_proc(void *argv) {
	int i;
	xmlDocPtr doc;
	xmlNodePtr curNode;
	//char *docname;
	//int ret;
	while (1) {
		sem_wait(&gXML_Rcv_File.sem_read);
		printf("[%s][%s][%d] sem_wait gXML_Rcv_File.sem_read ok\n", FILE_LINE);
		xmlKeepBlanksDefault(0);      //将空格忽略
		/*begin:yangfei modified 20130705 for XML incomplete xmlParseFile will return NULL */
		//doc = xmlReadFile(gXML_Rcv_File.XMLFile[gXML_Rcv_File.readpos],"UTF-8",XML_PARSE_RECOVER);
		doc = xmlParseFile(gXML_Rcv_File.XMLFile[gXML_Rcv_File.readpos]);
		/*end: */
		gXML_Rcv_File.readpos = (gXML_Rcv_File.readpos + 1) % XML_BUF_FILE_NUM;
		printf("[%s][%s][%d]gXML_Rcv_File.readpos = %d \n", FILE_LINE,
				gXML_Rcv_File.readpos);
		if (NULL == doc) {
			fprintf(stderr, "Error parsing file\n");
			//gXML_Rcv_File.readpos=(gXML_Rcv_File.readpos+1)%XML_BUF_FILE_NUM ;
			//debug_err(gDebugModule[XML_MODULE],"[%s][%s][%d]gXML_Rcv_File.readpos = %d \n",FILE_LINE,gXML_Rcv_File.readpos);
			sem_post(&gXML_Rcv_File.sem_write);
			continue;
		}

		//确定根节点	
		curNode = xmlDocGetRootElement(doc);

		if (NULL == curNode) {
			fprintf(stderr, "empty document\n");
			xmlFreeDoc(doc);
			//gXML_Rcv_File.readpos=(gXML_Rcv_File.readpos+1)%XML_BUF_FILE_NUM ;
			//debug_err(gDebugModule[XML_MODULE],"[%s][%s][%d]gXML_Rcv_File.readpos = %d \n",FILE_LINE,gXML_Rcv_File.readpos);
			sem_post(&gXML_Rcv_File.sem_write);
			continue;
		}
		if (xmlStrcmp(curNode->name, BAD_CAST "root")) {
			fprintf(stderr, "document of wrong type, root node!= root\n");
			xmlFreeDoc(doc);
			//gXML_Rcv_File.readpos=(gXML_Rcv_File.readpos+1)%XML_BUF_FILE_NUM ;
			//debug_err(gDebugModule[XML_MODULE],"[%s][%s][%d]gXML_Rcv_File.readpos = %d \n",FILE_LINE,gXML_Rcv_File.readpos);
			sem_post(&gXML_Rcv_File.sem_write);
			continue;
		}
		printf("%s\n", curNode->name);

		i = parserxml(curNode);   //解析xml文件

		CMD_ReviceXMLData(i);

		debug_info(gDebugModule[XML_MODULE],
				"[%s][%s][%d]gXML_Rcv_File.readpos = %d \n", FILE_LINE,
				gXML_Rcv_File.readpos);
		sem_post(&gXML_Rcv_File.sem_write);

#if 0		
		printf("type is %d\n",i);
		xmlCleanupParser();
		gXML_Rcv_File.readpos=(gXML_Rcv_File.readpos+1)%XML_BUF_FILE_NUM;

		ret=makexml(i,gXML_Send_File.XMLFile[gXML_Send_File.writepos]);
		if(ret == 0)
		{
			if((gXML_Send_File.writepos + 1)%XML_BUF_FILE_NUM == gXML_Send_File.readpos)
			{
				debug_err(gDebugModule[XML_MODULE], "[%s][%s][%d] XML_BUF_FILE is full\n",FILE_LINE);
			}
			sem_wait(&gXML_Send_File.sem_write);
			gXML_Send_File.writepos=(gXML_Send_File.writepos + 1)%XML_BUF_FILE_NUM;

			sem_post(&UpSend_Sem);

			debug_info(gDebugModule[XML_MODULE],"[%s][%s][%d]gXML_Send_File.writepos = %d \n",FILE_LINE,gXML_Send_File.writepos);
			sem_post(&gXML_Send_File.sem_read);
			//sem_post(&XMLSendFile_Sem);
		}
		else
		{
			debug_err(gDebugModule[XML_MODULE], "[%s][%s][%d] makexml err\n",FILE_LINE);
		}
#endif

	}
	//return(0);
}
