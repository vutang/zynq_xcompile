#include <stdio.h>
#include <time.h>
#include <stdint.h>

/* Message Type ,
* [1]Table 4
*/
#define API_MSG_TYPE_PARAM_REQ          0x0
#define API_MSG_TYPE_PARAM_RSP          0x1
#define API_MSG_TYPE_CONFIG_REQ         0x2
#define API_MSG_TYPE_CONFIG_RSP         0x3
#define API_MSG_TYPE_START_REQ          0x4
#define API_MSG_TYPE_STOP_REQ           0x5
#define API_MSG_TYPE_STOP_IND           0x6
#define API_MSG_TYPE_UE_CFG_REQ         0x7
#define API_MSG_TYPE_UE_CFG_RSP         0x8
#define API_MSG_TYPE_ERROR_IND          0x9
#define API_MSG_TYPE_UE_RELEASE_REQ     0xa
#define API_MSG_TYPE_UE_RELEASE_RSP     0xb
// RESERVED  0x0a - 0x7f
#define API_MSG_TYPE_DLCFG_REQ          0x80
#define API_MSG_TYPE_ULCFG_REQ          0x81
#define API_MSG_TYPE_SUBFRAME_IND       0x82
#define API_MSG_TYPE_HI_DCI0_REQ        0x83
#define API_MSG_TYPE_TX_REQ             0x84
#define API_MSG_TYPE_HARQ_IND           0x85
#define API_MSG_TYPE_CRC_IND            0x86
#define API_MSG_TYPE_RX_ULSCH_IND       0x87
#define API_MSG_TYPE_RACH_IND           0x88
#define API_MSG_TYPE_SRS_IND            0x89
#define API_MSG_TYPE_RX_SR_IND          0x8a
#define API_MSG_TYPE_RX_CQI_IND         0x8b

/*Define fapi_header*/
struct fapi_header {
	// uint8_t fapi_type;
	uint8_t msg_id;
	uint8_t msg_vendor_len;
	uint16_t msg_len;
	char msg_payload[200];
};

/*Transaction State*/
enum fapi_msg_send_st {
	IDLE,
	CONFIGURED,
	RUNNING,
	WAIT_INDICATION,
	FAPI_STATE_MAX
};
