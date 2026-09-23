#include "../platform/Application.hpp"

using namespace th10;
extern "C" __attribute__((export_name("multiplayer_commit_inputs")))
u32 multiplayer_commit_inputs(browser::Application* app,const u32* words,u32 count){
    if(!app||app->stopped||!app->state.multiplayer_session.configured||!words||
       count!=app->state.multiplayer_session.playerCount*5)return 0;
    Netplay::FrameInput inputs[3]{};
    const auto seats=app->state.multiplayer_session.playerCount;
    for(u32 seat=0;seat<seats;++seat){
        const auto* row=words+seat*5;
        if(row[0]>65535||row[1]>4||row[4]>7)return 0;
        auto& input=inputs[seat];input.buttons=u16(row[0]);
        input.analogMode=Netplay::AnalogMode(row[1]);
        std::memcpy(&input.x,row+2,4);std::memcpy(&input.y,row+3,4);
        input.unlimited=(row[4]&1)!=0;input.touchUsed=(row[4]&2)!=0;input.touchBomb=(row[4]&4)!=0;
    }
    return multiplayer::InputLanes::Commit(app->state.input_lanes,inputs,seats)?1:0;
}
extern "C" __attribute__((export_name("multiplayer_configure")))
u32 multiplayer_configure(browser::Application* app,const u32* words,u32 count){
    if(!app||app->stopped||app->world||app->value.screen!=-2)return 0;
    return multiplayer::DecodeSessionSetup(app->state.multiplayer_session,words,count)?1:0;
}
extern "C" __attribute__((export_name("multiplayer_contract")))
u32 multiplayer_contract(browser::Application* app){
    if(!app||!app->state.multiplayer_session.configured)return 0;
    return multiplayer::GameplayContract(app->state.multiplayer_session);
}
extern "C" __attribute__((export_name("multiplayer_status")))
const i32* multiplayer_status(browser::Application* app){
    static i32 words[44];std::memset(words,0,sizeof(words));
    if(!app)return words;
    const auto& setup=app->state.multiplayer_session;
    words[0]=1;words[1]=i32(setup.playerCount);words[2]=i32(setup.localPlayer);
    words[3]=setup.started;words[4]=app->world&&app->world->loading;
    words[5]=app->error;words[6]=app->state.game.stage;
    words[7]=i32(app->state.game.stage_frames);
    if(auto* world=app->world)for(u32 seat=0;seat<world->player_count;++seat){
        const auto& pilot=world->pilots[seat];auto* out=words+8+seat*12;
        out[0]=pilot.game.character;out[1]=pilot.game.shot_type;
        out[2]=pilot.game.lives;out[3]=pilot.game.power;
        if(pilot.player){
            out[4]=pilot.player->state;out[5]=pilot.player->fixed_position.x;
            out[6]=pilot.player->fixed_position.y;out[7]=pilot.player->focused;
        }
        out[8]=pilot.bomb?pilot.bomb->active:0;
        out[9]=i32(world->cooperation.seats[seat].lifeState);
        out[10]=world->cooperation.seats[seat].rescueTicks;
        out[11]=i32(pilot.input_keys);
    }
    return words;
}
