#
# Copyright (C) 2017 The Android Open Source Project
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

import logging
import os
import subprocess

from vts.runners.host import asserts
from vts.runners.host import base_test_with_webdb
from vts.runners.host import keys
from vts.runners.host import test_runner


class HostBinaryTest(base_test_with_webdb.BaseTestWithWebDbClass):
    """Base class to run a host-side, native binary test.

    Note that a host-side binary test is not highly recommended because
    such can be written in Python. Currently, this is used only for legacy
    host-side native tests. And all new host-side native tests should be
    written in Python or Java.
    """

    def setUpClass(self):
        """Retrieves the required param."""
        required_params = [
            keys.ConfigKeys.IKEY_BINARY_TEST_SOURCES,
        ]
        self.getUserParams(req_param_names=required_params)

    def testHostBinary(self):
        """Tests host-side binaries."""
        android_build_top = os.getenv("ANDROID_BUILD_TOP", "")
        asserts.assertTrue(
            android_build_top,
            "$ANDROID_BUILD_TOP is not set. Please run lunch <build target>")

        for binary_test_source in self.binary_test_sources:
            binary_test_source = str(binary_test_source)
            binary_path = os.path.join(android_build_top, binary_test_source)

            p = subprocess.Popen(
                [binary_path],
                stdout=subprocess.PIPE, stderr=subprocess.PIPE)
            cmd_stdout, cmd_stderr = p.communicate()

            asserts.assertEqual(
                0, p.returncode,
                "Test failed with the following results:\n "
                "stdout: %s\n stdedrr: %s" % (cmd_stdout, cmd_stderr))


if __name__ == "__main__":
    test_runner.main()
