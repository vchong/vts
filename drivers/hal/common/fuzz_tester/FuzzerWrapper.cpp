/*
 * Copyright 2016 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "fuzz_tester/FuzzerWrapper.h"

#include <dlfcn.h>

#include <iostream>
#include <sstream>
#include <string>

#include "component_loader/DllLoader.h"
#include "fuzz_tester/FuzzerBase.h"
#include "utils/InterfaceSpecUtil.h"
#include "utils/StringUtil.h"

#include "test/vts/proto/ComponentSpecificationMessage.pb.h"

using namespace std;

namespace android {
namespace vts {

FuzzerWrapper::FuzzerWrapper()
    : function_name_prefix_chars_(NULL), fuzzer_base_(NULL) {}

bool FuzzerWrapper::LoadInterfaceSpecificationLibrary(
    const char* spec_dll_path) {
  if (!spec_dll_path) {
    cerr << __func__ << ":" << __LINE__ << " arg is NULL" << endl;
    return false;
  }
  if (spec_dll_path_.size() > 0 &&
      !strcmp(spec_dll_path, spec_dll_path_.c_str())) {
    return true;
  }
  spec_dll_path_ = spec_dll_path;
  if (!dll_loader_.Load(spec_dll_path_.c_str(), false)) return false;
  cout << "DLL loaded " << spec_dll_path_ << endl;
  return true;
}

FuzzerBase* FuzzerWrapper::GetFuzzer(
    const vts::ComponentSpecificationMessage& message) {
  cout << __func__ << ":" << __LINE__ << " entry" << endl;
  if (spec_dll_path_.size() == 0) {
    cerr << __func__ << ": spec_dll_path_ not set" << endl;
    return NULL;
  }

  string function_name_prefix = GetFunctionNamePrefix(message);
  const char* function_name_prefix_chars = function_name_prefix.c_str();
  cout << __func__ << ": function name '" << function_name_prefix_chars
       << "'" << endl;
  if (function_name_prefix_chars_ && function_name_prefix_chars &&
      !strcmp(function_name_prefix_chars, function_name_prefix_chars_)) {
    return fuzzer_base_;
  }

  loader_function func =
      dll_loader_.GetLoaderFunction(function_name_prefix_chars);
  if (!func) {
    cerr << __func__ << ": function not found." << endl;
    return NULL;
  }
  cout << __func__ << ": function found; trying to call." << endl;
  fuzzer_base_ = func();
  function_name_prefix_chars_ =
      (char*)malloc(strlen(function_name_prefix_chars) + 1);
  strcpy(function_name_prefix_chars_, function_name_prefix_chars);
  return fuzzer_base_;
}

FuzzerBase* FuzzerWrapper::GetFuzzer(const string& name,
                                     const uint64_t interface_pt) const {
  // Assumption: no shared library lookup is needed because that is handled
  // the by the driver's linking dependency.
  // Example: name (android::hardware::gnss::V1_0::IAGnssRil) converted to
  // function name (vts_func_4_android_hardware_tests_bar_V1_0_IBar_with_arg)
  stringstream prefix_ss;
  string mutable_name = name;
  ReplaceSubString(mutable_name, "::", "_");
  prefix_ss << VTS_INTERFACE_SPECIFICATION_FUNCTION_NAME_PREFIX << HAL_HIDL
            << mutable_name << "_with_arg";
  string function_name_prefix = prefix_ss.str();
  loader_function_with_arg func =
      dll_loader_.GetLoaderFunctionWithArg(function_name_prefix.c_str());
  if (!func) {
    cerr << __func__ << ": function not found." << endl;
    return NULL;
  }
  return func(interface_pt);
}

}  // namespace vts
}  // namespace android
