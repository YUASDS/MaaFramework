#include "LibraryHolder/ControlUnit.h"

#include "ControlUnit/AdbControlUnitAPI.h"
#include "ControlUnit/DbgControlUnitAPI.h"
#include "ControlUnit/ThriftControlUnitAPI.h"
#include "ControlUnit/Win32ControlUnitAPI.h"
#include "Utils/Logger.h"

MAA_NS_BEGIN

template <typename ControlUnitT, typename GetVersionT>
bool check_version(const std::string& func_name)
{
    auto version_func = ControlUnitT::template get_function<GetVersionT>(func_name);
    if (!version_func) {
        LogError << "Failed to get function get_version";
        return false;
    }
    auto version = version_func();
    LogInfo << typeid(ControlUnitT).name() << "Library version:" << version;

    if (std::strcmp(version, MAA_VERSION) != 0) {
        LogWarn << "ControlUnit and MaaFramework are not same version,"
                << "ControlUnit:" << version << "MaaFramework:" << MAA_VERSION;
    }
    return true;
}

std::shared_ptr<MAA_CTRL_UNIT_NS::ControlUnitAPI> AdbControlUnitLibraryHolder::create_control_unit(
    MaaStringView adb_path, MaaStringView adb_serial, MaaAdbControllerType type, MaaStringView config,
    MaaStringView agent_path, MaaControllerCallback callback, MaaCallbackTransparentArg callback_arg)
{
    if (!load_library(libname_)) {
        LogError << "Failed to load library" << VAR(libname_);
        return nullptr;
    }

    check_version<AdbControlUnitLibraryHolder, decltype(MaaAdbControlUnitGetVersion)>(version_func_name_);

    auto create_control_unit_func = get_function<decltype(MaaAdbControlUnitCreate)>(create_func_name_);
    if (!create_control_unit_func) {
        LogError << "Failed to get function create_control_unit";
        return nullptr;
    }

    auto destroy_control_unit_func = get_function<decltype(MaaAdbControlUnitDestroy)>(destroy_func_name_);
    if (!destroy_control_unit_func) {
        LogError << "Failed to get function destroy_control_unit";
        return nullptr;
    }

    auto control_unit_handle = create_control_unit_func( //
        adb_path, adb_serial, type, config, agent_path, callback, callback_arg);

    if (!control_unit_handle) {
        LogError << "Failed to create control unit";
        return nullptr;
    }

    auto destroy_control_unit = [destroy_control_unit_func](MaaControlUnitHandle handle) {
        destroy_control_unit_func(handle);
        unload_library();
    };

    return std::shared_ptr<MAA_CTRL_UNIT_NS::ControlUnitAPI>(control_unit_handle, destroy_control_unit);
}

std::shared_ptr<MAA_CTRL_UNIT_NS::ControlUnitAPI> Win32ControlUnitLibraryHolder::create_control_unit(
    MaaWin32Hwnd hWnd, MaaWin32ControllerType type, MaaControllerCallback callback, MaaCallbackTransparentArg callback_arg)
{
    if (!load_library(libname_)) {
        LogError << "Failed to load library" << VAR(libname_);
        return nullptr;
    }

    check_version<Win32ControlUnitLibraryHolder, decltype(MaaWin32ControlUnitGetVersion)>(version_func_name_);

    auto create_control_unit_func = get_function<decltype(MaaWin32ControlUnitCreate)>(create_func_name_);
    if (!create_control_unit_func) {
        LogError << "Failed to get function create_control_unit";
        return nullptr;
    }

    auto destroy_control_unit_func = get_function<decltype(MaaWin32ControlUnitDestroy)>(destroy_func_name_);
    if (!destroy_control_unit_func) {
        LogError << "Failed to get function destroy_control_unit";
        return nullptr;
    }

    auto control_unit_handle = create_control_unit_func(hWnd, type, callback, callback_arg);

    if (!control_unit_handle) {
        LogError << "Failed to create control unit";
        return nullptr;
    }

    auto destroy_control_unit = [destroy_control_unit_func](MaaControlUnitHandle handle) {
        destroy_control_unit_func(handle);
        unload_library();
    };

    return std::shared_ptr<MAA_CTRL_UNIT_NS::ControlUnitAPI>(control_unit_handle, destroy_control_unit);
}

std::shared_ptr<MAA_CTRL_UNIT_NS::ControlUnitAPI> DbgControlUnitLibraryHolder::create_control_unit(
    MaaDbgControllerType type, MaaStringView read_path)
{
    if (!load_library(libname_)) {
        LogError << "Failed to load library" << VAR(libname_);
        return nullptr;
    }

    check_version<DbgControlUnitLibraryHolder, decltype(MaaDbgControlUnitGetVersion)>(version_func_name_);

    auto create_control_unit_func = get_function<decltype(MaaDbgControlUnitCreate)>(create_func_name_);
    if (!create_control_unit_func) {
        LogError << "Failed to get function create_control_unit";
        return nullptr;
    }

    auto destroy_control_unit_func = get_function<decltype(MaaDbgControlUnitDestroy)>(destroy_func_name_);
    if (!destroy_control_unit_func) {
        LogError << "Failed to get function destroy_control_unit";
        return nullptr;
    }

    auto control_unit_handle = create_control_unit_func(type, read_path);

    if (!control_unit_handle) {
        LogError << "Failed to create control unit";
        return nullptr;
    }

    auto destroy_control_unit = [destroy_control_unit_func](MaaControlUnitHandle handle) {
        destroy_control_unit_func(handle);
        unload_library();
    };

    return std::shared_ptr<MAA_CTRL_UNIT_NS::ControlUnitAPI>(control_unit_handle, destroy_control_unit);
}

std::shared_ptr<MAA_CTRL_UNIT_NS::ControlUnitAPI> ThriftControlUnitLibraryHolder::create_control_unit(
    MaaThriftControllerType type, MaaStringView host, int32_t port, MaaStringView config)
{
    if (!load_library(libname_)) {
        LogError << "Failed to load library" << VAR(libname_);
        return nullptr;
    }

    check_version<ThriftControlUnitLibraryHolder, decltype(MaaThriftControlUnitGetVersion)>(version_func_name_);

    auto create_control_unit_func = get_function<decltype(MaaThriftControlUnitCreate)>(create_func_name_);
    if (!create_control_unit_func) {
        LogError << "Failed to get function create_control_unit";
        return nullptr;
    }

    auto destroy_control_unit_func = get_function<decltype(MaaThriftControlUnitDestroy)>(destroy_func_name_);
    if (!destroy_control_unit_func) {
        LogError << "Failed to get function destroy_control_unit";
        return nullptr;
    }

    auto control_unit_handle = create_control_unit_func(type, host, port, config);

    if (!control_unit_handle) {
        LogError << "Failed to create control unit";
        return nullptr;
    }

    auto destroy_control_unit = [destroy_control_unit_func](MaaControlUnitHandle handle) {
        destroy_control_unit_func(handle);
        unload_library();
    };

    return std::shared_ptr<MAA_CTRL_UNIT_NS::ControlUnitAPI>(control_unit_handle, destroy_control_unit);
}

MAA_NS_END
