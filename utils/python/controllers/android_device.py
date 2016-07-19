#!/usr/bin/env python3.4
#
#   Copyright 2016 - The Android Open Source Project
#
#   Licensed under the Apache License, Version 2.0 (the "License");
#   you may not use this file except in compliance with the License.
#   You may obtain a copy of the License at
#
#       http://www.apache.org/licenses/LICENSE-2.0
#
#   Unless required by applicable law or agreed to in writing, software
#   distributed under the License is distributed on an "AS IS" BASIS,
#   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
#   See the License for the specific language governing permissions and
#   limitations under the License.

from builtins import str
from builtins import open

import logging
import os
import time
import traceback
import threading
import socket
import Queue

from vts.runners.host import logger as vts_logger
from vts.runners.host import signals
from vts.runners.host import utils
from vts.utils.python.controllers import adb
from vts.utils.python.controllers import event_dispatcher
from vts.utils.python.controllers import fastboot
from vts.runners.host.tcp_client import vts_tcp_client
from vts.utils.python.mirror import hal_mirror
from vts.utils.python.mirror import shell_mirror
from vts.utils.python.mirror import lib_mirror
from vts.runners.host import errors
import subprocess

VTS_CONTROLLER_CONFIG_NAME = "AndroidDevice"
VTS_CONTROLLER_REFERENCE_NAME = "android_devices"

ANDROID_DEVICE_PICK_ALL_TOKEN = "*"
# Key name for adb logcat extra params in config file.
ANDROID_DEVICE_ADB_LOGCAT_PARAM_KEY = "adb_logcat_param"
ANDROID_DEVICE_EMPTY_CONFIG_MSG = "Configuration is empty, abort!"
ANDROID_DEVICE_NOT_LIST_CONFIG_MSG = "Configuration should be a list, abort!"

# Target-side directory where the VTS binaries are uploaded
DEFAULT_AGENT_BASE_DIR = "/data/local/tmp"
# Time for which the current is put on sleep when the client is unable to
# make a connection.
THREAD_SLEEP_TIME = 1
# Max number of attempts that the client can make to connect to the agent
MAX_AGENT_CONNECT_RETRIES = 10


class AndroidDeviceError(signals.ControllerError):
    pass


def create(configs):
    if not configs:
        raise AndroidDeviceError(ANDROID_DEVICE_EMPTY_CONFIG_MSG)
    elif configs == ANDROID_DEVICE_PICK_ALL_TOKEN:
        ads = get_all_instances()
    elif not isinstance(configs, list):
        raise AndroidDeviceError(ANDROID_DEVICE_NOT_LIST_CONFIG_MSG)
    elif isinstance(configs[0], str):
        # Configs is a list of serials.
        ads = get_instances(configs)
    else:
        # Configs is a list of dicts.
        ads = get_instances_with_configs(configs)
    connected_ads = list_adb_devices()

    for ad in ads:
        if ad.serial not in connected_ads:
            raise AndroidDeviceError(
                ("Android device %s is specified in config"
                 " but is not attached.") % ad.serial)
        ad.startAdbLogcat()
        ad.startVtsAgent()
    return ads


def destroy(ads):
    for ad in ads:
        ad.stopVtsAgent()
        if ad.adb_logcat_process:
            ad.stopAdbLogcat()


def _parse_device_list(device_list_str, key):
    """Parses a byte string representing a list of devices. The string is
    generated by calling either adb or fastboot.

    Args:
        device_list_str: Output of adb or fastboot.
        key: The token that signifies a device in device_list_str.

    Returns:
        A list of android device serial numbers.
    """
    clean_lines = str(device_list_str, 'utf-8').strip().split('\n')
    results = []
    for line in clean_lines:
        tokens = line.strip().split('\t')
        if len(tokens) == 2 and tokens[1] == key:
            results.append(tokens[0])
    return results


def list_adb_devices():
    """List all target devices connected to the host and detected by adb.

    Returns:
        A list of android device serials. Empty if there's none.
    """
    out = adb.AdbProxy().devices()
    return _parse_device_list(out, "device")


def list_fastboot_devices():
    """List all android devices connected to the computer that are in in
    fastboot mode. These are detected by fastboot.

    Returns:
        A list of android device serials. Empty if there's none.
    """
    out = fastboot.FastbootProxy().devices()
    return _parse_device_list(out, "fastboot")


def get_instances(serials):
    """Create AndroidDevice instances from a list of serials.

    Args:
        serials: A list of android device serials.

    Returns:
        A list of AndroidDevice objects.
    """
    results = []
    for s in serials:
        results.append(AndroidDevice(s))
    return results


def get_instances_with_configs(configs):
    """Create AndroidDevice instances from a list of json configs.

    Each config should have the required key-value pair "serial".

    Args:
        configs: A list of dicts each representing the configuration of one
            android device.

    Returns:
        A list of AndroidDevice objects.
    """
    results = []
    for c in configs:
        try:
            serial = c.pop("serial")
        except KeyError:
            raise AndroidDeviceError(('Required value "serial" is missing in '
                                      'AndroidDevice config %s.') % c)
        ad = AndroidDevice(serial)
        ad.loadConfig(c)
        results.append(ad)
    return results


def get_all_instances(include_fastboot=False):
    """Create AndroidDevice instances for all attached android devices.

    Args:
        include_fastboot: Whether to include devices in bootloader mode or not.

    Returns:
        A list of AndroidDevice objects each representing an android device
        attached to the computer.
    """
    if include_fastboot:
        serial_list = list_adb_devices() + list_fastboot_devices()
        return get_instances(serial_list)
    return get_instances(list_adb_devices())


def filter_devices(ads, func):
    """Finds the AndroidDevice instances from a list that match certain
    conditions.

    Args:
        ads: A list of AndroidDevice instances.
        func: A function that takes an AndroidDevice object and returns True
            if the device satisfies the filter condition.

    Returns:
        A list of AndroidDevice instances that satisfy the filter condition.
    """
    results = []
    for ad in ads:
        if func(ad):
            results.append(ad)
    return results


def get_device(ads, **kwargs):
    """Finds a unique AndroidDevice instance from a list that has specific
    attributes of certain values.

    Example:
        get_device(android_devices, label="foo", phone_number="1234567890")
        get_device(android_devices, model="angler")

    Args:
        ads: A list of AndroidDevice instances.
        kwargs: keyword arguments used to filter AndroidDevice instances.

    Returns:
        The target AndroidDevice instance.

    Raises:
        AndroidDeviceError is raised if none or more than one device is
        matched.
    """

    def _get_device_filter(ad):
        for k, v in kwargs.items():
            if not hasattr(ad, k):
                return False
            elif getattr(ad, k) != v:
                return False
        return True

    filtered = filter_devices(ads, _get_device_filter)
    if not filtered:
        raise AndroidDeviceError(("Could not find a target device that matches"
                                  " condition: %s.") % kwargs)
    elif len(filtered) == 1:
        return filtered[0]
    else:
        serials = [ad.serial for ad in filtered]
        raise AndroidDeviceError("More than one device matched: %s" % serials)


def takeBugReports(ads, test_name, begin_time):
    """Takes bug reports on a list of android devices.

    If you want to take a bug report, call this function with a list of
    android_device objects in on_fail. But reports will be taken on all the
    devices in the list concurrently. Bug report takes a relative long
    time to take, so use this cautiously.

    Args:
        ads: A list of AndroidDevice instances.
        test_name: Name of the test case that triggered this bug report.
        begin_time: Logline format timestamp taken when the test started.
    """
    begin_time = vts_logger.normalizeLogLineTimestamp(begin_time)

    def take_br(test_name, begin_time, ad):
        ad.takeBugReport(test_name, begin_time)

    args = [(test_name, begin_time, ad) for ad in ads]
    utils.concurrent_exec(take_br, args)


class AndroidDevice(object):
    """Class representing an android device.

    Each object of this class represents one Android device in ACTS, including
    handles to adb, fastboot, and sl4a clients. In addition to direct adb
    commands, this object also uses adb port forwarding to talk to the Android
    device.

    Attributes:
        serial: A string that's the serial number of the Androi device.
        device_command_port: int, the port number used on the Android device
                for adb port forwarding (for command-response sessions).
        device_callback_port: int, the port number used on the Android device
                for adb port reverse forwarding (for callback sessions).
        log: A logger project with a device-specific prefix for each line -
             [AndroidDevice|<serial>]
        log_path: A string that is the path where all logs collected on this
                  android device should be stored.
        adb_logcat_process: A process that collects the adb logcat.
        adb_logcat_file_path: A string that's the full path to the adb logcat
                              file collected, if any.
        vts_agent_process: A process that runs the HAL agent.
        adb: An AdbProxy object used for interacting with the device via adb.
        fastboot: A FastbootProxy object used for interacting with the device
                  via fastboot.
        host_command_port: the host-side port for runner to agent sessions
                           (to send commands and receive responses).
        host_callback_port: the host-side port for agent to runner sessions
                            (to get callbacks from agent).
    """

    def __init__(self, serial="", device_port=5001, device_callback_port=5010):
        self.serial = serial
        self.device_command_port = device_port
        self.device_callback_port = device_callback_port
        self.log = AndroidDeviceLoggerAdapter(logging.getLogger(),
                                              {"serial": self.serial})
        base_log_path = getattr(logging, "log_path", "/tmp/logs/")
        self.log_path = os.path.join(base_log_path, "AndroidDevice%s" % serial)
        self.adb_logcat_process = None
        self.adb_logcat_file_path = None
        self.vts_agent_process = None
        self.adb = adb.AdbProxy(serial)
        self.fastboot = fastboot.FastbootProxy(serial)
        if not self.isBootloaderMode:
            self.rootAdb()
        self.host_command_port = adb.get_available_host_port()
        self.host_callback_port = adb.get_available_host_port()
        self.adb.tcp_forward(self.host_command_port, self.device_command_port)
        self.adb.reverse_tcp_forward(self.device_callback_port,
                                     self.host_callback_port)
        self.hal = hal_mirror.HalMirror(self.host_command_port,
                                        self.host_callback_port)
        self.lib = lib_mirror.LibMirror(self.host_command_port)
        self.shell = shell_mirror.ShellMirror(self.host_command_port)

    def __del__(self):
        if self.host_command_port:
            self.adb.forward("--remove tcp:%s" % self.host_command_port)
        if self.adb_logcat_process:
            self.stopAdbLogcat()

    @property
    def isBootloaderMode(self):
        """True if the device is in bootloader mode."""
        return self.serial in list_fastboot_devices()

    @property
    def isAdbRoot(self):
        """True if adb is running as root for this device."""
        id_str = self.adb.shell("id -u").decode("utf-8")
        self.log.info(id_str)
        return "root" in id_str

    @property
    def model(self):
        """The Android code name for the device."""
        # If device is in bootloader mode, get mode name from fastboot.
        if self.isBootloaderMode:
            out = self.fastboot.getvar("product").strip()
            # "out" is never empty because of the "total time" message fastboot
            # writes to stderr.
            lines = out.decode("utf-8").split('\n', 1)
            if lines:
                tokens = lines[0].split(' ')
                if len(tokens) > 1:
                    return tokens[1].lower()
            return None
        out = self.adb.shell('getprop | grep ro.build.product')
        model = out.decode("utf-8").strip().split('[')[-1][:-1].lower()
        if model == "sprout":
            return model
        else:
            out = self.adb.shell('getprop | grep ro.product.name')
            model = out.decode("utf-8").strip().split('[')[-1][:-1].lower()
            return model

    @property
    def isAdbLogcatOn(self):
        """Whether there is an ongoing adb logcat collection.
        """
        if self.adb_logcat_process:
            return True
        return False

    def loadConfig(self, config):
        """Add attributes to the AndroidDevice object based on json config.

        Args:
            config: A dictionary representing the configs.

        Raises:
            AndroidDeviceError is raised if the config is trying to overwrite
            an existing attribute.
        """
        for k, v in config.items():
            if hasattr(self, k):
                raise AndroidDeviceError(
                    "Attempting to set existing attribute %s on %s" %
                    (k, self.serial))
            setattr(self, k, v)

    def rootAdb(self):
        """Changes adb to root mode for this device."""
        if not self.isAdbRoot:
            try:
                self.adb.root()
                self.adb.wait_for_device()
                self.adb.remount()
                self.adb.wait_for_device()
            except adb.AdbError as e:
                # adb wait-for-device is not always possible in the lab
                # continue with an assumption it's done by the harness.
                logging.exception(e)

    def startAdbLogcat(self):
        """Starts a standing adb logcat collection in separate subprocesses and
        save the logcat in a file.
        """
        if self.isAdbLogcatOn:
            raise AndroidDeviceError(("Android device %s already has an adb "
                                      "logcat thread going on. Cannot start "
                                      "another one.") % self.serial)
        f_name = "adblog,%s,%s.txt" % (self.model, self.serial)
        utils.create_dir(self.log_path)
        logcat_file_path = os.path.join(self.log_path, f_name)
        try:
            extra_params = self.adb_logcat_param
        except AttributeError:
            extra_params = "-b all"
        cmd = "adb -s %s logcat -v threadtime %s >> %s" % (
            self.serial, extra_params, logcat_file_path)
        self.adb_logcat_process = utils.start_standing_subprocess(cmd)
        self.adb_logcat_file_path = logcat_file_path

    def stopAdbLogcat(self):
        """Stops the adb logcat collection subprocess.
        """
        if not self.isAdbLogcatOn:
            raise AndroidDeviceError(
                "Android device %s does not have an ongoing adb logcat collection."
                % self.serial)
        utils.stop_standing_subprocess(self.adb_logcat_process)
        self.adb_logcat_process = None

    def takeBugReport(self, test_name, begin_time):
        """Takes a bug report on the device and stores it in a file.

        Args:
            test_name: Name of the test case that triggered this bug report.
            begin_time: Logline format timestamp taken when the test started.
        """
        br_path = os.path.join(self.log_path, "BugReports")
        utils.create_dir(br_path)
        base_name = ",%s,%s.txt" % (begin_time, self.serial)
        test_name_len = utils.MAX_FILENAME_LEN - len(base_name)
        out_name = test_name[:test_name_len] + base_name
        full_out_path = os.path.join(br_path, out_name.replace(' ', '\ '))
        self.log.info("Taking bugreport for %s on %s", test_name, self.serial)
        self.adb.bugreport(" > %s" % full_out_path)
        self.log.info("Bugreport for %s taken at %s", test_name, full_out_path)

    @utils.timeout(15 * 60)
    def waitForBootCompletion(self):
        """Waits for Android framework to broadcast ACTION_BOOT_COMPLETED.

        This function times out after 15 minutes.
        """
        try:
            self.adb.wait_for_device()
        except adb.AdbError as e:
            # adb wait-for-device is not always possible in the lab
            logging.exception(e)
        while True:
            try:
                out = self.adb.shell("getprop sys.boot_completed")
                completed = out.decode('utf-8').strip()
                if completed == '1':
                    return
            except adb.AdbError:
                # adb shell calls may fail during certain period of booting
                # process, which is normal. Ignoring these errors.
                pass
            time.sleep(5)

    def reboot(self):
        """Reboots the device and wait for device to complete booting.

        This is probably going to print some error messages in console. Only
        use if there's no other option.

        Raises:
            AndroidDeviceError is raised if waiting for completion timed
            out.
        """
        if self.isBootloaderMode:
            self.fastboot.reboot()
            return
        has_adb_log = self.isAdbLogcatOn
        has_vts_agent = True if self.vts_agent_process else False
        if has_adb_log:
            self.stopAdbLogcat()
        if has_vts_agent:
            self.stopVtsAgent()
        self.adb.reboot()
        self.waitForBootCompletion()
        self.rootAdb()
        if has_adb_log:
            self.startAdbLogcat()
        if has_vts_agent:
            self.startVtsAgent()

    def startVtsAgent(self):
        """Start HAL agent on the AndroidDevice.

        This function starts the target side native agent and is persisted
        throughout the test run.
        """
        self.log.info("start a VTS agent")
        if self.vts_agent_process:
            raise AndroidDeviceError("HAL agent is already running on %s." %
                                     self.serial)

        cleanup_commands = [
            "rm -f /data/local/tmp/vts_driver_*",
            "rm -f /data/local/tmp/vts_agent_callback*"]
        kill_commands = ["killall vts_hal_agent", "killall fuzzer32",
                         "killall fuzzer64", "killall vts_shell_driver32",
                         "killall vts_shell_driver64"]
        cleanup_commands.extend(kill_commands)
        chmod_commands = [
            "chmod 755 %s/64/vts_hal_agent" % DEFAULT_AGENT_BASE_DIR,
            "chmod 755 %s/32/fuzzer32" % DEFAULT_AGENT_BASE_DIR,
            "chmod 755 %s/64/fuzzer64" % DEFAULT_AGENT_BASE_DIR,
            "chmod 755 %s/32/vts_shell_driver32" % DEFAULT_AGENT_BASE_DIR,
            "chmod 755 %s/64/vts_shell_driver64" % DEFAULT_AGENT_BASE_DIR]
        cleanup_commands.extend(chmod_commands)
        for cmd in cleanup_commands:
            try:
                self.adb.shell(cmd)
            except adb.AdbError as e:
                self.log.warning(
                        "A command to setup the env to start the VTS Agent failed %s", e)
        vts_agent_log_path = os.path.join(self.log_path, "vts_agent.log")
        cmd = (
            'adb -s {s} shell LD_LIBRARY_PATH={path}/64 {path}/64/vts_hal_agent'
            ' {path}/32/fuzzer32 {path}/64/fuzzer64 {path}/spec'
            ' {path}/32/vts_shell_driver32 {path}/64/vts_shell_driver64 >> {log}'
        ).format(s=self.serial,
                 path=DEFAULT_AGENT_BASE_DIR,
                 log=vts_agent_log_path)
        self.vts_agent_process = utils.start_standing_subprocess(
            cmd, check_health_delay=1)

    def stopVtsAgent(self):
        """Stop the HAL agent running on the AndroidDevice.
        """
        if self.vts_agent_process:
            utils.stop_standing_subprocess(self.vts_agent_process)
            self.vts_agent_process = None


class AndroidDeviceLoggerAdapter(logging.LoggerAdapter):
    """A wrapper class that attaches a prefix to all log lines from an
    AndroidDevice object.
    """

    def process(self, msg, kwargs):
        """Process every log message written via the wrapped logger object.

        We are adding the prefix "[AndroidDevice|<serial>]" to all log lines.

        Args:
            msg: string, the original log message.
            kwargs: dict, the key value pairs that can be used to modify the
                    original log message.
        """
        msg = "[AndroidDevice|%s] %s" % (self.extra["serial"], msg)
        return (msg, kwargs)
