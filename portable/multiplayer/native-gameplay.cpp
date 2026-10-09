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
    unsigned triangles_drawn=0;
    bool capture=false;std::vector<AnmVertex> captured;
    touhou::graphics::PipelineState& pipeline()override{return state;}
    u32 texture(const AnmResource&,u32 index)override{return index+1;}
    void bind_texture(u32)override{}
    void set_layout(touhou::graphics::VertexLayout)override{}
    void set_matrix(touhou::graphics::MatrixKind,const Matrix4&)override{}
    void triangles(u32 n,const void* vertices,u32 stride)override{triangles_drawn+=n;if(capture&&stride==sizeof(AnmVertex)){const auto* source=static_cast<const AnmVertex*>(vertices);captured.insert(captured.end(),source,source+n*3);}}
    void primitives(touhou::graphics::Topology,u32,const void*,u32)override{}
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
    i32 damage=0;check(w.mp_damage_position(target,size,damage)&&damage==60,"all-seat damage summed once after per-owner native cap");for(unsigned seat=0;seat<3;++seat)check(w.pilots[seat]->economy.score_units>0,"damage score attributed to every attacking owner");
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
void presentation_risk_cases(const std::vector<u8>& bytes){
    auto peer=std::make_unique<Peer>(bytes);MultiplayerOptions options;options.seat_count=3;options.selections={0,3,5};
    check(peer->session.begin_multiplayer(peer->resources,options),"presentation risk world");auto& w=*peer->session.battle;std::array<MultiplayerInput,3> in{};
    for(unsigned i=0;i<125;++i)step(*peer,in);
    for(unsigned seat=0;seat<3;++seat){w.pilots[seat]->player->state.invincibility.set(100000,&w.animations.rate);for(unsigned i=0;i<9;++i)check(w.mp_life_icons[seat][i].resource==&w.resources.core.front&&w.mp_life_icons[seat][i].script_index==i32(10+i),"HUD uses original per-seat star/fragment scripts");for(unsigned i=0;i<4;++i)check(w.mp_communication_icons[seat][i].resource==&w.resources.core.front&&w.mp_communication_icons[seat][i].script_index==i32(32+i),"HUD uses original communication scripts");}
    unsigned native_labels=0;if(auto* frame=w.animations.find(w.hud.frame_animation))for(auto* node=frame->child.next;node;node=node->next)if(node->value->script_index>=5&&node->value->script_index<=9){++native_labels;check(!(node->value->flags&2),"original private label owners hidden before seat repetitions");}check(native_labels==5,"all five native HUD label sources retained");
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
    for(unsigned seat=0;seat<3;++seat){const std::string tag=std::to_string(seat+1)+"P";bool found=false;for(const auto& request:w.ascii.requests)if(request.text==tag){found=true;check(request.position.y==48+seat*144,"seat starts at original y48 below the difficulty label");}check(found,"each seat retains its native-atlas tag");}
    for(const auto& request:w.ascii.requests){const float native_height=request.style.font==1?9:request.style.font==2?10:16;check(request.position.y>=48&&request.position.y+native_height*request.style.scale.y<=464,"all repeated native font rows fit above the original bottom border");}
    check(peer->session.multiplayer_hash()==visual_identity&&w.animations.script_rng.calls==visual_rng,"right frame and HUD reflow preserve world identity and RNG");
    auto& p=*w.pilots[0];check(!p.player->motion.state.focused&&!p.player->motion.focus_animation,"Always Hitbox fixture has no native Focus input");
    const auto identity=peer->session.multiplayer_hash(),rng=w.animations.script_rng.calls;const auto weapon=p.player->motion.state.weapon_mode;const auto fast=p.player->motion.state.normal_speed,slow=p.player->motion.state.focus_speed;
    peer->renderer.flush();w.mp_always_hitbox=false;peer->graphics.triangles_drawn=0;check(w.mp_draw_players(peer->renderer),"normal unFocused draw");peer->renderer.flush();const auto ordinary=peer->graphics.triangles_drawn;
    w.mp_always_hitbox=true;peer->graphics.triangles_drawn=0;check(w.mp_draw_players(peer->renderer),"Always Hitbox native marker draw");peer->renderer.flush();check(peer->graphics.triangles_drawn==ordinary+4,"Always Hitbox adds exactly native two textured marker quads");
    auto* marker=w.mp_presentation.find(w.mp_focus_marker);check(marker&&marker->resource==&w.resources.core.bullet&&marker->script_index==74&&marker->sprite_index==324,"Always Hitbox retains bullet 74 native marker sprite");check(marker&&marker->child.next&&marker->child.next->value->script_index==73,"Always Hitbox retains native counter-rotating child");
    check(!p.player->motion.state.focused&&!p.player->motion.focus_animation&&p.player->motion.state.weapon_mode==weapon&&p.player->motion.state.normal_speed==fast&&p.player->motion.state.focus_speed==slow,"draw-only hitbox does not synthesize Focus or modify native motion/weapon");check(peer->session.multiplayer_hash()==identity&&w.animations.script_rng.calls==rng,"Always Hitbox has no authoritative state/RNG effect");
    in[0].pause=true;step(*peer,in);check(peer->session.state.phase==GameSessionPhase::paused,"native pause fixture");check(w.mp_pause_label.resource==&w.resources.core.front&&w.mp_pause_label.script_index==75,"pause uses original label instead of English card");
    std::puts("PASS: native atlas HUD, original two-layer Always Hitbox, no Focus/RNG mutation");
}
void result_risk_cases(const std::vector<u8>& bytes){
    auto peer=std::make_unique<Peer>(bytes);MultiplayerOptions options;options.seat_count=3;options.selections={0,3,5};check(peer->session.begin_multiplayer(peer->resources,options),"result risk world");auto& w=*peer->session.battle;std::array<MultiplayerInput,3> in{};for(unsigned i=0;i<125;++i)step(*peer,in);
    for(unsigned seat=0;seat<3;++seat){w.pilots[seat]->economy.lives=-1;check(w.pilots[seat]->game_over(false),"result all ghosts");}in[0].held=1;
    for(unsigned i=0;i<179;++i)step(*peer,in);check(peer->session.state.phase==GameSessionPhase::stage&&w.mp_wipe_frames==179,"179th all-ghost tick remains combat");step(*peer,in);
    check(peer->session.state.phase==GameSessionPhase::game_over&&w.mp_wipe_frames==180&&peer->session.pause_menu&&peer->session.pause_menu->multiplayer_result,"180th all-ghost tick creates native MP Game Over owner");
    auto& menu=*peer->session.pause_menu;auto* root=w.animations.find(menu.menu_animation);check(root&&root->script_index==98,"wipe reuses original front 98 Game Over root");bool back=false,headline=false;for(auto* node=&root->child;node;node=node->next){const auto script=node->value->script_index;check(script!=94&&script!=96&&script!=97,"Continue ReplaySave Retry ANMs absent");back|=script==95;headline|=script==93;}check(back&&headline,"native Game Over headline and Return action retained");
    const auto frame=w.frame,stage_draw=w.stage->state.frame_count,rng=w.animations.script_rng.calls;const auto records=peer->session.scores.characters;const auto settings=peer->session.scores.settings;
    for(unsigned i=0;i<18;++i){in[1].held=i&1?1:0;step(*peer,in);}check(peer->session.state.phase==GameSessionPhase::game_over&&menu.state==29,"held P1 Shoot and P2 confirm do not close result");
    check(w.frame==frame&&w.stage->state.frame_count==stage_draw&&w.animations.script_rng.calls==rng,"Game Over confirmed frames freeze combat and stage draw clocks");
    in[0].held=0;step(*peer,in);in[0].held=0x200000;step(*peer,in);check(menu.state==29,"R does not restart or exit a result generation");in[0].held=0;step(*peer,in);in[0].held=256;step(*peer,in);check(menu.state==30,"P1 native Enter edge accepts Return action");
    unsigned exit_ticks=0;while(peer->session.state.phase==GameSessionPhase::game_over&&exit_ticks++<30)step(*peer,in);check(peer->session.state.phase==GameSessionPhase::finished&&exit_ticks>=22,"native confirmation and exit animation complete before retirement");
    check(peer->session.scores.characters==records&&peer->session.scores.settings==settings&&!menu.save_requested&&!menu.scan_requested&&!menu.recording_metadata_requested,"MP result never writes ordinary score/name/ReplaySave metadata");
    check(w.frame==frame&&w.mp_wipe_frames==180&&w.pilots[0]->economy.lives==-1,"result does not Continue or reset any combat resources");
    // A confirmed replay contains the same result frames. EOF itself calls no
    // native update; the last confirmed result state therefore remains frozen.
    const auto eof_hash=peer->session.multiplayer_hash();check(peer->session.draw(peer->renderer)&&peer->session.multiplayer_hash()==eof_hash,"terminal presentation cannot inject a result confirmation at EOF");
    options.seat_count=2;options.stage=7;options.difficulty=4;check(peer->session.begin_multiplayer(peer->resources,options),"Extra native terminal fixture");peer->session.battle->completion.state.exit=StageExit::Results;in={};step(*peer,in);check(peer->session.state.phase==GameSessionPhase::game_over&&peer->session.pause_menu->completed,"Extra Results use native completed result owner");root=peer->session.animations.find(peer->session.pause_menu->menu_animation);check(root&&root->script_index==103,"Extra completion retains original front 103 presentation");
    options.stage=1;options.difficulty=1;options.selections={4,5,0};check(peer->session.begin_multiplayer(peer->resources,options),"Ending terminal fixture");auto* combat=peer->session.battle.get();auto* original_p2=combat->pilots[1]->player;combat->completion.state.exit=StageExit::Ending;in={};step(*peer,in);
    check(peer->session.state.phase==GameSessionPhase::ending&&peer->session.ending&&peer->session.ending->index==10,"P1 Marisa B owns original no-Continue good Ending");check(peer->session.battle.get()==combat&&combat->pilots[1]->player==original_p2,"Ending retains permanent final MP seat owners");const auto ending_frame=combat->frame;
    for(unsigned i=0;i<4000&&peer->session.state.phase==GameSessionPhase::ending;++i){in[0].held=512|(i&1?1:0);step(*peer,in);}
    check(peer->session.state.phase==GameSessionPhase::game_over&&peer->session.title&&peer->session.title->multiplayer_result,"original Ending and Staff finish into native Result screen");check(combat->frame==ending_frame&&combat->pilots[1]->player==original_p2,"Ending and Staff never tick or replace combat owner");
    const auto result_records=peer->session.scores.characters;in={};for(unsigned i=0;i<12;++i){in[1].held=i&1?1:0;step(*peer,in);}check(peer->session.title->substate==2&&peer->session.state.phase==GameSessionPhase::game_over,"P2 cannot confirm the post-Ending Result");in[0].held=1;step(*peer,in);for(unsigned i=0;i<24&&peer->session.state.phase==GameSessionPhase::game_over;++i)step(*peer,in);check(peer->session.state.phase==GameSessionPhase::finished&&peer->session.scores.characters==result_records&&!peer->session.title->replay_save_requested,"native post-Ending result exits without score table/name/ordinary replay save");
    std::puts("PASS: 180-frame wipe, native Game Over, P1 edges, combat freeze, Extra Result, P1 Ending/Staff/Result");
}
}
int main(int argc,char** argv){if(argc<2||argc>3){std::fprintf(stderr,"retail archive path required\n");return 2;}std::ifstream file(argv[1],std::ios::binary);std::vector<u8> bytes((std::istreambuf_iterator<char>(file)),{});check(!bytes.empty(),"retail archive fixture present");if(argc==3&&std::strcmp(argv[2],"presentation")==0)presentation_risk_cases(bytes);else if(argc==3&&std::strcmp(argv[2],"restart")==0)restart_risk_cases(bytes);else if(argc==3&&std::strcmp(argv[2],"repair")==0){presentation_risk_cases(bytes);restart_risk_cases(bytes);}else{if(argc==2){battle_cases(bytes);rules_cases(bytes);owner_cases(bytes);}item_risk_cases(bytes);presentation_risk_cases(bytes);result_risk_cases(bytes);restart_risk_cases(bytes);}std::printf("PASS: %u checks, %u native logic/draw ticks; no original executable\n",checks,ticks);return 0;}
