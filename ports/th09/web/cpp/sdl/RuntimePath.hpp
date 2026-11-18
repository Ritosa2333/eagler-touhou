#pragma once

namespace th09 {
// The browser mounts a virtual root. The desktop host runs from its content directory.
constexpr const char* runtime_path(const char* rooted_path) {
#ifdef _WIN32
    return rooted_path + 1;
#else
    return rooted_path;
#endif
}
}
