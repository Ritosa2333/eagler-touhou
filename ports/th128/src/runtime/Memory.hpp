#pragma once
#include <cstddef>
#include <cstdint>
#ifdef __EMSCRIPTEN__
#include <emscripten/heap.h>
#endif

namespace th128::runtime {
inline bool inputRange(const void* pointer, std::size_t length) {
    if (!pointer || !length) return false;
#ifdef __EMSCRIPTEN__
    const auto begin = reinterpret_cast<std::uintptr_t>(pointer);
    const auto size = emscripten_get_heap_size();
    return begin <= size && length <= size - begin;
#else
    return true;
#endif
}
}
