#include "Distortion.hpp"

#include "AnmGeometry.hpp"

namespace th16 {

namespace {

constexpr int kTextScript = 0x3b;   // text.anm：RT1 全幅精灵的合成脚本
constexpr int kLayer = 0x22;        // 绘制链 0x43d2f0 在 RT1 → RT2 拷贝后画的层

void free_vertices(void* p) { delete[] static_cast<ScreenColorUvVertex*>(p); }

}  // namespace

// 0x418c70
DistortionGrid::DistortionGrid(AnmManager& animations, AnmFile& text, bool render_targets_ready, i32 rows)
    : animations_(&animations) {
    if (!render_targets_ready) return;
    columns_ = kColumns;
    rows_ = rows;
    strip_ids_.assign(size_t(columns_ - 1), 0u);
    strip_vms_.assign(size_t(columns_ - 1), nullptr);
    vertices_.assign(size_t(columns_) * size_t(rows_), ScreenColorUvVertex{});
    nodes_.assign(size_t(columns_) * size_t(rows_), Vec3{});

    // 母 VM：同一合成脚本，mode 清 0（画出一张与合成 VM 相同的 RT1 全幅四边形），绘制钩子 4 回调本对象
    AnmVm* master = nullptr;
    master_id_ = animations.spawn(text, kTextScript, kLayer, &master);
    // 原版还给母 VM 配了 0x70 字节（4 顶点）的宿主缓冲，但 mode 0 从不读取，这里不分配
    master->flags_lo &= 0xc1ffffffu;
    master->draw_callback = 4;
    master->owner = this;

    for (i32 i = 0; i < columns_ - 1; ++i) {
        strip_ids_[size_t(i)] = make_strip_vm(animations, text, rows_);
        AnmVm* strip = animations.find(strip_ids_[size_t(i)]);
        if (strip == nullptr) strip_ids_[size_t(i)] = 0;
        strip_vms_[size_t(i)] = strip;
        if (strip == nullptr) continue;
        strip->flags_lo &= 0xfffffe1fu;   // blend 位清零（正常混合）
        strip->flags_hi &= 0xfff3ffffu;   // 层组位清零（不加场地 / 屏幕偏移）
    }
}

// 0x409550
DistortionGrid::~DistortionGrid() {
    if (animations_ == nullptr) return;
    animations_->kill(master_id_);
    master_id_ = 0;
    for (u32& id : strip_ids_) {
        if (AnmVm* vm = animations_->find(id)) AnmManager::kill_tree(*vm);
        id = 0;
    }
}

// 0x43d8b0
u32 DistortionGrid::make_strip_vm(AnmManager& animations, AnmFile& text, i32 rows) {
    AnmVm* vm = nullptr;
    const u32 id = animations.spawn(text, kTextScript, kLayer, &vm);
    const u32 count = u32(rows) * 2u;
    auto* vertices = new ScreenColorUvVertex[count]();
    vm->vertex_buffer_size = count * u32(sizeof(ScreenColorUvVertex));
    vm->vertex_buffer = vertices;
    vm->vertex_buffer_free = free_vertices;
    if (rows < 3) {
        vm->flags_lo &= 0xc1ffffffu;   // 节点太少：退回 mode 0
        return id;
    }
    vm->int_vars[0] = rows;
    vm->flags_lo = (vm->flags_lo & 0xd9ffffffu) | 0x18000000u;   // mode 0xc
    for (u32 i = 0; i < count; ++i) {
        vertices[i].z = 0.0f;
        vertices[i].rhw = 1.0f;
        vertices[i].color = 0xffffffffu;
    }
    return id;
}

// 0x418df0
void DistortionGrid::fill(float x, float y, float width, float height, const AnmGeometry& geometry, u32 target_width,
                          u32 target_height) {
    if (columns_ == 0) return;
    const float step_x = width / float(columns_ - 1);
    const float step_y = height / float(rows_ - 1);
    const float target_w = float(target_width);    // DAT_004d9d2c
    const float target_h = float(target_height);   // DAT_004d9d30
    float gx = geometry.field_offset_x + x;
    ScreenColorUvVertex* vertex = vertices_.data();
    Vec3* node = nodes_.data();
    for (i32 column = 0; column < columns_; ++column) {
        float gy = geometry.field_offset_y + y;
        for (i32 row = 0; row < rows_; ++row, ++vertex, ++node) {
            node->x = gx;
            node->y = gy;
            node->z = 0.0f;
            vertex->x = gx;
            vertex->y = gy;
            vertex->z = 0.0f;
            vertex->u = node->x / target_w;
            vertex->v = node->y / target_h;
            if (vertex->u < 0.0f) vertex->u = 0.0f;
            if (vertex->v < 0.0f) vertex->v = 0.0f;
            vertex->rhw = 1.0f;
            gy = gy + step_y;
            vertex->color = 0xffffffffu;
        }
        gx = gx + step_x;
    }
    copy_to_strips();
}

// 0x418f40
void DistortionGrid::copy_to_strips() {
    if (columns_ == 0) return;
    for (i32 strip = 0; strip < columns_ - 1; ++strip) {
        AnmVm* vm = strip_vms_[size_t(strip)];
        if (vm == nullptr || vm->vertex_buffer == nullptr) continue;
        auto* out = static_cast<ScreenColorUvVertex*>(vm->vertex_buffer);
        const ScreenColorUvVertex* left = vertices_.data() + size_t(strip) * size_t(rows_);
        const ScreenColorUvVertex* right = left + rows_;
        for (i32 row = 0; row < rows_; ++row) {
            out[row * 2] = left[row];
            out[row * 2 + 1] = right[row];
        }
    }
}

// 0x418c60
void distortion_draw_callback(AnmVm& vm) {
    if (vm.owner != nullptr) static_cast<DistortionGrid*>(vm.owner)->copy_to_strips();
}

}  // namespace th16
