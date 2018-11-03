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

#define AC_STRUCTURE_PACK

typedef         char           int8;
typedef         short          int16;
typedef         int            int32;
typedef         long long      int64;

typedef         unsigned char           uint8;
typedef         unsigned short          uint16;
typedef         unsigned int            uint32;
typedef         unsigned long long      uint64;

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
	RESET_L1,
	FAPI_STATE_MAX
};

/* General Message Format, [1]3.1 Table 3*/
typedef struct AC_STRUCTURE_PACK {
    uint8   msgTypeId;
    /*BOOL, 0: no vendor-specific bytes, 1: have vendor-specific bytes, 
    the length is the first word of the vendor-specific*/
    uint8   msgVdrSpecFlag; 
    /*In bytes, Length of Message body*/
    uint16  msgBodyLength;
    /*  Message Body */
    /* Vendor-specific message body */
}ltePhyL2ApiMsgHdr_t;

/*------------CONFIG.request-------------*/
/* [1]3.2.2 Config */

/* value parameter is 16 bits */
typedef struct AC_STRUCTURE_PACK
{
    uint8   tag;
    uint8   length; // in bytes, length of value
    uint16  value;
}apiCfgTlv16_t;

/* General Configuration TLVs , 1 - 5 */
typedef struct AC_STRUCTURE_PACK
{
    apiCfgTlv16_t duplexMode;
    apiCfgTlv16_t pcifchPowerOffs;
    apiCfgTlv16_t PB;
    apiCfgTlv16_t dlCpType;
    apiCfgTlv16_t ulCpType;
}apiCfgTlvGeneral_t;

/* TDD for IDLE and CONFIGURED state , message body, Table 12 */
typedef struct AC_STRUCTURE_PACK {
    apiCfgTlvGeneral_t  general;
    // apiCfgTlvRF_t       rfConfig;
    // apiCfgTlvPHICH_t    phichCfg;
    // apiCfgTlvSCH_t      schConfig;
    // apiCfgTlvPRACH_t    prachCfg;
    // apiCfgTlvPUSCH_t    puschCfg;
    // apiCfgTlvPUCCH_t    pucchCfg;
    // apiCfgTlvSRS_t      srsConfig;
    // apiCfgTlvULRS_t     ulRsCfg;
    // apiCfgTlvTdd_t      tddCfg; // tdd specific
    // apiCfgTlv16_t       dataReportMode;
    // apiCfgTlvPhyCtrl_t  phyModeCtrl;
} apiCfgReqTDD_IDLE_CFGED_t;
/* FDD for IDLE and CONFIGURED state , message body, Table 13 */
typedef struct AC_STRUCTURE_PACK {
    apiCfgTlvGeneral_t  general;
    // apiCfgTlvRF_t       rfConfig;
    // apiCfgTlvPHICH_t    phichCfg;
    // apiCfgTlvSCH_t      schConfig;
    // apiCfgTlvPRACH_t    prachCfg;
    // apiCfgTlvPUSCH_t    puschCfg;
    // apiCfgTlvPUCCH_t    pucchCfg;
    // apiCfgTlvSRS_t      srsConfig;
    // apiCfgTlvULRS_t     ulRsCfg;
    // apiCfgTlv16_t       dataReportMode;
    // apiCfgTlvPhyCtrl_t  phyModeCtrl;
} apiCfgReqFDD_IDLE_CFGED_t;
/* TLVs in RUNNING state , message body, Table 14*/
typedef struct AC_STRUCTURE_PACK {
    apiCfgTlv16_t  SFN_SF;
    //////////////////////// TBD , FFS TLVs ????
} apiCfgReq_RUNNING_t;

/* CONFIG.request , Table 11 */
typedef struct AC_STRUCTURE_PACK {
    uint8   numTLVs;
    union {
        apiCfgReqFDD_IDLE_CFGED_t   infoFdd_IdleCfg;
        // apiCfgReqTDD_IDLE_CFGED_t   infoTdd_IdleCfg;
        // apiCfgReq_RUNNING_t         info_Running;
    }TLVs;
} ltePhyL2ApiConfigReq_t;

/*--------------DL_CONFIG.request--------------*/
/* DCI DL PDU, used by PHY to create DCI formats , Table 39*/
typedef struct AC_STRUCTURE_PACK
{
    uint8   dciFormat; // apiDciFormatDL_eum
    uint8   cceIndx;
    uint8   aggreLevel;// aggregation level
    uint16  rnti;
    uint8   resAllocType; // apiResAllocTypeDL_eum, type 0 or 1 here
    uint8   virtRbFlag;// apiVrbFlagDL_eum
    uint32  rbCoding;
    uint8   mcs_1;
    uint8   rv_1;
    uint8   newDataInd_1;
    uint8   tb2CwSwapFlag;
    uint8   mcs_2;
    uint8   rv_2;
    uint8   newDataInd_2;
    uint8   harqProcess;
    uint8   TPMI;
    uint8   PMI;
    uint8   precodeInfo;
    uint8   TPC;
    uint8   dlAssignIdx;
    uint8   N_GAP;
    uint8   tbsIdx;
    uint8   dlPowerOffs;
    uint8   prachFlag; // indicate that PRACH procedure is initiated
    uint8   preambleIdx;
    uint8   prachMskIdx;
    uint8   rntiType;
    uint16  transPower;
}apiDlCfgPduDCI_t;

/* BCH PDU Configuration, Table 40*/
typedef struct AC_STRUCTURE_PACK
{
    uint16  length;
    uint16  pduIdx;
    uint16  transPower;
}apiDlCfgPduBCH_t;

/* MCH PDU Configuration, Table 41*/
typedef struct AC_STRUCTURE_PACK
{
    uint16  length;
    uint16  pduIdx;
    uint16  rnti;
    uint8   resAllocType; // apiResAllocTypeDL_eum, type 0,1,2
    uint32  rbCoding;
    uint8   modulation; // apiModulationDL_enum
    uint16  transPower;
}apiDlCfgPduMCH_t;

/* PDU Config structure in DL_CONFIG */
typedef struct AC_STRUCTURE_PACK
{
    uint8   pduType; // apiDlCfgPduType_eum
    uint8   pduSize;
    union {
        apiDlCfgPduDCI_t    dciPDU; // DCI DL PDU
        apiDlCfgPduBCH_t    bchPDU; // BCH PDU
        apiDlCfgPduMCH_t    mchPDU; // MCH PDU
        // apiDlCfgPduDlSCH_t  dlschPDU; // DLSCH PDU
        // apiDlCfgPduPCH_t    pchPDU; //PCH PDU
        // apiDlCfgPduCSIRS_t  csirsPDU; //CSI RS PDU
        // apiDlCfgPduDCIR10_t dciPDUR10; // DCI DL PDU R10,extended by AC to support r10
        // apiDlCfgPduDlSCHR9_t dlschPDUR9; // DLSCH PDU R9, extended by AC to  support R9
        // apiDlCfgPduDlSCHR10_t dlschPDUR10; // DLSCH PDU R10, extended by AC to  support R10
    }pduConfig;
}apiDlCfgPdu_t;

/*********** DL_CONFIG.request , message body , 3.3.1.2, Tabale 38 */
typedef struct AC_STRUCTURE_PACK {
    uint16  SFN_SF;
    uint16  length;
    uint8   numPdcchSyms;
    uint8   numDCIs; // number of DCI PDUs
    uint16  numPDUs; // Number Of PDUs
    uint8   numPdschRntis; // number of unique RNTIs sent on the PDSCH
    uint16  pcfichPower;
    /* Number Of PDUs */
    /* NOTE: The size of each PDU is based on the PDU type,
    all the PDUs in the message are packed without any padding,
    We can not use array index to get next PDU directly,
    here Define a PDU array, just for information*/
    // 2 TB PDSCH + DCI DL + BCH
    // apiDlCfgPdu_t  PDU[AC_MAX_PDSCH_PER_SUBFRAME * 2 + AC_MAX_PDCCH_PER_SUBFRAME + 1];
    apiDlCfgPdu_t  PDU;// 2 TB PDSCH + DCI DL + BCH
} ltePhyL2ApiDlConfigReq_t;

/*Modify from 4G for suitable in 5G*/
typedef struct AC_STRUCTURE_PACK {
	uint32 vendor_specific_len;
    uint8 slot_num; 
    uint8 dl_numberlogy; 
    uint8 ul_numberlory;
    uint16 dl_bandwidth_parth;
    uint16 ul_bandwidth_parth;
}ltePhyL2ApiDlConfigVndrSpecMsg_t;
