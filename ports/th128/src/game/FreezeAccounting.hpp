#pragma once
#include <cstdint>
#include <optional>
namespace th128::freeze {
struct Accounting {float percent=0;int bonus=0,score=0;};
std::optional<Accounting> account(std::uint32_t pixels,int score) noexcept;
}
