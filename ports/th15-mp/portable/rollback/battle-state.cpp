#include "battle-digest.hpp"
#include "multiplayer/ItemFrameState.hpp"
#include "multiplayer/BulletFrameState.hpp"
#include "game/EnemyCallbacks.hpp"
#include "game/RecordStore.hpp"
#include "game/BulletArea.hpp"
#include "game/ItemCheckpoint.hpp"
#include <cstddef>
#include <limits>
static void conservative_contact_bounds(){
    PlayerCollision left,right;left.position={-150,0};left.hitbox_min={-153,-3};left.hitbox_max={-147,3};left.radius=3;right=left;right.position.x=150;right.hitbox_min.x=147;right.hitbox_max.x=153;
    BulletContactBounds gap;gap.include(2,right);gap.include(0,left);require(gap.last==2&&gap.active==5,"contact union lost seat order");
    BulletState middle;middle.flags=2;middle.hitbox={1,1};require(!gap.may_contact(middle),"rectangle gap entered exact contact queries");middle.flags|=16;require(!gap.may_contact(middle),"circle gap entered exact contact queries");
    left.hitbox_min.x=std::numeric_limits<float>::quiet_NaN();BulletContactBounds nonfinite;nonfinite.include(0,left);require(nonfinite.may_contact(middle),"nonfinite player bounds bypassed the exact predicate");
    for(unsigned count:{1u,2u,3u})for(bool circle:{false,true})for(bool enlarged:{false,true})for(float radius:{0.f,3.f,20.f,128.f})for(float scale:{.2f,1.f,3.5f}){
        BulletState bullet;bullet.flags=2|64|(circle?16:0);bullet.hitbox={radius,float(radius*.5f)};bullet.scale=scale;
        PlayerCollision player;player.position={10,-15};player.hitbox_min={-2,-3};player.hitbox_max={2,3};player.radius=3;player.enlarged=enlarged;player.size_multiplier=2.5f;
        std::array<PlayerCollision,3> players{player,player,player};players[1].position={-120,70};players[1].hitbox_min={-123,67};players[1].hitbox_max={-117,73};players[1].radius=9;
        players[2].position={120,70};players[2].hitbox_min={117,67};players[2].hitbox_max={123,73};players[2].enlarged=!enlarged;
        BulletContactBounds bounds;for(unsigned s=0;s<count;++s)bounds.include(s,players[s]);require(bounds.active==((1u<<count)-1),"contact union lost an active seat");
        for(float x:{-200.f,-65.f,-48.f,-41.f,-40.f,-24.f,-1.f,0.f,1.f,24.f,40.f,41.f,48.f,65.f,200.f})for(float y:{-200.f,-65.f,-48.f,-41.f,-40.f,-24.f,-1.f,0.f,1.f,24.f,40.f,41.f,48.f,65.f,200.f}){
            bullet.motion.position={x,y,0};for(unsigned s=0;s<count;++s)if(bullet.contact(players[s],false)!=PlayerContact::none)require(bounds.may_contact(bullet),"broad bounds rejected an exact hit or graze");
        }
        bullet.motion.position={10000,10000,0};require(!bounds.may_contact(bullet),"far bullet did not bypass contact queries");
        bullet.motion.position.x=std::numeric_limits<float>::quiet_NaN();require(bounds.may_contact(bullet),"nonfinite contact must use the exact predicate");
    }
}
static void contact_cache_hit(RetailAssets& assets){
    World world(assets,3,0,{0,1,3});auto& battle=*world.battle;auto& scene=*battle.bullet_scene;auto& manager=scene.manager;
    for(auto& pilot:battle.pilots){pilot->player->motion.position={140,400,0};pilot->player->motion.position_fixed={17920,51200};}
    BulletShooter shot;shot.count=shot.rows=1;shot.speed=0;shot.position={-150,100,0};require(manager.emit(shot,0),manager.error);const auto far=manager.first_active();
    shot.count=2;shot.position={0,100,0};require(manager.emit(shot,0),manager.error);for(unsigned n=0;n<20;++n)world.step({});
    auto* bullet=manager.state(far);require(bullet&&manager.active(far)&&bullet->phase==1,"contact cache fixture did not mature");bullet->graze_color=0xff112233;
    auto& player=*battle.pilots[0]->player;player.motion.position={0,100,0};player.motion.position_fixed={0,12800};player.life.state=1;player.life.invulnerability.set(0);world.step({});
    require(player.life.state==4,"contact cache fixture did not hit its first player");const auto* view=scene.viewpoint_graze(far);
    require(manager.active(far)&&view&&view->flags==0&&view->color==0,"far bullet reused a dead player's cached geometry or graze view");
}
static void canonical_digest_padding(){
    const auto check=[](auto first,size_t begin,size_t end){auto second=first;require(begin<end,"canonical padding fixture has no padding");
        for(size_t i=begin;i<end;++i){reinterpret_cast<u8*>(&first)[i]=0x11;reinterpret_cast<u8*>(&second)[i]=0x22;}
        require(std::memcmp(&first,&second,sizeof first)!=0,"canonical padding fixture has identical bytes");
        Digest a,b;a.add(first);b.add(second);require(a.value==b.value,"canonical state digest depends on padding bytes");
    };
    check(MultiplayerInput{},offsetof(MultiplayerInput,pause)+sizeof(bool),sizeof(MultiplayerInput));
    check(PlayerCollision{},offsetof(PlayerCollision,bomb_active)+sizeof(bool),offsetof(PlayerCollision,state));
    check(SessionState{},offsetof(SessionState,replay)+sizeof(bool),offsetof(SessionState,checkpoint_power));
    BulletCancellationRewards first,second;std::memset(&first,0x11,sizeof first);std::memset(&second,0x22,sizeof second);
    first.count=second.count=37;first.spell_active=second.spell_active=true;
    Digest a,b;a.add(first);b.add(second);require(a.value==b.value,"canonical reward digest depends on padding bytes");
    ReimuOrb orb,copy;std::memset(&orb,0x11,sizeof orb);std::memset(&copy,0x22,sizeof copy);
    orb.animation=copy.animation=17;orb.motion=copy.motion=MotionState{};orb.active=copy.active=1;orb.age=copy.age=Timer{};orb.displacement=copy.displacement=Vec3{};orb.target=copy.target=19;orb.index=copy.index=2;orb.damage_source=copy.damage_source=23;orb.archived=copy.archived=false;
    for(size_t i=offsetof(ReimuOrb,archived)+sizeof(bool);i<sizeof(ReimuOrb);++i){reinterpret_cast<u8*>(&orb)[i]=0x11;reinterpret_cast<u8*>(&copy)[i]=0x22;}
    require(std::memcmp(&orb,&copy,sizeof orb)!=0,"Reimu orb padding fixture has identical object bytes");
    Digest c,d;c.add(orb);d.add(copy);require(c.value==d.value,"canonical Reimu orb digest depends on padding bytes");
    copy.damage_source=24;Digest changed;changed.add(copy);require(changed.value!=c.value,"canonical Reimu orb digest omitted a logical field");
}
static void record_codec_reuse(){
    auto source=std::make_unique<RecordStore>(),origin=std::make_unique<RecordStore>(),reused=std::make_unique<RecordStore>();Rng random{417,0,0};std::vector<u8> bytes;
    source->reset(random);source->characters[0].modes[0].plays=3;require(source->save(bytes),source->error);*origin=*source;*reused=*source;
    source->characters[0].modes[0].plays=7;source->music[1]=1;source->name[0]='A';*reused=*source;
    require(reused->characters[0].modes[0].plays==7&&reused->music[1]==1&&reused->name[0]=='A',"retained logical record writes were skipped");
    require(reused->saved_file().characters==origin->saved_file().characters,"unchanged codec image differs during logical edits");
    require(source->save(bytes),source->error);*reused=*source;*source=*origin;
    source->characters[0].modes[0].plays=11;require(source->save(bytes),source->error);*reused=*source;
    const auto equal=[](const ScoreFile& a,const ScoreFile& b){return a.characters==b.characters&&a.settings==b.settings&&a.file_header()==b.file_header()&&a.error==b.error;};
    require(equal(reused->saved_file(),source->saved_file())&&reused->saved_file().characters!=origin->saved_file().characters,"codec identity reused an abandoned future");
    require(!source->open(nullptr,0),"invalid codec input accepted");*reused=*source;require(equal(reused->saved_file(),source->saved_file())&&reused->error==source->error,"codec failure state was not captured");
    source->reset(random);const auto& saved=origin->saved_file();require(source->import_blocks(saved.characters[0].data(),saved.settings.data()),source->error);*reused=*source;
    require(equal(reused->saved_file(),source->saved_file()),"imported codec image was skipped");
}
static void authored_bullet_callbacks(RetailAssets& assets){
    World world(assets,2,0,{0,1,0});world.step({});auto& battle=*world.battle;auto& manager=battle.bullet_scene->manager;
    const auto emit=[&](float x,i32 limit,float speed){BulletShooter shot;shot.count=shot.rows=1;shot.position={x,100,0};shot.speed=speed;shot.transforms[0].type=32768;shot.transforms[0].integers[0]=limit;require(manager.emit(shot,0),manager.error);return manager.first_active();};
    const auto source=emit(0,1,2),near=emit(10,0,3),far=emit(100,0,4);
    const auto& read=static_cast<const BulletManager&>(manager);const auto velocity=read.state(source)->motion.velocity;
    BattleFrameState saved;require(saved.capture(battle),saved.error);const auto before=digest(world);
    const auto run=[&](){
        inherit_nearby_bullet_motion(manager);require(read.state(near)->step_limit==2&&read.state(far)->step_limit==0,"authored inheritance changed its radius or ordering");
        require(read.state(near)->motion.velocity.x==velocity.x&&read.state(near)->motion.velocity.y==velocity.y,"near bullet did not inherit source motion");
        PlayerCollision player;player.position={read.state(source)->motion.position.x,read.state(source)->motion.position.y};
        scale_bullets_near_players(manager,&player,1,.5f,false);
        require(read.state(source)->motion.velocity.x==float(velocity.x*.5f)&&read.state(source)->motion.velocity.y==float(velocity.y*.5f),"authored velocity scaling changed");
        require(cancel_unmarked_bullets(*battle.bullet_scene,{0,100,0},1000),battle.bullet_scene->error);
    };
    run();const auto expected=digest(world);require(saved.restore(battle),saved.error);require(digest(world)==before,"callback writes escaped the battle snapshot");run();require(digest(world)==expected,"authored callback resimulation differs");
}
static void cold_bullet_reuse(RetailAssets& assets){
    World world(assets,2,0,{0,1,0});auto& scene=*world.battle->bullet_scene;auto& manager=scene.manager;
    th15::multiplayer::BulletFrameState origin,branch;
    require(origin.capture(scene),origin.error);
    for(unsigned n=0;n<8;++n){
        BulletShooter shot;shot.count=1;shot.rows=1;shot.speed=float(n+1);shot.position={float(n*7),100,0};
        shot.transforms[17].payload={u8(n+1),0};
        require(manager.emit(shot,0),manager.error);const auto id=manager.first_active();require(id!=BulletManager::none,"spawn versioned bullet");
        manager.recycle(id);require(branch.capture(scene),branch.error);
        const auto& read=static_cast<const BulletManager&>(manager);Digest expected;expected.add(*read.state(id));
        require(origin.restore(scene),origin.error);require(branch.restore(scene),branch.error);
        Digest actual;actual.add(*read.state(id));require(actual.value==expected.value,"cold bullet lost an abandoned future's version");
        require(origin.restore(scene),origin.error);
    }
    // A mutable pool API can hand out a reference that outlives one capture.
    auto* retained=manager.state(0);retained->transform_sound=19;retained->transforms[3].payload={19};
    require(branch.capture(scene),branch.error);retained->transform_sound=37;retained->transforms[3].payload={37,38};
    require(branch.capture(scene),branch.error);retained->transform_sound=99;retained->transforms[3].payload={99};
    require(branch.restore(scene),branch.error);require(retained->transform_sound==37&&retained->transforms[3].payload==std::vector<u8>({37,38}),"retained mutable cold bullet reference was skipped");
    auto* visual=scene.visual(0);visual->body.visual.layer=17;visual->body.visual.translation={19,23,0};visual->rate=.375f;
    require(branch.capture(scene),branch.error);visual->body.visual.layer=99;visual->body.visual.translation={91,92,0};visual->rate=.75f;
    require(branch.restore(scene),branch.error);
    require(visual->body.visual.layer==17&&visual->body.visual.translation.x==19&&visual->body.visual.translation.y==23&&visual->rate==.375f,"retained mutable cold visual binding was skipped");
}
static void cold_item_reuse(RetailAssets& assets){
    World world(assets,2,0,{0,1,0});auto& manager=*world.battle->items;const auto& read=static_cast<const ItemManager&>(manager);
    th15::multiplayer::ItemFrameState origin,reused;require(origin.capture(manager),origin.error);
    u32 id=0;
    for(i32 kind:{1,2,1,2}){
        require(origin.restore(manager),origin.error);id=manager.spawn(kind,{30,120,0},.3f,1);require(id&&manager.release(id),manager.error);
        require(reused.capture(manager),reused.error);require(origin.restore(manager),origin.error);require(reused.restore(manager),reused.error);
        require(read.find(id)->state.kind==kind&&!read.find(id)->state.state,"cold item reused an abandoned future");
    }
    ItemCheckpoint chapter(manager);require(chapter.capture(),chapter.error);
    require(manager.spawn(1,{80,60,0},.6f,2)==id&&manager.release(id),manager.error);require(reused.capture(manager),reused.error);
    require(chapter.restore(),chapter.error);require(reused.capture(manager),reused.error);
    require(origin.restore(manager),origin.error);require(reused.restore(manager),reused.error);
    require(read.find(id)->state.kind==2,"cold item chapter restore reused a stale slot version");
    // Const pool observation still exposes the shared VM. Its image is never skipped.
    auto body=read.find(id)->body;body->variables.position={19,23,0};require(reused.capture(manager),reused.error);
    body->variables.position={37,41,0};require(reused.capture(manager),reused.error);body->variables.position={99,99,0};
    require(reused.restore(manager),reused.error);require(body->variables.position.x==37&&body->variables.position.y==41,"cold item VM mutation was skipped");
    reused.discard();require(reused.capture(manager),reused.error);require(manager.reset(),manager.error);require(reused.restore(manager),reused.error);
    require(read.find(id)->body==body&&read.find(id)->state.kind==2,"discarded cold item binding was not reacquired");
    auto* retained=manager.find(id);retained->state.kind=7;retained->arrow_ended=true;require(reused.capture(manager),reused.error);
    retained->state.kind=4;retained->arrow_ended=false;require(reused.capture(manager),reused.error);retained->state.kind=15;retained->arrow_ended=true;
    require(reused.restore(manager),reused.error);require(retained->state.kind==4&&!retained->arrow_ended,"retained mutable cold item reference was skipped");
    reused.discard();retained->state.kind=3;require(reused.capture(manager),reused.error);retained->state.kind=15;require(reused.restore(manager),reused.error);
    require(retained->state.kind==3,"discarded item snapshot reused an empty image");
    require(origin.restore(manager),origin.error);std::vector<u32> ids;
    for(unsigned n=0;n<70;++n)for(i32 kind:{1,9}){const auto next=manager.spawn(kind,{float(n),120,0},.3f,1);require(next!=0,manager.error);ids.push_back(next);}
    require(reused.capture(manager,false),reused.error);
    for(size_t n=0;n<ids.size();n+=3)require(manager.release(ids[n]),manager.error);
    require(reused.capture(manager,false),reused.error);std::vector<ItemState> expected;for(auto index:ids)expected.push_back(read.find(index)->state);
    require(manager.reset(),manager.error);require(reused.restore(manager),reused.error);
    for(size_t n=0;n<ids.size();++n){Digest a,b;a.add(expected[n]);b.add(read.find(ids[n])->state);require(a.value==b.value,"item page boundary or active mask lost a slot");}
    require(manager.reset(),manager.error);const auto ordinary=manager.spawn(1,{0,100,0},0,0);require(ordinary!=0,manager.error);u32 late=0;i32 initial_delay=0;
    auto& pilot=*world.battle->pilots[0];ItemFrameContext context{pilot.player->motion,pilot.player->frame.bounds,pilot.score,*pilot.collection,world.visual};context.player_state=2;
    manager.multiplayer_context=[&](u32 index,const ItemState&)->ItemFrameContext*{if(index+1==ordinary&&!late){late=manager.spawn(9,{0,100,0},0,0);require(late!=0,manager.error);initial_delay=read.find(late)->state.delay;}return &context;};
    require(manager.update(context),manager.error);manager.multiplayer_context={};require(late&&read.find(late)->state.delay==initial_delay-1,"a newly populated later item page was skipped in the same update");
}
static void same_tick_movement(RetailAssets& assets){
    for(unsigned count:{2u,3u})for(unsigned seat=0;seat<count;++seat){
        World world(assets,count,seat,{0,1,3});auto& player=*world.battle->pilots[seat]->player;
        std::array<MultiplayerInput,3> lanes{};const float start=player.motion.position.x;
        lanes[seat].held=0x80;world.step(lanes);const float right=player.motion.position.x;
        require(right>start,"local movement waited for a second logical frame");
        lanes[seat].held=0x40;world.step(lanes);require(player.motion.position.x<right,"local reversal waited for a second logical frame");
    }
}
// A free cancellation slot keeps its old body until the delayed falling
// transition binds a new script. Standalone snapshots must preserve that VM.
static void cancel_item_reuse(RetailAssets& assets){
    World world(assets,2,0,{0,1,0});auto& manager=*world.battle->items;
    const auto id=manager.spawn(9,{30,120,0},.3f,1);require(id&&manager.falling(id),manager.error);
    auto* slot=manager.find(id);Digest before;before.vm(*slot->body);require(manager.release(id),manager.error);
    th15::multiplayer::ItemFrameState saved;require(saved.capture(manager),saved.error);
    require(manager.spawn(10,{80,60,0},.6f,2)==id,"reuse the same free cancellation slot");require(manager.falling(id),manager.error);
    slot->body->variables.position={99,88,77};const float rate=.375f;slot->body->timer.tick(&rate);
    require(saved.restore(manager),saved.error);Digest restored;restored.vm(*manager.find(id)->body);
    require(restored.value==before.value,"cold cancellation VM was not fully restored");
    require(manager.spawn(9,{30,120,0},.3f,1)==id,"restored cancellation free-list order");
    require(manager.find(id)->state.state==5,"cancellation pickup starts in delayed state");
    Digest delayed;delayed.vm(*manager.find(id)->body);require(delayed.value==before.value,"delayed pickup lost its retained VM");
}
int main(int argc,char** argv){
    require(argc==2,"Pass TH15_TEST_ARCHIVE");conservative_contact_bounds();canonical_digest_padding();record_codec_reuse();RetailAssets assets(argv[1]);contact_cache_hit(assets);authored_bullet_callbacks(assets);cancel_item_reuse(assets);cold_bullet_reuse(assets);cold_item_reuse(assets);same_tick_movement(assets);unsigned replayed=0;double capture_ms=0,restore_ms=0;unsigned captures=0;
    struct Case {unsigned players,local,stage;std::array<int,3> selection;};
    for(const Case config:std::array<Case,3>{{{3,2,1,{0,1,2}},{3,2,1,{3,3,0}},{2,1,7,{2,2,0}}}}){
        World world(assets,config.players,config.local,config.selection,false,config.stage);auto& b=*world.battle;EnemySpawnRequest main;main.routine="main";require(b.spawn(main),b.error);
        BattleFrameState saved;
        for(unsigned frame=0;frame<720;){
            for(unsigned n=0;n<48&&frame<720;++n,++frame)world.step(input(frame));
            if(frame>=720)break;
            b.items->spawn(2,{-90,150,0},.3f,1);b.items->spawn(9,{70,100,0},.4f,1);
            auto start=std::chrono::steady_clock::now();require(saved.capture(b),saved.error);capture_ms+=std::chrono::duration<double,std::milli>(std::chrono::steady_clock::now()-start).count();++captures;
            const auto before=digest(world);const auto identity=AnmVm::presentation_counter;
            std::array<u64,12> expected{};for(unsigned n=0;n<12;++n){world.step(input(frame+n));expected[n]=digest(world);}
            start=std::chrono::steady_clock::now();require(saved.restore(b),saved.error);restore_ms+=std::chrono::duration<double,std::milli>(std::chrono::steady_clock::now()-start).count();
            // An abandoned future uses different movement and edge-triggered
            // bombs. Its allocations, damage and RNG must all disappear.
            for(unsigned n=0;n<12;++n){auto wrong=input(frame+n+99);if(n==0)for(auto& lane:wrong)lane.held|=2;world.step(wrong);}
            require(saved.restore(b),saved.error);
            require(digest(world)==before,"Full battle differs immediately after restore at "+std::to_string(frame));require(AnmVm::presentation_counter==identity,"capture/restore changed presentation identity");
            for(unsigned n=0;n<12;++n){world.step(input(frame+n));require(digest(world)==expected[n],"Full battle resimulation mismatch at "+std::to_string(frame+n));++replayed;}
            frame+=12;
        }
        saved.discard();
    }
    std::printf("PASS: %u exact retail battle resimulations; mean capture %.3f ms, restore %.3f ms\n",replayed,capture_ms/captures,restore_ms/captures);
    return 0;
}
