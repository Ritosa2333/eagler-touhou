// Original 00461a60 selects sprites from the owning ANM file; 0046f990 /
// 0046fab0 likewise resolve child scripts in that file. Packing files into
// one web bank must preserve those domains, including portrait children.
#include "anm_test_fixtures.hpp"
#include "game/AnmRenderer.hpp"

using namespace th13;

namespace {
struct TextureGraphics final : ZunGraphics {
    touhou::graphics::PipelineState state;
    std::string selected;
    u32 triangles_drawn=0;
    touhou::graphics::PipelineState& pipeline()override{return state;}
    u32 texture(const AnmResource& file,u32 index)override {
        selected=file.textures[index].name;return index+1;
    }
    void bind_texture(u32)override{}
    void set_layout(touhou::graphics::VertexLayout)override{}
    void set_matrix(touhou::graphics::MatrixKind,const Matrix4&)override{}
    void triangles(u32 count,const void*,u32)override{triangles_drawn+=count;}
    void primitives(touhou::graphics::Topology,u32,const void*,u32)override{}
    bool select_target(const AnmResource*,u32)override{return true;}
    bool clear_target(u32,const GraphicsViewport*)override{return true;}
    bool clear_depth(const GraphicsViewport*)override{return true;}
    void set_viewport(const GraphicsViewport&)override{}
};

void compare_tree(AnmManager& packed, AnmVm& actual,
                  AnmManager& separate, AnmVm& expected, i32 script_base) {
    CHECK_EQ(actual.source_script, expected.source_script + script_base);
    CHECK_EQ(actual.visible, expected.visible);
    CHECK_EQ(actual.visual.color, expected.visual.color);
    CHECK_EQ(actual.sprite != nullptr, expected.sprite != nullptr);
    if (actual.sprite && expected.sprite) {
        CHECK_EQ(actual.visual.sprite, expected.visual.sprite);
        CHECK_EQ(actual.resource->textures[actual.sprite->texture].name,
                 expected.resource->textures[expected.sprite->texture].name);
        CHECK_EQ(actual.sprite->source_id, expected.sprite->source_id);
        CHECK_EQ(actual.visual.sprite_size.x, expected.visual.sprite_size.x);
        CHECK_EQ(actual.visual.sprite_size.y, expected.visual.sprite_size.y);
        CHECK_EQ(actual.visual.uv[0].x, expected.visual.uv[0].x);
        CHECK_EQ(actual.visual.uv[0].y, expected.visual.uv[0].y);
        // Check the material that production rendering actually submits,
        // not just the pointer used by the animation interpreter.
        TextureGraphics ag, eg;
        AnmRenderer ar{ag}, er{eg};
        CHECK_EQ(ar.draw(actual), er.draw(expected));
        ar.flush(); er.flush();
        CHECK_EQ(ag.selected, eg.selected);
        CHECK_EQ(ag.triangles_drawn, eg.triangles_drawn);
    }
    const auto children = packed.registry.children(actual);
    const auto reference = separate.registry.children(expected);
    CHECK_EQ(children.size(), reference.size());
    for (size_t i = 0; i < children.size() && i < reference.size(); ++i)
        compare_tree(packed, *children[i], separate, *reference[i], script_base);
}

void stage1_script(i32 script, i32 label) {
    th13test::TestFileReader reader;
    std::vector<u8> enemy, boss;
    CHECK(reader.read("artifacts/reference/raw/enemy.anm", enemy));
    CHECK(reader.read("artifacts/reference/raw/st01enm.anm", boss));
    AnmResource resource;
    CHECK(resource.open(enemy.data(), u32(enemy.size())));
    const i32 base = i32(resource.scripts.size());
    CHECK(resource.append(boss.data(), u32(boss.size())));
    GameRandom rng1{0}, rng2{0};
    AnmEnvironment env1, env2;
    AnmManager packed{rng1, env1}, separate{rng2, env2};
    CHECK(packed.adopt(1, std::move(resource)));
    CHECK(separate.load(1, boss.data(), u32(boss.size())));
    rng1.reseed(0); rng2.reseed(0);
    const u32 a = packed.create(1, base + script);
    const u32 b = separate.create(1, script);
    CHECK(a && b);
    if (!a || !b) return;
    if (label) {
        CHECK(packed.interrupt(a, label, true));
        CHECK(separate.interrupt(b, label, true));
    }
    for (int frame = 0; frame < 45; ++frame) {
        CHECK(packed.update());
        CHECK(separate.update());
    }
    auto* actual = packed.registry.find(a);
    auto* expected = separate.registry.find(b);
    CHECK(actual && expected);
    if (actual && expected) compare_tree(packed, *actual, separate, *expected, base);
}

TEST(yuyuko_body_uses_her_archive) { stage1_script(0, 0); }
TEST(yuyuko_portrait_children_use_her_archive) { stage1_script(10, 17); }
TEST(yuyuko_nameplate_uses_her_archive) { stage1_script(15, 0); }
TEST(yuyuko_spell_decoration_uses_her_archive) { stage1_script(18, 0); }

TEST(merged_detached_children_and_fallback_keep_file_identity) {
    std::vector<u8> idle;
    th13test::push_insn(idle, 300, 0, 0, {0});
    th13test::push_insn(idle, 2, 0);
    th13test::push_terminator(idle);
    const auto bytes=th13test::build_simple_anm({idle});
    AnmResource packed;
    for(int file=0;file<3;++file) CHECK(packed.append(bytes.data(),u32(bytes.size())));
    GameRandom rng{0};AnmEnvironment env;AnmManager manager{rng,env};
    CHECK(manager.adopt(1,std::move(packed)));
    const auto h=manager.create(1,2);
    auto* root=manager.registry.find(h);CHECK(root);if(!root)return;
    auto* child=manager.spawn_detached(*root,0);CHECK(child);if(!child)return;
    CHECK_EQ(child->source_script,2);
    CHECK_EQ(child->visual.sprite,0);
    CHECK_EQ(child->sprite->texture,2u);
    CHECK_EQ(child->texture_sprite(),child->sprite);
    auto* attached=manager.spawn_child(*root,0,0);CHECK(attached);
    CHECK(manager.registry.find_child_script(*root,0,0)==attached);
    AnmResource fallback;fallback.sprites.resize(302);fallback.textures.resize(1);
    fallback.textures[0].width=fallback.textures[0].height=32;
    env.fallback_sprite_resource=&fallback;
    CHECK(child->select_sprite(-1));
    CHECK(child->texture_sprite()==&fallback.sprites[301]);
    CHECK_EQ(child->visual.sprite,301);
}
}

int main() { return th13test::run_all("anm-merged-archive"); }
