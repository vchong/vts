#
# Copyright (C) 2016 The Android Open Source Project
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#
"""This module is where all the test signal classes and related utilities live.
"""

import functools
import json


def GeneratedTest(func):
    """A decorator used to suppress result reporting for the test case that
    kicks off a group of generated test cases.

    Returns:
        What the decorated function returns.
    """

    @functools.wraps(func)
    def wrapper(*args, **kwargs):
        func(*args, **kwargs)
        raise TestSilent("Result reporting for %s is suppressed" %
                         func.__name__)

    return wrapper


class TestSignalError(Exception):
    """Raised when an error occurs inside a test signal."""


class TestSignal(Exception):
    """Base class for all test result control signals."""

    def __init__(self, details, extras=None):
        if not isinstance(details, str):
            raise TestSignalError("Message has to be a string.")
        super(TestSignal, self).__init__(details)
        self.details = details
        try:
            json.dumps(extras)
            self.extras = extras
        except TypeError:
            raise TestSignalError(("Extras must be json serializable. %s "
                                   "is not.") % extras)

    def __str__(self):
        return "Details=%s, Extras=%s" % (self.details, self.extras)


class TestFailure(TestSignal):
    """Raised when a test has failed."""


class TestPass(TestSignal):
    """Raised when a test has passed."""


class TestSkip(TestSignal):
    """Raised when a test has been skipped."""


class TestSilent(TestSignal):
    """Raised when a test should not be reported. This should only be used for
    generated test cases.
    """


class TestAbortClass(TestSignal):
    """Raised when all subsequent test cases within the same test class should
    be aborted.
    """


class TestAbortAll(TestSignal):
    """Raised when all subsequent test cases should be aborted."""


class ControllerError(Exception):
    """Raised when an error occured in controller classes."""
