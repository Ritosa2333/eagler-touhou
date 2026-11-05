// TH12 1.00b portable C++ runtime. See NOTICE.md for rights.
#include "GameState.hpp"

namespace th12 {
bool Program::add(const u8* input, u32 size) {
  if (!input || size < 36 || size > 32u * 1024u * 1024u || rd<u32>(input) != 0x54504353 ||
      rd<u16>(input + 4) != 1)
    return false;
  const u32 table = 36 + rd<u16>(input + 6), count = rd<u16>(input + 16);
  if (table > size || count > (size - table) / 4)
    return false;
  std::vector<u8> candidate(input, input + size);
  const auto* b = candidate.data();
  u32 names = table + count * 4;
  std::vector<std::pair<std::string, u32>> entries;
  for (u32 i = 0; i < count; i++) {
    const u32 off = rd<u32>(b + table + i * 4), begin = names;
    while (names < size && b[names])
      names++;
    if (names == size || off > size - 16 || rd<u32>(b + off) != 0x484c4345)
      return false;
    entries.emplace_back(std::string(reinterpret_cast<const char*>(b + begin), names - begin),
                         off + 16);
    names++;
    u32 end = size;
    for (u32 j = 0; j < count; j++) {
      const u32 other = rd<u32>(b + table + j * 4);
      if (other > off)
        end = std::min(end, other);
    }
    for (u32 p = off + 16; p < end;) {
      if (end - p < 16)
        return false;
      const u32 n = rd<u16>(b + p + 6);
      if (n < 16 || n % 4 || n > end - p)
        return false;
      p += n;
    }
  }
  // Publish only after complete validation, preserving the existing program on
  // malformed input. Moving a vector retains its resource-owned byte addresses.
  files.push_back(std::move(candidate));
  for (const auto& [name, offset] : entries)
    routines[name] = files.back().data() + offset;
  return true;
}
const u8* Program::find(const std::string& name) const {
  const auto found = routines.find(name);
  return found == routines.end() ? nullptr : found->second;
}

} // namespace th12
