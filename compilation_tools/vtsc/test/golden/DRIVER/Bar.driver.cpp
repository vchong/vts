#include "android/hardware/tests/bar/1.0/Bar.vts.h"
#include "vts_measurement.h"
#include <iostream>
#include <hidl/HidlSupport.h>
#include <android/hardware/tests/bar/1.0/IBar.h>
#include <android/hardware/tests/foo/1.0/IFoo.h>
#include <android/hardware/tests/foo/1.0/IFooCallback.h>
#include <android/hardware/tests/foo/1.0/IMyTypes.h>
#include <android/hardware/tests/foo/1.0/ISimple.h>
#include <android/hardware/tests/foo/1.0/ITheirTypes.h>
#include <android/hardware/tests/foo/1.0/types.h>
#include <android/hidl/base/1.0/types.h>


using namespace android::hardware::tests::bar::V1_0;
namespace android {
namespace vts {
void MessageTo__android__hardware__tests__bar__V1_0__IBar__SomethingRelated(const VariableSpecificationMessage& var_msg, ::android::hardware::tests::bar::V1_0::IBar::SomethingRelated* arg) {
    MessageTo__android__hardware__tests__foo__V1_0__Unrelated(var_msg.struct_value(0), &(arg->myRelated));
}
bool Verify__android__hardware__tests__bar__V1_0__IBar__SomethingRelated(const VariableSpecificationMessage& expected_result, const VariableSpecificationMessage& actual_result){
    if (!Verify__android__hardware__tests__foo__V1_0__Unrelated(expected_result.struct_value(0), actual_result.struct_value(0))) { return false; }
    return true;
}

void SetResult__android__hardware__tests__bar__V1_0__IBar__SomethingRelated(VariableSpecificationMessage* result_msg, ::android::hardware::tests::bar::V1_0::IBar::SomethingRelated result_value){
    result_msg->set_type(TYPE_STRUCT);
    auto *result_msg_myRelated = result_msg->add_struct_value();
    result_msg_myRelated->set_type(TYPE_STRUCT);
    SetResult__android__hardware__tests__foo__V1_0__Unrelated(result_msg_myRelated, result_value.myRelated);
    result_msg_myRelated->set_name("myRelated");
}

bool FuzzerExtended_android_hardware_tests_bar_V1_0_IBar::GetService(bool get_stub, const char* service_name) {
    static bool initialized = false;
    if (!initialized) {
        cout << "[agent:hal] HIDL getService" << endl;
        if (service_name) {
          cout << "  - service name: " << service_name << endl;
        }
        hw_binder_proxy_ = ::android::hardware::tests::bar::V1_0::IBar::getService(service_name, get_stub);
        if (hw_binder_proxy_ == nullptr) {
            cerr << "getService() returned a null pointer." << endl;
            return false;
        }
        cout << "[agent:hal] hw_binder_proxy_ = " << hw_binder_proxy_.get() << endl;
        initialized = true;
    }
    return true;
}

bool FuzzerExtended_android_hardware_tests_bar_V1_0_IBar::Fuzz(
    FunctionSpecificationMessage* func_msg,
    void** result, const string& callback_socket_name) {
    return true;
}
bool FuzzerExtended_android_hardware_tests_bar_V1_0_IBar::GetAttribute(
    FunctionSpecificationMessage* func_msg,
    void** result) {
    cerr << "attribute not found" << endl;
    return false;
}
bool FuzzerExtended_android_hardware_tests_bar_V1_0_IBar::CallFunction(const FunctionSpecificationMessage& func_msg, const string& callback_socket_name, FunctionSpecificationMessage* result_msg) {
    const char* func_name = func_msg.name().c_str();
    cout << "Function: " << __func__ << " " << func_name << endl;
    if (!strcmp(func_name, "doThis")) {
        float arg0;
        arg0 = 0;
        arg0 = func_msg.arg(0).scalar_value().float_t();
        VtsMeasurement vts_measurement;
        vts_measurement.Start();
        cout << "Call an API" << endl;
        cout << "local_device = " << hw_binder_proxy_.get() << endl;
        hw_binder_proxy_->doThis(arg0);
        vector<float>* measured = vts_measurement.Stop();
        cout << "time " << (*measured)[0] << endl;
        result_msg->set_name("doThis");
        cout << "called" << endl;
        return true;
    }
    if (!strcmp(func_name, "doThatAndReturnSomething")) {
        int64_t arg0;
        arg0 = 0;
        arg0 = func_msg.arg(0).scalar_value().int64_t();
        VtsMeasurement vts_measurement;
        vts_measurement.Start();
        cout << "Call an API" << endl;
        cout << "local_device = " << hw_binder_proxy_.get() << endl;
        int32_t result0;
        result0 = hw_binder_proxy_->doThatAndReturnSomething(arg0);
        vector<float>* measured = vts_measurement.Stop();
        cout << "time " << (*measured)[0] << endl;
        result_msg->set_name("doThatAndReturnSomething");
        VariableSpecificationMessage* result_val_0 = result_msg->add_return_type_hidl();
        result_val_0->set_type(TYPE_SCALAR);
        result_val_0->set_scalar_type("int32_t");
        result_val_0->mutable_scalar_value()->set_int32_t(result0);
        cout << "called" << endl;
        return true;
    }
    if (!strcmp(func_name, "doQuiteABit")) {
        int32_t arg0;
        arg0 = 0;
        arg0 = func_msg.arg(0).scalar_value().int32_t();
        int64_t arg1;
        arg1 = 0;
        arg1 = func_msg.arg(1).scalar_value().int64_t();
        float arg2;
        arg2 = 0;
        arg2 = func_msg.arg(2).scalar_value().float_t();
        double arg3;
        arg3 = 0;
        arg3 = func_msg.arg(3).scalar_value().double_t();
        VtsMeasurement vts_measurement;
        vts_measurement.Start();
        cout << "Call an API" << endl;
        cout << "local_device = " << hw_binder_proxy_.get() << endl;
        double result0;
        result0 = hw_binder_proxy_->doQuiteABit(arg0,arg1,arg2,arg3);
        vector<float>* measured = vts_measurement.Stop();
        cout << "time " << (*measured)[0] << endl;
        result_msg->set_name("doQuiteABit");
        VariableSpecificationMessage* result_val_0 = result_msg->add_return_type_hidl();
        result_val_0->set_type(TYPE_SCALAR);
        result_val_0->set_scalar_type("double_t");
        result_val_0->mutable_scalar_value()->set_double_t(result0);
        cout << "called" << endl;
        return true;
    }
    if (!strcmp(func_name, "doSomethingElse")) {
         ::android::hardware::hidl_array<int32_t,15> arg0;
        for (int i = 0; i < func_msg.arg(0).vector_value_size(); i++) {
            arg0[i] = func_msg.arg(0).vector_value(i).scalar_value().int32_t();
        }
        VtsMeasurement vts_measurement;
        vts_measurement.Start();
        cout << "Call an API" << endl;
        cout << "local_device = " << hw_binder_proxy_.get() << endl;
        ::android::hardware::hidl_array<int32_t,32> result0;
        hw_binder_proxy_->doSomethingElse(arg0, [&](const ::android::hardware::hidl_array<int32_t,32>& arg0){
            cout << "callback doSomethingElse called" << endl;
            result0 = arg0;
        });
        vector<float>* measured = vts_measurement.Stop();
        cout << "time " << (*measured)[0] << endl;
        result_msg->set_name("doSomethingElse");
        VariableSpecificationMessage* result_val_0 = result_msg->add_return_type_hidl();
        result_val_0->set_type(TYPE_ARRAY);
        result_val_0->set_vector_size(1);
        for (int i = 0; i < 1; i++) {
            auto *result_val_0_array_i = result_val_0->add_vector_value();
            result_val_0_array_i->set_type(TYPE_SCALAR);
            result_val_0_array_i->set_scalar_type("int32_t");
            result_val_0_array_i->mutable_scalar_value()->set_int32_t(result0[i]);
        }
        cout << "called" << endl;
        return true;
    }
    if (!strcmp(func_name, "doStuffAndReturnAString")) {
        VtsMeasurement vts_measurement;
        vts_measurement.Start();
        cout << "Call an API" << endl;
        cout << "local_device = " << hw_binder_proxy_.get() << endl;
        ::android::hardware::hidl_string result0;
        hw_binder_proxy_->doStuffAndReturnAString([&](const ::android::hardware::hidl_string& arg0){
            cout << "callback doStuffAndReturnAString called" << endl;
            result0 = arg0;
        });
        vector<float>* measured = vts_measurement.Stop();
        cout << "time " << (*measured)[0] << endl;
        result_msg->set_name("doStuffAndReturnAString");
        VariableSpecificationMessage* result_val_0 = result_msg->add_return_type_hidl();
        result_val_0->set_type(TYPE_STRING);
        result_val_0->mutable_string_value()->set_message(result0.c_str());
        result_val_0->mutable_string_value()->set_length(result0.size());
        cout << "called" << endl;
        return true;
    }
    if (!strcmp(func_name, "mapThisVector")) {
         ::android::hardware::hidl_vec<int32_t> arg0;
        arg0.resize(func_msg.arg(0).vector_value_size());
        for (int i = 0; i <func_msg.arg(0).vector_value_size(); i++) {
            arg0[i] = func_msg.arg(0).vector_value(i).scalar_value().int32_t();
        }
        VtsMeasurement vts_measurement;
        vts_measurement.Start();
        cout << "Call an API" << endl;
        cout << "local_device = " << hw_binder_proxy_.get() << endl;
        ::android::hardware::hidl_vec<int32_t> result0;
        hw_binder_proxy_->mapThisVector(arg0, [&](const ::android::hardware::hidl_vec<int32_t>& arg0){
            cout << "callback mapThisVector called" << endl;
            result0 = arg0;
        });
        vector<float>* measured = vts_measurement.Stop();
        cout << "time " << (*measured)[0] << endl;
        result_msg->set_name("mapThisVector");
        VariableSpecificationMessage* result_val_0 = result_msg->add_return_type_hidl();
        result_val_0->set_type(TYPE_VECTOR);
        result_val_0->set_vector_size(result0.size());
        for (int i = 0; i < (int)result0.size(); i++) {
            auto *result_val_0_vector_i = result_val_0->add_vector_value();
            result_val_0_vector_i->set_type(TYPE_SCALAR);
            result_val_0_vector_i->set_scalar_type("int32_t");
            result_val_0_vector_i->mutable_scalar_value()->set_int32_t(result0[i]);
        }
        cout << "called" << endl;
        return true;
    }
    if (!strcmp(func_name, "callMe")) {
        sp<::android::hardware::tests::foo::V1_0::IFooCallback> arg0;
        arg0 = VtsFuzzerCreateVts_android_hardware_tests_foo_V1_0_IFooCallback(callback_socket_name);
        static_cast<Vts_android_hardware_tests_foo_V1_0_IFooCallback*>(arg0.get())->Register(func_msg.arg(0));
        VtsMeasurement vts_measurement;
        vts_measurement.Start();
        cout << "Call an API" << endl;
        cout << "local_device = " << hw_binder_proxy_.get() << endl;
        hw_binder_proxy_->callMe(arg0);
        vector<float>* measured = vts_measurement.Stop();
        cout << "time " << (*measured)[0] << endl;
        result_msg->set_name("callMe");
        cout << "called" << endl;
        return true;
    }
    if (!strcmp(func_name, "useAnEnum")) {
        ::android::hardware::tests::foo::V1_0::IFoo::SomeEnum arg0;
        arg0 = EnumValue__android__hardware__tests__foo__V1_0__IFoo__SomeEnum(func_msg.arg(0).scalar_value());
        VtsMeasurement vts_measurement;
        vts_measurement.Start();
        cout << "Call an API" << endl;
        cout << "local_device = " << hw_binder_proxy_.get() << endl;
        ::android::hardware::tests::foo::V1_0::IFoo::SomeEnum result0;
        result0 = hw_binder_proxy_->useAnEnum(arg0);
        vector<float>* measured = vts_measurement.Stop();
        cout << "time " << (*measured)[0] << endl;
        result_msg->set_name("useAnEnum");
        VariableSpecificationMessage* result_val_0 = result_msg->add_return_type_hidl();
        result_val_0->set_type(TYPE_ENUM);
        SetResult__android__hardware__tests__foo__V1_0__IFoo__SomeEnum(result_val_0, result0);
        cout << "called" << endl;
        return true;
    }
    if (!strcmp(func_name, "haveAGooberVec")) {
         ::android::hardware::hidl_vec<::android::hardware::tests::foo::V1_0::IFoo::Goober> arg0;
        arg0.resize(func_msg.arg(0).vector_value_size());
        for (int i = 0; i <func_msg.arg(0).vector_value_size(); i++) {
            MessageTo__android__hardware__tests__foo__V1_0__IFoo__Goober(func_msg.arg(0).vector_value(i), &(arg0[i]));
        }
        VtsMeasurement vts_measurement;
        vts_measurement.Start();
        cout << "Call an API" << endl;
        cout << "local_device = " << hw_binder_proxy_.get() << endl;
        hw_binder_proxy_->haveAGooberVec(arg0);
        vector<float>* measured = vts_measurement.Stop();
        cout << "time " << (*measured)[0] << endl;
        result_msg->set_name("haveAGooberVec");
        cout << "called" << endl;
        return true;
    }
    if (!strcmp(func_name, "haveAGoober")) {
        ::android::hardware::tests::foo::V1_0::IFoo::Goober arg0;
        MessageTo__android__hardware__tests__foo__V1_0__IFoo__Goober(func_msg.arg(0), &(arg0));
        VtsMeasurement vts_measurement;
        vts_measurement.Start();
        cout << "Call an API" << endl;
        cout << "local_device = " << hw_binder_proxy_.get() << endl;
        hw_binder_proxy_->haveAGoober(arg0);
        vector<float>* measured = vts_measurement.Stop();
        cout << "time " << (*measured)[0] << endl;
        result_msg->set_name("haveAGoober");
        cout << "called" << endl;
        return true;
    }
    if (!strcmp(func_name, "haveAGooberArray")) {
         ::android::hardware::hidl_array<::android::hardware::tests::foo::V1_0::IFoo::Goober,20> arg0;
        for (int i = 0; i < func_msg.arg(0).vector_value_size(); i++) {
            arg0[i].q = func_msg.arg(0).vector_value(i).struct_value(0).scalar_value().int32_t();
            arg0[i].name = ::android::hardware::hidl_string(func_msg.arg(0).vector_value(i).struct_value(1).string_value().message());
            arg0[i].address = ::android::hardware::hidl_string(func_msg.arg(0).vector_value(i).struct_value(2).string_value().message());
            for (int i = 0; i < func_msg.arg(0).vector_value(i).struct_value(3).vector_value_size(); i++) {
                arg0[i].numbers[i] = func_msg.arg(0).vector_value(i).struct_value(3).vector_value(i).scalar_value().double_t();
            }
            MessageTo__android__hardware__tests__foo__V1_0__IFoo__Fumble(func_msg.arg(0).vector_value(i).struct_value(4), &(arg0[i].fumble));
            MessageTo__android__hardware__tests__foo__V1_0__IFoo__Fumble(func_msg.arg(0).vector_value(i).struct_value(5), &(arg0[i].gumble));
        }
        VtsMeasurement vts_measurement;
        vts_measurement.Start();
        cout << "Call an API" << endl;
        cout << "local_device = " << hw_binder_proxy_.get() << endl;
        hw_binder_proxy_->haveAGooberArray(arg0);
        vector<float>* measured = vts_measurement.Stop();
        cout << "time " << (*measured)[0] << endl;
        result_msg->set_name("haveAGooberArray");
        cout << "called" << endl;
        return true;
    }
    if (!strcmp(func_name, "haveATypeFromAnotherFile")) {
        ::android::hardware::tests::foo::V1_0::Abc arg0;
        MessageTo__android__hardware__tests__foo__V1_0__Abc(func_msg.arg(0), &(arg0));
        VtsMeasurement vts_measurement;
        vts_measurement.Start();
        cout << "Call an API" << endl;
        cout << "local_device = " << hw_binder_proxy_.get() << endl;
        hw_binder_proxy_->haveATypeFromAnotherFile(arg0);
        vector<float>* measured = vts_measurement.Stop();
        cout << "time " << (*measured)[0] << endl;
        result_msg->set_name("haveATypeFromAnotherFile");
        cout << "called" << endl;
        return true;
    }
    if (!strcmp(func_name, "haveSomeStrings")) {
         ::android::hardware::hidl_array<::android::hardware::hidl_string,3> arg0;
        for (int i = 0; i < func_msg.arg(0).vector_value_size(); i++) {
            arg0[i] = ::android::hardware::hidl_string(func_msg.arg(0).vector_value(i).string_value().message());
        }
        VtsMeasurement vts_measurement;
        vts_measurement.Start();
        cout << "Call an API" << endl;
        cout << "local_device = " << hw_binder_proxy_.get() << endl;
        ::android::hardware::hidl_array<::android::hardware::hidl_string,2> result0;
        hw_binder_proxy_->haveSomeStrings(arg0, [&](const ::android::hardware::hidl_array<::android::hardware::hidl_string,2>& arg0){
            cout << "callback haveSomeStrings called" << endl;
            result0 = arg0;
        });
        vector<float>* measured = vts_measurement.Stop();
        cout << "time " << (*measured)[0] << endl;
        result_msg->set_name("haveSomeStrings");
        VariableSpecificationMessage* result_val_0 = result_msg->add_return_type_hidl();
        result_val_0->set_type(TYPE_ARRAY);
        result_val_0->set_vector_size(1);
        for (int i = 0; i < 1; i++) {
            auto *result_val_0_array_i = result_val_0->add_vector_value();
            result_val_0_array_i->set_type(TYPE_STRING);
            result_val_0_array_i->mutable_string_value()->set_message(result0[i].c_str());
            result_val_0_array_i->mutable_string_value()->set_length(result0[i].size());
        }
        cout << "called" << endl;
        return true;
    }
    if (!strcmp(func_name, "haveAStringVec")) {
         ::android::hardware::hidl_vec<::android::hardware::hidl_string> arg0;
        arg0.resize(func_msg.arg(0).vector_value_size());
        for (int i = 0; i <func_msg.arg(0).vector_value_size(); i++) {
            arg0[i] = ::android::hardware::hidl_string(func_msg.arg(0).vector_value(i).string_value().message());
        }
        VtsMeasurement vts_measurement;
        vts_measurement.Start();
        cout << "Call an API" << endl;
        cout << "local_device = " << hw_binder_proxy_.get() << endl;
        ::android::hardware::hidl_vec<::android::hardware::hidl_string> result0;
        hw_binder_proxy_->haveAStringVec(arg0, [&](const ::android::hardware::hidl_vec<::android::hardware::hidl_string>& arg0){
            cout << "callback haveAStringVec called" << endl;
            result0 = arg0;
        });
        vector<float>* measured = vts_measurement.Stop();
        cout << "time " << (*measured)[0] << endl;
        result_msg->set_name("haveAStringVec");
        VariableSpecificationMessage* result_val_0 = result_msg->add_return_type_hidl();
        result_val_0->set_type(TYPE_VECTOR);
        result_val_0->set_vector_size(result0.size());
        for (int i = 0; i < (int)result0.size(); i++) {
            auto *result_val_0_vector_i = result_val_0->add_vector_value();
            result_val_0_vector_i->set_type(TYPE_STRING);
            result_val_0_vector_i->mutable_string_value()->set_message(result0[i].c_str());
            result_val_0_vector_i->mutable_string_value()->set_length(result0[i].size());
        }
        cout << "called" << endl;
        return true;
    }
    if (!strcmp(func_name, "transposeMe")) {
         ::android::hardware::hidl_array<float, 3,5> arg0;
        for (int i = 0; i < func_msg.arg(0).vector_value_size(); i++) {
            for (int i = 0; i < func_msg.arg(0).vector_value(i).vector_value_size(); i++) {
                arg0[i][i] = func_msg.arg(0).vector_value(i).vector_value(i).scalar_value().float_t();
            }
        }
        VtsMeasurement vts_measurement;
        vts_measurement.Start();
        cout << "Call an API" << endl;
        cout << "local_device = " << hw_binder_proxy_.get() << endl;
        ::android::hardware::hidl_array<float, 5,3> result0;
        hw_binder_proxy_->transposeMe(arg0, [&](const ::android::hardware::hidl_array<float, 5,3>& arg0){
            cout << "callback transposeMe called" << endl;
            result0 = arg0;
        });
        vector<float>* measured = vts_measurement.Stop();
        cout << "time " << (*measured)[0] << endl;
        result_msg->set_name("transposeMe");
        VariableSpecificationMessage* result_val_0 = result_msg->add_return_type_hidl();
        result_val_0->set_type(TYPE_ARRAY);
        result_val_0->set_vector_size(1);
        for (int i = 0; i < 1; i++) {
            auto *result_val_0_array_i = result_val_0->add_vector_value();
            result_val_0_array_i->set_type(TYPE_ARRAY);
            result_val_0_array_i->set_vector_size(1);
            for (int i = 0; i < 1; i++) {
                auto *result_val_0_array_i_array_i = result_val_0_array_i->add_vector_value();
                result_val_0_array_i_array_i->set_type(TYPE_SCALAR);
                result_val_0_array_i_array_i->set_scalar_type("float_t");
                result_val_0_array_i_array_i->mutable_scalar_value()->set_float_t(result0[i][i]);
            }
        }
        cout << "called" << endl;
        return true;
    }
    if (!strcmp(func_name, "callingDrWho")) {
        ::android::hardware::tests::foo::V1_0::IFoo::MultiDimensional arg0;
        MessageTo__android__hardware__tests__foo__V1_0__IFoo__MultiDimensional(func_msg.arg(0), &(arg0));
        VtsMeasurement vts_measurement;
        vts_measurement.Start();
        cout << "Call an API" << endl;
        cout << "local_device = " << hw_binder_proxy_.get() << endl;
        ::android::hardware::tests::foo::V1_0::IFoo::MultiDimensional result0;
        hw_binder_proxy_->callingDrWho(arg0, [&](const ::android::hardware::tests::foo::V1_0::IFoo::MultiDimensional& arg0){
            cout << "callback callingDrWho called" << endl;
            result0 = arg0;
        });
        vector<float>* measured = vts_measurement.Stop();
        cout << "time " << (*measured)[0] << endl;
        result_msg->set_name("callingDrWho");
        VariableSpecificationMessage* result_val_0 = result_msg->add_return_type_hidl();
        result_val_0->set_type(TYPE_STRUCT);
        SetResult__android__hardware__tests__foo__V1_0__IFoo__MultiDimensional(result_val_0, result0);
        cout << "called" << endl;
        return true;
    }
    if (!strcmp(func_name, "transpose")) {
        ::android::hardware::tests::foo::V1_0::IFoo::StringMatrix5x3 arg0;
        MessageTo__android__hardware__tests__foo__V1_0__IFoo__StringMatrix5x3(func_msg.arg(0), &(arg0));
        VtsMeasurement vts_measurement;
        vts_measurement.Start();
        cout << "Call an API" << endl;
        cout << "local_device = " << hw_binder_proxy_.get() << endl;
        ::android::hardware::tests::foo::V1_0::IFoo::StringMatrix3x5 result0;
        hw_binder_proxy_->transpose(arg0, [&](const ::android::hardware::tests::foo::V1_0::IFoo::StringMatrix3x5& arg0){
            cout << "callback transpose called" << endl;
            result0 = arg0;
        });
        vector<float>* measured = vts_measurement.Stop();
        cout << "time " << (*measured)[0] << endl;
        result_msg->set_name("transpose");
        VariableSpecificationMessage* result_val_0 = result_msg->add_return_type_hidl();
        result_val_0->set_type(TYPE_STRUCT);
        SetResult__android__hardware__tests__foo__V1_0__IFoo__StringMatrix3x5(result_val_0, result0);
        cout << "called" << endl;
        return true;
    }
    if (!strcmp(func_name, "transpose2")) {
         ::android::hardware::hidl_array<::android::hardware::hidl_string, 5,3> arg0;
        for (int i = 0; i < func_msg.arg(0).vector_value_size(); i++) {
            for (int i = 0; i < func_msg.arg(0).vector_value(i).vector_value_size(); i++) {
                arg0[i][i] = ::android::hardware::hidl_string(func_msg.arg(0).vector_value(i).vector_value(i).string_value().message());
            }
        }
        VtsMeasurement vts_measurement;
        vts_measurement.Start();
        cout << "Call an API" << endl;
        cout << "local_device = " << hw_binder_proxy_.get() << endl;
        ::android::hardware::hidl_array<::android::hardware::hidl_string, 3,5> result0;
        hw_binder_proxy_->transpose2(arg0, [&](const ::android::hardware::hidl_array<::android::hardware::hidl_string, 3,5>& arg0){
            cout << "callback transpose2 called" << endl;
            result0 = arg0;
        });
        vector<float>* measured = vts_measurement.Stop();
        cout << "time " << (*measured)[0] << endl;
        result_msg->set_name("transpose2");
        VariableSpecificationMessage* result_val_0 = result_msg->add_return_type_hidl();
        result_val_0->set_type(TYPE_ARRAY);
        result_val_0->set_vector_size(1);
        for (int i = 0; i < 1; i++) {
            auto *result_val_0_array_i = result_val_0->add_vector_value();
            result_val_0_array_i->set_type(TYPE_ARRAY);
            result_val_0_array_i->set_vector_size(1);
            for (int i = 0; i < 1; i++) {
                auto *result_val_0_array_i_array_i = result_val_0_array_i->add_vector_value();
                result_val_0_array_i_array_i->set_type(TYPE_STRING);
                result_val_0_array_i_array_i->mutable_string_value()->set_message(result0[i][i].c_str());
                result_val_0_array_i_array_i->mutable_string_value()->set_length(result0[i][i].size());
            }
        }
        cout << "called" << endl;
        return true;
    }
    if (!strcmp(func_name, "sendVec")) {
         ::android::hardware::hidl_vec<uint8_t> arg0;
        arg0.resize(func_msg.arg(0).vector_value_size());
        for (int i = 0; i <func_msg.arg(0).vector_value_size(); i++) {
            arg0[i] = func_msg.arg(0).vector_value(i).scalar_value().uint8_t();
        }
        VtsMeasurement vts_measurement;
        vts_measurement.Start();
        cout << "Call an API" << endl;
        cout << "local_device = " << hw_binder_proxy_.get() << endl;
        ::android::hardware::hidl_vec<uint8_t> result0;
        hw_binder_proxy_->sendVec(arg0, [&](const ::android::hardware::hidl_vec<uint8_t>& arg0){
            cout << "callback sendVec called" << endl;
            result0 = arg0;
        });
        vector<float>* measured = vts_measurement.Stop();
        cout << "time " << (*measured)[0] << endl;
        result_msg->set_name("sendVec");
        VariableSpecificationMessage* result_val_0 = result_msg->add_return_type_hidl();
        result_val_0->set_type(TYPE_VECTOR);
        result_val_0->set_vector_size(result0.size());
        for (int i = 0; i < (int)result0.size(); i++) {
            auto *result_val_0_vector_i = result_val_0->add_vector_value();
            result_val_0_vector_i->set_type(TYPE_SCALAR);
            result_val_0_vector_i->set_scalar_type("uint8_t");
            result_val_0_vector_i->mutable_scalar_value()->set_uint8_t(result0[i]);
        }
        cout << "called" << endl;
        return true;
    }
    if (!strcmp(func_name, "sendVecVec")) {
        VtsMeasurement vts_measurement;
        vts_measurement.Start();
        cout << "Call an API" << endl;
        cout << "local_device = " << hw_binder_proxy_.get() << endl;
        ::android::hardware::hidl_vec<::android::hardware::hidl_vec<uint8_t>> result0;
        hw_binder_proxy_->sendVecVec([&](const ::android::hardware::hidl_vec<::android::hardware::hidl_vec<uint8_t>>& arg0){
            cout << "callback sendVecVec called" << endl;
            result0 = arg0;
        });
        vector<float>* measured = vts_measurement.Stop();
        cout << "time " << (*measured)[0] << endl;
        result_msg->set_name("sendVecVec");
        VariableSpecificationMessage* result_val_0 = result_msg->add_return_type_hidl();
        result_val_0->set_type(TYPE_VECTOR);
        result_val_0->set_vector_size(result0.size());
        for (int i = 0; i < (int)result0.size(); i++) {
            auto *result_val_0_vector_i = result_val_0->add_vector_value();
            result_val_0_vector_i->set_type(TYPE_VECTOR);
            result_val_0_vector_i->set_vector_size(result0[i].size());
            for (int i = 0; i < (int)result0[i].size(); i++) {
                auto *result_val_0_vector_i_vector_i = result_val_0_vector_i->add_vector_value();
                result_val_0_vector_i_vector_i->set_type(TYPE_SCALAR);
                result_val_0_vector_i_vector_i->set_scalar_type("uint8_t");
                result_val_0_vector_i_vector_i->mutable_scalar_value()->set_uint8_t(result0[i][i]);
            }
        }
        cout << "called" << endl;
        return true;
    }
    if (!strcmp(func_name, "haveAVectorOfInterfaces")) {
         ::android::hardware::hidl_vec<sp<::android::hardware::tests::foo::V1_0::ISimple>> arg0;
        arg0.resize(func_msg.arg(0).vector_value_size());
        for (int i = 0; i <func_msg.arg(0).vector_value_size(); i++) {
            /* ERROR: TYPE_HIDL_INTERFACE is not supported yet. */
        }
        VtsMeasurement vts_measurement;
        vts_measurement.Start();
        cout << "Call an API" << endl;
        cout << "local_device = " << hw_binder_proxy_.get() << endl;
        ::android::hardware::hidl_vec<sp<::android::hardware::tests::foo::V1_0::ISimple>> result0;
        hw_binder_proxy_->haveAVectorOfInterfaces(arg0, [&](const ::android::hardware::hidl_vec<sp<::android::hardware::tests::foo::V1_0::ISimple>>& arg0){
            cout << "callback haveAVectorOfInterfaces called" << endl;
            result0 = arg0;
        });
        vector<float>* measured = vts_measurement.Stop();
        cout << "time " << (*measured)[0] << endl;
        result_msg->set_name("haveAVectorOfInterfaces");
        VariableSpecificationMessage* result_val_0 = result_msg->add_return_type_hidl();
        result_val_0->set_type(TYPE_VECTOR);
        result_val_0->set_vector_size(result0.size());
        for (int i = 0; i < (int)result0.size(); i++) {
            auto *result_val_0_vector_i = result_val_0->add_vector_value();
            result_val_0_vector_i->set_type(TYPE_HIDL_INTERFACE);
            result_val_0_vector_i->set_hidl_interface_pointer(reinterpret_cast<uintptr_t>(result0[i].get()));
            result_val_0_vector_i->set_predefined_type("::android::hardware::tests::foo::V1_0::ISimple");
        }
        cout << "called" << endl;
        return true;
    }
    if (!strcmp(func_name, "haveAVectorOfGenericInterfaces")) {
         ::android::hardware::hidl_vec<sp<::android::hidl::base::V1_0::IBase>> arg0;
        arg0.resize(func_msg.arg(0).vector_value_size());
        for (int i = 0; i <func_msg.arg(0).vector_value_size(); i++) {
            /* ERROR: TYPE_HIDL_INTERFACE is not supported yet. */
        }
        VtsMeasurement vts_measurement;
        vts_measurement.Start();
        cout << "Call an API" << endl;
        cout << "local_device = " << hw_binder_proxy_.get() << endl;
        ::android::hardware::hidl_vec<sp<::android::hidl::base::V1_0::IBase>> result0;
        hw_binder_proxy_->haveAVectorOfGenericInterfaces(arg0, [&](const ::android::hardware::hidl_vec<sp<::android::hidl::base::V1_0::IBase>>& arg0){
            cout << "callback haveAVectorOfGenericInterfaces called" << endl;
            result0 = arg0;
        });
        vector<float>* measured = vts_measurement.Stop();
        cout << "time " << (*measured)[0] << endl;
        result_msg->set_name("haveAVectorOfGenericInterfaces");
        VariableSpecificationMessage* result_val_0 = result_msg->add_return_type_hidl();
        result_val_0->set_type(TYPE_VECTOR);
        result_val_0->set_vector_size(result0.size());
        for (int i = 0; i < (int)result0.size(); i++) {
            auto *result_val_0_vector_i = result_val_0->add_vector_value();
            result_val_0_vector_i->set_type(TYPE_HIDL_INTERFACE);
            result_val_0_vector_i->set_hidl_interface_pointer(reinterpret_cast<uintptr_t>(result0[i].get()));
            result_val_0_vector_i->set_predefined_type("::android::hidl::base::V1_0::IBase");
        }
        cout << "called" << endl;
        return true;
    }
    if (!strcmp(func_name, "echoNullInterface")) {
        sp<::android::hardware::tests::foo::V1_0::IFooCallback> arg0;
        arg0 = VtsFuzzerCreateVts_android_hardware_tests_foo_V1_0_IFooCallback(callback_socket_name);
        static_cast<Vts_android_hardware_tests_foo_V1_0_IFooCallback*>(arg0.get())->Register(func_msg.arg(0));
        VtsMeasurement vts_measurement;
        vts_measurement.Start();
        cout << "Call an API" << endl;
        cout << "local_device = " << hw_binder_proxy_.get() << endl;
        bool result0;
        sp<::android::hardware::tests::foo::V1_0::IFooCallback> result1;
        hw_binder_proxy_->echoNullInterface(arg0, [&](bool arg0,const sp<::android::hardware::tests::foo::V1_0::IFooCallback>& arg1){
            cout << "callback echoNullInterface called" << endl;
            result0 = arg0;
            result1 = arg1;
        });
        vector<float>* measured = vts_measurement.Stop();
        cout << "time " << (*measured)[0] << endl;
        result_msg->set_name("echoNullInterface");
        VariableSpecificationMessage* result_val_0 = result_msg->add_return_type_hidl();
        result_val_0->set_type(TYPE_SCALAR);
        result_val_0->set_scalar_type("bool_t");
        result_val_0->mutable_scalar_value()->set_bool_t(result0);
        VariableSpecificationMessage* result_val_1 = result_msg->add_return_type_hidl();
        result_val_1->set_type(TYPE_HIDL_CALLBACK);
        /* ERROR: TYPE_HIDL_CALLBACK is not supported yet. */
        cout << "called" << endl;
        return true;
    }
    if (!strcmp(func_name, "createMyHandle")) {
        VtsMeasurement vts_measurement;
        vts_measurement.Start();
        cout << "Call an API" << endl;
        cout << "local_device = " << hw_binder_proxy_.get() << endl;
        ::android::hardware::tests::foo::V1_0::IFoo::MyHandle result0;
        hw_binder_proxy_->createMyHandle([&](const ::android::hardware::tests::foo::V1_0::IFoo::MyHandle& arg0){
            cout << "callback createMyHandle called" << endl;
            result0 = arg0;
        });
        vector<float>* measured = vts_measurement.Stop();
        cout << "time " << (*measured)[0] << endl;
        result_msg->set_name("createMyHandle");
        VariableSpecificationMessage* result_val_0 = result_msg->add_return_type_hidl();
        result_val_0->set_type(TYPE_STRUCT);
        SetResult__android__hardware__tests__foo__V1_0__IFoo__MyHandle(result_val_0, result0);
        cout << "called" << endl;
        return true;
    }
    if (!strcmp(func_name, "createHandles")) {
        uint32_t arg0;
        arg0 = 0;
        arg0 = func_msg.arg(0).scalar_value().uint32_t();
        VtsMeasurement vts_measurement;
        vts_measurement.Start();
        cout << "Call an API" << endl;
        cout << "local_device = " << hw_binder_proxy_.get() << endl;
        ::android::hardware::hidl_vec<::android::hardware::hidl_handle> result0;
        hw_binder_proxy_->createHandles(arg0, [&](const ::android::hardware::hidl_vec<::android::hardware::hidl_handle>& arg0){
            cout << "callback createHandles called" << endl;
            result0 = arg0;
        });
        vector<float>* measured = vts_measurement.Stop();
        cout << "time " << (*measured)[0] << endl;
        result_msg->set_name("createHandles");
        VariableSpecificationMessage* result_val_0 = result_msg->add_return_type_hidl();
        result_val_0->set_type(TYPE_VECTOR);
        result_val_0->set_vector_size(result0.size());
        for (int i = 0; i < (int)result0.size(); i++) {
            auto *result_val_0_vector_i = result_val_0->add_vector_value();
            result_val_0_vector_i->set_type(TYPE_HANDLE);
            /* ERROR: TYPE_HANDLE is not supported yet. */
        }
        cout << "called" << endl;
        return true;
    }
    if (!strcmp(func_name, "closeHandles")) {
        VtsMeasurement vts_measurement;
        vts_measurement.Start();
        cout << "Call an API" << endl;
        cout << "local_device = " << hw_binder_proxy_.get() << endl;
        hw_binder_proxy_->closeHandles();
        vector<float>* measured = vts_measurement.Stop();
        cout << "time " << (*measured)[0] << endl;
        result_msg->set_name("closeHandles");
        cout << "called" << endl;
        return true;
    }
    if (!strcmp(func_name, "thisIsNew")) {
        VtsMeasurement vts_measurement;
        vts_measurement.Start();
        cout << "Call an API" << endl;
        cout << "local_device = " << hw_binder_proxy_.get() << endl;
        hw_binder_proxy_->thisIsNew();
        vector<float>* measured = vts_measurement.Stop();
        cout << "time " << (*measured)[0] << endl;
        result_msg->set_name("thisIsNew");
        cout << "called" << endl;
        return true;
    }
    if (!strcmp(func_name, "expectNullHandle")) {
        ::android::hardware::hidl_handle arg0;
        /* ERROR: TYPE_HANDLE is not supported yet. */
        ::android::hardware::tests::foo::V1_0::Abc arg1;
        MessageTo__android__hardware__tests__foo__V1_0__Abc(func_msg.arg(1), &(arg1));
        VtsMeasurement vts_measurement;
        vts_measurement.Start();
        cout << "Call an API" << endl;
        cout << "local_device = " << hw_binder_proxy_.get() << endl;
        bool result0;
        bool result1;
        hw_binder_proxy_->expectNullHandle(arg0,arg1, [&](bool arg0,bool arg1){
            cout << "callback expectNullHandle called" << endl;
            result0 = arg0;
            result1 = arg1;
        });
        vector<float>* measured = vts_measurement.Stop();
        cout << "time " << (*measured)[0] << endl;
        result_msg->set_name("expectNullHandle");
        VariableSpecificationMessage* result_val_0 = result_msg->add_return_type_hidl();
        result_val_0->set_type(TYPE_SCALAR);
        result_val_0->set_scalar_type("bool_t");
        result_val_0->mutable_scalar_value()->set_bool_t(result0);
        VariableSpecificationMessage* result_val_1 = result_msg->add_return_type_hidl();
        result_val_1->set_type(TYPE_SCALAR);
        result_val_1->set_scalar_type("bool_t");
        result_val_1->mutable_scalar_value()->set_bool_t(result1);
        cout << "called" << endl;
        return true;
    }
    if (!strcmp(func_name, "takeAMask")) {
        ::android::hardware::tests::foo::V1_0::IFoo::BitField arg0;
        arg0 = EnumValue__android__hardware__tests__foo__V1_0__IFoo__BitField(func_msg.arg(0).scalar_value());
        uint8_t arg1;
        arg1 = func_msg.arg(1).scalar_value().uint8_t();
        ::android::hardware::tests::foo::V1_0::IFoo::MyMask arg2;
        MessageTo__android__hardware__tests__foo__V1_0__IFoo__MyMask(func_msg.arg(2), &(arg2));
        uint8_t arg3;
        arg3 = func_msg.arg(3).scalar_value().uint8_t();
        VtsMeasurement vts_measurement;
        vts_measurement.Start();
        cout << "Call an API" << endl;
        cout << "local_device = " << hw_binder_proxy_.get() << endl;
        ::android::hardware::tests::foo::V1_0::IFoo::BitField result0;
        uint8_t result1;
        uint8_t result2;
        uint8_t result3;
        hw_binder_proxy_->takeAMask(arg0,arg1,arg2,arg3, [&](::android::hardware::tests::foo::V1_0::IFoo::BitField arg0,uint8_t arg1,uint8_t arg2,uint8_t arg3){
            cout << "callback takeAMask called" << endl;
            result0 = arg0;
            result1 = arg1;
            result2 = arg2;
            result3 = arg3;
        });
        vector<float>* measured = vts_measurement.Stop();
        cout << "time " << (*measured)[0] << endl;
        result_msg->set_name("takeAMask");
        VariableSpecificationMessage* result_val_0 = result_msg->add_return_type_hidl();
        result_val_0->set_type(TYPE_ENUM);
        SetResult__android__hardware__tests__foo__V1_0__IFoo__BitField(result_val_0, result0);
        VariableSpecificationMessage* result_val_1 = result_msg->add_return_type_hidl();
        result_val_1->set_type(TYPE_SCALAR);
        result_val_1->set_scalar_type("uint8_t");
        result_val_1->mutable_scalar_value()->set_uint8_t(result1);
        VariableSpecificationMessage* result_val_2 = result_msg->add_return_type_hidl();
        result_val_2->set_type(TYPE_SCALAR);
        result_val_2->set_scalar_type("uint8_t");
        result_val_2->mutable_scalar_value()->set_uint8_t(result2);
        VariableSpecificationMessage* result_val_3 = result_msg->add_return_type_hidl();
        result_val_3->set_type(TYPE_SCALAR);
        result_val_3->set_scalar_type("uint8_t");
        result_val_3->mutable_scalar_value()->set_uint8_t(result3);
        cout << "called" << endl;
        return true;
    }
    if (!strcmp(func_name, "haveAInterface")) {
        sp<::android::hardware::tests::foo::V1_0::ISimple> arg0;
        /* ERROR: TYPE_HIDL_INTERFACE is not supported yet. */
        VtsMeasurement vts_measurement;
        vts_measurement.Start();
        cout << "Call an API" << endl;
        cout << "local_device = " << hw_binder_proxy_.get() << endl;
        sp<::android::hardware::tests::foo::V1_0::ISimple> result0;
        result0 = hw_binder_proxy_->haveAInterface(arg0);
        vector<float>* measured = vts_measurement.Stop();
        cout << "time " << (*measured)[0] << endl;
        result_msg->set_name("haveAInterface");
        VariableSpecificationMessage* result_val_0 = result_msg->add_return_type_hidl();
        result_val_0->set_type(TYPE_HIDL_INTERFACE);
        result_val_0->set_hidl_interface_pointer(reinterpret_cast<uintptr_t>(result0.get()));
        result_val_0->set_predefined_type("::android::hardware::tests::foo::V1_0::ISimple");
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

bool FuzzerExtended_android_hardware_tests_bar_V1_0_IBar::VerifyResults(const FunctionSpecificationMessage& expected_result, const FunctionSpecificationMessage& actual_result) {
    if (!strcmp(actual_result.name().c_str(), "doThis")) {
        if (actual_result.return_type_hidl_size() != expected_result.return_type_hidl_size() ) { return false; }
        return true;
    }
    if (!strcmp(actual_result.name().c_str(), "doThatAndReturnSomething")) {
        if (actual_result.return_type_hidl_size() != expected_result.return_type_hidl_size() ) { return false; }
        if (actual_result.return_type_hidl(0).scalar_value().int32_t() != expected_result.return_type_hidl(0).scalar_value().int32_t()) { return false; }
        return true;
    }
    if (!strcmp(actual_result.name().c_str(), "doQuiteABit")) {
        if (actual_result.return_type_hidl_size() != expected_result.return_type_hidl_size() ) { return false; }
        if (actual_result.return_type_hidl(0).scalar_value().double_t() != expected_result.return_type_hidl(0).scalar_value().double_t()) { return false; }
        return true;
    }
    if (!strcmp(actual_result.name().c_str(), "doSomethingElse")) {
        if (actual_result.return_type_hidl_size() != expected_result.return_type_hidl_size() ) { return false; }
        if (actual_result.return_type_hidl(0).vector_value_size() != expected_result.return_type_hidl(0).vector_value_size()) {
            cerr << "Verification failed for vector size. expected: " << expected_result.return_type_hidl(0).vector_value_size() << " actual: " << actual_result.return_type_hidl(0).vector_value_size();
            return false;
        }
        for (int i = 0; i < expected_result.return_type_hidl(0).vector_value_size(); i++) {
            if (actual_result.return_type_hidl(0).vector_value(i).scalar_value().int32_t() != expected_result.return_type_hidl(0).vector_value(i).scalar_value().int32_t()) { return false; }
        }
        return true;
    }
    if (!strcmp(actual_result.name().c_str(), "doStuffAndReturnAString")) {
        if (actual_result.return_type_hidl_size() != expected_result.return_type_hidl_size() ) { return false; }
        if (strcmp(actual_result.return_type_hidl(0).string_value().message().c_str(), expected_result.return_type_hidl(0).string_value().message().c_str())!= 0){ return false; }
        return true;
    }
    if (!strcmp(actual_result.name().c_str(), "mapThisVector")) {
        if (actual_result.return_type_hidl_size() != expected_result.return_type_hidl_size() ) { return false; }
        if (actual_result.return_type_hidl(0).vector_value_size() != expected_result.return_type_hidl(0).vector_value_size()) {
            cerr << "Verification failed for vector size. expected: " << expected_result.return_type_hidl(0).vector_value_size() << " actual: " << actual_result.return_type_hidl(0).vector_value_size();
            return false;
        }
        for (int i = 0; i <expected_result.return_type_hidl(0).vector_value_size(); i++) {
            if (actual_result.return_type_hidl(0).vector_value(i).scalar_value().int32_t() != expected_result.return_type_hidl(0).vector_value(i).scalar_value().int32_t()) { return false; }
        }
        return true;
    }
    if (!strcmp(actual_result.name().c_str(), "callMe")) {
        if (actual_result.return_type_hidl_size() != expected_result.return_type_hidl_size() ) { return false; }
        return true;
    }
    if (!strcmp(actual_result.name().c_str(), "useAnEnum")) {
        if (actual_result.return_type_hidl_size() != expected_result.return_type_hidl_size() ) { return false; }
        if(!Verify__android__hardware__tests__foo__V1_0__IFoo__SomeEnum(expected_result.return_type_hidl(0), actual_result.return_type_hidl(0))) { return false; }
        return true;
    }
    if (!strcmp(actual_result.name().c_str(), "haveAGooberVec")) {
        if (actual_result.return_type_hidl_size() != expected_result.return_type_hidl_size() ) { return false; }
        return true;
    }
    if (!strcmp(actual_result.name().c_str(), "haveAGoober")) {
        if (actual_result.return_type_hidl_size() != expected_result.return_type_hidl_size() ) { return false; }
        return true;
    }
    if (!strcmp(actual_result.name().c_str(), "haveAGooberArray")) {
        if (actual_result.return_type_hidl_size() != expected_result.return_type_hidl_size() ) { return false; }
        return true;
    }
    if (!strcmp(actual_result.name().c_str(), "haveATypeFromAnotherFile")) {
        if (actual_result.return_type_hidl_size() != expected_result.return_type_hidl_size() ) { return false; }
        return true;
    }
    if (!strcmp(actual_result.name().c_str(), "haveSomeStrings")) {
        if (actual_result.return_type_hidl_size() != expected_result.return_type_hidl_size() ) { return false; }
        if (actual_result.return_type_hidl(0).vector_value_size() != expected_result.return_type_hidl(0).vector_value_size()) {
            cerr << "Verification failed for vector size. expected: " << expected_result.return_type_hidl(0).vector_value_size() << " actual: " << actual_result.return_type_hidl(0).vector_value_size();
            return false;
        }
        for (int i = 0; i < expected_result.return_type_hidl(0).vector_value_size(); i++) {
            if (strcmp(actual_result.return_type_hidl(0).vector_value(i).string_value().message().c_str(), expected_result.return_type_hidl(0).vector_value(i).string_value().message().c_str())!= 0){ return false; }
        }
        return true;
    }
    if (!strcmp(actual_result.name().c_str(), "haveAStringVec")) {
        if (actual_result.return_type_hidl_size() != expected_result.return_type_hidl_size() ) { return false; }
        if (actual_result.return_type_hidl(0).vector_value_size() != expected_result.return_type_hidl(0).vector_value_size()) {
            cerr << "Verification failed for vector size. expected: " << expected_result.return_type_hidl(0).vector_value_size() << " actual: " << actual_result.return_type_hidl(0).vector_value_size();
            return false;
        }
        for (int i = 0; i <expected_result.return_type_hidl(0).vector_value_size(); i++) {
            if (strcmp(actual_result.return_type_hidl(0).vector_value(i).string_value().message().c_str(), expected_result.return_type_hidl(0).vector_value(i).string_value().message().c_str())!= 0){ return false; }
        }
        return true;
    }
    if (!strcmp(actual_result.name().c_str(), "transposeMe")) {
        if (actual_result.return_type_hidl_size() != expected_result.return_type_hidl_size() ) { return false; }
        if (actual_result.return_type_hidl(0).vector_value_size() != expected_result.return_type_hidl(0).vector_value_size()) {
            cerr << "Verification failed for vector size. expected: " << expected_result.return_type_hidl(0).vector_value_size() << " actual: " << actual_result.return_type_hidl(0).vector_value_size();
            return false;
        }
        for (int i = 0; i < expected_result.return_type_hidl(0).vector_value_size(); i++) {
            if (actual_result.return_type_hidl(0).vector_value(i).vector_value_size() != expected_result.return_type_hidl(0).vector_value(i).vector_value_size()) {
                cerr << "Verification failed for vector size. expected: " << expected_result.return_type_hidl(0).vector_value(i).vector_value_size() << " actual: " << actual_result.return_type_hidl(0).vector_value(i).vector_value_size();
                return false;
            }
            for (int i = 0; i < expected_result.return_type_hidl(0).vector_value(i).vector_value_size(); i++) {
                if (actual_result.return_type_hidl(0).vector_value(i).vector_value(i).scalar_value().float_t() != expected_result.return_type_hidl(0).vector_value(i).vector_value(i).scalar_value().float_t()) { return false; }
            }
        }
        return true;
    }
    if (!strcmp(actual_result.name().c_str(), "callingDrWho")) {
        if (actual_result.return_type_hidl_size() != expected_result.return_type_hidl_size() ) { return false; }
        if (!Verify__android__hardware__tests__foo__V1_0__IFoo__MultiDimensional(expected_result.return_type_hidl(0), actual_result.return_type_hidl(0))) { return false; }
        return true;
    }
    if (!strcmp(actual_result.name().c_str(), "transpose")) {
        if (actual_result.return_type_hidl_size() != expected_result.return_type_hidl_size() ) { return false; }
        if (!Verify__android__hardware__tests__foo__V1_0__IFoo__StringMatrix3x5(expected_result.return_type_hidl(0), actual_result.return_type_hidl(0))) { return false; }
        return true;
    }
    if (!strcmp(actual_result.name().c_str(), "transpose2")) {
        if (actual_result.return_type_hidl_size() != expected_result.return_type_hidl_size() ) { return false; }
        if (actual_result.return_type_hidl(0).vector_value_size() != expected_result.return_type_hidl(0).vector_value_size()) {
            cerr << "Verification failed for vector size. expected: " << expected_result.return_type_hidl(0).vector_value_size() << " actual: " << actual_result.return_type_hidl(0).vector_value_size();
            return false;
        }
        for (int i = 0; i < expected_result.return_type_hidl(0).vector_value_size(); i++) {
            if (actual_result.return_type_hidl(0).vector_value(i).vector_value_size() != expected_result.return_type_hidl(0).vector_value(i).vector_value_size()) {
                cerr << "Verification failed for vector size. expected: " << expected_result.return_type_hidl(0).vector_value(i).vector_value_size() << " actual: " << actual_result.return_type_hidl(0).vector_value(i).vector_value_size();
                return false;
            }
            for (int i = 0; i < expected_result.return_type_hidl(0).vector_value(i).vector_value_size(); i++) {
                if (strcmp(actual_result.return_type_hidl(0).vector_value(i).vector_value(i).string_value().message().c_str(), expected_result.return_type_hidl(0).vector_value(i).vector_value(i).string_value().message().c_str())!= 0){ return false; }
            }
        }
        return true;
    }
    if (!strcmp(actual_result.name().c_str(), "sendVec")) {
        if (actual_result.return_type_hidl_size() != expected_result.return_type_hidl_size() ) { return false; }
        if (actual_result.return_type_hidl(0).vector_value_size() != expected_result.return_type_hidl(0).vector_value_size()) {
            cerr << "Verification failed for vector size. expected: " << expected_result.return_type_hidl(0).vector_value_size() << " actual: " << actual_result.return_type_hidl(0).vector_value_size();
            return false;
        }
        for (int i = 0; i <expected_result.return_type_hidl(0).vector_value_size(); i++) {
            if (actual_result.return_type_hidl(0).vector_value(i).scalar_value().uint8_t() != expected_result.return_type_hidl(0).vector_value(i).scalar_value().uint8_t()) { return false; }
        }
        return true;
    }
    if (!strcmp(actual_result.name().c_str(), "sendVecVec")) {
        if (actual_result.return_type_hidl_size() != expected_result.return_type_hidl_size() ) { return false; }
        if (actual_result.return_type_hidl(0).vector_value_size() != expected_result.return_type_hidl(0).vector_value_size()) {
            cerr << "Verification failed for vector size. expected: " << expected_result.return_type_hidl(0).vector_value_size() << " actual: " << actual_result.return_type_hidl(0).vector_value_size();
            return false;
        }
        for (int i = 0; i <expected_result.return_type_hidl(0).vector_value_size(); i++) {
            if (actual_result.return_type_hidl(0).vector_value(i).vector_value_size() != expected_result.return_type_hidl(0).vector_value(i).vector_value_size()) {
                cerr << "Verification failed for vector size. expected: " << expected_result.return_type_hidl(0).vector_value(i).vector_value_size() << " actual: " << actual_result.return_type_hidl(0).vector_value(i).vector_value_size();
                return false;
            }
            for (int i = 0; i <expected_result.return_type_hidl(0).vector_value(i).vector_value_size(); i++) {
                if (actual_result.return_type_hidl(0).vector_value(i).vector_value(i).scalar_value().uint8_t() != expected_result.return_type_hidl(0).vector_value(i).vector_value(i).scalar_value().uint8_t()) { return false; }
            }
        }
        return true;
    }
    if (!strcmp(actual_result.name().c_str(), "haveAVectorOfInterfaces")) {
        if (actual_result.return_type_hidl_size() != expected_result.return_type_hidl_size() ) { return false; }
        if (actual_result.return_type_hidl(0).vector_value_size() != expected_result.return_type_hidl(0).vector_value_size()) {
            cerr << "Verification failed for vector size. expected: " << expected_result.return_type_hidl(0).vector_value_size() << " actual: " << actual_result.return_type_hidl(0).vector_value_size();
            return false;
        }
        for (int i = 0; i <expected_result.return_type_hidl(0).vector_value_size(); i++) {
            /* ERROR: TYPE_HIDL_INTERFACE is not supported yet. */
        }
        return true;
    }
    if (!strcmp(actual_result.name().c_str(), "haveAVectorOfGenericInterfaces")) {
        if (actual_result.return_type_hidl_size() != expected_result.return_type_hidl_size() ) { return false; }
        if (actual_result.return_type_hidl(0).vector_value_size() != expected_result.return_type_hidl(0).vector_value_size()) {
            cerr << "Verification failed for vector size. expected: " << expected_result.return_type_hidl(0).vector_value_size() << " actual: " << actual_result.return_type_hidl(0).vector_value_size();
            return false;
        }
        for (int i = 0; i <expected_result.return_type_hidl(0).vector_value_size(); i++) {
            /* ERROR: TYPE_HIDL_INTERFACE is not supported yet. */
        }
        return true;
    }
    if (!strcmp(actual_result.name().c_str(), "echoNullInterface")) {
        if (actual_result.return_type_hidl_size() != expected_result.return_type_hidl_size() ) { return false; }
        if (actual_result.return_type_hidl(0).scalar_value().bool_t() != expected_result.return_type_hidl(0).scalar_value().bool_t()) { return false; }
        /* ERROR: TYPE_HIDL_CALLBACK is not supported yet. */
        return true;
    }
    if (!strcmp(actual_result.name().c_str(), "createMyHandle")) {
        if (actual_result.return_type_hidl_size() != expected_result.return_type_hidl_size() ) { return false; }
        if (!Verify__android__hardware__tests__foo__V1_0__IFoo__MyHandle(expected_result.return_type_hidl(0), actual_result.return_type_hidl(0))) { return false; }
        return true;
    }
    if (!strcmp(actual_result.name().c_str(), "createHandles")) {
        if (actual_result.return_type_hidl_size() != expected_result.return_type_hidl_size() ) { return false; }
        if (actual_result.return_type_hidl(0).vector_value_size() != expected_result.return_type_hidl(0).vector_value_size()) {
            cerr << "Verification failed for vector size. expected: " << expected_result.return_type_hidl(0).vector_value_size() << " actual: " << actual_result.return_type_hidl(0).vector_value_size();
            return false;
        }
        for (int i = 0; i <expected_result.return_type_hidl(0).vector_value_size(); i++) {
            /* ERROR: TYPE_HANDLE is not supported yet. */
        }
        return true;
    }
    if (!strcmp(actual_result.name().c_str(), "closeHandles")) {
        if (actual_result.return_type_hidl_size() != expected_result.return_type_hidl_size() ) { return false; }
        return true;
    }
    if (!strcmp(actual_result.name().c_str(), "thisIsNew")) {
        if (actual_result.return_type_hidl_size() != expected_result.return_type_hidl_size() ) { return false; }
        return true;
    }
    if (!strcmp(actual_result.name().c_str(), "expectNullHandle")) {
        if (actual_result.return_type_hidl_size() != expected_result.return_type_hidl_size() ) { return false; }
        if (actual_result.return_type_hidl(0).scalar_value().bool_t() != expected_result.return_type_hidl(0).scalar_value().bool_t()) { return false; }
        if (actual_result.return_type_hidl(1).scalar_value().bool_t() != expected_result.return_type_hidl(1).scalar_value().bool_t()) { return false; }
        return true;
    }
    if (!strcmp(actual_result.name().c_str(), "takeAMask")) {
        if (actual_result.return_type_hidl_size() != expected_result.return_type_hidl_size() ) { return false; }
        if(!Verify__android__hardware__tests__foo__V1_0__IFoo__BitField(expected_result.return_type_hidl(0), actual_result.return_type_hidl(0))) { return false; }
        if (actual_result.return_type_hidl(1).scalar_value().uint8_t() != expected_result.return_type_hidl(1).scalar_value().uint8_t()) { return false; }
        if (actual_result.return_type_hidl(2).scalar_value().uint8_t() != expected_result.return_type_hidl(2).scalar_value().uint8_t()) { return false; }
        if (actual_result.return_type_hidl(3).scalar_value().uint8_t() != expected_result.return_type_hidl(3).scalar_value().uint8_t()) { return false; }
        return true;
    }
    if (!strcmp(actual_result.name().c_str(), "haveAInterface")) {
        if (actual_result.return_type_hidl_size() != expected_result.return_type_hidl_size() ) { return false; }
        /* ERROR: TYPE_HIDL_INTERFACE is not supported yet. */
        return true;
    }
    return false;
}

extern "C" {
android::vts::FuzzerBase* vts_func_4_android_hardware_tests_bar_1_IBar_() {
    return (android::vts::FuzzerBase*) new android::vts::FuzzerExtended_android_hardware_tests_bar_V1_0_IBar();
}

android::vts::FuzzerBase* vts_func_4_android_hardware_tests_bar_1_IBar_with_arg(uint64_t hw_binder_proxy) {
    return (android::vts::FuzzerBase*)
        new android::vts::FuzzerExtended_android_hardware_tests_bar_V1_0_IBar(
            reinterpret_cast<::android::hardware::tests::bar::V1_0::IBar*>(
                hw_binder_proxy));
}

}
}  // namespace vts
}  // namespace android
