/******************************************************************************
 *
 *  Copyright 1999-2012 Broadcom Corporation
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at:
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 ******************************************************************************/
#ifndef BTM_INT_TYPES_H
#define BTM_INT_TYPES_H

#include "btif/include/btif_bqr.h"
#include "btm_api_types.h"
#include "btm_ble_api_types.h"
#include "btm_ble_int_types.h"
#include "hcidefs.h"
#include "osi/include/alarm.h"
#include "osi/include/list.h"
#include "rfcdefs.h"
#include "stack/acl/acl.h"
#include "stack/btm/security_device_record.h"

#define BTM_SEC_IS_SM4(sm) ((bool)(BTM_SM4_TRUE == ((sm)&BTM_SM4_TRUE)))
#define BTM_SEC_IS_SM4_LEGACY(sm) ((bool)(BTM_SM4_KNOWN == ((sm)&BTM_SM4_TRUE)))
#define BTM_SEC_IS_SM4_UNKNOWN(sm) \
  ((bool)(BTM_SM4_UNKNOWN == ((sm)&BTM_SM4_TRUE)))

#define BTM_SEC_LE_MASK                              \
  (BTM_SEC_LE_AUTHENTICATED | BTM_SEC_LE_ENCRYPTED | \
   BTM_SEC_LE_LINK_KEY_KNOWN | BTM_SEC_LE_LINK_KEY_AUTHED)

/*
 * Define device configuration structure
*/
typedef struct {
  tBTM_LOC_BD_NAME bd_name;  /* local Bluetooth device name */
  bool pin_type;             /* true if PIN type is fixed */
  uint8_t pin_code_len;      /* Bonding information */
  PIN_CODE pin_code;         /* PIN CODE if pin type is fixed */
  bool connectable;          /* If true page scan should be enabled */
  uint8_t def_inq_scan_mode; /* ??? limited/general/none */
} tBTM_CFG;

enum {
  BTM_PM_ST_ACTIVE = BTM_PM_STS_ACTIVE,
  BTM_PM_ST_HOLD = BTM_PM_STS_HOLD,
  BTM_PM_ST_SNIFF = BTM_PM_STS_SNIFF,
  BTM_PM_ST_PARK = BTM_PM_STS_PARK,
  BTM_PM_ST_PENDING = BTM_PM_STS_PENDING,
  BTM_PM_ST_INVALID = 0xFF
};

#define BTM_PM_REC_NOT_USED 0
typedef struct {
  tBTM_PM_STATUS_CBACK*
      cback;    /* to notify the registered party of mode change event */
  uint8_t mask; /* registered request mask. 0, if this entry is not used */
} tBTM_PM_RCB;

/* Pairing State */
enum {
  BTM_PAIR_STATE_IDLE, /* Idle                                         */
  BTM_PAIR_STATE_GET_REM_NAME, /* Getting the remote name (to check for SM4) */
  BTM_PAIR_STATE_WAIT_PIN_REQ, /* Started authentication, waiting for PIN req
                                  (PIN is pre-fetched) */
  BTM_PAIR_STATE_WAIT_LOCAL_PIN,       /* Waiting for local PIN code */
  BTM_PAIR_STATE_WAIT_NUMERIC_CONFIRM, /* Waiting user 'yes' to numeric
                                          confirmation   */
  BTM_PAIR_STATE_KEY_ENTRY, /* Key entry state (we are a keyboard)          */
  BTM_PAIR_STATE_WAIT_LOCAL_OOB_RSP, /* Waiting for local response to peer OOB
                                        data  */
  BTM_PAIR_STATE_WAIT_LOCAL_IOCAPS, /* Waiting for local IO capabilities and OOB
                                       data */
  BTM_PAIR_STATE_INCOMING_SSP, /* Incoming SSP (got peer IO caps when idle) */
  BTM_PAIR_STATE_WAIT_AUTH_COMPLETE, /* All done, waiting authentication
                                        cpmplete    */
  BTM_PAIR_STATE_WAIT_DISCONNECT     /* Waiting to disconnect the ACL */
};
typedef uint8_t tBTM_PAIRING_STATE;

#define BTM_PAIR_FLAGS_WE_STARTED_DD \
  0x01 /* We want to do dedicated bonding              */
#define BTM_PAIR_FLAGS_PEER_STARTED_DD \
  0x02 /* Peer initiated dedicated bonding             */
#define BTM_PAIR_FLAGS_DISC_WHEN_DONE 0x04 /* Disconnect when done     */
#define BTM_PAIR_FLAGS_PIN_REQD \
  0x08 /* set this bit when pin_callback is called     */
#define BTM_PAIR_FLAGS_PRE_FETCH_PIN \
  0x10 /* set this bit when pre-fetch pin     */
#define BTM_PAIR_FLAGS_REJECTED_CONNECT \
  0x20 /* set this bit when rejected incoming connection  */
#define BTM_PAIR_FLAGS_WE_CANCEL_DD \
  0x40 /* set this bit when cancelling a bonding procedure */
#define BTM_PAIR_FLAGS_LE_ACTIVE \
  0x80 /* use this bit when SMP pairing is active */

typedef struct {
  bool is_mux;
  RawAddress bd_addr;
  uint16_t psm;
  bool is_orig;
  tBTM_SEC_CALLBACK* p_callback;
  void* p_ref_data;
  uint32_t mx_proto_id;
  uint32_t mx_chan_id;
  tBT_TRANSPORT transport;
  tBTM_BLE_SEC_ACT sec_act;
} tBTM_SEC_QUEUE_ENTRY;

// Bluetooth Quality Report - Report receiver
typedef void(tBTM_BT_QUALITY_REPORT_RECEIVER)(uint8_t len, uint8_t* p_stream);

/* Define a structure to hold all the BTM data
*/

#define BTM_STATE_BUFFER_SIZE 5 /* size of state buffer */

typedef struct {
  tBTM_CFG cfg; /* Device configuration */

  /****************************************************
  **      Power Management
  ****************************************************/
  tBTM_PM_RCB pm_reg_db[BTM_MAX_PM_RECORDS + 1]; /* per application/module */

  uint8_t pm_pend_id;   /* the id pf the module, which has a pending PM cmd */

  /*****************************************************
  **      Device control
  *****************************************************/
  tBTM_DEVCB devcb;

  /*****************************************************
  **      BLE Device controllers
  *****************************************************/
  tBTM_BLE_CB ble_ctr_cb;

  uint16_t enc_handle;
  BT_OCTET8 enc_rand; /* received rand value from LTK request*/
  uint16_t ediv;      /* received ediv value from LTK request */
  uint8_t key_size;
  tBTM_BLE_VSC_CB cmn_ble_vsc_cb;

  /* Packet types supported by the local device */
  uint16_t btm_sco_pkt_types_supported;

  /*****************************************************
  **      Inquiry
  *****************************************************/
  tBTM_INQUIRY_VAR_ST btm_inq_vars;

  /*****************************************************
  **      SCO Management
  *****************************************************/
  tSCO_CB sco_cb;

  /*****************************************************
  **      Security Management
  *****************************************************/
  tBTM_APPL_INFO api;

#define BTM_SEC_MAX_RMT_NAME_CALLBACKS 2
  tBTM_RMT_NAME_CALLBACK* p_rmt_name_callback[BTM_SEC_MAX_RMT_NAME_CALLBACKS];

  tBTM_SEC_DEV_REC* p_collided_dev_rec;
  alarm_t* sec_collision_timer;
  uint64_t collision_start_time;
  uint32_t dev_rec_count; /* Counter used for device record timestamp */
  uint8_t security_mode;
  bool pairing_disabled;
  bool connect_only_paired;
  bool security_mode_changed; /* mode changed during bonding */
  bool pin_type_changed;      /* pin type changed during bonding */
  bool sec_req_pending;       /*   true if a request is pending */

  uint8_t pin_code_len;             /* for legacy devices */
  PIN_CODE pin_code;                /* for legacy devices */
  tBTM_PAIRING_STATE pairing_state; /* The current pairing state    */
  uint8_t pairing_flags;            /* The current pairing flags    */
  RawAddress pairing_bda;           /* The device currently pairing */
  alarm_t* pairing_timer;           /* Timer for pairing process    */
  uint16_t disc_handle;             /* for legacy devices */
  uint8_t disc_reason;              /* for legacy devices */
  tBTM_SEC_SERV_REC sec_serv_rec[BTM_SEC_MAX_SERVICE_RECORDS];
  list_t* sec_dev_rec; /* list of tBTM_SEC_DEV_REC */
  tBTM_SEC_SERV_REC* p_out_serv;
  tBTM_MKEY_CALLBACK* mkey_cback;

  RawAddress connecting_bda;
  DEV_CLASS connecting_dc;
  uint8_t trace_level;
  bool is_paging;     /* true, if paging is in progess */
  bool is_inquiry;    /* true, if inquiry is in progess */
  fixed_queue_t* page_queue;
  bool paging;
  bool discing;
  fixed_queue_t* sec_pending_q; /* pending sequrity requests in
                                   tBTM_SEC_QUEUE_ENTRY format */

  char state_temp_buffer[BTM_STATE_BUFFER_SIZE];
  // BQR Receiver
  tBTM_BT_QUALITY_REPORT_RECEIVER* p_bqr_report_receiver;

  tACL_CB acl_cb_;
} tBTM_CB;

/* security action for L2CAP COC channels */
#define BTM_SEC_OK 1
#define BTM_SEC_ENCRYPT 2         /* encrypt the link with current key */
#define BTM_SEC_ENCRYPT_NO_MITM 3 /* unauthenticated encryption or better */
#define BTM_SEC_ENCRYPT_MITM 4    /* authenticated encryption */
#define BTM_SEC_ENC_PENDING 5     /* wait for link encryption pending */

typedef uint8_t tBTM_SEC_ACTION;

#endif  // BTM_INT_TYPES_H
