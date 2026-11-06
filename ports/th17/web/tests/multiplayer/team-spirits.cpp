#include "../../cpp/game/ItemSpirits.hpp"
#include "../../cpp/game/EnemyCallbacks.hpp"
#include "../../cpp/game/LaserScene.hpp"
#include "../../cpp/multiplayer/FrameImage.hpp"
#include <array>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <iterator>

namespace {
using namespace th17;

void require(bool value, const char* message) {
    if (!value) {
        std::cerr << message << '\n';
        std::exit(1);
    }
}

struct Host final : ItemSpiritHost {
    unsigned collected = 0, activated = 0;
    bool options_changed() override { return true; }
    bool notice(i32) override { return true; }
    bool sound(i32 id, bool) override {
        collected += id == 63;
        activated += id == 77;
        return true;
    }
    bool popup(const Vec3&, i32, u32) override { return true; }
    bool life_hud(i32, i32) override { return true; }
    bool bomb_hud(i32, i32) override { return true; }
    bool trophy(i32) override { return true; }
    bool cancel_bullets(const Vec3&, float, i32) override { return true; }
    bool cancel_lasers(const Vec3&, float, i32, bool) override { return true; }
    bool active_player_shot() const override { return false; }
};

void load_bank(AnmManager& animations, const char* asset) {
    if (asset) {
        const auto load = [&](i32 bank, const std::filesystem::path& name) {
            std::ifstream source(name, std::ios::binary);
            const std::vector<u8> bytes{std::istreambuf_iterator<char>(source), {}};
            return animations.load(bank, bytes.data(), u32(bytes.size()));
        };
        require(load(2, std::filesystem::path(asset).parent_path() / "ascii.anm"), "load ascii.anm");
        require(animations.sprite_fallback(2), "configure fallback sprites");
        require(load(30, asset), "load beast.anm");
        return;
    }
    // A frozen script keeps the game VM and handle lifetime active without
    // requiring original artwork in the native test.
    AnmResource beast;
    beast.scripts.resize(150);
    for (auto& script : beast.scripts) {
        script.bytes = {2,0,8,0,0,0,0,0,255,255,8,0,0,0,0,0};
    }
    require(animations.load(30, std::move(beast)), "load test animation bank");
}

void shared_fragments(unsigned seats) {
    AnmEnvironment env;
    Rng random;
    AnmManager animations(random, env);
    load_bank(animations, nullptr);
    EffectManager effects(animations, 30);
    Host host;
    ItemScoreState score;
    std::array<PlayerMotion,3> motion{};
    std::array<PlayerBounds,3> bounds{};
    std::array<PlayerLifeSession,3> stocks{};
    std::array<i32,3> tiers{};
    auto items=std::make_unique<ItemManager>(animations,effects,30);
    items->sound=items->immediate_sound=[](i32){return true;};
    for(unsigned seat=0;seat<seats;++seat){
        motion[seat].position={-120.f+120.f*seat,260,0};
        const auto p=motion[seat].position;
        bounds[seat].item_near={{p.x-8,p.y-8,0},{p.x+8,p.y+8,0}};
    }
    stocks[0].extra_lives=8;stocks[0].bomb_pieces=2;
    stocks[1].life_pieces=2;
    stocks[2].bombs=8;
    ItemCollection one(stocks[0],score,motion[0],host,0,&tiers[0]);
    ItemCollection two(stocks[1],score,motion[1],host,1,&tiers[1]);
    ItemCollection three(stocks[2],score,motion[2],host,2,&tiers[2]);
    ItemFrameContext first{motion[0],bounds[0],one,random,1,5,0,1};
    ItemFrameContext second{motion[1],bounds[1],two,random,1,5,1,1};
    ItemFrameContext third{motion[2],bounds[2],three,random,1,5,2,1};
    const std::array<ItemFrameContext*,2> partners{&second,seats==3?&third:nullptr};
    const auto pickup=[&](i32 kind){
        const auto id=items->spawn(kind,motion[1].position,0,0);
        require(id!=0,"spawn cooperative item");
        if(!items->update(first,partners))require(false,items->error.c_str());
        require(!items->find(id)->state.state,"cooperative item collected once");
    };
    pickup(4);
    require(stocks[0].extra_lives==8&&stocks[0].bombs==4&&stocks[0].bomb_pieces==0,"full life overflow is local");
    require(stocks[1].extra_lives==3&&stocks[1].life_pieces==0&&tiers[1]==1,"partner earns life independently");
    if(seats==3)require(stocks[2].life_pieces==1&&stocks[2].bombs==8,"third pilot receives life fragment");
    pickup(6);
    require(stocks[0].bomb_pieces==1&&stocks[1].bomb_pieces==1,"both pilots receive bomb fragment");
    if(seats==3)require(stocks[2].bomb_pieces==0,"full bomb stock discards its own fragment");
    pickup(1);
    require(stocks[0].power==100&&stocks[1].power==100+i32(seats)&&stocks[2].power==100,
            "small P increases only the collector by the seat count");
}

void shared_inventory(const char* asset) {
    AnmEnvironment env;
    Rng visual;
    AnmManager animations(visual, env);
    load_bank(animations, asset);
    EffectManager effects(animations, 30);
    PlayerMotion motion;
    PlayerBounds bounds;
    PlayerLifeSession life;
    PlayerSpellStatus spell;
    DamageSources damage;
    ItemScoreState score;
    Host host;
    auto spirits = std::make_unique<ItemSpirits>(animations, effects, env.game_rng, visual, motion,
        bounds, life, motion.barrier_timer, spell, damage, score, host, 0, 30, 30);

    motion.position = {-96, 256, 0};
    AnimalSpiritTargets team{{{motion.position, true}, {{96, 256, 0}, true}, {{0, 256, 0}, true}}};
    const std::array<int, 5> seats{1, 0, 2, 1, 2};
    multiplayer::FrameImage image;
    u32 last_spirit=0;
    for (unsigned slot = 0; slot < seats.size(); ++slot) {
        const auto& player = team[seats[slot]];
        last_spirit=spirits->spirits.spawn(1, player.position, 0, false, true);
        require(last_spirit != 0, "spawn spirit");
        if(slot+1==seats.size()){
            image.save(env,visual,life,spell,damage);
            spirits->save_rollback(image);
            require(animations.save_rollback(image),"capture animal and HUD animation graph");
        }
        if (!spirits->update_spirits({}, team)) {
            std::cerr << spirits->roaring.error << '\n' << animations.error << '\n';
            require(false, spirits->error.c_str());
        }
        require(spirits->roaring.state.count == i32(slot + 1), "all seats add to one inventory");
    }
    require(host.collected == 5 && host.activated == 1, "five pickups activate one Roaring mode");
    require(spirits->roaring.state.kind == 1 && spirits->roaring.state.flags & 2, "shared mode active");
    const auto time = spirits->roaring.state.countdown.current;
    if (!spirits->update_spirits({}, team)) require(false, spirits->error.c_str());
    require(spirits->roaring.state.countdown.current == time - 1, "team countdown advances once");
    require(image.restore(),"restore shared inventory before activation");
    require(spirits->roaring.state.count==4&&!(spirits->roaring.state.flags&2),"restore pre-Roaring state");
    require(spirits->spirits.find(last_spirit)!=nullptr,"restore last spirit before pickup");
    require(spirits->update_spirits({},team),"replay fifth pickup");
    require(spirits->roaring.state.countdown.current==time,"replay produces the same countdown");
}

void borrowed_animation_graph(){
    AnmRegistry registry;
    AnmVm root;
    auto* child=registry.allocate();
    const auto handle=registry.submit(*child);
    require(registry.attach(*child,root),"attach borrowed root");
    child->creation_parent=&root;
    multiplayer::FrameImage image;
    require(registry.save_rollback(image),"capture borrowed root");
    require(registry.preserve_embedded_children(root),"detach a curved laser root");
    auto* replacement=registry.allocate();
    const auto next_handle=registry.submit(*replacement);
    require(registry.attach(*replacement,root),"reuse the root in a later frame");
    require(image.restore(),"restore the prior borrowed root identity");
    require(registry.find(handle)==child&&registry.find(next_handle)==nullptr,"restore old handles");
    const auto children=registry.children(root);
    require(children.size()==1&&children[0]==child&&child->creation_parent==&root,"restore borrowed child relationship");
    require(registry.submit(*registry.allocate())==next_handle,"restore slot and generation allocation order");
}

void animation_page_rollback(){
    AnmRegistry registry;
    for(unsigned i=0;i<128;++i)
        require(registry.submit(*registry.allocate())!=0,"fill first animation page");
    multiplayer::FrameImage image;
    require(registry.save_rollback(image),"capture at animation page boundary");
    auto* vm=registry.allocate();
    const auto handle=registry.submit(*vm);
    vm->visual.translation={42,8,0};
    require(registry.find(handle)==vm&&registry.slot(*vm)==128,"allocate next animation page");
    require(image.restore()&&registry.find(handle)==nullptr,"remove abandoned animation handle");
    require(registry.allocate()==vm&&vm->visual.translation.x==0,"fresh slot reuses its stable address with initial state");
    require(registry.submit(*vm)==handle,"restore generation across animation pages");
}

void immutable_bullet_program(){
    struct Host final:BulletManagerHost,BulletFrameHost {
        BulletFrameHost& bullet(u32)override{return *this;}
        void prepare(u32,BulletFrameContext&)override{}
        void play(i32)override{}
        bool interrupt(i32)override{return true;}
        bool bind_appearance(i32,i32)override{return true;}
        bool spawn_animation()override{return true;}
        bool emit_children(const BulletShooter&)override{return true;}
        bool cancel_bullet(i32)override{return true;}
        i32 contact(BulletState&,bool)override{return 0;}
        bool animation_finished(bool)override{return false;}
        void cancellation_effect(i32,const Vec3&,const Vec3&)override{}
        void recycle()override{}
    } host;
    Rng random;
    BulletManager bullets(host);
    bullets.random=&random;
    BulletShooter shot;
    shot.rows=1;shot.count=2;shot.speed=1;
    shot.transforms[8].payload={1,2,0};
    require(bullets.emit(shot,0),"emit first two bullets");
    auto* first=bullets.state(bullets.first_active());
    auto* second=bullets.state(bullets.next_active(bullets.first_active()));
    require(first->program&&first->program==second->program,"one emission shares its program");
    first->step_limit=100;
    multiplayer::FrameImage image;
    image.save(*first);
    EnemyState redirect;
    redirect.update_rule=EnemyUpdateRule::redirect_bullets;
    redirect.integer_registers={60,1};
    require(enemy_update_rule(redirect,bullets,random),"redirect just the matching bullet");
    require(first->transform_index==8&&first->transforms()[8].type==0x20000&&second->transforms()[8].type==0,
            "redirect forks a program without modifying its other owners");
    require(image.restore()&&first->program==second->program,"rollback restores the shared program");
    shot.transforms[8].payload[0]=7;
    require(bullets.emit(shot,0),"emit edited program");
    require(first->transforms()[8].payload[0]==1&&bullets.state(bullets.first_active())->transforms()[8].payload[0]==7,
            "editing the shooter preserves existing bullets and payloads");
}

void laser_restore(const char* asset){
    struct Host final:LaserSceneHost {
        PlayerCollision player;
        const PlayerCollision& player_collision()const noexcept override{return player;}
        void hit()override{}
        void sound(i32)override{}
        void laser_sound(i32,bool)override{}
        void cancellation_effect(i32,const Vec3&,const Vec3&)override{}
        void graze_spark(const Vec3&)override{}
        void graze()override{}
        void graze_resonance(float)override{}
        void item(i32,const Vec3&,float,float)override{}
    } host;
    AnmEnvironment env;Rng visual;
    AnmManager animations(visual,env);
    EffectManager effects(animations,7);BulletCancellationRewards rewards;
    LaserScene scene(host,animations,effects,env.game_rng,visual,rewards,7);
    SegmentedLaserRequest segment;segment.id=100;segment.length=64;
    require(scene.create_segmented(segment)!=0,"create segmented laser");
    u32 moving_id=0,stationary_id=0,curve_id=0,child_handle=0;
    AnmVm* curve_root=nullptr;
    if(asset){
        load_bank(animations,asset);
        std::ifstream file(std::filesystem::path(asset).parent_path()/"bullet.anm",std::ios::binary);
        const std::vector<u8> bytes{std::istreambuf_iterator<char>(file),{}};
        if(!animations.load(7,bytes.data(),u32(bytes.size())))require(false,animations.error.c_str());
        MovingLaserRequest moving;moving.position={5,64,0};moving.width=16;moving.maximum_length=120;moving.speed=2;
        StationaryLaserRequest stationary;stationary.id=101;stationary.position={-5,64,0};stationary.width=16;stationary.maximum_length=120;stationary.active=120;
        CurveLaserRequest curve;curve.position={0,96,0};curve.width=16;curve.speed=2;curve.type=1;curve.count=32;
        moving_id=scene.create_moving(moving);curve_id=scene.create_curved(curve);stationary_id=stationary.id;
        if(!moving_id||!curve_id||!scene.create_stationary(stationary))require(false,scene.error.c_str());
        curve_root=&scene.curve_visual(curve_id)->body;
        child_handle=animations.create(7,0);
        auto* child=animations.registry.find(child_handle);
        require(child&&animations.registry.attach(*child,*curve_root),"attach a child to the live curve");
        child->creation_parent=curve_root;
    }
    multiplayer::FrameImage image;
    scene.save_rollback(image);
    require(animations.save_rollback(image),"capture laser ownership and animation graph");
    require(scene.manager.clear(),"retire lasers on the abandoned timeline");
    require(scene.create_segmented(segment)!=0,"reuse a laser identifier on the abandoned timeline");
    require(image.restore(),"restore laser timeline");
    const auto* restored_segment=scene.segmented_motion(100);
    require(scene.manager.count()==(asset?4:1)&&restored_segment&&restored_segment->length==64,"restore laser order and state");
    if(asset){
        const auto* moving=scene.moving_motion(moving_id);
        const auto* stationary=scene.stationary_motion(stationary_id);
        require(moving&&stationary&&moving->speed==2&&stationary->active==120,
            "restore moving and stationary laser state");
        const auto* child=animations.registry.find(child_handle);
        require(scene.curve_motion(curve_id)&&child&&child->creation_parent==curve_root,
            "restore curved laser and its original embedded ANM owner");
    }
}

void team_contact(const char* asset) {
    AnmEnvironment env;
    Rng visual;
    AnmManager animations(visual, env);
    load_bank(animations, asset);
    EffectManager effects(animations, 30);
    PlayerMotion motion;
    PlayerBounds bounds;
    PlayerLifeSession life;
    PlayerSpellStatus spell;
    DamageSources damage;
    ItemScoreState score;
    Host host;
    auto spirits = std::make_unique<ItemSpirits>(animations, effects, env.game_rng, visual, motion,
        bounds, life, motion.barrier_timer, spell, damage, score, host, 0, 30, 30);

    AnimalSpiritTargets team{{{{-12,256,0},false},{{12,256,0},false},{}}};
    const auto id = spirits->spirits.spawn(1, {0,256,0}, 0, false, true);
    require(id != 0, "spawn contact test spirit");
    if (!spirits->update_spirits({}, team)) require(false, spirits->error.c_str());
    require(host.collected == 0, "inactive players cannot collect");
    const auto center = spirits->spirits.find(id)->position;
    team[0] = {{center.x - 12,center.y,0},true};
    team[1] = {{center.x + 12,center.y,0},true};
    if (!spirits->update_spirits({}, team)) require(false, spirits->error.c_str());
    require(host.collected == 1, "two contacts collect one spirit");
    const auto collected_x = spirits->spirits.find(id)->position.x;
    team[0] = {{-64,256,0},true};
    team[1] = {{collected_x,256,0},true};
    if (!spirits->update_spirits({}, team)) require(false, spirits->error.c_str());
    require(spirits->spirits.find(id)->position.x < collected_x, "tie chooses first seat and retains it");
}
}

int main(int argc, char** argv) {
    const char* asset = argc > 1 ? argv[1] : nullptr;
    shared_inventory(asset);
    team_contact(asset);
    shared_fragments(2);
    shared_fragments(3);
    borrowed_animation_graph();
    animation_page_rollback();
    immutable_bullet_program();
    laser_restore(asset);
    std::cout << "TH17 spirit inventory, countdown, fragments and laser rollback passed\n";
}
