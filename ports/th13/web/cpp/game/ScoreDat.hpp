// 原版 scoreth13.dat 格式孪生（镜像原样字节 + 字段级读写视图）。
// 全部布局/算法证据见外层 oracle/scorefmt.py docstring（0x449cd0 装载 /
// 0x449e50 保存 / 0x449b00 新装 / 0x449760 CR 默认 / 0x449a30 ST 默认+LCG /
// 0x43d6b0 高分入场；[D]=反汇编直读、[U]=Unicorn 实证）。黄金样本
// artifacts/score_golden/（Unicorn 原版代码产出，两次生成逐字节一致）。
//
// 文件 = 0x18 明文头（'TH31' + 总长 + u16 版本 4 + u16 0 + 0x100 + compSize +
// uncompSize）+ xor_encrypt(LZ 压缩(镜像体), key0=0xAC, step=0x35, block=0x10)。
// 镜像体 = 5×CR 块（Easy..Extra，各 0x56DC，按槽号顺序）+ ST 块（0x42C）。
// 加密/解密/解压消费 game/ReplayFile 的同名原语（0x4723a0/0x472280/0x458f30）；
// 压缩（0x458c30）等并行交付的 ReplayFile::deflate，落地前 build_file 不可用。
//
// 保存剔除怪癖（route_c 黄金实证）：保存逐槽校验 u16=='CR'，坏块整块不进文件
// 且 uncompSize 随之缩短；本模型以 cr_present=false 表达缺席块，build_image
// 只拼 present 块（游标即原版 0x449f2f 的 esi）。
//
// 与 oracle/scorefmt.py parse 的同一刻意偏差：结构异常（头/魔数/版本/块长/
// 校验和/槽号/未知魔数）一律失败——装载侧调用方（FileStore）按"坏档回退"
// 语义消费，不静默跳过。
#pragma once

#include "../game/Types.hpp"

#include <array>
#include <string>
#include <vector>

namespace th13 {

class ScoreDat {
public:
    static constexpr int kCRCount = 5;        // Easy..Extra 槽号 0..4
    static constexpr int kCharaCount = 6;     // 原版 chara 0..5（ReimuA..SanaeB）
    static constexpr int kRankCount = 10;
    static constexpr int kPracticeCount = 127;
    static constexpr size_t kCRSize = 0x56DC;
    static constexpr size_t kSTSize = 0x42C;
    static constexpr size_t kImageSize = size_t(kCRCount) * kCRSize + kSTSize;  // 0x1B678
    static constexpr size_t kEntrySize = 0x1C;
    static constexpr size_t kCharaStride = 0x118;   // 10 × 0x1C
    static constexpr size_t kEntriesOff = 0x10;
    static constexpr size_t kMidOff = 0x6A0;        // ..0x7B8 未见证，原样透传
    static constexpr size_t kProgressOff = 0x7B8;   // ..0xA08 进度区
    static constexpr size_t kPracticeOff = 0x978;   // 记录基址（+0xA08 是记录号字段）
    static constexpr size_t kPracticeSize = 0x9C;
    static constexpr size_t kStNameOff = 0x0C;
    static constexpr size_t kStTableOff = 0x50;
    static constexpr int kStTableLen = 0x1EE;       // 494
    static constexpr u32 kHeaderFieldC = 0x100;
    static constexpr u16 kFileVersion = 4;
    static constexpr size_t kHeaderSize = 0x18;
    // ST+0x16..0x22：ending seen 13 字节（0x416130 读写 score+0x1b26a..0x1b276；
    // ST 块在结构 +0x1B254）。init 路径 0x449a30 对 +0x15..0x50 memset 0。
    static constexpr size_t kStEndingOff = 0x16;
    static constexpr size_t kEndingBytes = 13;

    // 高分条目 0x1C 字节（0x43d6b0 写入点；默认 0x449760）。
    struct Entry {
        u32 score = 0;              // +0x00
        u8 stage = 0;               // +0x04 到达关卡
        u8 clear = 0;               // +0x05 通关旗（DAT_004be7c8）
        std::array<u8, 9> name{};   // +0x06 8 字符 + NUL（+0x0E）
        u32 time_lo = 0;            // +0x10 __time64 低半
        u32 time_hi = 0;            // +0x14 高半
        u32 rate_bits = 0;          // +0x18 f32 达成率%（位型）
        // +0x0F 垫 1 字节：memset 0、无写入者，序列化恒写 0。
    };

    // 镜像槽原样字节；cr_present=false = 文件中该块被剔除（route_c 怪癖）。
    std::array<std::array<u8, kCRSize>, kCRCount> cr{};
    std::array<bool, kCRCount> cr_present{};
    std::array<u8, kSTSize> st{};
    u16 seed = 0;  // 生成 defaults 时的 LCG 种子（信息性）
    std::string error;

    // 0x449a30 LCG：next = rol16(((state^0x9630)-0x6553)&0xFFFF, 2)；返回并推进。
    static u16 lcg_next(u16& state);

    // 无档新装（0x449b00 memset + 0x449a30 + 5×0x449760 等价）：全默认块 +
    // ST LCG 表全量推导。
    void set_defaults(u16 lcg_seed);
    // 单槽默认重建（0x449760 等价；槽越界则忽略）。用于缺席块的 boot 重建
    // 语义：原版内存恒有全 5 块，保存只看 'CR' 魔数——端口在保存前重建缺席
    // 块即与原版一致。ST 表不动（保留 parse/开机生成值）。
    void set_default_cr_block(u32 slot);

    // 装载（0x449cd0 孪生）：头校验 → xor_decrypt(0xAC,0x35,0x10) →
    // inflate（长度须恰为 uncompSize）→ 逐块魔数/版本/长度/校验和/槽号。
    bool parse_file(const u8* data, size_t size);
    // 镜像体直接走块（测试注入篡改镜像用）。
    bool parse_image(const u8* image, size_t size);

    // 保存镜像层（0x449e50 的 memcpy 序列孪生）：逐 present 块重写 +0xC=槽号、
    // +4=校验和后拼接，末尾 ST 重写 +4。返回写入长度（≤kImageSize）。
    size_t build_image(u8* out, size_t capacity);

    // 完整文件（0x449e50 尾段孪生）：build_image → ReplayFile::deflate
    // （bit-exact 0x458c30）→ xor_encrypt(0xAC,0x35,0x10) → 0x18 头。
    bool build_file(std::vector<u8>& out);

    // ---- 字段视图（直接读写镜像块字节；未见证字节永不触碰）----

    static Entry read_entry(const u8* block, int chara, int rank);
    static void write_entry(u8* block, int chara, int rank, const Entry& entry);

    // 进度区 u32：offset ∈ {0x7B8 游戏回数, 0x7C0 通关数, 0x7DC 第二通关计数}
    // （0x42b990 / 0x42cd40 写入点；上限 99999 由调用方钳制）。
    static u32 progress_u32(const u8* block, size_t offset, int chara);
    static void set_progress_u32(u8* block, size_t offset, int chara, u32 value);

    // 进度行 +0x7F0 + (chara*8+stage)*8（stage 0..5）：+0 u32 关卡最高分
    // （0x43f8f0 max 写）、+4 u8 通过旗（0x42cd40）、+5 u8 游玩旗（0x42b990）。
    static u32 progress_row_score(const u8* block, int chara, int stage);
    static void set_progress_row_score(u8* block, int chara, int stage, u32 value);
    static u8 progress_row_flag(const u8* block, int chara, int stage, int which);
    static void set_progress_row_flag(u8* block, int chara, int stage, int which, u8 value);

    // 练习记录 i（+0x978+i*0x9C）：+0x90 记录号、+0x94 关卡 id、+0x98 最好成绩
    // （0x42cd40 max 写）。主体 0x90 字节无实证写入偏移，保持透传不写。
    static u32 practice_u32(const u8* block, int i, size_t field_offset);
    static void set_practice_u32(u8* block, int i, size_t field_offset, u32 value);

    // ST 上次输入名（+0x0C 9 字节；静态默认 8 空格+NUL，0x449a55 拷贝）。
    static std::array<u8, 9> st_name(const u8* st);
    static void set_st_name(u8* st, const std::array<u8, 9>& name);

    // ending seen 13 字节（ST+0x16..0x22；0x416130 写入点）。
    static void read_ending_bytes(const u8* st, u8 (&out)[kEndingBytes]);
    static void write_ending_bytes(u8* st, const u8 (&in)[kEndingBytes]);

    // sum(block[8..size))（0x449700 thiscall 与保存内联四路累加器同构）。
    static u32 block_checksum(const u8* block, size_t size);

    // 全 present 块 60 条目 __time64 的最大 u64（缺席块不计）。FileStore dat 导入
    // 裁决用（"dat 明显更新" = 此值 > 应用档榜表最大时间戳）。
    u64 max_entry_timestamp() const;

private:
    bool fail(const std::string& message) {
        error = message;
        return false;
    }
};

}  // namespace th13
