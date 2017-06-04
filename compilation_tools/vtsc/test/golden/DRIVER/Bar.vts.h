#ifndef __VTS_DRIVER__android_hardware_tests_bar_V1_0_IBar__
#define __VTS_DRIVER__android_hardware_tests_bar_V1_0_IBar__

#undef LOG_TAG
#define LOG_TAG "FuzzerExtended_android_hardware_tests_bar_V1_0_IBar"

#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <utils/Log.h>

#include <fuzz_tester/FuzzerBase.h>
#include <fuzz_tester/FuzzerCallbackBase.h>

#include <android/hardware/tests/bar/1.0/IBar.h>
#include <hidl/HidlSupport.h>
#include <android/hardware/tests/foo/1.0/IFoo.h>
#include <android/hardware/tests/foo/1.0/Foo.vts.h>
#include <android/hardware/tests/foo/1.0/IFooCallback.h>
#include <android/hardware/tests/foo/1.0/FooCallback.vts.h>
#include <android/hardware/tests/foo/1.0/IMyTypes.h>
#include <android/hardware/tests/foo/1.0/MyTypes.vts.h>
#include <android/hardware/tests/foo/1.0/ISimple.h>
#include <android/hardware/tests/foo/1.0/Simple.vts.h>
#include <android/hardware/tests/foo/1.0/ITheirTypes.h>
#include <android/hardware/tests/foo/1.0/TheirTypes.vts.h>
#include <android/hardware/tests/foo/1.0/types.h>
#include <android/hardware/tests/foo/1.0/types.vts.h>
#include <android/hidl/base/1.0/types.h>


using namespace android::hardware::tests::bar::V1_0;
namespace android {
namespace vts {
void MessageTo__android__hardware__tests__bar__V1_0__IBar__SomethingRelated(const VariableSpecificationMessage& var_msg, ::android::hardware::tests::bar::V1_0::IBar::SomethingRelated* arg);
bool Verify__android__hardware__tests__bar__V1_0__IBar__SomethingRelated(const VariableSpecificationMessage& expected_result, const VariableSpecificationMessage& actual_result);
void SetResult__android__hardware__tests__bar__V1_0__IBar__SomethingRelated(VariableSpecificationMessage* result_msg, ::android::hardware::tests::bar::V1_0::IBar::SomethingRelated result_value);
class FuzzerExtended_android_hardware_tests_bar_V1_0_IBar : public FuzzerBase {
 public:
    FuzzerExtended_android_hardware_tests_bar_V1_0_IBar() : FuzzerBase(HAL_HIDL), hw_binder_proxy_() {}

    explicit FuzzerExtended_android_hardware_tests_bar_V1_0_IBar(::android::hardware::tests::bar::V1_0::IBar* hw_binder_proxy) : FuzzerBase(HAL_HIDL), hw_binder_proxy_(hw_binder_proxy) {}
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
    sp<::android::hardware::tests::bar::V1_0::IBar> hw_binder_proxy_;
};


extern "C" {
extern android::vts::FuzzerBase* vts_func_4_android_hardware_tests_bar_V1_0_IBar_();
extern android::vts::FuzzerBase* vts_func_4_android_hardware_tests_bar_V1_0_IBar_with_arg(uint64_t hw_binder_proxy);
}
}  // namespace vts
}  // namespace android
#endif
