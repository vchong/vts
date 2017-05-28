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

package com.android.tradefed.targetprep;

import java.io.BufferedReader;
import java.io.File;
import java.io.FileNotFoundException;
import java.io.FileReader;
import java.io.IOException;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.util.List;
import java.util.LinkedList;
import java.util.NoSuchElementException;

import com.android.tradefed.build.IBuildInfo;
import com.android.tradefed.device.ITestDevice;
import com.android.tradefed.log.LogUtil.CLog;
import com.android.tradefed.util.CommandResult;
import com.android.tradefed.util.CommandStatus;
import com.android.tradefed.util.FileUtil;
import com.android.tradefed.util.IRunUtil;
import com.android.tradefed.util.RunUtil;
import com.android.tradefed.util.VtsVendorConfigFileUtil;

import com.android.vts.proto.VtsReportMessage.DashboardPostMessage;
import com.android.vts.proto.VtsReportMessage.TestPlanReportMessage;

import com.google.api.client.auth.oauth2.Credential;
import com.google.api.client.googleapis.auth.oauth2.GoogleCredential;
import com.google.api.client.googleapis.javanet.GoogleNetHttpTransport;
import com.google.api.client.json.jackson2.JacksonFactory;
import com.google.api.client.json.JsonFactory;

/**
 * Uploads the VTS test plan execution result to the web DB using a RESTful API and
 * an OAuth2 credential kept in a json file.
 */
public class VtsTestPlanResultReporter implements ITargetPreparer, ITargetCleaner {
    private static final String PLUS_ME = "https://www.googleapis.com/auth/plus.me";
    private static final String TEST_PLAN_EXECUTION_RESULT = "vts-test-plan-execution-result";
    private static final String TEST_PLAN_REPORT_FILE = "TEST_PLAN_REPORT_FILE";
    private static VtsVendorConfigFileUtil configReader = null;
    private static final int BASE_TIMEOUT_MSECS = 1000 * 60;
    IRunUtil mRunUtil = new RunUtil();

    /**
     * {@inheritDoc}
     */
    @Override
    public void setUp(ITestDevice device, IBuildInfo buildInfo) {
        File mStatusDir = null;
        try {
            mStatusDir = FileUtil.createTempDir(TEST_PLAN_EXECUTION_RESULT);
            if (mStatusDir != null) {
                File statusFile = new File(mStatusDir, "status.json");
                buildInfo.setFile(TEST_PLAN_REPORT_FILE, statusFile, buildInfo.getBuildId());
            }
        } catch (IOException ex) {
            CLog.e("Can't create a temp dir to store test execution results.");
            return;
        }
        configReader = new VtsVendorConfigFileUtil();
        configReader.LoadVendorConfig(null);
    }

    /**
     * {@inheritDoc}
     */
    @Override
    public void tearDown(ITestDevice device, IBuildInfo buildInfo, Throwable e) {
        File reportFile = buildInfo.getFile(TEST_PLAN_REPORT_FILE);
        String repotFilePath = reportFile.getAbsolutePath();
        try (BufferedReader br = new BufferedReader(new FileReader(repotFilePath))) {
            DashboardPostMessage postMessage = new DashboardPostMessage();
            String currentLine;
            while ((currentLine = br.readLine()) != null) {
                String[] lineWords = currentLine.split("\\s+");
                if (lineWords.length != 2) {
                    CLog.e(String.format("Invalid keys %s", currentLine));
                    continue;
                }
                TestPlanReportMessage testPlanMessage = new TestPlanReportMessage();
                testPlanMessage.addTestModuleName(lineWords[0]);
                testPlanMessage.addTestModuleStartTimestamp(Long.parseLong(lineWords[1]));
                postMessage.addTestPlanReport(testPlanMessage);
            }
            Upload(postMessage);
        } catch (IOException ex) {
            CLog.d(String.format("Can't read the test plan result file %s", repotFilePath));
            return;
        }
    }

    /*
     * Returns an OAuth2 token string obtained using a service account json keyfile.
     *
     * Uses the service account keyfile located at config variable 'service_key_json_path'
     * to request an OAuth2 token.
     */
    private String GetToken() {
        String keyFilePath;
        try {
            keyFilePath = configReader.GetVendorConfigVariable("service_key_json_path");
        } catch (NoSuchElementException e) {
            return null;
        }

        JsonFactory jsonFactory = JacksonFactory.getDefaultInstance();
        Credential credential = null;
        try {
            List<String> listStrings = new LinkedList<String>();
            listStrings.add(PLUS_ME);
            credential = GoogleCredential.fromStream(new FileInputStream(keyFilePath))
                                 .createScoped(listStrings);
            credential.refreshToken();
            return credential.getAccessToken();
        } catch (FileNotFoundException e) {
            CLog.e(String.format("Service key file %s doesn't exist.", keyFilePath));
        } catch (IOException e) {
            CLog.e(String.format("Can't read the service key file, %s", keyFilePath));
        }
        return null;
    }

    /*
     * Uploads the given message to the web DB.
     *
     * @param message, DashboardPostMessage that keeps the result to upload.
     */
    private void Upload(DashboardPostMessage message) {
        message.setAccessToken(GetToken());
        try {
            String commandTemplate = configReader.GetVendorConfigVariable("dashboard_post_command");
            String filePath = WriteToTempFile(message.toByteArray());
            commandTemplate = commandTemplate.replace("{path}", filePath);
            commandTemplate = commandTemplate.replace("'", "");
            CLog.i(String.format("Upload command: %s", commandTemplate));
            CommandResult c =
                    mRunUtil.runTimedCmd(BASE_TIMEOUT_MSECS * 3, commandTemplate.split(" "));
            if (c == null || c.getStatus() != CommandStatus.SUCCESS) {
                CLog.e("Uploading the test plan execution result to GAE DB faiied.");
                CLog.e("Stdout: %s", c.getStdout());
                CLog.e("Stderr: %s", c.getStderr());
            }
        } catch (NoSuchElementException e) {
            CLog.e("dashboard_post_command unspecified in vendor config.");
        } catch (IOException e) {
            CLog.e("Couldn't write a proto message to a temp file.");
        }
    }

    /*
     * Simple wrapper to write data to a temp file.
     *
     * @param data, actual data to write to a file.
     * @throws IOException
     */
    private String WriteToTempFile(byte[] data) throws IOException {
        File tempFile = File.createTempFile("tempfile", ".tmp");
        String filePath = tempFile.getAbsolutePath();
        FileOutputStream out = new FileOutputStream(filePath);
        out.write(data);
        out.close();
        return filePath;
    }
}
