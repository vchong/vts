#!/bin/bash
#
# Copyright 2016 Google Inc. All Rights Reserved.
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
#

LOCAL_PIP_PATH=$HOME/vts-pypi-packages

echo "Making local dir at" $LOCAL_PIP_PATH
if [ -d "$LOCAL_PIP_PATH" ]; then
  echo $LOCAL_PIP_PATH "already exists"
else
  mkdir $LOCAL_PIP_PATH
fi

echo "Downloading PyPI packages to" $LOCAL_PIP_PATH
pip download -d $LOCAL_PIP_PATH -r ./test/vts/script/pip_requirements.txt --no-binary protobuf,grpcio,numpy,Pillow,scipy
# The --no-binary option is necessary for packages that have a
# "-cp27-cp27mu-manylinux1_x86_64.whl" version that will be downloaded instead
# if --no-binary option is not specified. This version is not compatible with
# the python virtualenv set up by VtsPythonVirtualenvPreparer.

pip download -d $LOCAL_PIP_PATH matplotlib --no-binary matplotlib,numpy
# TODO: Downloading matplotlib fails with an error that causes pip download to
# abort. Therefore separated temporarily. Must resolve matplotlib installation
# error in order to run CameraITS tests.
