#include "ReplayRecorder.hpp"
#include <algorithm>
#include <cstdio>

namespace th13 {
namespace {
inline i32 read_i32(const u8* p) {
    const u32 v = u32(p[0]) | u32(p[1]) << 8 | u32(p[2]) << 16 | u32(p[3]) << 24;
    return i32(v);
}
void put16(std::vector<u8>& v, size_t p, u16 x) { v[p]=u8(x); v[p+1]=u8(x>>8); }
void put32(std::vector<u8>& v, size_t p, u32 x) { for(int i=0;i<4;i++) v[p+i]=u8(x>>(8*i)); }
void append16(std::vector<u8>& v, u16 x) { v.push_back(u8(x)); v.push_back(u8(x>>8)); }
}
std::vector<u8> ReplayRecorder::top_snapshot(const SessionState& p) {
    std::vector<u8> v(0x74,0);
    put32(v,0x5c,u32(p.character)); put32(v,0x60,0); put32(v,0x64,u32(p.difficulty));
    // 00447900 stores the two replay mode bits separately from the name.
    v[0x0a] = u8(((p.run_mode_flags >> 4) & 1u) |
                    ((p.run_mode_flags & 0x30u) == 0x20u ? 2u : 0u));
    put32(v,0x70,u32(p.selected_spell_id)); return v;
}
std::vector<u8> ReplayRecorder::stage_snapshot(const SessionState& p,
        const HudPlayerState& player,const HudScoreState& score,u16 seed,
        i32 x128,i32 y128,bool focused) {
    std::vector<u8> v(0xc4,0); put16(v,0,u16(p.stage)); put16(v,2,seed);
    put32(v,0x0c,u32(x128)); put32(v,0x10,u32(y128));
    put32(v,0x14,u32(p.character)); put32(v,0x1c,u32(score.score));
    put32(v,0x20,u32(p.difficulty)); put32(v,0x24,u32(p.continues));
    put32(v,0x2c,u32(p.graze)); put32(v,0x30,u32(p.selected_spell_id));
    put32(v,0x38,u32(p.max_point));
    // 42b990 writes the immutable difficulty base to DAT_004be7e0.
    static constexpr i32 base[]={5000,10000,15000,20000,20000,100000};
    if(p.difficulty>=0 && p.difficulty<6) put32(v,0x3c,u32(base[p.difficulty]*100));
    put32(v,0x40,u32(p.max_point_cap));
    put32(v,0x44,u32(player.power)); put32(v,0x48,u32(player.max_power));
    put32(v,0x4c,u32(player.power_step)); put32(v,0x50,u32(player.extra_lives));
    put32(v,0x54,u32(player.life_pieces)); put32(v,0x58,u32(p.life_piece_threshold_index));
    put32(v,0x5c,u32(player.bombs)); put32(v,0x60,u32(player.bomb_pieces));
    put32(v,0x64,u32(p.ending_points)); put32(v,0x6c,focused?1:0); return v;
}
bool ReplayRecorder::start(const std::vector<u8>& top) {
    if(top.size()!=0x74) return false;
    top_=top; stages_.clear(); return true;
}
bool ReplayRecorder::begin_stage(const std::vector<u8>& header) {
    if(top_.size()!=0x74 || header.size()!=0xc4) return false;
    const u16 number=u16(header[0]) | u16(header[1])<<8;
    // 449070 owns one stage block per stage index; retry replaces that block.
    for(auto it=stages_.begin();it!=stages_.end();++it) if(it->number==number) {
        stages_.erase(it); break;
    }
    if(stages_.size()>=8) return false;
    ReplayStage stage; stage.number=number; stage.header=header; stages_.push_back(std::move(stage)); return true;
}
bool ReplayRecorder::record_spell_time(u32 index, i32 packed) {
    // 00412ee0 writes the per-stage array at header+0x70 (21 dwords).
    if (stages_.empty() || index >= (0xc4u - 0x70u) / 4u) return false;
    put32(stages_.back().header, 0x70 + index * 4, u32(packed));
    return true;
}
bool ReplayRecorder::record(const GameInput& input,u8 slow_rate) {
    if(stages_.empty()) return false;
    auto& stage=stages_.back();
    if(stage.frame_count%30==0) stage.events.push_back(slow_rate);
    append16(stage.input,u16(input.held)); append16(stage.input,u16(input.pressed));
    append16(stage.input,u16(input.released)); ++stage.frame_count; return true;
}
bool ReplayRecorder::encode(std::vector<u8>& out, const std::string& name, i32 score) const {
    return encode(out, name, score, ReplaySaveMeta{});
}

bool ReplayRecorder::encode(std::vector<u8>& out, const std::string& name, i32 score,
                            const ReplaySaveMeta& meta) const {
    if (!ready()) return false;
    auto top = top_;
    // 名字（0x4484d0:39-52）：拷入 +0x00 并以 0x20 填充至 8 字节；+0x0A 模式位
    // 与 0x447900 录入的其余快照字节原样保留。+0x08 一字节 NUL 终止（%s 打印）。
    std::fill(top.begin(), top.begin() + 9, u8(' '));
    const size_t name_len = std::min<size_t>(name.size(), 8);
    std::copy_n(name.begin(), name_len, top.begin());
    top[8] = 0;
    // 顶层 +0x0C u64 time_t（0x4549d0:84 保存确认时 __time64 写入）。
    const std::time_t date = meta.date != 0 ? meta.date : std::time(nullptr);
    const u64 stamp = u64(date);
    for (int i = 0; i < 8; ++i) top[0x0C + i] = u8(stamp >> (8 * i));
    // +0x14 总分（0x4484d0:0x4486ea 写 DAT_004be7c0；此处由调用方给出）。
    put32(top, 0x14, u32(score));
    // +0x54 慢速率 f32（0x4486e5..0x448715）：
    //   slow = (float)(100.0 - (double)(float)(num/den) * 100.0)
    // 原版分子/分母是 fps 管理器（0x4c218c+0x24/+0x2c）保存时刻的实时计数；
    // 注入缝给 denominator>0 时逐字节复刻。默认走事件字节流聚合（结构同
    // 0x424990:37-50 的逐窗分支：fps≤57 计实测值，否则计满 60）。
    double num = meta.slow_numerator;
    double den = meta.slow_denominator;
    if (den <= 0.0) {
        for (const ReplayStage& stage : stages_) {
            for (const u8 sample : stage.events) {
                den += 60.0;
                num += sample <= 57 ? double(sample) : 60.0;
            }
        }
    }
    const float slow = den > 0.0
        ? static_cast<float>(100.0 - static_cast<double>(static_cast<float>(num / den)) * 100.0)
        : 0.0f;
    put32(top, 0x54, float_to_bits(slow));
    // +0x58 关卡块数（0x4484d0:0x4486d9 写存在的槽计数）。
    top[0x58] = u8(stages_.size());
    // +0x68（0x4549d0:86 / 0x43fa90:197 保存确认流程写入）。
    put32(top, 0x68, u32(meta.stage_field));

    // 平流 = 顶层 0x74 + stage 块链（0x4484d0:0x448718..0x448838，槽序 = 关号升序）。
    std::vector<u8> plain = top;
    std::vector<const ReplayStage*> ordered;
    for (const auto& stage : stages_) ordered.push_back(&stage);
    std::sort(ordered.begin(), ordered.end(),
              [](const auto* a, const auto* b) { return a->number < b->number; });
    for (size_t i = 0; i < ordered.size(); ++i) {
        const ReplayStage& stage = *ordered[i];
        auto input = stage.input;
        // EOF（0x44854c..0x448598）：仅第三参非 0 的中断保存路径追加 6×0xFF
        // 输入字节，无事件字节（0x4480a0 的事件只写在录制路径；补 0 会破坏
        // (帧数+29)/30 不变式且文件无法解码）。
        if (meta.append_eof && i + 1 == ordered.size()) {
            for (int j = 0; j < 6; ++j) input.push_back(0xff);
        }
        auto header = stage.header;
        const u32 count = u32(input.size() / 6);
        put32(header, 4, count);
        put32(header, 8, u32(input.size() + stage.events.size()));
        plain.insert(plain.end(), header.begin(), header.end());
        plain.insert(plain.end(), input.begin(), input.end());
        plain.insert(plain.end(), stage.events.begin(), stage.events.end());
    }

    // 压缩（0x44883d 调 0x458c30）→ 双趟加密（0x448863 AL=0x7D/0x100、
    // 0x448879 AL=0x5C/0x400）。
    std::vector<u8> packed = ReplayFile::deflate(plain.data(), plain.size());
    ReplayFile::xor_encrypt(packed.data(), packed.size(), 0x7D, 0x3A, 0x100);
    ReplayFile::xor_encrypt(packed.data(), packed.size(), 0x5C, 0xE1, 0x400);

    out.assign(0x24, 0);
    out[0] = 't'; out[1] = '1'; out[2] = '3'; out[3] = 'r';
    put16(out, 4, 2);
    put32(out, 0x0C, u32(0x24 + packed.size()));
    put32(out, 0x10, 0x100);
    put32(out, 0x1C, u32(packed.size()));
    put32(out, 0x20, u32(plain.size()));
    out.insert(out.end(), packed.begin(), packed.end());

    // USER 段（0x4484d0:170-247）：两块，均为 "USER"+u32 块长+u8 flag+3 填充
    // +文本；文本后 sprintf NUL + 0x448ace/0x448b61 的 +1 预留 NUL，块长 =
    // (13+文本长) 向上 4 对齐（0x448ad9/0x448b6c 的 and 0x80000003 序列）。
    const auto append_user_block = [&out](u8 flag, const std::string& text) {
        const size_t base = out.size();
        const size_t block_len = (13 + text.size() + 3) & ~size_t(3);
        out.resize(base + block_len, 0);
        out[base] = 'U'; out[base + 1] = 'S'; out[base + 2] = 'E'; out[base + 3] = 'R';
        put32(out, base + 4, u32(block_len));
        out[base + 8] = flag;
        std::copy(text.begin(), text.end(), out.begin() + ptrdiff_t(base + 12));
    };

    // ---- 块 1（flag=0 摘要，0x4484d0:175-211 的 sprintf 序）----
    // SJIS 字节取自 .rdata 原样（0x4abdc0/%s 段 0x4abdcc、0x4abdf0、0x4abe00、
    // 0x4abe0c、0x4abe2c、0x4abe38、0x4abe6c/0x4abe7c/0x4abe88/0x4abe44/0x4abe58、
    // 0x4abe9c、0x4abea8），避免源码编码层转写。
    std::string text(
        "\x93\x8C\x95\xFB\x90\x5F\x97\xEC\x95\x5F"      // 東方神霊廟（0x4abdc0：神=90 5F、廟=95 5F）
        " "
        "\x83\x8A\x83\x76\x83\x8C\x83\x43\x83\x74\x83\x40\x83\x43\x83\x8B\x8F\xEE\x95\xF1"
        "\r\n");                                        // "%s リプレイファイル情報\r\n"
    text += "Version ";
    text += meta.version;
    text += "\r\n";
    // "Name %s"：参数 = 顶层名字缓冲（0x20 填充至 8 的同一 C 串）。
    text += "Name ";
    text.append(name, 0, name_len);
    text.append(size_t(8) - name_len, ' ');
    text += "\r\n";
    // "Date %.2d/%.2d/%.2d %.2d:%.2d"：localtime(顶层 +0x0C)。
    std::tm local{};
#ifdef _WIN32
    localtime_s(&local, &date);
#else
    localtime_r(&date, &local);
#endif
    char line[64];
    std::snprintf(line, sizeof line, "%02d/%02d/%02d %02d:%02d\r\n",
                  local.tm_year % 100, local.tm_mon + 1, local.tm_mday,
                  local.tm_hour, local.tm_min);
    text += "Date ";
    text += line;
    // "Chara %s"：表 0x4bb904，索引 = 顶层 +0x5C + +0x60（0x4489ee..0x4489fa）。
    static constexpr const char* kCharaTbl[8] = {
        "Reimu  ", "Marisa ", "Sanae  ", "Youmu  ",   // 角色表 4 项（0x4ab91c/914/90c/904）
        "Easy   ", "Normal ", "Hard   ", "Lunatic",  // 表后紧邻的难度串（原版无界索引按此读）
    };
    const i32 chara = read_i32(top.data() + 0x5C);
    const i32 shot = read_i32(top.data() + 0x60);
    text += "Chara ";
    text += (chara >= 0 && chara + shot >= 0 && chara + shot < 8) ? kCharaTbl[size_t(chara + shot)] : "";
    text += "\r\n";
    // "Rank %s"：表 0x4bb914，索引 = 顶层 +0x64（0x448a10..0x448a15）。
    static constexpr const char* kRankTbl[8] = {
        "Easy   ", "Normal ", "Hard   ", "Lunatic", "Extra  ", "O.D.   ", "E ", "N ",
    };
    const i32 rank = read_i32(top.data() + 0x64);
    text += "Rank ";
    text += (rank >= 0 && rank < 8) ? kRankTbl[size_t(rank)] : "";
    text += "\r\n";
    // Stage 行（0x448a2b..0x448a9e）：+0x68>7 → 全通分支（first==7 为 Extra
    // Clear）；否则 first/last = 存在槽的最小/最大关号，同关单行、区间行
    // "Stage %d \x81` %d"（0x4abe88，〜 = SJIS 0x81 0x60）。
    i32 first = 0;
    i32 last = 0;
    if (!ordered.empty()) {
        first = ordered.front()->number;
        last = ordered.back()->number;
    }
    if (meta.stage_field > 7) {
        text += first == 7 ? "Extra Stage Clear\r\n" : "Stage All Clear\r\n";
    } else if (first == last) {
        if (first == 7) {
            text += "Extra Stage\r\n";
        } else {
            text += "Stage ";
            text += std::to_string(first);
            text += "\r\n";
        }
    } else {
        text += "Stage ";
        text += std::to_string(first);
        text += ' ';
        text += char(0x81);
        text += char(0x60);  // 〜
        text += ' ';
        text += std::to_string(last);
        text += "\r\n";
    }
    text += "Score ";
    text += std::to_string(score);
    text += "\r\n";
    // "Slow Rate %2.2f"：f32 提升为 double 后 printf（0x448ab8..0x448ac3）。
    std::snprintf(line, sizeof line, "%2.2f\r\n", static_cast<double>(slow));
    text += "Slow Rate ";
    text += line;
    append_user_block(0, text);

    // ---- 块 2（flag=1 コメント占位，0x448b49..0x448b5c；无 \r\n）----
    append_user_block(1, std::string(
        "\x83\x52\x83\x81\x83\x93\x83\x67\x82\xF0\x8F\x91\x82\xAF\x82\xDC\x82\xB7"));
    return true;
}
}
