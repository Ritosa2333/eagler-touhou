// Real Stage 1 MSG -> RunScene -> MsgPresentation -> ANM file routing.
// Covers all four character scripts; GPU/font output is checked in-browser.
#include "../anm/anm_test_fixtures.hpp"
#include "game/RunScene.hpp"
#include <set>
#include <cstdlib>

using namespace th13;

namespace {
struct Graphics final : ZunGraphics {
    touhou::graphics::PipelineState state;
    std::set<std::string> textures;
    touhou::graphics::PipelineState& pipeline()override{return state;}
    u32 texture(const AnmResource& file,u32 index)override {
        textures.insert(file.textures[index].name);return index+1;
    }
    void bind_texture(u32)override{}
    void set_layout(touhou::graphics::VertexLayout)override{}
    void set_matrix(touhou::graphics::MatrixKind,const Matrix4&)override{}
    void triangles(u32,const void*,u32)override{}
    void primitives(touhou::graphics::Topology,u32,const void*,u32)override{}
    bool select_target(const AnmResource*,u32)override{return true;}
    bool clear_target(u32,const GraphicsViewport*)override{return true;}
    bool clear_depth(const GraphicsViewport*)override{return true;}
    void set_viewport(const GraphicsViewport&)override{}
};

struct Host final : RunSceneServices {
    AnmManager& animations;Graphics& graphics;
    th13test::TestFileReader reader;
    int lines=0,boss_music=0,clears=0;
    std::vector<i32> sounds;
    Host(AnmManager& a,Graphics& g):animations(a),graphics(g){}
    bool read_asset(const std::string& name,std::vector<u8>& bytes,bool=true)override {
        return reader.read("artifacts/reference/raw/"+name,bytes);
    }
    bool load(i32 bank,const std::string& name) {
        std::vector<u8> bytes;
        const bool ok=read_asset(name,bytes)&&animations.load(bank,bytes.data(),u32(bytes.size()));
        if(!ok)std::printf("load %s: %s\n",name.c_str(),animations.error.c_str());
        return ok;
    }
    bool preload_scenery_animation(i32 bank,const std::string& name)override{return load(bank,name);}
    ZunGraphics& stage_graphics()override{return graphics;}
    bool request_pause()override{return true;}
    bool request_game_over()override{return true;}
    bool request_destination(i32)override{return true;}
    bool record_stage_clear()override{++clears;return true;}
    bool hud_sound(i32 id)override{sounds.push_back(id);return true;}
    bool life_hud(i32,i32)override{return true;}
    bool bomb_hud(i32,i32)override{return true;}
    bool camera(DrawCamera,bool)override{return true;}
    bool hud_text(const HudTextDraw&)override{return true;}
    bool queue_music(i32 code,i32 value,const std::string&)override {
        if(code==2&&value==1)++boss_music;return true;
    }
    bool dialogue_line(i32,const MsgLine&,float,float)override{++lines;return true;}
};

void dialogue(i32 character,bool bomb_run=false,bool hud_run=false,i32 stage=1) {
    GameRandom rng{0};AnmEnvironment env;AnmManager animations{rng,env};
    Graphics graphics;AnmRenderer renderer{graphics};Host host{animations,graphics};
    CHECK(host.load(0,"text.anm"));
    CHECK(host.load(2,"ascii.anm"));
    CHECK(animations.sprite_fallback(2));
    for(const auto& bank:std::vector<std::pair<i32,std::string>>{
            {3,"front.anm"},
            {5,"bullet.anm"},{8,"effect.anm"},{9,"astral.anm"}})
        CHECK(host.load(bank.first,bank.second));
    CHECK(host.load(6,"pl0"+std::to_string(character)+".anm"));
    AnmResource merged;
    const std::string prefix="st0"+std::to_string(stage);
    std::vector<u8> ecl;CHECK(host.read_asset(prefix+".ecl",ecl));EclResource resource;
    CHECK(resource.open(ecl.data(),u32(ecl.size())));
    for(const auto& name:resource.animations) {
        std::vector<u8> bytes;CHECK(host.read_asset(name,bytes));
        CHECK(merged.append(bytes.data(),u32(bytes.size())));
    }
    CHECK(animations.adopt(4,std::move(merged)));
    SessionState progress;progress.character=character;progress.stage=stage;
    HudPlayerState player;HudScoreState score;FrameScheduler scheduler;
    RunScene scene{progress,player,score,animations,renderer,env,host,scheduler,
                   {4,5,6,8,9},3,2,7,prefix+".std"};
    scene.battle_sht="pl0"+std::to_string(character)+"a.sht";
    CHECK(scene.initialize());
    if(!scene.error.empty()){std::printf("initialize: %s\n",scene.error.c_str());return;}
    GameInput input;scene.controls(input);
    if(stage==4) {
        for(int frame=0;frame<400;++frame){
            CHECK(scene.update());CHECK(animations.update());
            if(!scene.error.empty()){std::printf("stage4: %s\n",scene.error.c_str());return;}
            for(u32 layer=0;layer<33;++layer)CHECK(renderer.draw_layer(animations.registry.layer(layer)));
            renderer.flush();
        }
        size_t count=0;
        for(auto* vm:animations.registry.layer(2))if(vm->geometry.wind){
            ++count;CHECK_EQ(vm->source_script,136);CHECK_EQ(vm->geometry.wind->vertices.size(),64u);
        }
        CHECK(count>0&&count<=25);
        CHECK(graphics.textures.count("effect/efflinegoast.png")>0);
        return;
    }
    if(hud_run) {
        // Isolated original Boss entry, not a claim of natural stage completion.
        auto* boss=scene.battle.sim.spawn_enemy("Boss",144,-16,40,nullptr,false);
        CHECK(boss);if(!boss)return;
        for(int frame=0;frame<180;++frame){
            CHECK(scene.update());CHECK(animations.update());
            if(!scene.error.empty()){std::printf("boss HUD: %s\n",scene.error.c_str());return;}
            for(u32 layer=0;layer<33;++layer)if(!renderer.draw_layer(animations.registry.layer(layer))){
                CHECK(false);std::printf("boss draw layer%u: %s\n",layer,renderer.error.c_str());return;
            }
            renderer.flush();
        }
        CHECK_EQ(boss->spell_base_life,11300);
        CHECK(scene.hud.boss_hud.timer_visible());
        CHECK(scene.hud.boss_hud.timer_seconds()>30&&scene.hud.boss_hud.timer_seconds()<35);
        CHECK_EQ(scene.hud.boss_hud.timer_centiseconds(),scene.battle.sim.boss_timer_centiseconds);
        CHECK(scene.hud.boss_hud.ring_arc_handle()!=0);
        CHECK_EQ(scene.battle.sim.boss_remaining_stars,2); // Easy/Normal Boss1
        bool star68=false,star69=false;
        for(u32 i=0;i<animations.registry.ordered_count(false);++i)
            if(auto* vm=animations.registry.find(animations.registry.ordered_handle(false,i)))
                if(vm->resource==animations.resource(3)){
                    star68|=vm->source_script==68;star69|=vm->source_script==69;
                }
        CHECK(star68&&star69);
        CHECK(graphics.textures.count("front/lifebar.png")!=0);
        return;
    }
    if(bomb_run) {
        const int stock=player.bombs;
        bool saw_background=false,saw_attack=false;
        constexpr i32 backgrounds[]={26,31,27,25};
        constexpr i32 durations[]={200,301,261,261};
        for(int frame=0;frame<durations[character]+60;++frame) {
            input.held=input.pressed=frame==0?2:0;
            CHECK(scene.update());
            if(!scene.error.empty()){std::printf("bomb: %s\n",scene.error.c_str());return;}
            CHECK(animations.update());
            if(character==2&&std::getenv("TH13_BOMB_GEOMETRY_TRACE")&&frame<=261) {
                auto bits=[](float value){u32 out;std::memcpy(&out,&value,4);return out;};
                const auto center=scene.battle.bomb_center();
                std::printf("S %d %u %u",frame,bits(center.x),bits(center.y));
                for(const auto& ray:scene.battle.bomb_rays)
                    std::printf(" %u %u %u %u %u",bits(ray.origin.x),bits(ray.origin.y),
                                bits(ray.angle),bits(ray.center.x),bits(ray.center.y));
                std::printf("\n");
            }
            int ray_count=0;
            for(u32 layer=0;layer<33;++layer)for(auto* vm:animations.registry.layer(layer)) {
                if(vm->resource!=animations.resource(6)||vm->creation_parent)continue;
                if(vm->source_script==backgrounds[character])saw_background=true;
                const int attack=character==0?18:character==3?19:23;
                if(vm->source_script==attack) {
                    saw_attack=true;++ray_count;
                    if(character==2&&scene.battle.bomb_timer>0) {
                        bool match=false;
                        for(const auto& ray:scene.battle.bomb_rays)
                            if(vm->visual.translation.x==ray.origin.x&&
                               vm->visual.translation.y==ray.origin.y&&
                               vm->variables.rotation.z==ray.angle)match=true;
                        CHECK(match);
                        CHECK_EQ(animations.registry.children(*vm).size(),3u);
                    }
                }
            }
            if(character==2&&scene.battle.bomb_timer>0)CHECK_EQ(ray_count,5);
            if((character==1||character==2)&&frame>=durations[character]+45)CHECK_EQ(ray_count,0);
            if(frame==0) {
                CHECK_EQ(player.bombs,stock-1);
                CHECK_EQ(scene.battle.bomb_age,0);
                CHECK_EQ(scene.battle.bomb_generation,1u);
            }
            for(u32 layer=0;layer<33;++layer)CHECK(renderer.draw_layer(animations.registry.layer(layer)));
            renderer.flush();
        }
        CHECK(saw_background);CHECK(saw_attack);
        CHECK_EQ(scene.battle.bomb_timer,0);
        CHECK_EQ(std::count(host.sounds.begin(),host.sounds.end(),0x31),1);
        if(character==2)CHECK_EQ(std::count(host.sounds.begin(),host.sounds.end(),0x1b),1);
        return;
    }
    for(i32 entry=0;entry<2;++entry) {
        scene.open_dialog(entry);
        CHECK(scene.dialog_active());
        const int previous_lines=host.lines;
        bool saw_portrait=false,saw_name=false,saw_ecl_release=false;
        for(int frame=0;frame<2400&&scene.dialog_active();++frame) {
            input.held=input.pressed=frame%30==0?1:0;
            input.duration[0]=input.held?1:0;
            if(!scene.update()){CHECK(scene.error.empty());std::printf("update: %s\n",scene.error.c_str());return;}
            CHECK(animations.update());
            saw_ecl_release|=scene.dialog_wait_released();
            for(u32 layer=0;layer<33;++layer)for(auto* vm:animations.registry.layer(layer)) {
                if(!vm->visible||!vm->texture_sprite()||!(vm->visual.color>>24))continue;
                const auto* source=vm->sprite_resource?vm->sprite_resource:vm->resource;
                const auto& name=source->textures[vm->texture_sprite()->texture].name;
                if(name.find("face/enemy1/face01")!=std::string::npos)saw_portrait=true;
                if(name=="face/enemy1/ename01.png")saw_name=true;
                CHECK(renderer.draw(*vm)!=-2); // -1 is a normal culled/hidden VM.
            }
            renderer.flush();
        }
        CHECK(!scene.dialog_active());
        CHECK(host.lines>previous_lines);
        CHECK(saw_portrait);
        if(entry==0){CHECK(saw_name);CHECK(saw_ecl_release);CHECK_EQ(host.boss_music,1);}
    }
    CHECK_EQ(host.clears,1);
}

TEST(reimu_stage1_boss_dialogue) {dialogue(0);}
TEST(marisa_stage1_boss_dialogue) {dialogue(1);}
TEST(sanae_stage1_boss_dialogue) {dialogue(2);}
TEST(youmu_stage1_boss_dialogue) {dialogue(3);}
TEST(reimu_bomb_real_animation_lifecycle) {dialogue(0,true);}
TEST(marisa_bomb_real_animation_lifecycle) {dialogue(1,true);}
TEST(sanae_bomb_real_animation_lifecycle) {dialogue(2,true);}
TEST(youmu_bomb_real_animation_lifecycle) {dialogue(3,true);}
TEST(stage1_original_boss_scripts_drive_production_hud) {dialogue(0,false,true);}
TEST(stage4_original_maple_enemy_creates_and_draws_wind_trails) {dialogue(0,false,false,4);}
}
int main(){return th13test::run_all("stage1-boss-dialogue");}
