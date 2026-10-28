// AnmResource 单测：
// 1) golden：真实 front.anm entry0 数值表逐字段断言（计划 §二 表）；
// 2) 指令流解码：script0 前 7 条 + 终止符 + script1 起点闭环；
// 3) 跨 entry 全局编号：153 精灵/244 脚本按 entry 表序追加（风险表 #5）；
// 4) text.anm 的 @/@R/外部名 Kind 判定；effect.anm rgba() 尺寸闭环；
// 5) 合成归档解析 + anm_decode_png（stb_image）。
#include "anm_test_fixtures.hpp"
#include "game/AnmTextureDecode.hpp"

#include <cstdio>

using namespace th13;

namespace {

th13test::TestFileReader& reader() {
    static th13test::TestFileReader instance;
    return instance;
}

TEST(front_entry0_golden) {
    // 计划 §二 字段表（raw front.anm 逐字节实测）。
    std::vector<th13::u8> bytes;
    CHECK(reader().read("artifacts/reference/raw/front.anm", bytes));
    th13::AnmResource resource;
    CHECK(resource.open(bytes.data(), th13::u32(bytes.size())));
    CHECK_EQ(resource.textures.size(), 11u);  // 11 个 entry
    const auto& texture = resource.textures[0];
    CHECK_EQ(texture.width, 512);
    CHECK_EQ(texture.height, 512);
    CHECK_EQ(texture.format, 1);
    CHECK_EQ(texture.name, std::string("front/front00.png"));
    CHECK_EQ(texture.source_x, 0);
    CHECK_EQ(texture.source_y, 0);
    CHECK_EQ(texture.priority, 10u);
    CHECK(texture.kind == th13::AnmTexture::Kind::Embedded);
    CHECK(!texture.low_resolution_scale);  // lowResScale 全 0
    CHECK_EQ(texture.pixel_format, 1);
    CHECK_EQ(texture.pixel_width, 512);
    CHECK_EQ(texture.pixel_height, 512);
    // THTX 像素量 0x100000 = 512*512*4，nextentry 0x108C18 = 0x8C08+16+0x100000 闭环。
    CHECK_EQ(texture.pixels.size(), 0x100000u);
    // entry0 sprite 记录：82 个（首项见偏移表 0x528）。
    CHECK_EQ(resource.sprites.size(), 153u);  // 全 11 entry 合计
    CHECK_EQ(resource.sprites[0].source_id, 0u);
}

TEST(front_script0_decode) {
    std::vector<th13::u8> bytes;
    CHECK(reader().read("artifacts/reference/raw/front.anm", bytes));
    th13::AnmResource resource;
    CHECK(resource.open(bytes.data(), th13::u32(bytes.size())));
    CHECK_EQ(resource.scripts.size(), 244u);
    const auto& script0 = resource.scripts[0];
    CHECK_EQ(script0.source_id, 0);
    CHECK_EQ(script0.file_offset, 0xB90u);          // 脚本表 {0, 0xB90}
    CHECK_EQ(script0.bytes.size(), 0x160u);         // 0xCF0 - 0xB90
    // 前 7 条（本轮 python 复核）：op438 / op500×6。
    const th13::AnmInstruction* insns[7] = {};
    for (int i = 0; i < 7; i++)
        insns[i] = reinterpret_cast<const th13::AnmInstruction*>(script0.bytes.data() + i * 12);
    const i16 expect_op[7] = {438, 500, 500, 500, 500, 500, 500};
    const i32 expect_arg[7] = {0, 2, 3, 4, 5, 15, 16};
    for (int i = 0; i < 7; i++) {
        CHECK_EQ(th13::u32(insns[i]->opcode), th13::u32(expect_op[i]));
        CHECK_EQ(insns[i]->length, 12);
        CHECK_EQ(th13::u32(insns[i]->time), 0u);
        CHECK_EQ(th13::u32(insns[i]->references), 0u);
        CHECK_EQ(insns[i]->argument<th13::i32>(0), expect_arg[i]);
    }
    // 终止符：script0 尾 8 字节 opcode=-1 size=0；script1 起点恰在 0xCF0 闭环。
    th13::i16 opcode;
    std::memcpy(&opcode, script0.bytes.data() + script0.bytes.size() - 8, 2);
    CHECK_EQ(opcode, th13::i16(-1));
    CHECK_EQ(resource.scripts[1].source_id, 1);
    CHECK_EQ(resource.scripts[1].file_offset, 0xCF0u);
}

TEST(front_global_numbering) {
    // 风险表 #5：ins_300 的编号是归档内全局 sprite 序号，解析必须按 entry 表序追加。
    std::vector<th13::u8> bytes;
    CHECK(reader().read("artifacts/reference/raw/front.anm", bytes));
    th13::AnmResource resource;
    CHECK(resource.open(bytes.data(), th13::u32(bytes.size())));
    const th13::u16 sprites_per_entry[11] = {82, 2, 2, 14, 5, 16, 2, 3, 8, 16, 3};
    const th13::u16 scripts_per_entry[11] = {112, 3, 2, 32, 6, 16, 0, 5, 16, 48, 4};
    th13::u32 script_base = 0, sprite_base = 0;
    for (u32 e = 0; e < 11; e++) {
        // 脚本 id 只是文件元数据（entry3 起编号跳到 219），全局序号 = 追加位置；
        // 此处校验每 entry 段内 id 连续递增 + 段内偏移单调。
        for (u32 i = 0; i < scripts_per_entry[e]; i++) {
            const auto& entry_script = resource.scripts[script_base + i];
            CHECK_EQ(entry_script.source_id,
                     resource.scripts[script_base].source_id + th13::i32(i));
            if (i)
                CHECK(entry_script.file_offset > resource.scripts[script_base + i - 1].file_offset);
        }
        CHECK(resource.sprites[sprite_base].texture == e);  // entry 边界换纹理
        sprite_base += sprites_per_entry[e];
        script_base += scripts_per_entry[e];
    }
    CHECK_EQ(sprite_base, 153u);   // 全局 sprite 序号 0..152 连续
    CHECK_EQ(script_base, 244u);
}

TEST(text_anm_special_names) {
    // §一：text.anm 含 @R（渲染目标）×2 与 @（空白）；hasData=0 为外部名。
    std::vector<th13::u8> bytes;
    CHECK(reader().read("artifacts/reference/raw/text.anm", bytes));
    th13::AnmResource resource;
    CHECK(resource.open(bytes.data(), th13::u32(bytes.size())));
    u32 render_targets = 0, blanks = 0, external = 0;
    for (const auto& texture : resource.textures) {
        if (texture.name == "@R") {
            ++render_targets;
            CHECK(texture.kind == th13::AnmTexture::Kind::RenderTarget);
        } else if (texture.name == "@") {
            ++blanks;
            CHECK(texture.kind == th13::AnmTexture::Kind::Blank);
        } else if (texture.kind == th13::AnmTexture::Kind::External) {
            ++external;  // hasData=0：外部名，像素由外部供给
        }
    }
    CHECK_EQ(render_targets, 2u);
    CHECK(blanks >= 1u);
    (void)external;
}

TEST(effect_anm_rgba) {
    std::vector<th13::u8> bytes;
    CHECK(reader().read("artifacts/reference/raw/effect.anm", bytes));
    th13::AnmResource resource;
    CHECK(resource.open(bytes.data(), th13::u32(bytes.size())));
    for (const auto& texture : resource.textures) {
        std::vector<th13::u8> rgba;
        CHECK(texture.rgba(rgba));
        CHECK_EQ(rgba.size(), size_t(texture.pixel_width) * texture.pixel_height * 4);
    }
}

TEST(synthetic_archive_and_invalid) {
    std::vector<th13::u8> script;
    th13test::push_insn(script, 402, 0, 0, {0x400000, 0x400000});
    th13test::push_terminator(script);
    const auto bytes = th13test::build_simple_anm({script});
    th13::AnmResource resource;
    CHECK(resource.open(bytes.data(), th13::u32(bytes.size())));
    CHECK_EQ(resource.sprites.size(), 1u);
    CHECK_EQ(resource.scripts.size(), 1u);
    CHECK_EQ(resource.scripts[0].bytes.size(), script.size());
    CHECK_EQ(resource.sprites[0].u1, 1.f);  // 2/2 全覆盖 → u1=1
    // 版本非 8 拒收。
    auto bad = bytes;
    bad[0] = 7;
    CHECK(!resource.open(bad.data(), th13::u32(bad.size())));
    // 截断拒收。
    CHECK(!resource.open(bytes.data(), 40));
}

TEST(png_decode_stb) {
    // thtk 提取产物（8-bit RGBA PNG）经 stb_image 装载：尺寸与字节量闭环。
    std::vector<th13::u8> bytes;
    CHECK(reader().read("artifacts/reference/textures/effect/eff_switch.png", bytes));
    th13::u32 width = 0, height = 0;
    std::vector<th13::u8> rgba;
    CHECK(th13::anm_decode_png(bytes.data(), th13::u32(bytes.size()), width, height, rgba));
    CHECK_EQ(width, 64u);
    CHECK_EQ(height, 64u);
    CHECK_EQ(rgba.size(), 64u * 64u * 4);
    CHECK(!th13::anm_decode_png(nullptr, 0, width, height, rgba));
}

}  // namespace

int main() { return th13test::run_all("anm-resource"); }
