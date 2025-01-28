/******************************************************************************
 *
 *  Copyright 2023 NXP
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
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

#include "NativeNfcTda.h"

#include <android-base/stringprintf.h>
#include <android-base/logging.h>
#include <nativehelper/ScopedPrimitiveArray.h>
#include <string.h>

#include "NfcJniUtil.h"
#include "nfa_tda_api.h"

using android::base::StringPrintf;
extern bool nfc_debug_enabled;
SyncEvent sCtLibSyncEvt;
static std::basic_string<uint8_t> sRxTdaDataBuff;
NFC_STATUS gStatus = NFC_STATUS_FAILED;

NativeNfcTda NativeNfcTda::sNativeNfcTdaInstance;

/*******************************************************************************
**
** Function:        nativeNfcTda_CtLibCB
**
** Description:     Callback to notify the CT Lib event and udpate the status.
**
** Returns:         None
**
*******************************************************************************/
static void nativeNfcTda_CtLibCB(uint8_t status) {
  LOG(INFO) << StringPrintf("%s Enter", __func__);
  gStatus = status;
  SyncEventGuard g(sCtLibSyncEvt);
  sCtLibSyncEvt.notifyOne();
}
/*******************************************************************************
**
** Function:        NativeNfcTda
**
** Description:     Initialize member variables.
**
** Returns:         None
**
*******************************************************************************/
NativeNfcTda::NativeNfcTda() noexcept : mNativeTdaData(NULL) {}
/*****************************************************************************
**
** Function:        getInstance
**
** Description:     Get the NativeNfcTda singleton object.
**
** Returns:         NativeNfcTda object.
**
*******************************************************************************/
NativeNfcTda& NativeNfcTda::getInstance() { return sNativeNfcTdaInstance; }

/*******************************************************************************
**
** Function:        initialize
**
** Description:     Initialize member variables.
**                  native: Native data.
**
** Returns:         None
**
*******************************************************************************/
void NativeNfcTda::initialize(nfc_jni_native_data* native) {
  mNativeTdaData = native;
}

/*******************************************************************************
**
** Function:        NativeNfcTda::discoverTDA
**
** Description:     get the attached TDAs Info
**
** Returns:         jobjectArray : TDAs Info to upper layer
**
*******************************************************************************/
jobjectArray NativeNfcTda::discoverTDA(JNIEnv* env, jobject thiz) {
  LOG(INFO) << StringPrintf("%s Enter", __func__);

  tda_control_t tdas_info;
  bool waitOk = false;
  SyncEventGuard g(sCtLibSyncEvt);

  NFA_Discover_tda_slots(&tdas_info, nativeNfcTda_CtLibCB);

  waitOk = sCtLibSyncEvt.wait(1000);
  if ((waitOk == false) || (gStatus != 0x00)) {
    LOG(ERROR) << StringPrintf("%s: didn't received apdu response", __func__);
    return NULL;
  }

  jclass tdaInfoClass = env->FindClass("com/nxp/nfc/NfcTDAInfo");
  if (tdaInfoClass == NULL) {
    LOG(INFO) << StringPrintf("Can't find tda class");
  }

  jclass cardInfoClass = env->FindClass("com/nxp/nfc/CardTLVInfo");
  if (cardInfoClass == NULL) {
    LOG(INFO) << StringPrintf("Can't find card class");
  }

  jmethodID tdaConstructor = env->GetMethodID(tdaInfoClass, "<init>", "()V");
  jfieldID idField = env->GetFieldID(tdaInfoClass, "id", "B");
  jfieldID statusField = env->GetFieldID(tdaInfoClass, "status", "I");
  jfieldID numProtocolsField =
      env->GetFieldID(tdaInfoClass, "numberOfProtocols", "B");
  jfieldID protocolsField = env->GetFieldID(tdaInfoClass, "protocols", "[I");
  jfieldID numCardInfoField =
      env->GetFieldID(tdaInfoClass, "numberOfCardInfo", "B");
  jfieldID cardInfoField = env->GetFieldID(tdaInfoClass, "cardTLVInfo",
                                           "[Lcom/nxp/nfc/CardTLVInfo;");

  jmethodID cardInfoConstructor =
      env->GetMethodID(cardInfoClass, "<init>", "()V");
  jfieldID typeField = env->GetFieldID(cardInfoClass, "type", "B");
  jfieldID lengthField = env->GetFieldID(cardInfoClass, "length", "B");
  jfieldID valueField = env->GetFieldID(cardInfoClass, "value", "[B");

  jobjectArray tdaInfoArray =
      env->NewObjectArray(tdas_info.num_tda_supported, tdaInfoClass, NULL);

  for (int i = 0; i < tdas_info.num_tda_supported; i++) {
    jobject tdaInfoObj = env->NewObject(tdaInfoClass, tdaConstructor);
    env->SetByteField(tdaInfoObj, idField, tdas_info.p_tda[i].id);
    env->SetIntField(tdaInfoObj, statusField, tdas_info.p_tda[i].status);
    env->SetByteField(tdaInfoObj, numProtocolsField,
                      tdas_info.p_tda[i].number_of_protocols);

    jbyteArray protocolsArray =
        env->NewByteArray(tdas_info.p_tda[i].number_of_protocols);
    env->SetByteArrayRegion(protocolsArray, 0,
                            tdas_info.p_tda[i].number_of_protocols,
                            (const jbyte*)tdas_info.p_tda[i].protocols_t);
    env->SetObjectField(tdaInfoObj, protocolsField, protocolsArray);

    env->SetByteField(tdaInfoObj, numCardInfoField,
                      tdas_info.p_tda[i].number_of_card_info);

    jobjectArray cardInfoArray = env->NewObjectArray(
        tdas_info.p_tda[i].number_of_card_info, cardInfoClass, NULL);
    for (int j = 0; j < tdas_info.p_tda[i].number_of_card_info; j++) {
      jobject cardInfoObj = env->NewObject(cardInfoClass, cardInfoConstructor);

      env->SetByteField(cardInfoObj, typeField,
                        tdas_info.p_tda[i].card_tlv_info[j].type);
      env->SetByteField(cardInfoObj, lengthField,
                        tdas_info.p_tda[i].card_tlv_info[j].length);

      jbyteArray valueArray =
          env->NewByteArray(tdas_info.p_tda[i].card_tlv_info[j].length);
      env->SetByteArrayRegion(
          valueArray, 0, tdas_info.p_tda[i].card_tlv_info[j].length,
          (const jbyte*)tdas_info.p_tda[i].card_tlv_info[j].value);
      env->SetObjectField(cardInfoObj, valueField, valueArray);

      env->SetObjectArrayElement(cardInfoArray, j, cardInfoObj);
      env->DeleteLocalRef(cardInfoObj);
      env->DeleteLocalRef(valueArray);
    }

    env->SetObjectField(tdaInfoObj, cardInfoField, cardInfoArray);
    env->DeleteLocalRef(cardInfoArray);

    env->SetObjectArrayElement(tdaInfoArray, i, tdaInfoObj);
    env->DeleteLocalRef(tdaInfoObj);
    env->DeleteLocalRef(protocolsArray);
  }

  return tdaInfoArray;
}

/*******************************************************************************
**
** Function:        NativeNfcTda::openTDA
**
** Description:     Open TDA
**
** Returns:         Connection ID
**
*******************************************************************************/
jbyte NativeNfcTda::openTDA(JNIEnv* e, jobject o, jbyte tdaID,
                            jboolean standBy) {
  LOG(INFO) << StringPrintf("%s ID : 0x%X", __func__, tdaID);

  int8_t cid = 0x00;
  bool waitOk = false;
  SyncEventGuard g(sCtLibSyncEvt);
  NFA_Open_tda_slot(tdaID, &cid, standBy, nativeNfcTda_CtLibCB);
  waitOk = sCtLibSyncEvt.wait(1000);  // mSec

  if (waitOk == false) {
    LOG(ERROR) << StringPrintf("%s: Failed", __func__);
    cid = 0x00;
  }
  return cid;
}

/*******************************************************************************
**
** Function:        NativeNfcTda::closeTDA
**
** Description:     close the TDA
**
** Returns:         status byte
**
*******************************************************************************/
jbyte NativeNfcTda::closeTDA(JNIEnv* e, jobject o, jbyte tdaID,
                             jboolean standBy) {
  LOG(INFO) << StringPrintf("%s: TDA ID=0x%X  standBy %d", __func__,
                                      tdaID, standBy);

  NFC_STATUS status = NFC_STATUS_FAILED;
  bool waitOk = false;

  SyncEventGuard g(sCtLibSyncEvt);
  NFA_Close_tda_slot(tdaID, standBy, nativeNfcTda_CtLibCB);
  waitOk = sCtLibSyncEvt.wait(1000);  // mSec

  if (waitOk == false) {
    LOG(ERROR) << StringPrintf("%s: Failed", __func__);
  } else {
    status = gStatus;
  }
  return status;
}

/*******************************************************************************
**
** Function:        NativeNfcTda::transceive
**
** Description:     Send raw data to the TDA. wait for receive the tda's
*response.
**                  e: JVM environment.
**                  o: Java object.
**                  in_data: raw data to tda.
**
** Returns:         tda's response.
**
*******************************************************************************/
jbyteArray NativeNfcTda::transceive(JNIEnv* e, jobject o, jbyteArray in_data) {
  LOG(INFO) << StringPrintf("%s Enter", __func__);

  bool waitOk = false;
  tda_data cmd_apdu, rsp_apdu;

  memset(&cmd_apdu, 0x00, sizeof(cmd_apdu));
  memset(&rsp_apdu, 0x00, sizeof(rsp_apdu));

  ScopedByteArrayRO bytes(e, in_data);
  uint8_t* buf =
      const_cast<uint8_t*>(reinterpret_cast<const uint8_t*>(&bytes[0]));

  cmd_apdu.len = (uint32_t)bytes.size();
  cmd_apdu.p_data = (uint8_t*)buf;

  SyncEventGuard g(sCtLibSyncEvt);
  sRxTdaDataBuff.clear();
  ScopedLocalRef<jbyteArray> rsp(e, NULL);

  NFA_transceive_tda(&cmd_apdu, &rsp_apdu, nativeNfcTda_CtLibCB);
  waitOk = sCtLibSyncEvt.wait(1000);  // mSec

  if (waitOk == false) {
    LOG(ERROR) << StringPrintf("%s: didn't received apdu response", __func__);
    goto exit;
  }

  rsp.reset(e->NewByteArray(rsp_apdu.len));

  if (rsp.get() != NULL) {
    e->SetByteArrayRegion(rsp.get(), 0, rsp_apdu.len,
                          (const jbyte*)rsp_apdu.p_data);
  }

exit:
  if (NULL != rsp_apdu.p_data) {
    free(rsp_apdu.p_data);
    rsp_apdu.p_data = NULL;
  }

  return rsp.release();
}
