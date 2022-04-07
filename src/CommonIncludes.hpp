#pragma once

#define NOMINMAX
#define _CRT_SECURE_NO_WARNINGS
#define WIN32_LEAN_AND_MEAN
#define _WIN32_WINNT _WIN32_WINNT_WIN7

#include <windows.h>
#include <string>
#include <array>
#include <optional>
#include <vector>
#include <sstream>
#include <ostream>
#include <algorithm>
#include <unordered_map>
#include <thread>
#include <chrono>

using namespace std::chrono_literals;

namespace gta
{
	inline bool g_Running = true;
}