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
#ifndef __VTS_SYSFUZZER_COMMON_REPLAYER_VTSHIDLHALREPLAYER_H__
#define __VTS_SYSFUZZER_COMMON_REPLAYER_VTSHIDLHALREPLAYER_H__

#include "fuzz_tester/FuzzerWrapper.h"
#include "specification_parser/SpecificationBuilder.h"
#include "test/vts/proto/VtsProfilingMessage.pb.h"

namespace android {
namespace vts {

// Class to perform VTS record and replay test.
// The class is responsible for:
// 1) Load and parse a given trace file.
// 2) Replay the API call sequence parsed from the trace file by calling
//    the HAL drive.
// 3) Verify the return results of each API calls.
class VtsHidlHalReplayer {
 public:
  VtsHidlHalReplayer(SpecificationBuilder* spec_build,
                     std::string& callback_socket_name)
      : spec_builder_(spec_build),
        callback_socket_name_(callback_socket_name) {}

  // Replays the API call sequence parsed from the trace file.
  bool ReplayTrace(const std::string& trace_file,
                   const std::string& hal_service_name);

 private:
  // Used to load the vts spec and driver library. Does not own.
  SpecificationBuilder* spec_builder_;
  // A FuzzerWrapper instance.
  FuzzerWrapper wrapper_;
  // The server socket port # of the agent.
  std::string callback_socket_name_;
};

}  // namespace vts
}  // namespace android

#endif  // __VTS_SYSFUZZER_COMMON_REPLAYER_VTSHIDLHALREPLAYER_H__
