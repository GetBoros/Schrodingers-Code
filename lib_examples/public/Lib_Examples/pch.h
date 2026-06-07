//------------------------------------------------------------------------------------------------------------
#pragma once
//------------------------------------------------------------------------------------------------------------
#include <iostream>
#include <vector>
#include <string>
#include <chrono>
#include <coroutine>
#include <thread>
#include <functional>
#include <condition_variable>
#include <fcntl.h>
#include <unistd.h>
#include <cstring>
#include <linux/uinput.h>
//------------------------------------------------------------------------------------------------------------
#include "lib_examples.h"
//------------------------------------------------------------------------------------------------------------
#include <Lib_Examples/Object.h>
#include <Lib_Examples/Example_Task.h>
#include <Lib_Examples/Example_Clicker.h>
#include <Lib_Examples/Example_Constexpr.hpp>
#include <Lib_Examples/Task.h>
#include <Lib_Examples/Task_Manager.h>
#include <Lib_Examples/Task_Awaiter.h>
//------------------------------------------------------------------------------------------------------------
namespace Platform {
#ifdef __linux__
    inline constexpr bool IsLinux = true;
    inline constexpr bool IsWindows = false;
#elif _WIN32
    inline constexpr bool IsLinux = false;
    inline constexpr bool IsWindows = true;
#else
    inline constexpr bool IsLinux = false;
    inline constexpr bool IsWindows = false;
#endif
}
//------------------------------------------------------------------------------------------------------------
