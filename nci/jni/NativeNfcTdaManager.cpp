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

#include "JavaClassConstants.h"
#include "NativeNfcTda.h"
#include "NfcJniUtil.h"
#include <android-base/stringprintf.h>
#include <base/logging.h>

extern bool nfc_debug_enabled;
using android::base::StringPrintf;

namespace android {

/*******************************************************************************
**
** Function:        NfcTdaManager_discoverTDA
**
** Description:     get the connected TDAs Info
**
** Returns:         jobjectArray
**
*******************************************************************************/
static jobjectArray NfcTdaManager_discoverTDA(JNIEnv *e, jobject o) {
  return nfcTdaProfile.discoverTDA(e, o);
}

/*******************************************************************************
**
** Function:        NfcTdaManager_openTDA
**
** Description:     Open the TDA
**
** Returns:         connection ID
**
*******************************************************************************/
static jbyte NfcTdaManager_openTDA(JNIEnv *e, jobject o, jbyte tdaID,
                                   jboolean standBy) {
  return nfcTdaProfile.openTDA(e, o, tdaID, standBy);
}

/*******************************************************************************
**
** Function:        NfcTdaManager_transceive
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
static jbyteArray NfcTdaManager_transceive(JNIEnv *e, jobject o,
                                           jbyteArray in_data) {
  return nfcTdaProfile.transceive(e, o, in_data);
}

/*******************************************************************************
**
** Function:        NfcTdaManager_closeTDA
**
** Description:     Close the TDA
**
** Returns:         status byte
**
*******************************************************************************/
static jbyte NfcTdaManager_closeTDA(JNIEnv *e, jobject o, jbyte tdaID,
                                    jboolean standBy) {
  return nfcTdaProfile.closeTDA(e, o, tdaID, standBy);
}
/*****************************************************************************
 **
 ** Description:     JNI functions
 **
 *****************************************************************************/
static JNINativeMethod gMethods[] = {
    {"discoverTDA", "()[Lcom/nxp/nfc/NfcTDAInfo;",
     (void *)NfcTdaManager_discoverTDA},
    {"openTDA", "(BZ)B", (void *)NfcTdaManager_openTDA},
    {"transceive", "([B)[B", (void *)NfcTdaManager_transceive},
    {"closeTDA", "(BZ)B", (void *)NfcTdaManager_closeTDA},
};

/*******************************************************************************
 **
 ** Function:        register_com_android_nfc_NativeNfcTda
 **
 ** Description:     Regisgter JNI functions with Java Virtual Machine.
 **                  e: Environment of JVM.
 **
 ** Returns:         Status of registration.
 **
 *******************************************************************************/
int register_com_android_nfc_NativeNfcTda(JNIEnv *e) {
  return jniRegisterNativeMethods(e, gNativeNfcTdaProfileClassName, gMethods,
                                  NELEM(gMethods));
}
} /* namespace android */
