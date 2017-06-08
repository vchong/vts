#ifndef __VTS_DRIVER__android_hardware_tests_msgq_V1_0_ITestMsgQ__
#define __VTS_DRIVER__android_hardware_tests_msgq_V1_0_ITestMsgQ__

#undef LOG_TAG
#define LOG_TAG "FuzzerExtended_android_hardware_tests_msgq_V1_0_ITestMsgQ"

#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <utils/Log.h>

#include <fuzz_tester/FuzzerBase.h>
#include <fuzz_tester/FuzzerCallbackBase.h>

#include <android/hardware/tests/msgq/1.0/ITestMsgQ.h>
#include <hidl/HidlSupport.h>
#include <android/hidl/base/1.0/types.h>


using namespace android::hardware::tests::msgq::V1_0;
namespace android {
namespace vts {
::android::hardware::tests::msgq::V1_0::ITestMsgQ::EventFlagBits EnumValue__android__hardware__tests__msgq__V1_0__ITestMsgQ__EventFlagBits(const ScalarDataValueMessage& arg);
::android::hardware::tests::msgq::V1_0::ITestMsgQ::EventFlagBits Random__android__hardware__tests__msgq__V1_0__ITestMsgQ__EventFlagBits();
bool Verify__android__hardware__tests__msgq__V1_0__ITestMsgQ__EventFlagBits(const VariableSpecificationMessage& expected_result, const VariableSpecificationMessage& actual_result);
void SetResult__android__hardware__tests__msgq__V1_0__ITestMsgQ__EventFlagBits(VariableSpecificationMessage* result_msg, ::android::hardware::tests::msgq::V1_0::ITestMsgQ::EventFlagBits result_value);
class FuzzerExtended_android_hardware_tests_msgq_V1_0_ITestMsgQ : public FuzzerBase {
 public:
    FuzzerExtended_android_hardware_tests_msgq_V1_0_ITestMsgQ() : FuzzerBase(HAL_HIDL), hw_binder_proxy_() {}

    explicit FuzzerExtended_android_hardware_tests_msgq_V1_0_ITestMsgQ(::android::hardware::tests::msgq::V1_0::ITestMsgQ* hw_binder_proxy) : FuzzerBase(HAL_HIDL), hw_binder_proxy_(hw_binder_proxy) {}
    uint64_t GetHidlInterfaceProxy() const {
        return reinterpret_cast<uintptr_t>(hw_binder_proxy_.get());
    }
 protected:
    bool Fuzz(FunctionSpecificationMessage* func_msg, void** result, const string& callback_socket_name);
    bool CallFunction(const FunctionSpecificationMessage& func_msg, const string& callback_socket_name, FunctionSpecificationMessage* result_msg);
    bool VerifyResults(const FunctionSpecificationMessage& expected_result, const FunctionSpecificationMessage& actual_result);
    bool GetAttribute(FunctionSpecificationMessage* func_msg, void** result);
    bool GetService(bool get_stub, const char* service_name);

 private:
    sp<::android::hardware::tests::msgq::V1_0::ITestMsgQ> hw_binder_proxy_;
};


extern "C" {
extern android::vts::FuzzerBase* vts_func_4_android_hardware_tests_msgq_V1_0_ITestMsgQ_();
extern android::vts::FuzzerBase* vts_func_4_android_hardware_tests_msgq_V1_0_ITestMsgQ_with_arg(uint64_t hw_binder_proxy);
}
}  // namespace vts
}  // namespace android
#endif
