// H-SPELL-NAME-RASTER: explicit synthetic glyph coverage verifies the production
// texture path; it supplies no substitute font and certifies no original glyph.
#include "../harness/test_framework.hpp"
#include "game/SpellPresentation.hpp"

#include <fstream>

using namespace th13;
namespace {
std::vector<u8> asset(const char* name) {
    std::ifstream file(std::string("artifacts/reference/raw/") + name, std::ios::binary);
    return {std::istreambuf_iterator<char>(file), {}};
}
struct TextureSink final : DialogueTextureSink {
    TextureImage image;
    const AnmResource* expected = nullptr;
    std::vector<std::array<u32, 4>> changes;
    bool available = true;
    TextureSink() {
        image.width = image.height = 512;
        image.pitch = 1024;
        image.packed_format = 5;
        image.format = touhou::graphics::PixelFormat::Argb4444;
        image.pixels.resize(size_t(image.pitch) * image.height);
        for (size_t at = 0; at < image.pixels.size(); at += 2) {
            image.pixels[at] = 0x34; image.pixels[at + 1] = 0x12;
        }
    }
    u32 texture(const AnmResource& resource, u32 index) override {
        CHECK(&resource == expected);
        CHECK_EQ(index, 0u);
        return available ? 7 : 0;
    }
    TextureImage* pixels(u32 handle) override { return handle == 7 ? &image : nullptr; }
    void changed(u32 handle, const std::array<u32, 4>& region) override {
        CHECK_EQ(handle, 7u); changes.push_back(region);
    }
    u16 pixel(u32 x, u32 y) const {
        u16 result;
        std::memcpy(&result, image.pixels.data() + size_t(y) * image.pitch + size_t(x) * 2, 2);
        return result;
    }
};
struct Fixture {
    GameRandom random;
    AnmEnvironment environment;
    AnmManager manager{random, environment};
    SpellPresentation presentation{manager};
    SpellState state;
    TextureSink sink;
    Fixture() {
        for (const auto& item : {std::pair<i32, const char*>{0, "text.anm"},
                                 {2, "ascii.anm"}, {8, "effect.anm"}}) {
            const auto bytes = asset(item.second);
            CHECK(!bytes.empty());
            CHECK(manager.load(item.first, bytes.data(), u32(bytes.size())));
            if (item.first == 2) CHECK(manager.sprite_fallback(2));
        }
        AnmResource stage;
        for (const char* name : {"enemy.anm", "st01enm.anm"}) {
            const auto bytes = asset(name);
            CHECK(stage.append(bytes.data(), u32(bytes.size())));
        }
        CHECK(manager.adopt(4, std::move(stage)));
        sink.expected = manager.resource(0);
        const float position[3] = {0, 128, 0};
        CHECK(presentation.consume(state.begin(0, "synthetic CP932", 3000, 1, 0), position, {}));
    }
};
void mask(TextureImage& image, i32 left, i32 right) {
    for (i32 y = 8; y < 28; ++y) for (i32 x = left; x < right; ++x) {
        const u16 value = 0xffff;
        std::memcpy(image.pixels.data() + size_t(y) * image.pitch + size_t(x) * 2, &value, 2);
    }
}
}

TEST(spell_name_raster_writes_real_text74_sprite_region_and_uploads_once) {
    Fixture fixture;
    i32 calls = 0;
    CHECK(fixture.presentation.rasterize_name(fixture.sink,
        [&](TextureImage& image, i32 x, i32 y, u32 color, const std::string& text) {
            ++calls;
            CHECK_EQ(image.width, 1024u); CHECK_EQ(image.height, 40u);
            CHECK_EQ(image.pitch, 2048u);
            // 004135c3 调 0046e970(0xffffff,0,0,名字)，其基址为精灵静态域
            // [rec+0x38]；原版 original-spell-ms f0008 墨列（294..418，320/662
            // 挤压 32px 步进）把有效落点钉在 0046ea30 中心式
            // param_4 = 精灵宽320/2 − (17−1)×len/4，15 字名 = 160−60 = 100；
            // 0045aef0 正文 TextOutA(param_4*2, 2) → 栅格坐标 x=200。
            CHECK_EQ(x, 200); CHECK_EQ(y, 2);
            CHECK_EQ(color, 0xffffffffu);
            CHECK(text == "synthetic CP932");
            mask(image, 8, 60);
            return true;
        }));
    CHECK_EQ(calls, 1);
    CHECK_EQ(fixture.sink.changes.size(), size_t(1));
    CHECK((fixture.sink.changes[0] == std::array<u32,4>{0,92,320,109}));
    u32 foreground = 0, outline = 0;
    for (u32 y = 0; y < 512; ++y) for (u32 x = 0; x < 512; ++x) {
        const u16 value = fixture.sink.pixel(x,y);
        if (y < 92 || y >= 109 || x >= 320) {
            CHECK_EQ(value, u16(0x1234));
        } else if (value >> 12) {
            if ((value & 0x0fff) == 0x0fff) ++foreground;
            if ((value & 0x0fff) < 0x0888) ++outline;
        }
    }
    CHECK(foreground > 0);
    CHECK(outline > 0); // black border coverage survives the ARGB4444 path
    CHECK_EQ(fixture.sink.pixel(319,108), u16(0));
}

TEST(spell_name_replacement_clears_previous_long_name_without_touching_other_text) {
    Fixture fixture;
    CHECK(fixture.presentation.rasterize_name(fixture.sink,
        [](TextureImage& image, i32, i32, u32, const std::string&) {
            mask(image, 8, 180); return true;
        }));
    CHECK(fixture.sink.pixel(60,100) != 0);
    CHECK(fixture.presentation.rasterize_name(fixture.sink,
        [](TextureImage& image, i32, i32, u32, const std::string&) {
            mask(image, 8, 20); return true;
        }));
    CHECK_EQ(fixture.sink.pixel(60,100), u16(0));
    CHECK_EQ(fixture.sink.pixel(0,91), u16(0x1234));
    CHECK_EQ(fixture.sink.changes.size(), size_t(2));
}

TEST(spell_name_font_failure_keeps_existing_texture_and_exposes_failure) {
    Fixture fixture;
    const auto previous = fixture.sink.image.pixels;
    CHECK(!fixture.presentation.rasterize_name(fixture.sink,
        [](TextureImage&, i32, i32, u32, const std::string&) { return false; }));
    CHECK(!fixture.presentation.error.empty());
    CHECK(fixture.sink.image.pixels == previous);
    CHECK(fixture.sink.changes.empty());
}

TEST(spell_name_missing_texture_keeps_font_uninvoked_and_exposes_failure) {
    Fixture fixture;
    fixture.sink.available = false;
    bool called = false;
    CHECK(!fixture.presentation.rasterize_name(fixture.sink,
        [&](TextureImage&, i32, i32, u32, const std::string&) { called = true; return true; }));
    CHECK(!called);
    CHECK(!fixture.presentation.error.empty());
    CHECK(fixture.sink.changes.empty());
}

int main() { return th13test::run_all("spell_records_presentation_name_test"); }
