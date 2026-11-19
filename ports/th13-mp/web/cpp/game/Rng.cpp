#include "game/Rng.hpp"

// 完整实现在头文件内（header-only 便于 harness 与网页双目标复用）。
// 此编译单元仅保证 game/Rng.hpp 进入构建与来源清单。
namespace th13 {
static_assert(Rng::kMultiplier != 0, "RNG 常量必须非零");
const bool kRngUnitLinked = true;
}  // namespace th13
