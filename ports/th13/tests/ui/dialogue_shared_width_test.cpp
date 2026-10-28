#include "../ecl/ecl_test_fixtures.hpp"
#include "game/DialogueText.hpp"

using namespace th13;
namespace {
struct Sink final : DialogueTextureSink {
    TextureImage image;
    Sink() {
        image.width=image.height=512;
        image.pitch=1024;
        image.format=touhou::graphics::PixelFormat::Argb4444;
        image.packed_format=5;
        image.pixels.resize(512*1024);
    }
    u32 texture(const AnmResource&,u32) override { return 1; }
    TextureImage* pixels(u32) override { return &image; }
    void changed(u32,const std::array<u32,4>&) override {}
};
AnmVm* row(AnmManager& manager,i32 script) {
    for (u32 i=0;i<manager.registry.ordered_count(false);++i)
        if (auto* vm=manager.registry.find(manager.registry.ordered_handle(false,i)))
            if (vm->source_script==script) return vm;
    return nullptr;
}
TEST(second_right_row_repositions_both_rows_with_shared_block_width) {
    th13test::TestFileReader reader;
    std::vector<u8> bytes;
    CHECK(reader.read("artifacts/reference/raw/text.anm",bytes));
    GameRandom random;
    AnmEnvironment environment;
    AnmManager manager(random,environment);
    CHECK(manager.load(0,bytes.data(),u32(bytes.size())));
    DialogueText text;
    text.bind_bubble_bank(-1);
    Sink sink;
    std::string error;
    const DialogueRasterizer raster=[](TextureImage&,i32,i32,bool,u32,const std::string&){return true;};
    MsgLine first;
    first.block=1;first.side=1;first.text_cp932="first";first.bubble_width=84;
    CHECK(text.submit(manager,sink,0,first,336,272,raster,error));
    auto* a=row(manager,0);
    CHECK(a);if(!a)return;
    CHECK_EQ(a->visual.translation.x,222.f);
    MsgLine second=first;
    second.text_cp932="second-longer";second.bubble_width=148;
    CHECK(text.submit(manager,sink,1,second,336,272,raster,error));
    auto* b=row(manager,1);
    CHECK(b);if(!b)return;
    // 00428ae0 second-line branch writes the shared+0x1a0 width,
    // then00470160 updates both text-line translations before label2.
    CHECK_EQ(a->visual.translation.x,158.f);
    CHECK_EQ(b->visual.translation.x,158.f);
    CHECK_EQ(a->visual.translation.y,272.f);
    CHECK_EQ(b->visual.translation.y,272.f);
    CHECK(text.submit(manager,sink,0,first,336,272,raster,error));
    CHECK_EQ(a->visual.translation.x,158.f);
    CHECK(manager.update());
    CHECK_EQ(a->visual.scale.x,.234375f);
    CHECK_EQ(b->visual.scale.x,.234375f);
    CHECK_EQ(a->visual.color>>24,31u);
    CHECK_EQ(b->visual.color>>24,31u);
}
}
int main(){return th13test::run_all("dialogue-shared-width");}
