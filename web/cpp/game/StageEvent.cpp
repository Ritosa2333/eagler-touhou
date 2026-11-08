// stage1 事件落盘实现。JSON 键序与 event-contract.md §4 一致；
// 浮点 %.9g（float32 往返无损），整数十进制。
#include "StageEvent.hpp"

#include "StageMath.hpp"

#include <cmath>
#include <cstring>

namespace th13 {

bool StageEventSink::open(const char* path) {
    close();
    failed_ = false;
    out_ = std::fopen(path, "wb");
    if (!out_) failed_ = true;
    return out_ != nullptr;
}

void StageEventSink::close() {
    if (out_) {
        if (std::fflush(out_) != 0 || std::fclose(out_) != 0) failed_ = true;
        out_ = nullptr;
    }
}

void StageEventSink::key(const char* name) {
    raw(",");
    raw(name);
    raw(":");
}

void StageEventSink::num(float value) {
    if (!std::isfinite(value)) {
        failed_ = true;  // 契约域外值：以失败暴露，不产出非法 JSON
        raw("null");
        return;
    }
    char buffer[40];
    std::snprintf(buffer, sizeof buffer, "%.9g", static_cast<double>(value));
    raw(buffer);
}

void StageEventSink::unum(u32 value) {
    char buffer[16];
    std::snprintf(buffer, sizeof buffer, "%u", value);
    raw(buffer);
}

void StageEventSink::enemy_spawn(u32 enemy, const std::string& routine, float x, float y, float z,
                                 float life, u32 parent) {
    if (!out_) return;
    ++emitted_;
    char head[96];
    std::snprintf(head, sizeof head, "{\"frame\":%u,\"seq\":%u,\"type\":\"enemy_spawn\",\"enemy\":%u",
                  frame_, seq_++, enemy);
    raw(head);
    raw(",\"routine\":\"");
    // 子程序名字符集为 [A-Za-z0-9_]（SCPT 名表约束），仍做最小转义防御。
    for (char c : routine) {
        if (c == '"' || c == '\\' || c < 0x20) {
            char escape[8];
            std::snprintf(escape, sizeof escape, "\\u%04x", c);
            raw(escape);
        } else {
            char s[2] = {c, 0};
            raw(s);
        }
    }
    raw("\",\"x\":");
    num(x);
    key("\"y\"");
    num(y);
    key("\"z\"");
    num(z);
    key("\"life\"");
    num(life);
    key("\"parent\"");
    unum(parent);
    raw("}\n");
}

void StageEventSink::enemy_move(u32 enemy, u16 op, const u32* integral, const float* values, u32 count) {
    if (!out_) return;
    ++emitted_;
    char head[96];
    std::snprintf(head, sizeof head, "{\"frame\":%u,\"seq\":%u,\"type\":\"enemy_move\",\"enemy\":%u,\"op\":%u",
                  frame_, seq_++, enemy, op);
    raw(head);
    raw(",\"args\":[");
    for (u32 i = 0; i < count; ++i) {
        if (i) raw(",");
        if (integral && integral[i]) {
            char buffer[16];
            std::snprintf(buffer, sizeof buffer, "%d", static_cast<i32>(values[i]));
            raw(buffer);
        } else {
            num(values[i]);
        }
    }
    raw("]}\n");
}

void StageEventSink::bullet_spawn(u32 enemy, u32 slot, u32 way, u32 idx, float x, float y, float z,
                                  float angle, float speed, u32 sprite, u32 subtype) {
    if (!out_) return;
    ++emitted_;
    char head[128];
    std::snprintf(head, sizeof head,
                  "{\"frame\":%u,\"seq\":%u,\"type\":\"bullet_spawn\",\"enemy\":%u,\"slot\":%u,"
                  "\"way\":%u,\"idx\":%u",
                  frame_, seq_++, enemy, slot, way, idx);
    raw(head);
    raw(",\"x\":");
    num(x);
    key("\"y\"");
    num(y);
    key("\"z\"");
    num(z);
    key("\"angle\"");
    num(angle);
    key("\"speed\"");
    num(speed);
    key("\"sprite\"");
    unum(sprite);
    key("\"subtype\"");
    unum(subtype);
    raw("}\n");
}

void StageEventSink::player(float x, float y, u32 state) {
    if (!out_) return;
    ++emitted_;
    char head[80];
    std::snprintf(head, sizeof head,
                  "{\"frame\":%u,\"seq\":%u,\"type\":\"player\",\"x\":", frame_, seq_++);
    raw(head);
    num(x);
    key("\"y\"");
    num(y);
    key("\"state\"");
    unum(state);
    raw("}\n");
}

void StageEventSink::kill(u32 enemy, float x, float y) {
    if (!out_) return;
    ++emitted_;
    char head[96];
    std::snprintf(head, sizeof head,
                  "{\"frame\":%u,\"seq\":%u,\"type\":\"kill\",\"enemy\":%u,\"x\":", frame_, seq_++,
                  enemy);
    raw(head);
    num(x);
    key("\"y\"");
    num(y);
    raw("}\n");
}

void StageEventSink::boss_op(u32 enemy, u16 op, const i32* args, u32 count, const char* name) {
    if (!out_) return;
    ++emitted_;
    char head[96];
    std::snprintf(head, sizeof head,
                  "{\"frame\":%u,\"seq\":%u,\"type\":\"boss_op\",\"enemy\":%u,\"op\":%u,\"args\":[",
                  frame_, seq_++, enemy, op);
    raw(head);
    for (u32 i = 0; i < count; ++i) {
        if (i) raw(",");
        char buffer[16];
        std::snprintf(buffer, sizeof buffer, "%d", args[i]);
        raw(buffer);
    }
    raw("],\"name\":\"");
    for (const char* p = name; p && *p; ++p) {
        if (*p == '"' || *p == '\\' || static_cast<unsigned char>(*p) < 0x20) {
            char escape[8];
            std::snprintf(escape, sizeof escape, "\\u%04x",
                          static_cast<unsigned char>(*p));
            raw(escape);
        } else {
            const char single[2] = {*p, 0};
            raw(single);
        }
    }
    raw("\"}\n");
}

}  // namespace th13
