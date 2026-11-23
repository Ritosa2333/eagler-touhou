#include "ResourceCrypt.hpp"
#include <algorithm>

namespace th16 {

bool resource_crypt(const u8* in, u8* out, u32 size, CryptParams p, bool encrypt) noexcept {
    if (!p.block) return false;
    // 0x402220：iVar5 = size % block；若 iVar5 >= block/4 则 iVar5 = 0；
    // 处理长度 = (size & ~1) - iVar5。
    const u32 untouched = (size % p.block < p.block / 4 ? size % p.block : 0) + (size & 1);
    i64 remaining = i64(size) - untouched, limit = p.limit;
    u32 cursor = 0;
    while (remaining > 0 && limit > 0) {
        const u32 block = std::min<u32>(p.block, u32(remaining));
        u32 linear = 0;
        // 原版：先 (block+1)/2 个字节写到奇偶位置 block-1, block-3, ...；再 block/2 个写到 block-2, block-4, ...
        for (i32 parity = 1; parity <= 2; ++parity) {
            for (i32 pos = i32(block) - parity; pos >= 0; pos -= 2) {
                if (encrypt) out[cursor + linear] = u8(in[cursor + pos] ^ p.key);
                else out[cursor + pos] = u8(in[cursor + linear] ^ p.key);
                p.key = u8(p.key + p.step);
                ++linear;
            }
        }
        cursor += block;
        remaining -= block;
        limit -= block;
    }
    if (cursor < size) std::memcpy(out + cursor, in + cursor, size - cursor);
    return true;
}

}  // namespace th16
