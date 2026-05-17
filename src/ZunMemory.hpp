#pragma once

#include <stdlib.h>

// In later games, ZUN uses a class we're calling "ZunMemory" to track allocated data for debugging
// purposes. Although this struct's full form does not seem to be present in EoSD, quirks in the
// codegen do reveal the existence of some early version of this system.

namespace th06
{
inline void *ZunAlloc(i32 size)
{
    return malloc(size);
}

inline void ZunFree(void *ptr)
{
    free(ptr);
}
}; // namespace th06
