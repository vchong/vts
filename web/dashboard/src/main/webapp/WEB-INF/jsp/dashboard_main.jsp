<%--
  ~ Copyright (c) 2016 Google Inc. All Rights Reserved.
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
  <link rel='stylesheet' href='/css/dashboard_main.css'>
  <%@ include file='header.jsp' %>
  <script type='text/javascript' src='https://ajax.googleapis.com/ajax/libs/jqueryui/1.12.0/jquery-ui.min.js'></script>
  <body>
    <script>
        var allTests = ${allTestsJson};
        var testSet = new Set(allTests);
        var subscriptionMap = ${subscriptionMapJson};

        var addFavorite = function() {
            if ($(this).hasClass('disabled')) {
                return;
            }
            var test = $('#input-box').val();
            if (!testSet.has(test) || test in subscriptionMap) {
                return;
            }
            $('#add-button').addClass('disabled');
            $.post('/api/favorites/' + test).then(function(data) {
                if (!data.key) {
                    return;
                }
                subscriptionMap[test] = data.key;
                var wrapper = $('<div></div>');
                var a = $('<a></a>')
                    .attr('href', '/show_table?testName=' + test);
                var div = $('<div class="col s11 card hoverable option"></div>');
                div.addClass('valign-wrapper waves-effect');
                div.appendTo(a);
                var span = $('<span class="entry valign"></span>').text(test);
                span.appendTo(div);
                a.appendTo(wrapper);
                var clear = $('<a class="col s1 btn-flat center"></a>');
                clear.addClass('clear-button');
                clear.append('<i class="material-icons">clear</i>');
                clear.attr('test', test);
                clear.appendTo(wrapper);
                clear.click(removeFavorite);
                wrapper.prependTo('#options').hide()
                                          .slideDown(150);
                $('#input-box').val(null);
                Materialize.updateTextFields();
            }).always(function() {
                $('#add-button').removeClass('disabled');
            });
        }

        var removeFavorite = function() {
            var self = $(this);
            if (self.hasClass('disabled')) {
                return;
            }
            var test = self.attr('test');
            if (!(test in subscriptionMap)) {
                return;
            }
            self.addClass('disabled');
            $.ajax({
                url: '/api/favorites/' + subscriptionMap[test],
                type: 'DELETE'
            }).always(function() {
                self.removeClass('disabled');
            }).then(function() {
                delete subscriptionMap[test];
                self.parent().slideUp(150, function() {
                    self.remove();
                });
            });
        }

        $.widget('custom.sizedAutocomplete', $.ui.autocomplete, {
            _resizeMenu: function() {
                this.menu.element.outerWidth($('#input-box').width());
            }
        });

        $(function() {
            $('#input-box').sizedAutocomplete({
                source: allTests,
                classes: {
                    'ui-autocomplete': 'card'
                }
            });

            $('#input-box').keyup(function(event) {
                if (event.keyCode == 13) {  // return button
                    $('#add-button').click();
                }
            });

            $('.clear-button').click(removeFavorite);
            $('#add-button').click(addFavorite);
        });
    </script>
    <div class='container'>
      <c:choose>
        <c:when test='${not empty error}'>
          <div id='error-container' class='row card'>
            <div class='col s12 center-align'>
              <h5>${error}</h5>
            </div>
          </div>
        </c:when>
        <c:otherwise>
          <c:set var='width' value='${showAll ? 12 : 11}' />
          <c:if test='${not showAll}'>
            <div class='row'>
              <div class='input-field col s8'>
                <input type='text' id='input-box'></input>
                <label for='input-box'>Search for tests to add to favorites</label>
              </div>
              <div id='add-button-wrapper' class='col s1 valign-wrapper'>
                <a id='add-button' class='btn-floating btn waves-effect waves-light red valign'><i class='material-icons'>add</i></a>
              </div>
            </div>
          </c:if>
          <div class='row'>
            <div class='col s12'>
              <h4 id='section-header'>${headerLabel}</h4>
            </div>
          </div>
          <div class='row' id='options'>
            <c:forEach items='${testNames}' var='test'>
              <div>
                <a href='/show_table?testName=${test.name}'>
                  <div class='col s${width} card hoverable option valign-wrapper waves-effect'>
                    <span class='entry valign'>${test.name}
                      <c:if test='${test.failCount >= 0 && test.passCount >= 0}'>
                        <c:set var='color' value='${test.failCount > 0 ? "red" : (test.passCount > 0 ? "green" : "grey")}' />
                        <span class='indicator center ${color}'>
                          ${test.passCount} / ${test.passCount + test.failCount}
                        </span>
                      </c:if>
                    </span>
                  </div>
                </a>
                <c:if test='${not showAll}'>
                  <a class='col s1 btn-flat center clear-button' test='${test.name}'>
                    <i class='material-icons'>clear</i>
                  </a>
                </c:if>
              </div>
            </c:forEach>
          </div>
        </c:otherwise>
      </c:choose>
    </div>
    <c:if test='${empty error}'>
      <div class='center'>
        <a href='${buttonLink}' id='show-button' class='btn waves-effect red'>${buttonLabel}
          <i id='show-button-arrow' class='material-icons right'>${buttonIcon}</i>
        </a>
      </div>
    </c:if>
    <%@ include file='footer.jsp' %>
  </body>
</html>
