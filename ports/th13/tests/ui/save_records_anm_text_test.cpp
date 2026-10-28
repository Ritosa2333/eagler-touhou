#include "../harness/test_framework.hpp"
#include "game/AnmText.hpp"

using namespace th13;
namespace {
struct Sink : DialogueTextureSink {
    TextureImage image;
    std::vector<std::array<u32,4>> changes;
    Sink() {
        image.width = image.height = 512;
        image.pitch = 1024;
        image.format = touhou::graphics::PixelFormat::Argb4444;
        image.pixels.resize(size_t(image.pitch) * image.height);
    }
    u32 texture(const AnmResource&,u32 index) override { CHECK_EQ(index,0u); return 1; }
    TextureImage* pixels(u32 handle) override { return handle == 1 ? &image : nullptr; }
    void changed(u32, const std::array<u32,4>& region) override { changes.push_back(region); }
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
};
}

TEST(anm_text_original_left_center_right_offsets_use_cp932_bytes) {
    Fixture fixture;
    const std::string text = "\x81\x48\x81\x48" "ABCD"; // eight original bytes
    for (const auto& entry : {std::pair<AnmTextAlignment,i32>{AnmTextAlignment::Left,0},
                              {AnmTextAlignment::Center,256}, {AnmTextAlignment::Right,512}}) {
        bool called = false;
        CHECK(rasterize_anm_text(fixture.vm,fixture.sink,
            [&](TextureImage&,i32 x,i32 y,u32 color,const std::string& bytes) {
                called = true; CHECK_EQ(x,entry.second); CHECK_EQ(y,2);
                CHECK_EQ(color,0xffffffffu); CHECK(bytes == text); return true;
            },0xefefef,text,entry.first));
        CHECK(called);
    }
    CHECK_EQ(fixture.sink.changes.size(),size_t(3));
    CHECK((fixture.sink.changes[0] == std::array<u32,4>{0,92,320,109}));
}

TEST(anm_text_colorref_keeps_requested_channels_and_publishes_real_pixels) {
    Fixture fixture;
    CHECK(rasterize_anm_text(fixture.vm,fixture.sink,
        [](TextureImage& image,i32,i32,u32,const std::string&) {
            for (u32 y = 6; y < 30; ++y) for (u32 x = 8; x < 64; ++x) {
                const u16 value = 0xffff;
                std::memcpy(image.pixels.data()+size_t(y)*image.pitch+x*2,&value,2);
            }
            return true;
        },0x000000ff,"red",AnmTextAlignment::Left));
    u32 red = 0;
    for (u32 y = 92; y < 109; ++y) for (u32 x = 0; x < 320; ++x) {
        u16 value;
        std::memcpy(&value,fixture.sink.image.pixels.data()+size_t(y)*1024+x*2,2);
        CHECK_EQ(value & 0x00ffu,0u);
        if (value & 0x0f00u) ++red;
    }
    CHECK(red > 0);
}

TEST(anm_text_unknown_nominal_font_or_invalid_sprite_fails_without_upload) {
    Fixture fixture;
    const auto raster=[](TextureImage&,i32,i32,u32,const std::string&) { return true; };
    std::string error;
    CHECK(!rasterize_anm_text(fixture.vm,fixture.sink,raster,0xffffff,"text",
                               AnmTextAlignment::Left,20,&error));
    CHECK(!error.empty()); CHECK(fixture.sink.changes.empty());
    fixture.sprite.x = 400;
    CHECK(!rasterize_anm_text(fixture.vm,fixture.sink,raster,0xffffff,"text"));
    CHECK(fixture.sink.changes.empty());
}

int main() { return th13test::run_all("save_records_anm_text_test"); }
