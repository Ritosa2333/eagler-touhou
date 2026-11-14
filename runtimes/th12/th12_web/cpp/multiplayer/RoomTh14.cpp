#include "RoomTh14.hpp"

namespace th11 {
namespace wire=Netplay::SpectatorStageWire;
namespace {
constexpr u32 schema=1,game=14;
constexpr u8 marker='E';
}
bool RoomSetupTh14::valid()const{
    if(!id||seats<2||seats>3||local>=seats||delay>(prediction?8u:9u)||prediction>max_prediction||difficulty>4||seed>65535||
       !(build[0]|build[1]|build[2]|build[3]))return false;
    for(u32 i=0;i<loadouts.size();++i){
        const auto p=loadouts[i];
        if(!player_selection_valid(p.character,p.subtype)||(i>=seats&&(p.character||p.subtype)))return false;
    }
    return true;
}
u32 RoomSetupTh14::abi()const{
    u32 hash=2166136261u;
    for(const auto word:describe())hash=(hash^word)*16777619u;
    return hash?hash:1u;
}
std::vector<u8> RoomSetupTh14::describe()const{
    std::vector<u8> bytes(32);
    bytes[0]=schema;bytes[1]=game;bytes[2]=u8(seats);bytes[3]=u8(difficulty);
    wire::Put(bytes,4,seed,2);bytes[6]=u8(delay);bytes[7]=u8(prediction);
    for(u32 i=0;i<3;++i){bytes[8+i*2]=loadouts[i].character;bytes[9+i*2]=loadouts[i].subtype;}
    for(u32 i=0;i<4;++i)wire::Put(bytes,16+i*4,build[i]);
    return bytes;
}
bool RoomSetupTh14::read(const Netplay::InputReplayInfo& info,const std::array<u32,4>& expected){
    const auto& config=info.config;const auto& b=config.description;
    if(config.gameId!=game||b.size()!=32||b[0]!=schema||b[1]!=game||b[2]!=config.playerCount||b[14]||b[15])return false;
    id=1;seats=b[2];difficulty=b[3];seed=u32(wire::Get(b.data(),4,2));delay=b[6];prediction=b[7];local=config.recordedPlayer;
    for(u32 i=0;i<3;++i)loadouts[i]={b[8+i*2],b[9+i*2]};
    for(u32 i=0;i<4;++i)build[i]=u32(wire::Get(b.data(),16+i*4));
    return valid()&&build==expected&&abi()==config.gameplayAbi;
}
bool RoomTh14::begin(const char* url,const char* spectator_id,u64 now){
    if(!setup.valid()||!url||!*url)return false;
    spectator=spectator_id&&*spectator_id;
    if(spectator)return transport.ConnectSpectator(url,spectator_id,u8(setup.seats),true);
    if(!transport.Connect(url,u8(setup.local),u8(setup.seats),true))return false;
    const Netplay::SessionConfig identity{setup.id,setup.seed,setup.abi(),game,u8(setup.seats),u8(setup.local)};
    Netplay::CoreConfig config;
    config.sessionId=setup.id;config.localPlayer=u8(setup.local);config.playerCount=u8(setup.seats);
    config.inputDelay=u8(setup.delay);config.allowPrediction=setup.prediction!=0;
    config.maxRollbackFrames=u8(setup.prediction?setup.prediction:RoomSetupTh14::max_prediction);
    config.predictableButtons=0xf9u; // Fire, focus and directions; wait for Bomb and Ctrl.
    config.directionButtons=0xf0u;
    config.prediction=Netplay::InputPrediction::ConfirmedHold;
    config.directTouchIsAbsolute=true;
    // Match TH17: extrapolate only unlimited drags, from contiguous
    // confirmed coordinates. Bomb edges are still never predicted.
    config.absoluteTouchForecastFrames=6;
    if(!gate.Reset(identity)||!inputs.Reset(config)||!channel.BeginSession(identity,now)||
       !replay.Begin({game,setup.abi(),u8(setup.seats),u8(setup.local),setup.describe()}))return false;
    if(!setup.local){
        if(!publisher.Reset(marker,u8(setup.seats),setup.id,setup.abi()))return false;
        transport.BeginSpectatorSession(setup.id,setup.seed);
    }
    return true;
}
bool RoomTh14::poll(u64 now){
    if(finished&&channel.Retiring())return channel.PumpRetirement(now);
    return channel.Pump(gate,inputs,now,captured!=0&&!finished);
}
bool RoomTh14::capture(u64 now_us,unsigned lead,const Netplay::FrameInput& local){
    if(!gate.CanStart()||finished)return true;
    const auto head=std::max(frame,replay_until);
    if(captured>=head&&(captured==head||captured-head<lead)&&capture_clock.CanCapture(captured,head,lead!=0,now_us)){
        if(!inputs.ScheduleLocalInput(captured,local)||!channel.LocalCaptured(inputs,captured,now_us/1000))return false;
        capture_clock.Captured(now_us,lead?3:1);
        ++captured;
    }
    return true;
}
std::array<u32,8> th14_room_status(const GameSession& session,u32 next){
    std::array<u32,8> out{next,session.state.stage,u32(session.state.phase)};
    if(const auto* ending=session.ending.get()){
        out[3]=ending->frames;out[4]=u32(ending->time.current);
        out[5]=ending->flags;out[6]=u32(ending->line);out[7]=ending->active;
        return out;
    }
    const auto* b=session.battle.get();
    if(!b)return out;
    u32 pilots=2166136261u,world=pilots;
    const auto add=[](u32& hash,u32 value){hash=(hash^value)*16777619u;};
    const auto pos=[&](u32& hash,const Vec3& p){
        add(hash,float_bits(p.x));add(hash,float_bits(p.y));add(hash,float_bits(p.z));
    };
    for(u32 seat=0;seat<b->coop_seats;++seat){
        const auto& p=b->pilot(seat);const auto& e=b->pilot_economy(seat);
        add(pilots,u32(p.motion.state.x));add(pilots,u32(p.motion.state.y));
        add(pilots,u32(p.state.life_state));add(pilots,u32(p.state.invincibility.current));
        add(pilots,u32(e.lives));add(pilots,u32(e.life_fragments));add(pilots,u32(e.th14_bombs));
        add(pilots,u32(e.th14_bomb_fragments));add(pilots,u32(e.power));add(pilots,u32(e.graze));
        add(pilots,u32(e.point_value));add(pilots,u32(e.score_units));add(pilots,b->ghosts[seat].random);
        add(pilots,b->coop_held[seat]);
        add(pilots,u32(b->coop_marisa_b_cooldown[seat].current));
        add(pilots,u32(e.th14_combo.count));add(pilots,u32(e.th14_combo.timer));
        if(auto* bomb=b->pilot_bomb(seat)){
            add(pilots,u32(bomb->state.active));add(pilots,u32(bomb->state.elapsed.current));add(pilots,u32(bomb->state.th14_mode));
        }
        add(pilots,p.shots.room_state_digest());
    }
    for(auto* node=b->enemies.first;node;node=node->next){
        const auto& e=node->value->state;
        add(world,u32(e.instance_id));add(world,u32(e.health));add(world,e.flags);pos(world,e.current.position);
        add(world,float_bits(node->value->script.root.time));
        for(const auto slowdown:e.th14_coop_knife_slowdown)add(world,float_bits(slowdown));
        for(auto* t=node->value->script.threads.next;t;t=t->next){
            add(world,u32(t->value->thread_id));add(world,float_bits(t->value->time));
        }
    }
    b->bullets.room_digest(world);b->items.room_digest(world);
    for(auto* laser=b->lasers.first();laser;laser=laser->next){
        add(world,u32(laser->id));add(world,u32(laser->type));pos(world,laser->position);
        add(world,float_bits(laser->angle));add(world,float_bits(laser->length));add(world,u32(laser->lifetime.current));
    }
    add(world,b->spell_flags);add(world,u32(b->spell_id));add(world,u32(b->spell_bonus));add(world,u32(b->spell_elapsed));
    add(world,u32(b->economy.th14_point_items));
    add(world,u32(b->coop_bomb_invincibility.current));add(world,u32(b->coop_knife_owner));add(world,u32(b->items.th14_poc_owner));
    add(world,u32(b->control_frame()));add(world,float_bits(session.animations.rate));
    const auto rng=[&](const Rng& r){add(world,r.seed_value());add(world,r.call_count());};
    rng(session.animations.visual_rng);rng(session.animations.script_rng);
    out[3]=pilots;out[4]=world;out[5]=b->bullets.active_count;out[6]=b->lasers.active_count;
    out[7]=b->enemies.count;return out;
}
bool RoomTh14::record(u32 number,const Netplay::InputReplay::Frame& keys,u32 stage,bool restart,u64){
    // Keep the network timeline and the menu inputs that caused each restart.
    // Replay chapters must have unique labels, even across repeated stages.
    const auto& info=replay.Info();
    const auto chapter=info.chapterCount==info.chapters.size()?info.chapters.back().label:replay_stage_offset+stage;
    if(!replay.Append(number,chapter,keys.data(),setup.seats))return false;
    if(restart)replay_stage_offset+=7;
    publish_confirmed();
    return true;
}
void RoomTh14::publish_confirmed(){
    if(!setup.local&&publish){
        // The confirmed replay already retains every input. Feed the small
        // publisher ring only as its socket drains, including after a stall.
        const auto send=[&](const auto& bytes){return transport.SendSpectator(bytes.data(),bytes.size());};
        if(transport.SpectatorState()!=1)return;
        for(u32 sent=0;sent<16&&publisher.Published()<replay.Info().frameCount;++sent){
            Netplay::SpectatorStagePublisher::Frame record;
            record.frame=publisher.Published();record.stage=setup.difficulty==4?7:1;
            if(!record.frame){record.schema=schema;record.initializer=setup.describe();}
            const auto* recorded=replay.FrameAt(record.frame);
            if(!recorded){publish=false;break;}
            record.inputs=*recorded;record.hasInputs=true;
            const auto frame=record.frame;
            if(!publisher.Record(std::move(record))){publish=false;break;}
            Netplay::SpectatorStagePublisher::PublishFailure failure;
            if(!publisher.Publish(frame,frame,[](u32,auto&){return false;},send,&failure)){
                publish=publisher.Retryable(failure);break;
            }
        }
        if(!publish)transport.StopSpectators();
    }
}
bool RoomTh14::finish_publication(u64 now){
    publish_confirmed();
    if(!setup.local&&publish){
        if(!publication_deadline)publication_deadline=now+8000;
        const auto send=[&](const auto& bytes){return transport.SendSpectator(bytes.data(),bytes.size());};
        if(publisher.Published()==confirmed&&publisher.Finish(confirmed,send)){
            transport.StopSpectators(true);publish=false;
        }else if(now>=publication_deadline||transport.SpectatorState()<0){
            transport.StopSpectators();publish=false;
        }
    }
    if(channel.Retiring())return true;
    return channel.FlushRetirementFence(inputs,confirmed-1,now)&&
        (!channel.CanRetire(inputs,confirmed-1)||channel.Retire(inputs,confirmed-1,now));
}
bool RoomTh14::receive(){
    if(transport.Failed()||transport.Disconnected())return false;
    u64 session;u32 seed;
    if(transport.TakeSpectatorSession(&session,&seed)){
        if(session!=setup.id||seed!=setup.seed)return false;
        viewer.Clear();view.Clear();
    }
    std::vector<u8> bytes;
    for(u32 n=0;n<256&&transport.Poll(&bytes);++n){
        if(Netplay::IsSpectatorStageEvent(bytes.data(),bytes.size())){
            Netplay::SpectatorStageEvent event;
            if(!Netplay::DecodeSpectatorStageEvent(bytes.data(),bytes.size(),&event)||event.game!=marker||
               event.playerCount!=setup.seats||event.sessionId!=setup.id||event.gameplayAbi!=setup.abi()||
               (event.kind==Netplay::SpectatorEventKind::Checkpoint&&(event.schema!=schema||event.state!=setup.describe()))||
               !viewer.Receive(event))return false;
        }else{
            Netplay::SpectatorFramePacket packet;
            if(!Netplay::DecodeSpectatorFramePacket(bytes.data(),bytes.size(),&packet)||!viewer.Receive(packet))return false;
        }
    }
    return true;
}
GameSessionInput RoomTh14::to_input(const Netplay::InputReplay::Frame& inputs){
    GameSessionInput frame;
    for(u32 i=0;i<inputs.size();++i){
        const auto& p=inputs[i];auto& seat=frame.coop.seats[i];
        frame.room_menu_buttons[i]=p.buttons&0x1f3u;
        seat.held=p.buttons&0x2fbu;
        if(p.analogMode==Netplay::AnalogMode::DirectTouch){
            seat.touch_mode=p.unlimited?2:1;seat.touch_x=p.x;seat.touch_y=p.y;
        }
    }
    frame.held=frame.coop.seats[0].held;
    return frame;
}
}
