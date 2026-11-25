#include "encounter_scene.hpp"
#include <iostream>
#include <source_location>

using namespace soew;

static unsigned checks=0;
static void check(bool condition,std::source_location at=std::source_location::current()){
    ++checks;
    if(!condition)throw std::runtime_error("Extra road check failed at line "+std::to_string(at.line()));
}
static void trace(std::uint64_t &hash,unsigned n){
    for(unsigned i=0;i<4;++i){hash^=(n>>(i*8))&255u;hash*=1099511628211ull;}
}
static void test_assets(const std::filesystem::path &assets){
    struct Asset {const char *name;std::uint64_t hash;};
    constexpr Asset original[]={
        {"STAGE5.DT1",0xc8490167a11ff024ull},{"STAGE5.MAP",0xc2f6bce961fa45f4ull},{"STAGE5.MPN",0x9aae8e91782108eeull},
        {"STAGE5.BFT",0xb68d784fac095443ull},{"STAGE5.BMT",0xf1fcaed72ce56f11ull},{"STAGE5.BBT",0x4fd271abd77caa38ull},
        {"STAGE5.TXT",0x9aaebfc1bb00a957ull},{"STAGE5B1.BFT",0xd27f8465cce1ebb4ull},{"STAGE5B2.BFT",0xdc1eeb38df6424b9ull},
        {"STAGE5.M",0xe84578cf84ce22c2ull},{"BOSS5.M",0xaed6258d56ef75ecull},
        {"STAGE5.MMD",0xb1de97491e5a0e57ull},{"BOSS5.MMD",0x4161dd9ecb808f03ull}
    };
    for(const auto &a:original){
        std::uint64_t hash=14695981039346656037ull;
        for(auto value:StageScene::read(assets/a.name)){hash^=std::to_integer<unsigned>(value);hash*=1099511628211ull;}
        if(hash!=a.hash)throw std::runtime_error(std::string("Original Extra resource fingerprint mismatch: ")+a.name);
        check(hash==a.hash);
    }
}
static void test_ring(){
    StageBackground bg;
    bg.map={0,0,0,1};
    bg.tiles.resize(3);
    bg.tiles[0].fill(1);bg.tiles[1].fill(2);bg.tiles[2].fill(3);
    bg.sections.fill(0);bg.sections[8*24]=2;
    StageTileRing ring;ring.reset(bg);
    check(ring.front_page==1&&ring.ring[6].map_row==17);
    check(ring.pixel(0,96,96)==1&&ring.pixel(1,96,96)==1);
    ring.set_and_put_both(bg,104,99,1);
    check(ring.ring[6].cells[4]==1&&ring.ring[6].cells[5]==0);
    for(unsigned p=0;p<2;++p){
        check(ring.pixel(p,103,96)==1&&ring.pixel(p,104,96)==2&&ring.pixel(p,119,96)==2&&ring.pixel(p,120,96)==1);
        check(ring.pixel(p,104,95)==1&&ring.pixel(p,104,112)==1);
    }
    ring.restore_cell(bg,0,6,4);
    check(ring.pixel(0,96,96)==2&&ring.pixel(1,96,96)==1);
    for(const auto &r:std::array<EnemyTileRequest,4>{{{40,400,0},{31,0,0},{33,0,0},{40,0,3}}})
        check(ring.write_error(bg,r.left,r.vram_y,r.image)!=nullptr);
    check(ring.writes==1);
    ring.set_and_put_both(bg,32,399,1);
    ring.scroll_both(bg,399,15,24,true);
    // Same frame: AF puts whole tile first; scroll installs the next generation and blits one line.
    check(ring.ring[24].map_row==24&&ring.ring[24].generation==1&&ring.ring[24].cells[0]==2);
    check(ring.pixel(0,32,0)==3&&ring.pixel(1,32,399)==2);
    // Next step uses the reload cache even after another update to the same ring cell.
    ring.set_and_put_both(bg,32,397,1);
    ring.scroll_both(bg,398,14,24,false);ring.flip(0);
    check(ring.front_page==0&&ring.ring[24].cells[0]==1&&ring.ring[24].generation==1);
    check(ring.pixel(0,32,0)==3&&ring.pixel(0,32,1)==2&&ring.pixel(1,32,399)==2);
}
static void test_midboss(){
    ExtraMidbossCore boss;
    RandomRing rng;IRand seed(1234);rng.fill(seed);
    BulletCore bullets(4);
    EnemyEffectSink sink;
    unsigned f=0,clock=0,accepted_pellets=0,clipped=0;
    std::array<unsigned,3> groups{};
    std::array<unsigned,6> images{};
    std::uint64_t events=14695981039346656037ull;
    sink.tile=[&](const EnemyTileRequest &r){
        unsigned p=0;while(p<ExtraMidbossCore::images.size()&&ExtraMidbossCore::images[p]!=r.image)++p;
        check(p<images.size());++images[p];
        if(f==23)check(r.left==128&&r.vram_y==128&&r.image==1);
        trace(events,f);trace(events,unsigned(r.left));trace(events,r.vram_y);trace(events,r.image);return true;
    };
    sink.bullet=[&](const EnemyBulletRequest &r){
        const unsigned i=r.group==0x21?0:r.group==0x24?1:2;
        check(r.kind==EnemyBulletRequest::Pellet&&r.angle==0&&r.speed==100&&r.group==std::array{0x21,0x24,0x25}[i]);
        if(f==63)check(r.left==132&&r.top==163);
        ++groups[i];
        trace(events,f);trace(events,unsigned(r.left));trace(events,unsigned(r.top));trace(events,r.group);
        auto before=bullets.spawned;
        check(bullets.add(r,clock&1,208,336,rng));
        if(r.top>383){check(bullets.spawned==before);++clipped;}
        accepted_pellets+=unsigned(bullets.spawned-before);return true;
    };
    boss.trigger(199);check(!boss.started);boss.trigger(200);
    for(f=1;f<=421;++f){
        clock=f-1;
        const int line=(400-int(clock/2))%400;
        bullets.begin_frame(clock&1);
        boss.update(line,sink);
        bullets.update(clock&1,208,336,clock);
        if(f==1)check(boss.active&&boss.frame==1&&boss.scheduled==0);
        if(f==96)check(boss.active&&boss.frame==96&&boss.scheduled==17&&boss.distance==0);
        if(f==192)check(boss.active&&boss.frame==192&&boss.scheduled==34&&boss.distance==0);
        if(f==288)check(boss.active&&boss.frame==288&&boss.scheduled==51&&boss.distance==80);
        if(f==420)check(boss.active&&boss.frame==420&&boss.expired==51);
    }
    check(!boss.active&&boss.finished&&boss.frame==0&&boss.accepted==51);
    boss.trigger(200);check(!boss.active);
    check(images==std::array<unsigned,6>{51,51,51,51,51,51});
    std::cout<<"midboss event hash="<<std::hex<<events<<std::dec<<" clipped="<<clipped<<" pellets="<<accepted_pellets<<'\n';
    check(groups==std::array<unsigned,3>{17,17,17}&&clipped==0&&accepted_pellets==462&&rng.cursor==0);
    check(events==0x5535af52de57d594ull);
    ExtraMidbossCore clipped_boss;clipped_boss.trigger(200);
    clipped_boss.schedule(128,-1,0x21,0,100);
    const auto before=bullets.spawned;
    for(unsigned i=0;i<48;++i)clipped_boss.update(0,sink);
    check(clipped_boss.expired==1&&bullets.spawned==before&&clipped==1&&rng.cursor==0);
    ExtraMidbossCore full;
    for(unsigned i=0;i<31;++i)check(full.schedule(128,i==0?-1:400,0x21,0,100)==(i<30));
    check(full.tiles[0].y==399&&full.tiles[1].y==0&&full.scheduled==31&&full.accepted==30);
}
static void test_real_map(const std::filesystem::path &assets){
    auto scene=EncounterScene::load(assets,5,0,4,2,1);
    const auto first_path=scene.script.enemies.at(7).path_slot;
    check(scene.script.paths.at(first_path).bytes[0]==0xaf);
    {
        auto strict=scene;
        for(unsigned i=0;i<256;++i)strict.tick(0);
        check(strict.ticks==255&&strict.runtime.blocked&&strict.runtime.blocked->type==7&&strict.runtime.blocked->result.opcode==0xaf&&strict.runtime.blocked->result.pc==0);
    }
    {
        auto invalid=scene;invalid.enable_extra_road();
        invalid.script.paths.at(first_path).bytes.at(2)=0xfe;
        for(unsigned i=0;i<256;++i)invalid.tick(0);
        check(invalid.ticks==255&&invalid.runtime.blocked&&invalid.runtime.blocked->result.pc==0&&invalid.runtime.blocked->result.reason=="tile image outside MPN");
        check(invalid.extra_tiles.writes==0);
    }
    {
        auto order=scene;order.enable_extra_road();
        // Inject a matching trigger into a fixture, not the original script.
        order.script.formations.clear();order.script.declared_formations=2;
        StageScript::Formation row;row.selectors.fill(255);row.selectors[9]=7;row.trigger=200;
        order.script.formations.push_back(row);row.trigger=201;order.script.formations.push_back(row);
        for(unsigned i=0;i<3220;++i){
            order.tick(0);
            if(order.extra_midboss.frame==1)check(order.runtime.spawns==1&&order.runtime.dispatch.suppressed==0);
        }
        check(order.runtime.dispatch.rows==2&&order.runtime.spawns==1&&order.runtime.dispatch.suppressed==1);
    }
    check(scene.script.formations.at(195).trigger==635&&scene.script.formations.at(196).trigger==592);
    check(scene.script.formations.at(203).trigger==599&&scene.script.formations.at(204).trigger==592);
    scene.enable_extra_road();
    std::vector<std::uint32_t> pixels(640*400),again(pixels.size());
    std::uint64_t events=14695981039346656037ull;
    unsigned stall_ticks=0,started_tick=0,ended_tick=0;
    for(unsigned i=0;i<12000&&!scene.runtime.scroll.done&&!scene.runtime.blocked;++i){
        const auto tiles=scene.extra_tiles.writes;
        const auto dispatched=scene.runtime.dispatch.rows;
        const auto spawned=scene.bullet_core.spawned;
        const auto random_before=scene.runtime.random.cursor;
        const auto active=scene.extra_midboss.active;
        scene.tick(0); // No player protection used; survival handling is disabled in this structural run.
        if(dispatched==196){
            ++stall_ticks;check(scene.runtime.dispatch.cursor==196);
            if(scene.runtime.enemies.high_water==0)check(scene.runtime.random.cursor==random_before);
        }
        if(i==254)check(scene.extra_tiles.writes==0);
        if(i==255){
            std::cout<<"first AF writes="<<scene.extra_tiles.writes<<" cell="<<unsigned(scene.extra_tiles.ring[6].cells[4])<<" line="<<scene.extra_tiles.display_line<<'\n';
            check(scene.extra_tiles.writes==1&&scene.extra_tiles.ring[6].cells[4]==1);
            check(scene.extra_tiles.front_page==1&&scene.extra_tiles.display_line==272);
            const auto &palette=scene.background.palette[scene.background.tiles[1][0]];
            const auto rgb=(unsigned(palette[0])<<16)|(unsigned(palette[1])<<8)|palette[2];
            scene.render(pixels);scene.render(again);
            check(pixels==again&&pixels[224*640+96]==rgb);
        }
        if(!active&&scene.extra_midboss.active){started_tick=unsigned(scene.ticks);check(scene.scroll.progress==200);}
        if(active&&!scene.extra_midboss.active)ended_tick=unsigned(scene.ticks);
        if(tiles!=scene.extra_tiles.writes||spawned!=scene.bullet_core.spawned||dispatched!=scene.runtime.dispatch.rows||active!=scene.extra_midboss.active){
            trace(events,unsigned(scene.ticks));trace(events,scene.extra_tiles.writes);trace(events,unsigned(scene.bullet_core.spawned));
            trace(events,unsigned(scene.runtime.dispatch.rows));trace(events,scene.runtime.random.cursor);trace(events,scene.extra_midboss.frame);
        }
    }
    if(scene.runtime.blocked){const auto &b=*scene.runtime.blocked;std::cerr<<"road blocked tick="<<scene.ticks<<" type="<<b.type<<" pc="<<b.result.pc<<" opcode="<<unsigned(b.result.opcode)<<" reason="<<b.result.reason<<'\n';}
    std::cout<<"road event hash="<<std::hex<<events<<std::dec<<" ticks="<<scene.ticks<<" starts="<<started_tick<<" ends="<<ended_tick
        <<" writes="<<scene.extra_tiles.writes<<" rows="<<scene.runtime.dispatch.rows<<" reloads="<<scene.extra_tiles.reloads
        <<" spawn="<<scene.runtime.spawns<<" bullets="<<scene.bullet_core.spawned<<" rng="<<unsigned(scene.runtime.random.cursor)<<'\n';
    check(scene.runtime.scroll.done&&!scene.runtime.blocked);
    check(scene.extra_midboss.finished&&scene.extra_midboss.expired==51&&started_tick==3200&&ended_tick==3620);
    check(scene.runtime.dispatch.cursor==196&&stall_ticks>0&&scene.runtime.dispatch.suppressed==0);
    check(scene.ticks==11009&&scene.scroll.display_pixels==5504&&scene.extra_tiles.reloads==344);
    check(events==0x040b16c4c0c4cc09ull);
    check(scene.extra_tiles.front_page==scene.rendered_page&&scene.runtime.page==(scene.rendered_page^1u));
    check(scene.extra_tiles.display_line==scene.scroll.line);
}
static void test_road_combat(const std::filesystem::path &assets){
    for(int shot=0;shot<3;++shot){
        auto s=EncounterScene::load(assets,5,shot,4,2,1);
        s.enable_extra_road();s.combat_enabled=s.survival_enabled=s.pickup_enabled=true;
        s.enable_effects();s.enable_bombing();
        for(unsigned i=0;i<12000&&!s.runtime.scroll.done&&!s.runtime.blocked&&!s.weapon_blocked;++i){
            // Refresh protection only for this progression test, not a survival claim.
            s.life.invincibility=2;
            auto input=Shot|(i%240<90?Left:i%240>=120&&i%240<210?Right:0);
            if(i==3200)input|=Bomb;
            s.tick(std::uint16_t(input));
        }
        if(s.runtime.blocked)std::cerr<<"combat road blocked: "<<s.runtime.blocked->result.reason<<'\n';
        check(s.runtime.scroll.done&&!s.runtime.blocked&&!s.weapon_blocked);
        check(s.extra_midboss.finished&&s.shots.fired>0&&s.bomb.stage_used==1&&s.life.stage_misses==0);
        std::cout<<"road combat ship="<<shot<<" ticks="<<s.ticks<<" shots="<<s.shots.fired<<" score="<<s.score_core.score<<'\n';
    }
}
int wmain(int argc,wchar_t **argv){try{
    if(argc!=2)throw std::runtime_error("Usage: extra-road-test <original-assets>");
    test_assets(argv[1]);test_ring();test_midboss();test_real_map(argv[1]);test_road_combat(argv[1]);
    std::cout<<"extra_road_checks="<<checks<<" failures=0\n";
    return 0;
}catch(const std::exception &e){std::cerr<<e.what()<<'\n';return 1;}}
