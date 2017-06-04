#ifndef __VTS_DRIVER__lib_shared_bionic_libc_V1_0__
#define __VTS_DRIVER__lib_shared_bionic_libc_V1_0__

#undef LOG_TAG
#define LOG_TAG "FuzzerExtended_libc"
#include <unistd.h>
#include <stdint.h>
#include <sys/types.h>
#include <linux/socket.h>

#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <utils/Log.h>

#include <fuzz_tester/FuzzerBase.h>
#include <fuzz_tester/FuzzerCallbackBase.h>

namespace android {
namespace vts {
class FuzzerExtended_libc : public FuzzerBase {
 public:
    FuzzerExtended_libc() : FuzzerBase(LIB_SHARED) {}
 protected:
    bool Fuzz(FunctionSpecificationMessage* func_msg, void** result, const string& callback_socket_name);
    bool CallFunction(const FunctionSpecificationMessage& func_msg, const string& callback_socket_name, FunctionSpecificationMessage* result_msg);
    bool VerifyResults(const FunctionSpecificationMessage& expected_result, const FunctionSpecificationMessage& actual_result);
    bool GetAttribute(FunctionSpecificationMessage* func_msg, void** result);
 private:
};


extern "C" {
extern android::vts::FuzzerBase* vts_func_11_1002_V1_0_();
}
}  // namespace vts
}  // namespace android
#endif
