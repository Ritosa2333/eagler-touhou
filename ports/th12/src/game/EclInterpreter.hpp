#pragma once
#include "GameTypes.hpp"

namespace th12 {
float global(Enemy&, int id, bool floating = true);
i32 globalInt(Enemy&, int id);
i32 argInt(Context&, Enemy&, int index, int referenceIndex = -1);
float arg(Context&, Enemy&, int index, bool isFloat = false, int referenceIndex = -1);
void store(Context&, Enemy&, int id, float value, bool isFloat);
void storeInt(Context&, Enemy&, int id, i32 value);
std::string stringArg(Context&, int at = 0);
std::array<u32, 256> callArguments(Context&, Enemy&, int skipped = 0);
void command(Enemy&, Context&, int opcode);
void runContext(Enemy&, Context&);
} // namespace th12
