#!/bin/bash
#
# Copyright 2016 The Android Open Source Project
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

function vts_multidevice_target_setup {
  DEVICE=$1

  pushd ${ANDROID_BUILD_TOP}
  echo "hidl-gen defaults"
  hidl-gen -o ${ANDROID_BUILD_TOP}/output -L vts -r android.hardware:hardware/interfaces -r android.hidl:system/libhidl/transport android.hardware.camera.provider@2.4
  hidl-gen -o ${ANDROID_BUILD_TOP}/output -L vts -r android.hardware:hardware/interfaces -r android.hidl:system/libhidl/transport android.hardware.camera.common@1.0
  hidl-gen -o ${ANDROID_BUILD_TOP}/output -L vts -r android.hardware:hardware/interfaces -r android.hidl:system/libhidl/transport android.hardware.camera.device@1.0
  hidl-gen -o ${ANDROID_BUILD_TOP}/output -L vts -r android.hardware:hardware/interfaces -r android.hidl:system/libhidl/transport android.hardware.camera.device@3.2
  hidl-gen -o ${ANDROID_BUILD_TOP}/output -L vts -r android.hardware:hardware/interfaces -r android.hidl:system/libhidl/transport android.hardware.camera.metadata@3.2
  hidl-gen -o ${ANDROID_BUILD_TOP}/output -L vts -r android.hardware:hardware/interfaces -r android.hidl:system/libhidl/transport android.hardware.gnss@1.0
  hidl-gen -o ${ANDROID_BUILD_TOP}/output -L vts -r android.hardware:hardware/interfaces -r android.hidl:system/libhidl/transport android.hardware.nfc@1.0
  hidl-gen -o ${ANDROID_BUILD_TOP}/output -L vts -r android.hardware:hardware/interfaces -r android.hidl:system/libhidl/transport android.hardware.vr@1.0
  hidl-gen -o ${ANDROID_BUILD_TOP}/output -L vts -r android.hardware:hardware/interfaces -r android.hidl:system/libhidl/transport android.hardware.automotive.vehicle@2.0
  hidl-gen -o ${ANDROID_BUILD_TOP}/output -L vts -r android.hardware:hardware/interfaces -r android.hidl:system/libhidl/transport android.hardware.automotive.vehicle@2.1
  hidl-gen -o ${ANDROID_BUILD_TOP}/output -L vts -r android.hardware:hardware/interfaces -r android.hidl:system/libhidl/transport android.hardware.sensors@1.0
  hidl-gen -o ${ANDROID_BUILD_TOP}/output -L vts -r android.hardware:hardware/interfaces -r android.hidl:system/libhidl/transport android.hardware.tv.cec@1.0
  hidl-gen -o ${ANDROID_BUILD_TOP}/output -L vts -r android.hardware:hardware/interfaces -r android.hidl:system/libhidl/transport android.hardware.vibrator@1.0
  hidl-gen -o ${ANDROID_BUILD_TOP}/output -L vts -r android.hardware:hardware/interfaces -r android.hidl:system/libhidl/transport android.hardware.contexthub@1.0
  echo "hidl-gen km gk vts"
  hidl-gen -o ${ANDROID_BUILD_TOP}/output -L vts -r android.hardware:hardware/interfaces -r android.hidl:system/libhidl/transport android.hardware.keymaster@3.0
  hidl-gen -o ${ANDROID_BUILD_TOP}/output -L vts -r android.hardware:hardware/interfaces -r android.hidl:system/libhidl/transport android.hardware.gatekeeper@1.0
  popd
}

vts_multidevice_target_setup $1
