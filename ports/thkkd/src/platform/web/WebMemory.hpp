#pragma once
// 原内存实现作为差分参照保留；Web 使用同一接口的地址索引后台。
#include "core/runtime/GameStartEntryContext.hpp"
#include "core/frontend/Replay.hpp"
#include <algorithm>
#include <cstring>
#include <map>
#include <type_traits>
#include <stdexcept>
#define GameStartEntryMemory ReferenceGameStartEntryMemory
#include "core/runtime/GameStartEntryMemory.hpp"
#undef GameStartEntryMemory
#include "IndexedGameMemory.hpp"
