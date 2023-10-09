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
package com.android.nfc.dhimpl;
import com.android.nfc.DeviceHost;
import com.nxp.nfc.NfcTDAInfo;

public class NativeNfcTdaManager {

  public native NfcTDAInfo[] discoverTDA();

  public native byte openTDA(byte tdaID, boolean standBy);

  public native byte[] transceive(byte[] in_cmd_data);

  public native byte closeTDA(byte tdaID, boolean standBy);
}
