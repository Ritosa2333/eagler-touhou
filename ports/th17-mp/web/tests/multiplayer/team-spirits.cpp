#include "../../cpp/game/ItemSpirits.hpp"
#include "../../cpp/game/EnemyCallbacks.hpp"
#include "../../cpp/game/EnemyAnmHost.hpp"
#include "../../cpp/game/LaserScene.hpp"
#include "../../cpp/game/Hud.hpp"
#include "../../cpp/game/SpellRecords.hpp"
#include "../../cpp/game/AnmCoordinates.hpp"
#include "../../cpp/game/PlayerShots.hpp"
#include "../../cpp/game/PlayerShooting.hpp"
#include "../../cpp/multiplayer/FrameImage.hpp"
#include "../../cpp/multiplayer/AnmCheckpoint.hpp"
#include <algorithm>
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

void stage_six_enemy_fade(const char* asset) {
    if(!asset)return;
    const auto folder=std::filesystem::path(asset).parent_path();
    const auto read=[&](const char* name){
        std::ifstream file(folder/name,std::ios::binary);
        return std::vector<u8>{std::istreambuf_iterator<char>(file),{}};
    };
    const auto atlas=read("enemyb.anm"),script=read("st06.ecl");
    EclProgram ecl;
    require(ecl.attach(script.data(),u32(script.size()),nullptr)>=0,"load retail Stage 6 ECL");
    EclContext context;
    require(context.bind(ecl,ecl.find_index("ECir00")),"find Stage 6 yin-yang orb");
    AnmEnvironment env;
    Rng rng;
    AnmManager manager(rng,env);
    require(manager.load(2,atlas.data(),u32(atlas.size())),"load retail orb atlas");
    EnemyState enemy;
    EnemyAnmHost host(manager);
    EnemyVisualCommands commands(enemy);
    commands.host=&host;
    for(u32 offset=0;offset<context.code_size;offset+=context.instruction->length){
        context.instruction=reinterpret_cast<const EclInstruction*>(context.code+offset);
        const auto op=context.instruction->opcode;
        if(op==23)break;
        if(op>=302&&op<=336)require(commands.execute(context,op)==0,"execute retail orb animation setup");
    }
    auto* vm=manager.registry.find(enemy.animation_handles[0]);
    require(vm&&vm->sprite&&vm->source_script==53&&!(vm->visual.color>>24),"orb starts with its native sprite and zero opacity");
    require(manager.update(false),"advance cached retail orb curves");
    require(manager.update(false),"advance retail orb fade a second frame");
    require((vm->visual.color>>24)>0&&vm->visual.scale.x>.5f,"Stage 6 orb becomes visible and grows");
}

void shared_fragments(unsigned seats, i32 difficulty) {
    AnmEnvironment env;
    Rng random;
    AnmManager animations(random, env);
    load_bank(animations, nullptr);
    EffectManager effects(animations, 30);
    Host host;
    ItemScoreState score;
    score.difficulty=difficulty;
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
    for(auto& stock:stocks)stock.enable_challenge();
    pickup(6);pickup(7);
    for(const auto& stock:stocks)require(!stock.bombs&&!stock.bomb_pieces,"challenge discards Bomb stock and pieces");
    const i32 initial_power=difficulty==4?350:100;
    for(auto& stock:stocks)stock.power=initial_power;
    pickup(1);
    require(stocks[0].power==initial_power&&stocks[1].power==initial_power+i32(seats)&&stocks[2].power==initial_power,
            "small P increases only the collector by the seat count");
    stocks[1].power=400;
    stocks[2].power=400;
    const auto forwarded=items->spawn(1,motion[1].position,0,0);
    require(forwarded!=0&&items->update(first,partners),"full pilot forwards Power");
    require(items->find(forwarded)->state.state==3&&stocks[1].power==400,
            "forwarded Power stays on the field and homes to the recipient");
    for(unsigned frame=0;frame<120&&items->find(forwarded)->state.state;++frame)
        require(items->update(first,partners),"forwarded Power approaches the teammate");
    require(!items->find(forwarded)->state.state&&stocks[0].power==initial_power+i32(seats),
            "underpowered teammate receives the room-sized Power value");

    // P1 and P3 are closer to the gift than P2 during the flight.
    motion[2].position=motion[0].position;
    bounds[2].item_near=bounds[0].item_near;
    stocks[0].extra_lives=2;
    stocks[0].power=100;
    stocks[1].power=100;
    stocks[2].power=100;
    const auto send_gift=[&](ItemKind kind){
        const auto id=items->spawn(i32(kind),motion[0].position,0,0);
        require(id!=0,"spawn a teammate gift");
        auto& item=items->find(id)->state;
        item.gift_seat(2);
        item.state=3;
        item.attraction_speed=5;
        for(unsigned frame=0;frame<120&&item.state;++frame)
            require(items->update(first,partners),"gift chases its assigned teammate");
        require(!item.state,"assigned teammate collected the gift");
    };
    send_gift(ItemKind::Life);
    require(stocks[0].extra_lives==2&&stocks[1].extra_lives==4&&stocks[2].extra_lives==2,
            "only the assigned teammate receives the gifted life");
    send_gift(ItemKind::LargePower);
    require(stocks[0].power==100&&stocks[1].power==200&&stocks[2].power==100,
            "gift is worth one full Power and cannot be intercepted");
}

void marisa_laser_modes(const char* asset){
    if(!asset)return;
    const auto directory=std::filesystem::path(asset).parent_path();
    const auto read=[&](const char* name){
        std::ifstream file(directory/name,std::ios::binary);
        return std::vector<u8>{std::istreambuf_iterator<char>(file),{}};
    };
    AnmEnvironment env;
    Rng random,visual;
    AnmManager animations(visual,env);
    load_bank(animations,asset);
    const auto atlas=read("pl01.anm");
    require(animations.load(10,atlas.data(),u32(atlas.size())),"load Marisa animations");
    for(auto* name:{"pl01a.sht","pl01b.sht","pl01c.sht"}){
        const auto data=read(name);
        ShtResource resource;
        require(resource.open(data.data(),u32(data.size())),"load Marisa shot table");
        PlayerMotion motion;
        motion.option_count=4;
        PlayerShotContext context{motion};
        context.power=400;
        context.cooperative=true;
        DamageSources damage;
        PlayerShots shots(resource,context,damage,animations,random,visual,10,30);
        shots.presentation_seat=2;
        const auto lasers=[&]{
            return std::count_if(shots.shots.begin(),shots.shots.end(),[&](const auto& shot){
                return shot.state&&shots.specification(shot.specification)->type==2;
            });
        };
        const auto fire=[&]{
            for(i32 frame=0;frame<15;++frame)require(shots.fire(frame,frame),"fire native shot group");
        };
        fire();
        require(lasers()==4,"four ordinary beams at full power");
        require(shots.synchronize_roaring(2,3)&&lasers()==0,"Eagle Roaring retires ordinary beams");
        fire();
        require(lasers()==5,"five Eagle beams use the released slots");
        auto beam=std::find_if(shots.shots.begin(),shots.shots.end(),[&](const auto& shot){
            return shot.state&&shots.specification(shot.specification)->type==2;
        });
        auto* beam_vm=animations.registry.find(beam->animation);
        require(beam_vm&&beam_vm->pilot_draw_owner==2&&beam_vm->pilot_effect,
                "Eagle laser uses its shooter's effect fade");
        require(animations.retire(beam->animation)&&shots.update()&&lasers()==4,"lost animation releases its beam");
        fire();
        require(lasers()==5,"lost beam can respawn");
        require(shots.synchronize_roaring(6,3)&&lasers()==0,"Roaring break clears Eagle beams");
        fire();
        require(lasers()==4,"ordinary beams return after Roaring break");
        require(shots.synchronize_roaring(2,1)&&lasers()==0,"Wolf Roaring clears ordinary beams");
        fire();
        require(lasers()==0,"Wolf shots contain no lasers");
        for(auto& shot:shots.shots)require(animations.retire(shot.animation),"retire fixture animation");
        require(animations.update(false),"release fixture animations");
    }
}

void all_player_shot_effects(const char* asset){
    if(!asset)return;
    const auto directory=std::filesystem::path(asset).parent_path();
    const auto read=[&](const std::string& name){
        std::ifstream file(directory/name,std::ios::binary);
        return std::vector<u8>{std::istreambuf_iterator<char>(file),{}};
    };
    AnmEnvironment environment;
    Rng random,visual;
    AnmManager animations(visual,environment);
    load_bank(animations,asset);
    unsigned checked=0;
    for(i32 character=0;character<3;++character){
        const auto prefix="pl0"+std::to_string(character);
        const i32 bank=10+character;
        const auto atlas=read(prefix+".anm");
        require(animations.load(bank,atlas.data(),u32(atlas.size())),"load original player animation bank");
        u8 seat=u8(character+1);
        const auto check_effects=[&]{
            require(animations.registry.count()>0,"player emits visible-effect animations");
            for(bool alternate:{false,true})for(u32 index=0;index<animations.registry.ordered_count(alternate);++index){
                const auto* vm=animations.registry.find(animations.registry.ordered_handle(alternate,index));
                require(vm&&vm->pilot_effect&&vm->pilot_draw_owner==seat,
                        "player shot, child and detached effect keep the shooter's seat");
            }
            animations.retire_resource(bank);
            require(animations.update(false)&&animations.update(true)&&!animations.registry.count(),
                    "clear tested player effects");
        };
        for(i32 animal=0;animal<3;++animal){
            const auto bytes=read(prefix+char('a'+animal)+".sht");
            ShtResource resource;
            require(resource.open(bytes.data(),u32(bytes.size())),"load original player shot table");
            // All power levels of unfocused, focused, Wolf and Eagle shots.
            // Otter and mixed Roaring use the ordinary shot groups.
            for(u32 group=0;group<resource.groups.size();++group){
                const auto& specs=resource.groups[group];
                if(specs.empty())continue;
                PlayerMotion motion;
                motion.position={0,400,0};
                motion.focus=group>=5&&group<10;
                motion.option_count=4;
                PlayerShotContext context{motion};
                context.power_step=resource.header.power_step;
                context.power=i32(group%5)*context.power_step;
                context.cooperative=true;
                DamageSources damage;
                PlayerShots shots(resource,context,damage,animations,random,visual,bank,30);
                shots.presentation_seat=seat;
                for(u32 index=0;index<specs.size();++index){
                    context.laser_power.fill(0);
                    require(shots.spawn((group<<8)|index,motion.position)>=0,shots.error.c_str());
                    const auto& shot=shots.shots[index];
                    const auto* vm=animations.registry.find(shot.animation);
                    require(shot.state&&vm&&vm->pilot_effect&&vm->pilot_draw_owner==seat,
                            "original shot entry creates a projectile with the shooter's fade");
                    ++checked;
                }
                require(animations.update(false)&&animations.update(true),animations.error.c_str());
                check_effects();
            }
            if(character==2){
                PlayerMotion motion;
                motion.focus=1;
                PlayerShootingAnmHost host(animations,bank,seat);
                host.audio=[](i32,ShotSoundAction,float){return true;};
                PlayerShooting shooting(motion,host);
                for(i32 frame=0;frame<62;++frame){
                    require(shooting.update(1,true,1,character,animal,0,0),shooting.error.c_str());
                    require(animations.update(false)&&animations.update(true),animations.error.c_str());
                }
                require(shooting.charge.current==62&&shooting.charge_handle,"Youmu completes charge animation");
                check_effects();
            }
        }
    }
    std::cout << "TH17 checked " << checked << " original shot entries and all Youmu charge variants\n";
}

void shared_otter_barriers(const char* asset) {
    AnmEnvironment env;
    Rng visual;
    AnmManager animations(visual,env);
    load_bank(animations,asset);
    EffectManager effects(animations,30);
    PlayerMotion motion;
    PlayerBounds bounds;
    PlayerLifeSession life;
    PlayerSpellStatus spell;
    DamageSources damage;
    ItemScoreState score;
    Host host;
    auto spirits=std::make_unique<ItemSpirits>(animations,effects,env.game_rng,visual,motion,
        bounds,life,motion.barrier_timer,spell,damage,score,host,0,30,30);
    require(spirits->initialize(),"initialize otter HUD");
    motion.position={-120,200,0};
    AnimalSpiritTargets team{{{motion.position,true},{{0,200,0},true},{{120,200,0},true}}};
    for(unsigned slot=0;slot<5;++slot)require(spirits->roaring.collect(2),"collect otter spirit");
    require(spirits->update_spirits({},team),"run shared otter mode");
    require(spirits->roaring.state.kind==2,"otter mode activated once");
    for(unsigned seat=1;seat<3;++seat)for(auto handle:spirits->roaring.partner_orbs[seat-1]){
        const auto* vm=animations.registry.find(handle);
        require(vm&&vm->visual.translation.x==team[seat].position.x,"otter barrier tracks its teammate");
        require(vm->pilot_effect&&vm->pilot_draw_owner==seat+1,"otter barrier fades for its own seat");
    }
    require(!animations.registry.find(spirits->roaring.visuals.inner_timer)->pilot_effect,
            "shared Roaring countdown stays legible");
    multiplayer::FrameImage image;
    spirits->save_rollback(image);
    require(animations.save_rollback(image),"capture team otter barriers");
    require(spirits->end_roaring(false)&&!spirits->roaring.partner_orbs[0][0],"team barrier retires with shared mode");
    require(image.restore()&&animations.registry.find(spirits->roaring.partner_orbs[0][0]),
            "rollback restores teammate orb handles");
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
    multiplayer::FrameImage::BufferPool buffers;
    multiplayer::FrameImage image;
    image.use_buffer_pool(buffers);
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
    multiplayer::FrameImage overlapping;
    overlapping.use_buffer_pool(buffers);
    require(registry.save_rollback(overlapping),"capture an adjacent animation frame");
    image.clear();
    require(registry.discard(*registry.find(next_handle))&&registry.save_rollback(image),"reuse cleared animation capture storage");
    require(overlapping.restore()&&registry.find(next_handle)!=nullptr,"adjacent capture survives buffer reuse");
    require(image.restore()&&registry.find(next_handle)==nullptr&&registry.find(handle)==child,
            "reused capture has the current handles and borrowed graph");
}

void animation_curve_rollback(){
    // Exercise every byte, all word alignments and both unequal directions.
    // Raw byte arrays avoid constructing an invalid bool/float representation.
    constexpr auto size=sizeof(AnmInterpolators);
    std::array<unsigned char,size+8> first_bytes{},second_bytes{};
    u32 seed=1234;
    for(auto& byte:first_bytes){seed=seed*1664525u+1013904223u;byte=u8(seed>>24);}
    for(size_t alignment=0;alignment<8;++alignment){
        auto* a=first_bytes.data()+alignment;auto* b=second_bytes.data()+alignment;
        std::memcpy(b,a,size);
        require(multiplayer::snapshot_equal<size>(a,b),"identical unaligned snapshot bytes match");
        for(size_t at=0;at<size;++at){
            b[at]^=0x80;
            require(!multiplayer::snapshot_equal<size>(a,b)&&!multiplayer::snapshot_equal<size>(b,a),
                    "snapshot comparison detects every changed byte");
            b[at]^=0x80;
        }
    }
    require(multiplayer::snapshot_equal<0>(nullptr,nullptr),"empty snapshot comparison does not load");
    require(multiplayer::snapshot_equal<1>(first_bytes.data(),first_bytes.data()),"short snapshot tail matches");
    AnmVm vm;
    vm.interpolators.mask_valid=true;
    vm.interpolators.position.control1={1,2,3};
    vm.geometry.screen_vertices.resize(2);
    vm.geometry.screen_vertices[0].color=0x80402010;
    multiplayer::AnmCheckpoint::Cache cache;
    multiplayer::AnmCheckpoint::Curves first_curves,second_curves,live_curves;
    multiplayer::AnmCheckpoint first(vm,cache,first_curves);
    const auto first_resting=cache.lock();
    multiplayer::AnmCheckpoint identical(vm,cache,second_curves);
    require(cache.lock()==first_resting&&second_curves.changing.empty(),"unchanged dormant curves share storage");
    vm.interpolators.position.control1[0]=4;
    multiplayer::AnmCheckpoint second(vm,cache,second_curves);
    require(cache.lock()!=first_resting,"different dormant controls require separate snapshots");
    vm.interpolators.position.begin(6,0,{1,2,3},{4,5,6});
    vm.interpolators.invalidate();
    vm.geometry.clear();
    multiplayer::AnmCheckpoint live(vm,cache,live_curves);
    require(live_curves.changing.size()==1,"active curves are copied into the frame");
    first.restore(vm,cache,first_curves);
    require(vm.interpolators.position.duration==0&&vm.interpolators.position.control1[0]==1&&
            vm.geometry.screen_vertices[0].color==0x80402010,"restore dormant controls and geometry");
    live.restore(vm,cache,live_curves);
    require(vm.interpolators.position.duration==6&&!vm.interpolators.mask_valid&&vm.geometry.screen_vertices.empty(),
            "restore pending interpolation and cleared geometry");
    second.restore(vm,cache,second_curves);
    require(vm.interpolators.position.duration==0&&vm.interpolators.position.control1[0]==4,
            "restore a separate dormant snapshot after changing branches");
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

void recycled_item_rollback(){
    AnmEnvironment env;
    Rng random;
    AnmManager animations(random,env);
    load_bank(animations,nullptr);
    EffectManager effects(animations,30);
    auto items=std::make_unique<ItemManager>(animations,effects,30);
    const ItemManager& view=*items;
    require(view.free_head(false)==ItemManager::ordinary_count&&view.free_head(true)==ItemManager::pool_size,
            "fresh item banks start from the highest free index");
    const u32 id=items->spawn(9,{0,100,0},0,1);
    require(id&&items->falling(id),"spawn cancellation item");
    auto* const vm=items->find(id)->body.get();
    vm->variables.position.x=11;
    require(items->release(id),"recycle cancellation item");
    multiplayer::FrameImage::BufferPool pool;
    multiplayer::FrameImage before,overlapping,active;
    for(auto* image:{&before,&overlapping,&active})image->use_buffer_pool(pool);
    items->save_rollback(before);
    items->save_rollback(overlapping);

    require(items->spawn(9,{0,120,0},0,1)==id&&items->falling(id),"reuse the original item slot");
    items->find(id)->body->variables.position.x=27;
    items->save_rollback(active);
    const auto abandoned=items->spawn(9,{0,140,0},0,1);
    require(abandoned&&abandoned<id&&items->falling(abandoned),"activate a new item slot in the discarded timeline");
    require(view.next_used(0)==abandoned-1,"visit used item slots in storage order");
    require(before.restore()&&view.find(id)->body.get()==vm&&!view.find(id)->state.state&&vm->variables.position.x==11,
            "restore an idle item without replacing its VM address");
    require(!view.find(abandoned)->state.state&&!view.find(abandoned)->body&&view.next_used(0)==id-1,
            "discard a later item and remove it from sparse traversal");
    require(active.restore()&&view.find(id)->state.state==2&&vm->variables.position.x==27,
            "restore the same item after reuse");
    require(view.free_head(true)==abandoned,"restore the untouched cancellation range below the active item");
    require(overlapping.restore()&&vm->variables.position.x==11,"overlapping idle snapshots remain immutable");
    require(view.free_head(true)==id,"restore the recycled item at the top of the free stack");
    before.clear();
    require(items->spawn(9,{0,160,0},0,1)==id&&items->falling(id),"consume the restored recycled item first");
    items->save_rollback(before);
    require(overlapping.restore()&&view.free_head(true)==id&&vm->variables.position.x==11,
            "reusing a discarded item's snapshot buffers leaves the live image unchanged");
}

void animation_page_order(){
    AnmRegistry registry;
    std::array<AnmVm*,270> nodes{};
    std::array<u32,270> handles{};
    std::array<std::vector<u32>,2> order;
    for(u32 i=0;i<nodes.size();++i){
        nodes[i]=registry.allocate();
        require(nodes[i]&&registry.slot(*nodes[i])==i,"lookup newly allocated pages");
        const bool alternate=i&1,prepend=i%5==0;
        handles[i]=registry.submit(*nodes[i],u32(alternate)*4+u32(prepend)*2);
        require(handles[i]!=0,"register multiple pages");
        auto& group=order[alternate];
        group.insert(prepend?group.begin():group.end(),handles[i]);
    }
    multiplayer::FrameImage image;
    require(registry.save_rollback(image),"capture both animation queues");
    for(u32 i=0;i<nodes.size();i+=3)require(registry.discard(*nodes[i]),"discard animations across pages");
    for(u32 i=0;i<90;++i)require(registry.submit(*registry.allocate(),i&1?4:0)!=0,"reuse pages in another order");
    require(image.restore(),"restore both animation queues");
    for(u32 i=0;i<nodes.size();++i)
        require(registry.slot(*nodes[i])==i&&registry.find(handles[i])==nodes[i],"resolve pages after rollback and reuse");
    for(u32 group=0;group<order.size();++group)
        for(u32 i=0;i<order[group].size();++i)
            require(registry.ordered_handle(group,i)==order[group][i],"restore exact prepend and append order");
}

void spell_record_rollback(){
    SpellRecords records;
    require(records.begin(0,0,false,"First spell"),"start the first spell record");
    multiplayer::FrameImage first,overlapping,second,branch;
    records.save_rollback(first);
    records.save_rollback(overlapping);
    bool all=false;
    require(records.capture(0,0,false,all),"capture before rolling back");
    require(records.begin(100,8,true,"A different loadout"),"write a later spell record");
    records.save_rollback(second);
    require(first.restore()&&records.loadouts[0][0].captures[0]==0&&
            records.aggregate[100].name.empty()&&records.loadouts[8][100].attempts[1]==0,
            "restore both personal and aggregate histories across loadouts");
    require(records.begin(0,0,false,"Replacement branch"),"start an alternate spell timeline");
    records.save_rollback(branch);
    require(overlapping.restore()&&records.aggregate[0].name=="First spell"&&
            records.loadouts[0][0].attempts[0]==1,"overlapping snapshots stay immutable");
    require(second.restore()&&records.loadouts[0][0].captures[0]==1&&
            records.aggregate[100].name=="A different loadout","restore the later snapshot again");
    require(branch.restore()&&records.aggregate[0].name=="Replacement branch"&&
            records.aggregate[100].name.empty(),"restore an independent branch");
    first.clear();overlapping.clear();second.clear();branch.clear();
    multiplayer::FrameImage fresh;
    records.save_rollback(fresh);
    require(records.capture(0,0,false,all)&&fresh.restore()&&
            records.loadouts[0][0].captures[0]==0,"capture after old snapshots are released");
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

    const auto a=bullets.first_active(),b=bullets.next_active(a);
    const auto pristine=bullets.first_free();
    bullets.recycle(a);bullets.recycle(b);
    multiplayer::FrameImage::BufferPool buffers;
    multiplayer::FrameImage free_image;
    free_image.use_buffer_pool(buffers);
    bullets.save_rollback(free_image);
    shot.count=3;
    require(bullets.emit(shot,0),"reuse two bullets and claim a pristine slot");
    require(bullets.first_active()==pristine,"allocate bullets in free-list order");
    require(free_image.restore(),"rewind mixed recycled and pristine bullets");
    require(bullets.first_free()==b&&!bullets.active(a)&&!bullets.active(pristine)&&
            bullets.state(a)->transforms()[8].payload[0]==7&&bullets.state(pristine)->phase==0,
            "restore the free-list head and both kinds of inactive state");
    require(bullets.emit(shot,0)&&bullets.first_active()==pristine&&
            bullets.next_active(pristine)==a&&bullets.next_active(a)==b,
            "replay preserves exact bullet allocation order");
    multiplayer::FrameImage later;
    later.use_buffer_pool(buffers);
    bullets.save_rollback(later);
    require(free_image.restore()&&!bullets.active(pristine),"older image restores the pristine suffix");
    require(later.restore()&&bullets.active(pristine)&&bullets.first_free()==pristine-1,
            "newer image restores a slot that the older image returned to the suffix");
    free_image.clear();
    bullets.recycle(pristine);
    bullets.save_rollback(free_image);
    require(later.restore()&&bullets.active(pristine),"adjacent bullet capture survives buffer reuse");
    require(free_image.restore()&&!bullets.active(pristine)&&bullets.first_free()==pristine,
            "reused bullet capture restores the latest free-list head");
}

void logical_animation_draw(){
    using namespace touhou::graphics;
    struct Device final:ZunGraphics {
        PipelineState state;
        u32 submissions=0, first_color=0;
        PipelineState& pipeline()override{return state;}
        u32 texture(const AnmResource&,u32)override{return 1;}
        void bind_texture(u32)override{}
        void set_layout(VertexLayout)override{}
        void set_matrix(MatrixKind,const Matrix4&)override{}
        void triangles(u32 count,const void* data,u32)override{
            submissions+=count;
            first_color=static_cast<const AnmGeometryVertex*>(data)->color;
        }
        void primitives(Topology,u32 count,const void* data,u32)override{
            submissions+=count;
            std::memcpy(&first_color,static_cast<const u8*>(data)+offsetof(AnmGeometryVertex,color),sizeof(first_color));
        }
        bool select_target(const AnmResource*,u32)override{return true;}
        bool clear_target(u32,const GraphicsViewport*)override{return true;}
        bool clear_depth(const GraphicsViewport*)override{return true;}
        void set_viewport(const GraphicsViewport&)override{}
    } device;
    AnmRenderer renderer(device);
    AnmResource resource;
    resource.sprites.emplace_back();
    AnmVm parent,vm;
    parent.visual.inherited_color=0xff404040;
    vm.creation_parent=&parent;
    vm.resource=&resource;
    vm.sprite=&resource.sprites[0];
    vm.visual.flags=3;
    vm.visual.render_flags|=0x2000000;
    vm.visual.sprite_size={32,32};
    vm.variables.position={100,100,0};
    auto presented=vm;
    renderer.logical_only=true;
    require(renderer.draw(vm)==0&&renderer.draw_screen_strip(vm,nullptr,6)==0,"logical animation and strip pass");
    renderer.flush();
    require(device.submissions==0,"logical draw does not pack vertices");
    renderer.logical_only=false;
    require(renderer.draw(presented)==0,"physical animation pass");
    renderer.flush();
    require(device.submissions==2&&vm.visual.inherited_color==presented.visual.inherited_color&&
            vm.visual.quad[0].x==presented.visual.quad[0].x&&vm.visual.quad[3].y==presented.visual.quad[3].y,
            "logical pass preserves bounds and inherited colour");
    presented.pilot_draw_owner=2;
    renderer.pilot_opacity[2]=72;
    require(renderer.draw(presented)==0,"draw nearby teammate");
    renderer.flush();
    require(device.first_color>>24==72&&presented.visual.inherited_color>>24==255,
            "teammate fade changes submitted alpha without changing the VM");
    presented.hud_viewer=2;
    require(renderer.draw(presented)==-1,"teammate hitbox is hidden");
    renderer.hud_viewer=2;
    renderer.pilot_opacity[2]=255;
    require(renderer.draw(presented)==0,"local hitbox stays visible");
    renderer.flush();
    require(device.first_color>>24==255,"local alpha is unchanged");

    presented.hud_viewer=0;
    presented.pilot_effect=true;
    presented.visual.flags|=9<<5;
    require(renderer.draw(presented)==0,"draw local MAX-blended shot");
    renderer.flush();
    require(device.state.blendEquation==BlendEquation::Maximum&&device.first_color>>24==255,
            "local effect keeps its blend and alpha");
    renderer.hud_viewer=1;
    renderer.pilot_opacity[2]=160;
    require(renderer.draw(presented)==0,"draw distant teammate's MAX-blended shot");
    renderer.flush();
    require(device.state.blendEquation==BlendEquation::Add&&
            device.state.sourceBlend==BlendFactor::SourceAlpha&&
            device.state.destinationBlend==BlendFactor::InverseSourceAlpha&&device.first_color>>24==40,
            "teammate MAX-blended effect actually uses the fade");

    renderer.pilot_opacity[2]=72;
    presented.visual.flags=3|(17u<<25)|(2u<<5);
    presented.variables.integers[0]=12;
    require(renderer.draw(presented)==0&&device.first_color>>24==18,
            "nearby Roaring ring uses effect opacity");
    presented.visual.flags=3|(9u<<25)|(1u<<5);
    presented.variables.integers[0]=2;
    presented.geometry.screen_vertices.resize(4);
    for(auto& vertex:presented.geometry.screen_vertices)vertex.color=0xffaabbcc;
    require(renderer.draw(presented)==0&&device.first_color==0x12aabbcc&&
            device.state.destinationBlend==BlendFactor::One&&
            presented.geometry.screen_vertices[0].color==0xffaabbcc,
            "Roaring break fades additive strip vertices without mutating geometry");
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

void room_hud(const char* asset) {
    if (!asset) return;
    const auto front = std::filesystem::path(asset).parent_path() / "front.anm";
    std::ifstream source(front, std::ios::binary);
    const std::vector<u8> bytes{std::istreambuf_iterator<char>(source), {}};
    struct TextCapture : HudDrawServices {
        std::vector<HudTextDraw> entries;
        bool hud_text(const HudTextDraw& entry) override { entries.push_back(entry); return true; }
        unsigned count(const std::string& value) const {
            return std::count_if(entries.begin(),entries.end(),[&](const auto& entry){return entry.text==value;});
        }
    };
    for (u32 seats : {1u,2u,3u}) {
        AnmEnvironment env;
        Rng visual;
        AnmManager animations(visual,env);
        load_bank(animations,asset);
        require(animations.load(5,bytes.data(),u32(bytes.size())),"load front.anm");
        EnemyWorldState enemies;
        PlayerMotion motion;
        PlayerSpellStatus spell;
        std::array<PlayerLifeSession,3> stock;
        stock[0].power=115;stock[1].power=237;stock[2].power=349;
        for (u32 seat=0;seat<seats;++seat) stock[seat].life_pieces=stock[seat].bomb_pieces=i32(seat);
        Hud hud(animations,enemies,stock[0],motion,spell);
        HudStartContext start;
        start.stage=7;start.screen=8;start.difficulty=4;
        require(hud.initialize(start,seats),hud.error.c_str());
        for (u32 seat=1;seat<seats;++seat) require(hud.initialize_partner(seat,i32(seat),stock[seat]),hud.error.c_str());
        for (int i=0;i<140;++i) require(animations.update(true),animations.error.c_str());
        TextCapture text;
        ItemScoreState score;score.difficulty=4;score.point_value=9000000;
        HudScoreState progress;
        const std::array<HudPilotStock,3> pilots{{{&stock[0],0},{&stock[1],9},{&stock[2],0}}};
        require(hud.draw({score,progress,678},text,seats-1,pilots),hud.error.c_str());
        require(text.count("15")==1&&text.count("37")==unsigned(seats>=2)&&text.count("49")==unsigned(seats==3),"per-seat Power");
        require(text.count("90,000")==1&&text.count("678")==1&&text.count("Graze")==0,"shared PIV and graze use native labels");
        const unsigned capped=seats>1;
        require(text.count("---/---")==capped&&text.count("/3")==seats*2-capped,"native fragment displays for every seat");
        for (u32 seat=0;seat<seats;++seat) {
            const auto tag=std::to_string(seat+1)+"P";
            const auto it=std::find_if(text.entries.begin(),text.entries.end(),[&](const auto& entry){return entry.text==tag;});
            if (seats==1) require(it==text.entries.end(),"ordinary HUD has no seat tag");
            else require(it!=text.entries.end()&&it->position.y==99.f+seat*(seats==2?120.f:108.f)&&
                (it->style.color&0xffffffu)==(seat==seats-1?0xffff80u:0xffffffu),"seat spacing and local highlight");
        }
        unsigned life=0,bomb=0,fragment=0,graze=0;
        for (auto* vm : animations.registry.layer(37)) {
            life+=vm->source_script==8;
            bomb+=vm->source_script==10;
            fragment+=vm->source_script==9||vm->source_script==11;
            if (vm->source_script==14) {
                Vec3 position;
                require(anm_position(*vm,position)&&position.y==(seats==1?226.f:seats==2?364.f:448.f),"native Graze sprite row");
                ++graze;
            }
            if (vm->source_script==30||vm->source_script==38) require(vm->hud_viewer==(seats==1?1:0),"each seat sees every room stock row");
        }
        require(life==seats&&bomb==seats&&fragment==seats*2&&graze==1,"native labels are on the HUD overlay layer");
        require(hud.result(0,1)&&hud.result_active&&hud.result_panel,"create a spell result panel");
        TextCapture logical;
        require(hud.draw({score,progress,678},logical,seats-1,pilots,false),hud.error.c_str());
        require(logical.entries.empty()&&!hud.result_active&&!hud.result_panel,
                "logical drawing retires the result panel without generating captions");
    }
}
}

int main(int argc, char** argv) {
    const char* asset = argc > 1 ? argv[1] : nullptr;
    stage_six_enemy_fade(asset);
    shared_inventory(asset);
    team_contact(asset);
    for(i32 difficulty:{1,4})for(unsigned seats:{2u,3u})shared_fragments(seats,difficulty);
    marisa_laser_modes(asset);
    all_player_shot_effects(asset);
    shared_otter_barriers(asset);
    borrowed_animation_graph();
    animation_page_rollback();
    animation_curve_rollback();
    recycled_item_rollback();
    animation_page_order();
    spell_record_rollback();
    immutable_bullet_program();
    logical_animation_draw();
    laser_restore(asset);
    room_hud(asset);
    std::cout << "TH17 spirit inventory, countdown, fragments, laser rollback and HUD passed\n";
}
