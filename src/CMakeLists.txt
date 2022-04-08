cmake_minimum_required (VERSION 3.8)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)

add_library(gta-antisig MODULE "Entry.cpp"  "CommonIncludes.hpp" "CommonIncludes.cpp" "gta/Include.hpp" "gta/Singleton.hpp" "memory/Include.hpp"  "memory/Memory.hpp" "gta/Tunables.hpp" "memory/MemoryPatch.hpp" "memory/MemoryPatch.cpp" "memory/ByteArray.hpp" "util/Log.hpp")

if (MSVC)
	# is this used on default my msvc?
	add_compile_options("/EHa")
endif()