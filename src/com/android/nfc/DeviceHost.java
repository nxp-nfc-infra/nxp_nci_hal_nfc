/*
 * Copyright (C) 2011 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

package com.android.nfc;

import android.annotation.Nullable;
import android.nfc.NdefMessage;
import android.nfc.cardemulation.PollingFrame;
import android.os.Bundle;

import java.io.FileDescriptor;
import java.io.IOException;
import java.util.List;

public interface DeviceHost {
    public interface DeviceHostListener {
        public void onRemoteEndpointDiscovered(TagEndpoint tag);

        /**
         */
        public void onHostCardEmulationActivated(int technology);
        public void onHostCardEmulationData(int technology, byte[] data);
        public void onHostCardEmulationDeactivated(int technology);

        public void onRemoteFieldActivated();

        public void onRemoteFieldDeactivated();

        public void onNfcTransactionEvent(byte[] aid, byte[] data, String seName);

        public void onEeUpdated();

        public void onHwErrorReported();

        public void onPollingLoopDetected(List<PollingFrame> pollingFrames);

        public void onWlcStopped(int wpt_end_condition);

        public void onVendorSpecificEvent(int gid, int oid, byte[] payload);
    }

    public interface TagEndpoint {
        boolean connect(int technology);
        boolean reconnect();
        boolean disconnect();

        boolean presenceCheck();
        boolean isPresent();
        void startPresenceChecking(int presenceCheckDelay,
                                   @Nullable TagDisconnectedCallback callback);
        void stopPresenceChecking();

        int[] getTechList();
        void removeTechnology(int tech); // TODO remove this one
        Bundle[] getTechExtras();
        byte[] getUid();
        int getHandle();

        byte[] transceive(byte[] data, boolean raw, int[] returnCode);

        boolean checkNdef(int[] out);
        byte[] readNdef();
        boolean writeNdef(byte[] data);
        NdefMessage findAndReadNdef();
        NdefMessage getNdef();
        boolean formatNdef(byte[] key);
        boolean isNdefFormatable();
        boolean makeReadOnly();

        int getConnectedTechnology();

        /**
         * Find Ndef only
         * As per NFC forum test specification ndef write test expects only
         * ndef detection followed by ndef write. System property
         * nfc.dta.skipNdefRead added to skip default ndef read before tag
         * dispatch. This system property is valid only in reader mode.
         */
        void findNdef();
    }

    public interface TagDisconnectedCallback {
        void onTagDisconnected();
    }

    public interface NfceeEndpoint {
        // TODO flesh out multi-EE and use this
    }

    public interface NfcDepEndpoint {
        /**
         * Invalid target mode
         */
        public static final short MODE_INVALID = 0xff;

        public byte[] receive();

        public boolean send(byte[] data);

        public boolean connect();

        public boolean disconnect();

        public byte[] transceive(byte[] data);

        public int getHandle();

        public int getMode();

        public byte[] getGeneralBytes();
    }

    /**
     * Called at boot if NFC is disabled to give the device host an opportunity
     * to check the firmware version to see if it needs updating. Normally the firmware version
     * is checked during {@link #initialize(boolean enableScreenOffSuspend)},
     * but the firmware may need to be updated after an OTA update.
     *
     * <p>This is called from a thread
     * that may block for long periods of time during the update process.
     */
    public boolean checkFirmware();

    public boolean initialize();

    public boolean deinitialize();

    public String getName();

    public void enableDiscovery(NfcDiscoveryParameters params, boolean restart);

    public void disableDiscovery();

    public boolean sendRawFrame(byte[] data);

    public boolean routeAid(byte[] aid, int route, int aidInfo, int power);

    public boolean unrouteAid(byte[] aid);

    public boolean commitRouting();

    public void registerT3tIdentifier(byte[] t3tIdentifier);

    public void deregisterT3tIdentifier(byte[] t3tIdentifier);

    public void clearT3tIdentifiersCache();

    public int getLfT3tMax();

    public void resetTimeouts();

    public boolean setTimeout(int technology, int timeout);

    public int getTimeout(int technology);

    public void doAbort(String msg);

    boolean canMakeReadOnly(int technology);

    int getMaxTransceiveLength(int technology);

    public int getAidTableSize();

    boolean getExtendedLengthApdusSupported();

    void dump(FileDescriptor fd);

    public void doSetScreenState(int screen_state_mask, boolean alwaysPoll);

    public int getNciVersion();

    public void enableDtaMode();

    public void disableDtaMode();

    public void factoryReset();

    public void shutdown();

    public boolean setNfcSecure(boolean enable);

    public boolean isObserveModeSupported();

    public boolean setObserveMode(boolean enable);

    public boolean isObserveModeEnabled();

    /**
    * Get the committed listen mode routing configuration
    */
    byte[] getRoutingTable();

    /**
    * Get the Max Routing Table size from cache
    */
    int getMaxRoutingTableSize();

    /**
    * Start or stop RF polling
    */
    void startStopPolling(boolean enable);

    /**
    * Set NFCC power state by sending NFCEE_POWER_AND_LINK_CNTRL_CMD
    */
    void setNfceePowerAndLinkCtrl(boolean enable);

    /**
     * Enable or Disable the Power Saving Mode based on flag
     */
    boolean setPowerSavingMode(boolean flag);

    boolean isMultiTag();

    void setIsoDepProtocolRoute(int route);
    void setTechnologyABRoute(int route);
    void clearRoutingEntry(int clearFlags);

    /**
    * Set NFCC discovery technology for polling and listening
    */
    void setDiscoveryTech(int pollTech, int listenTech);
    void resetDiscoveryTech();
    /**
     * Sends Vendor NCI command
     */
    NfcVendorNciResponse sendRawVendorCmd(int mt, int gid, int oid, byte[] payload);

    void enableVendorNciNotifications(boolean enabled);
}
