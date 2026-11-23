// th16 harness 整段摘要：把一帧的绘制操作流直接折成几条 sha1，口径与 tools/oracle/draw-compare.py 的 frame_digests、
// tools/draw-structure.mjs 的 frameDigests 逐字一致（tests/draw-long.test.mjs 用同一帧的原始 JSON 三方互校）。
//
// 原始绘制流每帧可达数百 KB，整段跑（十几万帧）不适合落盘或经管道传给脚本再解析；harness `game-draw` 的输出写成
// "digest:<dst>" 时改为每帧一行 {"frame","ops","s","v","a","b"}：
//   s  结构：每次 Clear / 绘制一行（目标 / 视口 / 图元 / 格式 / 数量 / 纹理 / 状态子集），不含顶点与矩阵
//   v  位级：结构行后接顶点字与世界矩阵的原始 32 位字
//   a / b  量化：浮点字换成格号。格宽 = 1e-4 × 2^max(0, 指数位 - 127)，格号 floor((|v| + half·格宽) / 格宽)；
//            网格 a 的 half = 0，网格 b 的 half = 0.5。个别字 1 ulp 的差在至少一张网格里同格。
// 三处实现都只用位运算与二进制精确的乘除（不调 libm），记号逐字一致。
#pragma once
#include "RecordingDevice.hpp"

#include <cmath>
#include <cstring>
#include <string>
#include <vector>

namespace th16 {

using touhou::graphics::VertexLayout;

class Sha1 {
public:
    void update(const void* data, size_t size) {
        const u8* p = static_cast<const u8*>(data);
        total_ += size;
        while (size != 0) {
            const size_t take = size < sizeof(block_) - used_ ? size : sizeof(block_) - used_;
            std::memcpy(block_ + used_, p, take);
            used_ += take;
            p += take;
            size -= take;
            if (used_ == sizeof(block_)) {
                transform();
                used_ = 0;
            }
        }
    }

    std::string hex() {
        Sha1 copy = *this;
        const u8 pad = 0x80;
        const u8 zero = 0;
        copy.update(&pad, 1);
        while (copy.used_ != 56) copy.update(&zero, 1);
        u8 length[8];
        const u64 bits = total_ * 8;
        for (int i = 0; i < 8; ++i) length[i] = u8(bits >> (56 - 8 * i));
        copy.update(length, 8);
        static const char* const kDigits = "0123456789abcdef";
        std::string out(40, '0');
        for (int i = 0; i < 5; ++i) {
            for (int j = 0; j < 4; ++j) {
                const u8 byte = u8(copy.state_[i] >> (24 - 8 * j));
                out[size_t(8 * i + 2 * j)] = kDigits[byte >> 4];
                out[size_t(8 * i + 2 * j + 1)] = kDigits[byte & 15];
            }
        }
        return out;
    }

private:
    void transform() {
        u32 w[80];
        for (int i = 0; i < 16; ++i) {
            w[i] = u32(block_[4 * i]) << 24 | u32(block_[4 * i + 1]) << 16 | u32(block_[4 * i + 2]) << 8 | u32(block_[4 * i + 3]);
        }
        for (int i = 16; i < 80; ++i) {
            const u32 t = w[i - 3] ^ w[i - 8] ^ w[i - 14] ^ w[i - 16];
            w[i] = t << 1 | t >> 31;
        }
        u32 a = state_[0], b = state_[1], c = state_[2], d = state_[3], e = state_[4];
        for (int i = 0; i < 80; ++i) {
            u32 f, k;
            if (i < 20) { f = (b & c) | (~b & d); k = 0x5a827999u; }
            else if (i < 40) { f = b ^ c ^ d; k = 0x6ed9eba1u; }
            else if (i < 60) { f = (b & c) | (b & d) | (c & d); k = 0x8f1bbcdcu; }
            else { f = b ^ c ^ d; k = 0xca62c1d6u; }
            const u32 temp = a << 5 | a >> 27;
            e = e + (temp + f + k + w[i]);
            b = b << 30 | b >> 2;
            const u32 rotate = e;
            e = d; d = c; c = b; b = a; a = rotate;
        }
        state_[0] += a; state_[1] += b; state_[2] += c; state_[3] += d; state_[4] += e;
    }

    u32 state_[5] = {0x67452301u, 0xefcdab89u, 0x98badcfeu, 0x10325476u, 0xc3d2e1f0u};
    u8 block_[64] = {};
    size_t used_ = 0;
    u64 total_ = 0;
};

namespace draw_digest_detail {

constexpr double kQuantEps = 1e-4;

// 各顶点格式一个顶点的字数与其中哪些是浮点（draw-compare.py LAYOUT_WORDS）
struct LayoutMask { int count; bool is_float[7]; };
constexpr LayoutMask layout_mask(VertexLayout layout) {
    switch (layout) {
    case VertexLayout::WorldColorUv: return {6, {true, true, true, false, true, true, false}};
    case VertexLayout::ScreenColorUv: return {7, {true, true, true, true, false, true, true}};
    case VertexLayout::ScreenColor: return {5, {true, true, true, true, false, false, false}};
    case VertexLayout::ScreenUv: return {6, {true, true, true, true, true, true, false}};
    default: return {5, {true, true, true, true, true, false, false}};   // WorldUv
    }
}

inline void append_number(std::string& out, unsigned long long value) {
    char digits[20];
    char* end = digits + sizeof(digits);
    char* p = end;
    do { *--p = char('0' + value % 10); value /= 10; } while (value != 0);
    out.append(p, size_t(end - p));
}

inline void append_signed(std::string& out, long long value) {
    const bool negative = value < 0;
    const unsigned long long magnitude = negative ? 0ull - static_cast<unsigned long long>(value) : static_cast<unsigned long long>(value);
    if (negative) out += '-';
    append_number(out, magnitude);
}

// draw-compare.py quant_token
inline void append_quant_token(std::string& out, u32 bits, double half) {
    float stored;
    std::memcpy(&stored, &bits, 4);
    const double value = stored;
    const unsigned exponent = (bits >> 23) & 0xffu;
    if (exponent == 0xffu) {
        if ((bits & 0x7fffffu) != 0) {
            static const char* const kHex = "0123456789abcdef";
            out += "n";
            for (int i = 7; i >= 0; --i) out += kHex[(bits >> (4 * i)) & 15u];
        } else {
            out += value < 0.0 ? "-inf" : "inf";
        }
        return;
    }
    if (value < 0.0) out += '-';
    append_number(out, exponent);
    out += ':';
    const double magnitude = std::fabs(value);
    const double cell = exponent < 128u ? kQuantEps : std::ldexp(kQuantEps, int(exponent) - 127);
    append_number(out, static_cast<unsigned long long>(std::floor((magnitude + cell * half) / cell)));
}

// 顶点字或矩阵：is_quant 时浮点字换格号，否则十进制位型
inline void append_words(std::string& out, const u32* words, size_t count, const LayoutMask& mask, bool quant, double half) {
    for (size_t i = 0; i < count; ++i) {
        if (i != 0) out += ',';
        if (quant && mask.is_float[i % size_t(mask.count)]) append_quant_token(out, words[i], half);
        else append_number(out, words[i]);
    }
}

// structure_lines：每次 Clear / 绘制一行（draw-compare.py structure_lines）
inline std::string structure_line(const RecordingDevice::Op& op) {
    using namespace touhou::graphics;
    static const char* const kTopology[] = {"points", "lines", "linestrip", "triangles", "strip", "fan"};
    static const char* const kLayout[] = {"WorldUv", "WorldColorUv", "ScreenColorUv", "ScreenColor", "ScreenUv"};
    std::string out;
    out += op.kind == RecordingDevice::OpKind::Clear ? 'C' : 'D';
    out += '|';
    append_signed(out, op.target.slot);
    out += ',';
    append_signed(out, op.target.entry);
    out += '|';
    if (op.kind == RecordingDevice::OpKind::Clear) {
        append_number(out, op.clear_flags);
        out += '|';
        append_number(out, op.clear_color);
        out += '|';
        if (op.has_rect) {
            append_signed(out, op.rect.left); out += ',';
            append_signed(out, op.rect.top); out += ',';
            append_signed(out, op.rect.right); out += ',';
            append_signed(out, op.rect.bottom);
        } else {
            out += '-';
        }
        return out;
    }
    const PipelineState& s = op.state;
    const bool world = op.layout == VertexLayout::WorldUv || op.layout == VertexLayout::WorldColorUv;
    out += kTopology[u32(op.topology)];
    out += '|';
    out += kLayout[u32(op.layout)];
    out += '|';
    append_number(out, op.primitives);
    out += '|';
    append_signed(out, op.texture.slot);
    out += ',';
    append_signed(out, op.texture.entry);
    out += '|';
    append_signed(out, op.viewport.x); out += ',';
    append_signed(out, op.viewport.y); out += ',';
    append_signed(out, op.viewport.width); out += ',';
    append_signed(out, op.viewport.height);
    out += '|';
    const u32 state[] = {
        s.depthTest ? 1u : 0u, s.depthWrite ? 1u : 0u, u32(s.depthCompare), s.blend ? 1u : 0u, u32(s.sourceBlend), u32(s.destinationBlend),
        u32(s.blendEquation), s.alphaTest ? 1u : 0u, s.alphaReference, u32(s.alphaCompare), s.fog ? 1u : 0u, s.fogColor, s.textureFactor,
        u32(s.color.operation), u32(s.color.first.source), u32(s.color.second.source), u32(s.alpha.operation),
        u32(s.alpha.first.source), u32(s.alpha.second.source), s.textureTransform ? 1u : 0u,
        u32(s.minFilter), u32(s.magFilter), u32(s.addressU), u32(s.addressV), u32(s.cull)};
    // textureTransform 只对世界格式入行：屏幕格式不经 TnL，原版从不关该位而移植版显式关掉
    constexpr size_t kTextureTransformIndex = 19;
    for (size_t i = 0; i < sizeof(state) / sizeof(state[0]); ++i) {
        if (i == kTextureTransformIndex && !world) continue;
        if (i != 0) out += ',';
        append_number(out, state[i]);
    }
    return out;
}

}  // namespace draw_digest_detail

struct FrameDigest {
    int ops = 0;
    std::string s, v, a, b;
};

inline FrameDigest digest_frame(const std::vector<RecordingDevice::Op>& ops) {
    using draw_digest_detail::append_words;
    using draw_digest_detail::layout_mask;
    using draw_digest_detail::structure_line;
    using Op = RecordingDevice::Op;
    Sha1 structure, exact, grid_a, grid_b;
    FrameDigest out;
    std::string tail;
    for (const Op& op : ops) {
        if (op.kind != RecordingDevice::OpKind::Clear && op.kind != RecordingDevice::OpKind::Draw) continue;
        if (out.ops != 0) {
            for (Sha1* digest : {&structure, &exact, &grid_a, &grid_b}) digest->update("\n", 1);
        }
        ++out.ops;
        const std::string line = structure_line(op);
        for (Sha1* digest : {&structure, &exact, &grid_a, &grid_b}) digest->update(line.data(), line.size());
        if (op.kind == RecordingDevice::OpKind::Clear) {
            for (Sha1* digest : {&exact, &grid_a, &grid_b}) digest->update("|", 1);
            continue;
        }
        // 三份尾部：位级 / 网格 a / 网格 b
        const bool world_layout = op.layout == VertexLayout::WorldUv || op.layout == VertexLayout::WorldColorUv;
        const float* const matrices[] = {op.world, op.view, op.projection, op.texture_matrix};
        const int matrix_count = world_layout ? 3 + (op.state.textureTransform ? 1 : 0) : 0;
        static const char* const kPrefix = "wvpt";
        for (int column = 0; column < 3; ++column) {
            tail.clear();
            tail += '|';
            append_words(tail, op.words.data(), op.words.size(), layout_mask(op.layout), column != 0, column == 2 ? 0.5 : 0.0);
            for (int m = 0; m < matrix_count; ++m) {
                u32 bits[16];
                for (int i = 0; i < 16; ++i) std::memcpy(&bits[i], &matrices[m][i], 4);
                tail += '|';
                tail += kPrefix[m];
                append_words(tail, bits, 16, {1, {true, false, false, false, false, false, false}}, column != 0, column == 2 ? 0.5 : 0.0);
            }
            (column == 0 ? exact : column == 1 ? grid_a : grid_b).update(tail.data(), tail.size());
        }
    }
    out.s = structure.hex();
    out.v = exact.hex();
    out.a = grid_a.hex();
    out.b = grid_b.hex();
    return out;
}

// 每帧一行：{"frame":n,"ops":k,"s":…,"v":…,"a":…,"b":…}
inline void write_digest_line(std::FILE* out, int frame, const std::vector<RecordingDevice::Op>& ops) {
    const FrameDigest digest = digest_frame(ops);
    std::fprintf(out, "{\"frame\":%d,\"ops\":%d,\"s\":\"%s\",\"v\":\"%s\",\"a\":\"%s\",\"b\":\"%s\"}\n",
                 frame, digest.ops, digest.s.c_str(), digest.v.c_str(), digest.a.c_str(), digest.b.c_str());
}

// 诊断（TH16_DIGEST_DUMP=<帧>）：把该帧的结构行逐条写到 stream，与 draw-compare.py structure_lines 的输出逐行对照
inline void dump_structure_lines(std::FILE* stream, const std::vector<RecordingDevice::Op>& ops) {
    for (const RecordingDevice::Op& op : ops) {
        if (op.kind != RecordingDevice::OpKind::Clear && op.kind != RecordingDevice::OpKind::Draw) continue;
        const std::string line = draw_digest_detail::structure_line(op);
        std::fprintf(stream, "%s\n", line.c_str());
    }
}

}  // namespace th16
