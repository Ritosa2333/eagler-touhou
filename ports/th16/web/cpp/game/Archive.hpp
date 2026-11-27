// th16.dat（THA1）档案——原版 0x4572e0（打开）/ 0x4574b0（目录）/ 0x457120（读条目）的行为镜像。
//
// 0x4572e0：读 16 字节头 → 0x402220(…,0x1b,0x37,0x10,0x10) → 魔数 "THA1"(0x31414854)；
//   条目数    = [12] − 135792468 (0x8180754)
//   目录明文  = [4]  − 123456789 (0x75BCD15)   （加 0xF8A432EB）
//   目录密文  = [8]  − 987654321 (0x3ADE68B1)  （加 0xC521974F）
//   目录位于文件末尾 packed 字节，0x402220(…,0x3e,0x9b,0x80,packed) 后经 0x457f00 LZSS 展开。
// 0x4574b0：目录项 = 以 0 结尾的名字（4 字节对齐）+ u32 偏移 + u32 原始大小 + u32 保留。
// 0x457120：条目密钥 = 名字字节和 & 7 → .data 0x49f278 的 8 组 {key, step, block, limit}；
//   压缩尺寸 = 下一条目偏移差；与原始大小相等时不经 LZSS。
#pragma once
#include "Lzss.hpp"
#include "ResourceCrypt.hpp"

namespace th16 {

struct ArchiveEntry {
    std::string name;
    u32 offset = 0, size = 0, reserved = 0, compressed = 0;
};

class Archive {
    const u8* source = nullptr;
    u32 source_size = 0;
    Lzss codec;

public:
    std::vector<ArchiveEntry> entries;
    std::string error;

    bool open(const u8* bytes, u32 size);
    i32 find(const char* name) const;
    bool read(u32 index, std::vector<u8>& output);
    bool read(const char* name, std::vector<u8>& output) {
        const i32 index = find(name);
        if (index < 0) { error = std::string("missing entry: ") + name; return false; }
        return read(u32(index), output);
    }

    // 0x49f278：8 组条目密钥（已与 th16.exe .data 逐字节核对）
    static constexpr CryptParams kEntryCiphers[8] = {
        {27, 115, 256, 14336}, {18, 67, 512, 15872}, {53, 121, 1024, 15360}, {3, 145, 128, 25600},
        {171, 220, 128, 28672}, {81, 158, 256, 16384}, {193, 21, 1024, 11264}, {153, 125, 128, 17408}};
    static constexpr CryptParams kHeaderCipher{0x1b, 0x37, 0x10, 0x10};
    static constexpr CryptParams kTableCipher{0x3e, 0x9b, 0x80, 0};  // limit = packed 大小，运行时填入
};

}  // namespace th16
