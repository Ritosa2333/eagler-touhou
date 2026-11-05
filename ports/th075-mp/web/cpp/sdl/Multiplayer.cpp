#if TH075_MULTIPLAYER
#include "Multiplayer.hpp"
#include "Application.hpp"
#include "RollbackState.hpp"
#include "../multiplayer/DelaySession.hpp"
#include "../multiplayer/RollbackAudio.hpp"
#include <eagler/netplay/BrowserPeerTransport.hpp>
#include <emscripten.h>
#include <deque>
#include <string>

namespace th075::sdl {extern std::unique_ptr<Application> app;BattleSystemInput sample();}
namespace th075::sdl::multiplayer {
using u64=std::uint64_t;
namespace {
Netplay::BrowserPeerTransport transport;
std::unique_ptr<th075::multiplayer::DelaySession> session;
Netplay::SessionConfig contract;
std::deque<Netplay::SpectatorFramePacket> spectatorFrames;
std::array<ControlCounts,2> counts;
std::array<i32,2> cancel{};
std::array<u32,64> status{};
std::array<u32,32> calibration{};
u32 seed=0,frame=0,restarts=0,first=0,other=0,firstCards=0,otherCards=0,difficulty=0;
bool enabled=false,spectator=false,restartPending=false,finishing=false,finished=false,saved=false;
bool timingSent=false,timingReceived=false,spectatorStopped=false;
bool connectionEnded=false;
u32 receivedFrame=0;
std::string failure;
std::vector<u8> timingBytes,output;
std::deque<std::vector<u8>> spectatorOutput;
u64 finishAt=0;
std::array<u32,8> confirmedActors{};u32 confirmedReplayFrame=0;
u32 confirmedCheckpoint=Netplay::INVALID_FRAME,confirmedDigest=0;
u32 digest=0,rollbackCount=0,resimulated=0,resimTarget=0;
struct History {
 u32 frame,digestBefore,digestAfter;std::array<ControlCounts,2> counts;std::array<i32,2> cancel;
 std::unique_ptr<RollbackState> state;std::vector<Application::RollbackSound> sounds;
 u32 replayFrame=0;std::array<u32,8> actors{};
};
std::deque<History> history;
th075::multiplayer::RollbackAudio<Application::RollbackSound> rollbackAudio;
void play_sound(const Application::RollbackSound& sound){if(sound.character)app->audio.character_sound(sound.side,sound.id);else app->audio.sound(sound.id);}
void stop_sound(const Application::RollbackSound& sound){app->audio.stop_sound(sound.character?u8(sound.side+1):0,sound.id);}

u64 integer(const u8* p,unsigned n){u64 v=0;for(unsigned i=0;i<n;++i)v|=u64(p[i])<<(8*i);return v;}
void put(std::vector<u8>& b,u64 v,unsigned n){for(unsigned i=0;i<n;++i)b.push_back(u8(v>>(i*8)));}
bool fail(const char* text){failure=text;if(app)app->error=failure;return false;}
u32 world_digest(){
    u32 hash=2166136261u;
    const auto word=[&](u32 v){for(unsigned i=0;i<4;++i){hash^=u8(v>>(i*8));hash*=16777619u;}};
    const auto number=[&](float v){u32 b;std::memcpy(&b,&v,4);word(b);};
    word(frame);word(restarts);
    if(!app->battle)return hash;
    auto& b=app->battle->simulation;
    number(b.story.camera.x);number(b.story.camera.y);number(b.story.camera.span);
    word(b.replay.random.state);word(unsigned(b.rules.phase));word(b.rules.countdown);
    word(app->battle->progress.paused);word(app->battle->progress.pauseChoice);word(app->battle->progress.pauseFade);word(app->battle->timing.replayFrame);
    for(unsigned side=0;side<2;++side){
        auto& a=b.characters[side]->actor;
        number(a.motion.x);number(a.motion.y);number(a.motion.velocityX);number(a.motion.velocityY);
        word(a.motion.facing);word(a.animation.cursor.action);word(a.animation.cursor.index);word(a.animation.cursor.elapsed);
        word(a.combat.health);word(a.combat.spellPhase);word(a.combat.spellCharge);word(a.combat.freeze);word(a.score);
        word(a.controls.current.held());for(const auto v:a.controls.history.frames)word(v);
        for(u8 layer=0;layer<4;++layer){word(b.objects[side].size(layer));for(u32 n=0;n<b.objects[side].size(layer);++n){
            auto& o=b.objects[side].get(layer,n);word(o.animation.cursor.action);word(o.animation.cursor.index);word(o.animation.cursor.elapsed);
            number(o.motion.x);number(o.motion.y);number(o.motion.velocityX);number(o.motion.velocityY);word(o.alive);word(o.hits);
        }}
    }
    for(u8 layer=0;layer<2;++layer){word(b.effects.size(layer));for(u32 n=0;n<b.effects.size(layer);++n){
        auto& e=b.effects.get(layer,n);word(e.cursor.action);word(e.cursor.index);word(e.cursor.elapsed);word(e.age);
        number(e.motion.x);number(e.motion.y);number(e.scale[0]);number(e.scale[1]);word(e.alpha);word(e.alive);
    }}
    return hash;
}
void remember_state(){
    status.fill(0);status[0]=1;status[1]=finished?5:!failure.empty()?7:connectionEnded||transport.Disconnected()?4:
        spectator?(timingReceived?3:1):session&&session->Ready()?3:1;
    status[2]=frame;status[3]=session?session->Confirmed():frame;status[4]=spectator;
    status[5]=contract.localPlayer;status[6]=session?session->Delay():timingBytes.size()==40?timingBytes[6]:0;
    status[7]=digest;status[8]=restarts;
    status[48]=rollbackCount;status[49]=resimulated;status[50]=u32(history.size());status[51]=session?session->PredictionLimit():0;status[52]=session?session->Mode():timingBytes.size()==40?timingBytes[5]:1;status[53]=confirmedCheckpoint;status[54]=confirmedDigest;status[55]=confirmedReplayFrame;std::copy(confirmedActors.begin(),confirmedActors.end(),status.begin()+56);
    if(app&&app->battle){auto& b=*app->battle;status[9]=unsigned(b.simulation.rules.phase);status[10]=b.progress.paused;
        status[11]=b.simulation.replay.random.state;status[12]=b.timing.replayFrame;status[13]=u32(b.progress.pauseChoice);status[14]=b.progress.pauseFade;status[15]=finishing;
        for(unsigned side=0;side<2;++side){auto& a=b.simulation.characters[side]->actor;auto at=16+side*16;
            std::memcpy(&status[at],&a.motion.x,4);std::memcpy(&status[at+1],&a.motion.y,4);
            status[at+2]=u32(a.combat.health);status[at+3]=u32(a.animation.cursor.action);status[at+4]=a.controls.current.held();
            status[at+5]=b.simulation.objects[side].size(0)+b.simulation.objects[side].size(1);status[at+6]=a.score;
        }
    }
}
void stop_spectators(){spectatorStopped=true;spectatorOutput.clear();transport.StopSpectators();}
void publish_timing(){
    if(timingSent||spectator||contract.localPlayer||!session||!session->Ready())return;
    const auto& s=session->Startup();const auto& a=s.ReportFor(0);const auto& b=s.ReportFor(1);
    timingBytes={'E','P','T','M',1,u8(session->Mode()),u8(session->Delay()),u8(session->PredictionLimit())};
    put(timingBytes,s.Selected().fullDelay,4);put(timingBytes,seed,4);put(timingBytes,contract.sessionId,8);
    put(timingBytes,std::max(a.p95Us,b.p95Us),4);put(timingBytes,a.lost+b.lost,4);
    put(timingBytes,EM_ASM_INT({return !!Module.eaglerOptions?.netplayInputDelayAuto;}),4);
    put(timingBytes,std::min(a.received,b.received),4);
    timingSent=true;spectatorOutput.push_back(timingBytes);
}
void upload(){
    if(spectator||contract.localPlayer||spectatorStopped)return;
    if(transport.SpectatorState()<0){stop_spectators();return;}
    // The upload lane is optional: it can never stop either player's world.
    for(unsigned n=0;n<32&&!spectatorOutput.empty();++n){const auto& bytes=spectatorOutput.front();
        if(!transport.SendSpectator(bytes.data(),bytes.size()))break;
        spectatorOutput.pop_front();
    }
    if(spectatorOutput.size()>4096)stop_spectators();
}
bool receive_spectator(){
    std::vector<u8> bytes;
    for(unsigned n=0;n<256&&transport.Poll(&bytes);++n){
        if(bytes.size()==40&&bytes[0]=='E'&&bytes[1]=='P'&&bytes[2]=='T'&&bytes[3]=='M'){
            if(bytes[4]!=1||(bytes[5]>1)||bytes[6]>9||(bytes[5]==1?bytes[7]!=0:bytes[7]<1||bytes[7]>8||bytes[6]!=0)||integer(bytes.data()+12,4)!=seed||integer(bytes.data()+16,8)!=contract.sessionId)
                return fail("Invalid TH075 spectator timing contract");
            const auto full=integer(bytes.data()+8,4),rtt=integer(bytes.data()+24,4),lost=integer(bytes.data()+28,4),automatic=integer(bytes.data()+32,4),samples=integer(bytes.data()+36,4);
            if(!rtt||rtt>1'000'000||lost>72||automatic>1||samples<96||samples>120||
               full!=std::max<u64>(1,((rtt/2)*60+999'999)/1'000'000)||(automatic&&(bytes[5]!=1||bytes[6]!=full)))
                return fail("Invalid TH075 spectator measurement");
            if(timingReceived&&timingBytes!=bytes)return fail("Conflicting TH075 spectator timing");
            timingBytes=bytes;timingReceived=true;continue;
        }
        Netplay::SpectatorFramePacket p;
        if(!timingReceived||!Netplay::DecodeSpectatorFramePacket(bytes.data(),bytes.size(),&p)||p.sessionId!=contract.sessionId||
           p.gameplayAbi!=contract.gameplayAbi||p.playerCount!=2)return fail("Invalid TH075 spectator frame");
        if(p.frame<receivedFrame)continue;
        if(p.frame!=receivedFrame||!th075::multiplayer::DelaySession::Valid(p.inputs[0])||!th075::multiplayer::DelaySession::Valid(p.inputs[1]))
            return fail("TH075 spectator input gap");
        if(spectatorFrames.size()>=8192)return fail("TH075 spectator fell behind");
        spectatorFrames.push_back(p);++receivedFrame;
    }
    return true;
}
bool play(const std::array<Netplay::FrameInput,Netplay::MAX_PLAYERS>& inputs){
    BattleSystemInput input;
    for(unsigned side=0;side<2;++side){const auto bits=inputs[side].buttons;counts[side].from_held(u8(bits));input.players[side]=counts[side];
        if(bits&256)input.keys[1]=128;cancel[side]=(bits&512)?increment_wrapped(cancel[side],1):0;input.cancel[side]=cancel[side];
    }
    if((inputs[0].buttons|inputs[1].buttons)&1024){
        if(!spectator)app->save_multiplayer_replay();++restarts;counts={};cancel={};
        if(!app->prepare_multiplayer(seed+restarts*0x9e3779b9u,u8(first),u8(other),u8(firstCards),u8(otherCards),u8(difficulty)))return false;
    }else if(!app->tick(input,u32(u64(frame)*1000/60)))return false;
    // Hash only at the actual comparison cadence, including on mobile CPUs.
    if(frame%60==0)digest=world_digest();return true;
}
}
bool active(){return enabled;}
bool pump(double us){
    if(!enabled)return true;
    // A peer can close just before our post-fence grace expires. A locally
    // confirmed and acknowledged terminal frame is already safe to retire.
    if(finishing&&session&&session->CanFinish()&&transport.Disconnected()){
        finished=true;remember_state();return true;
    }
    if(connectionEnded||transport.Disconnected()){remember_state();return true;}
    if(spectator){
        if(!receive_spectator())return false;
        // Drain an already received terminal backlog; an exhausted broken
        // spectator stream must fail explicitly instead of freezing forever.
        if(transport.Failed()&&spectatorFrames.empty()&&!finished)return fail(transport.LastError().c_str());
    }
    else{
        if(!session->Pump(u64(us))){
            if(session->ConnectionEnded()){connectionEnded=true;EM_ASM({globalThis.__eaglerPeerTransport?.fail?.('TH075 peer connection ended');});remember_state();return true;}
            return fail(session->Error());
        }
        publish_timing();upload();
        if(finishing){session->FlushFinish(u64(us));if(session->CanFinish()){
            if(!finishAt)finishAt=u64(us)+1'000'000;
            if(u64(us)>=finishAt)finished=true;
        }}
    }
    remember_state();return true;
}
double pace(double ms){return session?session->Pace(ms):ms;}
// Only reconciled, contiguous inputs may publish hashes, sounds or spectator frames.
bool retire(){
    while(!history.empty()){
        auto& h=history.front();std::array<Netplay::FrameInput,Netplay::MAX_PLAYERS> inputs;
        if(!session->ConfirmedInputs(h.frame,&inputs))break;
        if(!session->VerifyAt(h.frame,h.digestAfter))return fail(session->Error());
        if(h.frame%60==0){confirmedCheckpoint=h.frame;confirmedDigest=h.digestAfter;}
        confirmedReplayFrame=h.replayFrame;confirmedActors=h.actors;
        rollbackAudio.Confirm(h.frame);
        if(!contract.localPlayer&&!spectatorStopped){Netplay::SpectatorFramePacket p;p.sessionId=contract.sessionId;p.gameplayAbi=contract.gameplayAbi;p.frame=h.frame;p.playerCount=2;p.inputs=inputs;
            if(Netplay::EncodeSpectatorFramePacket(p,&output))spectatorOutput.push_back(output);}
        history.pop_front();
    }
    return true;
}
bool advance_one(double us,bool resimulation,bool& advanced){
    advanced=false;
    std::array<Netplay::FrameInput,Netplay::MAX_PLAYERS> inputs{};Netplay::FrameDecision decision;
    bool speculative=false;
    if(spectator){if(spectatorFrames.empty())return true;inputs=spectatorFrames.front().inputs;}
    else{
        if(!session->Ready())return true;
        if(session->NeedsCapture())return fail("TH075 simulation attempted uncaptured physical input");
        decision=session->Prepare();if(!decision.canAdvance)return true;inputs=decision.inputs;
        speculative=!session->InputConfirmed(frame);
        // Lifecycle and menu actions never run against an unconfirmed past.
        if(speculative&&(!app->rollback_predictable()||((inputs[0].buttons|inputs[1].buttons)&0x700)))return true;
    }
    History h{frame,digest,digest,counts,cancel,nullptr,{}};
    if(speculative){h.state=app->capture_rollback();if(!h.state)return fail("TH075 rollback capture unavailable");}
    app->rollbackSounds.clear();app->collectRollbackSound=!spectator&&session->Rollback();
    app->rollbackResimulating=resimulation;app->graphics.suppress(resimulation);
    const bool played=play(inputs);
    app->graphics.suppress(false);app->rollbackResimulating=false;app->collectRollbackSound=false;
    if(!played)return false;
    if(speculative&&!app->rollback_predictable()){
        // A predicted hit may end a round. Restore before committing that edge;
        // it is replayed exactly once when all preceding input is confirmed.
        if(!app->restore_rollback(*h.state))return fail("TH075 lifecycle restore failed");
        counts=h.counts;cancel=h.cancel;digest=h.digestBefore;app->rollbackSounds.clear();return true;
    }
    if(spectator){spectatorFrames.pop_front();if(frame%60==0){confirmedCheckpoint=frame;confirmedDigest=digest;}}
    else{
        h.digestAfter=digest;h.replayFrame=app->battle->timing.replayFrame;
        for(unsigned side=0;side<2;++side){const auto& a=app->battle->simulation.characters[side]->actor;std::memcpy(&h.actors[side*4],&a.motion.x,4);std::memcpy(&h.actors[side*4+1],&a.motion.y,4);h.actors[side*4+2]=u32(a.combat.health);h.actors[side*4+3]=u32(a.animation.cursor.action);}
        h.sounds=std::move(app->rollbackSounds);history.push_back(std::move(h));
        if(session->Rollback())rollbackAudio.Submit(frame,history.back().sounds,play_sound);
        if(!session->Complete(decision))return fail(session->Error());
    }
    ++frame;advanced=true;if(resimulation)++resimulated;
    if(!spectator&&!retire())return false;
    if(app->multiplayer_complete()){
        if(!history.empty())return fail("TH075 terminal frame is not confirmed");
        finishing=true;if(spectator)finished=true;
        if(!spectator&&!saved){saved=true;if(!app->save_multiplayer_replay())return false;}
    }
    return true;
}
bool step(const BattleSystemInput&,double us,bool* forwardAdvanced){
    if(forwardAdvanced)*forwardAdvanced=false;
    if(!enabled||finished||finishing||connectionEnded||transport.Disconnected())return true;
    // Send the current physical sample before historical work. Reconciliation
    // must never postpone the peer's fresh input behind our snapshot/replay cost.
    if(!spectator&&session->NeedsCapture()){
        const auto raw=th075::sdl::sample();u16 bits=raw.players[0].held();if(raw.keys[1])bits|=256;if(raw.cancel[0])bits|=512;if(restartPending){bits|=1024;restartPending=false;}
        if(!session->Capture(bits,u64(us)))return fail(session->Error());
    }
    if(!spectator&&session->HasRollbackRequest()){
        const auto first=session->RollbackFrame();auto at=std::find_if(history.begin(),history.end(),[&](const auto& h){return h.frame==first;});
        if(at==history.end()||!at->state)return fail("TH075 rollback snapshot expired");
        if(!app->restore_rollback(*at->state)||!session->Rewind(first))return fail("TH075 rollback restore failed");
        resimTarget=std::max(resimTarget,frame);frame=first;counts=at->counts;cancel=at->cancel;digest=at->digestBefore;
        rollbackAudio.Begin(first);history.erase(at,history.end());++rollbackCount;
    }
    if(!spectator&&!retire())return false;
    const double reconcileStarted=emscripten_get_now();
    for(unsigned n=0;n<9&&frame<resimTarget;++n){
        // A slow endpoint must yield to browser input/network/audio between
        // historical ticks. Keep the target and snapshots for the next callback.
        if(n&&emscripten_get_now()-reconcileStarted>=4.)break;
        bool advanced;if(!advance_one(us,true,advanced))return false;
        if(!advanced){remember_state();return true;}
    }
    if(frame<resimTarget){remember_state();return true;}
    rollbackAudio.Finish(stop_sound);
    bool advanced;if(!advance_one(us,false,advanced))return false;
    if(forwardAdvanced)*forwardAdvanced=advanced;
    remember_state();return true;
}
void render(){} // A missing peer frame must not re-run BattleScene::render's camera update.
}
extern "C" {
EMSCRIPTEN_KEEPALIVE int th075_mp_prepare(unsigned seed,unsigned first,unsigned other,unsigned a,unsigned b,unsigned difficulty){
    if(first>10||other>10||a>8||b>8||difficulty>3||!th075::sdl::app)return 0;
    auto& app=*th075::sdl::app;if(!app.prepare_multiplayer(seed,th075::u8(first),th075::u8(other),th075::u8(a),th075::u8(b),th075::u8(difficulty)))return 0;
    app.render_battle(*app.battle,true);app.present_battle();app.graphics.renderer.commit();return 1;
}
EMSCRIPTEN_KEEPALIVE int th075_mp_begin(const char* url,const char* spectatorId,unsigned side,unsigned low,unsigned high,unsigned abi,unsigned seed,unsigned first,unsigned other,unsigned a,unsigned b,unsigned difficulty,unsigned delay,unsigned automatic){
    using namespace th075::sdl::multiplayer;
    const auto mode=unsigned(EM_ASM_INT({return Module.eaglerOptions?.netplayAdonisMode??1;}));
    const auto limit=unsigned(EM_ASM_INT({return Module.eaglerOptions?.netplayPredictionLimit??0;}));
    if(mode>1||(mode==0&&(limit<1||limit>8||automatic||delay))||(mode==1&&limit))return 0;
    if(enabled||!abi||first>10||other>10||a>8||b>8||difficulty>3||delay>9||automatic>1||side>2)return 0;
    contract={};contract.sessionId=std::uint64_t(low)|(std::uint64_t(high)<<32);contract.seed=seed;contract.gameplayAbi=abi;contract.gameId=75;contract.playerCount=2;contract.localPlayer=th075::u8(side==2?0:side);
    if(!contract.sessionId)return 0;
    spectator=side==2;
    if(!(spectator?transport.ConnectSpectator(url,spectatorId,2):transport.Connect(url,th075::u8(side),2)))return 0;
    th075::sdl::multiplayer::seed=seed;th075::sdl::multiplayer::first=first;th075::sdl::multiplayer::other=other;
    firstCards=a;otherCards=b;th075::sdl::multiplayer::difficulty=difficulty;
    frame=restarts=receivedFrame=digest=rollbackCount=resimulated=resimTarget=0;history.clear();rollbackAudio.Reset();confirmedCheckpoint=Netplay::INVALID_FRAME;confirmedDigest=confirmedReplayFrame=0;confirmedActors={};finishAt=0;enabled=true;restartPending=finishing=finished=saved=timingSent=timingReceived=spectatorStopped=connectionEnded=false;
    failure.clear();counts={};cancel={};spectatorFrames.clear();spectatorOutput.clear();timingBytes.clear();
    if(!spectator){session=std::make_unique<th075::multiplayer::DelaySession>(transport);if(!session->Begin(contract,std::uint64_t(emscripten_get_now()*1000),automatic?Netplay::AdonisStartup::Automatic:delay,mode,limit))return 0;}
    remember_state();return 1;
}
#if TH075_DEVELOPMENT_HARNESS
// Diagnostic-only restore equivalence gate. It never enters published Wasm.
EMSCRIPTEN_KEEPALIVE int th075_probe_rollback(unsigned span){
 using namespace th075::sdl::multiplayer;using namespace th075;using namespace th075::sdl;
 const unsigned phase=span>>8;span&=255;
 if(enabled||!app||!app->battle||span<1||span>16||phase>3)return 0;
 counts={};cancel={};frame=0;restarts=0;digest=0;app->collectRollbackSound=true;app->graphics.suppress(true);
 const auto run=[&](unsigned ticks,unsigned variant,std::vector<u32>* trace,bool draw=false){app->graphics.suppress(!draw);for(unsigned n=0;n<ticks;++n){
  std::array<Netplay::FrameInput,Netplay::MAX_PLAYERS> inputs{};
  for(unsigned side=0;side<2;++side){const auto t=frame+variant*7+side*13;inputs[side].buttons=u16(((t/19)%2?1:2)|((t%11<3)?(16u<<((t/11)%4)):0));}
  if(!play(inputs))return false;
  if(trace)trace->push_back(world_digest());++frame;
 }return true;};
 if(!run(phase==1?45:360,0,nullptr))return 0;
 if(phase==2){app->battle->simulation.rules.phase=BattlePhase::RoundKnockout;app->battle->simulation.rules.countdown=15;}
 if(phase==3){app->battle->simulation.rules.phase=BattlePhase::RoundRecovery;app->battle->simulation.rules.countdown=155;}
 auto checkpoint=app->capture_rollback();const auto initialCounts=counts;const auto initialCancel=cancel;const auto initialFrame=frame;
 std::vector<u32> expected,actual;
 if(!run(span,0,&expected,true))return 0;
 const ReplayRound expectedRecord=*app->battle->simulation.replay.round;
 const auto reset=[&]{if(!app->restore_rollback(*checkpoint))return false;counts=initialCounts;cancel=initialCancel;frame=initialFrame;return true;};
 if(!reset()||!run(span,1,nullptr)||!reset()||!run(span,0,&actual))return 0;
 const auto& record=*app->battle->simulation.replay.round;
 const bool same=expected==actual&&expectedRecord.playerInputs==record.playerInputs&&expectedRecord.randomValues==record.randomValues&&expectedRecord.auxiliaryInputs==record.auxiliaryInputs&&expectedRecord.events==record.events;
 app->collectRollbackSound=false;app->rollbackSounds.clear();app->graphics.suppress(false);
 return same?1:0;
}
#endif

EMSCRIPTEN_KEEPALIVE int th075_mp_poll(double nowUs){return th075::sdl::multiplayer::pump(nowUs);}
EMSCRIPTEN_KEEPALIVE const unsigned* th075_mp_status(){th075::sdl::multiplayer::remember_state();return th075::sdl::multiplayer::status.data();}
EMSCRIPTEN_KEEPALIVE const unsigned char* th075_mp_timing(){using namespace th075::sdl::multiplayer;return timingReceived?timingBytes.data():nullptr;}
EMSCRIPTEN_KEEPALIVE const unsigned* th075_mp_calibration(){
    using namespace th075::sdl::multiplayer;calibration.fill(0);calibration[0]=1;if(!session)return calibration.data();
    const auto& s=session->Startup();constexpr unsigned phases[]{0,1,3,4,5,6,7,8};calibration[1]=phases[unsigned(s.State())];
    calibration[2]=s.Probes();calibration[3]=s.Replies();calibration[4]=2;calibration[5]=contract.localPlayer;calibration[6]=session->Mode();calibration[10]=0;
    calibration[7]=EM_ASM_INT({return !!Module.eaglerOptions?.netplayInputDelayAuto;});calibration[8]=s.Selected().delay;calibration[9]=s.Selected().fullDelay;
    calibration[11]=seed;for(unsigned player=0;player<2;++player){const auto& p=s.ReportFor(player);const auto at=12+player*6;
        calibration[at]=p.p95Us;calibration[at+1]=p.received;calibration[at+2]=p.lost;calibration[at+3]=p.minUs;calibration[at+4]=p.maxUs;calibration[at+5]=p.meanUs;}
    calibration[30]=s.Attempt()|(unsigned(s.Reason())<<8);calibration[31]=s.NextWakeUs();return calibration.data();
}
EMSCRIPTEN_KEEPALIVE void th075_mp_restart(){using namespace th075::sdl::multiplayer;if(enabled&&!spectator&&!finished)restartPending=true;}
EMSCRIPTEN_KEEPALIVE int th075_mp_save_replay(){using namespace th075::sdl::multiplayer;if(!enabled||spectator||saved||frame==0)return 1;
    if(!retire())return 0;
    if(!history.empty()){const auto& h=history.front();if(!h.state||!th075::sdl::app->restore_rollback(*h.state)||!session->Rewind(h.frame))return 0;frame=h.frame;counts=h.counts;cancel=h.cancel;digest=h.digestBefore;history.clear();}
    saved=true;return th075::sdl::app->save_multiplayer_replay();}
EMSCRIPTEN_KEEPALIVE int th075_mp_replay_viewer(){return th075::sdl::app&&th075::sdl::app->show_replays();}
EMSCRIPTEN_KEEPALIVE void th075_mp_end(){using namespace th075::sdl::multiplayer;enabled=false;history.clear();session.reset();transport.Close();spectatorFrames.clear();spectatorOutput.clear();}
}
#endif
