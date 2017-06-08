#include "android/hardware/tests/memory/1.0/MemoryTest.vts.h"
#include "vts_measurement.h"
#include <iostream>
#include <hidl/HidlSupport.h>
#include <android/hardware/tests/memory/1.0/IMemoryTest.h>
#include <android/hidl/base/1.0/types.h>
#include <android/hidl/allocator/1.0/IAllocator.h>
#include <fmq/MessageQueue.h>
#include <sys/stat.h>
#include <unistd.h>


using namespace android::hardware::tests::memory::V1_0;
namespace android {
namespace vts {
bool FuzzerExtended_android_hardware_tests_memory_V1_0_IMemoryTest::GetService(bool get_stub, const char* service_name) {
    static bool initialized = false;
    if (!initialized) {
        cout << "[agent:hal] HIDL getService" << endl;
        if (service_name) {
          cout << "  - service name: " << service_name << endl;
        }
        hw_binder_proxy_ = ::android::hardware::tests::memory::V1_0::IMemoryTest::getService(service_name, get_stub);
        if (hw_binder_proxy_ == nullptr) {
            cerr << "getService() returned a null pointer." << endl;
            return false;
        }
        cout << "[agent:hal] hw_binder_proxy_ = " << hw_binder_proxy_.get() << endl;
        initialized = true;
    }
    return true;
}

bool FuzzerExtended_android_hardware_tests_memory_V1_0_IMemoryTest::Fuzz(
    FunctionSpecificationMessage* func_msg,
    void** result, const string& callback_socket_name) {
    return true;
}
bool FuzzerExtended_android_hardware_tests_memory_V1_0_IMemoryTest::GetAttribute(
    FunctionSpecificationMessage* func_msg,
    void** result) {
    cerr << "attribute not found" << endl;
    return false;
}
bool FuzzerExtended_android_hardware_tests_memory_V1_0_IMemoryTest::CallFunction(const FunctionSpecificationMessage& func_msg, const string& callback_socket_name, FunctionSpecificationMessage* result_msg) {
    const char* func_name = func_msg.name().c_str();
    cout << "Function: " << __func__ << " " << func_name << endl;
    if (!strcmp(func_name, "haveSomeMemory")) {
        ::android::hardware::hidl_memory arg0;
        sp<::android::hidl::allocator::V1_0::IAllocator> ashmemAllocator = ::android::hidl::allocator::V1_0::IAllocator::getService("ashmem");
        if (ashmemAllocator == nullptr) {
            cerr << "Failed to get ashmemAllocator! " << endl;
            exit(-1);
        }
        auto res = ashmemAllocator->allocate(func_msg.arg(0).hidl_memory_value().size(), [&](bool success, const hardware::hidl_memory& memory) {
            if (!success) {
                cerr << "Failed to allocate memory! " << endl;
                arg0 = ::android::hardware::hidl_memory();
                return;
            }
            arg0 = memory;
        });
        VtsMeasurement vts_measurement;
        vts_measurement.Start();
        cout << "Call an API" << endl;
        cout << "local_device = " << hw_binder_proxy_.get() << endl;
        ::android::hardware::hidl_memory result0;
        hw_binder_proxy_->haveSomeMemory(arg0, [&](const ::android::hardware::hidl_memory& arg0){
            cout << "callback haveSomeMemory called" << endl;
            result0 = arg0;
        });
        vector<float>* measured = vts_measurement.Stop();
        cout << "time " << (*measured)[0] << endl;
        result_msg->set_name("haveSomeMemory");
        VariableSpecificationMessage* result_val_0 = result_msg->add_return_type_hidl();
        result_val_0->set_type(TYPE_HIDL_MEMORY);
        /* ERROR: TYPE_HIDL_MEMORY is not supported yet. */
        cout << "called" << endl;
        return true;
    }
    if (!strcmp(func_name, "fillMemory")) {
        ::android::hardware::hidl_memory arg0;
        sp<::android::hidl::allocator::V1_0::IAllocator> ashmemAllocator = ::android::hidl::allocator::V1_0::IAllocator::getService("ashmem");
        if (ashmemAllocator == nullptr) {
            cerr << "Failed to get ashmemAllocator! " << endl;
            exit(-1);
        }
        auto res = ashmemAllocator->allocate(func_msg.arg(0).hidl_memory_value().size(), [&](bool success, const hardware::hidl_memory& memory) {
            if (!success) {
                cerr << "Failed to allocate memory! " << endl;
                arg0 = ::android::hardware::hidl_memory();
                return;
            }
            arg0 = memory;
        });
        uint8_t arg1 = 0;
        arg1 = func_msg.arg(1).scalar_value().uint8_t();
        VtsMeasurement vts_measurement;
        vts_measurement.Start();
        cout << "Call an API" << endl;
        cout << "local_device = " << hw_binder_proxy_.get() << endl;
        hw_binder_proxy_->fillMemory(arg0, arg1);
        vector<float>* measured = vts_measurement.Stop();
        cout << "time " << (*measured)[0] << endl;
        result_msg->set_name("fillMemory");
        cout << "called" << endl;
        return true;
    }
    if (!strcmp(func_name, "notifySyspropsChanged")) {
        cout << "Call notifySyspropsChanged" << endl;
        hw_binder_proxy_->notifySyspropsChanged();
        result_msg->set_name("notifySyspropsChanged");
        cout << "called" << endl;
        return true;
    }
    return false;
}

bool FuzzerExtended_android_hardware_tests_memory_V1_0_IMemoryTest::VerifyResults(const FunctionSpecificationMessage& expected_result, const FunctionSpecificationMessage& actual_result) {
    if (!strcmp(actual_result.name().c_str(), "haveSomeMemory")) {
        if (actual_result.return_type_hidl_size() != expected_result.return_type_hidl_size() ) { return false; }
        /* ERROR: TYPE_HIDL_MEMORY is not supported yet. */
        return true;
    }
    if (!strcmp(actual_result.name().c_str(), "fillMemory")) {
        if (actual_result.return_type_hidl_size() != expected_result.return_type_hidl_size() ) { return false; }
        return true;
    }
    return false;
}

extern "C" {
android::vts::FuzzerBase* vts_func_4_android_hardware_tests_memory_V1_0_IMemoryTest_() {
    return (android::vts::FuzzerBase*) new android::vts::FuzzerExtended_android_hardware_tests_memory_V1_0_IMemoryTest();
}

android::vts::FuzzerBase* vts_func_4_android_hardware_tests_memory_V1_0_IMemoryTest_with_arg(uint64_t hw_binder_proxy) {
    ::android::hardware::tests::memory::V1_0::IMemoryTest* arg = reinterpret_cast<::android::hardware::tests::memory::V1_0::IMemoryTest*>(hw_binder_proxy);
    android::vts::FuzzerBase* result =
        new android::vts::FuzzerExtended_android_hardware_tests_memory_V1_0_IMemoryTest(
            arg);
    arg->decStrong(arg);
    return result;
}

}
}  // namespace vts
}  // namespace android
