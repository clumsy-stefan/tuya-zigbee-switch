#include "tl_common.h"
#include "zb_api.h"
#include "zcl_include.h"
#include "bdb.h"
#include "ota.h"
#include "device.h"

#include "device_config/device_config.h"

void device_bdbInitCb(u8 status, u8 joinedNetwork);
void device_bdbCommissioningCb(u8 status, void *arg);
void device_bdbIdentifyCb(u8 endpoint, u16 srcAddr, u16 identifyTime);

void device_otaProcessMsgHandler(u8 evt, u8 status);


bdb_appCb_t g_deviceBdbCb =
{
  device_bdbInitCb,
  device_bdbCommissioningCb,
  device_bdbIdentifyCb,
  NULL
};

ota_callBack_t baseEndpoint_otaCb =
{
  device_otaProcessMsgHandler,
};


ev_timer_event_t *steerTimerEvt = NULL;

/**********************************************************************
 * FUNCTIONS
 */
s32 device_bdbNetworkSteerStart(void *arg)
{
  bdb_networkSteerStart();

  steerTimerEvt = NULL;
  return(-1);
}

void device_bdbInitCb(u8 status, u8 joinedNetwork)
{
  if (status == BDB_INIT_STATUS_SUCCESS)
  {
    /*
     * for non-factory-new device:
     *    load zcl data from NV, start poll rate, start ota query, bdb_networkSteerStart
     *
     * for factory-new device:
     *    steer a network
     *
     */
    if (joinedNetwork)
    {
      ota_queryStart(15 * 60);                   // 15 m
    }
    else
    {
      u16 jitter = 0;
      do
      {
        jitter = zb_random() % 0x0fff;
      }while (jitter == 0);

      if (steerTimerEvt)
      {
        TL_ZB_TIMER_CANCEL(&steerTimerEvt);
      }
      ///time_soff = 0;
      steerTimerEvt = TL_ZB_TIMER_SCHEDULE(device_bdbNetworkSteerStart, NULL, jitter);
    }
  }
  else
  {
    if (joinedNetwork)
    {
      zb_rejoinReqWithBackOff(zb_apsChannelMaskGet(), g_bdbAttrs.scanDuration);
    }
  }
}

/*********************************************************************
 * @fn      device_bdbCommissioningCb
 *
 * @brief   application callback for bdb commissioning
 *
 * @param   status - the status of bdb commissioning
 *
 * @param   arg
 *
 * @return  None
 */
void device_bdbCommissioningCb(u8 status, void *arg)
{
  switch (status)
  {
  case BDB_COMMISSION_STA_SUCCESS:
    network_indicator_commission_success(&network_indicator);

    if (steerTimerEvt)
    {
      TL_ZB_TIMER_CANCEL(&steerTimerEvt);
    }
    ota_queryStart(OTA_PERIODIC_QUERY_INTERVAL);
    break;
  case BDB_COMMISSION_STA_IN_PROGRESS:
    break;
  case BDB_COMMISSION_STA_NOT_AA_CAPABLE:
    break;
  case BDB_COMMISSION_STA_NO_NETWORK:
  case BDB_COMMISSION_STA_TCLK_EX_FAILURE:
  case BDB_COMMISSION_STA_TARGET_FAILURE:
  {
    u16 jitter = 0;
    do
    {
      jitter = zb_random() % 0x0fff;
    }while (jitter == 0);

    if (steerTimerEvt)
    {
      TL_ZB_TIMER_CANCEL(&steerTimerEvt);
    }
    steerTimerEvt = TL_ZB_TIMER_SCHEDULE(device_bdbNetworkSteerStart, NULL, jitter);
  }
  break;
  case BDB_COMMISSION_STA_FORMATION_FAILURE:
    break;
  case BDB_COMMISSION_STA_NO_IDENTIFY_QUERY_RESPONSE:
    break;
  case BDB_COMMISSION_STA_BINDING_TABLE_FULL:
    break;
  case BDB_COMMISSION_STA_NOT_PERMITTED:
    break;
  case BDB_COMMISSION_STA_NO_SCAN_RESPONSE:
  case BDB_COMMISSION_STA_PARENT_LOST:
    zb_rejoinReqWithBackOff(zb_apsChannelMaskGet(), g_bdbAttrs.scanDuration);
    break;
  case BDB_COMMISSION_STA_REJOIN_FAILURE:
    if (!zb_isDeviceFactoryNew())
    {
      zb_rejoinReqWithBackOff(zb_apsChannelMaskGet(), g_bdbAttrs.scanDuration);
    }
    break;
  default:
    break;
  }
}

void device_bdbIdentifyCb(u8 endpoint, u16 srcAddr, u16 identifyTime)
{
  // TODO: maybe handle this with blinking of system LED?
}

void device_otaProcessMsgHandler(u8 evt, u8 status)
{
  if (evt == OTA_EVT_COMPLETE)
  {
    if (status == ZCL_STA_SUCCESS)
    {
      ota_mcuReboot();
    }
    else
    {
      ota_queryStart(OTA_PERIODIC_QUERY_INTERVAL);
    }
  }
}

/*********************************************************************
 * @fn      device_leaveCnfHandler
 *
 * @brief   Handler for ZDO Leave Confirm message.
 *
 * @param   pRsp - parameter of leave confirm
 *
 * @return  None
 */
void device_leaveCnfHandler(nlme_leave_cnf_t *pLeaveCnf)
{
  if (pLeaveCnf->status == SUCCESS)
  {
    SYSTEM_RESET();
  }
}

/*********************************************************************
 * @fn      device_leaveIndHandler
 *
 * @brief   Handler for ZDO leave indication message.
 *
 * @param   pInd - parameter of leave indication
 *
 * @return  None
 */
void device_leaveIndHandler(nlme_leave_ind_t *pLeaveInd)
{
  // NO-OP
}
