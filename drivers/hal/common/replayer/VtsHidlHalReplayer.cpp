/*
 * Copyright (C) 2016 The Android Open Source Project
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
#include "replayer/VtsHidlHalReplayer.h"

#include <fcntl.h>
#include <unistd.h>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>

#include <cutils/properties.h>
#include <google/protobuf/io/zero_copy_stream_impl.h>
#include <google/protobuf/text_format.h>

#include "fuzz_tester/FuzzerBase.h"
#include "fuzz_tester/FuzzerWrapper.h"
#include "specification_parser/InterfaceSpecificationParser.h"
#include "test/vts/proto/ComponentSpecificationMessage.pb.h"
#include "test/vts/proto/VtsProfilingMessage.pb.h"
#include "utils/StringUtil.h"
#include "utils/VtsProfilingUtil.h"

using namespace std;

namespace android {
namespace vts {

VtsHidlHalReplayer::VtsHidlHalReplayer(const string& spec_path,
                                       const string& callback_socket_name)
    : spec_path_(spec_path), callback_socket_name_(callback_socket_name) {}

bool VtsHidlHalReplayer::LoadComponentSpecification(const string& package,
    float version, const string& interface_name,
    ComponentSpecificationMessage* message) {
  if (spec_path_.empty()) {
    cerr << __func__ << "spec file not specified. " << endl;
    return false;
  }
  if (!message) {
    cerr << __func__ << "message could not be NULL. " << endl;
    return false;
  }
  string package_name = package;
  ReplaceSubString(package_name, ".", "/");
  stringstream stream;
  stream << fixed << setprecision(1) << version;
  string version_str = stream.str();
  string spec_file = spec_path_ + '/' + package_name + '/'
      + version_str + '/' + interface_name.substr(1) + ".vts";
  cout << "spec_file: " << spec_file << endl;
  if (InterfaceSpecificationParser::parse(spec_file.c_str(), message)) {
    if (message->component_class() != HAL_HIDL) {
      cerr << __func__ << ": only support Hidl Hal. " << endl;
      return false;
    }

    if (message->package() != package
        || message->component_type_version() != version
        || message->component_name() != interface_name) {
      cerr << __func__ << ": spec file mismatch. " << "expected package: "
           << package << " version: " << version << " interface_name: "
           << interface_name << ", actual: package: " << message->package()
           << " version: " << message->component_type_version()
           << " interface_name: " << message->component_name();
      return false;
    }
  } else {
    cerr << __func__ << ": can not parse spec: " << spec_file << endl;
    return false;
  }
  return true;
}

bool VtsHidlHalReplayer::ReplayTrace(const string& spec_lib_file_path,
    const string& trace_file, const string& hal_service_name) {
  if (!wrapper_.LoadInterfaceSpecificationLibrary(spec_lib_file_path.c_str())) {
    return false;
  }

  // Determine the binder/passthrough mode based on system property.
  char get_sub_property[PROPERTY_VALUE_MAX];
  bool get_stub = false; /* default is binderized */
  if (property_get("vts.hidl.get_stub", get_sub_property, "") > 0) {
    if (!strcmp(get_sub_property, "true") || !strcmp(get_sub_property, "True")
        || !strcmp(get_sub_property, "1")) {
      get_stub = true;
    }
  }

  // Parse the trace file to get the sequence of function calls.
  int fd =
      open(trace_file.c_str(), O_RDONLY, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
  if (fd < 0) {
    cerr << "Can not open trace file: " << trace_file
         << "error: " << std::strerror(errno);
    return false;
  }

  google::protobuf::io::FileInputStream input(fd);

  // long record_num = 0;
  string interface = "";
  FuzzerBase* fuzzer = NULL;
  VtsProfilingRecord call_msg;
  VtsProfilingRecord expected_result_msg;
  while (readOneDelimited(&call_msg, &input) &&
         readOneDelimited(&expected_result_msg, &input)) {
    if (call_msg.event() != InstrumentationEventType::SERVER_API_ENTRY &&
        call_msg.event() != InstrumentationEventType::CLIENT_API_ENTRY &&
        call_msg.event() != InstrumentationEventType::SYNC_CALLBACK_ENTRY &&
        call_msg.event() != InstrumentationEventType::ASYNC_CALLBACK_ENTRY &&
        call_msg.event() != InstrumentationEventType::PASSTHROUGH_ENTRY) {
      cerr << "Expected a call message but got message with event: "
           << call_msg.event();
      continue;
    }
    if (expected_result_msg.event() !=
            InstrumentationEventType::SERVER_API_EXIT &&
        expected_result_msg.event() !=
            InstrumentationEventType::CLIENT_API_EXIT &&
        expected_result_msg.event() !=
            InstrumentationEventType::SYNC_CALLBACK_EXIT &&
        expected_result_msg.event() !=
            InstrumentationEventType::ASYNC_CALLBACK_EXIT &&
        expected_result_msg.event() !=
            InstrumentationEventType::PASSTHROUGH_EXIT) {
      cerr << "Expected a result message but got message with event: "
           << call_msg.event();
      continue;
    }

    cout << __func__ << ": replay function: " << call_msg.func_msg().name();

    // Load spec file and get fuzzer.
    if (interface != call_msg.interface()) {
      interface = call_msg.interface();
      ComponentSpecificationMessage interface_specification_message;
      if (!LoadComponentSpecification(call_msg.package(), call_msg.version(),
                                      call_msg.interface(),
                                      &interface_specification_message)) {
        cerr << __func__ << ": can not load component spec: " << spec_path_;
        return false;
      }
      fuzzer = wrapper_.GetFuzzer(interface_specification_message);
      if (!fuzzer) {
        cerr << __func__ << ": couldn't get a fuzzer base class" << endl;
        return false;
      }

      if (!fuzzer->GetService(get_stub, hal_service_name.c_str())) {
        cerr << __func__ << ": couldn't get service: " << hal_service_name
             << endl;
        return false;
      }
    }

    vts::FunctionSpecificationMessage result_msg;
    if (!fuzzer->CallFunction(call_msg.func_msg(), callback_socket_name_,
                              &result_msg)) {
      cerr << __func__ << ": replay function fail." << endl;
      return false;
    }
    if (!fuzzer->VerifyResults(expected_result_msg.func_msg(), result_msg)) {
      // Verification is not strict, i.e. if fail, output error message and
      // continue the process.
      cerr << __func__ << ": verification fail." << endl;
    }
    call_msg.Clear();
    expected_result_msg.Clear();
  }
  return true;
}

}  // namespace vts
}  // namespace android
