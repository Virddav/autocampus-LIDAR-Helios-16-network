/**
 * @addtogroup mod_raw_its ITS MSG Tx/Rx Module
 * @{
 *
 * ITS MSG sender & receiver thread
 *
 * @file
 *
 */

//------------------------------------------------------------------------------
// Copyright (c) 2012 Cohda Wireless Pty Ltd
//-----------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Included headers
//------------------------------------------------------------------------------
#include "raw-its.h"
#include "raw-asn.h"
#include "raw-asn_def.h"

#include "ExampleETSI_debug.h"

// v2x-lib includes
#include "id-global.h"
#include "util.h"
#include "ext.h"
#include "PacketBuffer.h"
#include "etsi-msg-if.h"
#include "etsi-btp.h"
#include "etsi-g5.h"
#include "debug-levels.h"

#include "libconfig.h" // used to read configuration file

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>

//------------------------------------------------------------------------------
// Local Macros & Constants
//------------------------------------------------------------------------------

#define D_LEVEL ExampleETSI_ExampleETSI_Raw_DebugLevel

//------------------------------------------------------------------------------
// AOC definitions
//------------------------------------------------------------------------------
#define AOC_MAX_RAW_DATA_SIZE 18720

//------------------------------------------------------------------------------
// Local Type definitions
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Local (static) Function Prototypes
//------------------------------------------------------------------------------

static int RAWIts_LoadConfig(struct RAWIts *pRAW,
                             char *pConfigFileName);

static void RAWIts_ThreadProc(void *pArg);

static void RAWIts_ExtCallback(tExtEventId Event,
                               tExtMessage *pMsg,
                               void *pPriv);

static int RAWIts_ReqSend(tETSIMSGHandleCode Handle,
                          tRAWItsParams *pParams);

//------------------------------------------------------------------------------
// Local Variables
//------------------------------------------------------------------------------

/// Local pointer to config
struct RAWIts *pRAWStat;

//------------------------------------------------------------------------------
// API Functions
//------------------------------------------------------------------------------

/**
 * @brief Allocate RAWIts object, Setup thread and associated data structures
 * @param pRAW pointer to RAW handle (populated by this function)
 * @param pAttr POSIX thread attributes
 * @param pConfigFileName Configuration filename
 * @return Zero for success or an negative errno
 */
int RAWIts_Open(struct RAWIts *pRAW,
                pthread_attr_t *pAttr,
                char *pConfigFileName)
{
    int Res;
    d_fnstart(D_INTERN, NULL, "(%p,%p,%s)\n",
              pRAW, pAttr, pConfigFileName);

    memset(pRAW, 0, sizeof(struct RAWIts));

    // Load config settings
    Res = RAWIts_LoadConfig(pRAW, pConfigFileName);
    if (Res != 0)
    {
        d_printf(D_INFO, NULL, "RAWIts_LoadConfig(%p,%s) != 0\n", pRAW, pConfigFileName);
        goto Error;
    }

    pRAWStat = pRAW;

    // Init state

    // Assign thread attributes from input param
    pRAW->ThreadAttr = *pAttr;

    // Create RAWIts thread
    pRAW->ThreadState |= RAWITS_THREAD_STATE_INIT;
    Res = pthread_create(&pRAW->ThreadID,
                         &pRAW->ThreadAttr,
                         (void *)RAWIts_ThreadProc,
                         pRAW);
    if (Res != 0)
    {
        d_printf(D_INFO, NULL, "pthread_create() failed\n");
        // Thread creation error
        pRAW->ThreadState = RAWITS_THREAD_STATE_NONE;
        goto Error;
    }

    // Success!
    Res = 0;
    d_printf(D_INFO, NULL, "Successfully started raw-its\n");
    goto Success;

Error:
    d_error(D_ERR, 0, "Error!\n");
    RAWIts_Close(pRAW);
Success:
    d_fnend(D_INFO, NULL, "()\n");
    return Res;
}

/**
 * @brief Stop execution of RAW, free the thread and its associated resources
 * @param pRAW RAW handle
 */
void RAWIts_Close(struct RAWIts *pRAW)
{
    int Res = -ENOSYS;
    d_fnstart(D_INFO, NULL, "()\n");

    // Catch attempt to close invalid object pointer
    if (pRAW == NULL)
    {
        Res = -EINVAL;
        goto Error;
    }

    // Signal thread to terminate
    pRAW->ThreadState |= RAWITS_THREAD_STATE_STOP;
    // Wait for thread termination
    if (pRAW->ThreadState & RAWITS_THREAD_STATE_INIT)
        pthread_join(pRAW->ThreadID, NULL);

    // Success!
    Res = 0;

Error:
    (void)Res; // value not currently used
    d_printf(D_DEBUG, 0, "[AOC] RAWITS thread close! (%d)\n", Res);
    d_fnend(D_INFO, NULL, "()\n");
    return;
}

/**
 * @brief Prints the statistics
 * @param pRAW RAW handle
 */
void RAWIts_PrintStats(struct RAWIts *pRAW)
{
    fprintf(stdout, "RAWIts: Rx (Okay %8u Fail %8u)\n",
            pRAW->Stats.Rx.Okay, pRAW->Stats.Rx.Fail);
}

//------------------------------------------------------------------------------
// Local Functions
//------------------------------------------------------------------------------

/**
 * @brief Load RAWITS parameters from the config file
 * @param pRAW RAW receiver handle
 * @param pConfigFileName Filename of the config file
 * @return Zero for success or a negative errno
 */
static int RAWIts_LoadConfig(struct RAWIts *pRAW,
                             char *pConfigFileName)
{
    int Res = -ENOSYS;
    config_t Config; // configuration object read from config file
    config_t *pConfig;
    config_setting_t *pSetting; // setting for RAWITS in config file
    int ConfigVal = 0;
    long long ConfigVal64 = 0;

    pConfig = &Config;
    config_init(pConfig); // initialise the config object

    // Set defaults
    pRAW->Params.BTPPort = RAWITS_CONFIG_VALUE_DEFAULT_BTPPORT;
    pRAW->Params.TxInterval = RAWITS_CONFIG_VALUE_DEFAULT_TXINTERVAL;
    pRAW->Params.RxInterval = RAWITS_CONFIG_VALUE_DEFAULT_RXINTERVAL;
    pRAW->Params.DestInfo = RAWITS_CONFIG_VALUE_DEFAULT_DESTINFO;
    pRAW->Params.PktTransport = RAWITS_CONFIG_VALUE_DEFAULT_PKTTRANSPORT;
    pRAW->Params.Location = RAWITS_CONFIG_VALUE_DEFAULT_LOCATION;
    pRAW->Params.AreaLat = RAWITS_CONFIG_VALUE_DEFAULT_AREALAT;
    pRAW->Params.AreaLong = RAWITS_CONFIG_VALUE_DEFAULT_AREALONG;
    pRAW->Params.AreaDistA = RAWITS_CONFIG_VALUE_DEFAULT_AREADISTA;
    pRAW->Params.AreaDistB = RAWITS_CONFIG_VALUE_DEFAULT_AREADISTB;
    pRAW->Params.AreaAngle = RAWITS_CONFIG_VALUE_DEFAULT_AREAANGLE;
    pRAW->Params.AreaShape = RAWITS_CONFIG_VALUE_DEFAULT_AREASHAPE;
    pRAW->Params.CommProfile = RAWITS_CONFIG_VALUE_DEFAULT_COMMPROFILE;
    pRAW->Params.TrafficClass = RAWITS_CONFIG_VALUE_DEFAULT_TRAFFICCLASS;
    pRAW->Params.MaxPktLifetime = RAWITS_CONFIG_VALUE_DEFAULT_MAXPKTLIFETIME;
    pRAW->Params.SecProfile = RAWITS_CONFIG_VALUE_DEFAULT_SECPROFILE;
    pRAW->Params.DataLength = RAWITS_CONFIG_VALUE_DEFAULT_DATALENGTH;
    memcpy(pRAW->Params.Data,
           RAWITS_CONFIG_VALUE_DEFAULT_DATA,
           RAWITS_CONFIG_VALUE_DEFAULT_DATALENGTH);

    // Try to read the specified config file into the config object
    if (config_read_file(pConfig, pConfigFileName) != CONFIG_TRUE)
    {
        d_error(D_ERR, 0, "Could not open %s\n", pConfigFileName);
        // "libconfig: %s at line %d\n", config_error_text (pConfig),
        Res = -EINVAL;
        goto Error;
    }

    // Get the RAWIts Setting
    pSetting = config_lookup(pConfig, RAWITS_CONFIG_PATH_NAME);
    if (pSetting == NULL)
    {
        d_error(D_ERR, 0, "config_lookup(%s) failed\n", RAWITS_CONFIG_PATH_NAME);
        Res = -EINVAL;
        goto Error;
    }

    // config is open, now look for configuration entries

    // BTP Port
    if (config_setting_lookup_int(pSetting,
                                  RAWITS_CONFIG_VALUE_NAME_BTPPORT,
                                  &ConfigVal))
        pRAW->Params.BTPPort = (uint16_t)ConfigVal;

    // Rx thread Interval
    if (config_setting_lookup_int(pSetting,
                                  RAWITS_CONFIG_VALUE_NAME_RXINTERVAL,
                                  &ConfigVal))
        pRAW->Params.RxInterval = (uint32_t)ConfigVal;

    // TxInterval
    if (config_setting_lookup_int(pSetting,
                                  RAWITS_CONFIG_VALUE_NAME_TXINTERVAL,
                                  &ConfigVal))
        pRAW->Params.TxInterval = (uint32_t)ConfigVal;

    // Packet
    config_setting_t *pPacket;
    pPacket = config_setting_get_member(pSetting,
                                        RAWITS_CONFIG_VALUE_NAME_PACKET);

    if (pPacket != NULL)
    {
        // DestInfo
        if (config_setting_lookup_int(pPacket,
                                      RAWITS_CONFIG_VALUE_NAME_DESTINFO,
                                      &ConfigVal))
            pRAW->Params.DestInfo = (uint16_t)ConfigVal;

        // PktTransport
        if (config_setting_lookup_int(pPacket,
                                      RAWITS_CONFIG_VALUE_NAME_PKTTRANSPORT,
                                      &ConfigVal))
            pRAW->Params.PktTransport = (uint8_t)ConfigVal;

        // CommProfile
        if (config_setting_lookup_int(pPacket,
                                      RAWITS_CONFIG_VALUE_NAME_COMMPROFILE,
                                      &ConfigVal))
            pRAW->Params.CommProfile = (uint8_t)ConfigVal;

        // TrafficClass
        if (config_setting_lookup_int(pPacket,
                                      RAWITS_CONFIG_VALUE_NAME_TRAFFICCLASS,
                                      &ConfigVal))
            pRAW->Params.TrafficClass = (uint8_t)ConfigVal;

        // MaxPktLifetime
        if (config_setting_lookup_int(pPacket,
                                      RAWITS_CONFIG_VALUE_NAME_MAXPKTLIFETIME,
                                      &ConfigVal))
            pRAW->Params.MaxPktLifetime = ConfigVal;

        // Security Profile
        if (config_setting_lookup_int(pPacket,
                                      RAWITS_CONFIG_VALUE_NAME_SECPROFILE,
                                      &ConfigVal))
            pRAW->Params.SecProfile = (uint8_t)ConfigVal;

        // Location
        if (config_setting_lookup_int64(pPacket,
                                        RAWITS_CONFIG_VALUE_NAME_LOCATION,
                                        &ConfigVal64))
            pRAW->Params.Location = ConfigVal64;

        // Area
        if (config_setting_lookup_int(pPacket,
                                      RAWITS_CONFIG_VALUE_NAME_AREALAT,
                                      &ConfigVal))
            pRAW->Params.AreaLat = ConfigVal;
        if (config_setting_lookup_int(pPacket,
                                      RAWITS_CONFIG_VALUE_NAME_AREALONG,
                                      &ConfigVal))
            pRAW->Params.AreaLong = ConfigVal;
        if (config_setting_lookup_int(pPacket,
                                      RAWITS_CONFIG_VALUE_NAME_AREADISTA,
                                      &ConfigVal))
            pRAW->Params.AreaDistA = ConfigVal;
        if (config_setting_lookup_int(pPacket,
                                      RAWITS_CONFIG_VALUE_NAME_AREADISTB,
                                      &ConfigVal))
            pRAW->Params.AreaDistB = ConfigVal;
        if (config_setting_lookup_int(pPacket,
                                      RAWITS_CONFIG_VALUE_NAME_AREAANGLE,
                                      &ConfigVal))
            pRAW->Params.AreaAngle = ConfigVal;
        if (config_setting_lookup_int(pPacket,
                                      RAWITS_CONFIG_VALUE_NAME_AREASHAPE,
                                      &ConfigVal))
            pRAW->Params.AreaShape = ConfigVal;

        // Data
        config_setting_t *pData;
        pData = config_setting_get_member(pPacket, RAWITS_CONFIG_VALUE_NAME_DATA);
        if (pData != NULL)
        {
            int i;
            int Cnt = config_setting_length(pData);

            if (Cnt > RAWITS_DATA_BUF_SIZE)
                Cnt = RAWITS_DATA_BUF_SIZE;

            for (i = 0; i < Cnt; i++)
            {
                ConfigVal = config_setting_get_int_elem(pData, i);
                pRAW->Params.Data[i] = (uint8_t)ConfigVal;
            }
            // Data Length
            pRAW->Params.DataLength = Cnt;
        }
    }

    // Update complete - close the configuration
    config_destroy(pConfig);

    // Success!
    Res = 0;

Error:
    return Res;
}

/**
 * @brief RAW Periodic thread processing - sends a RAW
 * @param pArg Pointer to RAWIts object passed as generic input parameter
 *
 */
static void RAWIts_ThreadProc(void *pArg)
{
    struct RAWIts *pRAW;
    struct timespec Time;
    int RAWHandle;
    int ExtHandle;

    d_printf(D_INFO, NULL, "Started thread\n");

    pRAW = (struct RAWIts *)pArg;

    // Initialise the timespec struct for the polling loop
    clock_gettime(CLOCK_MONOTONIC, &Time);

    // "Starting RAW Periodic Thread");
    pRAW->ThreadState |= RAWITS_THREAD_STATE_RUN;

    // Register the callback with the Ext Module
    ExtHandle = Ext_CallbackRegister(RAWIts_ExtCallback, pRAW);

    if (ExtHandle < 0)
    {
        d_error(D_ERR, 0, "Unable to register callback with EXT module\n");
        goto Error;
    }

    // Open BTP Port
    // No handler provided, use EXT interface to get MSG Indications
    RAWHandle = ETSIMSG_OpenInterface(pRAW->Params.BTPPort, NULL);

    if (RAWHandle < ETSIMSG_SUCCESS)
    {
        d_error(D_ERR, 0, "Unable to open BTP port %d\n", pRAW->Params.BTPPort);
        goto Error;
    }

    // AOC: Hello, World!
    int hello_count = 0;
    char hello_data[AOC_MAX_RAW_DATA_SIZE];
    size_t hello_length = 0;
    // Thread loop
    while ((pRAW->ThreadState & RAWITS_THREAD_STATE_STOP) == 0)
    {
        // AOC: Hello, World!
        snprintf(hello_data, AOC_MAX_RAW_DATA_SIZE, "Hello, World! (%d)", hello_count);
        hello_count++;

        hello_length = strlen(hello_data) + 1;
        if (hello_length <= AOC_MAX_RAW_DATA_SIZE)
        {
            memcpy(pRAW->Params.Data, hello_data, hello_length);
            pRAW->Params.DataLength = hello_length;
            // --------------------------------
            // sleeping delay
            Util_Nap(pRAW->Params.TxInterval, &Time);

            // Send something
            RAWIts_ReqSend(RAWHandle, &pRAW->Params);
        }
        else
        {
            d_error(D_ERR, 0, "[AOC] Hello data too long. (%zu)\n", hello_length);
            break;
        }
    }


    // Close BTP Port
    ETSIMSG_CloseInterface(RAWHandle);

    // Deregister the Ext Callback
    Ext_CallbackDeregister(ExtHandle);

Error:
    // exit thread

    d_printf(D_DEBUG, 0, "[AOC] RAWITS thread proc stopped after %d messages\n", hello_count);
    (void)pthread_exit(NULL);
}

/**
 * @brief RAW Packet Sending with UPER-encoded data
 * @param Handle Handle from ETSI MSG API
 * @param pParams Pointer to parameters of control and data
 *
 */
static int RAWIts_ReqSend(tETSIMSGHandleCode Handle,
                          tRAWItsParams *pParams)
{
    int Ret = -1;
    tETSIMSGStatusCode Stat;

    // [AOC] ------------------------------------------------
    static size_t success_count = 0;
    static size_t total_count = 0;
    total_count++;
    // ------------------------------------------------------
    (void)Handle;

    // Fill BTP header
    tETSIMSGReqMgmt MSGReqHdr;

    memset(&MSGReqHdr, 0, sizeof(MSGReqHdr));

    MSGReqHdr.ID = ETSIMSG_ACTIONID(0x1234);
    MSGReqHdr.RepetitionDuration = ETSIFAC_REPETITIONDURATIONNONE;
    MSGReqHdr.RepetitionInterval = ETSIFAC_REPETITIONINTERVALNONE;

    MSGReqHdr.DestPort = pParams->BTPPort;
    MSGReqHdr.DestInfo = pParams->DestInfo;
    MSGReqHdr.BTPType = ETSIFAC_BTP_TYPE_B;

    MSGReqHdr.PktTransport = pParams->PktTransport;
    MSGReqHdr.CommProfile = pParams->CommProfile;
    MSGReqHdr.TrafficClass = pParams->TrafficClass;
    MSGReqHdr.HopLimit = ETSIFAC_GN_HOP_LIMIT_DEFAULT;
    MSGReqHdr.MaxPktLifetime = pParams->MaxPktLifetime;

    // Destination Information, possibly not needed for some PktTransport types
    // but filled anyway to show example
    // For GUC, address in 64-bit host format
    MSGReqHdr.Address.GN_ADDR_as_64 = ntohll(pParams->Location);
    // For GBC/GAC
    MSGReqHdr.Area.Latitude = pParams->AreaLat;
    MSGReqHdr.Area.Longitude = pParams->AreaLong;
    MSGReqHdr.Area.Distance_a = pParams->AreaDistA;
    MSGReqHdr.Area.Distance_b = pParams->AreaDistB;
    MSGReqHdr.Area.Angle = pParams->AreaAngle;
    MSGReqHdr.Area.Shape = pParams->AreaShape;

    // Secure or Unsecure packet
    if (pParams->SecProfile == 0)
    {
        MSGReqHdr.SecControl = ETSIFAC_GN_SEC_PROF_NONE;
    }
    else if (pParams->SecProfile == 1)
    {
        // Custom security
        MSGReqHdr.SecControl = ETSIFAC_GN_SEC_PROF_AID_SSP;

        // CAM ITS-AID
        MSGReqHdr.SecInfo.AID = 0x24;
        // Replicate CAM security SSP
        MSGReqHdr.SecInfo.SSPBitmap = true;
        MSGReqHdr.SecInfo.SSPLen = 3;
        MSGReqHdr.SecInfo.SSPBits[0] = 0x01;
        MSGReqHdr.SecInfo.SSPBits[1] = 0x00;
        MSGReqHdr.SecInfo.SSPBits[2] = 0x00;
        MSGReqHdr.SecInfo.SSPMask[0] = 0xFF;
        MSGReqHdr.SecInfo.SSPMask[1] = 0x00;
        MSGReqHdr.SecInfo.SSPMask[2] = 0x00;
    }
    else if (pParams->SecProfile == 2)
    {
        // Fixed CAM security profile
        MSGReqHdr.SecControl = ETSIFAC_GN_SEC_PROF_CAM;
    }
    else
    {
        // Fixed General security profile
        MSGReqHdr.SecControl = ETSIFAC_GN_SEC_PROF_GENERAL;
    }

    // Create and populate the BTP payload
    // Allocate structure to populate that will be encoded
    RAW_DUMMYPDU *pDummyPDU = (RAW_DUMMYPDU *)asn1_mallocz_value(asn1_type_RAW_DUMMYPDU);

    if (pDummyPDU == NULL)
    {
        d_error(D_ERR, 0, "pDummyPDU allocation failed\n");
        goto Exit;
    }

    // set the DUMMY PDU header
    pDummyPDU->header.protocolVersion = RAW_ItsPduHeaderProtocolVersion_ID_currentVersion;
    pDummyPDU->header.messageID = RAW_ItsPduHeaderMessageID_ID_dummy;
    // [AOC] ------------------------------------------------
    pDummyPDU->header.stationID = CAM_GetCurrentInfo().stationID;
    // ------------------------------------------------------

    // set the DUMMY Message content
    static uint16_t Seq = 0;
    pDummyPDU->dummy.sequenceNumber = Seq++;

    // Allocate memory for octet string
    pDummyPDU->dummy.someData.buf = asn1_mallocz(sizeof(uint8_t) * pParams->DataLength);

    if (pDummyPDU->dummy.someData.buf == NULL)
    {
        d_error(D_ERR, 0, "pDummyPDU Data allocation failed\n");
        goto Exit;
    }

    // Copy the User data to the appropriate Message field
    memcpy(pDummyPDU->dummy.someData.buf, pParams->Data, pParams->DataLength);
    pDummyPDU->dummy.someData.len = pParams->DataLength;

    // Use the GSER encoder to show the message in the debug output
    uint8_t *pGSerBuf;
    unsigned int GSerLen;
    ASN1GSERParams Params = {
        .filter_opaque = NULL,
        .filter_func = NULL,
        .indent = 0,
        .hex_bit_string_disabled = true};

    // GSER encode function (uses asn1_type_RAW_DUMMYPDU information to
    // GSER-encode pDummyPDU data to new buffer at pGSerBuf
    GSerLen = asn1_gser_encode2(&pGSerBuf, asn1_type_RAW_DUMMYPDU, pDummyPDU, &Params);

    if (GSerLen <= 0)
    {
        d_printf(D_WARN, NULL, "GSER encode failed: %d\n", GSerLen);
        goto Exit;
    }

    d_printf(D_INFO, NULL, "GSER[%d]:\n%s", GSerLen, pGSerBuf);
    free(pGSerBuf);

    // Use the UPER encoder to create (encoded) message payload
    int Len;
    uint8_t *pUperBuf;
    ASN1Error Err;

    // UPER encode function (uses asn1_type_RAW_DUMMYPDU information to
    // UPER-encode pDummyPDU data to new buffer at pUperBuf
    Len = asn1_uper_encode2(&pUperBuf, asn1_type_RAW_DUMMYPDU, pDummyPDU, &Err);

    if (Len <= 0)
    {
        d_printf(D_WARN, NULL, "UPER encode failed: %d (bit %d, %s)\n", Len, Err.bit_pos, Err.msg);
        goto Exit;
    }

    // Encoding finished successfully
    d_printf(D_INFO, NULL, "UPER encoded %d bytes\n", Len);
    d_dump(D_INFO, 0, pUperBuf, Len);

    // And send via MSG API
    Stat = ETSIMSG_SendPacket(&MSGReqHdr, pUperBuf, Len);

    if (Stat == ETSIMSG_SUCCESS)
    {
        success_count++;
        d_printf(D_DEBUG, 0, "[AOC] RAW message successfully transmited(%zu/%zu) (Seq = %hu).\n", success_count, total_count, Seq);
        Ret = 0;
    }
    else
    {
        d_printf(D_DEBUG, 0, "[AOC] RAW message failed to transmit (%zu/%zu).\n", success_count, total_count);
    }

    // Free allocated structure (all dynamically allocated elements will be freed)
    asn1_free_value(asn1_type_RAW_DUMMYPDU, pDummyPDU);
    // Free UPER data buffer
    free(pUperBuf);
Exit:
    return Ret;
}

/**
 * @brief EXT Callback
 * @param Event Event type of EXT event
 * @param pMsg The EXT message structure
 * @param pPriv Private pointer provided in registration
 *
 */
static void RAWIts_ExtCallback(tExtEventId Event,
                               tExtMessage *pMsg,
                               void *pPriv)
{
    (void)pPriv;
    // [AOC] ------------------------------------------------
    static size_t success_count = 0;
    // ------------------------------------------------------

    // Check Event type is a 'message type' event
    if (Event == QSMSG_EXT_RX_ITSFL_MSG)
    {
        d_printf(D_INFO, NULL, "QSMSG_EXT_RX_ITSFL_MSG\n");
        d_assert(pMsg != NULL);
        d_assert(pMsg->pPkt != NULL);

        struct RAWIts *pRAW = (struct RAWIts *)pPriv;
        tUtilPacketBuffer *pRxPkt = pMsg->pPkt;

        // DestPort from BTP descriptor
        const tETSIGeoNetDesc *pBTP = pRxPkt->Elem.pETSIGeoNetDesc;
        d_printf(D_INFO, NULL, "DestPort=%d\n", pBTP->DestPort);

        if (pBTP->DestPort == pRAW->Params.BTPPort)
        {
            // Access the payload of the PacketBuffer
            uint8_t *pPayload = pRxPkt->Elem.Payload.pData;
            uint16_t Length = pRxPkt->Elem.Payload.Len;

            // Dump the packet payload
            d_printf(D_INFO, NULL, "Payload[%d]...\n", Length);
            d_dump(D_INFO, 0, pPayload, Length);

            if (pPayload && (Length > 0))
            {
                pRAWStat->Stats.Rx.Okay++;
            }

            // Decode (from UPER), decoded buffer will be created
            ASN1Error Err;
            RAW_DUMMYPDU *pRawRx;

            int Bytes = asn1_uper_decode((void **)&pRawRx, asn1_type_RAW_DUMMYPDU, (const uint8_t *)pPayload, Length, &Err);

            if (Bytes <= 0)
            {
                d_printf(D_WARN, NULL, "UPER decode failed: %d (bit %d, %s)\n", Bytes, Err.bit_pos, Err.msg);
                d_printf(D_DEBUG, NULL, "[AOC] UPER decode failed: %d (bit %d, %s)\n", Bytes, Err.bit_pos, Err.msg);
            }
            else
            {
                success_count++;
                // Access some parts of the structure
                d_printf(D_INFO, NULL, "StationID=0x%04x, Seq %d\n",
                         (int)pRawRx->header.stationID,
                         (int)pRawRx->dummy.sequenceNumber);
                d_printf(D_DEBUG, NULL, "[AOC] Raw message from StationID=0x%04x, Seq %d, MSG: %s\n",
                         (int)pRawRx->header.stationID,
                         (int)pRawRx->dummy.sequenceNumber,
                         (char *)pRawRx->dummy.someData.buf);
                d_printf(D_DEBUG, NULL, "[AOC] Rx statistics: (COUNT) %4zu / %d \t(RSSI) %4d \t (CHANNEL LOAD) %4d\n",
                         success_count,
                         (int)pRawRx->dummy.sequenceNumber + 1,
                         pRxPkt->Elem.pETSIG5Desc->Rx.RSSI,
                         (int)pRxPkt->Elem.pETSIG5Desc->Rx.ChanLoad);

                // Use the GSER encoder to show the message in the debug output
                uint8_t *pGSerBuf;
                unsigned int GSerLen;

                GSerLen = asn1_gser_encode(&pGSerBuf, asn1_type_RAW_DUMMYPDU, pRawRx);

                d_printf(D_INFO, NULL, "GSER[%d]:\n%s", GSerLen, pGSerBuf);
                free(pGSerBuf);
            }
        }

        // Access some information in the packet descriptor
        // RSSI from G5 descriptor
        tETSIG5Desc *pG5 = pRxPkt->Elem.pETSIG5Desc;
        d_printf(D_INFO, NULL, "RSSI=%d\n", pG5->Rx.RSSI);
    }
}
// Close the doxygen group
/**
 * @}
 */
