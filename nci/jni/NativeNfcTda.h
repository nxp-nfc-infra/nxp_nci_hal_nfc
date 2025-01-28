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

#pragma once
#include <nativehelper/ScopedLocalRef.h>

#include "JavaClassConstants.h"
#include "NfcJniUtil.h"
#include "SyncEvent.h"
#include "nfa_api.h"

#define nfcTdaProfile (NativeNfcTda::getInstance())

class NativeNfcTda {
 public:
  NativeNfcTda() noexcept;
  /*******************************************************************************
   **
   ** Function:        initialize
   **
   ** Description:     Initialize member variables.
   **
   ** Returns:         None
   **
   *******************************************************************************/
  void initialize(nfc_jni_native_data* native);

  /*****************************************************************************
  **
  ** Function:        getInstance
  **
  ** Description:     Get the NativeNfcDiscoveryMode singleton object.
  **
  ** Returns:         NativeNfcDiscoveryMode object.
  **
  *******************************************************************************/
  static NativeNfcTda& getInstance();

  /*******************************************************************************
  **
  ** Function:        NativeNfcTda::discoverTDA
  **
  ** Description:     get the attached TDAs Info
  **
  ** Returns:         jobjectArray : TDAs Info to upper layer
  **
  *******************************************************************************/
  jobjectArray discoverTDA(JNIEnv* e, jobject o);

  /*******************************************************************************
  **
  ** Function:        NativeNfcTda::openTDA
  **
  ** Description:     Open TDA
  **
  ** Returns:         Connection ID
  **
  *******************************************************************************/
  jbyte openTDA(JNIEnv* e, jobject o, jbyte tdaID, jboolean standBy);

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
  jbyteArray transceive(JNIEnv* e, jobject o, jbyteArray in_data);

  /*******************************************************************************
  **
  ** Function:        NativeNfcTda::closeTDA
  **
  ** Description:     close the TDA
  **
  ** Returns:         status byte
  **
  *******************************************************************************/
  jbyte closeTDA(JNIEnv* e, jobject o, jbyte tdaID, jboolean standBy);

 private:
  static NativeNfcTda sNativeNfcTdaInstance;
  nfc_jni_native_data* mNativeTdaData;
};
