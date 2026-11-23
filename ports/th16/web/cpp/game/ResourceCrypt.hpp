// th16 资源加解密——原版 0x402220 的行为镜像（THA1 头、目录表、各条目共用）。
//
// 0x402220(buffer, size, key, step, block, limit)：
//   把 buffer 复制一份作为源，按 block 分块；每块先写奇位置（从块尾向前 2 步），
//   再写偶位置，逐字节 XOR key 后 key += step。处理字节数受 limit 限制，
//   当 size % block < block/4 时尾部零头保留原样；奇数 size 时最后 1 字节不处理。
#pragma once
#include "Types.hpp"

namespace th16 {

struct CryptParams {
    u8 key, step;
    u32 block, limit;
};

// 源与目标不得重叠。encrypt=false 复现原版 0x402220 对文件字节的那一次调用（读线性、写交错），
// encrypt=true 是它的逆映射，供录像/存档写出与往返测试使用。
bool resource_crypt(const u8* source, u8* destination, u32 size, CryptParams params, bool encrypt) noexcept;

}  // namespace th16
