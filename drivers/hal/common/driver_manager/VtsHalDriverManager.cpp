/*
 * Copyright (C) 2017 The Android Open Source Project
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
#include "driver_manager/VtsHalDriverManager.h"

#include <iostream>
#include <string>

#include <google/protobuf/text_format.h>

#include "utils/InterfaceSpecUtil.h"
#include "utils/StringUtil.h"

static constexpr const char* kErrorString = "error";
static constexpr const char* kVoidString = "void";

namespace android {
namespace vts {

VtsHalDriverManager::VtsHalDriverManager(const string& spec_dir,
                                         const int epoch_count,
                                         const string& callback_socket_name)
    : callback_socket_name_(callback_socket_name),
      default_driver_lib_name_(""),
      spec_builder_(
          SpecificationBuilder(spec_dir, epoch_count, callback_socket_name)) {}

DriverId VtsHalDriverManager::LoadTargetComponent(
    const string& dll_file_name, const string& spec_lib_file_path,
    const int component_class, const int component_type, const float version,
    const string& package_name, const string& component_name,
    const string& hw_binder_service_name, const string& submodule_name) {
  cout << __func__ << " entry dll_file_name = " << dll_file_name << endl;
  ComponentSpecificationMessage spec_message;
  if (!spec_builder_.FindComponentSpecification(
          component_class, package_name, version, component_name,
          component_type, submodule_name, &spec_message)) {
    cerr << __func__ << ": Faild to load specification for component with "
         << "class: " << component_class << " type: " << component_type
         << " version: " << version << endl;
    return -1;
  }
  cout << "loaded specification for component with class: " << component_class
       << " type: " << component_type << " version: " << version << endl;

  string driver_lib_path = "";
  if (component_class == HAL_HIDL) {
    driver_lib_path = GetHidlHalDriverLibName(package_name, version);
  } else {
    driver_lib_path = spec_lib_file_path;
    default_driver_lib_name_ = driver_lib_path;
  }

  cout << __func__ << " driver lib path " << driver_lib_path << endl;

  std::unique_ptr<FuzzerBase> hal_driver = nullptr;
  hal_driver.reset(spec_builder_.GetDriver(driver_lib_path, spec_message,
                                           hw_binder_service_name, 0, false,
                                           dll_file_name, ""));
  if (!hal_driver) {
    cerr << "can't load driver for component with class: " << component_class
         << " type: " << component_type << " version: " << version << endl;
    return -1;
  }

  // TODO (zhuoyao): get hidl_proxy_pointer for loaded hidl hal dirver.
  uint64_t interface_pt = 0;
  return RegisterDriver(std::move(hal_driver), spec_message, "", interface_pt);
}

string VtsHalDriverManager::CallFunction(
    FunctionSpecificationMessage* func_msg) {
  FuzzerBase* driver = nullptr;
  int component_class = -1;
  string output = "";
  DriverId driver_id = FindDriverIdWithFuncMsg(func_msg);
  if (driver_id == -1) {
    cerr << "can't find driver for '" << func_msg->name() << endl;
    return kErrorString;
  }

  component_class = GetComponentSpecById(driver_id)->component_class();
  driver = GetDriverById(driver_id);
  if (!driver) {
    return kErrorString;
  }

  // Special process to open conventional hal.
  if (func_msg->name() == "#Open") {
    cout << __func__ << ":" << __LINE__ << " #Open" << endl;
    if (func_msg->arg().size() > 0) {
      cout << __func__ << " open conventional hal with arg:"
           << func_msg->arg(0).string_value().message() << endl;
      driver->OpenConventionalHal(
          func_msg->arg(0).string_value().message().c_str());
    } else {
      cout << __func__ << " open conventional hal with no arg" << endl;
      driver->OpenConventionalHal();
    }
    // return the return value from open;
    if (func_msg->return_type().has_type()) {
      cout << __func__ << " return_type exists" << endl;
      // TODO handle when the size > 1.
      if (!strcmp(func_msg->return_type().scalar_type().c_str(), "int32_t")) {
        cout << __func__ << " return_type is int32_t" << endl;
        func_msg->mutable_return_type()->mutable_scalar_value()->set_int32_t(0);
        cout << "result " << endl;
        // todo handle more types;
        google::protobuf::TextFormat::PrintToString(*func_msg, &output);
        return output;
      }
    }
    cerr << __func__ << " return_type unknown" << endl;
    google::protobuf::TextFormat::PrintToString(*func_msg, &output);
    return output;
  }

  void* result;
  FunctionSpecificationMessage result_msg;
  driver->FunctionCallBegin();
  cout << __func__ << " Call Function " << func_msg->name() << " parent_path("
       << func_msg->parent_path() << ")" << endl;
  // For Hidl HAL, use CallFunction method.
  if (component_class == HAL_HIDL) {
    if (!driver->CallFunction(*func_msg, callback_socket_name_, &result_msg)) {
      cerr << __func__
           << " Failed to call function: " << func_msg->DebugString() << endl;
      return kErrorString;
    }
  } else {
    if (!driver->Fuzz(func_msg, &result, callback_socket_name_)) {
      cerr << __func__
           << " Failed to call function: " << func_msg->DebugString() << endl;
      return kErrorString;
    }
  }
  cout << __func__ << ": called funcation " << func_msg->name() << endl;

  // set coverage data.
  driver->FunctionCallEnd(func_msg);

  if (component_class == HAL_HIDL) {
    for (int index = 0; index < result_msg.return_type_hidl_size(); index++) {
      auto* return_val = result_msg.mutable_return_type_hidl(index);
      if (return_val->type() == TYPE_HIDL_INTERFACE &&
          return_val->hidl_interface_pointer() != 0) {
        string type_name = return_val->predefined_type();
        uint64_t interface_pt = return_val->hidl_interface_pointer();
        std::unique_ptr<FuzzerBase> driver;
        ComponentSpecificationMessage spec_msg;
        string package_name = GetPackageName(type_name);
        float version = GetVersion(type_name);
        string component_name = GetComponentName(type_name);
        if (!spec_builder_.FindComponentSpecification(HAL_HIDL, package_name,
                                                      version, component_name,
                                                      0, "", &spec_msg)) {
          cerr << __func__
               << " Failed to load specification for gnerated interface :"
               << type_name << endl;
          return kErrorString;
        }
        string driver_lib_path = GetHidlHalDriverLibName(package_name, version);
        // TODO(zhuoyao): figure out a way to get the service_name.
        string hw_binder_service_name = "default";
        driver.reset(spec_builder_.GetDriver(driver_lib_path, spec_msg,
                                             hw_binder_service_name,
                                             interface_pt, true, "", ""));
        int32_t driver_id =
            RegisterDriver(std::move(driver), spec_msg, "", interface_pt);
        return_val->set_hidl_interface_id(driver_id);
      }
    }
    google::protobuf::TextFormat::PrintToString(result_msg, &output);
    return output;
  } else {
    return ProcessFuncResultsForConventionalHal(func_msg, result);
  }
  return kVoidString;
}

string VtsHalDriverManager::GetAttribute(
    FunctionSpecificationMessage* func_msg) {
  FuzzerBase* driver = nullptr;
  int component_class = -1;
  string output = "";
  DriverId driver_id = FindDriverIdWithFuncMsg(func_msg);
  if (driver_id == -1) {
    cerr << "can't find driver for '" << func_msg->name() << endl;
    return kErrorString;
  }
  driver = GetDriverById(driver_id);
  component_class = GetComponentSpecById(driver_id)->component_class();
  if (!driver) {
    return kErrorString;
  }

  void* result;
  cout << __func__ << " Get Atrribute " << func_msg->name() << " parent_path("
       << func_msg->parent_path() << ")" << endl;
  if (!driver->GetAttribute(func_msg, &result)) {
    cerr << __func__ << " attribute not found - todo handle more explicitly"
         << endl;
    return kErrorString;
  }
  cout << __func__ << ": called" << endl;

  if (component_class == HAL_HIDL) {
    cout << __func__ << ": for a HIDL HAL" << endl;
    func_msg->mutable_return_type()->set_type(TYPE_STRING);
    func_msg->mutable_return_type()->mutable_string_value()->set_message(
        *(string*)result);
    func_msg->mutable_return_type()->mutable_string_value()->set_length(
        ((string*)result)->size());
    free(result);
    string* output = new string();
    google::protobuf::TextFormat::PrintToString(*func_msg, output);
    return *output;
  } else {
    return ProcessFuncResultsForConventionalHal(func_msg, result);
  }
  return kVoidString;
}

DriverId VtsHalDriverManager::RegisterDriver(
    std::unique_ptr<FuzzerBase> driver,
    const ComponentSpecificationMessage& spec_msg, const string& submodule_name,
    const uint64_t interface_pt) {
  DriverId driver_id =
      FindDriverIdInternal(spec_msg, submodule_name, interface_pt, true);
  if (driver_id == -1) {
    driver_id = hal_driver_map_.size();
    hal_driver_map_.insert(
        make_pair(driver_id, HalDriverInfo(spec_msg, submodule_name,
                                           interface_pt, std::move(driver))));
  } else {
    cout << __func__ << " Driver already exists. ";
  }

  return driver_id;
}

FuzzerBase* VtsHalDriverManager::GetDriverById(const int32_t id) {
  auto res = hal_driver_map_.find(id);
  if (res == hal_driver_map_.end()) {
    cerr << "Failed to find driver info with id: " << id << endl;
    return nullptr;
  }
  cout << __func__ << " found driver info with id: " << id << endl;
  return res->second.driver.get();
}

ComponentSpecificationMessage* VtsHalDriverManager::GetComponentSpecById(
    const int32_t id) {
  auto res = hal_driver_map_.find(id);
  if (res == hal_driver_map_.end()) {
    cerr << "Failed to find driver info with id: " << id << endl;
    return nullptr;
  }
  cout << __func__ << " found driver info with id: " << id << endl;
  return &(res->second.spec_msg);
}

FuzzerBase* VtsHalDriverManager::GetDriverForHidlHalInterface(
    const string& package_name, const float version,
    const string& interface_name, const string& hal_service_name) {
  ComponentSpecificationMessage spec_msg;
  spec_msg.set_component_class(HAL_HIDL);
  spec_msg.set_package(package_name);
  spec_msg.set_component_type_version(version);
  spec_msg.set_component_name(interface_name);
  int32_t driver_id = FindDriverIdInternal(spec_msg);
  if (driver_id == -1) {
    string driver_lib_path = GetHidlHalDriverLibName(package_name, version);
    driver_id =
        LoadTargetComponent("", driver_lib_path, HAL_HIDL, 0, version,
                            package_name, interface_name, hal_service_name, "");
  }
  return GetDriverById(driver_id);
}

bool VtsHalDriverManager::FindComponentSpecification(
    const int component_class, const int component_type, const float version,
    const string& submodule_name, const string& package_name,
    const string& component_name, ComponentSpecificationMessage* spec_msg) {
  return spec_builder_.FindComponentSpecification(
      component_class, package_name, version, component_name, component_type,
      submodule_name, spec_msg);
}

ComponentSpecificationMessage*
VtsHalDriverManager::GetComponentSpecification() {
  if (hal_driver_map_.empty()) {
    return nullptr;
  } else {
    return &(hal_driver_map_.find(0)->second.spec_msg);
  }
}

DriverId VtsHalDriverManager::FindDriverIdInternal(
    const ComponentSpecificationMessage& spec_msg, const string& submodule_name,
    const uint64_t interface_pt, bool with_interface_pointer) {
  if (!spec_msg.has_component_class()) {
    cerr << __func__ << " Component class not specified. " << endl;
    return -1;
  }
  if (spec_msg.component_class() == HAL_HIDL) {
    if (!spec_msg.has_package() || spec_msg.package().empty()) {
      cerr << __func__ << " Package name is requried but not specified. "
           << endl;
      return -1;
    }
    if (!spec_msg.has_component_type_version()) {
      cerr << __func__ << " Package version is requried but not specified. "
           << endl;
      return -1;
    }
    if (!spec_msg.has_component_name() || spec_msg.component_name().empty()) {
      cerr << __func__ << " Component name is requried but not specified. "
           << endl;
      return -1;
    }
  } else {
    if (submodule_name.empty()) {
      cerr << __func__ << " Submodule name is requried but not specified. "
           << endl;
      return -1;
    }
  }
  for (auto it = hal_driver_map_.begin(); it != hal_driver_map_.end(); ++it) {
    ComponentSpecificationMessage cur_spec_msg = it->second.spec_msg;
    if (cur_spec_msg.component_class() != spec_msg.component_class()) {
      continue;
    }
    // If package name is specified, match package name.
    if (spec_msg.has_package()) {
      if (!cur_spec_msg.has_package() ||
          cur_spec_msg.package() != spec_msg.package()) {
        continue;
      }
    }
    // If version is specified, match version.
    if (spec_msg.has_component_type_version()) {
      if (!cur_spec_msg.has_component_type_version() ||
          cur_spec_msg.component_type_version() !=
              spec_msg.component_type_version()) {
        continue;
      }
    }
    if (spec_msg.component_class() == HAL_HIDL) {
      if (cur_spec_msg.component_name() != spec_msg.component_name()) {
        continue;
      }
      if (with_interface_pointer &&
          it->second.hidl_hal_proxy_pt != interface_pt) {
        continue;
      }
      cout << __func__ << " Found hidl hal driver with id: " << it->first
           << endl;
      return it->first;
    } else {
      if ((!spec_msg.has_component_type() ||
           cur_spec_msg.component_type() == spec_msg.component_type()) &&
          it->second.submodule_name == submodule_name) {
        cout << __func__
             << " Found conventional hal driver with id: " << it->first << endl;
        return it->first;
      }
    }
  }
  return -1;
}

DriverId VtsHalDriverManager::FindDriverIdWithFuncMsg(
    FunctionSpecificationMessage* func_msg) {
  if (func_msg->submodule_name().size() > 0) {
    string submodule_name = func_msg->submodule_name();
    cout << __func__ << " submodule name " << submodule_name << endl;
    DriverId driver_id = FindDriverIdWithSubModuleName(submodule_name);
    if (driver_id != -1) {
      cout << __func__ << " call is for a submodule" << endl;
      return driver_id;
    } else {
      cerr << __func__ << " called an API of a non-loaded submodule." << endl;
      return -1;
    }
  } else {
    // If hidl_interface_id is specified, get fuzzer base from the interface_map
    // directly.
    if (func_msg->hidl_interface_id() != 0) {
      return func_msg->hidl_interface_id();
    } else {
      // No driver is registered,
      if (hal_driver_map_.size() == 0) {
        cerr << __func__ << " called an API of a non-loaded hal." << endl;
        return -1;
      }
      // TODO(zhuoyao): under the current assumption, driver_id = 0 means the
      // Hal the initially loaded, need to change then when we support
      // multi-hal testing.
      return 0;
    }
  }
}

DriverId VtsHalDriverManager::FindDriverIdWithSubModuleName(
    const string& submodule_name) {
  ComponentSpecificationMessage spec_msg;
  spec_msg.set_component_class(HAL_CONVENTIONAL);
  return FindDriverIdInternal(spec_msg, submodule_name);
}

string VtsHalDriverManager::ProcessFuncResultsForConventionalHal(
    FunctionSpecificationMessage* func_msg, void* result) {
  string output = "";
  if (func_msg->return_type().type() == TYPE_PREDEFINED) {
    // TODO: actually handle this case.
    if (result != NULL) {
      // loads that interface spec and enqueues all functions.
      cout << __func__ << " return type: " << func_msg->return_type().type()
           << endl;
    } else {
      cout << __func__ << " return value = NULL" << endl;
    }
    cerr << __func__ << " todo: support aggregate" << endl;
    google::protobuf::TextFormat::PrintToString(*func_msg, &output);
    return output;
  } else if (func_msg->return_type().type() == TYPE_SCALAR) {
    // TODO handle when the size > 1.
    // todo handle more types;
    if (!strcmp(func_msg->return_type().scalar_type().c_str(), "int32_t")) {
      func_msg->mutable_return_type()->mutable_scalar_value()->set_int32_t(
          *((int*)(&result)));
      google::protobuf::TextFormat::PrintToString(*func_msg, &output);
      return output;
    } else if (!strcmp(func_msg->return_type().scalar_type().c_str(),
                       "uint32_t")) {
      func_msg->mutable_return_type()->mutable_scalar_value()->set_uint32_t(
          *((int*)(&result)));
      google::protobuf::TextFormat::PrintToString(*func_msg, &output);
      return output;
    } else if (!strcmp(func_msg->return_type().scalar_type().c_str(),
                       "int16_t")) {
      func_msg->mutable_return_type()->mutable_scalar_value()->set_int16_t(
          *((int*)(&result)));
      google::protobuf::TextFormat::PrintToString(*func_msg, &output);
      return output;
    } else if (!strcmp(func_msg->return_type().scalar_type().c_str(),
                       "uint16_t")) {
      google::protobuf::TextFormat::PrintToString(*func_msg, &output);
      return output;
    }
  } else if (func_msg->return_type().type() == TYPE_SUBMODULE) {
    cerr << __func__ << "[driver:hal] return type TYPE_SUBMODULE" << endl;
    if (result != NULL) {
      // loads that interface spec and enqueues all functions.
      cout << __func__ << " return type: " << func_msg->return_type().type()
           << endl;
    } else {
      cout << __func__ << " return value = NULL" << endl;
    }
    // find a VTS spec for that module
    string submodule_name = func_msg->return_type().predefined_type().substr(
        0, func_msg->return_type().predefined_type().size() - 1);
    ComponentSpecificationMessage submodule_iface_spec_msg;
    DriverId driver_id = FindDriverIdWithSubModuleName(submodule_name);
    if (driver_id != -1) {
      cout << __func__ << " submodule InterfaceSpecification already loaded"
           << endl;
      ComponentSpecificationMessage* spec_msg = GetComponentSpecById(driver_id);
      func_msg->set_allocated_return_type_submodule_spec(spec_msg);
    } else {
      // TODO(zhuoyao): under the current assumption, driver_id = 0 means the
      // Hal the initially loaded, need to change then when we support
      // multi-hal testing.
      ComponentSpecificationMessage* spec_msg = GetComponentSpecById(0);
      if (spec_builder_.FindComponentSpecification(
              spec_msg->component_class(), spec_msg->package(),
              spec_msg->component_type_version(), spec_msg->component_name(),
              spec_msg->component_type(), submodule_name,
              &submodule_iface_spec_msg)) {
        cout << __func__ << " submodule InterfaceSpecification found" << endl;
        func_msg->set_allocated_return_type_submodule_spec(
            &submodule_iface_spec_msg);
        std::unique_ptr<FuzzerBase> driver = nullptr;
        driver.reset(spec_builder_.GetDriverForSubModule(
            default_driver_lib_name_, submodule_iface_spec_msg, result));
        RegisterDriver(std::move(driver), submodule_iface_spec_msg,
                       submodule_name, 0);
      } else {
        cerr << __func__ << " submodule InterfaceSpecification not found"
             << endl;
      }
    }
    google::protobuf::TextFormat::PrintToString(*func_msg, &output);
    return output;
  }
  return kVoidString;
}

}  // namespace vts
}  // namespace android
