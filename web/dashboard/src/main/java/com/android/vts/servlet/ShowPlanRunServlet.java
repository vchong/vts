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

import com.android.vts.entity.TestPlanEntity;
import com.android.vts.entity.TestPlanRunEntity;
import com.android.vts.entity.TestRunEntity;
import com.android.vts.proto.VtsReportMessage.TestCaseResult;
import com.android.vts.util.TestRunMetadata;
import com.google.appengine.api.datastore.DatastoreService;
import com.google.appengine.api.datastore.DatastoreServiceFactory;
import com.google.appengine.api.datastore.Entity;
import com.google.appengine.api.datastore.EntityNotFoundException;
import com.google.appengine.api.datastore.Key;
import com.google.appengine.api.datastore.KeyFactory;
import com.google.gson.Gson;
import com.google.gson.JsonObject;
import java.io.IOException;
import java.util.ArrayList;
import java.util.Collections;
import java.util.HashSet;
import java.util.List;
import java.util.Map;
import java.util.Set;
import java.util.logging.Level;
import javax.servlet.RequestDispatcher;
import javax.servlet.ServletException;
import javax.servlet.http.HttpServletRequest;
import javax.servlet.http.HttpServletResponse;

/** Servlet for handling requests to load individual plan runs. */
public class ShowPlanRunServlet extends BaseServlet {
    private static final String PLAN_RUN_JSP = "WEB-INF/jsp/show_plan_run.jsp";
    private static final String PROFILING_DATA_ALERT = "No profiling data was found.";

    @Override
    public PageType getNavParentType() {
        return PageType.RELEASE;
    }

    @Override
    public List<Page> getBreadcrumbLinks(HttpServletRequest request) {
        List<Page> links = new ArrayList<>();
        String planName = request.getParameter("plan");
        links.add(new Page(PageType.PLAN_RELEASE, planName.toUpperCase(), "?plan=" + planName));

        String time = request.getParameter("time");
        links.add(new Page(PageType.PLAN_RUN, "?plan=" + planName + "&time=" + time));
        return links;
    }

    @Override
    public void doGetHandler(HttpServletRequest request, HttpServletResponse response)
            throws IOException {
        Long startTime = null; // time in microseconds
        DatastoreService datastore = DatastoreServiceFactory.getDatastoreService();
        RequestDispatcher dispatcher = null;

        String plan = request.getParameter("plan");

        if (request.getParameter("time") != null) {
            String time = request.getParameter("time");
            try {
                startTime = Long.parseLong(time);
                startTime = startTime > 0 ? startTime : null;
            } catch (NumberFormatException e) {
                startTime = null;
            }
        }

        // Add result names to list
        List<String> resultNames = new ArrayList<>();
        for (TestCaseResult r : TestCaseResult.values()) {
            resultNames.add(r.name());
        }

        List<TestRunMetadata> testRunMetadata = new ArrayList<>();
        List<JsonObject> testRunObjects = new ArrayList<>();

        Key planKey = KeyFactory.createKey(TestPlanEntity.KIND, plan);
        Key planRunKey = KeyFactory.createKey(planKey, TestPlanRunEntity.KIND, startTime);
        int passCount = 0;
        int failCount = 0;
        try {
            Entity testPlanRunEntity = datastore.get(planRunKey);
            TestPlanRunEntity testPlanRun = TestPlanRunEntity.fromEntity(testPlanRunEntity);
            Map<Key, Entity> testRuns = datastore.get(testPlanRun.testRuns);
            passCount = (int) testPlanRun.passCount;
            failCount = (int) testPlanRun.failCount;

            for (Key key : testPlanRun.testRuns) {
                if (!testRuns.containsKey(key))
                    continue;
                TestRunEntity testRunEntity = TestRunEntity.fromEntity(testRuns.get(key));
                if (testRunEntity == null)
                    continue;
                TestRunMetadata metadata =
                        new TestRunMetadata(key.getParent().getName(), testRunEntity);
                testRunMetadata.add(metadata);
                testRunObjects.add(metadata.toJson());
            }
        } catch (EntityNotFoundException e) {
            // Invalid parameters
        }

        int[] topBuildResultCounts = new int[TestCaseResult.values().length];
        topBuildResultCounts = new int[TestCaseResult.values().length];
        topBuildResultCounts[TestCaseResult.TEST_CASE_RESULT_PASS.getNumber()] = passCount;
        topBuildResultCounts[TestCaseResult.TEST_CASE_RESULT_FAIL.getNumber()] = failCount;

        Set<String> profilingPoints = new HashSet<>();

        String profilingDataAlert = "";
        if (profilingPoints.size() == 0) {
            profilingDataAlert = PROFILING_DATA_ALERT;
        }
        List<String> profilingPointNames = new ArrayList<>(profilingPoints);
        Collections.sort(profilingPointNames);

        request.setAttribute("plan", request.getParameter("plan"));
        request.setAttribute("time", request.getParameter("time"));

        request.setAttribute("error", profilingDataAlert);

        request.setAttribute("profilingPointNames", profilingPointNames);
        request.setAttribute("resultNames", resultNames);
        request.setAttribute("resultNamesJson", new Gson().toJson(resultNames));
        request.setAttribute("testRuns", new Gson().toJson(testRunObjects));

        // data for pie chart
        request.setAttribute("topBuildResultCounts", new Gson().toJson(topBuildResultCounts));

        dispatcher = request.getRequestDispatcher(PLAN_RUN_JSP);
        try {
            dispatcher.forward(request, response);
        } catch (ServletException e) {
            logger.log(Level.SEVERE, "Servlet Exception caught : " + e.toString());
        }
    }
}
