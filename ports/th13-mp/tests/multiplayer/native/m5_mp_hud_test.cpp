// M5 原生固定 case：多人右侧资源 HUD（每席图标集 + 每席文本行 + 逐席回调）。
// 被测生产面：
//   - GameHud::enable_multiplayer（席 1/2 图标 VM 组、kMpRowPitch 平移、
//     共享得点/Graze 标签下移、非法席数拒绝、幂等）；
//   - GameHud::life_seat/bombs_seat（每席 interrupt 布局、逐席阈值
//     R-A、seat 0 /单席回落单套面且互不串扰）；
//   - GameHud::draw 的 MpHudSeats 分支（每席行标签/碎片/Power 的坐标、
//     颜色（本地席黄、幽灵后缀）、共享行下移；mp==null 时 SP 原位布局）；
//   - ItemManager::grant_resources（R-I3 广播逐席回调）与 on_bomb_key
//     （active_seat 归属）；
//   - GameHud::RollbackState 快照/恢复包含 MP 域。
//
// 固定 case 映射（workflow-cases.json m5.mp_hud_seat_rows）：
//   mp_hud_seat_icon_layout    → 图标集创建/平移/interrupt/阈值/隔离
//   mp_hud_text_rows           → 文本面 MP 布局与 SP 零变化
//   mp_hud_seat_callbacks      → 逐席资源回调（发放/Bomb/定向赠送）
//   mp_hud_rollback_roundtrip  → 快照恢复含 MP 域
#include "../../ui/ui_test_fixtures.hpp"
#include "../../ecl/ecl_test_fixtures.hpp"
#include "../../harness/test_framework.hpp"

#include "game/GameHud.hpp"
#include "game/AsciiText.hpp"
#include "game/AnmCoordinates.hpp"
#include "game/AnmDrawSchedule.hpp"
#include "game/HudDrawData.hpp"
#include "game/FrameScheduler.hpp"
#include "game/StageBattle.hpp"
#include "multiplayer/WorldState.hpp"
#include "multiplayer/Session.hpp"
#include "sdl/ApplicationState.hpp"

#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <string>
#include <vector>

using namespace th13;
using namespace th13::test;
using namespace th13test;  // ecl/harness 夹具命名空间（emit/build_scpt/TEST 宏体）

namespace {

// ---- GameHud 级夹具（同 tests/ui/hud_test.cpp：合成 front/ascii bank）----
struct HudHarness {
    GameRandom random;
    AnmEnvironment environment;
    AnmManager manager{random, environment};
    SessionState progress;
    HudPlayerState player;
    HudScoreState score;
    FrameScheduler scheduler;
    GameHud hud{manager, progress, player, score, 3, 2};
    struct SoundStub : HudFrameServices {
        bool hud_sound(i32) override { return true; }
    } sound;
    struct TextSink : HudDrawServices {
        std::vector<HudTextDraw> draws;
        bool hud_text(const HudTextDraw& draw) override {
            draws.push_back(draw);
            return true;
        }
        const HudTextDraw* find(const char* text, float x, float y) const {
            for (const auto& draw : draws)
                if (draw.text == text && draw.position.x == x && draw.position.y == y)
                    return &draw;
            return nullptr;
        }
        void reset() { draws.clear(); }
    } sink;
    HudHarness() {
        auto bank = synthetic_anm(8, trivial_scripts(172));
        CHECK(load_bank(manager, 3, bank));
        CHECK(load_bank(manager, 2, synthetic_anm(302, trivial_scripts(46))));
        CHECK(hud.initialize(HudEntryContext{}));
    }
};

i32 pending(AnmManager& manager, u32 handle) {
    auto* vm = const_cast<AnmManager&>(manager).registry.find(handle);
    return vm ? vm->pending_interrupt : -999;
}

float translation_y(AnmManager& manager, u32 handle) {
    auto* vm = const_cast<AnmManager&>(manager).registry.find(handle);
    return vm ? vm->visual.translation.y : -1.f;
}

// ---- 战斗级夹具（同 m5_lifecycle_test：记录逐席 HUD 回调）----
struct RecordingHost final : StageBattleServices {
    std::vector<std::array<i32, 3>> life_calls, bomb_calls;
    bool life_hud(i32, i32) override { return true; }
    bool bomb_hud(i32, i32) override { return true; }
    bool life_hud_seat(i32 seat, i32 lives, i32 pieces) override {
        life_calls.push_back({seat, lives, pieces});
        return true;
    }
    bool bomb_hud_seat(i32 seat, i32 bombs, i32 pieces) override {
        bomb_calls.push_back({seat, bombs, pieces});
        return true;
    }
    bool request_game_over() override { return true; }
    bool request_player_game_over() override { return true; }
    bool hud_sound(i32) override { return true; }
    bool hud_sound(i32, float) override { return true; }
    bool stop_sound(i32) override { return true; }
};

class Provider final : public EclResourceProvider {
public:
    Provider() {
        std::vector<u8> main;
        emit(main, 0, 23, 0, 0xff, 0, {600});
        emit(main, 0, 10, 0, 0xff, 0, {});
        stage = build_scpt({{"main", main}}, {"enemy.anm", "st01enm.anm"});
    }
    bool read(const std::string& name, std::vector<u8>& out) override {
        if (name == "mphud.ecl") {
            out = stage;
            return true;
        }
        if (name.size() > 4 && name.compare(name.size() - 4, 4, ".sht") == 0) {
            static const std::string root = [] {
                std::string path = __FILE__;
                for (int i = 0; i < 8; ++i) {
                    const std::string probe = path + "/artifacts/reference/raw/pl00a.sht";
                    std::FILE* check = std::fopen(probe.c_str(), "rb");
                    if (check) { std::fclose(check); break; }
                    const auto cut = path.find_last_of("/\\");
                    if (cut == std::string::npos) break;
                    path.erase(cut);
                }
                return path;
            }();
            std::FILE* file = std::fopen((root + "/artifacts/reference/raw/" + name).c_str(), "rb");
            if (!file) return false;
            std::fseek(file, 0, SEEK_END);
            const long size = std::ftell(file);
            std::fseek(file, 0, SEEK_SET);
            out.resize(size_t(size < 0 ? 0 : size));
            const size_t got = out.empty() ? 1 : std::fread(out.data(), 1, out.size(), file);
            std::fclose(file);
            return got == out.size();
        }
        return false;
    }
    bool animation(u32, const std::string& name) override {
        if (animations) animations->push_back(name);
        return true;
    }
    std::vector<std::string>* animations = nullptr;
    std::vector<u8> stage;
};

struct BattleFixture {
    Provider provider;
    RecordingHost host;
    SessionState progress;
    HudPlayerState player;
    HudScoreState score;
    StageBattle battle;
    GameInput idle;

    bool init() {
        provider.animations = &battle.sim.anim_list;
        if (!battle.load(provider, "mphud.ecl", 2, host, progress, player, score)) {
            std::printf("battle.load error: %s\n", battle.error().c_str());
            return false;
        }
        return true;
    }
    void enable(i32 count, std::array<i32, 3> characters) {
        CHECK(battle.enable_seats(provider, count, characters));
    }
};

}  // namespace

// ---- mp_hud_seat_icon_layout：图标集创建 / 平移 / interrupt / 阈值 / 隔离 ----
TEST(mp_hud_seat_icon_layout) {
    HudHarness harness;
    // 非法席数拒绝；未启用的席位回落单套面。
    CHECK(!harness.hud.enable_multiplayer(1));
    CHECK(!harness.hud.enable_multiplayer(4));
    CHECK(harness.hud.enable_multiplayer(2));
    CHECK_EQ(harness.hud.mp_seat_count, 2);
    // 幂等：重复启用不重复建组。
    const u32 seat1_first = harness.hud.mp_life_icons[0][0];
    CHECK(harness.hud.enable_multiplayer(2));
    CHECK_EQ(harness.hud.mp_life_icons[0][0], seat1_first);

    // 席 1 图标组：8 残机 + 8 灵击，平移 = 1*pitch；render_flags 清 0xc000
    //（与 initialize 的单套同式）。
    for (u32 i = 0; i < 8; ++i) {
        CHECK(harness.hud.mp_life_icons[0][i] != 0);
        CHECK(harness.hud.mp_bomb_icons[0][i] != 0);
        CHECK_EQ(translation_y(harness.manager, harness.hud.mp_life_icons[0][i]),
                 GameHud::kMpRowPitch);
        CHECK_EQ(translation_y(harness.manager, harness.hud.mp_bomb_icons[0][i]),
                 GameHud::kMpRowPitch - 10.f);
        auto* vm = const_cast<AnmManager&>(harness.manager).registry.find(
            harness.hud.mp_life_icons[0][i]);
        CHECK(vm && (vm->visual.render_flags & 0xc000u) == 0);
    }
    // 席 0 单套仍在 SP 原位（不平移）。
    CHECK_EQ(translation_y(harness.manager, harness.hud.life_icons[0]), 0.f);

    // life_seat(1, 2, 5, idx0)：阈值 8 → 碎片档位 (5*5)/8+7 = 10。
    CHECK(harness.hud.life_seat(1, 2, 5, 0));
    for (u32 i = 0; i < 2; ++i)
        CHECK_EQ(pending(harness.manager, harness.hud.mp_life_icons[0][i]), 2);
    CHECK_EQ(pending(harness.manager, harness.hud.mp_life_icons[0][2]), 10);
    for (u32 i = 3; i < 8; ++i)
        CHECK_EQ(pending(harness.manager, harness.hud.mp_life_icons[0][i]), 3);

    // 逐席阈值（R-A）：同碎片数、席阈值索引 1（阈值 10）→ 档位 9。
    CHECK(harness.hud.life_seat(1, 2, 5, 1));
    CHECK_EQ(pending(harness.manager, harness.hud.mp_life_icons[0][2]), 9);

    // bombs_seat(1, 1, 4)：碎片档位 (4*6)/8+7 = 10。
    CHECK(harness.hud.bombs_seat(1, 1, 4));
    CHECK_EQ(pending(harness.manager, harness.hud.mp_bomb_icons[0][0]), 2);
    CHECK_EQ(pending(harness.manager, harness.hud.mp_bomb_icons[0][1]), 10);
    for (u32 i = 2; i < 8; ++i)
        CHECK_EQ(pending(harness.manager, harness.hud.mp_bomb_icons[0][i]), 3);

    // 席位刷新不串扰席 0 单套：先定席 0 状态，再刷席 1，席 0 pending 不变。
    CHECK(harness.hud.life(3, 0));
    CHECK_EQ(pending(harness.manager, harness.hud.life_icons[0]), 2);
    CHECK_EQ(pending(harness.manager, harness.hud.life_icons[3]), 7);  // 0 碎片 → 档 7
    CHECK(harness.hud.life_seat(1, 2, 5, 0));
    CHECK_EQ(pending(harness.manager, harness.hud.life_icons[0]), 2);
    CHECK_EQ(pending(harness.manager, harness.hud.life_icons[3]), 7);

    // 1P 图标与数字都应使用该席阈值，即使单人兼容字段仍为初值。
    CHECK_EQ(harness.progress.life_piece_threshold_index, 0);
    CHECK(harness.hud.life_seat(0, 3, 5, 2));
    CHECK_EQ(pending(harness.manager, harness.hud.life_icons[3]), 9);

    // 3 席：席 2 组平移 = 2*pitch。
    CHECK(harness.hud.enable_multiplayer(3));
    CHECK_EQ(harness.hud.mp_seat_count, 3);
    for (u32 i = 0; i < 8; ++i)
        CHECK_EQ(translation_y(harness.manager, harness.hud.mp_life_icons[1][i]),
                 2 * GameHud::kMpRowPitch);
}

// ---- mp_hud_text_rows：真实生产文本及 ANM 几何共用同帧状态 ----
TEST(mp_hud_text_rows) {
    HudHarness harness;
    CHECK(harness.hud.enable_multiplayer(2));
    HudPlayerState seat0, seat1;
    seat0.life_pieces=5; seat0.bomb_pieces=4; seat0.power=100; seat0.max_power=400; seat0.power_step=100;
    seat0.life_piece_threshold_index=2;
    seat1.life_pieces=0; seat1.bomb_pieces=0; seat1.power=200; seat1.max_power=400; seat1.power_step=100;
    seat1.life_piece_threshold_index=1;
    harness.progress.graze=7; harness.progress.max_point=123456;
    MpHudSeats view; view.count=2; view.local_seat=1; view.ghost=1;
    view.seats={&seat0,&seat1,nullptr};
    HudDrawContext context; context.mp=&view;
    CHECK(harness.hud.draw(context,harness.sink));
    const auto* label0=harness.sink.find("1P",488,90);
    const auto* ghost0=harness.sink.find("GHOST",428,90);
    const auto* label1=harness.sink.find("2P",488,170);
    CHECK(label0 && ghost0 && label1);
    if (label0 && ghost0 && label1) {
        CHECK_EQ(label0->style.color,0x8cffffffu);
        CHECK_EQ(ghost0->style.color,0x8cffffffu);
        CHECK_EQ(label1->style.color,0xffffff00u);
        CHECK_EQ(label0->style.font,1);
        CHECK_EQ(label0->style.scale.x,1.f);
    }
    CHECK(harness.sink.find("  5/12",628,117));
    CHECK(harness.sink.find("  4/8",628,145));
    CHECK(harness.sink.find("1.",540,156));
    CHECK(harness.sink.find("  0/10",628,197));
    CHECK(harness.sink.find("  0/8",628,225));
    CHECK(harness.sink.find("2.",540,236));
    CHECK(harness.sink.find("1,230",620,268));
    const auto* normal_graze=harness.sink.find("7",620,290);
    CHECK(normal_graze != nullptr);
    if(normal_graze) {CHECK_EQ(normal_graze->style.font,4);CHECK_EQ(normal_graze->style.scale.x,1.f);}
    CHECK(!harness.sink.find("MAX POINT",424,268));
    CHECK(!harness.sink.find("GRAZE",424,290));
    harness.sink.reset();
    CHECK(harness.hud.draw(HudDrawContext{},harness.sink));
    CHECK(harness.sink.find("  0",576,122));
    CHECK(harness.sink.find("  0",576,160));
    CHECK(harness.sink.find("1.",540,216));
    CHECK(harness.sink.find("7",620,260));
    CHECK(!harness.sink.find("2P",488,170));

    const char* root=std::getenv("TH13_NATIVE_ASSET_ROOT");
    CHECK(root != nullptr); if (!root) return;
    auto read=[&](const char* name) { std::ifstream f(std::string(root)+"/"+name,std::ios::binary);
        return std::vector<u8>{std::istreambuf_iterator<char>(f),{}}; };
    const auto front=read("front.anm"), ascii=read("ascii.anm");
    CHECK(!front.empty() && !ascii.empty()); if (front.empty() || ascii.empty()) return;
    GameRandom rng; AnmEnvironment env; AnmManager anm{rng,env};
    CHECK(anm.load(3,front.data(),u32(front.size())));
    CHECK(anm.load(2,ascii.data(),u32(ascii.size())));
    CHECK(anm.sprite_fallback(2));
    SessionState progress; HudPlayerState base; HudScoreState score;
    GameHud real{anm,progress,base,score,3,2};
    CHECK(real.initialize({})); CHECK(real.enable_multiplayer(3));
    std::array<HudPlayerState,3> full{};
    for (int seat=0;seat<3;++seat) {
        auto& p=full[size_t(seat)]; p.extra_lives=8; p.bombs=8; p.life_pieces=999; p.bomb_pieces=999;
        p.life_piece_threshold_index=6; p.power=400; p.max_power=400; p.power_step=100;
        CHECK(real.life_seat(seat,8,999,6)); CHECK(real.bombs_seat(seat,8,999));
    }
    progress.life_piece_threshold_index=6;
    progress.max_point=2147483647; progress.graze=2147483647;
    for(int i=0;i<45;++i) CHECK(anm.update());

    struct Rect { float l,t,r,b; };
    auto rect=[](AnmVm& vm) {
        Vec3 p[4]{}; CHECK(anm_quad_positions(vm,p)); Rect r{p[0].x,p[0].y,p[0].x,p[0].y};
        for(const auto& v:p) {r.l=std::min(r.l,v.x);r.r=std::max(r.r,v.x);r.t=std::min(r.t,v.y);r.b=std::max(r.b,v.y);} return r;
    };
    auto intersects=[](const Rect& a,const Rect& b) {return a.l<b.r && b.l<a.r && a.t<b.b && b.t<a.b;};
    auto in_screen=[](const Rect& r){return r.l>=0 && r.r<=640 && r.t>=0 && r.b<=480;};
    struct Glyphs final:AsciiGlyphServices {
        struct Sample { Vec3 corners[4]; float u0,v0,u1,v1;u32 color; };
        std::vector<Rect> rects;std::vector<Sample> samples;
        bool ascii_glyph(AnmVm& vm) override {
            Vec3 p[4]{}; if(!anm_quad_positions(vm,p)) return false;
            Rect r{p[0].x,p[0].y,p[0].x,p[0].y};
            for(const auto& v:p) {r.l=std::min(r.l,v.x);r.r=std::max(r.r,v.x);r.t=std::min(r.t,v.y);r.b=std::max(r.b,v.y);} rects.push_back(r);
            const auto* sprite=vm.texture_sprite();if(!sprite)return false;
            Sample sample{};for(int i=0;i<4;++i)sample.corners[i]=p[i];
            sample.u0=sprite->u0;sample.v0=sprite->v0;sample.u1=sprite->u1;sample.v1=sprite->v1;sample.color=vm.visual.color;samples.push_back(sample);return true;
        }
    };
    std::array<Rect,48> icons{};
    for(size_t seat=0;seat<3;++seat) for(size_t kind=0;kind<2;++kind) for(size_t i=0;i<8;++i) {
        const auto handle=seat ? (kind?real.mp_bomb_icons[seat-1][i]:real.mp_life_icons[seat-1][i])
                               : (kind?real.bomb_icons[i]:real.life_icons[i]);
        auto* vm=anm.registry.find(handle); CHECK(vm != nullptr); if(!vm) return;
        CHECK(vm->visible); icons[seat*16+kind*8+i]=rect(*vm);
    }
    for(size_t a=0;a<icons.size();++a) {
        CHECK(in_screen(icons[a]));
        CHECK(icons[a].r>icons[a].l && icons[a].b>icons[a].t);
        for(size_t b=a+1;b<icons.size();++b) CHECK(!intersects(icons[a],icons[b]));
    }
    // 所有生产字形和移动后的负控字形使用同一碰撞检测器。
    const auto collides_icon=[&](const Rect& r) {
        return std::any_of(icons.begin(),icons.end(),[&](const Rect& icon){return intersects(r,icon);});
    };
    std::array<Rect,17> labels{};
    std::array<u32,17> label_handles{};
    auto* root_vm=anm.registry.find(real.root); CHECK(root_vm != nullptr); if(!root_vm) return;
    constexpr i32 label_scripts[]={8,9,10,11,12};
    for(size_t seat=0;seat<3;++seat) for(size_t i=0;i<5;++i) {
        auto* vm=seat?anm.registry.find(real.mp_resource_labels[seat-1][i])
                     :anm.registry.find_child_script(*root_vm,label_scripts[i],0);
        CHECK(vm != nullptr); if(!vm) return;
        label_handles[seat*5+i]=anm.registry.handle(*vm);
    }
    for(int i=0;i<2;++i) {
        auto* vm=anm.registry.find_child_script(*root_vm,13+i,0);CHECK(vm != nullptr);if(!vm) return;
        label_handles[15+i]=anm.registry.handle(*vm);
    }
    // At 45 ticks original front scripts 8/9 have begun fading in; the
    // later scripts have alpha zero and must not acquire synthetic draws.
    {
        struct StartupGraphics final:ZunGraphics {
            touhou::graphics::PipelineState state;size_t vertices=0;
            touhou::graphics::PipelineState& pipeline() override{return state;}
            u32 texture(const AnmResource&,u32) override{return 1;}
            void bind_texture(u32) override{} void set_layout(touhou::graphics::VertexLayout) override{}
            void set_matrix(touhou::graphics::MatrixKind,const Matrix4&) override{}
            void triangles(u32 count,const void*,u32) override{vertices+=size_t(count)*3;}
            void primitives(touhou::graphics::Topology,u32,const void*,u32) override{}
            bool select_target(const AnmResource*,u32) override{return true;}
            bool clear_target(u32,const GraphicsViewport*) override{return true;}
            bool clear_depth(const GraphicsViewport*) override{return true;}
            void set_viewport(const GraphicsViewport&) override{}
        } g;
        AnmRenderer r{g};
        for(size_t i=0;i<label_handles.size();++i) {
            auto* vm=anm.registry.find(label_handles[i]); CHECK(vm != nullptr);if(!vm)return;
            const bool faded_in=i<15 && (i%5==0 || i%5==1);
            CHECK_EQ((vm->visual.color>>24)!=0,faded_in);
            const auto before=g.vertices;const int result=r.draw(*vm);r.flush();
            if(faded_in) {CHECK(result!=-2);CHECK(g.vertices>before);}
            else {CHECK_EQ(result,-1);CHECK_EQ(g.vertices,before);}
        }
    }
    // Finish the genuine front.anm startup before testing all labels as drawn.
    for(int i=0;i<65;++i) CHECK(anm.update());
    const auto collides_label=[&](const Rect& r) {
        return std::any_of(labels.begin(),labels.end(),[&](const Rect& label){return intersects(r,label);});
    };
    // Capture the actual SP Power glyphs for exact comparison with each MP
    // row. Only the vertical translation and authorized ghost alpha differ.
    base.power=400;base.max_power=400;base.power_step=100;
    HudHarness::TextSink sp_power;CHECK(real.draw(HudDrawContext{},sp_power));
    const HudTextDraw* sp_segments[]={sp_power.find("4.",540,216),sp_power.find("00",560,223),
                                      sp_power.find("/4.",574,216),sp_power.find("00",606,223)};
    std::array<Glyphs,4> sp_glyphs;
    for(size_t i=0;i<4;++i) {
        CHECK(sp_segments[i] != nullptr);if(!sp_segments[i])return;
        AsciiText t{anm,env};CHECK(t.initialize(2));CHECK(t.hud_text(*sp_segments[i]));CHECK(t.draw(0,sp_glyphs[i]));
    }
    // Record actual op606 production vertices, including original shared lines.
    struct RuleGraphics final:ZunGraphics {
        struct V {Vec3 p;float w;u32 color;};std::vector<V> vertices;touhou::graphics::PipelineState state;
        touhou::graphics::PipelineState& pipeline()override{return state;}
        u32 texture(const AnmResource&,u32)override{return 1;}void bind_texture(u32)override{}
        void set_layout(touhou::graphics::VertexLayout)override{}void set_matrix(touhou::graphics::MatrixKind,const Matrix4&)override{}
        void triangles(u32,const void*,u32)override{}
        void primitives(touhou::graphics::Topology kind,u32 n,const void* data,u32 stride)override {
            CHECK_EQ(stride,u32(sizeof(V)));if(stride!=sizeof(V))return;
            auto* p=static_cast<const V*>(data);vertices.insert(vertices.end(),p,p+touhou::graphics::vertex_count(kind,n));
        }
        bool select_target(const AnmResource*,u32)override{return true;}bool clear_target(u32,const GraphicsViewport*)override{return true;}
        bool clear_depth(const GraphicsViewport*)override{return true;}void set_viewport(const GraphicsViewport&)override{}
    } rule_graphics;
    AnmRenderer rule_renderer{rule_graphics};std::array<Rect,10> rules{};
    auto capture_rules=[&]() {
        for(int script=19;script<=28;++script) {
            auto* vm=anm.registry.find_child_script(*root_vm,script,0);CHECK(vm != nullptr);if(!vm)return false;
            rule_graphics.vertices.clear();rule_renderer.invalidate();CHECK(rule_renderer.draw(*vm)!=-2);rule_renderer.flush();
            CHECK_EQ(rule_graphics.vertices.size(),size_t(4));if(rule_graphics.vertices.size()!=4)return false;
            Rect box{10000,10000,-10000,-10000};for(const auto& v:rule_graphics.vertices) {
                box.l=std::min(box.l,v.p.x);box.r=std::max(box.r,v.p.x);box.t=std::min(box.t,v.p.y);box.b=std::max(box.b,v.p.y);
            } rules[size_t(script-19)]=box;
        }return true;
    };
    // Let the original line expansion complete naturally before capturing it.
    for(int i=0;i<25;++i) CHECK(anm.update());CHECK(capture_rules());
    for(size_t i=0;i<6;++i) {
        const float y=175.f+80.f*float(i/2);CHECK_EQ(rules[i].t,y);CHECK_EQ(rules[i].b,y+2);
        CHECK_EQ(rules[i].l,(i%2)?567.f-52.8f:567.f);CHECK_EQ(rules[i].r,(i%2)?567.f:567.f+52.8f);
    }
    for(size_t i=6;i<10;++i) {const float y=i<8?368.f:390.f;CHECK_EQ(rules[i].t,y);CHECK_EQ(rules[i].b,y+2);}
    MpHudSeats triple; triple.count=3; triple.ghost=4; triple.seats={&full[0],&full[1],&full[2]};
    HudDrawContext triple_context; triple_context.mp=&triple;
    const i32 graze_cases[]={7,999999999,1000000000,2147483647};
    const char* graze_text[]={"7","999,999,999","1,000,000,000","2,147,483,647"};
    for(int local : {0,2}) for(bool sentinel : {false,true}) for(int graze_case : {0,1,2,3}) for(int pieces : {99,999}) {
        triple.local_seat=local;
        for(auto& p:full) {p.life_pieces=pieces;p.bomb_pieces=pieces;}
        progress.graze=graze_cases[graze_case];
        progress.life_piece_threshold_index=sentinel?7:6;
        for(auto& p:full) p.life_piece_threshold_index=sentinel?7:6;
        for(int seat=0;seat<3;++seat) {
            const auto& p=full[size_t(seat)];
            CHECK(real.life_seat(seat,p.extra_lives,p.life_pieces,p.life_piece_threshold_index));
            CHECK(real.bombs_seat(seat,p.bombs,p.bomb_pieces));
        }
        CHECK(anm.update());
        for(size_t seat=0;seat<3;++seat) for(size_t kind=0;kind<2;++kind) for(size_t i=0;i<8;++i) {
            const auto handle=seat ? (kind?real.mp_bomb_icons[seat-1][i]:real.mp_life_icons[seat-1][i])
                                   : (kind?real.bomb_icons[i]:real.life_icons[i]);
            auto* vm=anm.registry.find(handle); CHECK(vm != nullptr); if(!vm) return;
            CHECK(vm->visible); icons[seat*16+kind*8+i]=rect(*vm);
        }
        for(size_t a=0;a<icons.size();++a) {
            CHECK(in_screen(icons[a])); CHECK(icons[a].r>icons[a].l && icons[a].b>icons[a].t);
            for(size_t b=a+1;b<icons.size();++b) CHECK(!intersects(icons[a],icons[b]));
        }
        for(size_t i=0;i<labels.size();++i) {
            auto* vm=anm.registry.find(label_handles[i]);CHECK(vm != nullptr);if(!vm)return;
            CHECK(vm->visible);CHECK((vm->visual.color>>24)>0);labels[i]=rect(*vm);CHECK(in_screen(labels[i]));
            CHECK(labels[i].r>labels[i].l && labels[i].b>labels[i].t);
        }
        for(size_t i=0;i<labels.size();++i) {
            CHECK(!collides_icon(labels[i]));
            for(const auto& rule:rules) CHECK(!intersects(labels[i],rule));
            for(size_t j=i+1;j<labels.size();++j) CHECK(!intersects(labels[i],labels[j]));
        }
        for(const auto& icon:icons)for(const auto& rule:rules)CHECK(!intersects(icon,rule));
        HudHarness::TextSink actual; CHECK(real.draw(triple_context,actual));
        CHECK_EQ(actual.draws.size(),size_t(26));
        for(int seat=0;seat<3;++seat) {
            const float row=100.f+80.f*seat;
            const std::string title=std::to_string(seat+1)+"P";
            const auto* title_draw=actual.find(title.c_str(),488,row-10);
            CHECK(title_draw != nullptr);
            if(title_draw) {
                CHECK_EQ(title_draw->style.font,1); CHECK_EQ(title_draw->style.scale.x,1.f);
                const u32 expected=(seat==2?0x8c000000u:0xff000000u)|(seat==local?0xffff00u:0xffffffu);
                CHECK_EQ(title_draw->style.color,expected);
            }
            const auto* ghost_draw=actual.find("GHOST",428,row-10);
            CHECK_EQ(ghost_draw!=nullptr,seat==2);
            if(ghost_draw && title_draw) CHECK_EQ(ghost_draw->style.color,title_draw->style.color);
            const auto* life=actual.find(sentinel?"---/---":pieces==99?" 99/25":"999/25",628,row+17);
            const auto* bomb=actual.find(pieces==99?" 99/8":"999/8",628,row+45);
            const HudTextDraw* power[]={actual.find("4.",540,row+56),actual.find("00",560,row+63),
                                         actual.find("/4.",574,row+56),actual.find("00",606,row+63)};
            CHECK(life && bomb && power[0] && power[1] && power[2] && power[3]);
            if(life && bomb) {
                CHECK_EQ(life->style.font,2);CHECK_EQ(bomb->style.font,2);CHECK_EQ(life->style.alignment,2);
                CHECK_EQ(life->style.color>>24,seat==2?0x8cu:0xffu);
            }
            for(size_t k=0;k<4;++k) {
                if(!power[k])return;
                CHECK_EQ(power[k]->style.font,4);CHECK_EQ(power[k]->style.alignment,1);
                AsciiText text{anm,env};CHECK(text.initialize(2));CHECK(text.hud_text(*power[k]));
                Glyphs mp;CHECK(text.draw(0,mp));CHECK_EQ(mp.samples.size(),sp_glyphs[k].samples.size());
                const float dy=power[k]->position.y-sp_segments[k]->position.y;
                for(size_t n=0;n<std::min(mp.samples.size(),sp_glyphs[k].samples.size());++n) {
                    const auto& a=mp.samples[n];const auto& b=sp_glyphs[k].samples[n];
                    for(int v=0;v<4;++v) {
                        const float translated_y=b.corners[v].y+dy;
                        CHECK(std::memcmp(&a.corners[v].x,&b.corners[v].x,sizeof(float))==0);
                        CHECK(std::memcmp(&a.corners[v].y,&translated_y,sizeof(float))==0);
                        CHECK(std::memcmp(&a.corners[v].z,&b.corners[v].z,sizeof(float))==0);
                    }
                    CHECK(std::memcmp(&a.u0,&b.u0,sizeof(float))==0);CHECK(std::memcmp(&a.v0,&b.v0,sizeof(float))==0);
                    CHECK(std::memcmp(&a.u1,&b.u1,sizeof(float))==0);CHECK(std::memcmp(&a.v1,&b.v1,sizeof(float))==0);
                    const u32 expected_color=(b.color&0xffffffu)|(seat==2?0x8c000000u:0xff000000u);
                    CHECK(std::memcmp(&a.color,&expected_color,sizeof(u32))==0);
                }
            }
        }
        int max_values=0,graze_values=0;
        std::vector<Rect> first_power;
        std::vector<std::vector<Rect>> groups;std::vector<int> group_ids;
        for(const auto& draw:actual.draws) {
            int power_group=-1;
            if(draw.style.font==4 && draw.style.alignment==1 && draw.position.y>=156 && draw.position.y<=336)
                power_group=int((draw.position.y-156)/80);
            max_values += draw.text=="21,474,830"; graze_values += draw.text==graze_text[graze_case];
            AsciiText text{anm,env}; CHECK(text.initialize(2)); CHECK(text.hud_text(draw));
            Glyphs glyphs; CHECK(text.draw(0,glyphs));
            if(power_group==0)first_power.insert(first_power.end(),glyphs.rects.begin(),glyphs.rects.end());
            for(const auto& g:glyphs.rects) {
                CHECK(in_screen(g)); CHECK(!collides_icon(g)); CHECK(!collides_label(g));
                for(const auto& rule:rules) CHECK(!intersects(g,rule));
                for(size_t j=0;j<groups.size();++j) if(power_group<0 || group_ids[j]!=power_group)
                    for(const auto& old:groups[j]) CHECK(!intersects(g,old));
            }
            groups.push_back(std::move(glyphs.rects));group_ids.push_back(power_group);
        }
        CHECK_EQ(max_values,1); CHECK_EQ(graze_values,1);
        const auto* graze=actual.find(graze_text[graze_case],620,370);
        CHECK(graze != nullptr);
        if(graze) {CHECK_EQ(graze->style.font,graze_case>=2?5:4); CHECK_EQ(graze->style.scale.x,graze_case>=2?.85f:1.f);}
        CHECK(!first_power.empty());
        if(!first_power.empty()) {
            float left=first_power.front().l,top=first_power.front().t;
            for(const auto& r:first_power){left=std::min(left,r.l);top=std::min(top,r.t);}
            const float dx=icons[7].l-left,dy=icons[7].t-top;bool rejected=false;
            for(auto r:first_power){r.l+=dx;r.r+=dx;r.t+=dy;r.b+=dy;rejected |= collides_icon(r);}
            CHECK(rejected);
        }
    }
    // A two-seat HUD must collapse only the absent third pair to zero area.
    {
        GameRandom two_rng;AnmEnvironment two_env;AnmManager two{two_rng,two_env};
        CHECK(two.load(3,front.data(),u32(front.size())));CHECK(two.load(2,ascii.data(),u32(ascii.size())));CHECK(two.sprite_fallback(2));
        SessionState progress2;HudPlayerState player2;HudScoreState score2;GameHud hud2{two,progress2,player2,score2,3,2};
        CHECK(hud2.initialize({}));CHECK(hud2.enable_multiplayer(2));for(int i=0;i<130;++i)CHECK(two.update());
        auto* two_root=two.registry.find(hud2.root);CHECK(two_root != nullptr);if(!two_root)return;
        for(int script=19;script<=28;++script) {
            auto* vm=two.registry.find_child_script(*two_root,script,0);CHECK(vm != nullptr);if(!vm)return;
            rule_graphics.vertices.clear();rule_renderer.invalidate();CHECK(rule_renderer.draw(*vm)!=-2);rule_renderer.flush();
            CHECK_EQ(rule_graphics.vertices.size(),size_t(4));if(rule_graphics.vertices.size()!=4)return;
            float l=10000,r=-10000,t=10000,b=-10000;for(const auto& v:rule_graphics.vertices){l=std::min(l,v.p.x);r=std::max(r,v.p.x);t=std::min(t,v.p.y);b=std::max(b,v.p.y);}
            if(script==23 || script==24) CHECK_EQ(l,r);
            else {CHECK(l<r);CHECK(t<b);}
        }
    }
    // Draw identical production state three times: only selected seat sprites
    // may fade, and clearing ghost must restore their original colors.
    struct Graphics final:ZunGraphics {
        touhou::graphics::PipelineState state;
        touhou::graphics::VertexLayout layout=touhou::graphics::VertexLayout::WorldUv;
        std::vector<AnmGeometryVertex> sprites;
        struct ColorVertex {Vec3 position;float reciprocal_w;u32 color;};
        std::vector<ColorVertex> panel;
        size_t panel_vertices=0; u32 last_stride=0;
        touhou::graphics::PipelineState& pipeline() override {return state;}
        u32 texture(const AnmResource&,u32) override {return 1;}
        void bind_texture(u32) override {}
        void set_layout(touhou::graphics::VertexLayout value) override {layout=value;}
        void set_matrix(touhou::graphics::MatrixKind,const Matrix4&) override {}
        void triangles(u32 count,const void* data,u32 stride) override {
            last_stride=stride;
            if(layout==touhou::graphics::VertexLayout::ScreenColor) {
                CHECK_EQ(stride,u32(sizeof(ColorVertex))); panel_vertices+=size_t(count)*3;
                const auto* p=static_cast<const ColorVertex*>(data);panel.insert(panel.end(),p,p+count*3);return;
            }
            if(layout==touhou::graphics::VertexLayout::ScreenColorUv) {
                CHECK_EQ(stride,u32(sizeof(AnmGeometryVertex)));
                auto* p=static_cast<const AnmGeometryVertex*>(data); sprites.insert(sprites.end(),p,p+count*3);
            }
        }
        void primitives(touhou::graphics::Topology,u32,const void*,u32) override {}
        bool select_target(const AnmResource*,u32) override {return true;}
        bool clear_target(u32,const GraphicsViewport*) override {return true;}
        bool clear_depth(const GraphicsViewport*) override {return true;}
        void set_viewport(const GraphicsViewport&) override {}
    } graphics;
    struct Cameras final:AnmDrawServices {bool camera(DrawCamera,bool) override{return true;}} cameras;
    FrameScheduler scheduler; AnmRenderer renderer{graphics};
    AnmDrawSchedule schedule{scheduler,anm,renderer,graphics,cameras}; CHECK(schedule.error.empty());
    std::array<std::array<u32,21>,3> handles{};
    constexpr i32 scripts[]={8,9,10,11,12};
    for(size_t seat=0;seat<3;++seat) {
        for(size_t i=0;i<8;++i) {
            handles[seat][i]=seat?real.mp_life_icons[seat-1][i]:real.life_icons[i];
            handles[seat][i+8]=seat?real.mp_bomb_icons[seat-1][i]:real.bomb_icons[i];
        }
        for(size_t i=0;i<5;++i) {
            auto* label=seat?anm.registry.find(real.mp_resource_labels[seat-1][i]):anm.registry.find_child_script(*root_vm,scripts[i],0);
            CHECK(label != nullptr); if(!label) return;
            handles[seat][16+i]=anm.registry.handle(*label);
        }
    }
    auto* selected=anm.registry.find(real.life_icons[0]); CHECK(selected != nullptr); if(!selected) return;
    const u32 original_color=selected->visual.color;
    auto draw_ghost=[&](u8 ghost) {
        graphics.sprites.clear();graphics.panel.clear();graphics.panel_vertices=0;
        AnmDrawSchedule::set_hud_ghost_icons(anm,handles,ghost);
        HudHarness::TextSink unused; CHECK(real.draw(triple_context,unused));
        CHECK(scheduler.draw()>=0);renderer.flush();
        return graphics.sprites;
    };
    const auto baseline=draw_ghost(0), ghost=draw_ghost(1), restored=draw_ghost(0);
    CHECK(!baseline.empty()); CHECK_EQ(ghost.size(),baseline.size()); CHECK_EQ(restored.size(),baseline.size());
    bool faded=false,unchanged=false;
    for(size_t i=0;i<std::min(baseline.size(),std::min(ghost.size(),restored.size()));++i) {
        const auto& a=baseline[i];const auto& b=ghost[i];const auto& c=restored[i];
        CHECK_EQ(a.position.x,b.position.x);CHECK_EQ(a.position.y,b.position.y);CHECK_EQ(a.position.z,b.position.z);
        CHECK_EQ(a.uv.x,b.uv.x);CHECK_EQ(a.uv.y,b.uv.y);CHECK_EQ(a.reciprocal_w,b.reciprocal_w);
        CHECK_EQ(a.color,c.color);
        if(a.color==b.color) unchanged=true;
        else {CHECK_EQ(a.color&0xffffffu,b.color&0xffffffu);CHECK_EQ(b.color>>24,((a.color>>24)*140u+127u)/255u);faded=true;}
    }
    CHECK(faded && unchanged); CHECK_EQ(selected->visual.color,original_color);
    // Every native label must produce actual scheduler vertices. Compare its
    // isolated production draw with a contiguous quad in the scheduler output,
    // then repeat at ghost opacity and verify its VM state is unchanged.
    for(const auto handle:label_handles) {
        auto* vm=anm.registry.find(handle);CHECK(vm != nullptr);if(!vm)return;
        graphics.sprites.clear();renderer.invalidate();CHECK(renderer.draw(*vm)!=-2);renderer.flush();
        const auto isolated=graphics.sprites;
        CHECK_EQ(isolated.size(),size_t(6));
        bool submitted=false;
        for(size_t i=0;i+isolated.size()<=baseline.size() && !isolated.empty();++i) {
            bool same=true;
            for(size_t j=0;j<isolated.size();++j) {
                const auto& a=isolated[j];const auto& b=baseline[i+j];
                same &= a.position.x==b.position.x && a.position.y==b.position.y && a.position.z==b.position.z
                    && a.reciprocal_w==b.reciprocal_w && a.color==b.color && a.uv.x==b.uv.x && a.uv.y==b.uv.y;
            }
            if(same) {submitted=true;break;}
        }
        CHECK(submitted);
        std::array<u8,sizeof(vm->visual)> visual_bytes{};
        std::array<u8,sizeof(vm->variables)> variable_bytes{};
        std::memcpy(visual_bytes.data(),&vm->visual,visual_bytes.size());
        std::memcpy(variable_bytes.data(),&vm->variables,variable_bytes.size());
        const auto time=vm->timer.current;const auto pending=vm->pending_interrupt;
        graphics.sprites.clear();renderer.invalidate();CHECK(renderer.draw_with_opacity(*vm,140)!=-2);renderer.flush();
        CHECK_EQ(graphics.sprites.size(),isolated.size());
        CHECK(std::memcmp(visual_bytes.data(),&vm->visual,visual_bytes.size())==0);
        CHECK(std::memcmp(variable_bytes.data(),&vm->variables,variable_bytes.size())==0);
        CHECK_EQ(vm->timer.current,time);CHECK_EQ(vm->pending_interrupt,pending);
    }

    // An empty registry and multiplayer selection must submit no new backing
    // geometry. Later submissions still set their own vertex layout and stride.
    {
        GameRandom empty_rng; AnmEnvironment empty_env; AnmManager empty{empty_rng,empty_env};
        Graphics state_graphics; AnmRenderer state_renderer{state_graphics};
        FrameScheduler state_scheduler; AnmDrawSchedule state_schedule{state_scheduler,empty,state_renderer,state_graphics,cameras};
        struct Probe {
            Graphics& g; touhou::graphics::PipelineState before,after; FrameCallback pre,post;
            explicit Probe(Graphics& graphics):g(graphics) {
                pre.owner=this; pre.enabled=true;pre.run=[](void* p){auto& s=*static_cast<Probe*>(p);s.before=s.g.state;return i32(FrameAction::Continue);};
                post.owner=this;post.enabled=true;post.run=[](void* p){auto& s=*static_cast<Probe*>(p);s.after=s.g.state;
                    s.g.set_layout(touhou::graphics::VertexLayout::WorldColorUv);s.g.triangles(1,nullptr,24);return i32(FrameAction::Continue);};
            }
        } probe{state_graphics};
        CHECK(state_scheduler.add(probe.pre,FramePass::Draw,56)>=0);
        CHECK(state_scheduler.add(probe.post,FramePass::Draw,58)>=0);
        std::array<std::array<u32,21>,3> registered{};
        registered[0][0]=1;registered[1][0]=2;registered[2][0]=3;
        AnmDrawSchedule::set_hud_ghost_icons(empty,registered,0);
        CHECK(state_scheduler.draw()>=0);
        CHECK(probe.before.compatible(probe.after));
        CHECK_EQ(state_graphics.layout,touhou::graphics::VertexLayout::WorldColorUv);
        CHECK_EQ(state_graphics.last_stride,24u);
        CHECK(state_graphics.panel.empty());
        CHECK(state_graphics.sprites.empty());
        CHECK_EQ(empty.registry.count(),0u);
        state_graphics.panel.clear();
        AnmDrawSchedule::set_hud_ghost_icons(empty,{},0);
        CHECK(state_scheduler.draw()>=0);
        CHECK(state_graphics.panel.empty());
    }

}

// ---- mp_hud_seat_callbacks：逐席资源回调（R-I3 发放 / Bomb / 定向赠送）----
ItemCollector collector_at(float x, float y, i32 seat, i32 character,
                          HudPlayerState& hud, SessionState& progress,
                          HudScoreState& score) {
    ItemCollector c;
    c.x = x;
    c.y = y;
    c.state = 1;
    c.dialog = false;
    c.homing_speed = 4.0f;
    const float half = 60.0f * 0.25f;
    c.box = {x - half, y - half, x + half, y + half};
    const float auto_half = 60.0f * 0.5f;
    c.box_auto = {x - auto_half, y - auto_half, x + auto_half, y + auto_half};
    c.progress = &progress;
    c.player = &hud;
    c.score = &score;
    c.seat = seat;
    c.eligible = true;
    c.character = character;
    return c;
}

TEST(mp_hud_seat_callbacks) {
    BattleFixture dual;
    CHECK(dual.init());
    dual.enable(2, {0, 1, 0});
    auto& battle = dual.battle;
    auto& manager = battle.item_manager;
    battle.seats[0].x = -100.f;
    battle.seats[0].y = 400.f;
    battle.seats[1].x = 100.f;
    battle.seats[1].y = 400.f;
    battle.seat_huds[1].life_pieces = 3;

    // R-I3 广播发放：残机碎片被席 0 拾取 → 全员各 +1 碎片，逐席回调携带
    // 各席权威值（update_seats → collect_check → grant_resources 公共路径）。
    manager.spawn_item(-95.f, 400.f, 0.f, ItemLifePiece, 0.f, 0.f);
    ItemCollector seats[2] = {
        collector_at(-100.f, 400.f, 0, 0, battle.seat_huds[0], dual.progress, dual.score),
        collector_at(100.f, 400.f, 1, 1, battle.seat_huds[1], dual.progress, dual.score)};
    manager.update_seats(seats, 2);
    CHECK_EQ(battle.seat_huds[0].life_pieces, 1);
    CHECK_EQ(battle.seat_huds[1].life_pieces, 4);
    CHECK_EQ(dual.host.life_calls.size(), size_t(2));
    CHECK_EQ(dual.host.life_calls[0][0], 0);
    CHECK_EQ(dual.host.life_calls[0][2], 1);
    CHECK_EQ(dual.host.life_calls[1][0], 1);
    CHECK_EQ(dual.host.life_calls[1][2], 4);
    CHECK(dual.host.bomb_calls.empty());  // 无 Bomb 资源 → 不触发

    // Bomb 键：席位输入沿独立，X 边沿扣该席 Bomb 并按 active_seat 回调。
    dual.host.life_calls.clear();
    GameInput none;
    for (int frame = 0; frame < 25; ++frame) CHECK(battle.step(none));
    battle.seat_inputs[1].update(0x02);  // 席 1 按 X
    CHECK(battle.step(none));
    CHECK_EQ(battle.seat_huds[1].bombs, 1);
    bool seat1_bomb = false;
    for (const auto& call : dual.host.bomb_calls)
        seat1_bomb |= call[0] == 1 && call[1] == 1;
    CHECK(seat1_bomb);
    bool foreign_seat = false;
    for (const auto& call : dual.host.bomb_calls) foreign_seat |= call[0] != 1;
    CHECK(!foreign_seat);  // 席 0 未按 X，不被串扰

    // R-I9 定向赠送（update_transfers）：席 0 五拍送残给幽灵席 1 →
    // 救援事务只刷受影响席（donor 残机回调 + 接收席 Bomb 回调）。
    // MP 下 step 参数写本地席（mp_local_seat=0）的输入沿，远端席直写
    // seat_inputs；送残 = Shift 边沿且全程不按 Z。
    dual.host.life_calls.clear();
    dual.host.bomb_calls.clear();
    battle.seats[1].ghost = true;      // 席 1 幽灵可被救
    battle.seat_huds[0].extra_lives = 2;
    GameInput shift;
    for (int tap = 0; tap < 6; ++tap) {
        shift.update(0x08);
        CHECK(battle.step(shift));
        shift.update(0x00);
        CHECK(battle.step(shift));
    }
    CHECK(!battle.seats[1].ghost);  // 救援成功
    bool donor_life = false, rescued_bomb = false;
    for (const auto& call : dual.host.life_calls)
        donor_life |= call[0] == 0 && call[1] == 1;
    for (const auto& call : dual.host.bomb_calls)
        rescued_bomb |= call[0] == 1 && call[1] == 2;
    CHECK(donor_life);
    CHECK(rescued_bomb);
    // 只有受影响席位被回调（不重发无关席——避免复位图标插值）。
    for (const auto& call : dual.host.life_calls) CHECK_EQ(call[0], 0);
    for (const auto& call : dual.host.bomb_calls) CHECK_EQ(call[0], 1);
}

// ---- mp_hud_rollback_roundtrip：快照恢复含 MP 域 ----
TEST(mp_hud_rollback_roundtrip) {
    HudHarness harness;
    CHECK(harness.hud.enable_multiplayer(2));
    CHECK(harness.hud.life_seat(1, 2, 5, 0));
    const auto snapshot = harness.hud.capture_rollback();
    CHECK(snapshot.mp_seat_count == 2);
    CHECK(snapshot.mp_life_icons[0][2] == harness.hud.mp_life_icons[0][2]);

    // 扰动：扩到 3 席并改句柄。
    CHECK(harness.hud.enable_multiplayer(3));
    CHECK(harness.hud.mp_life_icons[1][2] != 0);
    harness.hud.restore_rollback(snapshot);
    CHECK_EQ(harness.hud.mp_seat_count, 2);
    CHECK_EQ(harness.hud.mp_life_icons[0][2], snapshot.mp_life_icons[0][2]);
    CHECK_EQ(harness.hud.mp_life_icons[1][2], 0u);  // 席 2 组被恢复清除
    // 单套域照旧恢复。
    CHECK_EQ(harness.hud.life_icons[2], snapshot.life_icons[2]);
}

TEST(mp_hud_world_restore_immediate_draw) {
    multiplayer::session.Stop();
    const char* assets = std::getenv("TH13_NATIVE_ASSET_ROOT");
    const char* root = std::getenv("TH13_NATIVE_REPO_ROOT");
    CHECK(assets && root); if (!assets || !root) return;
    ::setenv("TH13_SAVE_PATH", "hud-restore-draw.bin", 1);
    sdl::ApplicationState app;
    CHECK(app.audio_device.initialize_for_test(assets, std::string(root) + "/games/th13/thbgmogg.dat"));
    CHECK(app.initialize(false));
    CHECK(app.configure_multiplayer(3, {0, 1, 3}, 0));
    CHECK(app.start_game(1));
    app.pending_destination = -1;
    CHECK(app.begin_run()); if (!app.run) return;
    GameInput input;
    for (int frame = 0; frame < 35; ++frame) {
        input.update(0);
        CHECK(app.update(input, false) && app.commit(60.f) && app.present());
    }
    auto& battle = app.run->battle;
    battle.seat_huds[0].power = 123;
    battle.seat_huds[1].power = 234;
    battle.seat_huds[2].power = 345;
    battle.seats[1].ghost = true;
    battle.seats[1].dead = true;
    app.run->sync_mp_hud_view();
    multiplayer::WorldState saved;
    CHECK(saved.capture(battle, app.visual, nullptr, app.run->presentation.get(),
                        app.run.get(), app.run_transition.handle()));
    const auto baseline = saved.digest();
    battle.seats[1].ghost = false;
    battle.seats[1].dead = false;
    battle.seat_huds[1].power = 400;
    battle.mp_local_seat = 2; // local presentation is not rollback state.
    app.run->sync_mp_hud_view();
    CHECK(saved.restore(battle, app.visual, nullptr, app.run->presentation.get(), app.run.get()));
    CHECK(battle.seats[1].ghost);
    CHECK_EQ(battle.mp_local_seat, 2);
    CHECK_EQ(app.run->mp_hud_seats_.local_seat, 2);
    CHECK_EQ(app.run->mp_hud_seats_.ghost, 2u);
    for (int seat = 0; seat < 3; ++seat)
        CHECK(app.run->mp_hud_seats_.seats[size_t(seat)] == &battle.seat_huds[size_t(seat)]);
    CHECK_EQ(app.run->mp_hud_seats_.seats[1]->power, 234);
    multiplayer::WorldState restored;
    CHECK(restored.capture(battle, app.visual, nullptr, app.run->presentation.get(),
                           app.run.get(), app.run_transition.handle()));
    CHECK_EQ(restored.digest(), baseline);
    CHECK(app.present()); // no update or sync between restore and draw
    multiplayer::WorldState drawn;
    CHECK(drawn.capture(battle, app.visual, nullptr, app.run->presentation.get(),
                        app.run.get(), app.run_transition.handle()));
    CHECK_EQ(drawn.digest(), baseline);
    CHECK(app.present());
    CHECK(drawn.capture(battle, app.visual, nullptr, app.run->presentation.get(),
                        app.run.get(), app.run_transition.handle()));
    CHECK_EQ(drawn.digest(), baseline);

    struct Graphics final : ZunGraphics {
        touhou::graphics::PipelineState state;
        std::vector<AnmGeometryVertex> vertices;
        touhou::graphics::PipelineState& pipeline() override { return state; }
        u32 texture(const AnmResource&, u32) override { return 1; }
        void bind_texture(u32) override {}
        void set_layout(touhou::graphics::VertexLayout) override {}
        void set_matrix(touhou::graphics::MatrixKind, const Matrix4&) override {}
        void triangles(u32 count, const void* data, u32 stride) override {
            if (stride != sizeof(AnmGeometryVertex)) return;
            const auto* vertex = static_cast<const AnmGeometryVertex*>(data);
            vertices.insert(vertices.end(), vertex, vertex + count * 3);
        }
        void primitives(touhou::graphics::Topology topology, u32 count, const void* data, u32 stride) override {
            if (stride != sizeof(AnmGeometryVertex)) return;
            const auto* vertex = static_cast<const AnmGeometryVertex*>(data);
            const u32 length = topology == touhou::graphics::Topology::Triangles ? count * 3 : count + 2;
            vertices.insert(vertices.end(), vertex, vertex + length);
        }
        bool select_target(const AnmResource*, u32) override { return true; }
        bool clear_target(u32, const GraphicsViewport*) override { return true; }
        bool clear_depth(const GraphicsViewport*) override { return true; }
        void set_viewport(const GraphicsViewport&) override {}
    } graphics;
    struct Cameras final : AnmDrawServices {
        bool camera(DrawCamera, bool) override { return true; }
    } cameras;
    FrameScheduler scheduler;
    AnmRenderer renderer{graphics};
    AnmDrawSchedule schedule{scheduler, app.animations, renderer, graphics, cameras};
    CHECK(schedule.error.empty());
    const auto& hud_state = app.run->hud;
    for (int seat = 0; seat < 3; ++seat) {
        const u32 handle = seat == 0 ? hud_state.life_icons[0] : hud_state.mp_life_icons[size_t(seat - 1)][0];
        const auto* vm = app.animations.registry.find(handle);
        std::printf("# restore icon seat=%d handle=%u vm=%p flags=%u mode=%u color=%08x layer=%u sprite=%p pos=%g,%g sprite_size=%g,%g\n",
                    seat, handle, static_cast<const void*>(vm), vm ? vm->visual.flags : 0,
                    vm ? vm->visual.draw_mode() : 0, vm ? vm->visual.color : 0,
                    vm ? u32(vm->visual.layer) : 0, vm ? static_cast<const void*>(vm->sprite) : nullptr,
                    vm ? vm->visual.translation.x : 0, vm ? vm->visual.translation.y : 0,
                    vm ? vm->visual.sprite_size.x : 0, vm ? vm->visual.sprite_size.y : 0);
    }
    CHECK(scheduler.draw() >= 0); renderer.flush();
    for (int seat = 0; seat < 3; ++seat) {
        const u32 handle = seat == 0 ? hud_state.life_icons[0] : hud_state.mp_life_icons[size_t(seat - 1)][0];
        const auto* vm = app.animations.registry.find(handle);
        if (vm) std::printf("# quad seat=%d %g,%g -> %g,%g viewport=%u,%u,%u,%u\n", seat,
            vm->visual.quad[0].x, vm->visual.quad[0].y, vm->visual.quad[3].x, vm->visual.quad[3].y,
            renderer.viewport.x, renderer.viewport.y, renderer.viewport.width, renderer.viewport.height);
    }
    const auto ghost_vertices = graphics.vertices;
    CHECK(!ghost_vertices.empty());
    battle.seats[1].ghost = false;
    app.run->sync_mp_hud_view();
    graphics.vertices.clear();
    CHECK(scheduler.draw() >= 0); renderer.flush();
    CHECK_EQ(graphics.vertices.size(), ghost_vertices.size());
    bool faded = false, unchanged = false;
    for (size_t i = 0; i < std::min(graphics.vertices.size(), ghost_vertices.size()); ++i) {
        const u32 alive = graphics.vertices[i].color, ghost = ghost_vertices[i].color;
        if (alive == ghost) unchanged = true;
        else {
            CHECK_EQ(ghost & 0xffffffu, alive & 0xffffffu);
            CHECK_EQ(ghost >> 24, ((alive >> 24) * 140u + 127u) / 255u);
            faded = true;
        }
    }
    CHECK(faded && unchanged);
    CHECK(saved.restore(battle, app.visual, nullptr, app.run->presentation.get(), app.run.get()));
    battle.seat_huds[0].extra_lives = 2;
    CHECK(battle.rescue_seat(0, 1));
    app.run->sync_mp_hud_view();
    CHECK_EQ(app.run->mp_hud_seats_.ghost, 0u);
    CHECK(!battle.seats[1].ghost);
}

int main() { return th13test::run_all("m5.mp_hud"); }
