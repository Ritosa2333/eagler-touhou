// text.anm 对话动态纹理提交定向测试（不依赖 SDL/字体资产）。
// 正控：四 VM 创建、texture0 sprite 条带写入、changed 区域、行块缓存/清空/释放。
// 负控：非 ARGB4444 texture0、缺失 text.anm、越界行都必须显式失败。
#include "../harness/test_framework.hpp"
#include "ui_test_fixtures.hpp"

#include "game/DialogueText.hpp"

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>
#include <vector>

using namespace th13;
using namespace th13::test;

namespace {

struct AnmInstructionSpec {
    i16 opcode = 0;
    i16 time = 0;
    std::vector<i32> args;
};

u32 encoded_length(const AnmInstructionSpec& instruction) {
    return u32(8 + instruction.args.size() * 4);
}

// 合成 text.anm entry0：Blank "@" 512x512 format5(ARGB4444)、四 sprite
// (0,1/24/47/70,512,20)、四脚本（sprite i + 层 26 + 位置 + 真实 ins_4
// 隐藏等待 + label2/label3/label1 分支）。0x428ae0 用 0x46fc30(handle,2)
// 让 ins_4 等待中的 VM 跳到显示分支；label3 用于块首清槽、label1 释放。
std::vector<u8> synthetic_text_anm() {
    constexpr u32 kSprites = 4, kScripts = 4;
    const u32 table_end = 64 + kSprites * 4 + kScripts * 8;
    u32 cursor = table_end;
    std::vector<u8> body;
    std::vector<u32> sprite_offsets(kSprites), script_offsets(kScripts);
    for (u32 index = 0; index < kSprites; index++) {
        sprite_offsets[index] = cursor;
        std::vector<u8> record(20, 0);
        const u32 sprite_id = index;
        const float x = 0.f, y = 1.f + 23.f * float(index), width = 512.f, height = 20.f;
        std::memcpy(record.data(), &sprite_id, 4);
        std::memcpy(record.data() + 4, &x, 4);
        std::memcpy(record.data() + 8, &y, 4);
        std::memcpy(record.data() + 12, &width, 4);
        std::memcpy(record.data() + 16, &height, 4);
        body.insert(body.end(), record.begin(), record.end());
        cursor += 20;
    }
    for (u32 index = 0; index < kScripts; index++) {
        script_offsets[index] = cursor;
        const i32 sprite = i32(index);
        const i32 layer = 26;
        const std::vector<AnmInstructionSpec> script = {
            {300, 0, {sprite}}, {304, 0, {layer}}, {400, 0, {0, 0, 0}},
            {4, 0, {}}, {5, 0, {2}}, {3, 8, {}}, {5, 8, {3}}, {7, 8, {}},
            {5, 8, {1}}, {-1, 16, {}}};
        for (const auto& instruction : script) {
            std::vector<u8> encoded(8, 0);
            const u16 length = u16(encoded_length(instruction));
            std::memcpy(encoded.data() + 0, &instruction.opcode, 2);
            std::memcpy(encoded.data() + 2, &length, 2);
            std::memcpy(encoded.data() + 4, &instruction.time, 2);
            body.insert(body.end(), encoded.begin(), encoded.end());
            for (i32 arg : instruction.args) {
                u8 bytes[4];
                std::memcpy(bytes, &arg, 4);
                body.insert(body.end(), bytes, bytes + 4);
            }
            cursor += encoded_length(instruction);
        }
    }
    // 位置参数改写为真实浮点（2,22 / 2,42 / 16,16 / 16,36）。
    for (u32 index = 0; index < kScripts; index++) {
        const u32 at = script_offsets[index] + encoded_length({300, 0, {0}}) +
                       encoded_length({304, 0, {0}}) + 8;
        const float values[3] = {index < 2 ? 2.f : 16.f,
                                 index < 2 ? 22.f + 20.f * float(index)
                                           : 16.f + 20.f * float(index - 2),
                                 0.f};
        std::memcpy(body.data() + (at - table_end), values, sizeof values);
    }
    const char name[] = "@";
    const u32 name_at = cursor;
    std::vector<u8> chunk(table_end + body.size() + sizeof name, 0);
    auto put16 = [&](u32 at, u16 value) { std::memcpy(chunk.data() + at, &value, 2); };
    auto put32 = [&](u32 at, u32 value) { std::memcpy(chunk.data() + at, &value, 4); };
    put32(0, 8);
    put16(4, u16(kSprites));
    put16(6, u16(kScripts));
    put16(10, 512);
    put16(12, 512);
    put16(14, 5);
    put32(16, name_at);
    chunk[32] = 0;  // hasData=0；name "@" → Blank
    put32(36, 0);
    for (u32 index = 0; index < kSprites; index++) put32(64 + index * 4, sprite_offsets[index]);
    for (u32 index = 0; index < kScripts; index++) {
        put32(64 + kSprites * 4 + index * 8 + 0, index);
        put32(64 + kSprites * 4 + index * 8 + 4, script_offsets[index]);
    }
    std::memcpy(chunk.data() + table_end, body.data(), body.size());
    std::memcpy(chunk.data() + name_at, name, sizeof name);
    return chunk;
}

struct FakeSink final : DialogueTextureSink {
    TextureImage image;
    u32 texture_calls = 0, uploads = 0;
    std::array<u32, 4> last_region{};
    FakeSink() {
        image.width = 512;
        image.height = 512;
        image.pitch = image.width * 2;
        image.packed_format = 5;
        image.format = touhou::graphics::PixelFormat::Argb4444;
        image.pixels.assign(size_t(image.pitch) * image.height, 0);
    }
    u32 texture(const AnmResource&, u32 index) override {
        texture_calls++;
        return index == 0 ? 7u : 0u;
    }
    TextureImage* pixels(u32 handle) override { return handle == 7 ? &image : nullptr; }
    void changed(u32 handle, const std::array<u32, 4>& region) override {
        if (handle == 7) {
            uploads++;
            last_region = region;
        }
    }
};

bool region_has_pixel(const TextureImage& image, u32 x, u32 y, u32 width, u32 height) {
    for (u32 row = 0; row < height; row++)
        for (u32 column = 0; column < width; column++) {
            u16 value = 0;
            std::memcpy(&value, image.pixels.data() + size_t(y + row) * image.pitch +
                                   size_t(x + column) * 2, 2);
            if (value) return true;
        }
    return false;
}

AnmVm* script_vm(AnmManager& manager, i32 script) {
    for (u32 index = 0; index < manager.registry.ordered_count(false); index++) {
        if (AnmVm* vm = manager.registry.find(manager.registry.ordered_handle(false, index)))
            if (vm->source_script == script) return vm;
    }
    return nullptr;
}

std::vector<u8> read_reference_anm(const char* name) {
    std::vector<std::string> candidates;
    if (const char* dir = std::getenv("TH13_RAW_DIR"))
        candidates.push_back(std::string(dir) + "/" + name);
    candidates.push_back(std::string("../artifacts/reference/raw/") + name);
    candidates.push_back(std::string("../../../artifacts/reference/raw/") + name);
    std::vector<u8> bytes;
    for (const auto& path : candidates) {
        FILE* file = std::fopen(path.c_str(), "rb");
        if (!file) continue;
        u8 buffer[8192];
        size_t got = 0;
        while ((got = std::fread(buffer, 1, sizeof buffer, file)) > 0)
            bytes.insert(bytes.end(), buffer, buffer + got);
        std::fclose(file);
        break;
    }
    return bytes;
}

bool raster_block(TextureImage& image, i32 x, i32 y, bool, u32, const std::string&) {
    for (i32 row = 0; row < 4; row++)
        for (i32 column = 0; column < 4; column++) {
            const u16 value = 0xFFFF;
            std::memcpy(image.pixels.data() + size_t(y + row) * image.pitch +
                            size_t(x + column) * 2, &value, 2);
        }
    return true;
}

}  // namespace

TEST(dialogue_text_submits_body_strip_and_owns_four_vms) {
    GameRandom random;
    AnmEnvironment environment;
    AnmManager manager{random, environment};
    const auto bytes = synthetic_text_anm();
    CHECK(manager.load(0, bytes.data(), u32(bytes.size())));

    FakeSink sink;
    DialogueText text;
    MsgLine line;
    line.text_cp932 = "\x82\xb1\x82\xf1\x82\xc9\x82\xbf\x82\xcd";
    line.block = 1;
    std::string error;
    CHECK(text.submit(manager, sink, 0, line, 100.f, 200.f, raster_block, error));
    CHECK_EQ(error, std::string());
    CHECK_EQ(manager.registry.count(), 4u);  // 0x4282b0 四文本 VM
    CHECK_EQ(sink.texture_calls, 1u);
    CHECK(region_has_pixel(sink.image, 0, 1, 512, 20));
    CHECK(!region_has_pixel(sink.image, 0, 24, 512, 20));
    CHECK_EQ(sink.last_region[1], 47u);  // ruby slot0 条带清空也提交 changed
    // script 以 ins_4 隐藏等待；0x428ae0 在写入后发 label2 deferred 中断。
    AnmVm* row0 = script_vm(manager, 0);
    CHECK(row0 != nullptr);
    CHECK(!row0->visible);  // 中断尚未被 ANM update 消费
    CHECK(manager.update());
    CHECK(row0->visible);   // label2 显示分支
    CHECK_EQ(row0->visual.flags & 1u, 1u);

    // 同行同块重复提交不得重传（原版 opcode 17 只在指令执行时栅格化）。
    const u32 uploads = sink.uploads;
    CHECK(text.submit(manager, sink, 0, line, 100.f, 200.f, raster_block, error));
    CHECK_EQ(sink.uploads, uploads);

    // 新行块必须清空旧条带并重新写入。
    MsgLine next = line;
    next.block = 2;
    CHECK(text.submit(manager, sink, 0, next, 100.f, 200.f, raster_block, error));
    CHECK(sink.uploads > uploads);

    MsgLine second = line;
    second.block = 2;
    second.text_cp932 = "\x82\xbb\x82\xa4";
    CHECK(text.submit(manager, sink, 1, second, 100.f, 200.f, raster_block, error));
    CHECK(region_has_pixel(sink.image, 0, 24, 512, 20));
    AnmVm* row1 = script_vm(manager, 1);
    CHECK(row1 != nullptr);
    CHECK(!row1->visible);  // label2 deferred，未 tick 前仍隐藏
    CHECK(manager.update());
    CHECK(row1->visible);
    CHECK(row0->visible);   // 已显示行不被无关行提交隐藏

    // opcode 6/18 的 clear/fade 只发 label，不清 texture0 像素。
    const u32 uploads_before_fade = sink.uploads;
    const auto pixels_before_fade = sink.image.pixels;
    CHECK(text.clear(sink, error));
    CHECK_EQ(sink.uploads, uploads_before_fade);
    CHECK(sink.image.pixels == pixels_before_fade);
    CHECK(region_has_pixel(sink.image, 0, 1, 512, 20));
    CHECK(region_has_pixel(sink.image, 0, 24, 512, 20));
    // cached 保留：RunScene 继续提交同一行不得重新上传/重发 label2。
    CHECK(text.submit(manager, sink, 0, next, 100.f, 200.f, raster_block, error));
    CHECK_EQ(sink.uploads, uploads_before_fade);

    text.release(manager);
    manager.update(false);
    CHECK_EQ(manager.registry.count(), 0u);
}

TEST(dialogue_text_ruby_uses_second_parameter_as_spacing) {
    // 0x428ae0:464/529：ruby 第一数 = x，第二数 = 逐字步进。
    // 真实 st01a `|4,14,ここ` → scratch x=4*2，第二字 x=8+14*2。
    GameRandom random;
    AnmEnvironment environment;
    AnmManager manager{random, environment};
    const auto bytes = synthetic_text_anm();
    CHECK(manager.load(0, bytes.data(), u32(bytes.size())));
    FakeSink sink;
    DialogueText text;
    std::string error;
    struct Call { i32 x; bool ruby; std::string text; };
    std::vector<Call> calls;
    const auto record = [&calls](TextureImage& image, i32 x, i32 y, bool ruby, u32,
                                 const std::string& value) {
        calls.push_back({x, ruby, value});
        for (i32 row = 0; row < 4; row++)
            for (i32 column = 0; column < 4; column++) {
                const u16 pixel = 0xFFFF;
                std::memcpy(image.pixels.data() + size_t(y + row) * image.pitch +
                                size_t(x + column) * 2, &pixel, 2);
            }
        return true;
    };
    MsgLine line;
    line.text_cp932 = "\x82\xb1\x82\xf1\x82\xc9\x82\xbf\x82\xcd";
    line.block = 1;
    line.has_ruby = true;
    line.ruby_x = 4;
    line.ruby_spacing = 14;
    line.ruby_cp932 = "\x82\xb1\x82\xb1";  // ここ
    CHECK(text.submit(manager, sink, 0, line, 100.f, 200.f, record, error));
    CHECK_EQ(calls.size(), 3u);
    CHECK(!calls[0].ruby);           // 正文
    CHECK_EQ(calls[0].x, 0);
    CHECK(calls[1].ruby);            // ruby 第一字
    CHECK_EQ(calls[1].x, 8);
    CHECK_EQ(calls[1].text, std::string("\x82\xb1", 2));
    CHECK(calls[2].ruby);            // ruby 第二字 = 第一字 + 14*2
    CHECK_EQ(calls[2].x, 8 + 14 * 2);
    CHECK_EQ(calls[2].text, std::string("\x82\xb1", 2));
    CHECK(region_has_pixel(sink.image, 4, 48, 2, 2));   // 8/2
    CHECK(region_has_pixel(sink.image, 18, 48, 2, 2));  // 36/2
    CHECK(!region_has_pixel(sink.image, 7, 48, 10, 2)); // 两字间无像素

    // spacing=0：0x45aef0 param_9==0 走连续排版，整串只调一次。
    calls.clear();
    MsgLine continuous = line;
    continuous.block = 2;
    continuous.ruby_spacing = 0;
    CHECK(text.submit(manager, sink, 0, continuous, 100.f, 200.f, record, error));
    CHECK_EQ(calls.size(), 2u);
    CHECK_EQ(calls[1].x, 8);
    CHECK(calls[1].ruby);
    CHECK_EQ(calls[1].text, line.ruby_cp932);
}

TEST(dialogue_text_real_text_anm_label2_visibility) {
    // 真实 text.anm（资产缺席时跳过）：script 以 ins_4 隐藏等待，原版
    // 0x46fc30(handle,2) 后才显示。这里验证 port 的 deferred label 路径。
    const std::vector<u8> bytes = read_reference_anm("text.anm");
    if (bytes.empty()) {
        std::printf("# 跳过：text.anm 不在本机（相对 repo 根）\n");
        return;
    }
    GameRandom random;
    AnmEnvironment environment;
    AnmManager manager{random, environment};
    CHECK(manager.load(0, bytes.data(), u32(bytes.size())));
    FakeSink sink;
    DialogueText text;
    MsgLine line;
    // st01a.msg entry0 首句（さーて、幽霊の親玉を倒さないとね！）。
    line.text_cp932 = "\x82\xb3\x81\x5b\x82\xc4\x81\x41\x97\x48\x97\xec\x82\xcc"
                      "\x90\x65\x8b\xca\x82\xf0\x93\x7c\x82\xb3\x82\xc8\x82\xa2"
                      "\x82\xc6\x82\xcb\x81\x49";
    line.block = 1;
    std::string error;
    CHECK(text.submit(manager, sink, 0, line, 112.f, 288.f, raster_block, error));
    AnmVm* row0 = script_vm(manager, 0);
    CHECK(row0 != nullptr);
    CHECK(!row0->visible);  // ins_4 隐藏等待
    for (int frame = 0; frame < 10; frame++) CHECK(manager.update());
    CHECK(row0->visible);
    CHECK(region_has_pixel(sink.image, 0, 1, 512, 20));
    text.release(manager);
    manager.update(false);
    CHECK_EQ(manager.registry.count(), 0u);
}

TEST(dialogue_text_real_front_bubble_and_side_position) {
    // 0x428ae0:485-553：front.anm script224+param（param=side+2*variant）
    // 创建对话框；child 176+param/208+param 的 variables.floats[0]=宽度。
    // 正文 x：左=box_x，右=box_x-(width+30)；文本 VM 创建位 0。
    const std::vector<u8> text_bytes = read_reference_anm("text.anm");
    const std::vector<u8> front_bytes = read_reference_anm("front.anm");
    if (text_bytes.empty() || front_bytes.empty()) {
        std::printf("# 跳过：text.anm/front.anm 不在本机（相对 repo 根）\n");
        return;
    }
    GameRandom random;
    AnmEnvironment environment;
    AnmManager manager{random, environment};
    CHECK(manager.load(0, text_bytes.data(), u32(text_bytes.size())));
    CHECK(manager.load(3, front_bytes.data(), u32(front_bytes.size())));
    FakeSink sink;
    DialogueText text;
    text.bind_bubble_bank(3);
    std::string error;
    MsgLine line;
    line.text_cp932 = "\x82\xb1\x82\xf1\x82\xc9\x82\xbf\x82\xcd";
    line.block = 1;
    line.bubble_width = 244.0f;
    line.side = 0;
    CHECK(text.submit(manager, sink, 0, line, 112.f, 288.f, raster_block, error));
    CHECK(text.bubble_handle() != 0);
    AnmVm* root = script_vm(manager, 224);  // 左 variant 0 根
    CHECK(root != nullptr);
    AnmVm* vm0 = script_vm(manager, 0);
    CHECK(vm0 != nullptr);
    CHECK_EQ(vm0->visual.translation.x, 112.f);  // 左侧文字 x=box_x
    CHECK_EQ(vm0->visual.translation.y, 288.f);
    bool left_cap = false, right_cap = false;
    for (AnmVm* child : manager.registry.children(*root)) {
        if (child->source_script == 176) {
            left_cap = true;
            CHECK_EQ(child->variables.floats[0], 244.0f);
        }
        if (child->source_script == 208) {
            right_cap = true;
            CHECK_EQ(child->variables.floats[0], 244.0f);
        }
    }
    CHECK(left_cap);
    CHECK(right_cap);
    MsgLine second_line = line;
    CHECK(text.submit(manager, sink, 1, second_line, 112.f, 288.f, raster_block, error));
    const auto* double_root = manager.registry.find(text.bubble_handle());
    CHECK(double_root);
    CHECK_EQ(double_root->source_script, 232);

    // opcode 29 的 bit2-5 = variant：text_attr=4 → param=2 → 根 script226。
    MsgLine variant = line;
    variant.block = 2;
    variant.text_attr = 4;
    variant.bubble_width = 150.0f;
    CHECK(text.submit(manager, sink, 0, variant, 112.f, 288.f, raster_block, error));
    CHECK(script_vm(manager, 226) != nullptr);

    // 右说话人：variant 同一行块，side=1 → 根 225，文字右对齐。
    MsgLine right = line;
    right.block = 3;
    right.side = 1;
    right.bubble_width = 200.0f;
    CHECK(text.submit(manager, sink, 0, right, 336.f, 272.f, raster_block, error));
    AnmVm* right_root = script_vm(manager, 225);
    CHECK(right_root != nullptr);
    CHECK_EQ(vm0->visual.translation.x, 336.f - (200.0f + 30.0f));
    CHECK_EQ(vm0->visual.translation.y, 272.f);

    CHECK(text.clear(sink, error));
    CHECK_EQ(text.bubble_handle(), 0u);
    text.release(manager);
    manager.update(false);
    CHECK_EQ(manager.registry.count(), 0u);
}

TEST(dialogue_text_negative_controls) {
    GameRandom random;
    AnmEnvironment environment;
    AnmManager manager{random, environment};
    FakeSink sink;
    DialogueText text;
    MsgLine line;
    line.text_cp932 = "\x82\xb1";
    std::string error;

    // 缺失 text.anm bank 0：拒绝，不建 VM。
    CHECK(!text.submit(manager, sink, 0, line, 0.f, 0.f, raster_block, error));
    CHECK(!error.empty());
    CHECK_EQ(manager.registry.count(), 0u);

    const auto bytes = synthetic_text_anm();
    CHECK(manager.load(0, bytes.data(), u32(bytes.size())));
    // 越界行：拒绝。
    error.clear();
    CHECK(!text.submit(manager, sink, 2, line, 0.f, 0.f, raster_block, error));
    CHECK(!error.empty());

    // texture0 非 ARGB4444：拒绝在写入前失败。
    FakeSink wrong;
    wrong.image.format = touhou::graphics::PixelFormat::Bgra8;
    wrong.image.pitch = wrong.image.width * 4;
    wrong.image.pixels.assign(size_t(wrong.image.pitch) * wrong.image.height, 0);
    error.clear();
    CHECK(!text.submit(manager, wrong, 0, line, 0.f, 0.f, raster_block, error));
    CHECK(!error.empty());
}

int main() { return th13test::run_all("text.anm 对话动态纹理"); }
