#ifndef __VTS_PROFILER_types.profiler__
#define __VTS_PROFILER_types.profiler__


#include <android-base/logging.h>
#include <hidl/HidlSupport.h>
#include <test/vts/proto/ComponentSpecificationMessage.pb.h>
#include <VtsProfilingInterface.h>

#include <android/hardware/nfc/1.0/types.h>


using namespace android::hardware;
using namespace android::hardware::nfc::V1_0;

namespace android {
namespace vts {

extern "C" {
    void profile__nfc_event_t(VariableSpecificationMessage* arg_name,
    nfc_event_t arg_val_name);
    void profile__nfc_status_t(VariableSpecificationMessage* arg_name,
    nfc_status_t arg_val_name);
    void profile__nfc_data_t(VariableSpecificationMessage* arg_name,
    nfc_data_t arg_val_name);
}

}  // namespace vts
}  // namespace android
#endif
