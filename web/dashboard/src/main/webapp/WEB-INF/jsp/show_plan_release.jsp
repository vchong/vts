<%--
  ~ Copyright (c) 2017 Google Inc. All Rights Reserved.
  ~
  ~ Licensed under the Apache License, Version 2.0 (the "License"); you
  ~ may not use this file except in compliance with the License. You may
  ~ obtain a copy of the License at
  ~
  ~     http://www.apache.org/licenses/LICENSE-2.0
  ~
  ~ Unless required by applicable law or agreed to in writing, software
  ~ distributed under the License is distributed on an "AS IS" BASIS,
  ~ WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or
  ~ implied. See the License for the specific language governing
  ~ permissions and limitations under the License.
  --%>
<%@ page contentType='text/html;charset=UTF-8' language='java' %>
<%@ taglib prefix='fn' uri='http://java.sun.com/jsp/jstl/functions' %>
<%@ taglib prefix='c' uri='http://java.sun.com/jsp/jstl/core'%>

<html>
  <%@ include file="header.jsp" %>
  <link rel='stylesheet' href='/css/show_plan_release.css'>
  <link rel='stylesheet' href='/css/plan_runs.css'>
  <script src='https://www.gstatic.com/external_hosted/moment/min/moment-with-locales.min.js'></script>
  <script src='js/time.js'></script>
  <script src='js/plan_runs.js'></script>
  <script type='text/javascript'>
      $(document).ready(function() {
          // disable buttons on load
          if (!${hasNewer}) {
              $('#newer-button').toggleClass('disabled');
          }
          if (!${hasOlder}) {
              $('#older-button').toggleClass('disabled');
          }

          $('#newer-button').click(prev);
          $('#older-button').click(next);
          $('#release-container').showPlanRuns(${planRuns});
      });

      // view older data
      function next() {
          if($(this).hasClass('disabled')) return;
          var endTime = ${startTime};
          var link = '${pageContext.request.contextPath}' +
              '/show_plan_release?plan=${plan}&endTime=' + endTime;
          window.open(link,'_self');
      }

      // view newer data
      function prev() {
          if($(this).hasClass('disabled')) return;
          var startTime = ${endTime};
          var link = '${pageContext.request.contextPath}' +
              '/show_plan_release?plan=${plan}&startTime=' + startTime;
          window.open(link,'_self');
        }
  </script>

  <body>
    <div class='wide container'>
      <h4 id='section-header'>${plan}</h4>
      <div class='row' id='release-container'></div>
      <div id='newer-wrapper' class='page-button-wrapper fixed-action-btn'>
        <a id='newer-button' class='btn-floating btn red waves-effect'>
          <i class='large material-icons'>keyboard_arrow_left</i>
        </a>
      </div>
      <div id='older-wrapper' class='page-button-wrapper fixed-action-btn'>
        <a id='older-button' class='btn-floating btn red waves-effect'>
          <i class='large material-icons'>keyboard_arrow_right</i>
        </a>
      </div>
    </div>
    <%@ include file="footer.jsp" %>
  </body>
</html>
