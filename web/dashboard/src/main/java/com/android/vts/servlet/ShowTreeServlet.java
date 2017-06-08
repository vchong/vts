/*
 * Copyright (c) 2017 Google Inc. All Rights Reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); you
 * may not use this file except in compliance with the License. You may
 * obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or
 * implied. See the License for the specific language governing
 * permissions and limitations under the License.
 */

package com.android.vts.servlet;

import com.android.vts.entity.DeviceInfoEntity;
import com.android.vts.entity.ProfilingPointRunEntity;
import com.android.vts.entity.TestCaseRunEntity;
import com.android.vts.entity.TestEntity;
import com.android.vts.entity.TestRunEntity;
import com.android.vts.proto.VtsReportMessage.TestCaseResult;
import com.android.vts.util.DatastoreHelper;
import com.android.vts.util.FilterUtil;
import com.android.vts.util.TestRunDetails;
import com.android.vts.util.TestRunMetadata;
import com.google.appengine.api.datastore.DatastoreService;
import com.google.appengine.api.datastore.DatastoreServiceFactory;
import com.google.appengine.api.datastore.Entity;
import com.google.appengine.api.datastore.FetchOptions;
import com.google.appengine.api.datastore.Key;
import com.google.appengine.api.datastore.KeyFactory;
import com.google.appengine.api.datastore.Query;
import com.google.appengine.api.datastore.Query.CompositeFilterOperator;
import com.google.appengine.api.datastore.Query.Filter;
import com.google.appengine.api.datastore.Query.SortDirection;
import com.google.gson.Gson;
import com.google.gson.JsonObject;
import java.io.IOException;
import java.util.ArrayList;
import java.util.Collections;
import java.util.Comparator;
import java.util.HashSet;
import java.util.List;
import java.util.Map;
import java.util.Set;
import java.util.logging.Level;
import javax.servlet.RequestDispatcher;
import javax.servlet.ServletException;
import javax.servlet.http.HttpServletRequest;
import javax.servlet.http.HttpServletResponse;
import org.apache.commons.lang.StringUtils;

/** Servlet for handling requests to load individual tables. */
public class ShowTreeServlet extends BaseServlet {
    private static final String TABLE_JSP = "WEB-INF/jsp/show_tree.jsp";
    // Error message displayed on the webpage is tableName passed is null.
    private static final String TABLE_NAME_ERROR = "Error : Table name must be passed!";
    private static final String PROFILING_DATA_ALERT = "No profiling data was found.";
    private static final int MAX_BUILD_IDS_PER_PAGE = 20;
    private static final int MAX_PREFETCH_COUNT = 10;

    private static final String SEARCH_HELP_HEADER = "Search Help";
    private static final String SEARCH_HELP = "Data can be queried using one or more filters. "
            + "If more than one filter is provided, results will be returned that match <i>all</i>. "
            + "<br><br>Filters are delimited by spaces; to specify a multi-word token, enclose it in "
            + "double quotes. A query must be in the format: \"field:value\".<br><br>"
            + "<b>Supported field qualifiers:</b> "
            + StringUtils.join(FilterUtil.FilterKey.values(), ", ") + ".";

    @Override
    public List<String[]> getNavbarLinks(HttpServletRequest request) {
        List<String[]> links = new ArrayList<>();
        Page root = Page.HOME;
        String[] rootEntry = new String[] {root.getUrl(), root.getName()};
        links.add(rootEntry);

        Page table = Page.TABLE;
        String testName = request.getParameter("testName");
        String name = table.getName() + testName;
        String url = table.getUrl() + "?testName=" + testName;
        String[] tableEntry = new String[] {url, name};
        links.add(tableEntry);
        return links;
    }

    /**
     * Get the test run details for a test run.
     * @param metadata The metadata for the test run whose details will be fetched.
     * @return The TestRunDetails object for the provided test run.
     */
    public static TestRunDetails processTestDetails(TestRunMetadata metadata) {
        DatastoreService datastore = DatastoreServiceFactory.getDatastoreService();
        TestRunDetails details = new TestRunDetails();
        List<Key> gets = new ArrayList<>();
        for (long testCaseId : metadata.testRun.testCaseIds) {
            gets.add(KeyFactory.createKey(TestCaseRunEntity.KIND, testCaseId));
        }
        Map<Key, Entity> entityMap = datastore.get(gets);
        for (int i = 0; i < 1; i++) {
            for (Key key : entityMap.keySet()) {
                TestCaseRunEntity testCaseRun = TestCaseRunEntity.fromEntity(entityMap.get(key));
                if (testCaseRun == null) {
                    continue;
                }
                details.addTestCase(testCaseRun);
            }
        }
        return details;
    }

    @Override
    public void doGetHandler(HttpServletRequest request, HttpServletResponse response)
            throws IOException {
        boolean unfiltered = request.getParameter("unfiltered") != null;
        boolean showPresubmit = request.getParameter("showPresubmit") != null;
        boolean showPostsubmit = request.getParameter("showPostsubmit") != null;
        String searchString = request.getParameter("search");
        Long startTime = null; // time in microseconds
        Long endTime = null; // time in microseconds
        DatastoreService datastore = DatastoreServiceFactory.getDatastoreService();
        RequestDispatcher dispatcher = null;

        // message to display if profiling point data is not available
        String profilingDataAlert = "";

        if (request.getParameter("testName") == null) {
            request.setAttribute("testName", TABLE_NAME_ERROR);
            return;
        }
        String testName = request.getParameter("testName");

        if (request.getParameter("startTime") != null) {
            String time = request.getParameter("startTime");
            try {
                startTime = Long.parseLong(time);
                startTime = startTime > 0 ? startTime : null;
            } catch (NumberFormatException e) {
                startTime = null;
            }
        }
        if (request.getParameter("endTime") != null) {
            String time = request.getParameter("endTime");
            try {
                endTime = Long.parseLong(time);
                endTime = endTime > 0 ? endTime : null;
            } catch (NumberFormatException e) {
                endTime = null;
            }
        }

        // If no params are specified, set to default of postsubmit-only.
        if (!(showPresubmit || showPostsubmit)) {
            showPostsubmit = true;
        }

        // If unfiltered, set showPre- and Post-submit to true for accurate UI.
        if (unfiltered) {
            showPostsubmit = true;
            showPresubmit = true;
        }

        // Add result names to list
        List<String> resultNames = new ArrayList<>();
        for (TestCaseResult r : TestCaseResult.values()) {
            resultNames.add(r.name());
        }

        SortDirection dir = SortDirection.DESCENDING;
        if (startTime != null && endTime == null) {
            dir = SortDirection.ASCENDING;
        }
        Key testKey = KeyFactory.createKey(TestEntity.KIND, testName);
        Filter userTestFilter = FilterUtil.getUserTestFilter(searchString);
        Filter userDeviceFilter = FilterUtil.getDeviceFilter(searchString);

        Filter typeFilter = FilterUtil.getTestTypeFilter(showPresubmit, showPostsubmit, unfiltered);
        Filter testFilter = FilterUtil.getTimeFilter(testKey, startTime, endTime, typeFilter);

        List<TestRunMetadata> testRunMetadata = new ArrayList<>();
        if (userTestFilter == null && userDeviceFilter == null) {
            Query testRunQuery = new Query(TestRunEntity.KIND)
                                         .setAncestor(testKey)
                                         .setFilter(testFilter)
                                         .addSort(Entity.KEY_RESERVED_PROPERTY, dir);
            for (Entity testRun :
                    datastore.prepare(testRunQuery)
                            .asIterable(FetchOptions.Builder.withLimit(MAX_BUILD_IDS_PER_PAGE))) {
                TestRunEntity testRunEntity = TestRunEntity.fromEntity(testRun);
                if (testRunEntity == null) {
                    continue;
                }
                TestRunMetadata metadata = new TestRunMetadata(testName, testRunEntity);
                testRunMetadata.add(metadata);
            }
        } else {
            List<Key> gets = new ArrayList<>();
            Set<Key> matchingTestKeys = new HashSet<>();
            if (userTestFilter != null) {
                testFilter = CompositeFilterOperator.and(userTestFilter, testFilter);
            }
            Query testRunQuery = new Query(TestRunEntity.KIND)
                                         .setAncestor(testKey)
                                         .setFilter(testFilter)
                                         .setKeysOnly();
            for (Entity testRunKey : datastore.prepare(testRunQuery).asIterable()) {
                matchingTestKeys.add(testRunKey.getKey());
            }

            Set<Key> allMatchingKeys;
            if (userDeviceFilter == null) {
                allMatchingKeys = matchingTestKeys;
            } else {
                allMatchingKeys = new HashSet<>();
                Query deviceQuery = new Query(DeviceInfoEntity.KIND)
                                            .setAncestor(testKey)
                                            .setFilter(userDeviceFilter)
                                            .setKeysOnly();
                for (Entity device : datastore.prepare(deviceQuery).asIterable()) {
                    if (matchingTestKeys.contains(device.getKey().getParent())) {
                        allMatchingKeys.add(device.getKey().getParent());
                    }
                }
            }
            List<Key> allKeysSorted = new ArrayList<>(allMatchingKeys);
            if (dir == SortDirection.DESCENDING) {
                Collections.sort(allKeysSorted, Collections.reverseOrder());
            } else {
                Collections.sort(allKeysSorted);
            }
            allKeysSorted = allKeysSorted.subList(
                    0, Math.min(allKeysSorted.size(), MAX_BUILD_IDS_PER_PAGE));
            for (Key key : allKeysSorted) {
                gets.add(key);
            }
            Map<Key, Entity> entityMap = datastore.get(gets);
            for (Key key : allKeysSorted) {
                if (!entityMap.containsKey(key)) {
                    continue;
                }
                TestRunEntity testRunEntity = TestRunEntity.fromEntity(entityMap.get(key));
                if (testRunEntity == null) {
                    continue;
                }
                TestRunMetadata metadata = new TestRunMetadata(testName, testRunEntity);
                testRunMetadata.add(metadata);
            }
        }

        Comparator<TestRunMetadata> comparator = new Comparator<TestRunMetadata>() {
            @Override
            public int compare(TestRunMetadata t1, TestRunMetadata t2) {
                return new Long(t2.testRun.startTimestamp).compareTo(t1.testRun.startTimestamp);
            }
        };
        Collections.sort(testRunMetadata, comparator);
        List<JsonObject> testRunObjects = new ArrayList<>();

        int prefetchCount = 0;
        for (TestRunMetadata metadata : testRunMetadata) {
            if (metadata.testRun.failCount > 0 && prefetchCount < MAX_PREFETCH_COUNT) {
                // process
                metadata.addDetails(processTestDetails(metadata));
                ++prefetchCount;
            }
            testRunObjects.add(metadata.toJson());
        }

        int[] topBuildResultCounts = null;
        String topBuild = "";
        if (testRunMetadata.size() > 0) {
            TestRunMetadata firstRun = testRunMetadata.get(0);
            topBuild = firstRun.getDeviceInfo();
            endTime = firstRun.testRun.startTimestamp;
            TestRunDetails topDetails = firstRun.getDetails();
            if (topDetails == null) {
                topDetails = processTestDetails(firstRun);
            }
            topBuildResultCounts = topDetails.resultCounts;

            TestRunMetadata lastRun = testRunMetadata.get(testRunMetadata.size() - 1);
            startTime = lastRun.testRun.startTimestamp;
        }

        Set<String> profilingPoints = new HashSet<>();
        Query profilingPointQuery =
                new Query(ProfilingPointRunEntity.KIND).setAncestor(testKey).setKeysOnly();
        for (Entity e : datastore.prepare(profilingPointQuery).asIterable()) {
            profilingPoints.add(e.getKey().getName());
        }

        if (profilingPoints.size() == 0) {
            profilingDataAlert = PROFILING_DATA_ALERT;
        }
        List<String> profilingPointNames = new ArrayList<>(profilingPoints);
        Collections.sort(profilingPointNames);

        request.setAttribute("testName", request.getParameter("testName"));

        request.setAttribute("error", profilingDataAlert);
        request.setAttribute("searchString", searchString);
        request.setAttribute("searchHelpHeader", SEARCH_HELP_HEADER);
        request.setAttribute("searchHelpBody", SEARCH_HELP);

        request.setAttribute("profilingPointNames", profilingPointNames);
        request.setAttribute("resultNames", resultNames);
        request.setAttribute("resultNamesJson", new Gson().toJson(resultNames));
        request.setAttribute("testRuns", new Gson().toJson(testRunObjects));

        // data for pie chart
        request.setAttribute("topBuildResultCounts", new Gson().toJson(topBuildResultCounts));
        request.setAttribute("topBuildId", topBuild);
        request.setAttribute("startTime", new Gson().toJson(startTime));
        request.setAttribute("endTime", new Gson().toJson(endTime));
        request.setAttribute("hasNewer",
                new Gson().toJson(DatastoreHelper.hasNewer(testKey, TestRunEntity.KIND, endTime)));
        request.setAttribute("hasOlder", new Gson().toJson(DatastoreHelper.hasOlder(
                                                 testKey, TestRunEntity.KIND, startTime)));
        request.setAttribute("unfiltered", unfiltered);
        request.setAttribute("showPresubmit", showPresubmit);
        request.setAttribute("showPostsubmit", showPostsubmit);

        dispatcher = request.getRequestDispatcher(TABLE_JSP);
        try {
            dispatcher.forward(request, response);
        } catch (ServletException e) {
            logger.log(Level.SEVERE, "Servlet Exception caught : " + e.toString());
        }
    }
}
