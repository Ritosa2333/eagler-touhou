// DrawCapture 实现：契约 JSONL 序列化细节全部在本文件（%.9g、键序、
// viewport 的 DWORD/float 口径），与 oracle/render_oracle.py 的 json.dumps 输出对齐。
#include "DrawCapture.hpp"

#include <cmath>
#include <cstring>

namespace th13 {

DrawCapture* draw_capture = nullptr;

namespace {

using touhou::graphics::MatrixKind;
using touhou::graphics::Topology;
using touhou::graphics::VertexLayout;

// D3DPT_* 值（TRIANGLESTRIP=5 等）：port Topology 枚举序与之一致
// （GraphicsState.hpp Primitives 别名实证）。
u32 d3d_primitive(Topology kind) {
    return kind == Topology::Points ? 1u
           : kind == Topology::Lines ? 2u
           : kind == Topology::LineStrip ? 3u
           : kind == Topology::Triangles ? 4u
           : kind == Topology::Strip ? 5u
                                         : 6u;
}

// D3D 顶点数 = 图元参数 → 顶点数（D3DPT_* 口径；与 render_oracle.py 的
// d3d_vertex_count 逐分支同式）。"count" 字段仍是图元数，verts 数组长度必须
// 是顶点数。strip/fan 的 0 图元特例：oracle 返回 0（不是 +1/+2）——0 图元
// 绘制不产生顶点，port 侧必须同语义（此前 count+1/count+2 与 oracle 不同）。
u32 d3d_vertex_count(u32 prim, u32 count) {
    return prim == 1u ? count
           : prim == 2u ? count * 2u
           : prim == 3u ? (count ? count + 1u : 0u)
           : prim == 4u ? count * 3u
                        : (count ? count + 2u : 0u);  // TRIANGLESTRIP / TRIANGLEFAN
}

u32 d3d_fvf(VertexLayout layout) {
    return layout == VertexLayout::WorldUv ? 0x102u
           : layout == VertexLayout::WorldColorUv ? 0x142u
           : layout == VertexLayout::ScreenColorUv ? 0x144u
           : layout == VertexLayout::ScreenColor ? 0x44u
                                                        : 0x104u;
}

u32 matrix_slot(MatrixKind kind) {
    return kind == MatrixKind::World ? 0u
           : kind == MatrixKind::View ? 1u
           : kind == MatrixKind::Projection ? 2u
                                            : 3u;
}

// D3DVIEWPORT9 的 X/Y/W/H 是 DWORD 像素：oracle d3d_read_viewport 以
// "%.9g" % float(dword) 序列化（Python float 即 double，精确表示 u32），
// 这里同式，避免先压成 binary32 再格式化（像素值本身不是 f32 位型）。
void append_pixel(std::string& out, u32 value) {
    char buffer[32];
    const int written = std::snprintf(buffer, sizeof buffer, "%.9g", double(value));
    out.push_back('"');
    out.append(buffer, size_t(written > 0 ? written : 0));
    out.push_back('"');
}

void append_int(std::string& out, i64 value) {
    char buffer[24];
    const int written = std::snprintf(buffer, sizeof buffer, "%lld",
                                      static_cast<long long>(value));
    out.append(buffer, size_t(written > 0 ? written : 0));
}

void append_json_string(std::string& out, const std::string& value) {
    // ASCII 路径与 json.dumps(ensure_ascii=True) 一致；≥0x80 字节按 latin-1
    // 逃生为 \u00XX（窗口内 ANM 纹理名实测全 ASCII，该分支只是可见兜底）。
    out.push_back('"');
    for (const unsigned char c : value) {
        if (c == '"' || c == '\\') {
            out.push_back('\\');
            out.push_back(char(c));
        } else if (c < 0x20 || c >= 0x80) {
            char buffer[8];
            std::snprintf(buffer, sizeof buffer, "\\u%04x", unsigned(c));
            out.append(buffer, 6);
        } else {
            out.push_back(char(c));
        }
    }
    out.push_back('"');
}

}  // namespace

void DrawCapture::append_float(std::string& out, float value) {
    // 契约 §3：%.9g（binary32 往返无损；NaN → "nan"/"-nan" 按符号位）。
    char buffer[40];
    const int written = std::snprintf(buffer, sizeof buffer, "%.9g", double(value));
    if (written <= 0) {
        out.append("\"nan\"");
        return;
    }
    out.push_back('"');
    out.append(buffer, size_t(written));
    out.push_back('"');
}

void DrawCapture::register_texture(const AnmResource* file, u32 first, u32 count, i32 slot) {
    if (!file) return;
    for (u32 index = first; index < first + count; index++) {
        const auto key = std::make_pair(file, index);
        if (handles_.count(key)) continue;
        Identity identity;
        identity.slot = slot;
        identity.entry = i32(index - first);
        identity.name = index < file->textures.size() ? file->textures[index].name : "";
        handles_.emplace(key, u32(identities_.size() + 1));
        identities_.push_back(std::move(identity));
    }
}

bool DrawCapture::open_output(const char* path) {
    out_ = std::fopen(path, "wb");
    if (!out_) {
        fail("cannot open draws output");
        return false;
    }
    // 设备初装：TFACTOR=0（真机 D3D 初始设备态，蓝图 §8.8；共享 GraphicsState
    // 缺省 0xffffffff 非 th13 语义，本作不经 configure_game 路径）。
    pipeline_.textureFactor = 0;
    return true;
}

bool DrawCapture::close_output() noexcept {
    if (out_) {
        if (std::fflush(out_) != 0) fail("draws flush failed");
        if (std::fclose(out_) != 0) fail("draws close failed");
        out_ = nullptr;
    }
    return error_.empty();
}

void DrawCapture::begin_frame(u32 frame) {
    frame_ = frame;
    seq_ = 0;
    line_.clear();
}

bool DrawCapture::end_frame() {
    if (!out_) {
        fail("draws output not open");
        return false;
    }
    std::string out;
    out += "{\"frame\":";
    append_int(out, frame_);
    out += ",\"viewport\":";
    if (viewport_set_) {
        // D3DVIEWPORT9 口径：X/Y/W/H 是 DWORD 像素，oracle/render_oracle.py
        // d3d_read_viewport 按 <6I 读取后把前四项转成数值（不是按 float 位型
        // 解引用；旧口径会把 640 写成 denormal 8.96831017e-43）；MinZ/MaxZ
        // 本就是 float。
        out.push_back('[');
        append_pixel(out, viewport_.x);
        out.push_back(',');
        append_pixel(out, viewport_.y);
        out.push_back(',');
        append_pixel(out, viewport_.width);
        out.push_back(',');
        append_pixel(out, viewport_.height);
        out.push_back(',');
        append_float(out, viewport_.near_depth);
        out.push_back(',');
        append_float(out, viewport_.far_depth);
        out.push_back(']');
    } else {
        out += "null";
    }
    out += ",\"ops\":[";
    out += line_;
    out += "]}\n";
    if (std::fwrite(out.data(), 1, out.size(), out_) != out.size() || std::fflush(out_) != 0) {
        fail("draws write failed");
        return false;
    }
    if (seq_ == 0) zero_frames_++;
    frames_out_++;
    return true;
}

void DrawCapture::sprite(i32 layer, u32 flags) noexcept {
    layer_ = layer;
    anchor_ = ((flags >> 23 & 3u) * 3u + (flags >> 21 & 3u)) * 4u;
}

u32 DrawCapture::texture(const AnmResource& file, u32 index) {
    const auto key = std::make_pair(&file, index);
    const auto found = handles_.find(key);
    if (found != handles_.end()) return found->second;
    // 未登记即被引用：可见标记（不静默；比对时按分歧暴露）。
    Identity identity;
    identity.slot = -1;
    identity.entry = -1;
    identity.name = "port:unregistered:" + std::to_string(index);
    const u32 handle = u32(identities_.size() + 1);
    handles_.emplace(key, handle);
    identities_.push_back(std::move(identity));
    return handle;
}

void DrawCapture::bind_texture(u32 handle) {
    bound_set_ = true;  // handle 0 ≡ SetTexture(0,0)：tex 序列化为 null
    bound_ = handle && handle <= identities_.size() ? identities_[handle - 1]
                                                       : Identity{};
}

void DrawCapture::set_layout(VertexLayout layout) { layout_ = layout; }

void DrawCapture::set_matrix(MatrixKind kind, const Matrix4& matrix) {
    matrices_[matrix_slot(kind)] = matrix;
}

void DrawCapture::append_tex(const Identity& identity) {
    line_ += "[";
    append_int(line_, identity.slot);
    line_ += ",";
    append_int(line_, identity.entry);
    line_ += ",";
    append_json_string(line_, identity.name);
    line_ += "]";
}

void DrawCapture::append_blend() {
    const auto& pipeline = this->pipeline();
    line_ += "[";
    // D3DBLEND_* 从 1 起（ZERO=1、ONE=2、SRCCOLOR=3、INVSRCCOLOR=4、SRCALPHA=5
    // …）；port BlendFactor 枚举恰为 D3D 值 - 1（GraphicsState.hpp 序实证）。
    append_int(line_, i64(pipeline.sourceBlend) + 1);
    line_ += ",";
    append_int(line_, i64(pipeline.destinationBlend) + 1);
    line_ += ",";  // D3DBLENDOP 从 1 起（ADD=1）；port BlendEquation 从 0 起
    append_int(line_, i64(pipeline.blendEquation) + 1);
    line_ += "]";
}

void DrawCapture::append_matrix(const Matrix4& matrix) {
    line_ += "[";
    for (u32 i = 0; i < 16; i++) {
        if (i) line_ += ",";
        append_float(line_, matrix.m[i]);
    }
    line_ += "]";
}

void DrawCapture::emit_quad(u32 prim, u32 count, u32 stride, const void* data) {
    if (!out_) return;
    if (seq_) line_ += ",";
    line_ += "{\"seq\":";
    append_int(line_, seq_++);
    line_ += ",\"op\":\"quad\",\"prim\":";
    append_int(line_, prim);
    line_ += ",\"anchor\":";
    append_int(line_, anchor_);
    line_ += ",\"count\":";
    append_int(line_, count);
    line_ += ",\"tex\":";
    if (bound_set_)
        append_tex(bound_);
    else
        line_ += "null";  // SetTexture(0,0) 之后的绘制（契约 §2.1）
    line_ += ",\"layer\":";
    append_int(line_, layer_);
    line_ += ",\"blend\":";
    append_blend();
    line_ += ",\"tfactor\":";
    append_int(line_, i64(pipeline().textureFactor));
    line_ += ",\"fvf\":";
    append_int(line_, i64(d3d_fvf(layout_)));
    line_ += ",\"world\":";
    append_matrix(matrices_[0]);
    line_ += ",\"texmat\":";
    append_matrix(matrices_[3]);
    line_ += ",\"view\":";
    append_matrix(matrices_[1]);
    line_ += ",\"proj\":";
    append_matrix(matrices_[2]);
    line_ += ",\"verts\":[";
    // [反证修正] 矩阵路径（静态 VB + DrawPrimitive）此前固定写空数组，而
    // oracle render_oracle.py 的 _dev_draw_primitive 按 D3D 顶点口径读回
    // vertex_count(topology,count) 个顶点——窗B 15k+ 个 ascii/loading 粒子
    // quad 的首色因此 port=None vs oracle=0，strict draw_compare 不可能通过。
    // 这里按 oracle _read_vertices 的 FVF 分支序列化：
    //   WorldUv（FVF 0x102，x y z u v）→ [x,y,z,0,0,u,v]（w/color 为 int 0）
    //   其余（WorldColorUv：position+diffuse+uv，无 rhw 字段）→
    //     [x,y,z,0,color,u,v]（rhw 写 float 0，与 oracle 的 fmt_f 同型）
    const auto attributes = touhou::graphics::attributes(layout_);
    const u32 per = stride / 4;
    const u8* bytes = static_cast<const u8*>(data);
    const u32 vertices = d3d_vertex_count(prim, count);
    const bool world_uv = layout_ == VertexLayout::WorldUv;
    for (u32 vertex = 0; vertex < vertices; vertex++) {
        if (vertex) line_ += ",";
        const u8* at = bytes + size_t(vertex) * stride;
        auto field = [&](u32 index) -> float {
            float value = 0;
            if (index < per) std::memcpy(&value, at + index * 4, 4);
            return value;
        };
        u32 color = 0;
        if (attributes.diffuse < stride)
            std::memcpy(&color, at + attributes.diffuse, 4);
        line_ += "[";
        append_float(line_, field(0));
        line_ += ",";
        append_float(line_, field(1));
        line_ += ",";
        append_float(line_, field(2));
        line_ += ",";
        if (world_uv) {
            line_ += "0,0,";
            append_float(line_, field(3));
            line_ += ",";
            append_float(line_, field(4));
        } else {
            append_float(line_, 0.0f);  // rhw（port WorldColorUv 无该字段）
            line_ += ",";
            append_int(line_, color);
            line_ += ",";
            if (attributes.uv < stride) {
                append_float(line_, field(attributes.uv / 4));
                line_ += ",";
                append_float(line_, field(attributes.uv / 4 + 1));
            } else {
                line_ += "0,0";
            }
        }
        line_ += "]";
    }
    line_ += "]}";
    quads_++;
}

void DrawCapture::emit_up(u32 prim, u32 count, u32 stride, const void* data) {
    if (!out_) return;
    if (seq_) line_ += ",";
    line_ += "{\"seq\":";
    append_int(line_, seq_++);
    line_ += ",\"op\":\"up\",\"prim\":";
    append_int(line_, prim);
    line_ += ",\"count\":";
    append_int(line_, count);
    line_ += ",\"stride\":";
    append_int(line_, stride);
    line_ += ",\"tex\":";
    if (bound_set_)
        append_tex(bound_);
    else
        line_ += "null";
    line_ += ",\"layer\":";
    append_int(line_, layer_);
    line_ += ",\"blend\":";
    append_blend();
    line_ += ",\"tfactor\":";
    append_int(line_, i64(pipeline().textureFactor));
    line_ += ",\"fvf\":";
    append_int(line_, i64(d3d_fvf(layout_)));
    line_ += ",\"verts\":[";
    // 契约 §2.2 序列化口径：顶点数 = vertex_count(topology, 图元数参数)，与
    // oracle render_oracle.py 的 d3d_vertex_count 一致（count 字段仍存图元数）。
    const auto attributes = touhou::graphics::attributes(layout_);
    const u32 per = stride / 4;
    const u8* bytes = static_cast<const u8*>(data);
    const u32 vertices = d3d_vertex_count(prim, count);
    for (u32 vertex = 0; vertex < vertices; vertex++) {
        if (vertex) line_ += ",";
        const u8* at = bytes + size_t(vertex) * stride;
        auto field = [&](u32 index) -> float {
            float value = 0;
            if (index < per) std::memcpy(&value, at + index * 4, 4);
            return value;
        };
        u32 color = 0;
        if (attributes.diffuse < stride)
            std::memcpy(&color, at + attributes.diffuse, 4);
        line_ += "[";
        append_float(line_, field(0));
        line_ += ",";
        append_float(line_, field(1));
        line_ += ",";
        append_float(line_, field(2));
        line_ += ",";
        append_float(line_, field(3));
        line_ += ",";
        append_int(line_, color);
        line_ += ",";
        if (attributes.uv < stride) {
            append_float(line_, field(attributes.uv / 4));
            line_ += ",";
            append_float(line_, field(attributes.uv / 4 + 1));
        } else {
            // FVF 0x44（XYZRHW|DIFFUSE，无 UV）：u=v 恒 JSON int 0——与
            // oracle _dev_draw_primitive_up 的 per>=5 且 per<7 分支（`u=0,
            // v=0` 整数）同型。此前输出字符串 "0" 会造成 draw_compare 的
            // str-vs-int 潜在假分歧（当前窗口 port 无 0x44 up op 不可达）。
            line_ += "0,0";
        }
        line_ += "]";
    }
    line_ += "]}";
    ups_++;
}

void DrawCapture::triangles(u32 count, const void* data, u32 stride) {
    // AnmRenderer 的批 flush：预变换顶点 TRIANGLELIST（原版 0x4679a0 同位）。
    emit_up(4u, count, stride, data);
}

void DrawCapture::primitives(Topology kind, u32 count, const void* data, u32 stride) {
    const u32 prim = d3d_primitive(kind);
    if (layout_ == VertexLayout::WorldUv || layout_ == VertexLayout::WorldColorUv) {
        // 矩阵路径（原版 §2.1 静态 VB + DrawPrimitive）。
        emit_quad(prim, count, stride, data);
        return;
    }
    // 预变换直交（原版 §2.2/§2.3 DrawPrimitiveUP 站点）。
    emit_up(prim, count, stride, data);
}

}  // namespace th13
