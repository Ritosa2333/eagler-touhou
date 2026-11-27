// th16 ANM 管理器实现——VM 池、链表、装载与逐帧推进（地址见 AnmManager.hpp）。
#include "AnmManager.hpp"

namespace th16 {

// 0x4090f0：VM 复位（保留 0x5e0 偏移、池下标、layer）
void AnmManager::reset_vm(AnmVm& vm) {
    const Vec3 offset = vm.entity_offset;
    const u32 pool_index = vm.pool_index;
    const i32 layer = vm.layer;
    vm.clear();
    vm.entity_offset = offset;
    vm.pool_index = pool_index;
    vm.layer = layer;
    vm.scale = {1, 1};
    vm.scale2 = {1, 1};
    vm.uv_scale = {1, 1};
    vm.color[0] = vm.color[1] = vm.color[2] = vm.color[3] = 255;
    for (int k = 0; k < 16; ++k) vm.texture_matrix[k] = (k % 5 == 0) ? 1.0f : 0.0f;
    vm.flags_lo = 7;
    vm.time.current = 0; vm.time.previous = i32(0xfff0bdc1); vm.time.current_f = 0;
    vm.age.current = 0; vm.age.previous = i32(0xfff0bdc1); vm.age.current_f = 0;
    vm.flags_hi = (vm.flags_hi & 0xffff7fffu) | 0x4000u;
    vm.rand_scale = 1.0f;
    vm.rand_angle_scale = 3.1415927410125732f;
    vm.rand_int_max = 0x10000;
    vm.root = nullptr;
    vm.parent = nullptr;
    vm.world_node = {&vm, nullptr, nullptr, nullptr};
    vm.sibling_node = {&vm, nullptr, nullptr, nullptr};
    vm.children = {&vm, nullptr, nullptr, nullptr};
    vm.delete_node = {&vm, nullptr, nullptr, nullptr};
}

namespace {

// 0x407a50：spawn 前清 0x538..0x5fc 尾部（保留 layer / pool_index / entity_offset）并重建链表结点。
// 逐字段清零而非按原版字节范围 memset：64 位宿主（测试 harness）下指针字段把尾部撑开，字节偏移不再对应。
void reset_tail(AnmVm& vm) {
    vm.id = 0;
    vm.time = Timer{};
    vm.age = Timer{};
    vm.world_node = {&vm, nullptr, nullptr, nullptr};
    vm.sibling_node = {&vm, nullptr, nullptr, nullptr};
    vm.children = {&vm, nullptr, nullptr, nullptr};
    vm.delete_node = {nullptr, nullptr, nullptr, nullptr};
    vm.layer_next = nullptr;
    vm.root = nullptr;
    vm.parent = nullptr;
    vm.slowdown = 0;
    vm.vertex_buffer = nullptr;
    vm.vertex_buffer_size = 0;
    vm.time_callback = 0;
    vm.update_callback = 0;
    vm.draw_callback = 0;
    vm.delete_callback = 0;
    vm.interrupt_callback = 0;
    std::memset(vm.unknown_5d4, 0, sizeof vm.unknown_5d4);
    vm.sprite_mapper = 0;
    vm.unknown_5ec = 0;
    vm.world_rotation = Vec3{};
    vm.sprite_source_slot = -1;   // 移植附加字段：上一任 VM 的精灵来源不能带到新脚本
}

// 0x45f020：模板初始化
void init_template(AnmFile& file, AnmVm& vm, int script) {
    if (file.scripts[script] == nullptr) { vm.clear(); return; }
    AnmManager::reset_vm(vm);
    vm.template_script = i16(script);
    vm.flags_lo &= 0xffffe7ffu;
    vm.file_slot = file.slot;
    vm.script_index = script;
    vm.ins_offset = 0;
    vm.flags_hi = (vm.flags_hi & 0xffffbfffu) | 0x8000u;
    vm.age.reset();
    vm.time.reset();
    vm.flags_lo &= ~AnmVm::kVisible;
}

void unlink(AnmListNode& node) {
    if (node.next != nullptr) node.next->prev = node.prev;
    if (node.prev != nullptr) node.prev->next = node.next;
    node.next = nullptr;
    node.prev = nullptr;
}

}  // namespace

// 速度（引用）在类里先于池声明，初始化列表按声明次序
AnmManager::AnmManager() : game_speed(game_speed_storage), pool_(kPoolSize), pool_in_use_(kPoolSize, 0) {
    init_pool();
}

AnmManager::AnmManager(float& speed) : game_speed(speed), pool_(kPoolSize), pool_in_use_(kPoolSize, 0) {
    init_pool();
}

void AnmManager::init_pool() {
    free_stack_.reserve(kPoolSize);
    for (u32 i = 0; i < kPoolSize; ++i) {
        pool_[i].pool_index = i;
        pool_[i].world_node.owner = &pool_[i];
        pool_[i].sibling_node.owner = &pool_[i];
        pool_[i].children.owner = &pool_[i];
        pool_[i].delete_node.owner = &pool_[i];
        free_stack_.push_back(i);  // 构造时逐个头插 → 栈顶为 0x1ffe
    }
}

AnmFile* AnmManager::load(int slot, const AnmResource& resource, float first_entry_sprite_scale) {
    auto f = std::make_unique<AnmFile>();
    f->slot = slot;
    f->resource = &resource;
    f->sprites.reserve(resource.total_sprites);
    f->scripts.reserve(resource.total_scripts);
    for (u32 e = 0; e < resource.entries.size(); ++e) {
        const AnmEntry& entry = resource.entries[e];
        // 0x46d3b0：'@R' 渲染目标 entry 的头部宽高被改写为后备缓冲尺寸（DAT_004d9d2c/30 = 640/480 × 缩放），
        // 其余 entry 由 D3DXCreateTexture 按声明尺寸创建并经 GetLevelDesc 回读
        u32 entry_w = entry.width, entry_h = entry.height;
        if (entry.name.size() >= 2 && entry.name[0] == '@' && entry.name[1] == 'R') {
            entry_w = render_target_width;
            entry_h = render_target_height;
        }
        const u32 tex_w = entry_w, tex_h = entry_h;
        f->texture_width.push_back(tex_w);
        f->texture_height.push_back(tex_h);
        const float ratio_x = float(tex_w) / float(entry_w);
        const float ratio_y = float(tex_h) / float(entry_h);
        // Keep a denser atlas's UVs while converting its sprite dimensions to logical pixels.
        const float sprite_scale = e == 0 ? first_entry_sprite_scale : 1.0f;
        for (const AnmSprite& s : entry.sprites) {
            AnmRuntimeSprite r;
            r.file_slot = slot;
            r.entry_index = i32(e);
            r.key = i32((u32(slot) << 8) | e);
            r.x0 = (s.x * ratio_x) * sprite_scale;
            r.y0 = (s.y * ratio_y) * sprite_scale;
            r.x1 = ((s.w + s.x) * ratio_x) * sprite_scale;
            r.y1 = ((s.h + s.y) * ratio_y) * sprite_scale;
            r.tex_h = float(tex_h) * sprite_scale;
            r.tex_w = float(tex_w) * sprite_scale;
            r.ratio_x = ratio_x;
            r.ratio_y = ratio_y;
            r.u0 = r.x0 / r.tex_w;
            r.u1 = r.x1 / r.tex_w;
            r.v0 = r.y0 / r.tex_h;
            r.v1 = r.y1 / r.tex_h;
            r.width = (r.x1 - r.x0) / ratio_x;
            r.height = (r.y1 - r.y0) / ratio_y;
            f->sprites.push_back(r);
        }
        for (const AnmScript& s : entry.scripts) f->scripts.push_back(&s);
    }
    AnmFile* file = f.get();
    banks_[slot] = std::move(f);
    file->templates.resize(file->scripts.size());
    for (u32 i = 0; i < file->scripts.size(); ++i) {
        AnmVm& t = file->templates[i];
        t.pool_index = 0;
        reset_vm(t);
        init_template(*file, t, int(i));
        t.time.set(-1);
        t.age.set(-1);
        run(t);
    }
    return file;
}

void AnmManager::kill_file(int slot) {
    // 0x46f270：按 AnmFile.slot（= bank 下标）给世界/UI 链上的 VM 打 0x20 待删位
    auto mark = [slot](AnmListNode* head) {
        for (AnmListNode* n = head; n != nullptr; n = n->next) {
            AnmVm& vm = *n->owner;
            if (vm.file_slot == slot) vm.flags_hi = (vm.flags_hi & 0xffffffbfu) | 0x20u;
        }
    };
    mark(world_head_);
    mark(ui_head_);
}

void AnmManager::unload(int slot) {
    if (slot < 0 || slot >= kBankSlots) return;
    banks_[slot].reset();
}

AnmVm* AnmManager::allocate() {
    if (!free_stack_.empty()) {
        const u32 index = free_stack_.back();
        free_stack_.pop_back();
        AnmVm& vm = pool_[index];
        pool_in_use_[index] = 1;
        vm.pool_index = index;
        vm.root = nullptr;
        vm.parent = nullptr;
        vm.world_node = {&vm, nullptr, nullptr, nullptr};
        vm.sibling_node = {&vm, nullptr, nullptr, nullptr};
        vm.children = {&vm, nullptr, nullptr, nullptr};
        return &vm;
    }
    AnmVm* vm = new AnmVm();
    vm->pool_index = 0x1fff;  // 池耗尽时的堆分配 VM（0x46f600 末尾路径）
    reset_vm(*vm);
    return vm;
}

void AnmManager::init_from_template(AnmFile& file, AnmVm& vm, int script) {
    reset_tail(vm);
    std::memcpy(static_cast<void*>(&vm), static_cast<const void*>(&file.templates[script]), 0x538);
    vm.age.reset();
    vm.time.reset();
}

// 0x45f160：与 0x45f020 相同的脚本绑定，但随即跑首帧并把 0x8000（首帧前）转成 0x4000
void AnmManager::init_script(AnmFile& file, AnmVm& vm, int script) {
    if (script < 0 || u32(script) >= file.scripts.size() || file.scripts[u32(script)] == nullptr) {
        // 原版 memset(vm, 0, 0x5fc)：脚本缺失时整个 VM 清零；宿主指针是移植附加字段，保留以免精灵映射钩子解引用空指针
        void* owner = vm.owner;
        vm.clear();
        vm.owner = owner;
        return;
    }
    vm.template_script = i16(script);
    vm.flags_lo &= 0xffffe7ffu;
    vm.file_slot = file.slot;
    vm.script_index = script;
    vm.ins_offset = 0;
    vm.flags_hi = (vm.flags_hi & 0xffffbfffu) | 0x8000u;
    vm.age.reset();
    vm.time.reset();
    vm.flags_lo &= ~AnmVm::kVisible;
    run(vm);
    ++spawn_count;
    if ((vm.flags_hi & 0xc000u) == 0x8000u) vm.flags_hi = (vm.flags_hi & 0xffff7fffu) | 0x4000u;
}

u32 AnmManager::next_id(AnmVm& vm) {
    id_counter_ = (id_counter_ + 1) & 0x7ffffu;
    if (id_counter_ == 0) id_counter_ = 1;
    vm.id = (vm.pool_index & 0x1fffu) | (id_counter_ << 13);
    return vm.id;
}

u32 AnmManager::insert_world_back(AnmVm& vm) {
    AnmListNode& node = vm.world_node;
    node = {&vm, nullptr, nullptr, nullptr};
    if (world_head_ == nullptr) {
        world_head_ = &node;
    } else {
        AnmListNode* tail = world_tail_;
        if (tail->next != nullptr) { node.next = tail->next; tail->next->prev = &node; }
        if (tail->extra != nullptr) tail->extra = &node;
        tail->next = &node;
        node.prev = tail;
    }
    world_tail_ = &node;
    return next_id(vm);
}

u32 AnmManager::insert_world_front(AnmVm& vm) {
    AnmListNode& node = vm.world_node;
    node = {&vm, nullptr, nullptr, nullptr};
    if (world_head_ == nullptr) {
        world_tail_ = &node;
    } else {
        AnmListNode* head = world_head_;
        node.next = head;
        head->prev = &node;
    }
    world_head_ = &node;
    return next_id(vm);
}

u32 AnmManager::insert_ui_back(AnmVm& vm) {
    AnmListNode& node = vm.world_node;
    node = {&vm, nullptr, nullptr, nullptr};
    if (ui_head_ == nullptr) {
        ui_head_ = &node;
    } else {
        AnmListNode* tail = ui_tail_;
        if (tail->next != nullptr) { node.next = tail->next; tail->next->prev = &node; }
        if (tail->extra != nullptr) tail->extra = &node;
        tail->next = &node;
        node.prev = tail;
    }
    ui_tail_ = &node;
    return next_id(vm);
}

u32 AnmManager::insert_ui_front(AnmVm& vm) {
    AnmListNode& node = vm.world_node;
    node = {&vm, nullptr, nullptr, nullptr};
    if (ui_head_ == nullptr) {
        ui_tail_ = &node;
    } else {
        AnmListNode* head = ui_head_;
        node.next = head;
        head->prev = &node;
    }
    ui_head_ = &node;
    return next_id(vm);
}

// 0x406380
u32 AnmManager::spawn(AnmFile& file, int script, int layer, AnmVm** out_vm) {
    return spawn_vm(file, script, layer, Vec3{}, 0.0f, 0, false, out_vm);
}

// 0x40e5c0
u32 AnmManager::spawn_effect(AnmFile& file, int script, int layer, const Vec3& pos, float rotation_z) {
    return spawn_vm(file, script, layer, pos, rotation_z, 0, false, nullptr);
}

// 0x426160
u32 AnmManager::spawn_front(AnmFile& file, int script, int layer, AnmVm** out_vm) {
    return spawn_vm(file, script, layer, Vec3{}, 0.0f, 2, true, out_vm);
}

u32 AnmManager::spawn_vm(AnmFile& file, int script, int layer, const Vec3& offset, float rotation_z,
                         i32 creation_flags, bool front, AnmVm** out_vm) {
    ++spawn_count;
    AnmVm* vm = allocate();
    if (out_vm != nullptr) *out_vm = vm;
    init_from_template(file, *vm, script);
    const u32 hi = vm->flags_hi;
    vm->flags_hi = hi | 0x400u;
    if (layer >= 0) {
        vm->layer = layer;
        if (layer < 0x18) vm->flags_hi = (hi & 0xfff7ffffu) | 0x40400u;
    }
    vm->entity_offset = offset;
    vm->rotation.z = rotation_z;
    run(*vm);
    vm->creation_flags = creation_flags;
    return front ? insert_world_front(*vm) : insert_world_back(*vm);
}

// 0x418fe0 / 0x42c920
u32 AnmManager::spawn_ui(AnmFile& file, int script, AnmVm** out_vm) {
    ++spawn_count;
    AnmVm* vm = allocate();
    if (out_vm != nullptr) *out_vm = vm;
    init_from_template(file, *vm, script);
    vm->flags_hi |= 0x400u;
    vm->entity_offset = {};
    vm->rotation.z = 0.0f;
    run(*vm);
    vm->creation_flags = 4;
    const u32 id = insert_ui_back(*vm);
    vm->flags_hi &= 0xffff3fffu;
    return id;
}

// 0x406340
float AnmManager::root_slowdown(const AnmVm& vm) noexcept {
    const AnmVm* cur = &vm;
    const AnmVm* up = cur->root;
    while (up != nullptr && (cur->flags_hi & AnmVm::kHiDetached) == 0) {
        cur = up;
        up = cur->root;
    }
    return cur->slowdown;
}

// 0x46f220
void AnmManager::kill_tree(AnmVm& vm) {
    if ((vm.flags_hi & 0x4000000u) != 0) return;
    vm.flags_hi = (vm.flags_hi & ~0x40u) | 0x20u;
    for (AnmListNode* n = vm.children.next; n != nullptr; n = n->next) {
        if (n->owner) kill_tree(*n->owner);
    }
}

// 0x46f3b0
void AnmManager::hide_tree(AnmVm& vm) {
    vm.flags_lo &= ~2u;
    for (AnmListNode* n = vm.children.next; n != nullptr; n = n->next) {
        if (n->owner) hide_tree(*n->owner);
    }
}

// 0x46f380
void AnmManager::show_tree(AnmVm& vm) {
    vm.flags_lo |= 2u;
    for (AnmListNode* n = vm.children.next; n != nullptr; n = n->next) {
        if (n->owner) show_tree(*n->owner);
    }
}

// 0x46f1c0
void AnmManager::kill(u32 id) {
    AnmVm* vm = find(id);
    if (vm == nullptr) return;
    kill_tree(*vm);
}

// 0x46ed60
AnmVm* AnmManager::create_child(AnmFile& file, int script, AnmVm& parent, u32 list_flags, u32* out_id) {
    ++spawn_count;
    AnmVm* vm = allocate();
    vm->flags_hi |= 0x400u;
    vm->layer = parent.layer;
    vm->entity_offset = {};
    init_from_template(file, *vm, script);
    vm->parent = &parent;
    vm->flags_hi ^= (parent.flags_hi ^ vm->flags_hi) & 0x2000000u;
    vm->root = parent.root != nullptr ? parent.root : &parent;
    run(*vm);
    vm->creation_flags = i32(list_flags);
    u32 id;
    if ((list_flags & 6) == 6) id = insert_ui_front(*vm);
    else if ((list_flags & 4) == 0) id = (list_flags & 2) == 0 ? insert_world_back(*vm) : insert_world_front(*vm);
    else id = insert_ui_back(*vm);
    if (out_id != nullptr) *out_id = id;
    AnmListNode& node = vm->sibling_node;
    node.owner = vm;
    if (parent.children.next != nullptr) {
        node.next = parent.children.next;
        parent.children.next->prev = &node;
    }
    if (parent.children.extra != nullptr) parent.children.extra = &node;
    parent.children.next = &node;
    node.prev = &parent.children;
    return vm;
}

// 0x46eea0
AnmVm* AnmManager::create_detached(AnmFile& file, int script, AnmVm& parent, u32* out_id) {
    ++spawn_count;
    AnmVm* vm = allocate();
    init_from_template(file, *vm, script);
    vm->layer = parent.layer;
    vm->flags_hi |= 0x400u;
    vm->flags_hi = ((parent.flags_hi ^ vm->flags_hi) & 0x2000000u) ^ vm->flags_hi;
    vm->entity_offset = parent.entity_offset;
    vm->rotation = parent.rotation;
    vm->pos2 = parent.pos;
    run(*vm);
    vm->creation_flags = 0;
    const u32 id = insert_world_back(*vm);
    if (out_id != nullptr) *out_id = id;
    return vm;
}

// 0x46efa0
AnmVm* AnmManager::find(u32 id) {
    if (id == 0) return nullptr;
    if ((id & 0x1fffu) == 0x1fffu) {
        for (AnmListNode* n = world_head_; n != nullptr; n = n->next) if (n->owner->id == id) return n->owner;
        for (AnmListNode* n = ui_head_; n != nullptr; n = n->next) if (n->owner->id == id) return n->owner;
        return nullptr;
    }
    const u32 index = id & 0x1fffu;
    if (pool_in_use_[index] != 0 && pool_[index].id == id) return &pool_[index];
    return nullptr;
}

// 0x46f510
AnmVm* AnmManager::find_child(AnmVm& vm, i32 script, i32 nth) {
    for (AnmListNode* node = &vm.children; node != nullptr; node = node->next) {
        AnmVm* child = node->owner;
        if (child == nullptr || child == &vm) continue;
        if (i32(child->template_script) == script || script == -1) {
            if (nth == 0) return child;
            --nth;
        }
        if (child->children.next != nullptr) {
            if (AnmVm* deep = find_child(*child, script, nth)) return deep;
        }
        // 原版遗留分支：模板脚本 -2 且已到表尾时返回当前结点
        if (vm.template_script == -2 && node->next == nullptr) return child;
    }
    return nullptr;
}

// 0x46f0b0（interrupt_callback 表 DAT_00491b0c 暂无登记项，原版此表全部为空函数指针槽）
void AnmManager::interrupt(u32 id, i32 value) {
    AnmVm* vm = find(id);
    if (vm == nullptr) return;
    if (vm->interrupt_callback != 0 && hooks != nullptr) hooks->interrupted(*vm, value);
    vm->pending_interrupt = value;
    for (AnmListNode* n = vm->children.next; n != nullptr; n = n->next) {
        if (n->owner == nullptr) continue;
        if (n->owner->interrupt_callback != 0 && hooks != nullptr) hooks->interrupted(*n->owner, value);
        n->owner->pending_interrupt = value;
    }
}

// 0x4174d0（及 0x42c280 / 0x42c600 等处的内联展开）
void AnmManager::request_interrupt(AnmVm& vm, i32 value) {
    if (vm.interrupt_callback != 0 && hooks != nullptr) hooks->interrupted(vm, value);
    vm.pending_interrupt = value;
}

// 0x42c890 / 0x418440 / 0x418470：置中断后立刻跑一帧，让 VM 当帧就切到中断分支
void AnmManager::interrupt_now(AnmVm& vm, i32 value) {
    request_interrupt(vm, value);
    run(vm);
}

// 0x46f130
void AnmManager::interrupt_now(u32 id, i32 value) {
    AnmVm* vm = find(id);
    if (vm == nullptr) return;
    if (vm->interrupt_callback != 0 && hooks != nullptr) hooks->interrupted(*vm, value);
    vm->pending_interrupt = value;
    run(*vm);
    for (AnmListNode* n = vm->children.next; n != nullptr; n = n->next) {
        if (n->owner == nullptr) continue;
        if (n->owner->interrupt_callback != 0 && hooks != nullptr) hooks->interrupted(*n->owner, value);
        n->owner->pending_interrupt = value;
        run(*n->owner);
    }
}

// 0x46e660：递归标记删除并头插到待删链
void AnmManager::mark_deleted(AnmVm& vm, AnmListNode& list) {
    for (AnmListNode* n = vm.children.next; n != nullptr;) {
        AnmListNode* next = n->next;
        mark_deleted(*n->owner, list);
        n = next;
    }
    if ((vm.flags_hi & 0x60u) != 0x40u) {
        AnmListNode& node = vm.delete_node;
        node = {&vm, nullptr, nullptr, nullptr};
        if (list.next != nullptr) { node.next = list.next; list.next->prev = &node; }
        if (list.extra != nullptr) list.extra = &node;
        list.next = &node;
        node.prev = &list;
    }
    vm.root = nullptr;
    vm.parent = nullptr;
    vm.flags_hi = (vm.flags_hi & 0xffffffdfu) | 0x40u;
}

// 0x46eab0
void AnmManager::release(AnmVm& vm) {
    AnmListNode& node = vm.world_node;
    if (&node == world_tail_) world_tail_ = node.prev;
    if (&node == world_head_) world_head_ = node.next;
    if (&node == ui_tail_) ui_tail_ = node.prev;
    if (&node == ui_head_) ui_head_ = node.next;
    if (vm.delete_callback != 0 && hooks != nullptr) hooks->deleted(vm);
    unlink(node);
    unlink(vm.delete_node);
    unlink(vm.sibling_node);
    vm.root = nullptr;
    vm.parent = nullptr;
    if (vm.pool_index < kPoolSize && &pool_[vm.pool_index] == &vm) {
        pool_in_use_[vm.pool_index] = 0;
        free_stack_.push_back(vm.pool_index);
        if (vm.vertex_buffer != nullptr && vm.vertex_buffer_free != nullptr) vm.vertex_buffer_free(vm.vertex_buffer);
        vm.vertex_buffer = nullptr;
        vm.vertex_buffer_free = nullptr;
        vm.vertex_buffer_size = 0;
        vm.ins_offset = -1;
        vm.id = 0;
        return;
    }
    delete &vm;
}

void AnmManager::tick_list(AnmListNode*& head, bool ui) {
    // 各自只清本表的层链：世界表 0..0x23，UI 表 0x24..0x2a
    const int first = ui ? kWorldLayerCount : 0;
    const int last = ui ? kLayerCount : kWorldLayerCount;
    for (int l = first; l < last; ++l) layer_heads_[l] = nullptr;
    AnmVm* tails[kLayerCount] = {};
    AnmListNode pending{nullptr, nullptr, nullptr, nullptr};
    for (AnmListNode* n = head; n != nullptr;) {
        AnmListNode* next = n->next;
        AnmVm& vm = *n->owner;
        const u32 state = vm.flags_hi & 0x60u;
        if (state == 0x20u) {
            mark_deleted(vm, pending);
        } else if (state == 0) {
            if (run(vm) == 0) {
                int layer = vm.layer;
                if (ui) {
                    if (layer < 0x18 || 0x1f < layer) { if (layer < 0x24 || 0x2a < layer) layer = 0x26; }
                    else layer += 0xc;
                } else if (0x23 < layer && layer < 0x2b) {
                    layer -= 0xc;
                }
                vm.layer = layer;
                if (tails[layer] != nullptr) tails[layer]->layer_next = &vm; else layer_heads_[layer] = &vm;
                vm.layer_next = nullptr;
                ++world_count;
                tails[layer] = &vm;
            } else {
                mark_deleted(vm, pending);
            }
        }
        n = next;
    }
    for (AnmListNode* n = pending.next; n != nullptr;) {
        AnmListNode* next = n->next;
        release(*n->owner);
        n = next;
    }
}

void AnmManager::tick_world() { tick_list(world_head_, false); }
void AnmManager::tick_ui() { tick_list(ui_head_, true); }

}  // namespace th16
