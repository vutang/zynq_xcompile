/*******************************************************************************************************
 *                       ArrayComm Confidential
 *                  Copyright (c) ArrayComm, LLC. 1995-2015
 *
 *      This software is furnished under license and may be used or
 *      copied only in accordance with the terms of such license.
 ********************************************************************************************************
 *
 * @file   ac_ltePhyL2Api.h
 * @brief  MAC-PHY interface header file, based on freescale platform.
 *
 * Reference document:
 *      [1] LTE eNB L1 API Definition v1.1, Femto Forum
 *      [2] LTE eNB L1 API definition v082.04.01, Femto Forum
 *      [3] LTE eNB L1 API definition v082.05.02, Femto Forum
 * 
 * Change Log:
 *      Date                 Who             What
 *      2011/09/08           Xiaofang Xie    Created, 1st version
 *      2012/06/30           Xiaofang Xie    Make it not FSL specific
 *      2012/11/20           Nanqing Zhou    Extend FAPI to add TLVs in config.request and param.response
 *      2013/01/16           Nanqing Zhou    Clean the header file for PHY static parameter set
 *      2014/05/07           Xin Chen        add E-TM mode definition
 *      2014/06/05           Xin Chen        extend the FAPI API to FSL B4860 platform
 *      2014/06/24           Yang Chen       extend the FAPI to release 10 for carrier aggregation
 *      2014/08/19           Yang Chen       Modified some PDU order of R10; Modified apiAperiodicPmiRiRpt_t,apiUciHarqInfoR10_t; Defined apiHarqMode_enum
 *      2015/01/05           Yang Chen       add apiHarqMode_frt1a1b for FDD in apiHarqMode_enum;add Butt at the end of Enum;set unknown pdu type to 255;add apiErrIndNumTLV_t
 *      2015/03/24           Yuling Xiang    add apiCfgTlvTag_eRF_txPhyAnt; reorder the added parameters before
 *      2015/04/07           Xin Chen         add ULSCH data address array in ltePhyL2ApiUlConfigReq_t, remove the apiUlCfgPduDataAddrTLV_t and apiUlCfgPduDataAddrValue_t
 *      2015/05/12           Yang Chen       extend fapi to support TM9,CSI-RS PDU
 *      2015/11/05           Yang Chen       add some vendor specific fields in the indication messages to report more uplink receiver metrics
 *      2015/12/17           Yang Chen       define Noise plus Interference for PUSCH/PUCCH/SRS indication in vendor specific fields
 *      2016/02/25           Yang Chen       define PUSCH IRC and MRC receiver algorithm switch TLV
 *      2016/05/09           Yang Chen       add mex pragma, add min/max NI in ulsch/srs vendor specific metrics, modify PUSCH receiver TLV, add a comment of HARQ-ACK mode in apiUlCfgHarqInfo_t, add HARQ-ACK information bits and resource in vendor specific
 *      2016/07/26           Yang Chen       define a TLV to support MAC config Beamforming granularity
 *      2016/08/17           Yang Chen       Add 'control type' in periodic CSI report in PUSCH, support 256QAM in PDSCH
 *      2016/10/5            Tiennt12        define ICIC receiver algorithm switch TLV. Structure for ICIC measurement
 
 ********************************************************************************************************
 */
#ifndef AC_LTEPHYL2API_H
#define AC_LTEPHYL2API_H

// #include "ac_ltePhyStaticSysCfg.h"

#if defined(TARGET_FSL_SC3850) || defined(TARGET_FSL_SC3900) || defined(TARGET_x86)
    #include "ac_types.h"
#else
    typedef         char           int8;
    typedef         short          int16;
    typedef         int            int32;
    typedef         long long      int64;

    typedef         unsigned char           uint8;
    typedef         unsigned short          uint16;
    typedef         unsigned int            uint32;
    typedef         unsigned long long      uint64;
#endif

#ifdef _WIN32
   #pragma pack(1)
   #define AC_STRUCTURE_PACK
#else
   #define AC_STRUCTURE_PACK  __attribute__((packed))
#endif


/* FAPI P5 Version Definition
 *
 * <customer name abbr>+'_'+<FAPI major version>+'.'+<FAPI minor version>+'.'+<AC extend major version>+'.'+<AC extend minor version>.
 * Examples would be:
 * STD stand for standard baseport FAPI interface
 * #define FAPI_P5_VER "STD_1.1.2.3" //standard product
 * #define FAPI_P5_VER "CTMA_1.1.2.2" //customer A)
 * #define FAPI_P5_VER "CTMB_1.1.2.4" //customer B)
 */
#define FAPI_P5_VER "STD_5.2.0.0"

#include "vt_ltePhyDefine.h"

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
#if(ON == VT_ICIC_MES)
#define API_MSG_TYPE_ICIC_MES         	0x96 /* Viettel define: L1 send measurement report for L2/L3 with ICIC function*/
#endif
// RESERVED  0x8c - 0xff
#define API_MSG_TYPE_CUSTOM_DEF_IND      0xff

/* L1 API error codes, [1]Table 81, section 3.4 */
#define API_ERRCODES_MSG_OK                     0
#define API_ERRCODES_MSG_INVALID_STATE          1
#define API_ERRCODES_MSG_INVALID_CFG            2	/* CONFIG.request is invalid, but we have CONFIG.rsp to indicate this?! */
#define API_ERRCODES_SFN_OUT_OF_SYNC            3	/* DL_CONFIG.req received with an unexpected SFN*/
#define API_ERRCODES_MSG_PDU_ERR                4	/* Error in UL_CONFIG.req or DL_CONFIG.req, sub error codes should be supplied */
#define API_ERRCODES_MSG_BCH_MISSING            5	/* A BCH PDU was expeced in DL_CONFIG.req, but it is not present */
#define API_ERRCODES_MSG_INVALID_SFN            6	/* The SFN in HIDCI0.req or TX.request was not expected. */
#define API_ERRCODES_MSG_HI_ERR                 7	/* Error in HIDCI0.req, sub error codes should be supplied */
#define API_ERRCODES_MSG_TX_ERR                 8	/* Error in TX.req, sub error codes should be supplied */
#define API_ERRCODES_MSG_DL_CONFIG_MISS         9	/* DL configuration is missing */
#define API_ERRCODES_MSG_UL_CONFIG_MISS         10	/* UL configuration is missing */
#define API_ERRCODES_MSG_NOT_SUPPORT            11	/* the FAPI message are not supported*/
#define API_ERRCODES_MSG_TX_REQ_MISS            12	/* tx.req configuration is missing*/

#if (ON == VT_DEBUG_RACH_CFG_ERR)
#define API_ERRCODES_MSG_RACH_CONFIG_ERR		13	/* Viettel define: Prach configuration is not correct*/
#endif
    
#if (ON == VT_DEBUG_ERROR_OUT_OF_SYNC)    
#define API_ERRCODES_MSG_SUBFRAME_IND_SEND_FAIL	14	/* Viettel define: L1 could not send SF.IND to MAC*/
#endif
    
#if (ON == VT_DEBUG_CPRI_ERR)    
#define API_ERRCODES_CPRI_RX_ERR				15	/* Viettel define: Error in CPRI RX path*/
#endif

#if (ON == VT_MAPLE_JOB_ERR)
#define API_ERRCODES_MAPLE_NULL_JOB_ERR  		16	/* Viettel define: Maple null job error */  
#endif
    
#if(ON == VT_DEBUG_ERROR_OUT_OF_SYNC)
#define API_ERRCODES_OUT_OF_SYNC				17	/* Viettel define: L1 & MAC out of sync */
#endif
    
#define AC_MAX_ERR_TYPE_NUM                     18 /*MAX error indication type number*/

#define API_TLV_STRING_LEN_MAX                  42/*Maximal string length in TLV*/
/******************************************** ENUMERATION DEFINES ********************************
*
***************************************************************************************************/

/* Configuration TLVs enumeration,
*[1] 3.2.3, Table 18
*/
typedef enum
{
    /*Configuration TLVs ,General Config [1]3.2.3  */
    apiCfgTlvTag_eDuplexMode = 1,
    apiCfgTlvTag_ePcfichPowerOffs,
    apiCfgTlvTag_ePB,
    apiCfgTlvTag_eDlCpType,
    apiCfgTlvTag_eUlCpType,
    apiCfgTlvTag_eGeneralNum = 5,
    /* Configuration TLVs, RF Config, 6 - 10 */
    apiCfgTlvTag_eRF_dlBW = 6,
    apiCfgTlvTag_eRF_ulBW,
    apiCfgTlvTag_eRF_rsPower,
    apiCfgTlvTag_eRF_txAntPorts,
    apiCfgTlvTag_eRF_rxAntPorts,
    apiCfgTlvTag_eRfNum = 5,
    /* Configuration TLVs, PHICH Config, 11 -  13 */
    apiCfgTlvTag_ePHICH_resource = 11, // Ng
    apiCfgTlvTag_ePHICH_duration,  // Normal, Extended
    apiCfgTlvTag_ePHICH_powerOffs,
    apiCfgTlvTag_ePhichNum = 3,
    /* Configuration TLVs, SCH Config , 14 - 16 */
    apiCfgTlvTag_eSCH_pssEPRE = 14,
    apiCfgTlvTag_eSCH_sssEPRE,
    apiCfgTlvTag_eSCH_cellId,
    apiCfgTlvTag_eSchNum = 3,
    /* Configuration TLVs, PRACH Config, 17 - 21 */
    apiCfgTlvTag_ePRACH_cfgIdx = 17,
    apiCfgTlvTag_ePRACH_rootSeqIdx,
    apiCfgTlvTag_ePRACH_Ncs,
    apiCfgTlvTag_ePRACH_HighSpdFlag,
    apiCfgTlvTag_ePRACH_freqOffs,
    apiCfgTlvTag_ePrachNum = 5,
    /* Configuration TLVs, PUSCH Config , 22 - 24 */
    apiCfgTlvTag_ePUSCH_hopMode = 22,
    apiCfgTlvTag_ePUSCH_hopOffs,
    apiCfgTlvTag_ePUSCH_subBandsNum,
    apiCfgTlvTag_ePuschNum = 3,
    /* Configuration TLVs, PUCCH Config, 25 - 28 */
    apiCfgTlvTag_ePUCCH_deltaShift = 25,
    apiCfgTlvTag_ePUCCH_NcqiRb,
    apiCfgTlvTag_ePUCCH_NanCs,
    apiCfgTlvTag_ePUCCH_N1PucchAn,
    apiCfgTlvTag_ePucchNum = 4,
    /* Configuration TLVs, SRS Config, 29 - 32 */
    apiCfgTlvTag_eSRS_bwCfg = 29,
    apiCfgTlvTag_eSRS_maxUpPts,
    apiCfgTlvTag_eSRS_srsSfrmCfg,
    apiCfgTlvTag_eSRS_srsAckSameSfrm,
    apiCfgTlvTag_eSrsNum = 4,
    /* Configuration TLVs, Uplink Reference Signal Config , 33 - 35 */
    apiCfgTlvTag_eULRS_hopMode = 33,
    apiCfgTlvTag_eULRS_deltaSeqShift,
    apiCfgTlvTag_eULRS_cs1DMRS,
    apiCfgTlvTag_eUlRsNum = 3,
    /* Configuration TLVs, TDD Frame Structure Config , 36 - 37*/
    apiCfgTlvTag_eTdd_frmStrucCfg = 36,
    apiCfgTlvTag_eTdd_speSfrmPattern,
    apiCfgTlvTag_eTddSpecNum = 2,
    /* 38 - 39 Reserved */
    /* Configuration TLVs, Used by l1 to report physical capabilities to L2/L3 Software , 40 - 44 */
    apiCfgTlvTag_eDlBWsuppInRB = 40,
    apiCfgTlvTag_eUlBWsuppInRB,
    apiCfgTlvTag_eDlModSupport,
    apiCfgTlvTag_eUlModSupport,
    apiCfgTlvTag_ePhyAntCapabi,
    apiCfgTlvTag_ePhyCapabNum = 5,
    /* 45 - 49 Reserved */
    /* Configuration TLVs, Used by l2/l3 softawre to configure the interaction between l2/l3 and l1, 50 - 51 */
    apiCfgTlvTag_eDataReportMode = 50,
    apiCfgTlvTag_eSFN_SF,
    /* 52- 59 Reserved */
    /* Configuration TLVs, Used by L1 to report its current status, 60 */
    apiCfgTlvTag_ePhyState = 60,
    apiCfgTlvTag_eNumTddCfgTlvs = 44, // exclude SFN_SF only
    apiCfgTlvTag_eNumFddCfgTlvs= 42,// exclude SFN_SF , tdd specific tlvs

    /*PHY run-time control TLVs 61-70*/
    apiPhyCtrlTlvTag_eSfnSyncMasterMode = 61,
    apiPhyCtrlTlvTag_eRunningMode,
    apiPhyCtrlTlvTag_eTimerMode,
    apiPhyCtrlTlvTag_eSymTimerTick,

    apiPhyCtrlTlvTag_eTxAntMask,
    apiPhyCtrlTlvTag_eRxAntMask,

    apiPhyCtrlTlvTag_ePssSssTransAntPort,
    apiPhyCtrlTlvTag_ePssSssTransMode,

    apiPhyCtrlTlvTag_txPhyAnt,  // only support 1/2/4/8 
    
    apiPhyCtrlTlvTag_eTxVecLenth,
    apiPhyCtrlTlvTag_eRxVecLenth,

    apiPhyCtrlTlvTag_eAicUlTdDataEna,
    apiPhyCtrlTlvTag_eDlMsgTimeOffset,   //this value also affect config.req in running mode.
    apiPhyCtrlTlvTag_eETMMode, 
    apiPhyCtrlTlvTag_eNumTlvs = 14,
    
    /* PHY algorithm related TLVs*/
    apiPhyAlgTlvTag_ePUSCH_ACK_scaling = 75,
    apiPhyAlgTlvTag_ePUCCH_ACK_scaling,
    apiPhyAlgTlvTag_ePRACH_PREAMBLE_scaling,
    apiPhyAlgTlvTag_ePUSCH_Receiver_selection,
	apiPhyAlgTlvTag_ePDSCH_BF_Gran_config,    // For MAC config beamforming granularity, now the range is [0,1,2,3]
    apiPhyAlgTlvTag_eNumTlvs = 5,
    
    /*PHY version TLVs 200-204*/
    apiVersionTlvTag_ePhyVersion = 200,
    apiVersionTlvTag_eFapiVersion,
    apiVersionTlvTag_eBuildTime,
    apiVersionTlvTag_eSdosVersion,
    apiVersionTlvTag_eMapleVersion,
    apiVersionTlvTag_eNumTlvs = 5,
    
    /*PHY report related TLVs*/
    apiRptTlvTag_eUlProcLatency = 205,
    apiRptTlvTag_eErrIndNum = 206,
    apiRptTlvTag_eNpiRssiRptAddr = 207,
    apiRptTlvTag_eNumTlvs = 3,//

#if (ON == VT_ICIC_MES)
    /*PHY report measurement for ICIC related TLVs*/
    apiMesRptTlvTag_eICICMesStatPeriodic  = 208,
    apiMesRptTlvTag_eNumTlvs = 1,//
#endif
    
#if (ON == VT_CPRI_DELAY_FROM_CONFIG)
    apiCpriCfgTlvTag_eRxCpriDelay = 209,
    apiCpriCfgTlvTag_eNumTlvs = 1,
#endif
#if (ON == VT_CPRI_CHANGE_ANT_PORT)
    apiCpriCfgTlvTag_eRxAntPortId = 210,
    apiCpriSwitchPortTlvTag_eNumTlvs = 1,
#endif
    apiCfgTlvTag_eButt = 255
}ltePhyL2ApiTlvTag_eum;

/*PUSCH Receiver selection Enumeration*/
typedef enum{
    apiPuschReceiver_eMrc = 0,
    apiPuschReceiver_eIrc,
    apiPuschReceiver_eMrcIrcAdpt,

    apiPuschReceiver_eButt
}apiPuschReceiver_eum;

/* Duplex Mode Enumeration */
typedef enum{
    apiDuplexMode_eTdd = 0,
    apiDuplexMode_eFdd,
    apiDuplexMode_eHdFdd,

    apiDuplexMode_eButt
}apiDuplexMode_eum;

/* HARQ Feedback Mode Enumeration */
typedef enum{
    apiHarqMode_frt1a1b = 0,       // FDD: format 1a/1b
    apiHarqMode_eBundling = 0,    // TDD:format 1a/1b bundling
    apiHarqMode_eMultiplexing,    // TDD:format 1a/ab multiplexing; FDD: reserved
    apiHarqMode_eSpecialBundling, // TDD:special bundling; FDD: reserved
    apiHarqMode_frt1bCs,         // TDD/FDD:format 1b with channel selection
    apiHarqMode_frt3,              // TDD/FDD:format 3

    apiHarqMode_eButt
}apiHarqMode_enum;

/* CP Type Enumeration */
typedef enum{
    apiCpType_eNormal = 0,
    apiCpType_eExtended,

    apiCpType_eButt
}apiCpType_enum;

typedef enum{
    apiRsHop_eNO = 0,
    apiRsHop_eGrpHopping,
    apiRsHop_eSeqHopping,

    apiRsHop_eButt
}apiRsHop_enum;

typedef enum{
    apiNDI_eRetran = 0,
    apiNDI_eNewData,

    apiNDI_eButt
}apiNDI_enum;

typedef enum{
    apiSfnSyncMode_ePhyIsMaster = 0,
    apiSfnSyncMode_eMacIsMaster,

    apiSfnSyncMode_eButt
}apiSfnSyncMode_enum;

typedef enum{
    apiPhyRunningMode_eDDR = 0,
    apiPhyRunningMode_eRF,

    apiPhyRunningMode_eButt
}apiPhyRunningMode_enum;
/******************************************** General Message Format ********************************
*[1]3.1 Table 3
***************************************************************************************************/
typedef struct AC_STRUCTURE_PACK
{
    uint8   msgTypeId;
    uint8   msgVdrSpecFlag; // BOOL, 0: no vendor-specific bytes, 1: have vendor-specific bytes, the length is the first word of the vendor-specific
    uint16  msgBodyLength; // In bytes, Length of Message body
    /*  Message Body */
    /* Vendor-specific message body */
}ltePhyL2ApiMsgHdr_t;

/******************************************** Configuration Messages ********************************
* [1]section 3.2
***************************************************************************************************/
/* TLV format ,
*[1] 3.2.3 , Table 17
*/
/* value parameter is 32 bits */
typedef struct AC_STRUCTURE_PACK
{
    uint8   tag;
    uint8   length; // in bytes
    char    value[API_TLV_STRING_LEN_MAX];
}apiCfgTlvString_t;

/* value parameter is 32 bits */
typedef struct AC_STRUCTURE_PACK
{
    uint8   tag;
    uint8   length; // in bytes
    uint32  value;
    uint16  padding;
}apiCfgTlv32_t;

/* value parameter is 16 bits */
typedef struct AC_STRUCTURE_PACK
{
    uint8   tag;
    uint8   length; // in bytes, length of value
    uint16  value;
}apiCfgTlv16_t;
/* value parameter is 8 bits */
typedef struct AC_STRUCTURE_PACK
{
    uint8   tag;
    uint8   length; // in bytes
    uint8   value;
    uint8   padding;
}apiCfgTlv8_t;

/* General Configuration TLVs , 1 - 5 */
typedef struct AC_STRUCTURE_PACK
{
    apiCfgTlv16_t duplexMode;
    apiCfgTlv16_t pcifchPowerOffs;
    apiCfgTlv16_t PB;
    apiCfgTlv16_t dlCpType;
    apiCfgTlv16_t ulCpType;
}apiCfgTlvGeneral_t;

/* General Configuration Values, 1 - 5 */
typedef struct AC_STRUCTURE_PACK
{
    uint16 duplexMode;
    uint16 pcifchPowerOffs;
    uint16 PB;
    uint16 dlCpType;
    uint16 ulCpType;
}apiCfgValueGeneral_t;

/* RF Config TLVs , 6 - 10 */
typedef struct AC_STRUCTURE_PACK
{
    apiCfgTlv16_t dlBW;
    apiCfgTlv16_t ulBW;
    apiCfgTlv16_t rsPower;
    apiCfgTlv16_t txAntPorts;
    apiCfgTlv16_t rxAntPorts;
}apiCfgTlvRF_t;

/* RF Config Values , 6 - 10 */
typedef struct AC_STRUCTURE_PACK
{
    uint16 dlBW;
    uint16 ulBW;
    uint16 rsPower;
    uint16 txAntPorts;
    uint16 rxAntPorts;
}apiCfgValueRF_t;

/* PHICH Config TLVs, 11 -13  */
typedef struct AC_STRUCTURE_PACK
{
    apiCfgTlv16_t resource;
    apiCfgTlv16_t duration;
    apiCfgTlv16_t powerOffs;
}apiCfgTlvPHICH_t;

/* PHICH Config Values, 11 -13  */
typedef struct AC_STRUCTURE_PACK
{
    uint16 resource;
    uint16 duration;
    uint16 powerOffs;
}apiCfgValuePHICH_t;

/* SCH Config TLVs , 14 -16 */
typedef struct AC_STRUCTURE_PACK
{
    apiCfgTlv16_t pssEPRE;
    apiCfgTlv16_t sssEPRE;
    apiCfgTlv16_t cellId;
}apiCfgTlvSCH_t;

/* SCH Config Values , 14 -16 */
typedef struct AC_STRUCTURE_PACK
{
    uint16 pssEPRE;
    uint16 sssEPRE;
    uint16 cellId;
}apiCfgValueSCH_t;

/* PRACH Config TLVs  17 -  21 */
typedef struct AC_STRUCTURE_PACK
{
    apiCfgTlv16_t cfgIdx;
    apiCfgTlv16_t rootSeqIdx;
    apiCfgTlv16_t Ncs;
    apiCfgTlv16_t HighSpdFlag;
    apiCfgTlv16_t freqOffs;
}apiCfgTlvPRACH_t;

/* PRACH Config Values  17 -  21 */
typedef struct AC_STRUCTURE_PACK
{
    uint16 cfgIdx;
    uint16 rootSeqIdx;
    uint16 Ncs;
    uint16 HighSpdFlag;
    uint16 freqOffs;
}apiCfgValuePRACH_t;

/* PUSCH Config TLVs, 22 -24  */
typedef struct AC_STRUCTURE_PACK
{
    apiCfgTlv16_t hopMode;
    apiCfgTlv16_t hopOffs;
    apiCfgTlv16_t numSubBands;
}apiCfgTlvPUSCH_t;

/* PUSCH Config Values, 22 -24  */
typedef struct AC_STRUCTURE_PACK
{
    uint16 hopMode;
    uint16 hopOffs;
    uint16 numSubBands;
}apiCfgValuePUSCH_t;

/*PUCCH Config TLVs, 25 - 28 */
typedef struct AC_STRUCTURE_PACK
{
    apiCfgTlv16_t deltaShift;
    apiCfgTlv16_t NcqiRb;
    apiCfgTlv16_t NanCs;
    apiCfgTlv16_t N1PucchAn;
}apiCfgTlvPUCCH_t;

/*PUCCH Config Values, 25 - 28 */
typedef struct AC_STRUCTURE_PACK
{
    uint16 deltaShift;
    uint16 NcqiRb;
    uint16 NanCs;
    uint16 N1PucchAn;
}apiCfgValuePUCCH_t;

/* SRS Config TLVs, 29 - 32 */
typedef struct AC_STRUCTURE_PACK
{
    apiCfgTlv16_t bwCfg;
    apiCfgTlv16_t maxUpPts;
    apiCfgTlv16_t srsSfrmCfg;
    apiCfgTlv8_t  srsAckSameSfrm; // uint8
}apiCfgTlvSRS_t;

/* SRS Config Values, 29 - 32 */
typedef struct AC_STRUCTURE_PACK
{
    uint16 bwCfg;
    uint16 maxUpPts;
    uint16 srsSfrmCfg;
    uint8  srsAckSameSfrm;
    uint8  padding;
}apiCfgValueSRS_t;

/* Uplink Reference Signal Config TLVs , 33 - 35*/
typedef struct AC_STRUCTURE_PACK
{
    apiCfgTlv16_t hopMode;
    apiCfgTlv16_t deltaSeqShift;
    apiCfgTlv16_t cs1DMRS;
}apiCfgTlvULRS_t;

/* Uplink Reference Signal Config Values , 33 - 35*/
typedef struct AC_STRUCTURE_PACK
{
    uint16 hopMode;
    uint16 deltaSeqShift;
    uint16 cs1DMRS;
}apiCfgValueULRS_t;

/* TDD Frame Structure Config TLVs, 36 - 37 */
typedef struct AC_STRUCTURE_PACK
{
    apiCfgTlv16_t frmStrucCfg;
    apiCfgTlv16_t speSfrmPattern;
}apiCfgTlvTdd_t;

/* TDD Frame Structure Config Values, 36 - 37 */
typedef struct AC_STRUCTURE_PACK
{
    uint16 frmStrucCfg;
    uint16 speSfrmPattern;
}apiCfgValueTdd_t;

/*   TLVs used by l1 to report physical capabilities to L2/L3 Software , 40 - 44 */
typedef struct AC_STRUCTURE_PACK
{
    apiCfgTlv16_t dlBWsuppInRB;
    apiCfgTlv16_t ulBWsuppInRB;
    apiCfgTlv16_t dlModSupport;
    apiCfgTlv16_t ulModSupport;
    apiCfgTlv16_t phyAntCapabi;
}apiCfgTlvCapab_t;

/*   Values used by l1 to report physical capabilities to L2/L3 Software , 40 - 44 */
typedef struct AC_STRUCTURE_PACK
{
    uint16 dlBWsuppInRB;
    uint16 ulBWsuppInRB;
    uint16 dlModSupport;
    uint16 ulModSupport;
    uint16 phyAntCapabi;
}apiCfgValueCapab_t;

/* TLVs used by l2/l3 softawre to configure the interaction between l2/l3 and l1, 50 - 51 */
typedef struct AC_STRUCTURE_PACK
{
    apiCfgTlv16_t dataReportMode;
    apiCfgTlv16_t SFN_SF;
}apiCfgTlvInteract_t;


/* TLVs used by l2/l3 softawre to configure PHY mode, 61 - 71, 211-214 */
typedef struct AC_STRUCTURE_PACK
{
    apiCfgTlv16_t sfnSyncMasterMode;  // 61
    apiCfgTlv16_t runningMode;        // 62
    apiCfgTlv16_t timerMode;          // 63
    apiCfgTlv16_t symTimerTick;       // 64
    apiCfgTlv16_t txAntMask;          // 65
    apiCfgTlv16_t rxAntMask;          //66
    apiCfgTlv16_t pssSssTransAntPort; //67
    apiCfgTlv16_t pssSssTransMode;    // 68
    apiCfgTlv16_t txVecLength;        //69
    apiCfgTlv16_t rxVecLength;        // 70
    apiCfgTlv16_t aicUlTdDataEna;     // 71
    apiCfgTlv16_t rfTxPhyAnt;         // 120
    apiCfgTlv16_t puschAckScaling;    // 211
    apiCfgTlv16_t pucchAckScaling;    //212
    apiCfgTlv16_t prachPreambleScaling; // 213
    apiCfgTlv16_t dlMsgTimeOffset;      //214
    apiCfgTlv16_t puschMrcIrcSwitch;    //215
#if (ON == VT_ICIC_MES)
    apiCfgTlv16_t iCICValCfg;			//208
#endif
#if (ON == VT_CPRI_DELAY_FROM_CONFIG)
    apiCfgTlv16_t rxCpriDelay;			//209
#endif
#if (ON == VT_CPRI_CHANGE_ANT_PORT)
	apiCfgTlv16_t rxAntPortId; 			//210
#endif
}apiCfgTlvPhyCtrl_t;

/* values used by l2/l3 softawre to configure PHY mode, 61 - 68 */
typedef struct AC_STRUCTURE_PACK
{
    uint16 sfnSyncMasterMode;
    uint16 runningMode;
    uint16 timerMode;
    uint16 symTimerTick;
    uint16 txAntMask;
    uint16 rxAntMask;
    uint16 pssSssTransAntPort;
    uint16 pssSssTransMode;
    uint16 txVecLength;
    uint16 rxVecLength;
}apiCfgValuePhyCtrl_t;


/* TLVs used by l2/l3 softawre to get PHY version, 200 - 204 */
typedef struct AC_STRUCTURE_PACK
{
    apiCfgTlvString_t phyVersion;
    apiCfgTlvString_t fapiVersion;
    apiCfgTlvString_t buildTime;
    apiCfgTlvString_t sdosVersion;
    apiCfgTlvString_t mapleVersion;
}apiCfgTlvPhyVersion_t;

#if (ON == VT_ICIC_MES)
/*TLVs in MES  when PHY in Config state for FDD*/
/* MES Config TLVs , 208 */
typedef struct AC_STRUCTURE_PACK
{
	apiCfgTlv16_t iCICMesStatPeriodic;
}apiCfgTlvMES_t;

/* MES Config Values , 208 */
typedef struct AC_STRUCTURE_PACK
{
	uint32 iCICMesStatPeriodic;
}apiCfgValueMES_t;
#endif

/* [1]3.2.1 Param */
/*********** PARAM.request , no message body */

/* TLVs in PARAM.response when PHY in IDLE state for TDD and FDD, is the same, Table 6 and 8*/
typedef struct AC_STRUCTURE_PACK
{
    apiCfgTlv16_t          phyState;
    apiCfgTlvPhyVersion_t  phyVersion;
    apiCfgTlvCapab_t       capability;
}apiParamRsp_IDLE_t;
/* TLVs in PARAM.response when PHY in Config state for TDD, Table 7*/
typedef struct AC_STRUCTURE_PACK
{
    apiCfgTlv16_t       phyState;
    apiCfgTlvPhyVersion_t  phyVersion;
    apiCfgTlvCapab_t    capability;
    apiCfgTlvGeneral_t  general;
    apiCfgTlvRF_t       rfConfig;
    apiCfgTlvPHICH_t    phichCfg;
    apiCfgTlvSCH_t      schConfig;
    apiCfgTlvPRACH_t    prachCfg;
    apiCfgTlvPUSCH_t    puschCfg;
    apiCfgTlvPUCCH_t    pucchCfg;
    apiCfgTlvSRS_t      srsConfig;
    apiCfgTlvULRS_t     ulRsCfg;
    apiCfgTlvTdd_t      tddCfg; // tdd specific
    apiCfgTlv16_t       dataReportMode;
}apiParamRspTDD_CFGED_t;

/* TLVs in PARAM.response when PHY in Config state for FDD, Table 9*/
typedef struct AC_STRUCTURE_PACK
{
    apiCfgTlv16_t       phyState;
    apiCfgTlvPhyVersion_t  phyVersion;
    apiCfgTlvCapab_t    capability;
    apiCfgTlvGeneral_t  general;
    apiCfgTlvRF_t       rfConfig;
    apiCfgTlvPHICH_t    phichCfg;
    apiCfgTlvSCH_t      schConfig;
    apiCfgTlvPRACH_t    prachCfg;
    apiCfgTlvPUSCH_t    puschCfg;
    apiCfgTlvPUCCH_t    pucchCfg;
    apiCfgTlvSRS_t      srsConfig;
    apiCfgTlvULRS_t     ulRsCfg;
    apiCfgTlv16_t       dataReportMode;
#if (ON == VT_ICIC_MES)
    apiCfgTlvMES_t 		iCICValCfg;
#endif
}apiParamRspFDD_CFGED_t;

/*********** PARAM.response, message body, Table 5 */
typedef struct AC_STRUCTURE_PACK
{
    uint8   errorCode; // only MSG_OK and MSG_INVALIDATE_STATE are support
    uint8   numTLVs;
    /* TLVs , variable size in message, message body is different when PHY in IDLE/Config state */
    union {
         apiParamRsp_IDLE_t   info_Idle;
        apiParamRspTDD_CFGED_t  infoTdd_Cfged;
        apiParamRspFDD_CFGED_t  infoFdd_Cfged;
    }TLVs;
}ltePhyL2ApiParamRsp_t;

/* [1]3.2.2 Config */
/* TDD for IDLE and CONFIGURED state , message body, Table 12 */
typedef struct AC_STRUCTURE_PACK
{
    apiCfgTlvGeneral_t  general;
    apiCfgTlvRF_t       rfConfig;
    apiCfgTlvPHICH_t    phichCfg;
    apiCfgTlvSCH_t      schConfig;
    apiCfgTlvPRACH_t    prachCfg;
    apiCfgTlvPUSCH_t    puschCfg;
    apiCfgTlvPUCCH_t    pucchCfg;
    apiCfgTlvSRS_t      srsConfig;
    apiCfgTlvULRS_t     ulRsCfg;
    apiCfgTlvTdd_t      tddCfg; // tdd specific
    apiCfgTlv16_t       dataReportMode;
    apiCfgTlvPhyCtrl_t  phyModeCtrl;
}apiCfgReqTDD_IDLE_CFGED_t;
/* FDD for IDLE and CONFIGURED state , message body, Table 13 */
typedef struct AC_STRUCTURE_PACK
{
    apiCfgTlvGeneral_t  general;
    apiCfgTlvRF_t       rfConfig;
    apiCfgTlvPHICH_t    phichCfg;
    apiCfgTlvSCH_t      schConfig;
    apiCfgTlvPRACH_t    prachCfg;
    apiCfgTlvPUSCH_t    puschCfg;
    apiCfgTlvPUCCH_t    pucchCfg;
    apiCfgTlvSRS_t      srsConfig;
    apiCfgTlvULRS_t     ulRsCfg;
    apiCfgTlv16_t       dataReportMode;
    apiCfgTlvPhyCtrl_t  phyModeCtrl;
}apiCfgReqFDD_IDLE_CFGED_t;
/* TLVs in RUNNING state , message body, Table 14*/
typedef struct AC_STRUCTURE_PACK
{
    apiCfgTlv16_t  SFN_SF;
    //////////////////////// TBD , FFS TLVs ????
}apiCfgReq_RUNNING_t;

/*********** CONFIG.request , Table 11 */
typedef struct AC_STRUCTURE_PACK
{
    uint8   numTLVs;
    union {
        apiCfgReqFDD_IDLE_CFGED_t   infoFdd_IdleCfg;
        apiCfgReqTDD_IDLE_CFGED_t   infoTdd_IdleCfg;
        apiCfgReq_RUNNING_t         info_Running;
    }TLVs;
}ltePhyL2ApiConfigReq_t;

/*********** CONFIG.request value , Table 18 */
typedef struct
{
    uint16 headPadding;                  //for index start from 1
    apiCfgValueGeneral_t general;        //General Configuration Values, 1 - 5
    apiCfgValueRF_t      rfConfig;       //RF Config Values , 6 - 10
    apiCfgValuePHICH_t   phichCfg;       //PHICH Config Values, 11 -13
    apiCfgValueSCH_t     schConfig;      //SCH Config Values , 14 -16
    apiCfgValuePRACH_t   prachCfg;       //PRACH Config Values  17 -  21
    apiCfgValuePUSCH_t   puschCfg;       //PUSCH Config Values, 22 -24
    apiCfgValuePUCCH_t   pucchCfg;       //PUCCH Config Values, 25 - 28
    apiCfgValueSRS_t     srsConfig;      //SRS Config Values, 29 - 32
    apiCfgValueULRS_t    ulRsCfg;        //Uplink Reference Signal Config Values , 33 - 35
    apiCfgValueTdd_t     tddCfg;         //TDD Frame Structure Config Values, 36 - 37
    uint16               reserve0[2];    //vacant, 38 - 39
    apiCfgValueCapab_t   capability;     //Capabilities Values, 40 - 44 */
    uint16               reserve1[5];    //vacant, 45 - 49
    uint16               dataReportMode; //Data Report Mode, 50
    uint16               SFN_SF;         //SFN/SF, 51
    uint16               reserve2[8];    //vacant, 52 - 59
    uint16               phyState;       //PHY State, 60
    apiCfgValuePhyCtrl_t phyModeCtrl;    //PHY mode,61-68
#if (ON == VT_ICIC_MES)
    apiCfgValueMES_t 	iCICValCfg;
#endif
}ltePhyL2ApiConfigReqValue_t;

/*********** CONFIG.response, message body, Table 15 */
typedef struct AC_STRUCTURE_PACK
{
    uint8   errorCode; // only MSG_OK and MSG_INVALIDATE_CONFIG are support
    uint8   numInvalidTlvs;
    uint8   numMissingTlvs;
    /* Invalid and unsupport Tlvs , size is variable depend on numInvalidTlvs, MAX apiCfgTlvTag_eButt for structure define */
    apiCfgTlv16_t   invalidTlvs[apiCfgTlvTag_eButt];
    /* Missing Tlvs, size is variable depend on numMissingTlvs, MAX apiCfgTlvTag_eButt for structure define  */
    apiCfgTlv16_t   missingTlvs[apiCfgTlvTag_eButt];
}ltePhyL2ApiConfigRsp_t;

/* [1]3.2.4 START */
/*********** START.request , no message body , 3.2.4.1*/
/* If error occurs for START.request , ERROR.indication will be sent with MSG_INVALIDATE_STATE*/

/* [1]3.2.5 STOP */
/*********** STOP.request , no message body , 3.2.5.1*/
/* If error occurs for STOP.request , ERROR.indication will be sent with MSG_INVALIDATE_STATE, 3.2.5.3*/
/*********** STOP.indication , no message body , 3.2.5.2*/

/* [1]3.2.6 UE CONFIG  */

/* CQI Config in UE_CONFIG.request */
typedef struct AC_STRUCTURE_PACK
{
    apiCfgTlv16_t     pucchResIdx;
    apiCfgTlv16_t     pmiCfgIdx;
    apiCfgTlv16_t     riCfgIdx;
    apiCfgTlv8_t      AnCqiSimul; // Indicates if simultaneous trasmission of CQI and ACK/NACK
}apiUeCfgTlvCQI_t;
/* ACK/NACK Config in UE_CONFIG.request */
typedef struct AC_STRUCTURE_PACK
{
    apiCfgTlv8_t      repeFactor;
    apiCfgTlv16_t     n1PUCCHAnRep;
    apiCfgTlv8_t      tddAnFbMode; // ACK/NACK feedback mode, apiHarqMode_enum
}apiUeCfgTlvAN_t;
/* SRS Config in UE_CONFIG.request */
typedef struct AC_STRUCTURE_PACK
{
    apiCfgTlv8_t      bandwidth;
    apiCfgTlv8_t      hoppingBW;
    apiCfgTlv8_t      freqDomainPos;
    apiCfgTlv8_t      duration;
    apiCfgTlv16_t     ISRS;
    apiCfgTlv8_t      transComb;
    apiCfgTlv8_t      srCS; // sounding reference cyclic shift
}apiUeCfgTlvSRS_t;
/* SR Config in UE_CONFIG.request */
typedef struct AC_STRUCTURE_PACK
{
    apiCfgTlv16_t     pucchResIdx;
    apiCfgTlv8_t      cfgIdx;
}apiUeCfgTlvSR_t;
/* SPS Config in UE_CONFIG.request */
typedef struct AC_STRUCTURE_PACK
{
    apiCfgTlv16_t     srInterval; // SPS dl config scheduling interval
    apiCfgTlv16_t     pucchAnResCfg; // DL n1PUCCH AN Persistent
}apiUeCfgTlvSPS_t;

/* [1]3.2.7 UE RELEASE  */
/*********** UE_RELEASE.request , message body , 3.2.7.1, Table 27 & 28 */
typedef struct AC_STRUCTURE_PACK
{
    uint8   numTLVs;
    apiCfgTlv32_t     handle;
    apiCfgTlv16_t     rnti;
}ltePhyL2ApiUeReleaseReq_t;

/*********** UE_RELEASE.response , message body , 3.2.7.2 */
typedef struct AC_STRUCTURE_PACK
{
    uint8   errorCode; // only MSG_OK and MSG_INVALIDATE_CONFIG, MSG_INVALIDATE_STATE are support
    uint8   numInvalidTlvs;
    uint8   numMissingTlvs;
    /* Invalid and unsupport Tlvs , size is variable depend on numInvalidTlvs, MAX NUM for structure define */
    apiCfgTlv16_t invalidTlvs[3]; // 2 TLVs in request, one is TLV32, the other is TLV16
    /* Missing Tlvs, size is variable depend on numMissingTlvs, MAX NUM for structure define  */
    apiCfgTlv16_t missingTlvs[3];
}ltePhyL2ApiUeReleaseRsp_t;


/* [1]3.2.8.1 ERROR INDICATION  */
/* Error code dependent values if MSG_INVALIDATE_STATE and MSG_BCH_MISSING, Table 32  */
    /* Not used */
/* Error code dependent values if SFN_OUT_OF_SYNC and MSG_INVALIDATE_SFN, Table 33 */
typedef struct AC_STRUCTURE_PACK
{
    uint16  recSFN_SF;// received SFN/SF
    uint16  expSFN_SF;// expected SFN/SF
}apiErrorInd_sfn_t;
/* Error code dependent values if MSG_PDU_ERR, Table 34 */
typedef struct AC_STRUCTURE_PACK
{
    uint16    subErrorCode; // [1]section 3.4.1
    uint16    rnti;
    uint8     direction:1; // 0 is DL, 1 is UL
    uint8     pduType:7;
}apiErrorInd_pdu_t;
/* Define a structure to wrap the number of pdu error and all the pdu error structure*/
typedef struct AC_STRUCTURE_PACK
{
    uint8 errIndNum;
    apiErrorInd_pdu_t apiErrorInd_pdu[AC_MAX_ERR_NUM_PER_ERR_TYPE];
}apiErrorInd_pdu_all_t;
/* Error code dependent values if MSG_HI_ERR, Table 35 */
typedef struct AC_STRUCTURE_PACK
{
    uint16  subErrorCode; // [1]section 3.4.1
    uint8   lowestUlRbIndx;
}apiErrorInd_hi_t;

/* Define a structure to wrap the number of hi error and all the hi error structure*/
typedef struct AC_STRUCTURE_PACK
{
    uint8 errIndNum;
    apiErrorInd_hi_t apiErrorInd_hi[AC_MAX_ERR_NUM_PER_ERR_TYPE];
}apiErrorInd_hi_all_t;
/* Error code dependent values if MSG_TX_ERR, Table 36 */
typedef struct AC_STRUCTURE_PACK
{
    uint16  subErrorCode; // [1]section 3.4.1
    uint16  pduIdx;
}apiErrorInd_tx_t;
/* Define a structure to wrap the number of tx error and all the tx error structure*/
typedef struct AC_STRUCTURE_PACK
{
    uint8 errIndNum;
    apiErrorInd_tx_t apiErrorInd_tx[AC_MAX_ERR_NUM_PER_ERR_TYPE];
}apiErrorInd_tx_all_t;

/*********** ERROR.indication , message body , 3.2.8.1, Table 31 */
typedef struct AC_STRUCTURE_PACK
{
    uint8   messageId;
    uint8   errorCode;
    union {
        apiErrorInd_sfn_t   sfnErr;
        apiErrorInd_pdu_all_t   pduErr;
        apiErrorInd_hi_all_t    hiErr;
        apiErrorInd_tx_all_t    txErr;
    }values;
}apiErrorIndStru_t;

typedef struct AC_STRUCTURE_PACK
{
	uint32 allErrorNum;
	apiErrorIndStru_t errorInd[AC_MAX_ERR_TYPE_NUM];
} ltePhyL2ApiErrorIndAll_t;


/******************************************** Subframe Messages ********************************
* [1]section 3.3
***************************************************************************************************/
/* [1]3.3.1 SUBFRAME  */
/*********** SUBFRAME.indication , message body , 3.3.1.1 */
typedef struct AC_STRUCTURE_PACK
{
    uint16  SFN_SF; // system frame number and subframe number
}ltePhyL2ApiSubframeInd_t;

/* [1]3.3.1.2 DL_CONFIG  */
/* DL DCI Format Type */
typedef enum{
    apiDciFormatDL_e1 = 0,
    apiDciFormatDL_e1A,
    apiDciFormatDL_e1B,
    apiDciFormatDL_e1C,
    apiDciFormatDL_e1D,
    apiDciFormatDL_e2,
    apiDciFormatDL_e2A,
    apiDciFormatDL_e2B,
    apiDciFormatDL_e2C,

    apiDciFormatDL_eButt
}apiDciFormatDL_eum;

/* DL Virtula RB assignment flag */
typedef enum
{
    apiVrbFlagDL_eLocalized = 0,
    apiVrbFlagDL_eDistributed,

    apiVrbFlagDL_eButt
}apiVrbFlagDL_eum;

/* DL Resource Allocation Type */
typedef enum
{
    apiResAllocTypeDL_e0 = 0,
    apiResAllocTypeDL_e1,
    apiResAllocTypeDL_e2,
    apiResAllocTypeDL_e3,

    apiResAllocTypeDL_eButt
}apiResAllocTypeDL_eum;

/* Modulation Type */
typedef enum
{
    apiModulationDL_eQPSK = 2,
    apiModulationDL_eQAM16 = 4,
    apiModulationDL_eQAM64 = 6,
    apiModulationDL_eQAM256 = 8,

    apiModulationDL_eButt
}apiModulationDL_enum;

typedef enum
{
    apiRntiType_eC = 1,
    apiRntiType_eRA_P_SI,
    apiRntiType_eSpsC,

    apiRntiType_eButt
}apiRntiType_enum;

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

/* DL Transmission Scheme */
typedef enum
{
    apiTransSchemeDL_eSingleAnt0 = 0,
    apiTransSchemeDL_eTxDiversity,
    apiTransSchemeDL_eLargeDlyCDD,
    apiTransSchemeDL_eCloseLpSpatialMp, // closed loop spatial multiplexing
    apiTransSchemeDL_eMultiUserMimo,
    apiTransSchemeDL_eCloseLpRank1,
    apiTransSchemeDL_eSinglePort5,
    apiTransSchemeDL_eSinglePort7,
    apiTransSchemeDL_eSinglePort8,
    apiTransSchemeDL_eDualPorts7And8,
    apiTransSchemeDL_eUpTo8Layer,

    apiTransSchemeDL_eButt
}apiTransSchemeDL_enum;

/* BfVectorType Structure , Table 43 */
//#define MAX_NUM_PHYSICAL_ANTENNAS 4
typedef struct AC_STRUCTURE_PACK
{
    uint8   subbandIdx;
    uint8   numAnts;
    uint16  bfValue[AC_MAX_NUM_TX_ANT]; // real 8 bits followed by image 8 bits
}apiDlSchBfVector;

/* DLSCH PDU Configuration, Table 42*/
//#define MAX_NUM_SUBBANDS  13
typedef struct AC_STRUCTURE_PACK
{
    uint16  length;
    uint16  pduIdx;
    uint16  rnti;
    uint8   resAllocType;// apiResAllocTypeDL_eum, type 0,1,2,3.  3 = type 2 (allocated by DCI format 1C)
    uint8   virtRbFlag;// apiVrbFlagDL_eum, localized or distributed
    uint32  rbCoding;
    uint8   modulation; // apiModulationDL_enum
    uint8   rv;
    uint8   tbIdx; // 2 means this is the 2nd transport block
    uint8   tbCwSwapFlag;
    uint8   transScheme; // apiTransSchemeDL_enum
    uint8   numLayers;
    uint8   numSubbands; // Number of Subbands
    uint8   codeBookIdx[AC_MAX_NUM_DL_BF_SUBBANDS]; // size is variable, only numSubbands sent in the message
    uint8   ueCategory;
    uint8   PA;
    uint8   deltaPowerOffs;
    uint8   N_GAP;
    uint8   N_PRB;
    uint8   TransMode;
    uint8   numPrbPerSubband; // number of PRBS that are treated as one subbband
    uint8   numBfVector; // one bf vector is specified for each subband
    apiDlSchBfVector    bfVector[AC_MAX_NUM_DL_BF_SUBBANDS]; // size is variable depends on numBfVectors
}apiDlCfgPduDlSCH_t;

/* PCH PDU Configuration, Table 44*/
typedef struct AC_STRUCTURE_PACK
{
    uint16  length;
    uint16  pduIdx;
    uint16  P_RNTI;
    uint8   resAllocType; // apiResAllocTypeDL_eum, type 2
    uint8   virtRbFlag;// apiVrbFlagDL_eum,localized or distributed
    uint32  rbCoding;
    uint8   mcs;
    uint8   rv;
    uint8   numTBs; // only 1 tb is sent on the PCH per subframe, value is 1
    uint8   tb2CwSwapFlag;
    uint8   transScheme;// apiTransSchemeDL_enum, only 0, 1, 6 support
    uint8   numLayers;
    uint8   codebookIdx;
    uint8   ueCategory;
    uint8   PA;
    uint16  transPower;
    uint8   N_PRB; // 0, N_PRB = 2; 1, N_PRB = 3
}apiDlCfgPduPCH_t;

 /****** FAPI extension for Release 10***************Start*/
/*DCI DL PDU Release 10 parameters, [2]table 3-39*/
typedef struct AC_STRUCTURE_PACK 
{   uint8   csiSizeOfDciFmt0; // extended by AC for DCI packing,1:1 bit   2:2 bit
    uint8   ccsFlag; 
    uint8   carrierIndicator;
    uint8   srsFlag;
    uint8   srsRequest;
    uint8   antPortsNscid;
}apiDlDCIR10Param_t;

/* DCI DL PDU(type 100), extended by AC to support r10*/
typedef struct AC_STRUCTURE_PACK 
{
    apiDlCfgPduDCI_t    dlDCIPduR8;
    apiDlDCIR10Param_t  dlDciR10Param;
}apiDlCfgPduDCIR10_t;

/*DLSCH PDU Release 10 parameters, [2]table 3-45*/
typedef struct AC_STRUCTURE_PACK 
{
    uint8   csiRsPortNumForPmiReport; // 1,2,4,8, only valid in TM9, indicates the number of ant ports of codebook feedback from UE 
    uint8   csiRsFlag; //0:CSI-RS parameters are not valid; 1:CSI-RS parameters are valid    
    uint8   csiRsResConfig;
    uint16  csiRs0PowMap;
}apiDlschR10Param_t;

/*DLSCH PDU Release 9 parameters, [2]table 3-44*/
typedef struct AC_STRUCTURE_PACK 
{   
    uint8   Nscid;  // used with DCI format 2B and 2C,0: N_SCID = 0; 1:N_SCID = 1
}apiDlschR9Param_t;

/* DLSCH PDU(type 103), extended by AC to support r9*/
typedef struct AC_STRUCTURE_PACK 
{
    apiDlCfgPduDlSCH_t  dlschPduR8;
    apiDlschR9Param_t   dlschR9Param;
}apiDlCfgPduDlSCHR9_t;

/* DLSCH PDU(type 104), extended by AC to support r10*/
typedef struct AC_STRUCTURE_PACK 
{
    apiDlCfgPduDlSCH_t  dlschPduR8;
    apiDlschR9Param_t   dlschR9Param;
    apiDlschR10Param_t  dlschR10Param;
}apiDlCfgPduDlSCHR10_t;

/* CSI-RS PDU(type 6), extended by AC to support r10*/
typedef struct AC_STRUCTURE_PACK 
{
    uint8   csiRsPortNum; //1,2,4,8
    uint8   csiRsResConfig;
    uint16  transPower;
    uint16  csiRs0PowMap;
}apiDlCfgPduCSIRS_t;




 /****** FAPI extension for Release 10***************Stop*/

/* PDU Type Enum */
typedef enum
{
    apiDlCfgPduType_eDciDl = 0,
    apiDlCfgPduType_eBCH,
    apiDlCfgPduType_eMCH,
    apiDlCfgPduType_eDlSch,
    apiDlCfgPduType_ePCH,
    apiDlCfgPduType_eCSIRS = 6,
    apiDlCfgPduType_eDciDlR10 = 100,
    apiDlCfgPduType_eDlSchR9 = 103,
    apiDlCfgPduType_eDlSchR10 = 104,
    apiDlCfgPduType_eUnknown = 255
}apiDlCfgPduType_eum;

/* PDU Config structure in DL_CONFIG */
typedef struct AC_STRUCTURE_PACK
{
    uint8   pduType; // apiDlCfgPduType_eum
    uint8   pduSize;
    union {
        apiDlCfgPduDCI_t    dciPDU; // DCI DL PDU
        apiDlCfgPduBCH_t    bchPDU; // BCH PDU
        apiDlCfgPduMCH_t    mchPDU; // MCH PDU
        apiDlCfgPduDlSCH_t  dlschPDU; // DLSCH PDU
        apiDlCfgPduPCH_t    pchPDU; //PCH PDU
        apiDlCfgPduCSIRS_t  csirsPDU; //CSI RS PDU
        apiDlCfgPduDCIR10_t dciPDUR10; // DCI DL PDU R10,extended by AC to support r10
        apiDlCfgPduDlSCHR9_t dlschPDUR9; // DLSCH PDU R9, extended by AC to  support R9
        apiDlCfgPduDlSCHR10_t dlschPDUR10; // DLSCH PDU R10, extended by AC to  support R10
    }pduConfig;
}apiDlCfgPdu_t;

/*********** DL_CONFIG.request , message body , 3.3.1.2, Tabale 38 */
//#define MAX_NUM_DCI 90
typedef struct AC_STRUCTURE_PACK
{
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
    apiDlCfgPdu_t  PDU[AC_MAX_PDSCH_PER_SUBFRAME * 2 + AC_MAX_PDCCH_PER_SUBFRAME + 1];// 2 TB PDSCH + DCI DL + BCH
}ltePhyL2ApiDlConfigReq_t;

/* [1]3.3.1.3 UL_CONFIG  */
/* UL TX Mode */
typedef enum
{
    apiUlTxMode_eSISO_SIMO = 0,
    apiUlTxMode_eMIMO,

    apiUlTxMode_eButt
}apiUlTxMode_enum;

/** UL SCH PDU, 3.3.1.3.1, Table 46*/
typedef struct AC_STRUCTURE_PACK
{
    uint32  handle; //this filed will be used as the buffer address for UL_SCH data
    uint16  size;// in bytes
    uint16  rnti;
    uint8   rbStart;
    uint8   numRbs;
    uint8   modType; // apiModulationDL_enum
    uint8   cs2DMRS;
    uint8   freqHopFlag;
    uint8   freqHopBits;
    uint8   newDataInd;
    uint8   rv;
    uint8   harqProcNum;// HARQ Process number
    uint8   txMode; // apiUlTxMode_enum
    uint8   currTxNB; // the current HQRQ transmission count of this transport block valid if freqHop enable.
    uint8   Nsrs;// Indicate if the resource blocks allocated for this grant overlap with the SRS configuration.
}apiUlCfgPduUlSCH_t;

/* CQI_RI information, Table 48 */
typedef struct AC_STRUCTURE_PACK
{
    uint8   cqiPmiSizeRank1; // The size of DL CQI/PMI in bits in case of rank1 report
    uint8   cqiPmiSizeNotRank1; // The size of the CQI/PMI in bits in case of rank>1 report
    uint8   riSize; // in bits
    uint8   deltaOffsCqi; // Delta offset for CQI
    uint8   deltaOffsRi;
}apiUlCfgCqiRiInfo_t;

/* Initial Transmission Parameters, 3.3.1.3.5, Table 52 */
typedef struct AC_STRUCTURE_PACK
{
    uint8   NsrsIni; // 0, last OFDMA symbol is not punctured; 1, punctured
    uint8   iniNumRbs;
}apiUlCfgIniTransParams_t;

/** ULSCH_CQI_RI PDU, 3.3.1.3.2, Table 47*/
typedef struct AC_STRUCTURE_PACK
{
    apiUlCfgPduUlSCH_t          ulSchPdu;
    apiUlCfgCqiRiInfo_t         cqiRiInfo;
    apiUlCfgIniTransParams_t    iniParams;
}apiUlCfgPduUlschCqiRi_t;

/* HARQ Information, Table 50 */
typedef struct AC_STRUCTURE_PACK
{
    uint8   harqSize; // The size of ACK/NACK in bits
    uint8   deltaOffsHarq;
    uint8   NBundle:7; // used for PUSCH ACK/NACK bundling
    uint8   ackNackMode:1; // apiHarqMode_enums, this field is only used in TDD, and reserved in FDD
}apiUlCfgHarqInfo_t;

/** ULSCH_HARQ PDU, 3.3.1.3.3, Table 49 */
typedef struct AC_STRUCTURE_PACK
{
    apiUlCfgPduUlSCH_t          ulSchPdu;
    apiUlCfgHarqInfo_t          harqInfo;
    apiUlCfgIniTransParams_t    iniParams;
}apiUlCfgPduUlschHarq_t;

/** ULSCH_CQI_HARQ_RI PDU, 3.3.1.3.4, Table 51*/
typedef struct AC_STRUCTURE_PACK
{
    apiUlCfgPduUlSCH_t          ulSchPdu;
    apiUlCfgCqiRiInfo_t         cqiRiInfo;
    apiUlCfgHarqInfo_t          harqInfo;
    apiUlCfgIniTransParams_t    iniParams;
}apiUlCfgPduUlschCqiHarqRi_t;

/* Information in UCI */
/* CQI Information in UCI_CQI, 3.3.1.3.13, Table 60 */
typedef struct AC_STRUCTURE_PACK
{
    uint16  pucchIdx; // nPucch2
    uint8   cqiPmiSize; // in bits
}apiUciCqiInfo_t;

/* SR Information, 3.3.1.3.14 , Table 61 */
typedef struct AC_STRUCTURE_PACK
{
    uint16  pucchIdx;
}apiUciSrInfo_t;

 /****** FAPI extension for Release 10***************Start*/
/* apiPeriodicPmiRiRpt_t,[2]table 3-55 */
typedef struct AC_STRUCTURE_PACK 
{
    uint8 cqiPmiRiSize;
    uint8 ctrlType;  // 0:CQI/PMI   1:RI
}apiPeriodicPmiRiRpt_t ;

typedef struct AC_STRUCTURE_PACK
{
    uint8 riSize;
    uint8 cqiPmiSize[AC_MAX_NUM_RANK];  // variable length, 2^riSize
}aperiodicRiCqiPmiSize_t;

/* apiAperiodicPmiRiRpt_t,[2]table3-56 */
typedef struct AC_STRUCTURE_PACK 
{
    uint8 numCC; 
    aperiodicRiCqiPmiSize_t riCqiPmiSize[AC_MAX_NUM_CC]; // variable length
}apiAperiodicPmiRiRpt_t ;

/* apiPmiRiInfo_t, [2]table3-55 or 3-56*/
typedef struct AC_STRUCTURE_PACK 
{   union {
        apiPeriodicPmiRiRpt_t   periodicRpt;
        apiAperiodicPmiRiRpt_t  aperiodicRpt;
    }pmiRiInfo;
}apiPmiRiInfo_t;

/* apiUlCfgCsiInfoR10_t, [2]table3-54*/
typedef struct AC_STRUCTURE_PACK 
{
    uint8 rptType;
    uint8 deltaOffsCqi;
    uint8 deltaOffsRi;
    apiPmiRiInfo_t  pmiRiInfo;
}apiUlCfgCsiInfoR10_t ;



/* ULSCH_CQI_RI (type 101),extended by AC to support r10*/
typedef struct AC_STRUCTURE_PACK 
{   apiUlCfgPduUlSCH_t          ulSchPdu;
    apiUlCfgCsiInfoR10_t        cqiRiInfo; 
    apiUlCfgIniTransParams_t    iniParams;
} apiUlCfgPduUlschCqiRiR10_t;

/*apiUlCfgHarqInfoR10_t,[2]table3-58*/
typedef struct AC_STRUCTURE_PACK 
{
    uint8   harqSize; 
    uint8   deltaOffsHarq;
    uint8   NBundle;  // extended by AC for bundling
    uint8   ackNackMode;
}apiUlCfgHarqInfoR10_t;

/* ULSCH_HARQ(type 102),[2]table3-57*/
typedef struct AC_STRUCTURE_PACK 
{
    apiUlCfgPduUlSCH_t          ulSchPdu;
    apiUlCfgHarqInfoR10_t       harqInfo;
    apiUlCfgIniTransParams_t    iniParams;
}apiUlCfgPduUlschHarqR10_t;

/*ULSCH_CQI_HARQ_RI (type 103),[2]table3-59*/
typedef struct AC_STRUCTURE_PACK 
{
    apiUlCfgPduUlSCH_t          ulSchPdu;
    apiUlCfgCsiInfoR10_t        cqiRiInfo;
    apiUlCfgHarqInfoR10_t       harqInfo;
    apiUlCfgIniTransParams_t    iniParams;
}apiUlCfgPduUlschCqiHarqRiR10_t;

/* UCI HARQ Information,[2]table3-74*/
typedef struct AC_STRUCTURE_PACK
{
    uint8   harqSize;
    uint8   ackNackMode;
    uint8   numPucchRes;
    uint16  n_pucch_1_0;
    uint16  n_pucch_1_1;
    uint16  n_pucch_1_2;
    uint16  n_pucch_1_3;
}apiUciHarqInfoR10_t;

/* UCI_HARQ(type 106),[2]table3-63*/
typedef struct AC_STRUCTURE_PACK 
{
    uint32  handle;
    uint16  rnti;
    apiUciHarqInfoR10_t harqInfoR10;
}apiUlCfgPduUciHarqR10_t;



/* UCI_SR_HARQ(type 107),[2]table3-64*/
typedef struct AC_STRUCTURE_PACK 
{
    uint32  handle;
    uint16  rnti;
    apiUciSrInfo_t         srInfo;
    apiUciHarqInfoR10_t    harqInfoR10;
} apiUlCfgPduUciSrHarqR10_t;

/* UCI_CQI_HARQ(type 108),[2]table3-65*/
typedef struct AC_STRUCTURE_PACK 
{
    uint32  handle;
    uint16  rnti;
    apiUciCqiInfo_t  cqiInfo;
    apiUciHarqInfoR10_t    harqInfoR10;
}apiUlCfgPduUciCqiHarqR10_t;

/* UCI_CQI_SR_HARQ(type 110),[2]table3-67*/
typedef struct AC_STRUCTURE_PACK 
{
    uint32  handle;
    uint16  rnti;
    apiUciCqiInfo_t  cqiInfo;
    apiUciSrInfo_t      srInfo;
    apiUciHarqInfoR10_t    harqInfoR10;
}apiUlCfgPduUciCqiSrHarqR10_t;

/* ULSCH_UCI_CSI (type 113),[2]table3-78*/
typedef struct AC_STRUCTURE_PACK 
{
    apiUlCfgPduUlSCH_t  ulSchPdu;
    apiUciCqiInfo_t     cqiInfo;
}apiUlCfgPduUlschUciCsiR10_t;

/* ULSCH_UCI_HARQ (type 114),[2]table3-79*/
typedef struct AC_STRUCTURE_PACK 
{
    apiUlCfgPduUlSCH_t  ulSchPdu;
    apiUciHarqInfoR10_t harqInfoR10;
}apiUlCfgPduUlschUciHarqR10_t;

/* ULSCH_CSI_UCI_HARQ (type 115),[2]table3-80*/
typedef struct AC_STRUCTURE_PACK 
{
    apiUlCfgPduUlSCH_t  ulSchPdu;
    apiUlCfgCsiInfoR10_t  cqiRiInfo;
    apiUlCfgIniTransParams_t  iniParams;
    apiUciHarqInfoR10_t harqInfoR10;
}apiUlCfgPduUlschCsiUciHarqR10_t;
 /****** FAPI extension for Release 10***************Stop*/

/* HARQ Information TDD, Table 62; FDD, Table 63 */
/* FDD */
typedef struct AC_STRUCTURE_PACK
{
    uint16  pucchIdx;
    uint8   harqSize;
}apiUciHarqInfoFDD_t;

/* TDD */
typedef struct AC_STRUCTURE_PACK
{
    uint8   harqSize;
    uint8   ackNackMode; // apiHarqMode_enum
    uint8   numPucchRes;// The number of ACK/NACK responses received in this subframe.
    uint16  n_pucch_1_0;
    uint16  n_pucch_1_1;
    uint16  n_pucch_1_2;
    uint16  n_pucch_1_3;
}apiUciHarqInfoTDD_t;

typedef struct AC_STRUCTURE_PACK
{
    union {
        apiUciHarqInfoFDD_t fddHarqInfo;
        apiUciHarqInfoTDD_t tddHarqInfo;
    };
}apiUciHarqInfo_t;

/** UCI_CQI PDU, 3.3.1.3.6, Table 53 */
typedef struct AC_STRUCTURE_PACK
{
    uint32  handle;
    uint16  rnti;
    apiUciCqiInfo_t   cqiInfo;
}apiUlCfgPduUciCqi_t;

/** UCI_SR PDU, 3.3.1.3.7, Table 54 */
typedef struct AC_STRUCTURE_PACK
{
    uint32  handle;
    uint16  rnti;
    apiUciSrInfo_t    srInfo;
}apiUlCfgPduUciSr_t;


/** UCI_HARQ PDU, 3.3.1.3.8, Table 55 */
typedef struct AC_STRUCTURE_PACK
{
    uint32  handle;
    uint16  rnti;
    apiUciHarqInfo_t    harqInfo;
}apiUlCfgPduUciHarq_t;

/** UCI_SR_HARQ PDU, 3.3.1.3.9, Table 56 */
typedef struct AC_STRUCTURE_PACK
{
    uint32  handle;
    uint16  rnti;
    apiUciSrInfo_t      srInfo;
    apiUciHarqInfo_t    harqInfo;
}apiUlCfgPduUciSrHarq_t;

/** UCI_CQI_HARQ PDU, 3.3.1.3.10, Table 57 */
typedef struct AC_STRUCTURE_PACK
{
    uint32  handle;
    uint16  rnti;
    apiUciCqiInfo_t     cqiInfo;
    apiUciHarqInfo_t    harqInfo;
}apiUlCfgPduUciCqiHarq_t;

/** UCI_CQI_SR PDU, 3.3.1.3.11, Table 58 */
typedef struct AC_STRUCTURE_PACK
{
    uint32  handle;
    uint16  rnti;
    apiUciCqiInfo_t     cqiInfo;
    apiUciSrInfo_t      srInfo;
}apiUlCfgPduUciCqiSr_t;

/** UCI_CQI_SR_HARQ PDU, 3.3.1.3.12, Table 59 */
typedef struct AC_STRUCTURE_PACK
{
    uint32  handle;
    uint16  rnti;
    apiUciCqiInfo_t     cqiInfo;
    apiUciSrInfo_t      srInfo;
    apiUciHarqInfo_t    harqInfo;
}apiUlCfgPduUciCqiSrHarq_t;

/** SRS PDU Information, 3.3.1.3.16, Table 64 */
typedef struct AC_STRUCTURE_PACK
{
    uint32  handle;
    uint16  size;
    uint16  rnti;
    uint8   srsBw;
    uint8   freqPos;
    uint8   srsHopBw;
    uint8   transComb;
    uint16  Isrs;
    uint8   srsCs;// sounding reference cyclic shift
}apiUlCfgPduSrs_t;

/** HARQ_BUFFER PDU Information, 3.3.1.3.17, Table 65 */
/* Indicate for which UE the HARQ buffer should be released, not used yet? */
typedef struct AC_STRUCTURE_PACK
{
    uint32    handle;
    uint16    rnti;
}apiUlCfgPudHarqBuf_t;

/* PDU Type */
typedef enum
{
    apiUlCfgPduType_eUlSch = 0,
    apiUlCfgPduType_eUlSchCqiRi,
    apiUlCfgPduType_eUlSchHarq,
    apiUlCfgPduType_eUlSchCqiHarqRi,
    apiUlCfgPduType_eUciCqi,
    apiUlCfgPduType_eUciSr,
    apiUlCfgPduType_eUciHarq,
    apiUlCfgPduType_eUciSrHarq,
    apiUlCfgPduType_eUciCqiHarq,
    apiUlCfgPduType_eUciCqiSr,
    apiUlCfgPduType_eUciCqiSrHarq,
    apiUlCfgPduType_eSrs,
    apiUlCfgPduType_eHarqBuffer,
    apiUlCfgPduType_eUlSchCqiRiR10 = 101,
    apiUlCfgPduType_eUlSchHarqR10 = 102,
    apiUlCfgPduType_eUlSchCqiHarqRiR10 = 103,
    apiUlCfgPduType_eUciHarqR10 = 106,
    apiUlCfgPduType_eUciSrHarqR10 = 107,
    apiUlCfgPduType_eUciCqiHarqR10 = 108,
    apiUlCfgPduType_eUciCqiSrHarqR10 = 110,
    apiUlCfgPduType_eUlSchUciCsiR10 = 113,
    apiUlCfgPduType_eUlSchUciHarqR10 = 114,
    apiUlCfgPduType_eUlSchCsiUciHarqR10 = 115,
    apiUlCfgPduType_eUnKnown = 255
}apiUlCfgPduType_enum;

/* PDU Config structure in UL_CONFIG */
typedef struct AC_STRUCTURE_PACK
{
    uint8   pduType;  // apiUlCfgPduType_enum
    uint8   pduSize;
    union {
        apiUlCfgPduUlSCH_t          ulSch;// 0, UL SCH PDU
        apiUlCfgPduUlschCqiRi_t     ulSchCqiRi;// 1, ULSCH_CQI_RI PDU
        apiUlCfgPduUlschHarq_t      ulSchHarq;// 2, ULSCH_HARQ PDU
        apiUlCfgPduUlschCqiHarqRi_t ulSchCqiHarqRi;// 3, ULSCH_CQI_HARQ_RI PDU
        apiUlCfgPduUciCqi_t         uciCqi;// 4, UCI_CQI
        apiUlCfgPduUciSr_t          uciSr;// 5, UCI_SR
        apiUlCfgPduUciHarq_t        uciHarq; // 6, UCI_HARQ
        apiUlCfgPduUciSrHarq_t      uciSrHarq;// 7, UCI_SR_HARQ
        apiUlCfgPduUciCqiHarq_t     uciCqiHarq; // 8, UCI_CQI_HARQ
        apiUlCfgPduUciCqiSr_t       uciCqiSr; // 9, UCI_CQI_SR
        apiUlCfgPduUciCqiSrHarq_t   uciCqiSrHarq;// 10, UCI_CQI_SR_HARQ
        apiUlCfgPduSrs_t            srs;// 11, SRS PDU 
        apiUlCfgPduUlschCqiRiR10_t  ulSchCqiRiR10;//101,ULSCH_CQI_RI for R10 CA
        apiUlCfgPduUlschHarqR10_t   ulSchHarqR10;//102, ULSCH_HARQ for R10 CA
        apiUlCfgPduUlschCqiHarqRiR10_t ulSchCqiHarqRiR10;//103,ULSCH_CQI_HARQ_RI for R10 CA
        apiUlCfgPduUciHarqR10_t        uciHarqR10;//106,UCI_HARQ for R10 CA
        apiUlCfgPduUciSrHarqR10_t      uciSrHarqR10;//107, UCI_SR_HARQ for R10 CA
        apiUlCfgPduUciCqiHarqR10_t     uciCqiHarqR10;//108, UCI_CQI_HARQ for R10 CA
        apiUlCfgPduUciCqiSrHarqR10_t   uciCqiSrHarqR10;//110, UCI_CQI_SR_HARQ for R10 CA
        apiUlCfgPduUlschUciCsiR10_t    ulSchUciCsiR10;//113, ULSCH_UCI_CSI for R10 CA
        apiUlCfgPduUlschUciHarqR10_t   ulschUciHarqR10;//114,ULSCH_UCI_HARQ for R10 CA
        apiUlCfgPduUlschCsiUciHarqR10_t  ulschCsiUciHarqR10;//115,ULSCH_CSI_UCI_HARQ for R10 CA
    }pduConfig;
}apiUlCfgPdu_t;

/*********** UL_CONFIG.request , message body , 3.3.1.3, Tabale 45 */
typedef struct AC_STRUCTURE_PACK
{
    uint16  SFN_SF;
    uint16  length;
    uint8   numPDUs; // Number of PDUs
    uint8   rachFreqRes;
    uint8   srsPresent;
    /* Number of PDUs */
    /* NOTE: The size of each PDU is based on the PDU type,
    all the PDUs in the message are packed without any padding,
    We can not use array index to get next PDU directly,
    here Define a PDU array, just for information*/
    apiUlCfgPdu_t PDU[AC_MAX_PUSCH_PER_SUBFRAME + AC_MAX_PUCCH_PER_SUBFRAME + AC_MAX_SRS_PER_SUBFRAME]; 
}ltePhyL2ApiUlConfigReq_t;


/* [1]3.3.1.4 HI_DCI0  */
// HI_DCI0 PDU Type
typedef enum
{
    apiHIDCI0PduType_eHI = 0,
    apiHIDCI0PduType_eDciUl,
    apiHIDCI0PduType_eDciUlR10 = 101,
    apiHIDCI0PduType_eUnKnown = 255
}apiHIDCI0PduType_enum;

/* HI PDU, 3.3.1.4.1 Table 67 */
typedef struct AC_STRUCTURE_PACK
{
    uint8   rbStart;
    uint8   cs2DMRS;
    uint8   hiVal;
    uint8   I_PHICH;
    uint16  transPower;
}apiHIDCI0PduHI_t;

/* UL DCI Format */
typedef enum
{
    apiUlDciFormat_e0 = 0,
    apiUlDciFormat_e3,
    apiUlDciFormat_e3A,

    apiUlDciFormat_eButt
}apiUlDciFormat_enum;

/* UE TX Antenna Selection */
typedef enum
{
    apiUeTxAntSelect_eNotCfg = 0,
    apiUeTxAntSelect_eCfgPort0,
    apiUeTxAntSelect_eCfgPort1,

    apiUeTxAntSelect_eButt
}apiUeTxAntSelect_enum;

/* DCI UL PDU, 3.3.1.4.2 Table 68 */
typedef struct AC_STRUCTURE_PACK
{
    uint8   dciFormat; // apiUlDciFormat_enum
    uint8   cceIdx;
    uint8   aggreLevel;
    uint16  rnti;
    uint8   rbStart;
    uint8   numRbs;
    uint8   mcs;
    uint8   cs2DMRS;
    uint8   freqHopFlag; // indicates if hopping is being used
    uint8   freqHopBits;
    uint8   newDataInd;
    uint8   ueTxAntSelec; // apiUeTxAntSelect_enum
    uint8   TPC;
    uint8   cqiReq; // aperiodic CQI request flag, 0, not requested; 1, requested
    uint8   ulIdx;
    uint8   dlAssignIdx;
    uint32  tpcBitmap;
    uint16  transPower;
}apiHIDCI0PduUlDCI_t;

/*DCI UL PDU R10, [2]table 3-85*/
typedef struct AC_STRUCTURE_PACK 
{
    uint8   ccsFlag; 
    uint8   carrierIndicator;
    uint8   csiSize;
    uint8   srsFlag;
    uint8   srsRequest;
    uint8   rscAllocFlag;
    uint8   rscAllocType; 
    uint32  rbCoding;
    uint8   mcs_2;
    uint8   ndi_2;
    uint8   TPMI;
}apiUlDCIR10Param_t;

/* DCI UL PDU(type 101),[2]table 3-84*/
typedef struct AC_STRUCTURE_PACK 
{
    apiHIDCI0PduUlDCI_t  ulDCIPduR8;
    apiUlDCIR10Param_t   ulDCIR10Para;  
} apiHIDCI0PduUlDCIR10_t;

/* PDU structure in HI_DCI0 */
typedef struct AC_STRUCTURE_PACK
{
    uint8   pduType; // apiHIDCI0PduType_enum
    uint8   pduSize; // in bytes
    union {
        apiHIDCI0PduHI_t        hiPDU; // HI PDU
        apiHIDCI0PduUlDCI_t     ulDciPDU; //UL DCI PDU
        apiHIDCI0PduUlDCIR10_t  ulDciPDUR10; // UL DCI PDU R10
    }pduConfig;
}apiHIDCI0Pdu_t;

/*********** HI_DCI0.request , message body , 3.3.1.4, Table 66 */
typedef struct AC_STRUCTURE_PACK 
{
    uint16  SFN_SF;
    uint8   numDCIs;
    uint8   numHIs;
    /* NOTE: The size of each PDU is based on the PDU type,
    all the PDUs in the message are packed without any padding,
    We can not use array index to get next PDU directly,
    here Define a PDU array, just for information*/
    apiHIDCI0Pdu_t  PDU[AC_MAX_PDCCH_PER_SUBFRAME + AC_MAX_PHICH_PER_SUBFRAME];// DCI also put here
}ltePhyL2ApiHIDCI0Req_t;


/* [1]3.3.2 Downlink Data  */
/* TVL Structure for TX.request , Table 73 */
typedef struct AC_STRUCTURE_PACK
{
    uint16  tag;
    uint16  length;
    uint32  value; // offset to the payload
}apiDlDataTlv_t;

/* PDU structure, Table 72*/
typedef struct AC_STRUCTURE_PACK
{
   uint16   pduSize; // in bytes
   uint16   pduIdx; // reset to 0 every sbuframe
   uint32   numTlvs; // the number of TLVs describing the data of the transport block
   apiDlDataTlv_t   TLV[2];   // MAX 2 codeword each PDU
}apiTxDataPdu_t;

/*********** TX.request , message body , 3.3.2.1, Tabale 72 */
typedef struct AC_STRUCTURE_PACK
{
    uint16  SFN_SF;
    uint16  numPDUs;
    apiTxDataPdu_t  PDU[AC_MAX_PDCCH_PER_SUBFRAME * 2];  // variable size depends on numPdus
}ltePhyL2ApiTxReq_t;


/* [1]3.3.3 Uplink Data  */


/* PDU information in UL_SCH indication */
typedef struct AC_STRUCTURE_PACK
{
    uint32  handle;
    uint16  rnti;
    uint16  length; // length of PDU in bytes
    uint16  dataOffs; // PDU#i data address offset from the begining of the 'Number of PDUs' field.
    uint8   ulCqi;
    uint16  timAdv;
}apiRxUlschIndPdu_t;

/** RX_ULSCH Indication, 3.3.3.1, Table 75 */
typedef struct AC_STRUCTURE_PACK
{
    uint16  SFN_SF;
    uint16  numPdus;
    /* Number of PDUs field */
    apiRxUlschIndPdu_t  pdu[AC_MAX_PUSCH_PER_SUBFRAME];  // variable size depends on numPdus
    /* PDU data is followed */
    /* PDU #1 */
    /* PDU #2 */
    /*  ...... */
    /* PDU # numPdus */
}ltePhyL2ApiRxUlschInd_t;

/* HARQ result enumeration */
typedef enum
{
    apiHarq_eACK = 1,
    apiHarq_eNACK,
    apiHarq_eACKorNACK,
    apiHarq_eDTX,
    apiHarq_eACKorDTX,
    apiHarq_eNACKorDTX,
    apiHarq_eACKorNACKorDTX,

    apiHarq_eButt
}apiHarqResults_enum;

/* Special bundling Harq Data enumeration*/
typedef enum
{
    apiHarqResponse_e0orNone = 0,
    apiHarqResponse_e1or4or7ACKs,
    apiHarqResponse_e2or5or8ACKs,
    apiHarqResponse_e3or6or9ACKs,
    apiHarqResponse_DTX,

    apiHarqResponse_eButt
}apiHarqResponse_enum;

/* TDD HARQ Data format for mode = BUNDLING, Table 77 */
typedef struct AC_STRUCTURE_PACK
{
    uint8   value0;  // apiHarqResults_enum
    uint8   value1;  // range 1 - 7 , see above
}apiHarqDataBundling_t;

/* TDD HARQ Data format for mode = MULTIPLEXING, Table 78 */
typedef struct AC_STRUCTURE_PACK
{
    uint8   value0;  // apiHarqResults_enum
    uint8   value1;  // range 1 - 7, see above
    uint8   value2;  // see above
    uint8   value3;  // see above
}apiHarqDataMultiplexing_t;

/* TDD HARQ Data format for mode = SPECIAL BUNDLING, Table 79 */
typedef struct AC_STRUCTURE_PACK
{
    uint8   value0; // apiHarqResponse_enum
}apiHarqDataSpecBundling_t;

/*apiHarqDataR10_t,[2]table 3-96*/
typedef struct AC_STRUCTURE_PACK 
{
    uint8 harqTb[AC_MAX_NUM_HARQ_ACK_BITS];
}apiHarqDataR10_t ;

typedef union AC_STRUCTURE_PACK
{
    apiHarqDataBundling_t   bundling;
    apiHarqDataMultiplexing_t   multiplexing;
    apiHarqDataSpecBundling_t   specBundling; // special bundling
    apiHarqDataR10_t        harqDataR10; 
}apiHarqDataModeUnion_t;

/* Information in HARQ.indication for TDD */
typedef struct AC_STRUCTURE_PACK
{
    uint32  handle;
    uint16  rnti;
    uint8   mode; 
    uint8   numANs; 
    apiHarqDataModeUnion_t harqData;
}apiHarqIndInfoTDD_t;


/* Information in HARQ.indication for FDD ,[2]table 3-103*/
typedef struct AC_STRUCTURE_PACK 
{
    uint32  handle;
    uint16  rnti;
    uint8   mode;
    uint8   numAck;
    uint8   harqTb[AC_MAX_NUM_HARQ_ACK_BITS]; //variable size depends on numAck
}apiHarqIndInfoFDD_t;

typedef union AC_STRUCTURE_PACK
{
    apiHarqIndInfoFDD_t fdd;
    apiHarqIndInfoTDD_t tdd;
}apiHarqIndInfoUnion_t;

/** HARQ Indication, 3.3.3.2, TDD, Table 76; FDD, Table 80 */
typedef struct AC_STRUCTURE_PACK
{
    uint16  SFN_SF;
    uint16  numHarqs;
    /* NOTE: The size of each harq info is based on the mode and frameType,
    all the Harq info in the message are packed without any padding,
    We need calculate the info length base on frameType and mode to get next PDU ,
    we can not use array index to get next info start directly
   */
   apiHarqIndInfoUnion_t info[AC_MAX_NUM_HARQ_PER_SUBFRAME];  // variable size depends on numHarqs
}ltePhyL2ApiHarqInd_t;

/* Information in CRC.indication */
typedef struct AC_STRUCTURE_PACK
{
    uint32  handle;
    uint16  rnti;
    uint8   crcFlag; // 0, CRC_CORRECT; 1, CRC_ERROR
}apiCrcIndInfo_t;

/** CRC Indication, 3.3.3.3, Table 81 */
typedef struct AC_STRUCTURE_PACK
{
    uint16  SFN_SF;
    uint16  numCrcs;
    apiCrcIndInfo_t info[AC_MAX_PUSCH_PER_SUBFRAME];  // variable size depends on numCrcs
}ltePhyL2ApiCrcInd_t;

/* Information in RX_SR indication */
typedef struct AC_STRUCTURE_PACK
{
    uint32  handle;
    uint16  rnti;
}apiRxSrIndInfo_t;

/** RX_SR Indication, 3.3.3.4, Table 82 */
typedef struct AC_STRUCTURE_PACK
{
    uint16  SFN_SF;
    uint16  numSrs;
    apiRxSrIndInfo_t    info[AC_MAX_PUCCH_NUM_SR_PER_SUBFRMAE];  // variable size depends on numSrs
}ltePhyL2ApiRxSrInd_t;

/* PDU information in RX_CQI.indication,[2]table 3-108 */
typedef struct AC_STRUCTURE_PACK 
{
    uint32  handle;
    uint16  rnti;
    uint16  length; 
    uint16  dataOffs; 
    uint8   ulCqi; 
    uint8   numCC;
    uint8   ri[AC_MAX_NUM_CC]; //variable size depends on numCC
    uint16  timAdv;
}apiRxCqiIndPdu_t;

/** RX_CQI Indication, 3.3.3.5, Table 83 */
typedef struct AC_STRUCTURE_PACK
{
    uint16  SFN_SF;
    uint16  numPdus;
    /* Number of PDUs field */
    apiRxCqiIndPdu_t    info[AC_MAX_NUM_CQI_PER_SUBFRAME];  // variable size depends on numPdus
    /* PDU data is followed */
    /* PDU #1 */
    /* PDU #2 */
    /*  ...... */
    /* PDU # numPdus */
}ltePhyL2ApiRxCqiInd_t;

/* RACH Information report */
typedef struct AC_STRUCTURE_PACK
{
    uint16  rnti;
    uint8   preamble;
    uint16  timAdv;
}apiRachIndInfo_t;

/** RACH Indication, 3.3.3.6, Table 84 */
typedef struct AC_STRUCTURE_PACK
{
    uint16  SFN_SF;
    uint8   numPreambles;
    apiRachIndInfo_t  rachInfo[AC_MAX_NUM_PRACH_UE_PER_SUBFRAME];  // variable size depends on numPreambles
}ltePhyL2ApiRachInd_t;

/* Information report by each UE for SRS */
typedef struct AC_STRUCTURE_PACK
{
    uint32  handle;
    uint16  rnti;
    uint16  dopplerEst;
    uint16  timingAdv;
    uint8   numRbs;
    uint8   rbStart;
    uint8   snr[AC_MAX_SRS_RB_PER_UE]; // variable size depends on numRbs
}apiSrsIndUeInfo_t;

/** SRS Indication, 3.3.3.7, Table 85 */
typedef struct AC_STRUCTURE_PACK
{
    uint16  SFN_SF;
    uint8   numUes;
    apiSrsIndUeInfo_t ueReport[AC_MAX_SRS_PER_SUBFRAME];  // variable size depends on numUes
}ltePhyL2ApiSrsInd_t;

/** **/
/* vendor specific filed, to indicate the ULSCH data physical address, 
          the data address array is permutated with the order of ULSCH relevant PDU .
          specially, the ULSCH with no data also occupies the place in data address array.
          For example: 
          the PDU order is:  ULSCH/UCI/ULSCH_CQI_HARQ_RI(no data)/SRS/ULSCH_CQI_RI.
          the permutation in ulschPduDataAddr: ULSCH/ULSCH_CQI_HARQ_RI(no data) / ULSCH_CQI_RI          
          the data address field length in mesage depends on the ULSCH relevent PDU number. the "vendor specific body length" in message
          header indicates the actual length of this field. */
typedef struct AC_STRUCTURE_PACK
{
    uint16         numUEs;
    uint32         ulschPduDataAddr[AC_MAX_PUSCH_PER_SUBFRAME]; 
}apiUlCfgVendor_t;

typedef struct AC_STRUCTURE_PACK
{
    uint32         vendorLength;
    apiUlCfgVendor_t    ulCfgVendor; 
}ltePhyL2ApiUlCfgVendor_t;
/** subframe indication vendor specific fields **/
typedef struct AC_STRUCTURE_PACK
{
    uint32         procLatency;
    uint16         numTasks;
    uint32         taskLatency[32]; 
}apiSfrmIndVendor_t;

typedef struct AC_STRUCTURE_PACK
{
    uint32         vendorLength;
    apiSfrmIndVendor_t    sfrmIndVendor; 
}ltePhyL2ApiSfrmIndVendor_t;


/**start request vendor specific fields**/
typedef struct AC_STRUCTURE_PACK
{
    uint32         vendorLength;
    uint16         subframe; 
}ltePhyL2ApiStartReqVendor_t;

/** some vendor specific fields in the indication messages **/
/****UL SCH indication vendor specific structure*****/
typedef struct AC_STRUCTURE_PACK
{
    uint16         rnti;
    uint8          rssi;  // the step and the range
    uint16         freqOffset;  // 32767 represents 0Hz, with step 0.1Hz
    uint8          AoA;     // We must report AoA on SRS, and we'd better report AoA on PUSCH.
    uint8          mvSpeed;    // the step and the range
    uint8          ni;         // Average noise plus Interference
	uint8          minNi;      // minimum noise plus interference of each window
	uint8          maxNi;      // maximum noise plus interference of each window
    uint8          rsv[13];    // reserved field
}apiRxUlschDetMetrics_t;

typedef struct AC_STRUCTURE_PACK
{
    uint16         numUEs;
    uint32         procLatency;
    apiRxUlschDetMetrics_t    rxUlschDetMetrics[AC_MAX_PUSCH_PER_SUBFRAME]; 
}apiRxUlschIndVendor_t;

typedef struct AC_STRUCTURE_PACK
{
    uint32         vendorLength;
    apiRxUlschIndVendor_t    rxUlschIndVendor; 
}ltePhyL2ApiRxUlschIndVendor_t;

/****HARQ indication vendor specific structure*****/
typedef struct AC_STRUCTURE_PACK
{
    uint16         rnti;
    uint8          rssi;  // the step and the range
    uint8          ulCqi;  // Value: 0-255, representing -64dB to 63.5dB,with 0.5dB step size.
    uint16         freqOffset;  // 32767 represents 0Hz, with step 0.1Hz
    uint8          dtxThr;     // the step and the range, if format 3
    uint8          nonDTXVal;    // the step and the range
    uint16         timAdv;   //  Value:0 ~ 2047 (measured in steps of 2Ts)  2048 ~ 7690 (measured in steps of 8Ts)
    uint8          ni;         // Average noise plus Interference
	uint8          harqInfoBits;  // The decoded HARQ information bits(b0 is in LSB). Valid when HARQ-ACK mode is multiplexing, special bundling, and format 1b with channel selection
    uint8          pucchResIndex;     // PUCCH resource index in which HARQ is decoded. Only valid in PUCCH when HARQ-ACK mode is multiplexing and format 1b with channel selection
	uint8          minNi;      // minimum noise plus interference of each window, only valid when HARQ is multiplexed on PUSCH
	uint8          maxNi;      // maximum noise plus interference of each window, only valid when HARQ is multiplexed on PUSCH
    uint8          rsv[11];    // reserved field
}apiRxHarqDetMetrics_t;

typedef struct AC_STRUCTURE_PACK
{
    uint16         numUEs;
    uint32         procLatency;
    apiRxHarqDetMetrics_t    rxHarqDetMetrics[AC_MAX_PUCCH_PER_SUBFRAME + AC_MAX_PUSCH_PER_SUBFRAME]; 
}apiRxHarqIndVendor_t;

typedef struct AC_STRUCTURE_PACK
{
    uint32         vendorLength;
    apiRxHarqIndVendor_t    rxHarqIndVendor; 
}ltePhyL2ApiRxHarqIndVendor_t;


/****SR indication vendor specific structure*****/
typedef struct AC_STRUCTURE_PACK
{
    uint16         rnti;
    uint8          rssi;  // the step and the range
    uint8          ulCqi;  // Value: 0-255, representing -64dB to 63.5dB,with 0.5dB step size.
    uint16         freqOffset;  // 32767 represents 0Hz, with step 0.1Hz
    uint8          srVal;
    uint8          srThr;    //   the step and the range
    uint16         timAdv; // Value:0 ~ 2047 (measured in steps of 2Ts)  2048 ~ 7690 (measured in steps of 8Ts)
    uint8          ni;    // Noise plus Interference
    uint8          rsv[15];  // reserved field
}apiRxSrDetMetrics_t;

#if(ON == VT_ICIC_MES)
/****MES report vendor specific structure*****/
typedef struct AC_STRUCTURE_PACK
{
    uint8          measurementBitMask; //Indicates Measurements Reported by L1. Bit 0 of Byte 0 indicates uplinkInterferencePower[], 
    								   // bit 1 representsThermal Noise present, etc and so on
    uint8          uplinkInterferencePower[AC_MAX_NUM_RB];  // Reported Uplink Interference per PRB as per 36.214
    uint32         thermalNoise;  // Thermal noise measurement from L1 as per 36.214
    uint32         dlRsTxPower;  //Downlink Reference Signal Transmit Power from L1 as per 36.214
#if(ON == VT_AVG_RTWP_RX)
    uint32         avgRtwpRX; 	 // Thermal Noise + Noise Figure + Interference.  
#endif
}apiMesRptMetrics_t;

typedef struct AC_STRUCTURE_PACK
{
    uint16  SFN_SF;
    uint16  numUePerRB;
    apiMesRptMetrics_t info;  // variable size depends on number of UE MES
}ltePhyL2ApiIcicMes_t;
#endif

typedef struct AC_STRUCTURE_PACK
{
    uint16         numUEs;
    uint32         procLatency;
    apiRxSrDetMetrics_t    rxSrDetMetrics[AC_MAX_PUCCH_NUM_SR_PER_SUBFRMAE];
}apiRxSrIndVendor_t;

typedef struct AC_STRUCTURE_PACK
{
    uint32         vendorLength;
    apiRxSrIndVendor_t    rxSrIndVendor; 
}ltePhyL2ApiRxSrIndVendor_t;


/****CQI indication vendor specific structure*****/
typedef struct AC_STRUCTURE_PACK
{
    uint16         rnti;
    uint8          rssi;  // the step and the range
    uint16         freqOffset;  // 32767 represents 0Hz, with step 0.1Hz
    uint8          ni;         // Average noise plus Interference
	uint8          minNi;      // minimum noise plus interference of each window, only valid when CQI is multiplexed on PUSCH
	uint8          maxNi;      // maximum noise plus interference of each window, only valid when CQI is multiplexed on PUSCH
    uint8          rsv[13];    // reserved field
}apiRxCqiDetMetrics_t;

typedef struct AC_STRUCTURE_PACK
{
    uint16         numUEs;
    uint32         procLatency;
    apiRxCqiDetMetrics_t    rxCqiDetMetrics[AC_MAX_PUCCH_PER_SUBFRAME + AC_MAX_PUSCH_PER_SUBFRAME]; 
}apiRxCqiIndVendor_t;

typedef struct AC_STRUCTURE_PACK
{
    uint32         vendorLength;
    apiRxCqiIndVendor_t    rxCqiIndVendor; 
}ltePhyL2ApiRxCqiIndVendor_t;


/****RACH indication vendor specific structure*****/
typedef struct AC_STRUCTURE_PACK
{
    uint16         rnti;
	uint8          preamble;
    uint8          peakPow;   // value of peak which resulted in detection,the step and the range
    uint8          rachThr;   // the step and the range
    uint8          rsv[16];  // reserved field
}apiRachDetMetrics_t;

typedef struct AC_STRUCTURE_PACK
{
    uint16         numUEs;
    uint32         procLatency;
    apiRachDetMetrics_t    rachDetMetrics[AC_MAX_NUM_PRACH_UE_PER_SUBFRAME]; 
}apiRachIndVendor_t;

typedef struct AC_STRUCTURE_PACK
{
    uint32         vendorLength;
    apiRachIndVendor_t    rachIndVendor; 
}ltePhyL2ApiRachIndVendor_t;


/****SRS indication vendor specific structure*****/
typedef struct AC_STRUCTURE_PACK
{
    uint16         rnti;
    uint8          rssi;  // the step and the range
    uint8          AoA;   // the step and the range
    uint8          mvSpeed;    // the step and the range
    uint8          ni;         // Average noise plus Interference
	uint8          minNi;      // minimum noise plus interference of each window
	uint8          maxNi;      // maximum noise plus interference of each window
    uint8          rsv[13];    // reserved field
}apiRxSrsDetMetrics_t;

typedef struct AC_STRUCTURE_PACK
{
    uint16         numUEs;
    uint32         procLatency;
    apiRxSrsDetMetrics_t    rxSrsDetMetrics[AC_MAX_SRS_PER_SUBFRAME]; 
}apiRxSrsIndVendor_t;

typedef struct AC_STRUCTURE_PACK
{
    uint32         vendorLength;
    apiRxSrsIndVendor_t    rxSrsIndVendor; 
}ltePhyL2ApiRxSrsIndVendor_t;

/** CUSTOM Define Indication */
typedef struct AC_STRUCTURE_PACK
{
    uint8    numUes;
    uint8    exponent;
    uint8    info[AC_MAX_PUSCH_CONSTELLATION_SIZE_PER_UE * AC_MAX_PUSCH_PER_SUBFRAME];
}apiCusDefIndPuschCons_t;

typedef struct AC_STRUCTURE_PACK
{
    uint16  SFN_SF;
    uint8  subMsgId; // apiCustomIndMsgId_enum
    /* following is detail msg information */
    union
    {
            apiCusDefIndPuschCons_t  puschCons;
            // reserved for other messages
    }subMsgInfo;
}ltePhyL2ApiCustomDefInd_t;


typedef struct AC_STRUCTURE_PACK
{
    uint8  maxRadiationPowerMode; // The value '0b0','0b1' means 'reset', 'set' respectively.
    uint8  antMask; // tx/rx antenna mask, 0b0: disable, 0b1: enable
}ltePhyL2ApiDlConfigVndrSpecMsg_t;


/*CUSTOM Define to indicate the number of error indication int this TTI*/
typedef struct AC_STRUCTURE_PACK
{
    uint8  tag;   //should be apiCfgTlvTag_eErrIndNum
    uint8  length;
    uint16 errIndNum;
} apiErrIndNumTLV_t;


/** MACRO for calculating L2 API message address and vendor length **/
/* Macro for getting message body address */
#define AC_L2API_GET_MSG_BODY_ADDR(msgHdrP)       ((uint8*)msgHdrP + sizeof(ltePhyL2ApiMsgHdr_t))

/* Macro for getting vendor specific length address */
#define AC_L2API_GET_VDR_LEN_ADDR(msgHdrP)        ((uint8*)AC_L2API_GET_MSG_BODY_ADDR(msgHdrP) + ((ltePhyL2ApiMsgHdr_t*)msgHdrP)->msgBodyLength)

/* Macro for getting vendor specific length */
#define AC_L2API_GET_VDR_LEN(msgHdrP)             ((1 == ((ltePhyL2ApiMsgHdr_t*)msgHdrP)->msgVdrSpecFlag) ? *((uint32*)AC_L2API_GET_VDR_LEN_ADDR(msgHdrP)) : 0) 

/* size of the message header */
#define AC_L2API_MSG_HDR_SIZE                     (sizeof(ltePhyL2ApiMsgHdr_t))

/* size of the vendor specific length */
#define AC_L2API_VDR_LEN_SIZE                     (sizeof(uint32)) 

/* Macro for getting whole message size */
#define AC_L2API_GET_WHOLE_MSG_SIZE(msgHdrP)      (AC_L2API_MSG_HDR_SIZE + ((ltePhyL2ApiMsgHdr_t*)msgHdrP)->msgBodyLength + ((1 == ((ltePhyL2ApiMsgHdr_t*)msgHdrP)->msgVdrSpecFlag) ? (AC_L2API_GET_VDR_LEN(msgHdrP) + AC_L2API_VDR_LEN_SIZE) : 0))

#ifdef _WIN32
   #pragma pack()
#endif

#endif // AC_LTEPHYL2API_H
