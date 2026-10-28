// 符卡宣言名（text.anm script74 / sprite9）栅格落点窄测试。
// 依据：0046ea30.c L26-27 中心式 x = W/2 - (nominal-1)*len/4；原版
// original-spell-ms f0008 墨列 294..418（320/662 挤压后纹理 ~90.5 起）。
// sprite9 = (0,92,320,17)（text.anm entry0 实测），nominal 17。
#include "../harness/test_framework.hpp"
#include "game/AnmText.hpp"

using namespace th13;
namespace {
struct Sink : DialogueTextureSink {
    TextureImage image;
    std::vector<std::array<u32, 4>> changes;
    Sink() {
        image.width = image.height = 512;
        image.pitch = 1024;
        image.format = touhou::graphics::PixelFormat::Argb4444;
        image.pixels.assign(size_t(image.pitch) * image.height, 0);
    }
    u32 texture(const AnmResource&, u32) override { return 1; }
    TextureImage* pixels(u32 handle) override { return handle == 1 ? &image : nullptr; }
    void changed(u32, const std::array<u32, 4>& region) override { changes.push_back(region); }
};
struct Fixture {
    AnmResource resource;
    AnmSprite sprite;
    AnmVm vm;
    Sink sink;
    Fixture() {
        sprite.x = 0; sprite.y = 92; sprite.width = 320; sprite.height = 17;
        vm.resource = &resource; vm.sprite = &sprite;
    }
    // 8/6 个全角字符的替身栅格：32 scratch 像素步进、每字符 12 像素白色竖条
    // （模拟 MS Gothic 32px 固定步进），返回回调收到的 x。
    int fill(i32 chars) {
        int drawn = -1;
        CHECK(rasterize_anm_text(vm, sink,
            [&](TextureImage& image, i32 x, i32 y, u32, const std::string&) {
                drawn = x;
                for (i32 c = 0; c < chars; ++c)
                    for (i32 yy = y; yy < y + 30; ++yy)
                        for (i32 dx = 0; dx < 12; ++dx) {
                            const i32 px = x + c * 32 + 10 + dx;
                            if (px < 0 || px >= int(image.width)) continue;
                            const u16 value = 0xf000;
                            std::memcpy(image.pixels.data() + size_t(yy) * image.pitch +
                                            size_t(px) * 2, &value, 2);
                        }
                return true;
            }, 0xffffffu, std::string(size_t(chars) * 2, '\x81'), AnmTextAlignment::Center, 17));
        return drawn;
    }
    int ink_start() const {
        for (u32 px = 0; px < 512; ++px) {
            u16 value;
            std::memcpy(&value, image_row(px), 2);
            if (value & 0x0fff) return int(px);
        }
        return -1;
    }
    const u8* image_row(u32 px) const {
        return sink.image.pixels.data() + size_t(100) * sink.image.pitch + size_t(px) * 2;
    }
};
}  // namespace

// 16 字节名（符牒「死蝶の舞」形）：中心式 = 320/2 - 16*16/4 = 96（scratch 192），
// 320/662 挤压后墨列应落在纹理 ~92.8（原版实测 90.5±2.5）。
TEST(spell_name_center_lands_on_original_ink_columns) {
    Fixture fixture;
    const int drawn = fixture.fill(8);
    CHECK_EQ(drawn, 192);
    const int start = fixture.ink_start();
    CHECK(start >= 88 && start <= 97);
    CHECK_EQ(fixture.sink.changes.size(), size_t(1));
    CHECK((fixture.sink.changes[0] == std::array<u32, 4>{0, 92, 320, 109}));
}

// 12 字节名：中心式 = 160 - 16*12/4 = 112（scratch 224）；右对齐旧读法
// （W-(nominal-1)*len/2 = 224，scratch 448）会把墨列推到纹理 ~216（原版排除）。
TEST(spell_name_center_scales_with_cp932_byte_count) {
    Fixture fixture;
    CHECK_EQ(fixture.fill(6), 224);
    const int start = fixture.ink_start();
    CHECK(start >= 105 && start <= 114);
}

int main() { return th13test::run_all("spell_name_alignment_test"); }
