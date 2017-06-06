/**
 * Copyright 2016 Google Inc. All Rights Reserved.
 *
 * <p>Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file
 * except in compliance with the License. You may obtain a copy of the License at
 *
 * <p>http://www.apache.org/licenses/LICENSE-2.0
 *
 * <p>Unless required by applicable law or agreed to in writing, software distributed under the
 * License is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either
 * express or implied. See the License for the specific language governing permissions and
 * limitations under the License.
 */
package com.android.vts.util;

import com.android.vts.entity.CoverageEntity;
import com.android.vts.entity.DeviceInfoEntity;
import com.android.vts.entity.ProfilingPointRunEntity;
import com.android.vts.entity.TestCaseRunEntity;
import com.android.vts.entity.TestEntity;
import com.android.vts.entity.TestPlanEntity;
import com.android.vts.entity.TestPlanRunEntity;
import com.android.vts.entity.TestRunEntity;
import com.android.vts.entity.TestRunEntity.TestRunType;
import com.android.vts.proto.VtsReportMessage.AndroidDeviceInfoMessage;
import com.android.vts.proto.VtsReportMessage.CoverageReportMessage;
import com.android.vts.proto.VtsReportMessage.LogMessage;
import com.android.vts.proto.VtsReportMessage.ProfilingReportMessage;
import com.android.vts.proto.VtsReportMessage.TestCaseReportMessage;
import com.android.vts.proto.VtsReportMessage.TestCaseResult;
import com.android.vts.proto.VtsReportMessage.TestPlanReportMessage;
import com.android.vts.proto.VtsReportMessage.TestReportMessage;
import com.google.appengine.api.datastore.DatastoreService;
import com.google.appengine.api.datastore.DatastoreServiceFactory;
import com.google.appengine.api.datastore.Entity;
import com.google.appengine.api.datastore.EntityNotFoundException;
import com.google.appengine.api.datastore.FetchOptions;
import com.google.appengine.api.datastore.Key;
import com.google.appengine.api.datastore.KeyFactory;
import com.google.appengine.api.datastore.KeyRange;
import com.google.appengine.api.datastore.PropertyProjection;
import com.google.appengine.api.datastore.Query;
import com.google.appengine.api.datastore.Query.Filter;
import com.google.appengine.api.datastore.Query.FilterOperator;
import com.google.appengine.api.datastore.Query.FilterPredicate;
import com.google.appengine.api.datastore.Transaction;
import java.io.IOException;
import java.util.ArrayList;
import java.util.HashSet;
import java.util.List;
import java.util.Map;
import java.util.Set;
import java.util.logging.Level;
import java.util.logging.Logger;

/** DatastoreHelper, a helper class for interacting with Cloud Datastore. */
public class DatastoreHelper {
    protected static final Logger logger = Logger.getLogger(DatastoreHelper.class.getName());

    /**
     * Returns true if there are data points newer than lowerBound in the results table.
     *
     * @param testName The test to check.
     * @param lowerBound The (exclusive) lower time bound, long, microseconds.
     * @return boolean True if there are newer data points.
     * @throws IOException
     */
    public static boolean hasNewer(String testName, Long lowerBound) throws IOException {
        if (lowerBound == null || lowerBound <= 0)
            return false;
        DatastoreService datastore = DatastoreServiceFactory.getDatastoreService();
        Key testKey = KeyFactory.createKey(TestEntity.KIND, testName);
        Key startKey = KeyFactory.createKey(testKey, TestRunEntity.KIND, lowerBound);
        Filter startFilter = new FilterPredicate(
                Entity.KEY_RESERVED_PROPERTY, FilterOperator.GREATER_THAN, startKey);
        Query q = new Query(TestRunEntity.KIND)
                          .setAncestor(KeyFactory.createKey(TestEntity.KIND, testName))
                          .setFilter(startFilter)
                          .setKeysOnly();
        return datastore.prepare(q).countEntities(FetchOptions.Builder.withLimit(1)) > 0;
    }

    /**
     * Returns true if there are data points older than upperBound in the table.
     *
     * @param testName The test to check.
     * @param upperBound The (exclusive) upper time bound, long, microseconds.
     * @return boolean True if there are older data points.
     * @throws IOException
     */
    public static boolean hasOlder(String testName, Long upperBound) throws IOException {
        if (upperBound == null || upperBound <= 0)
            return false;
        DatastoreService datastore = DatastoreServiceFactory.getDatastoreService();
        Key testKey = KeyFactory.createKey(TestEntity.KIND, testName);
        Key endKey = KeyFactory.createKey(testKey, TestRunEntity.KIND, upperBound);
        Filter endFilter =
                new FilterPredicate(Entity.KEY_RESERVED_PROPERTY, FilterOperator.LESS_THAN, endKey);
        Query q = new Query(TestRunEntity.KIND)
                          .setAncestor(KeyFactory.createKey(TestEntity.KIND, testName))
                          .setFilter(endFilter)
                          .setKeysOnly();
        return datastore.prepare(q).countEntities(FetchOptions.Builder.withLimit(1)) > 0;
    }

    /**
     * Determines if any entities match the provided query.
     *
     * @param query The query to test.
     * @return true if entities match the query.
     */
    public static boolean hasEntities(Query query) {
        DatastoreService datastore = DatastoreServiceFactory.getDatastoreService();
        FetchOptions limitOne = FetchOptions.Builder.withLimit(1);
        return datastore.prepare(query).countEntities(limitOne) > 0;
    }

    /**
     * Get all of the target product names.
     *
     * @return a list of all device product names.
     */
    public static List<String> getAllProducts() {
        DatastoreService datastore = DatastoreServiceFactory.getDatastoreService();
        Query query = new Query(DeviceInfoEntity.KIND)
                              .addProjection(new PropertyProjection(
                                      DeviceInfoEntity.PRODUCT, String.class))
                              .setDistinct(true);
        List<String> devices = new ArrayList<>();
        for (Entity e : datastore.prepare(query).asIterable()) {
            devices.add((String) e.getProperty(DeviceInfoEntity.PRODUCT));
        }
        return devices;
    }

    /**
     * Upload data from a test report message
     *
     * @param report The test report containing data to upload.
     */
    public static void insertTestReport(TestReportMessage report) {
        DatastoreService datastore = DatastoreServiceFactory.getDatastoreService();
        List<Entity> puts = new ArrayList<>();

        if (!report.hasStartTimestamp() || !report.hasEndTimestamp() || !report.hasTest()
                || !report.hasHostInfo() || !report.hasBuildInfo()) {
            // missing information
            return;
        }
        long startTimestamp = report.getStartTimestamp();
        long endTimestamp = report.getEndTimestamp();
        String testName = report.getTest().toStringUtf8();
        String testBuildId = report.getBuildInfo().getId().toStringUtf8();
        String hostName = report.getHostInfo().getHostname().toStringUtf8();

        Entity testEntity = new TestEntity(testName).toEntity();
        List<Long> testCaseIds = new ArrayList<>();

        Key testRunKey = KeyFactory.createKey(
                testEntity.getKey(), TestRunEntity.KIND, report.getStartTimestamp());

        long passCount = 0;
        long failCount = 0;
        long coveredLineCount = 0;
        long totalLineCount = 0;

        List<TestCaseRunEntity> testCases = new ArrayList<>();

        // Process test cases
        for (TestCaseReportMessage testCase : report.getTestCaseList()) {
            String testCaseName = testCase.getName().toStringUtf8();
            TestCaseResult result = testCase.getTestResult();
            // Track global pass/fail counts
            if (result == TestCaseResult.TEST_CASE_RESULT_PASS) {
                ++passCount;
            } else if (result != TestCaseResult.TEST_CASE_RESULT_SKIP) {
                ++failCount;
            }
            String systraceLink = null;
            if (testCase.getSystraceCount() > 0
                    && testCase.getSystraceList().get(0).getUrlCount() > 0) {
                systraceLink = testCase.getSystraceList().get(0).getUrl(0).toStringUtf8();
            }
            // Process coverage data for test case
            for (CoverageReportMessage coverage : testCase.getCoverageList()) {
                CoverageEntity coverageEntity =
                        CoverageEntity.fromCoverageReport(testRunKey, testCaseName, coverage);
                if (coverageEntity == null) {
                    logger.log(Level.WARNING, "Invalid coverage report in test run " + testRunKey);
                    continue;
                }
                coveredLineCount += coverageEntity.coveredLineCount;
                totalLineCount += coverageEntity.totalLineCount;
                puts.add(coverageEntity.toEntity());
            }
            // Process profiling data for test case
            for (ProfilingReportMessage profiling : testCase.getProfilingList()) {
                ProfilingPointRunEntity profilingEntity =
                        ProfilingPointRunEntity.fromProfilingReport(testRunKey, profiling);
                if (profilingEntity == null) {
                    logger.log(Level.WARNING, "Invalid profiling report in test run " + testRunKey);
                }
                puts.add(profilingEntity.toEntity());
            }

            int lastIndex = testCases.size() - 1;
            if (lastIndex < 0 || testCases.get(lastIndex).isFull()) {
                KeyRange keys = datastore.allocateIds(TestCaseRunEntity.KIND, 1);
                testCaseIds.add(keys.getStart().getId());
                testCases.add(new TestCaseRunEntity(keys.getStart()));
                ++lastIndex;
            }
            TestCaseRunEntity testCaseEntity = testCases.get(lastIndex);
            testCaseEntity.addTestCase(testCaseName, result.getNumber());
            testCaseEntity.setSystraceUrl(systraceLink);
        }
        List<Entity> testCasePuts = new ArrayList<>();
        for (TestCaseRunEntity testCaseEntity : testCases) {
            testCasePuts.add(testCaseEntity.toEntity());
        }
        datastore.put(testCasePuts);

        // Process device information
        TestRunType testRunType = null;
        for (AndroidDeviceInfoMessage device : report.getDeviceInfoList()) {
            DeviceInfoEntity deviceInfoEntity =
                    DeviceInfoEntity.fromDeviceInfoMessage(testRunKey, device);
            if (deviceInfoEntity == null) {
                logger.log(Level.WARNING, "Invalid device info in test run " + testRunKey);
            }

            // Run type on devices must be the same, else set to OTHER
            TestRunType runType = TestRunType.fromBuildId(deviceInfoEntity.buildId);
            if (testRunType == null) {
                testRunType = runType;
            } else if (runType != testRunType) {
                testRunType = TestRunType.OTHER;
            }
            puts.add(deviceInfoEntity.toEntity());
        }

        // Overall run type should be determined by the device builds unless test build is OTHER
        if (testRunType == null) {
            testRunType = TestRunType.fromBuildId(testBuildId);
        } else if (TestRunType.fromBuildId(testBuildId) == TestRunType.OTHER) {
            testRunType = TestRunType.OTHER;
        }

        // Process global coverage data
        for (CoverageReportMessage coverage : report.getCoverageList()) {
            CoverageEntity coverageEntity =
                    CoverageEntity.fromCoverageReport(testRunKey, new String(), coverage);
            if (coverageEntity == null) {
                logger.log(Level.WARNING, "Invalid coverage report in test run " + testRunKey);
                continue;
            }
            coveredLineCount += coverageEntity.coveredLineCount;
            totalLineCount += coverageEntity.totalLineCount;
            puts.add(coverageEntity.toEntity());
        }

        // Process global profiling data
        for (ProfilingReportMessage profiling : report.getProfilingList()) {
            ProfilingPointRunEntity profilingEntity =
                    ProfilingPointRunEntity.fromProfilingReport(testRunKey, profiling);
            if (profilingEntity == null) {
                logger.log(Level.WARNING, "Invalid profiling report in test run " + testRunKey);
            }
            puts.add(profilingEntity.toEntity());
        }

        List<String> logLinks = new ArrayList<>();
        // Process log data
        for (LogMessage log : report.getLogList()) {
            if (!log.hasUrl())
                continue;
            logLinks.add(log.getUrl().toStringUtf8());
        }

        TestRunEntity testRunEntity = new TestRunEntity(testEntity.getKey(), testRunType,
                startTimestamp, endTimestamp, testBuildId, hostName, passCount, failCount,
                testCaseIds, logLinks, coveredLineCount, totalLineCount);
        puts.add(testRunEntity.toEntity());

        Transaction txn = datastore.beginTransaction();
        try {
            // Check if test already exists in the database
            try {
                datastore.get(testEntity.getKey());
            } catch (EntityNotFoundException e) {
                puts.add(testEntity);
            }
            datastore.put(puts);
            txn.commit();
        } finally {
            if (txn.isActive()) {
                logger.log(
                        Level.WARNING, "Transaction rollback forced for run: " + testRunEntity.key);
                txn.rollback();
            }
        }
    }

    /**
     * Upload data from a test plan report message
     *
     * @param report The test plan report containing data to upload.
     */
    public static void insertTestPlanReport(TestPlanReportMessage report) {
        DatastoreService datastore = DatastoreServiceFactory.getDatastoreService();
        List<Entity> puts = new ArrayList<>();

        List<String> testModules = report.getTestModuleNameList();
        List<Long> testTimes = report.getTestModuleStartTimestampList();
        if (testModules.size() != testTimes.size() || !report.hasTestPlanName()) {
            logger.log(Level.WARNING, "TestPlanReportMessage is missing information.");
            return;
        }

        String testPlanName = report.getTestPlanName();
        Entity testPlanEntity = new TestPlanEntity(testPlanName).toEntity();
        List<Key> testRunKeys = new ArrayList<>();
        for (int i = 0; i < testModules.size(); i++) {
            String test = testModules.get(i);
            long time = testTimes.get(i);
            Key parentKey = KeyFactory.createKey(TestEntity.KIND, test);
            Key testRunKey = KeyFactory.createKey(parentKey, TestRunEntity.KIND, time);
            testRunKeys.add(testRunKey);
        }
        Map<Key, Entity> testRuns = datastore.get(testRunKeys);
        long passCount = 0;
        long failCount = 0;
        long startTimestamp = -1;
        String testBuildId = null;
        TestRunType type = null;
        Set<DeviceInfoEntity> devices = new HashSet<>();
        for (Key testRunKey : testRuns.keySet()) {
            TestRunEntity testRun = TestRunEntity.fromEntity(testRuns.get(testRunKey));
            if (testRun == null) {
                continue; // not a valid test run
            }
            passCount += testRun.passCount;
            failCount += testRun.failCount;
            if (startTimestamp < 0 || testRunKey.getId() < startTimestamp) {
                startTimestamp = testRunKey.getId();
            }
            if (type == null) {
                type = testRun.type;
            } else if (type != testRun.type) {
                type = TestRunType.OTHER;
            }
            testBuildId = testRun.testBuildId;
            Query deviceInfoQuery = new Query(DeviceInfoEntity.KIND).setAncestor(testRunKey);
            for (Entity deviceInfoEntity : datastore.prepare(deviceInfoQuery).asIterable()) {
                DeviceInfoEntity device = DeviceInfoEntity.fromEntity(deviceInfoEntity);
                if (device == null) {
                    continue; // invalid entity
                }
                devices.add(device);
            }
        }
        if (startTimestamp < 0 || testBuildId == null || type == null) {
            logger.log(Level.WARNING, "Couldn't infer test run information from runs.");
            return;
        }
        TestPlanRunEntity testPlanRun = new TestPlanRunEntity(testPlanEntity.getKey(), testPlanName,
                type, startTimestamp, testBuildId, passCount, failCount, testRunKeys);

        // Create the device infos.
        for (DeviceInfoEntity device : devices) {
            puts.add(device.copyWithParent(testPlanRun.key).toEntity());
        }
        puts.add(testPlanRun.toEntity());

        Transaction txn = datastore.beginTransaction();
        try {
            // Check if test already exists in the database
            try {
                datastore.get(testPlanEntity.getKey());
            } catch (EntityNotFoundException e) {
                puts.add(testPlanEntity);
            }
            datastore.put(puts);
            txn.commit();
        } finally {
            if (txn.isActive()) {
                logger.log(Level.WARNING,
                        "Transaction rollback forced for plan run: " + testPlanRun.key);
                txn.rollback();
            }
        }
    }
}
