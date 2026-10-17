// WMI client interface surface for the Emscripten/SDL3 (TH_SDL3) build.
// The reconstructed input_win32.cpp uses IWbemLocator/IWbemServices to query
// Win32_PNPEntity for XInput device detection; the declarations live in the
// windows.h shim. Implementations are a Phase 3 host concern.
#pragma once
#include "windows.h"
