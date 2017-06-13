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
import com.android.vts.entity.TestPlanEntity;
import com.android.vts.entity.TestPlanRunEntity;
import com.android.vts.util.DatastoreHelper;
import com.android.vts.util.FilterUtil;
import com.google.appengine.api.datastore.DatastoreService;
import com.google.appengine.api.datastore.DatastoreServiceFactory;
import com.google.appengine.api.datastore.Entity;
import com.google.appengine.api.datastore.FetchOptions;
import com.google.appengine.api.datastore.Key;
import com.google.appengine.api.datastore.KeyFactory;
import com.google.appengine.api.datastore.Query;
import com.google.appengine.api.datastore.Query.Filter;
import com.google.appengine.api.datastore.Query.SortDirection;
import com.google.gson.Gson;
import com.google.gson.JsonObject;
import com.google.gson.JsonPrimitive;
import java.io.IOException;
import java.util.ArrayList;
import java.util.Collections;
import java.util.List;
import java.util.concurrent.TimeUnit;
import java.util.logging.Level;
import java.util.Set;
import java.util.HashSet;
import javax.servlet.RequestDispatcher;
import javax.servlet.ServletException;
import javax.servlet.http.HttpServletRequest;
import javax.servlet.http.HttpServletResponse;
import org.apache.commons.lang.StringUtils;

public class ShowPlanReleaseServlet extends BaseServlet {
    private static final String PLAN_RELEASE_JSP = "WEB-INF/jsp/show_plan_release.jsp";
    private static final int MAX_RUNS_PER_PAGE = 90;

    @Override
    public PageType getNavParentType() {
        return PageType.RELEASE;
    }

    @Override
    public List<Page> getBreadcrumbLinks(HttpServletRequest request) {
        List<Page> links = new ArrayList<>();
        String planName = request.getParameter("plan");
        links.add(new Page(PageType.PLAN_RELEASE, planName.toUpperCase(), "?plan=" + planName));
        return links;
    }

    /**
     * Model to describe each test plan run .
     */
    private class TestPlanRunMetadata implements Comparable<TestPlanRunMetadata> {
        public final TestPlanRunEntity testPlanRun;
        public final List<String> devices;
        public final Set<DeviceInfoEntity> deviceSet;

        public TestPlanRunMetadata(TestPlanRunEntity testPlanRun) {
            this.testPlanRun = testPlanRun;
            this.devices = new ArrayList<>();
            this.deviceSet = new HashSet<>();
        }

        public void addDevice(DeviceInfoEntity device) {
            if (device == null || deviceSet.contains(device))
                return;
            devices.add(device.branch + "/" + device.buildFlavor + " (" + device.buildId + ")");
            deviceSet.add(device);
        }

        public JsonObject toJson() {
            JsonObject obj = new JsonObject();
            obj.add("testPlanRun", testPlanRun.toJson());
            obj.add("deviceInfo", new JsonPrimitive(StringUtils.join(devices, ", ")));
            return obj;
        }

        @Override
        public int compareTo(TestPlanRunMetadata o) {
            return new Long(o.testPlanRun.startTimestamp)
                    .compareTo(this.testPlanRun.startTimestamp);
        }
    }

    @Override
    public void doGetHandler(HttpServletRequest request, HttpServletResponse response)
            throws IOException {
        DatastoreService datastore = DatastoreServiceFactory.getDatastoreService();
        Long startTime = null; // time in microseconds
        Long endTime = null; // time in microseconds
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
        SortDirection dir = SortDirection.DESCENDING;
        if (startTime != null && endTime == null) {
            dir = SortDirection.ASCENDING;
        }
        String testPlan = request.getParameter("plan");
        Key testPlanKey = KeyFactory.createKey(TestPlanEntity.KIND, testPlan);
        Filter testPlanRunFilter =
                FilterUtil.getTimeFilter(testPlanKey, TestPlanRunEntity.KIND, startTime, endTime);

        Query testPlanRunQuery = new Query(TestPlanRunEntity.KIND)
                                         .setAncestor(testPlanKey)
                                         .setFilter(testPlanRunFilter)
                                         .addSort(Entity.KEY_RESERVED_PROPERTY, dir);

        List<TestPlanRunMetadata> testPlanRuns = new ArrayList<>();
        for (Entity testPlanRunEntity :
                datastore.prepare(testPlanRunQuery)
                        .asIterable(FetchOptions.Builder.withLimit(MAX_RUNS_PER_PAGE))) {
            TestPlanRunEntity testPlanRun = TestPlanRunEntity.fromEntity(testPlanRunEntity);
            if (testPlanRun == null) {
                logger.log(Level.WARNING, "Invalid test plan run: " + testPlanRunEntity.getKey());
                continue;
            }
            TestPlanRunMetadata metadata = new TestPlanRunMetadata(testPlanRun);
            testPlanRuns.add(metadata);
            Query deviceInfoQuery = new Query(DeviceInfoEntity.KIND).setAncestor(testPlanRun.key);
            for (Entity deviceInfoEntity : datastore.prepare(deviceInfoQuery).asIterable()) {
                DeviceInfoEntity device = DeviceInfoEntity.fromEntity(deviceInfoEntity);
                metadata.addDevice(device);
            }
        }
        Collections.sort(testPlanRuns);

        if (testPlanRuns.size() > 0) {
            TestPlanRunMetadata firstRun = testPlanRuns.get(0);
            endTime = firstRun.testPlanRun.startTimestamp;

            TestPlanRunMetadata lastRun = testPlanRuns.get(testPlanRuns.size() - 1);
            startTime = lastRun.testPlanRun.startTimestamp;
        }

        List<JsonObject> testPlanRunObjects = new ArrayList<>();
        for (TestPlanRunMetadata metadata : testPlanRuns) {
            testPlanRunObjects.add(metadata.toJson());
        }

        request.setAttribute("plan", request.getParameter("plan"));
        request.setAttribute("hasNewer", new Gson().toJson(DatastoreHelper.hasNewer(
                                                 testPlanKey, TestPlanRunEntity.KIND, endTime)));
        request.setAttribute("hasOlder", new Gson().toJson(DatastoreHelper.hasOlder(
                                                 testPlanKey, TestPlanRunEntity.KIND, startTime)));
        request.setAttribute("planRuns", new Gson().toJson(testPlanRunObjects));
        request.setAttribute("startTime", new Gson().toJson(startTime));
        request.setAttribute("endTime", new Gson().toJson(endTime));
        response.setStatus(HttpServletResponse.SC_OK);
        RequestDispatcher dispatcher = request.getRequestDispatcher(PLAN_RELEASE_JSP);
        try {
            dispatcher.forward(request, response);
        } catch (ServletException e) {
            logger.log(Level.SEVERE, "Servlet Excpetion caught : ", e);
        }
    }
}
