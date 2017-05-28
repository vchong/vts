/*
 * Copyright (C) 2017 The Android Open Source Project
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

package com.android.tradefed.util;

import java.io.InputStream;
import java.io.IOException;
import java.util.NoSuchElementException;

import com.android.tradefed.log.LogUtil.CLog;
import com.android.tradefed.util.StreamUtil;

import org.json.JSONException;
import org.json.JSONObject;

/**
 * Util to access a VTS config file.
 */
public class VtsVendorConfigFileUtil {
    private static final String VENDOR_TEST_CONFIG_FILE_PATH =
            "/config/google-tradefed-vts-config.config";
    private JSONObject vendorConfigJson = null;

    /**
     * Loads a VTS vendor config file.
     *
     * @param configPath, the path of a config file.
     * @throws RuntimeException
     */
    public boolean LoadVendorConfig(String configPath) throws RuntimeException {
        if (configPath == null) {
            configPath = VENDOR_TEST_CONFIG_FILE_PATH;
        }
        CLog.i("Loading vendor test config %s", configPath);
        InputStream config = getClass().getResourceAsStream(configPath);
        if (config == null) {
            CLog.e("Vendor test config file %s does not exist", VENDOR_TEST_CONFIG_FILE_PATH);
            return false;
        }
        try {
            String content = StreamUtil.getStringFromStream(config);
            if (content == null) {
                CLog.e("Loaded vendor test config is empty");
                return false;
            }
            CLog.i("Loaded vendor test config %s", content);
            vendorConfigJson = new JSONObject(content);
        } catch (IOException e) {
            throw new RuntimeException("Failed to read vendor config json file");
        } catch (JSONException e) {
            throw new RuntimeException("Failed to parse vendor config json data");
        }
        return true;
    }

    /**
     * Gets the value of a config variable.
     *
     * @param varName, the name of a variable.
     * @throws NoSuchElementException
     */
    public String GetVendorConfigVariable(String varName) throws NoSuchElementException {
        if (vendorConfigJson == null) {
            CLog.e("Vendor config json file invalid or not yet loaded.");
            throw new NoSuchElementException("config is empty");
        }
        try {
            return vendorConfigJson.getString(varName);
        } catch (JSONException e) {
            CLog.e("Vendor config file does not define %s", varName);
            throw new NoSuchElementException("config parsing error");
        }
    }
}
