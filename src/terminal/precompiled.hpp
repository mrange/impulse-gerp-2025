#pragma once

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX             // Avoid conflict with C++ std::min and std::max
#define STRICT               // Enforce type safety
#define WINVER 0x0A00        // Target Windows 10
#define _WIN32_WINNT 0x0A00  // Match WINVER

#include <windows.h>
#include <mfapi.h>
#include <mfplay.h>
#include <mfreadwrite.h>
#include <mferror.h>

#include <gl/GL.h>
#include "glext.h"

#include <mmintrin.h>
#include <xmmintrin.h>



#include <algorithm>
#include <array>
#include <cassert>
#include <cmath>
#include <condition_variable>
#include <cstdint>
#include <cstdlib>
#include <iterator>
#include <format>
#include <functional>
#include <map>
#include <memory>
#include <mutex>
#include <random>
#include <string>
#include <thread>
#include <type_traits>
#include <vector>

#pragma warning(disable : 4100)

#ifdef _DEBUG
#else
// Because assert conditions are not evaluated in release
#pragma warning(disable : 4189)
#endif

