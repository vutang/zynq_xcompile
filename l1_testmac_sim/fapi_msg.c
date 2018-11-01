/*
* @Author: vutang
* @Date:   2018-10-23 10:49:26
* @Last Modified by:   vutang
* @Last Modified time: 2018-11-01 17:54:09
*/

#include <string.h>
#include <sys/time.h>
#include "fapi_msg.h"
#include "logger.h"

char msg_type_1_name[][50]  = {
	"PARAM_REQ",
	"PARAM_RSP",
	"CONFIG_REQ",
	"CONFIG_RSP",
	"START_REQ",
	"STOP_REQ",
	"STOP_IND",
	"UE_CFG_REQ",
	"UE_CFG_RSP",
	"ERROR_IND",
	"UE_RELEASE_REQ",
	"UE_RELEASE_RSP"
};

char msg_type_2_name[][50] = {
	"API_MSG_TYPE_DLCFG_REQ",
	"API_MSG_TYPE_ULCFG_REQ",
	"API_MSG_TYPE_SUBFRAME_IND",
	"API_MSG_TYPE_HI_DCI0_REQ ",
	"API_MSG_TYPE_TX_REQ",
	"API_MSG_TYPE_HARQ_IND",
	"API_MSG_TYPE_CRC_IND",
	"API_MSG_TYPE_RX_ULSCH_IND",
	"API_MSG_TYPE_RACH_IND",
	"API_MSG_TYPE_SRS_IND",
	"API_MSG_TYPE_RX_SR_IND",
	"API_MSG_TYPE_RX_CQI_IND"
};

char fapi_msg_state_name[][50] = {
	"IDLE",
	"CONFIGURED",
	"RUNNING",
	"WAIT_INDICATION"	
};

/*Get FAPI State*/
char *get_fapi_msg_state_name(int state) {
	if (state > FAPI_STATE_MAX)
		return NULL;
	return fapi_msg_state_name[state];
}

int get_msg_type_name(uint8_t msg_id, char *name) {
	if ((msg_id < 0x80) && (msg_id >= 0)) {
		strcpy(name, msg_type_1_name[msg_id]);
		return 0;
	}
	else if((msg_id >= 0x80) && (msg_id <= 0x8b)) {
		strcpy(name, msg_type_2_name[msg_id & 0x0F]);
		return 0;
	}
	else {
		strcpy(name, "UNDEFINED");
	} 

	return -1;
}

void gettimetostring(char *timestr){
	struct timeval  tv;
	struct tm      *tm;
	time_t ltime;

	gettimeofday(&tv, NULL);
	ltime = time(NULL);
	tm = localtime(&ltime);
	sprintf(timestr, "%02d:%02d:%02d",tm->tm_hour,tm->tm_min,tm->tm_sec);
}

int prep_fpga_config_req(char *buf) {
	ltePhyL2ApiMsgHdr_t *fapi_hdr = (ltePhyL2ApiMsgHdr_t *) buf;
	ltePhyL2ApiConfigReq_t *fapi_msg_body = \
			(ltePhyL2ApiConfigReq_t *) buf + sizeof(ltePhyL2ApiMsgHdr_t);
	/*CONFIG.request has no vendor data*/
	fapi_hdr->msgVdrSpecFlag = 0;

	fapi_msg_body->numTLVs = 0x55; /*For test*/
	return (sizeof(ltePhyL2ApiMsgHdr_t) + sizeof(ltePhyL2ApiConfigReq_t));
}

int prep_fpga_dlconfig_req(char *buf) {
	ltePhyL2ApiMsgHdr_t *fapi_hdr = (ltePhyL2ApiMsgHdr_t *) buf;
	ltePhyL2ApiDlConfigReq_t *fapi_msg_body = \
		(ltePhyL2ApiDlConfigReq_t *) buf + sizeof(ltePhyL2ApiMsgHdr_t);
	ltePhyL2ApiDlConfigVndrSpecMsg_t *fapi_msg_vendor = \
		(ltePhyL2ApiDlConfigVndrSpecMsg_t *) buf + sizeof(ltePhyL2ApiMsgHdr_t) + \
		+ sizeof(ltePhyL2ApiDlConfigReq_t);
	/*Header*/
	fapi_hdr->msgBodyLength = sizeof(ltePhyL2ApiMsgHdr_t) + sizeof(ltePhyL2ApiDlConfigReq_t) + \
		sizeof(ltePhyL2ApiDlConfigVndrSpecMsg_t);
	fapi_hdr->msgVdrSpecFlag = 1;
	/*Body*/
		/*Default: Fix me later*/
	/*Vendor*/
	fapi_msg_vendor->vendor_specific_len = sizeof(ltePhyL2ApiDlConfigVndrSpecMsg_t);


	return (sizeof(ltePhyL2ApiMsgHdr_t) + sizeof(ltePhyL2ApiDlConfigReq_t) + \
		sizeof(ltePhyL2ApiDlConfigVndrSpecMsg_t));	
}

int prep_fapi_msg(char *buf, uint8_t msg_id) {
	char msg_type_name[50], time[50];
	int fapi_msg_sz = -1;
	// struct fapi_header *fapi_hdr = (struct fapi_header *) buf;
	ltePhyL2ApiMsgHdr_t *fapi_hdr = (ltePhyL2ApiMsgHdr_t *) buf;

	// fapi_hdr->fapi_type = 0xAA;
	fapi_hdr->msgTypeId = msg_id;

	switch(msg_id) {
		/*PARAM.request: nobody msg, no vedor specific*/
		case API_MSG_TYPE_PARAM_REQ:
		/*START.request: nobody msg, no vendor specific*/
		case API_MSG_TYPE_START_REQ:
			fapi_hdr->msgVdrSpecFlag = 0;
			fapi_hdr->msgBodyLength = 0;
			fapi_msg_sz = sizeof(ltePhyL2ApiMsgHdr_t);
			break;
		case API_MSG_TYPE_CONFIG_REQ:
			fapi_msg_sz = prep_fpga_config_req(buf);
			break;
		case API_MSG_TYPE_DLCFG_REQ: 
			fapi_msg_sz = prep_fpga_dlconfig_req(buf);

	}

	// get_msg_type_name(msg_id, msg_type_name);
	// LOG_INFO("--->: %s", msg_type_name);
	// gettimetostring(time);

	// sprintf(msg_type_name, "%s-%s", msg_type_name, time);

	// fapi_hdr->msg_len = strlen(msg_type_name);
	// memcpy(&fapi_hdr->msg_payload[0], &msg_type_name[0], strlen(msg_type_name));

	return fapi_msg_sz;
}

/*FAPI MSG Handle*/
int handle_fapi_msg(char *buf) {
	struct fapi_header *fapi_hdr = (struct fapi_header *) buf;
	char msg_type_name[50];	

	get_msg_type_name(fapi_hdr->msg_id, msg_type_name);
	LOG_INFO("<---: %s", msg_type_name);
	LOG_INFO("    : %s", fapi_hdr->msg_payload);

	return fapi_hdr->msg_id;
}