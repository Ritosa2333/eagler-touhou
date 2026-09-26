#include "ResourceTrace.hpp"
#if defined(TH_MULTIPLAYER_FIXTURES) || defined(TH_MULTIPLAYER_RESOURCE_TRACE)
#include "../platform/BrowserRuntime.hpp"
#include <deque>
#include <sstream>
#include <cstdint>

namespace th08::multiplayer::diagnostic {
using Words=std::vector<std::int64_t>;
struct Snapshot {Words values;std::vector<Words> items;bool operator==(const Snapshot& b)const{return values==b.values&&items==b.items;}};
struct TraceEvent {const char* name;int seat,argument;bool after;Words values,item;};
struct Attempt {Snapshot begin,end;Words inputs;std::vector<TraceEvent> events;bool complete=false,overflow=false;};
struct Frame {u32 frame=0,revision=0,predicted=0;bool correcting=false;std::size_t cost=0;Attempt current,first;};
static BrowserRuntime* observed=nullptr;
static SessionSetup observed_setup;
static u32 observed_player=0;
static std::deque<Frame> frames;
static Frame* active=nullptr;
static bool recording_overflow=false;
static bool frozen=false;
static u32 frozen_next=0,frozen_last=~u32(0),frozen_confirmed=~u32(0),frozen_rollback=~u32(0),run_generation=0;
static std::size_t stored_bytes=0;
static u32 dropped_frames=0;
static u32 first_frame=0,restore_mismatch=~u32(0),restore_checks=0;
static Snapshot restore_expected,restore_actual;
// Keep enough real-device history to cover a death, its rollback correction,
// and the later item/resource symptoms. The byte ceiling remains authoritative
// on busy scenes so diagnostics cannot grow without bound on mobile.
constexpr u32 max_frames=2400,max_events=256;
constexpr std::size_t max_bytes=32*1024*1024;
static void freeze(){
    const auto& net=observed->app.session.netplay;frozen=true;
    frozen_next=net.NextFrame();frozen_last=net.LastFrame();frozen_confirmed=net.ConfirmedThrough();frozen_rollback=net.RollbackFrame();
}
static std::size_t memory(const Words& words){return words.capacity()*sizeof(Words::value_type);}
static std::size_t memory(const Snapshot& s){std::size_t n=memory(s.values)+s.items.capacity()*sizeof(Words);for(const auto& i:s.items)n+=memory(i);return n;}
static std::size_t memory(const Attempt& a){std::size_t n=memory(a.begin)+memory(a.end)+memory(a.inputs)+a.events.capacity()*sizeof(TraceEvent);for(const auto& e:a.events)n+=memory(e.values)+memory(e.item);return n;}
static bool retire_oldest(){
    if(frames.empty()||&frames.front()==active)return false;
    const auto through=observed->app.session.netplay.ConfirmedThrough();
    if(through==~u32(0)||frames.front().frame>through)return false;
    stored_bytes-=frames.front().cost;frames.pop_front();++dropped_frames;return true;
}
static u32 bits(float f){u32 v;std::memcpy(&v,&f,4);return v;}
static int item_slot(const ItemState* p){
    if(!observed||!p)return -1;
    const auto& pool=observed->app.game.items.status();
    if(p==&pool.head)return -2;
    const auto a=std::uintptr_t(p),b=std::uintptr_t(pool.items);
    return a>=b&&a-b<sizeof(pool.items)&&(a-b)%sizeof(ItemState)==0?int((a-b)/sizeof(ItemState)):-3;
}
static Words values(){
    auto& a=observed->app;const auto& s=a.session;auto& g=a.game;const auto& n=s.numbers;
    Words v{n.time_orbs,n.total_time_orbs,n.last_spell_requirement,n.point_value,n.clock_time,
        n.score,n.points,n.graze,s.random.seed,s.random.backup,s.random.calls,g.globals.stage,
        g.globals.game_flags,g.globals.spell_flags,g.globals.spell_time_items,
        g.menus.context.pause_state,g.paused,g.retrying,g.enemies.state.frames,
        g.projectile_pool.active_count,g.projectile_pool.timer.current,g.projectile_pool.cancel_frames,
        g.projectile_pool.unknown_counter,g.effect_pool.cursor,g.effect_pool.active_count,g.effect_pool.frames,
        a.supervisor.state.active,a.supervisor.state.target,g.control.state.play_frames,
        g.menus.context.supervisor_state};
    for(u32 seat=0;seat<s.player_count;++seat){
        const auto& r=s.pilot_resources[seat];const auto& p=g.pilot(seat).status();
        v.insert(v.end(),{Scalar::truncate(r.power),Scalar::truncate(r.lives),Scalar::truncate(r.bombs),r.gauge,
            Scalar::truncate(r.deaths),p.life.state,p.life.predead_count,p.life.timer.current,p.life.clear_frames,
            p.context.power,p.context.time_orbs,p.context.bombs,p.context.gauge,p.context.game_flags,
            p.motion.form.focused,p.bomb.active,p.bomb.timer.current,p.shots.shooting_timer.current,
            bits(p.motion.movement.position.x),bits(p.motion.movement.position.y),g.committed_buttons[seat],
            int(g.cooperation.seats[seat].spirit),p.cancel_item,p.item_gauge_lock.current});
    }
    const auto& pool=g.items.status();v.insert(v.end(),{pool.next_index,pool.count,item_slot(pool.head.next),item_slot(pool.tail)});
    return v;
}
static Words item_words(const ItemState* item){
    const int slot=item_slot(item);if(slot<0)return {};
    const auto& i=*item;
    return {slot,i.active,i.type,i.state,i.timer.current,observed->app.game.items.diagnostic_owner(u32(slot)),
        observed->app.game.items.diagnostic_recipient(u32(slot)),bits(i.position.x),bits(i.position.y),
        bits(i.velocity.x),bits(i.velocity.y),bits(i.target.x),bits(i.target.y),item_slot(i.next),item_slot(i.previous),i.max_value};
}
static Snapshot snapshot(){
    Snapshot result;result.values=values();const auto& pool=observed->app.game.items.status();
    for(u32 slot=0;slot<ItemPoolState::capacity;++slot)if(pool.items[slot].active)result.items.push_back(item_words(&pool.items[slot]));
    return result;
}
int Seat(const PilotResources* p){if(observed)for(u32 i=0;i<observed->app.session.player_count;++i)if(p==&observed->app.session.pilot_resources[i])return int(i);return -1;}
int Seat(const PlayerLifeContext* p){if(observed)for(u32 i=0;i<observed->app.session.player_count;++i)if(p==&observed->app.game.pilot(i).status().context)return int(i);return -1;}
void Event(const char* name,int seat,int argument,const ItemState* item,bool after){
    if(!observed||!active)return;
    auto& attempt=active->current;
    if(attempt.events.size()>=max_events){attempt.overflow=recording_overflow=true;return;}
    attempt.events.push_back({name,seat,argument,after,values(),item_words(item)});
}
void Begin(BrowserRuntime& r,u32 frame,u32 predicted,bool correcting){
    if(observed!=&r||frozen||!r.app.in_game())return;
    if(run_generation!=r.app.session.netplay.Generation()){
        observed_setup=r.app.session.multiplayer_session;observed_player=r.app.session.local_player;
        run_generation=r.app.session.netplay.Generation();frames.clear();active=nullptr;stored_bytes=0;dropped_frames=0;first_frame=frame;
        recording_overflow=false;restore_mismatch=~u32(0);restore_checks=0;restore_expected={};restore_actual={};
    }
    if(frame<first_frame)return;
    active=nullptr;
    for(auto& f:frames)if(f.frame==frame){
        stored_bytes-=f.cost;f.cost=0;
        if(f.revision==1)f.first=std::move(f.current);
        f.current=Attempt{};active=&f;break;
    }
    if(!active){
        if(frames.size()>=max_frames&&!retire_oldest()){recording_overflow=true;freeze();return;}
        frames.emplace_back();active=&frames.back();active->frame=frame;
    }
    ++active->revision;active->predicted=predicted;active->correcting=correcting;active->current.begin=snapshot();
    for(u32 seat=0;seat<r.app.session.player_count;++seat){const auto& in=r.app.session.network_frame.inputs[seat];
        active->current.inputs.insert(active->current.inputs.end(),{in.buttons,int(in.analogMode),bits(in.x),bits(in.y),int(in.unlimited),int(in.touchUsed),int(in.touchBomb)});}
    Event("frame.begin");
}
void End(BrowserRuntime& r){
    if(observed!=&r||!active)return;
    Event("frame.end");active->current.end=snapshot();active->current.complete=true;
    active->cost=sizeof(Frame)+memory(active->current)+memory(active->first);stored_bytes+=active->cost;active=nullptr;
    while(stored_bytes>max_bytes){if(!retire_oldest()){recording_overflow=true;freeze();break;}}
}
void Disable(BrowserRuntime& r){if(observed==&r){observed=nullptr;active=nullptr;frames.clear();stored_bytes=0;}}
void Restored(BrowserRuntime& r,u32 frame){
    if(observed!=&r||frozen||restore_mismatch!=~u32(0))return;
    active=nullptr;
    for(const auto& f:frames)if(f.frame==frame){
        ++restore_checks;auto actual=snapshot();
        if(!(f.current.begin==actual)){restore_mismatch=frame;restore_expected=f.current.begin;restore_actual=std::move(actual);}
        break;
    }
}
static void json_words(std::ostream& out,const Words& v){out<<'[';for(std::size_t i=0;i<v.size();++i){if(i)out<<',';out<<v[i];}out<<']';}
static void json_snapshot(std::ostream& out,const Snapshot& s){
    out<<"{\"values\":";json_words(out,s.values);out<<",\"items\":[";
    for(std::size_t i=0;i<s.items.size();++i){if(i)out<<',';json_words(out,s.items[i]);}out<<"]}";
}
static void json_attempt(std::ostream& out,const Attempt& a,bool detail){
    out<<"{\"inputs\":";json_words(out,a.inputs);
    out<<",\"complete\":"<<(a.complete?"true":"false")<<",\"overflow\":"<<(a.overflow?"true":"false")<<",\"end\":";json_snapshot(out,a.end);
    if(detail){out<<",\"begin\":";json_snapshot(out,a.begin);out<<",\"events\":[";
        for(std::size_t i=0;i<a.events.size();++i){if(i)out<<',';const auto& e=a.events[i];
            out<<"{\"kind\":\""<<e.name<<"\",\"seat\":"<<e.seat<<",\"argument\":"<<e.argument<<",\"after\":"<<(e.after?"true":"false")<<",\"values\":";json_words(out,e.values);
            out<<",\"item\":";json_words(out,e.item);out<<'}';}out<<']';}
    out<<'}';
}
}
extern "C" {
__attribute__((export_name("multiplayer_resource_trace_enable")))
th08::u32 multiplayer_resource_trace_enable(th08::BrowserRuntime* r,th08::u32 start){
    using namespace th08::multiplayer::diagnostic;
    if(!r||!r->app.in_game())return 0;observed=r;observed_setup=r->app.session.multiplayer_session;observed_player=r->app.session.local_player;
    first_frame=start;frames.clear();active=nullptr;
    stored_bytes=0;dropped_frames=0;frozen=false;run_generation=r->app.session.netplay.Generation();
    recording_overflow=false;restore_mismatch=~th08::u32(0);restore_checks=0;restore_expected={};restore_actual={};return 1;
}
__attribute__((export_name("multiplayer_resource_trace_freeze")))
th08::u32 multiplayer_resource_trace_freeze(th08::BrowserRuntime* r){using namespace th08::multiplayer::diagnostic;if(observed!=r||active)return 0;if(!frozen)freeze();return 1;}
__attribute__((export_name("multiplayer_resource_trace_read")))
const char* multiplayer_resource_trace_read(th08::BrowserRuntime* r,th08::u32 frame,th08::u32 detail){
    using namespace th08::multiplayer::diagnostic;
    static std::string text;std::ostringstream out;
    if(observed!=r)return "null";
    const auto& session=r->app.session;const auto& net=session.netplay;const auto& setup=observed_setup;
    out<<"{\"schema\":1,\"localPlayer\":"<<observed_player<<",\"playerCount\":"<<setup.player_count
        <<",\"sessionId\":\""<<setup.session_id<<"\",\"generation\":"<<run_generation<<",\"seed\":"<<setup.seed
        <<",\"difficulty\":"<<setup.difficulty<<",\"loadouts\":[";
    for(th08::u32 i=0;i<setup.player_count;++i){if(i)out<<',';out<<setup.characters[i];}
    out<<"],\"nextFrame\":"<<(frozen?frozen_next:net.NextFrame())<<",\"lastFrame\":"<<(frozen?frozen_last:net.LastFrame())<<",\"confirmedThrough\":"<<(frozen?frozen_confirmed:net.ConfirmedThrough())
        <<",\"rollbackFrame\":"<<(frozen?frozen_rollback:net.RollbackFrame())<<",\"storedBytes\":"<<stored_bytes<<",\"droppedFrames\":"<<dropped_frames
        <<",\"oldestFrame\":"<<(frames.empty()?~th08::u32(0):frames.front().frame)<<",\"latestFrame\":"<<(frames.empty()?~th08::u32(0):frames.back().frame)
        <<",\"frozen\":"<<(frozen?"true":"false")<<",\"traceOverflow\":"<<(recording_overflow?"true":"false")<<",\"restoreChecks\":"<<restore_checks<<",\"restoreMismatch\":";
    if(restore_mismatch==~th08::u32(0))out<<"null";else out<<restore_mismatch;
    if(detail&&restore_mismatch!=~th08::u32(0)){out<<",\"restoreExpected\":";json_snapshot(out,restore_expected);out<<",\"restoreActual\":";json_snapshot(out,restore_actual);}
    out<<",\"record\":";bool found=false;
    for(const auto& f:frames)if(f.frame==frame){found=true;out<<"{\"frame\":"<<frame<<",\"revision\":"<<f.revision<<",\"predictedMask\":"<<f.predicted
        <<",\"correcting\":"<<(f.correcting?"true":"false")<<",\"attempt\":";json_attempt(out,f.current,detail!=0);
        if(detail&&f.revision>1){out<<",\"firstAttempt\":";json_attempt(out,f.first,true);}out<<'}';break;}
    if(!found)out<<"null";out<<'}';text=out.str();return text.c_str();
}
}
#endif
