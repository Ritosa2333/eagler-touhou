#include "../../th11_web/cpp/game/GameSession.hpp"
#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <iterator>
#include <memory>
using namespace th11;
namespace {
unsigned checks=0,ticks=0;
void check(bool ok,const char* message){++checks;if(!ok){std::fprintf(stderr,"FAIL: %s\n",message);std::exit(1);}}
struct NullGraphics final:ZunGraphics {
    touhou::graphics::PipelineState state{};
    unsigned triangles_drawn=0,primitive_calls=0;
    bool capture=false;std::vector<AnmVertex> captured;
    touhou::graphics::PipelineState& pipeline()override{return state;}
    u32 texture(const AnmResource&,u32 index)override{return index+1;}
    void bind_texture(u32)override{}
    void set_layout(touhou::graphics::VertexLayout)override{}
    void set_matrix(touhou::graphics::MatrixKind,const Matrix4&)override{}
    void triangles(u32 n,const void* vertices,u32 stride)override{triangles_drawn+=n;if(capture&&stride==sizeof(AnmVertex)){const auto* source=static_cast<const AnmVertex*>(vertices);captured.insert(captured.end(),source,source+n*3);}}
    void primitives(touhou::graphics::Topology,u32,const void*,u32)override{++primitive_calls;}
    bool select_target(const AnmResource*,u32)override{return true;}
    bool clear_target(u32,const GraphicsViewport*)override{return true;}
};
struct Peer {GameResources resources;GameSession session;NullGraphics graphics;AnmRenderer renderer{graphics};explicit Peer(const std::vector<u8>& raw){check(resources.open_archive(raw.data(),u32(raw.size())),"open retail archive");}};
void step(Peer& peer,const std::array<MultiplayerInput,3>& input){if(!peer.session.update_multiplayer(input)){std::fprintf(stderr,"%s\n",peer.session.error.c_str());check(false,"native confirmed tick");}if(!peer.session.draw(peer.renderer)){std::fprintf(stderr,"draw frame=%u session=%s battle=%s code=%d\n",peer.session.multiplayer_frame,peer.session.error.c_str(),peer.session.battle->error.c_str(),peer.session.battle->last_error);check(false,"native draw transaction");}++ticks;}
void put(PlayerFrame& p,float x,float y){p.motion.state.x=i32(x*128);p.motion.state.y=i32(y*128);p.motion.state.position={x,y,0};}
u64 code_token(const EclOwner& owner,uintptr_t address){unsigned index=0;for(const auto& file:owner.program->files){++index;const auto begin=reinterpret_cast<uintptr_t>(file->bytes.data());if(address>=begin&&address<begin+file->bytes.size())return (u64(index)<<32)|(address-begin);}return address;}
void compare_owners(const GameBattle& a,const GameBattle& b){
    auto* left=a.enemies.first;auto* right=b.enemies.first;
    for(;left&&right;left=left->next,right=right->next){const auto& x=left->value->state;const auto& y=right->value->state;
        for(unsigned i=0;i<4;++i){check(x.integers[i]==y.integers[i],"direct ECL integer owner");check(float_bits(x.floats[i])==float_bits(y.floats[i])&&float_bits(x.extra_floats[i])==float_bits(y.extra_floats[i]),"direct ECL float owner");}
        const auto& p=left->value->script;const auto& q=right->value->script;auto* u=&p.threads;auto* v=&q.threads;
        for(;u&&v;u=u->next,v=v->next){const auto& m=*u->value;const auto& n=*v->value;check(m.thread_id==n.thread_id&&float_bits(m.time)==float_bits(n.time),"direct ECL thread clock");check(code_token(p,reinterpret_cast<uintptr_t>(m.instruction))==code_token(q,reinterpret_cast<uintptr_t>(n.instruction)),"direct ECL program counter");check(m.stack.top==n.stack.top&&m.stack.frame_base==n.stack.frame_base,"direct ECL stack extent");
            for(i32 offset=0;offset<m.stack.top;offset+=4){u32 first=0,second=0;std::memcpy(&first,m.stack.data+offset,4);std::memcpy(&second,n.stack.data+offset,4);check(code_token(p,first)==code_token(q,second),"direct live ECL stack value/return PC");}
        }check(!u&&!v,"direct ECL thread roster");
    }check(!left&&!right,"direct shared enemy roster");
    for(unsigned seat=0;seat<a.mp_options.seat_count;++seat){const auto& p=*a.pilots[seat];const auto& q=*b.pilots[seat];check(p.economy.lives==q.economy.lives&&p.economy.power==q.economy.power&&p.economy.score_units==q.economy.score_units,"direct per-seat economy");check(p.player->motion.state.x==q.player->motion.state.x&&p.player->motion.state.y==q.player->motion.state.y,"direct per-seat motion");}
}
void ghost_dialogue_cases(const std::vector<u8>& bytes){
    for(unsigned count:{2u,3u})for(unsigned seat=0;seat<count;++seat)for(u32 confirm:{1u,256u}){
        auto peer=std::make_unique<Peer>(bytes);MultiplayerOptions o;o.seat_count=count;
        check(peer->session.begin_multiplayer(peer->resources,o),"ghost dialogue world");auto& w=*peer->session.battle;std::array<MultiplayerInput,3> in{};
        for(unsigned n=0;n<125;++n)step(*peer,in);
        auto& host=*w.pilots[0];host.economy.lives=-1;check(host.game_over(false),"P1 becomes a native MP ghost before dialogue");
        // A real MSG wait opcode with a long deadline and native terminator.
        // Run it through the actual battle update, not a mocked key selector.
        const std::vector<u8> msg={1,0,0,0,12,0,0,0,0,0,0,0,0,0,11,4,0x58,2,0,0,0,0,0,0};
        check(w.dialogue->begin(msg,0),"native MSG confirmation fixture");step(*peer,in);check(w.dialogue->active&&w.dialogue->state.wait.current>0,"MSG waits for a real confirmed press");
        in[seat].held=confirm;step(*peer,in);
        check(!w.dialogue->active,"any seat's Shoot or Enter advances dialogue with ghost P1");
        check(host.ghost&&host.player->input.movement.held==0,"story confirmation does not restore ghost movement or shooting");
    }
    for(unsigned count:{2u,3u})for(unsigned seat=0;seat<count;++seat){
        auto peer=std::make_unique<Peer>(bytes);MultiplayerOptions o;o.seat_count=count;check(peer->session.begin_multiplayer(peer->resources,o),"all-participant ending story world");
        auto& s=peer->session;s.ending=std::make_unique<Ending>(s.animations,s.resources.core.text,s.scores);
        const u8 msg[]={0xe8,3,1,0,0xd0,7,1,0};auto& e=*s.ending;e.instruction=msg;e.end=msg+sizeof(msg);e.active=true;e.time.set(0,&s.animations.rate);e.elapsed.set(0,&s.animations.rate);e.wait.set(0,&s.animations.rate);s.state.phase=GameSessionPhase::ending;
        std::array<MultiplayerInput,3> in{};step(*peer,in);check(e.instruction==msg,"native ending waits for confirmation before future-timed text");in[seat].held=256;step(*peer,in);check(e.instruction==msg+4,"any participant's Enter advances native ending story");
    }
    for(unsigned count:{2u,3u}){
        auto peer=std::make_unique<Peer>(bytes);MultiplayerOptions o;o.seat_count=count;check(peer->session.begin_multiplayer(peer->resources,o),"dialogue transfer world");auto& w=*peer->session.battle;std::array<MultiplayerInput,3> in{};
        for(unsigned n=0;n<125;++n)step(*peer,in);
        std::vector<u8> msg={1,0,0,0,12,0,0,0,0,0,0,0};for(unsigned n=0;n<10;++n){const u8 wait[]={u8(n*2000),u8((n*2000)>>8),11,4,0x58,2,0,0};msg.insert(msg.end(),wait,wait+8);}msg.insert(msg.end(),{0,0,0,0});
        auto& ghost=*w.pilots[0];ghost.economy.lives=-1;check(ghost.game_over(false)&&w.dialogue->begin(msg,0),"ghost host and active native dialogue");auto& giver=*w.pilots[1];giver.economy.lives=3;put(*giver.player,0,400);put(*ghost.player,10,400);
        for(unsigned n=0;n<90;++n){ghost.ghost_clock=1;ghost.ghost_dx=ghost.ghost_dy=0;in[1].held=8;step(*peer,in);}
        check(w.dialogue->active&&!ghost.ghost&&giver.economy.lives==2,"90-tick life rescue works during dialogue without ending it");
        in={};step(*peer,in);put(*giver.player,0,400);put(*ghost.player,10,400);giver.economy.power=giver.economy.max_power;ghost.economy.power=0;check(giver.power_changed()&&ghost.power_changed(),"dialogue Power transfer setup");const auto before=giver.economy.power;
        for(unsigned n=0;n<5;++n){in[1].held=1;step(*peer,in);in[1].held=0;step(*peer,in);}
        check(w.dialogue->active&&giver.economy.power==before-giver.economy.power_step,"five Shoot taps transfer Power while native dialogue remains active");
        for(unsigned n=0;n<8;++n)step(*peer,in);check(ghost.economy.power==ghost.economy.power_step,"native directed Power gift is collected during dialogue");
    }
    std::puts("PASS: native MSG through real battle update, 2P/3P ghost P1, every participant Shoot/Enter, no ghost gameplay control, dialogue life rescue and Power transfers");
}
void reported_rule_cases(const std::vector<u8>& bytes){
    for(unsigned count:{2u,3u})for(unsigned victim=0;victim<count;++victim){
        auto peer=std::make_unique<Peer>(bytes);MultiplayerOptions o;o.seat_count=count;o.selections={0,3,5};
        check(peer->session.begin_multiplayer(peer->resources,o),"reported death world");auto& w=*peer->session.battle;std::array<MultiplayerInput,3> in{};
        for(unsigned n=0;n<125;++n)step(*peer,in);
        auto& dead=*w.pilots[victim];dead.economy.lives=0;dead.economy.power=0;
        check(w.mp_hit(victim,{CollisionKind::Hit,true}),"real native hit enters final death");
        for(unsigned n=0;n<41;++n)step(*peer,in);
        check(dead.ghost&&!w.game_over_requested&&peer->session.state.phase==GameSessionPhase::stage,"every seat including P1 becomes a ghost without single-player Game Over");
        peer->renderer.flush();peer->graphics.triangles_drawn=0;check(w.mp_draw_players(peer->renderer),"ghost body draw");peer->renderer.flush();check((dead.player->motion.body.flags&2)&&peer->graphics.triangles_drawn>0,"native ghost sprite stays visible");
        const auto old_body=dead.player->motion.body;dead.player->motion.body.flags&=~2u;dead.player->motion.body.color&=0xffffff;
        const auto hidden_body=dead.player->motion.body;peer->renderer.flush();peer->graphics.triangles_drawn=0;check(w.mp_draw_players(peer->renderer),"ghost ignores native hidden/zero-alpha draw flags");peer->renderer.flush();check(peer->graphics.triangles_drawn==count*2,"every ghost still submits its original sprite quad");check(std::memcmp(&hidden_body,&dead.player->motion.body,sizeof(hidden_body))==0,"ghost visibility is a draw copy, never gameplay animation mutation");dead.player->motion.body=old_body;
        const unsigned giver=(victim+1)%count;auto& donor=*w.pilots[giver];donor.economy.lives=3;
        put(*donor.player,0,400);put(*dead.player,10,400);in[giver].held=8;if(count==3){in[giver].touch_mode=2;in[giver].touch_x=0;in[giver].touch_y=400;}
        for(unsigned n=0;n<90;++n){dead.ghost_dx=dead.ghost_dy=0;dead.ghost_clock=1;step(*peer,in);}
        check(!dead.ghost&&dead.player->state.life_state==1&&donor.economy.lives==2,"all-seat actual final deaths can receive 90-tick rescue");
    }
    for(unsigned source:{0u,1u})for(int selection=0;selection<6;++selection){
        auto peer=std::make_unique<Peer>(bytes);MultiplayerOptions o;o.seat_count=3;o.selections={0,0,0};o.selections[source]=selection;
        check(peer->session.begin_multiplayer(peer->resources,o),"Bomb sharing world");auto& w=*peer->session.battle;std::array<MultiplayerInput,3> in{};
        for(unsigned n=0;n<125;++n)step(*peer,in);
        for(auto& p:w.pilots)p->player->state.invincibility.set(0,&w.animations.rate);
        w.pilots[source]->economy.power=w.pilots[source]->economy.max_power;check(w.pilots[source]->power_changed(),"Bomb Power setup");
        in[source].held=2;step(*peer,in);in={};step(*peer,in);
        for(unsigned seat=0;seat<3;++seat)if(seat!=source){const auto& p=*w.pilots[seat];check(p.player->state.invincibility.current==(selection==3||selection==5?0:selection==0?1:40),"ordinary Bomb shares native protection, Marisa A and unused shield do not");check(!p.bomb->state.active&&!(p.player->motion.state.flags&6),"shared invincibility never copies Bomb or shield mechanics");}
        if(selection==5){check(w.mp_hit(source,{CollisionKind::Hit,true})&&w.pilots[source]->tick_bomb(),"Nitori protection starts only on shield hit");for(unsigned seat=0;seat<3;++seat)if(seat!=source)check(w.pilots[seat]->player->state.invincibility.current==40,"triggered shield shares native invincibility");}
        w.pilots[(source+1)%3]->player->state.invincibility.set(500,&w.animations.rate);w.mp_share_invincibility(source,40);check(w.pilots[(source+1)%3]->player->state.invincibility.current==500,"Bomb cannot shorten existing rescue protection");
    }
    std::puts("PASS: actual 2P/3P all-seat final deaths and rescue, six native Bombs with Marisa A exception and Nitori timing");
}
void battle_cases(const std::vector<u8>& bytes){
    auto a=std::make_unique<Peer>(bytes),b=std::make_unique<Peer>(bytes);
    for(unsigned count:{2u,3u}){
        MultiplayerOptions options;options.seat_count=count;options.selections={0,3,5};options.seed=0x31725119;
        check(a->session.begin_multiplayer(a->resources,options),"begin MP P1");options.local_seat=1;check(b->session.begin_multiplayer(b->resources,options),"begin MP P2");
        check(a->session.multiplayer_seat(0).max_power==80&&a->session.multiplayer_seat(1).max_power==96,"selected SHT power systems");
        check(a->session.multiplayer_hash()==b->session.multiplayer_hash(),"local seat excluded from state identity");
        std::array<MultiplayerInput,3> input{};
        for(unsigned frame=0;frame<720;++frame){for(unsigned seat=0;seat<count;++seat){input[seat].held=1|(frame%240<60?(seat&1?64:128):frame%240<120?(seat&1?128:64):8);input[seat].touch_mode=0;}if(frame==160)input[0].pause=true;if(frame==161)input[0].pause=false;if(frame==163)input[1].pause=true;if(frame==164)input[1].pause=false;step(*a,input);step(*b,input);check(a->session.multiplayer_hash()==b->session.multiplayer_hash(),"different local seat peers remain identical after render");}
        auto& world=*a->session.battle;check(world.pilots[0]->player!=world.pilots[1]->player,"per-seat player owners");
        compare_owners(world,*b->session.battle);
        check(world.enemies.count==b->session.battle->enemies.count,"shared enemies tick once");
    }
    for(int selection=0;selection<6;++selection){MultiplayerOptions options;options.selections={selection,(selection+1)%6,0};check(a->session.begin_multiplayer(a->resources,options),"six loadout MP begin");auto& world=*a->session.battle;std::array<MultiplayerInput,3> input{};
        for(unsigned frame=0;frame<125;++frame)step(*a,input);
        for(unsigned seat=0;seat<2;++seat){auto& p=*world.pilots[seat];p.economy.power=p.economy.max_power;check(p.power_changed(),"rebuild native options");}
        input[0].held=input[1].held=2;step(*a,input);check(world.pilots[0]->bomb->state.active&&world.pilots[1]->bomb->state.active,"independent simultaneous native bombs");
        input={};for(unsigned frame=0;frame<45;++frame)step(*a,input);
    }
    std::puts("PASS: 2P/3P deterministic confirmed ticks, six loadouts, simultaneous bombs");
}
void rules_cases(const std::vector<u8>& bytes){
    auto peer=std::make_unique<Peer>(bytes);MultiplayerOptions options;options.seat_count=3;options.selections={0,3,5};check(peer->session.begin_multiplayer(peer->resources,options),"rule world begin");auto& w=*peer->session.battle;
    std::array<MultiplayerInput,3> in{};for(unsigned i=0;i<125;++i)step(*peer,in);
    auto& p=*w.pilots[0];auto& q=*w.pilots[1];auto& r=*w.pilots[2];
    // Isolate transfer timing from incidental stage bullets. Collision and
    // shield cases below deliberately restore native vulnerability.
    for(auto& pilot:w.pilots)if(pilot)pilot->player->state.invincibility.set(100000,&w.animations.rate);
    put(*p.player,0,400);put(*q.player,10,400);put(*r.player,100,400);p.economy.power=40;q.economy.power=0;
    for(unsigned i=0;i<5;++i){in[0].held=1;step(*peer,in);in[0].held=0;step(*peer,in);}
    for(unsigned i=0;i<15;++i)step(*peer,in);check(p.economy.power==20&&q.economy.power==12,"cross-loadout transfer consumes and grants one native level");
    p.economy.lives=3;q.economy.lives=1;for(unsigned i=0;i<90;++i){in[0].held=8;step(*peer,in);}for(unsigned i=0;i<15;++i)step(*peer,in);check(p.economy.lives==2&&q.economy.lives==2,"life transfer item delivery");
    for(unsigned i=0;i<95;++i)step(*peer,in);check(p.economy.lives==2,"life transfer latches until focus release");
    in={};step(*peer,in);q.economy.lives=-1;check(q.game_over(false),"native final death becomes ghost");
    put(*p.player,0,400);put(*q.player,10,400);q.ghost_dx=q.ghost_dy=0;q.ghost_clock=1;
    for(unsigned i=0;i<90;++i){q.ghost_dx=q.ghost_dy=0;q.ghost_clock=1;in[0].held=8;step(*peer,in);}check(!q.ghost&&q.economy.power==48,"ghost rescue restores selected half power");check(q.player->state.invincibility.current==280,"rescue invincibility begins at 280 logic ticks");
    check(q.player->state.life_state==1&&q.player->state.state_timer.current>=60,"rescue bypasses native cancel-all entrance");
    ItemState fragment{};fragment.type=5;w.items.multiplayer_current_target=0;bool convert=false;const int old=p.economy.lives;
    for(unsigned i=0;i<5;++i)check(w.mp_collect(fragment,convert),"shared fragment pickup");check(p.economy.lives==old+1&&w.mp_fragments==0,"five shared fragments grant team extend");
    q.economy.lives=-1;check(q.game_over(false),"second ghost");for(unsigned i=0;i<5;++i)check(w.mp_collect(fragment,convert),"ghost eligible shared fragment reward");check(q.ghost,"reward life never revives ghost");
    check(w.next_stage(peer->resources,2),"native overlapping stage transition");bool checked_activation=false;for(unsigned i=0;i<170;++i){in={};step(*peer,in);if(w.stage_active&&!checked_activation){checked_activation=true;check(q.economy.power==48&&q.player->state.invincibility.current==119,"stage reset preserves selected half power and first-tick protection");}}check(checked_activation&&!q.ghost&&w.stage_active,"stage activation revives ghosts");check(w.animations.multiplayer_tag(q.player->motion.body)==2,"stage revival retains body owner");
    for(unsigned i=0;i<3;++i){auto& seat=*w.pilots[i];seat.economy.lives=-1;check(seat.game_over(false),"all seats become ghosts");}
    for(unsigned i=0;i<179;++i)step(*peer,in);check(peer->session.state.phase==GameSessionPhase::stage,"team wipe retains 179 ticks");step(*peer,in);check(peer->session.state.phase==GameSessionPhase::game_over,"team wipe enters native Game Over at 180 ticks");
    std::vector<u8> output;check(!peer->session.save_scores(output)&&output.empty(),"MP never serializes ordinary score");check(!peer->session.save_replay("TEST",output),"MP never serializes single-seat replay");
    std::puts("PASS: power/life transfers, latch, rescue, shared fragments, stage revival, wipe, storage isolation");
}
void owner_cases(const std::vector<u8>& bytes){
    auto peer=std::make_unique<Peer>(bytes);MultiplayerOptions options;options.seat_count=3;options.selections={0,3,5};check(peer->session.begin_multiplayer(peer->resources,options),"owner test world");auto& w=*peer->session.battle;std::array<MultiplayerInput,3> in{};for(unsigned frame=0;frame<125;++frame)step(*peer,in);
    const Vec3 target{0,100,0};const Vec2 size{16,16};
    for(unsigned seat=0;seat<3;++seat){auto& p=*w.pilots[seat];p.player->state.invincibility.set(100000,&w.animations.rate);check(p.player->shots.damage_areas.circle(target,32,0,31,10*(seat+1),&w.animations.rate)!=nullptr,"seat-owned native damage area");}
    i32 damage=0;check(w.mp_damage_position(target,size,damage)&&damage==60,"all-seat damage summed once after per-owner native cap");check(w.economy.score_units>0,"native attacking owners contribute to the single shared score");
    Enemy enemy{};enemy.state.script_owner=&enemy;enemy.state.flags=0x80000;enemy.state.health=enemy.state.max_health=1000;
    check(w.multiplayer_boss_damage(enemy.state,damage)==40&&enemy.state.health==1000&&enemy.state.max_health==1000,"3P boss coefficient changes damage only");w.pilots[1]->ghost=true;check(w.mp_damage_position(target,size,damage)&&damage==40&&w.multiplayer_boss_damage(enemy.state,damage)==30,"ghost removes own damage and changes active 2P coefficient");w.pilots[2]->ghost=true;check(w.mp_damage_position(target,size,damage)&&damage==10&&w.multiplayer_boss_damage(enemy.state,damage)==10,"one remaining attacker uses native damage");
    for(auto& pilot:w.pilots)if(pilot){pilot->ghost=false;for(auto& area:pilot->player->shots.damage_areas.areas)area.flags=0;}
    auto& shield=*w.pilots[2];shield.economy.power=80;shield.player->state.invincibility.set(0,&w.animations.rate);in[2].held=2;step(*peer,in);in={};step(*peer,in);
    check(shield.bomb->state.active&&(shield.player->motion.state.flags&4)&&shield.economy.power==60,"Nitori activation consumes own native level");check(w.enemy_environment.shared_integers[1]==0,"unused shield does not report team Bomb use");
    w.spell_flags=3;w.spell_elapsed=61;w.spell_bonus=12340;EnemyAnimations helper(w.animations);helper.erase(shield.bomb->state.animation);check(w.animations.update(false)&&shield.tick_bomb(),"unused shield expiry");check(shield.economy.power==70&&(w.spell_flags&2)&&w.spell_bonus==12340,"unused Nitori shield refunds ten raw power without failing shared spell");
    w.spell_flags=0;shield.economy.power=80;in[2].held=2;step(*peer,in);in={};w.spell_flags=3;w.spell_elapsed=61;w.spell_bonus=12340;const int lives=shield.economy.lives,misses=w.enemy_environment.shared_integers[0];
    check(w.mp_hit(2,{CollisionKind::Hit,true})&&shield.tick_bomb(),"Nitori absorbs its own hit");check(shield.player->state.life_state==1&&shield.economy.lives==lives&&w.enemy_environment.shared_integers[0]==misses,"shield hit preserves lives and shared Miss count");check(!(w.spell_flags&2)&&w.spell_bonus==0&&w.enemy_environment.shared_integers[1]>0,"hit shield fails shared capture and reports team Bomb use");w.spell_flags=0;
    auto& victim=*w.pilots[1];const int before=victim.economy.lives;victim.player->state.invincibility.set(0,&w.animations.rate);check(w.mp_hit(1,{CollisionKind::Hit,true}),"P2 hit uses exact owner");for(unsigned i=0;i<9;++i)check(victim.tick(),"native P2 death sequence");check(victim.economy.lives==before-1&&w.enemy_environment.shared_integers[0]==misses+1,"P2 Miss increments shared stage death counter once");
    victim.economy.lives=-1;check(victim.game_over(false)&&victim.revive(true),"P2 ghost and rescue cleanup");check(w.animations.multiplayer_tag(victim.player->motion.body)==2,"rescue body keeps permanent seat owner");for(const auto& option:victim.player->motion.options)if(auto* vm=w.animations.find(option.animation))check(w.animations.multiplayer_tag(*vm)==2,"rescue options keep permanent seat owner");
    victim.economy.power=96;check(victim.power_changed(),"restore Marisa A power");in[1].held=2;step(*peer,in);in={};check(victim.bomb->distortion()!=nullptr,"native remote Bomb owns distortion");const auto ids=victim.bomb->distortion()->animations;check(victim.game_over(false)&&!victim.bomb->distortion(),"ghost retires Bomb distortion owner");check(w.animations.update(false),"consume ANM retirement markers");for(auto id:ids)check(!w.animations.find(id),"distortion ANM released before next draw");check(peer->session.draw(peer->renderer),"draw after Bomb owner cleanup");
    w.stage->state.effects_enabled=2;const auto prior=w.stage->state.effects_enabled;in[0].pause=true;step(*peer,in);in[0].pause=false;for(unsigned i=0;i<5;++i)step(*peer,in);check(w.stage->state.effects_enabled==prior,"paused confirmed presentation does not advance stage draw effects");
    check(shield.bomb_background_color(0x20202020),"Nitori shared background tint owner");const auto native_tint=w.stage->state.tint;const auto identity=peer->session.multiplayer_hash();w.mp_options.local_seat=0;check(peer->session.draw(peer->renderer),"teammate shield tinted presentation");check(peer->renderer.multiplayer_tint(native_tint,3)!=native_tint,"teammate tint attenuates toward neutral multiplier");w.mp_options.local_seat=2;check(peer->session.draw(peer->renderer),"local shield tinted presentation");check(peer->renderer.multiplayer_tint(native_tint,3)==native_tint,"local tint retains native multiplier");check(w.stage->state.tint==native_tint&&peer->session.multiplayer_hash()==identity,"local tint never mutates shared stage or authority fingerprint");
    std::puts("PASS: direct ECL owners, all-seat damage, Nitori shield, shared Miss, ANM cleanup, frozen pause draw");
}
void restart_risk_cases(const std::vector<u8>& bytes){
    for(unsigned count:{2u,3u}){
        auto reused=std::make_unique<Peer>(bytes);MultiplayerOptions options;options.seat_count=count;options.selections={0,3,5};options.seed=0x31725119;
        check(reused->session.begin_multiplayer(reused->resources,options),"restart source world");std::array<MultiplayerInput,3> in{};
        for(unsigned i=0;i<125;++i)step(*reused,in);
        for(unsigned generation=0;generation<2;++generation){
            in={};in[count-1].pause=true;step(*reused,in);
            check(reused->session.state.phase==GameSessionPhase::paused&&reused->session.animations.rate==0,"restart originates from actual confirmed pause");
            const auto paused_frame=reused->session.battle->frame;in={};for(unsigned i=0;i<3;++i)step(*reused,in);
            check(reused->session.battle->frame==paused_frame,"old paused world is frozen before generation replacement");
            if(generation)options.seed=0x8e2a45c7;
            auto fresh=std::make_unique<Peer>(bytes);
            check(reused->session.begin_multiplayer(reused->resources,options),"paused same/new seed session rebuild");
            check(fresh->session.begin_multiplayer(fresh->resources,options),"fresh same/new seed control world");
            check(reused->session.animations.rate==1,"MP entry restores normal time before constructing native owners");
            check(reused->session.multiplayer_frame==0&&reused->session.state.frame==0&&reused->session.multiplayer_hash()==fresh->session.multiplayer_hash(),"restarted frame zero equals fresh negotiated world");
            std::array<i32,3> initial_x{};for(unsigned seat=0;seat<count;++seat)initial_x[seat]=reused->session.battle->pilots[seat]->player->motion.state.x;
            for(unsigned frame=0;frame<220;++frame){
                in={};for(unsigned seat=0;seat<count;++seat)in[seat].held=1|(seat&1?64:128);
                step(*reused,in);step(*fresh,in);
                check(reused->session.multiplayer_hash()==fresh->session.multiplayer_hash(),"paused restart/seek matches fresh peer after every confirmed update and draw");
            }
            check(reused->session.battle->frame==220&&reused->session.state.frame==220&&reused->session.multiplayer_frame==220,"restarted shared world and session clocks advance once per tick");
            for(unsigned seat=0;seat<count;++seat){const auto& p=*reused->session.battle->pilots[seat]->player;check(p.state.life_state==1&&p.state.state_timer.current>0&&p.motion.state.x!=initial_x[seat],"restarted native birth timer finishes and selected seat actually moves");}
            compare_owners(*reused->session.battle,*fresh->session.battle);
        }
    }
    std::puts("PASS: actual paused 2P/3P same/new seed restart, fresh-peer per-frame identity, native time and movement");
}
#include "native-items.hpp"
#include "native-score.hpp"
#include "native-resources.hpp"
#include "native-challenge.hpp"
void presentation_risk_cases(const std::vector<u8>& bytes){
    auto peer=std::make_unique<Peer>(bytes);MultiplayerOptions options;options.seat_count=3;options.selections={0,3,5};
    check(peer->session.begin_multiplayer(peer->resources,options),"presentation risk world");auto& w=*peer->session.battle;std::array<MultiplayerInput,3> in{};
    for(unsigned i=0;i<125;++i)step(*peer,in);
    for(unsigned seat=0;seat<3;++seat){w.pilots[seat]->player->state.invincibility.set(100000,&w.animations.rate);for(unsigned i=0;i<9;++i)check(w.mp_life_icons[seat][i].resource==&w.resources.core.front&&w.mp_life_icons[seat][i].script_index==i32(10+i),"HUD uses original per-seat star/fragment scripts");for(unsigned i=0;i<4;++i)check(w.mp_communication_icons[seat][i].resource==&w.resources.core.front&&w.mp_communication_icons[seat][i].script_index==i32(32+i),"HUD uses original communication scripts");}
    unsigned native_labels=0;if(auto* frame=w.animations.find(w.hud.frame_animation))for(auto* node=frame->child.next;node;node=node->next)if(node->value->script_index>=5&&node->value->script_index<=9){++native_labels;check(bool(node->value->flags&2)==(node->value->script_index<=6),"shared score stays visible; Life/Power repeat and local Graze relocates");}check(native_labels==5,"all five native HUD label sources retained");
    local_hud_cases(*peer);
    check(peer->session.draw(peer->renderer),"native repeated HUD draw");unsigned tags=0;for(const auto& request:w.ascii.requests){check(request.text.find("CO-OP")==std::string::npos&&request.text.find("TEAM")==std::string::npos&&request.text.find("restart")==std::string::npos&&request.text.find("COMM")==std::string::npos,"HUD contains no invented dashboard/terminal labels");if(request.text=="1P"||request.text=="2P"||request.text=="3P")++tags;}check(tags==3,"stable native atlas seat labels");
    const auto visual_identity=peer->session.multiplayer_hash(),visual_rng=w.animations.script_rng.calls;
    const auto* frame_source=peer->renderer.multiplayer_hud_background;check(frame_source&&frame_source->script_index==2&&frame_source->sprite_index==1,"only original right frame sprite owns watermark replacement");
    auto& frame=*const_cast<AnmVm*>(frame_source);const auto original_frame=frame;const auto original_pixels=w.resources.core.front.textures[0].pixels;
    peer->renderer.flush();peer->graphics.captured.clear();peer->graphics.capture=true;check(peer->renderer.draw(frame)!=-2,"right frame native band draw");peer->renderer.flush();peer->graphics.capture=false;
    check(peer->graphics.captured.size()==18,"native right frame is split into top border interior bottom border");
    const float span=frame.uv[2].y-frame.uv[0].y;
    const float source_min[]={0,16,464},source_max[]={16,176,480};
    for(unsigned strip=0;strip<3;++strip){float low=1,high=0;for(unsigned i=0;i<6;++i){const auto y=peer->graphics.captured[strip*6+i].uv.y;low=std::min(low,y);high=std::max(high,y);}check(std::abs(low-(frame.uv[0].y+span*source_min[strip]/480))<.00001f&&std::abs(high-(frame.uv[0].y+span*source_max[strip]/480))<.00001f,"right frame samples native borders and only the unlettered interior band");}
    check(std::memcmp(&frame,&original_frame,sizeof(frame))==0&&w.resources.core.front.textures[0].pixels==original_pixels,"watermark suppression cannot mutate native VM or texture pixels");
    peer->renderer.multiplayer_hud_background=nullptr;peer->graphics.captured.clear();peer->graphics.capture=true;check(peer->renderer.draw(frame)!=-2,"ordinary frame owner draw");peer->renderer.flush();peer->graphics.capture=false;check(peer->graphics.captured.size()==6,"without MP owner the original frame remains a single unchanged quad");w.mp_prepare_presentation(peer->renderer);
    w.ascii.clear();check(w.mp_draw_hud(peer->renderer),"three-seat native HUD clearance draw");
    for(unsigned seat=0;seat<3;++seat){const std::string tag=std::to_string(seat+1)+"P";bool found=false;for(const auto& request:w.ascii.requests)if(request.text==tag){const float y[]={88,160,232};found=true;check(request.position.x==436&&request.position.y==y[seat],"Life/Power group headings leave the original local Graze row clear");}check(found,"each seat retains its native-atlas tag");}
    for(const auto& request:w.ascii.requests){const float native_height=request.style.font==1?9:request.style.font==2?10:16;check(request.position.y>=48&&request.position.y+native_height*request.style.scale.y<=464,"all repeated native font rows fit above the original bottom border");}
    check(peer->session.multiplayer_hash()==visual_identity&&w.animations.script_rng.calls==visual_rng,"right frame and HUD reflow preserve world identity and RNG");
    auto& p=*w.pilots[0];check(!p.player->motion.state.focused&&!p.player->motion.focus_animation,"Always Hitbox fixture has no native Focus input");
    const auto identity=peer->session.multiplayer_hash(),rng=w.animations.script_rng.calls;const auto weapon=p.player->motion.state.weapon_mode;const auto fast=p.player->motion.state.normal_speed,slow=p.player->motion.state.focus_speed;
    peer->renderer.flush();w.mp_always_hitbox=false;peer->graphics.triangles_drawn=0;check(w.mp_draw_players(peer->renderer),"normal unFocused draw");peer->renderer.flush();const auto ordinary=peer->graphics.triangles_drawn;
    w.mp_always_hitbox=true;peer->graphics.triangles_drawn=0;check(w.mp_draw_players(peer->renderer),"Always Hitbox native marker draw");peer->renderer.flush();check(peer->graphics.triangles_drawn==ordinary+4,"Always Hitbox adds exactly native two textured marker quads");
    auto* marker=w.hitbox_presentation.find(w.hitbox_marker);check(marker&&marker->resource==&w.resources.core.bullet&&marker->script_index==74&&marker->sprite_index==324,"Always Hitbox retains bullet 74 native marker sprite");check(marker&&marker->child.next&&marker->child.next->value->script_index==73,"Always Hitbox retains native counter-rotating child");
    check(!p.player->motion.state.focused&&!p.player->motion.focus_animation&&p.player->motion.state.weapon_mode==weapon&&p.player->motion.state.normal_speed==fast&&p.player->motion.state.focus_speed==slow,"draw-only hitbox does not synthesize Focus or modify native motion/weapon");check(peer->session.multiplayer_hash()==identity&&w.animations.script_rng.calls==rng,"Always Hitbox has no authoritative state/RNG effect");
    w.mp_local_visibility=false;peer->graphics.primitive_calls=0;check(w.mp_draw_players(peer->renderer),"local visibility disabled draw");check(peer->graphics.primitive_calls==0,"disabled local visibility adds no highlight");
    w.mp_local_visibility=true;check(w.mp_draw_players(peer->renderer),"local visibility enabled draw");check(peer->graphics.primitive_calls==4,"enabled local visibility marks only the local pilot");
    check(peer->session.multiplayer_hash()==identity,"local visibility never changes authoritative gameplay");
    w.mp_enabled=false;w.always_hitbox=false;peer->renderer.flush();peer->graphics.triangles_drawn=0;check(w.draw(peer->renderer,SceneDrawKind::Player),"ordinary player draw");peer->renderer.flush();const auto normal_triangles=peer->graphics.triangles_drawn;
    w.always_hitbox=true;peer->graphics.triangles_drawn=0;check(w.draw(peer->renderer,SceneDrawKind::Player),"ordinary always-hitbox draw");peer->renderer.flush();check(peer->graphics.triangles_drawn==normal_triangles+4,"ordinary always-hitbox also adds the native two-layer marker");w.mp_enabled=true;
    in[0].pause=true;step(*peer,in);check(peer->session.state.phase==GameSessionPhase::paused,"native pause fixture");auto* pause=w.mp_presentation.find(peer->session.pause_menu->menu_animation);check(pause&&pause->resource==&w.resources.core.front&&pause->script_index==89,"pause uses complete original parent menu owner");
    std::puts("PASS: native atlas HUD, original two-layer Always Hitbox, no Focus/RNG mutation");
}
void native_ui_cases(const std::vector<u8>& bytes){
    auto a=std::make_unique<Peer>(bytes),b=std::make_unique<Peer>(bytes);MultiplayerOptions options;options.seat_count=3;options.selections={0,3,5};
    check(a->session.begin_multiplayer(a->resources,options),"native UI primary world");options.local_seat=1;check(b->session.begin_multiplayer(b->resources,options),"native UI different viewer world");
    std::array<MultiplayerInput,3> in{};for(unsigned i=0;i<125;++i){step(*a,in);step(*b,in);}
    in[0].held=1;in[1].pause=true;step(*a,in);step(*b,in);auto& w=*a->session.battle;
    const auto frozen=w.mp_hash(),rng=w.animations.script_rng.calls,frame=w.frame;check(a->session.animations.rate==0&&w.mp_presentation.rate==1,"pause animation rate is independent from frozen combat");
    auto* menu=w.mp_presentation.find(a->session.pause_menu->menu_animation);auto* background=w.mp_presentation.find(a->session.pause_menu->background_animation);
    check(menu&&menu->script_index==89&&background&&background->resource==&w.resources.core.text&&background->script_index==75,"native pause parent and captured-background resource owners");
    check(a->session.pause_menu->background_vm()==background,"GPU capture resolves the PauseMenu owning ANM manager");
    unsigned children=0;for(auto* n=menu->child.next;n;n=n->next){++children;check(n->value->script_index==74||n->value->script_index==75||(n->value->script_index>=77&&n->value->script_index<=80),"pause retains the original four choices");}check(children==6,"all four native pause choices are present");
    in[1].pause=false;for(unsigned i=0;i<25;++i){in[1].held=i&1?1:0;step(*a,in);step(*b,in);check(a->session.multiplayer_hash()==b->session.multiplayer_hash(),"pause menu remains deterministic across local viewers");}
    check(a->session.state.phase==GameSessionPhase::paused&&a->session.pause_menu->state==33,"held entry Shot and P2 edges cannot confirm P1 native menu");
    check(w.frame==frame&&w.mp_hash()==frozen&&w.animations.script_rng.calls==rng,"native overlay animations cannot advance combat or combat RNG");
    const auto eof=a->session.multiplayer_hash();check(a->session.draw(a->renderer)&&a->session.multiplayer_hash()==eof,"paused Replay EOF draw cannot advance native menu timers");
    in={};step(*a,in);step(*b,in);in[0].held=1;step(*a,in);step(*b,in);check(a->session.pause_menu->state==34,"P1 fresh confirmation enters original pause dismissal animation");
    in={};for(unsigned i=0;i<13&&a->session.state.phase==GameSessionPhase::paused;++i){step(*a,in);step(*b,in);check(a->session.multiplayer_hash()==b->session.multiplayer_hash(),"native pause dismissal has identical confirmed-frame boundary");}
    check(a->session.state.phase==GameSessionPhase::stage&&w.frame==frame&&a->session.animations.rate==1&&!a->session.pause_menu,"Resume restores normal time after native overlay dismissal without ticking combat early");
    step(*a,in);step(*b,in);check(w.frame==frame+1,"first post-menu confirmed frame advances world exactly once");
    in[0].pause=true;step(*a,in);step(*b,in);in={};step(*a,in);step(*b,in);in[2].pause=true;step(*a,in);step(*b,in);in={};for(unsigned i=0;i<13&&a->session.state.phase==GameSessionPhase::paused;++i){step(*a,in);step(*b,in);}check(a->session.state.phase==GameSessionPhase::stage&&a->session.multiplayer_hash()==b->session.multiplayer_hash(),"any seat retains confirmed Pause-key resume during native intro");
    in={};in[0].pause=true;step(*a,in);step(*b,in);in={};for(unsigned i=0;i<18;++i){step(*a,in);step(*b,in);}
    const auto restart_frame=w.frame,restart_hash=w.mp_hash();
    in[0].held=32;step(*a,in);step(*b,in);check(a->session.pause_menu->cursor.selected==1,"first Down selects Return");
    in={};step(*a,in);step(*b,in);in[0].held=32;step(*a,in);step(*b,in);check(a->session.pause_menu->cursor.selected==2,"second Down selects Replay Save");
    in={};step(*a,in);step(*b,in);in[0].held=32;step(*a,in);step(*b,in);check(a->session.pause_menu->cursor.selected==3&&b->session.pause_menu->cursor.selected==3,"third Down selects Restart");
    in={};step(*a,in);step(*b,in);in[0].held=1;step(*a,in);step(*b,in);
    check(a->session.pause_menu->action==PauseAction::Restart&&b->session.pause_menu->action==PauseAction::Restart,"native Restart emits the confirmed generation request on both viewers");
    check(a->session.state.phase==GameSessionPhase::paused&&w.frame==restart_frame&&w.mp_hash()==restart_hash,"Restart cannot rebuild a local world before its network fence");
    in={};step(*a,in);step(*b,in);in[0].held=0x200000;step(*a,in);step(*b,in);
    check(a->session.pause_menu->action==PauseAction::Restart&&a->session.multiplayer_hash()==b->session.multiplayer_hash(),"R emits the same deterministic Restart action");
    in={};step(*a,in);step(*b,in);in[0].held=16;step(*a,in);step(*b,in);check(a->session.pause_menu->cursor.selected==2,"Up from Restart selects Replay Save");
    in={};step(*a,in);step(*b,in);in[0].held=16;step(*a,in);step(*b,in);check(a->session.pause_menu->cursor.selected==1,"Up again selects Return");
    in={};step(*a,in);step(*b,in);in[0].held=1;step(*a,in);step(*b,in);
    check(a->session.state.phase==GameSessionPhase::finished&&b->session.state.phase==GameSessionPhase::finished&&a->session.multiplayer_hash()==b->session.multiplayer_hash(),"Return ends both viewers at the same confirmed boundary");
    check(w.frame==restart_frame&&w.mp_hash()==restart_hash,"Return keeps combat frozen until network retirement and saving");
    options.local_seat=0;check(a->session.begin_multiplayer(a->resources,options),"life UI fixture");auto& world=*a->session.battle;in={};for(unsigned i=0;i<125;++i)step(*a,in);
    auto& donor=*world.pilots[0];auto& ghost=*world.pilots[1];donor.economy.lives=3;ghost.economy.lives=-1;check(ghost.game_over(false),"native ghost feedback fixture");
    for(auto& p:world.pilots)if(p)p->player->state.invincibility.set(100000,&world.animations.rate);put(*donor.player,0,400);put(*ghost.player,10,400);put(*world.pilots[2]->player,120,400);in[0].held=8;
    unsigned previous_alpha=0;for(unsigned tick=1;tick<=89;++tick){ghost.ghost_dx=ghost.ghost_dy=0;ghost.ghost_clock=1;step(*a,in);
        const auto alpha=a->renderer.multiplayer_opacity[2];check(alpha>=previous_alpha,"recipient ghost brightens monotonically with confirmed life progress");previous_alpha=alpha;
        if(tick==30||tick==60||tick==89){const std::string text=std::to_string(tick*100/90)+"%";bool shown=false;for(const auto& q:world.ascii.requests)if(q.text==text){shown=true;check(q.style.font==2&&q.style.pass==1&&q.position.x>200&&q.position.x<240&&q.position.y==392,"life feedback uses native numeric atlas in field next to giver");}check(shown,"native life percentage is visible during transfer");}
    }
    check(previous_alpha>245&&ghost.ghost,"ghost is nearly fully visible before rescue completes");const auto identity=world.mp_hash(),calls=world.animations.script_rng.calls;check(a->session.draw(a->renderer)&&world.mp_hash()==identity&&world.animations.script_rng.calls==calls,"progress and ghost alpha are presentation-only");
    const auto sound_begin=world.events.size();ghost.ghost_dx=ghost.ghost_dy=0;ghost.ghost_clock=1;step(*a,in);check(!ghost.ghost&&donor.economy.lives==2&&donor.life_hold==0&&donor.life_latched,"90th tick preserves exact native rescue resource boundary");
    bool cue=false;for(size_t i=sound_begin;i<world.events.size();++i)cue|=world.events[i].kind==BattleEventKind::Sound&&world.events[i].value==0x2c;check(cue,"rescue success uses native extend sound");
    for(const auto& q:world.ascii.requests)check(q.text.find('%')==std::string::npos,"completed life operation has no lingering percentage");
    std::puts("PASS: original pause overlay/Resume, confirmed P1/menu input, combat freeze, native life feedback and ghost rescue alpha");
}
void result_risk_cases(const std::vector<u8>& bytes){
    auto peer=std::make_unique<Peer>(bytes);MultiplayerOptions options;options.seat_count=3;options.selections={0,3,5};
    check(peer->session.begin_multiplayer(peer->resources,options),"native result world");auto& w=*peer->session.battle;
    std::array<MultiplayerInput,3> in{};for(unsigned i=0;i<125;++i)step(*peer,in);
    auto tap=[&](u32 held){in={};step(*peer,in);in[0].held=held;step(*peer,in);in={};step(*peer,in);};
    auto wait=[&](unsigned n){in={};for(unsigned i=0;i<n;++i)step(*peer,in);};
    auto finish_name=[&]{tap(1);tap(16);tap(64);tap(1);}; // Type A, then native Up/Left to End.
    w.economy.score_units=1234567;
    for(unsigned seat=0;seat<3;++seat){w.pilots[seat]->economy.lives=-1;check(w.pilots[seat]->game_over(false),"result all ghosts");}
    in[0].held=1;for(unsigned i=0;i<179;++i)step(*peer,in);check(peer->session.state.phase==GameSessionPhase::stage&&w.mp_wipe_frames==179,"179th ghost frame remains combat");step(*peer,in);
    check(peer->session.state.phase==GameSessionPhase::game_over&&peer->session.pause_menu&&peer->session.pause_menu->multiplayer_result,"180th ghost frame enters original end-menu owner");
    auto& menu=*peer->session.pause_menu;const auto frame=w.frame,stage_draw=w.stage->state.frame_count,rng=w.animations.script_rng.calls;
    for(unsigned i=0;i<24;++i){in[1].held=i&1?1:0;step(*peer,in);}
    check(menu.state==18&&menu.result_score==1234567&&!menu.unranked,"Game Over keeps one native Score Ranking with shared score");
    check(peer->session.scores.high_score(0,options.difficulty)==1234567&&peer->session.scores.high_score(3,options.difficulty)==1000000,"shared score enters only original P1 loadout table in memory");
    check(menu.name_length==0,"held P1 Shoot and P2 confirm cannot edit the new result name");
    check(w.frame==frame&&w.stage->state.frame_count==stage_draw&&w.animations.script_rng.calls==rng,"native result and name UI keep combat frozen");
    finish_name();check(menu.state==14&&menu.cursor.selected==1,"original name confirmation enters native Return/Save choices");
    auto* root=w.animations.find(menu.menu_animation);check(root&&root->script_index==98,"Game Over reuses original front 98");
    bool back=false,save=false;for(auto* node=&root->child;node;node=node->next){const auto script=node->value->script_index;check(script!=94&&script!=97,"Continue and Retry animations are absent");back|=script==95;save|=script==96;}
    check(back&&save,"native Game Over Return and Replay Save both remain available");
    tap(16);check(menu.cursor.selected==2,"Up skips forbidden Continue and Retry");
    tap(32);check(menu.cursor.selected==1,"Down skips forbidden Continue and Retry");
    tap(32);tap(1);check(menu.state==16&&menu.cursor.selected==0,"native Replay Save opens numbered slots");
    wait(12);in[0].held=1;step(*peer,in);check(menu.state==17&&menu.recording_metadata_requested,"native Replay name requests current MP recording metadata");
    wait(12);tap(1);check(menu.save_requested&&menu.state==16&&menu.entered_name[0]=='A',"native Replay Save emits original slot/name request");
    menu.save_requested=false;wait(12);tap(2);check(menu.state==14,"Replay list Back returns to original end choices");
    in={};step(*peer,in);in[0].pause=true;step(*peer,in);check(menu.cursor.selected==1,"confirmed P1 Escape selects native Return");
    tap(1);for(unsigned i=0;i<20&&peer->session.state.phase!=GameSessionPhase::finished;++i)wait(1);
    check(peer->session.state.phase==GameSessionPhase::finished&&w.economy.score_units==1234567,"native Return ends the generation without Continue or score reset");
    std::vector<u8> output;check(!peer->session.save_scores(output)&&output.empty(),"native in-memory name registration cannot serialize MP scores");
    const auto eof_hash=peer->session.multiplayer_hash();check(peer->session.draw(peer->renderer)&&peer->session.multiplayer_hash()==eof_hash,"EOF presentation cannot inject menu input");
    options.seat_count=2;options.stage=7;options.difficulty=4;
    check(peer->session.begin_multiplayer(peer->resources,options),"Extra result fixture");peer->session.economy.score_units=2345678;peer->session.battle->completion.state.exit=StageExit::Results;wait(25);
    auto& extra=*peer->session.pause_menu;check(extra.state==25&&extra.result_score==2345678,"Extra completion retains original native ranking/name stage");
    finish_name();check(extra.state==22&&extra.cursor.selected==0,"Extra name finishes into native compact result menu");
    root=peer->session.animations.find(extra.menu_animation);check(root&&root->script_index==103,"Extra keeps original front 103");
    back=save=false;for(auto* node=&root->child;node;node=node->next){check(node->value->script_index!=102,"Extra Retry is absent");back|=node->value->script_index==100;save|=node->value->script_index==101;}
    check(back&&save,"Extra retains original Return and Replay Save");tap(32);tap(1);check(extra.state==23,"Extra uses native Replay slots");wait(12);tap(1);wait(12);tap(1);check(extra.save_requested&&extra.state==23,"Extra native name confirmation emits Replay Save request");
    options.stage=1;options.difficulty=1;options.selections={4,5,0};
    check(peer->session.begin_multiplayer(peer->resources,options),"Ending result fixture");peer->session.economy.score_units=3456789;auto* combat=peer->session.battle.get();auto* original_p2=combat->pilots[1]->player;combat->completion.state.exit=StageExit::Ending;wait(1);
    check(peer->session.state.phase==GameSessionPhase::ending&&peer->session.ending&&peer->session.ending->index==10,"P1 Marisa B keeps native good Ending");
    const auto ending_frame=combat->frame;for(unsigned i=0;i<4000&&peer->session.state.phase==GameSessionPhase::ending;++i){in={};in[0].held=512|(i&1?1:0);step(*peer,in);}
    check(peer->session.state.phase==GameSessionPhase::game_over&&peer->session.title&&peer->session.title->multiplayer_result,"Ending and Staff finish into original Title Result");
    check(combat->frame==ending_frame&&combat->pilots[1]->player==original_p2,"Ending never ticks or replaces combat owners");
    auto& result=*peer->session.title;auto* heading=peer->session.animations.find(result.handles[102]);
    check(heading&&heading->resource==&peer->session.resources.core.title&&heading->script_index==102&&!result.handles[100],"post-Ending uses original Name Regist, not an invented Player Data result");
    check(result.handles[151]&&result.handles[156]&&result.handles[159],"one shared result retains original P1 character/partner and shared difficulty badges");
    wait(12);check(result.screen==TitleScreen::Results&&result.substate==2&&result.result_score==3456789,"native Title Result contains shared score");
    for(unsigned i=0;i<8;++i){in={};in[1].held=i&1?1:0;step(*peer,in);}check(result.name_length==0,"P2 cannot edit the shared result name");
    finish_name();wait(18);check(result.screen==TitleScreen::ReplaySave&&result.substate==2,"original result name leads to native Replay Save");
    tap(1);wait(12);tap(1);check(result.replay_save_requested&&result.substate==2&&result.entered_name[0]=='A',"post-Ending native Replay name emits slot/name save request");
    result.replay_save_requested=false;tap(2);wait(8);
    check(result.multiplayer_result_done&&peer->session.state.phase==GameSessionPhase::finished,"native Replay Save Back retires the shared generation");
    check(!peer->session.save_scores(output)&&output.empty(),"post-Ending shared ranking remains memory-only");
    std::puts("PASS: native Game Over/Extra/Ending shared score ranking and name, original Replay Save requests, P1 inputs, no Continue/Retry or score persistence");
}
}
int main(int argc,char** argv){
    if(argc<2||argc>3){std::fprintf(stderr,"retail archive path required\n");return 2;}
    std::ifstream file(argv[1],std::ios::binary);std::vector<u8> bytes((std::istreambuf_iterator<char>(file)),{});
    check(!bytes.empty(),"retail archive fixture present");
    const auto mode=[&](const char* value){return argc==3&&std::strcmp(argv[2],value)==0;};
    if(mode("presentation")){resource_cases(bytes);presentation_risk_cases(bytes);}
    else if(mode("restart"))restart_risk_cases(bytes);
    else if(mode("challenge"))challenge_cases(bytes);
    else if(mode("score")){score_cases(bytes);result_risk_cases(bytes);}
    else if(mode("ui")){native_ui_cases(bytes);presentation_risk_cases(bytes);restart_risk_cases(bytes);}
    else if(mode("repair")){presentation_risk_cases(bytes);restart_risk_cases(bytes);}
    else{
        if(argc==2){battle_cases(bytes);rules_cases(bytes);owner_cases(bytes);}
        ghost_dialogue_cases(bytes);reported_rule_cases(bytes);challenge_cases(bytes);score_cases(bytes);item_risk_cases(bytes);resource_cases(bytes);presentation_risk_cases(bytes);result_risk_cases(bytes);restart_risk_cases(bytes);native_ui_cases(bytes);
    }
    std::printf("PASS: %u checks, %u native logic/draw ticks; no original executable\n",checks,ticks);return 0;
}
