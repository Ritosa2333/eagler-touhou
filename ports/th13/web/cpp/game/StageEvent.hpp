// stage1 事件落盘（JSONL）。字段与精度是 oracle 对拍合同，见 docs/plans/event-contract.md；
// 改动键名/键序/精度前必须同步该文档与 oracle 侧。
#pragma once

#include "Types.hpp"

#include <cstdio>
#include <string>

namespace th13 {

class StageEventSink {
public:
    ~StageEventSink() { close(); }
    bool open(const char* path);
    void close();
    bool ok() const { return !failed_; }  // close() 后仍可查询（落盘失败亦置位）
    u64 emitted() const { return emitted_; }
    void set_frame(u32 frame) { frame_ = frame; seq_ = 0; }

    void enemy_spawn(u32 enemy, const std::string& routine, float x, float y, float z, float life, u32 parent);
    void enemy_move(u32 enemy, u16 op, const u32* integral, const float* values, u32 count);
    void bullet_spawn(u32 enemy, u32 slot, u32 way, u32 idx, float x, float y, float z, float angle,
                      float speed, u32 sprite, u32 subtype);
    // 回放会话扩展（event-contract.md §7.3）。
    void player(float x, float y, u32 state);
    void kill(u32 enemy, float x, float y);
    void boss_op(u32 enemy, u16 op, const i32* args, u32 count, const char* name);

private:
    void raw(const char* text) { if (std::fputs(text, out_) < 0) failed_ = true; }
    void key(const char* name);          // ,"name":
    void num(float value);               // %.9g（非有限值置失败标记）
    void unum(u32 value);                // %u
    std::FILE* out_ = nullptr;
    u32 frame_ = 0, seq_ = 0;
    u64 emitted_ = 0;
    bool failed_ = false;
};

}  // namespace th13
