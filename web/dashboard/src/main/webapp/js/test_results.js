/**
 * Copyright (c) 2017 Google Inc. All Rights Reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); you
 * may not use this file except in compliance with the License. You may
 * obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or
 * implied. See the License for the specific language governing
 * permissions and limitations under the License.
 */

(function ($) {

  /**
   * Renders a timestamp in the user timezone.
   * @param timestamp The long timestamp to render (in microseconds).
   * @param showTimezone True if the timezone should be rendered, false otherwise.
   * @returns the string-formatted version of the provided timestamp.
   */
  function renderTime(timestamp, showTimezone) {
    var time = moment(timestamp / 1000);
    var format = 'H:mm:ss';
    if (!time.isSame(moment(), 'd')) {
        format = 'M/D/YY ' + format;
    }
    if (!!showTimezone) {
        format = format + 'ZZ';
    }
    return time.format(format);
  }

  /**
   * Renders a duration in the user timezone.
   * @param durationTimestamp The long duration to render (in microseconds).
   * @returns the string-formatted duration of the provided duration timestamp.
   */
  function renderDuration(durationTimestamp) {
    var fmt = 's[s]';
    var duration = moment.utc(durationTimestamp / 1000);
    if (duration.hours() > 0) {
      fmt = 'H[h], m[m], ' + fmt;
    } else if (duration.minutes() > 0) {
      fmt = 'm[m], ' + fmt;
    }
    return duration.format(fmt);
  }

  /**
   * Display the log links in a modal window.
   * @param logList A list of [name, url] tuples representing log links.
   */
  function showLogs(container, logList) {
    if (!logList || logList.length == 0) return;

    var logCollection = $('<ul class="collection"></ul>');
    var entries = logList.reduce(function(acc, entry) {
      if (!entry || entry.length == 0) return acc;
      var link = '<a href="' + entry[1] + '"';
      link += 'class="collection-item">' + entry[0] + '</li>';
      return acc + link;
    }, '');
    logCollection.html(entries);

    if (container.find('#info-modal').length == 0) {
      var modal = $('<div id="info-modal" class="modal"></div>');
      var content = $('<div class="modal-content"></div>');
      content.append('<h4>Logs</h4>');
      content.append('<div class="info-container"></div>');
      content.appendTo(modal);
      modal.appendTo(container);
    }
    var infoContainer = $('#info-modal>.modal-content>.info-container');
    infoContainer.empty();
    logCollection.appendTo(infoContainer);
    $('#info-modal').openModal();
  }

  /**
   * Get the nickname for a test case result.
   *
   * Removes the result prefix and suffix, extracting only the result name.
   *
   * @param testCaseResult The string name of a VtsReportMessage.TestCaseResult.
   * @returns the string nickname of the result.
   */
  function getNickname(testCaseResult) {
    return testCaseResult
      .replace('TEST_CASE_RESULT_', '')
      .replace('_RESULT', '')
      .trim().toLowerCase();
  }

  /**
   * Display test data in the body beneath a test run's metadata.
   * @param container The jquery object in which to insert the test metadata.
   * @param data The json object containing the columns to display.
   * @param lineHeight The height of each list element.
   */
  function displayTestDetails(container, data, lineHeight) {
    var nCol = data.length;
    var width = 12 / nCol;
    test = container;
    var maxLines = 0;
    data.forEach(function (column) {
      if (column.data == undefined || column.name == undefined) {
        return;
      }
      var colContainer =
        $('<div class="col s' + width + ' test-col"></div>');
      var col = $('<div class="test-case-container"></div>');
      colContainer.appendTo(container);
      var count = column.data.length;
      $('<h5>' + getNickname(column.name) + ' (' + count + ')' + '</h5>')
        .appendTo(colContainer).css('text-transform', 'capitalize');
      col.appendTo(colContainer);
      var list = $('<ul></ul>').appendTo(col);
      column.data.forEach(function (testCase) {
        $('<li></li>')
          .text(testCase)
          .addClass('test-case')
          .css('font-size', lineHeight - 2)
          .css('line-height', lineHeight + 'px')
          .appendTo(list);
      });
      if (count > maxLines) {
        maxLines = count;
      }
    });
    var containers = container.find('.test-case-container');
    containers.height(maxLines * lineHeight);
  }

  /**
   * Click handler for displaying test run details.
   * @param e The click event.
   */
  function testRunClick(e) {
    var header = $(this);
    var icon = header.find('.material-icons.expand-arrow');
    var container = header.parent().find('.test-results');
    var test = header.attr('test');
    var time = header.attr('time');
    var url = '/api/test_run?test=' + test + '&timestamp=' + time;
    if (header.parent().hasClass('active')) {
      header.parent().removeClass('active');
      header.removeClass('active');
      icon.removeClass('rotate');
      header.siblings('.collapsible-body').stop(true, false).slideUp({
        duration: 100,
        easing: "easeOutQuart",
        queue: false,
        complete: function() { header.css('height', ''); }
      });
    } else {
      container.empty();
      header.parent().addClass('active');
      header.addClass('active');
      header.addClass('disabled');
      icon.addClass('rotate');
      $.get(url).done(function(data) {
        displayTestDetails(container, data, 16);
        header.siblings('.collapsible-body').stop(true, false).slideDown({
          duration: 100,
          easing: "easeOutQuart",
          queue: false,
          complete: function() { header.css('height', ''); }
        });
      }).fail(function() {
        icon.removeClass('rotate');
      }).always(function() {
        header.removeClass('disabled');
      });
    }
  }

  /**
   * Append a clickable indicator link to the container.
   * @param container The jquery object to append the indicator to.
   * @param content The text to display in the indicator.
   * @param classes Additional space-delimited classes to add to the indicator.
   * @param click The click handler to assign to the indicator.
   * @returns The jquery object for the indicator.
   */
  function createClickableIndicator(container, content, classes, click) {
    var link = $('<a></a>');
    link.addClass('indicator right center padded hoverable waves-effect');
    link.addClass(classes)
    link.append(content);
    link.appendTo(container);
    link.click(click);
    return link;
  }

  /**
   * Display test metadata in a vertical popout.
   * @param container The jquery object in which to insert the test metadata.
   * @param metadataList The list of metadata objects to render on the display.
   */
  function displayTestMetadata(container, metadataList) {
    var popout = $('<ul></ul>');
    popout.attr('data-collapsible', 'expandable');
    popout.addClass('collapsible popout test-runs');
    popout.appendTo(container);
    popout.unbind();
    metadataList.forEach(function (metadata) {
      var li = $('<li class="test-run-container"></li>');
      li.appendTo(popout);
      var div = $('<div></div>');
      var test = metadata.testRun.testName;
      var startTime = metadata.testRun.startTimestamp;
      var endTime = metadata.testRun.endTimestamp;
      div.attr('test', test);
      div.attr('time', startTime);
      div.addClass('collapsible-header test-run');
      div.appendTo(li);
      div.unbind().click(testRunClick);
      var span = $('<span></span>');
      span.addClass('test-run-metadata');
      span.appendTo(div);
      span.click(function() { return false; });
      $('<b></b>').text(metadata.deviceInfo).appendTo(span);
      span.append('<br>');
      $('<b></b>').text('ABI: ')
            .appendTo(span)
      span.append(metadata.abiInfo).append('<br>');
      $('<b></b>').text('VTS Build: ')
            .appendTo(span)
      span.append(metadata.testRun.testBuildId).append('<br>');
      $('<b></b>').text('Host: ')
            .appendTo(span)
      span.append(metadata.testRun.hostName).append('<br>');
      var timeString = (
        renderTime(startTime, false) + ' - ' +
        renderTime(endTime, true) + ' (' +
        renderDuration(endTime - startTime) + ')');
      span.append(timeString);
      var indicator = $('<span></span>');
      var color = metadata.testRun.failCount > 0 ? 'red' : 'green';
      indicator.addClass('indicator right center ' + color);
      indicator.append(
        metadata.testRun.passCount + '/' +
        (metadata.testRun.passCount + metadata.testRun.failCount));
      indicator.appendTo(div);
      if (metadata.testRun.coveredLineCount != undefined &&
        metadata.testRun.totalLineCount != undefined) {
        var url = (
          '/show_coverage?testName=' + test + '&startTime=' + startTime);
        covered = metadata.testRun.coveredLineCount;
        total = metadata.testRun.totalLineCount;
        covPct = Math.round(covered / total * 1000) / 10;
        var color = covPct < 70 ? 'red' : 'green';
        var coverage = (
          'Coverage: ' + covered + '/' + total + ' (' + covPct + '%)');
        createClickableIndicator(
          div, coverage, color,
          function () { window.location.href = url; return false; });
      }
      if (metadata.testRun.logLinks != undefined) {
        createClickableIndicator(
          div, 'Logs', 'grey lighten-1',
          function () {
            showLogs(popout, metadata.testRun.logLinks);
            return false;
          });
      }
      var expand = $('<i></i>');
      expand.addClass('material-icons expand-arrow')
      expand.text('expand_more');
      expand.appendTo(div);
      var body = $('<div></div>')
        .addClass('collapsible-body test-results row grey lighten-4')
        .appendTo(li);
      if (metadata.testDetails != undefined) {
        expand.addClass('rotate');
        li.addClass('active');
        div.addClass('active');
        displayTestDetails(body, metadata.testDetails, 16);
        div.siblings('.collapsible-body').stop(true, false).slideDown({
          duration: 0,
          queue: false,
          complete: function() { div.css('height', ''); }
        });
      }
    });
  }

  $.fn.showTests = function(metadataList) {
    displayTestMetadata($(this), metadataList);
  }

})(jQuery);
