#include "ReplayArchive.hpp"
#include "../platform/Application.hpp"
#include <algorithm>

using namespace th10;
extern "C" __attribute__((export_name("multiplayer_replay_validate")))
u32 multiplayer_replay_validate(const u8* bytes,u32 size){
    Netplay::InputReplayInfo info;multiplayer::ReplayDescription description;
    return multiplayer::ReplayArchive::Inspect(bytes,size,info,description)?1:0;
}
extern "C" __attribute__((export_name("multiplayer_replay_status")))
const u32* multiplayer_replay_status(browser::Application* app){
    static u32 words[13]{};std::fill(words,words+13,0);if(!app)return words;
    const auto& tape=app->state.multiplayer_replay;
    words[0]=1;words[1]=tape.Recording();words[2]=tape.Playing();words[3]=tape.Frames();
    words[4]=tape.Cursor();words[5]=tape.Generation();words[6]=tape.Complete();
    words[7]=app->multiplayer_replay_seek_target;words[8]=app->multiplayer_replay_scope;
    words[9]=app->startup&&app->startup->scores&&app->startup->scores->replay_read_only();
    words[10]=tape.Base();words[11]=app->multiplayer_replay_seeking();
    words[12]=app->multiplayer_replay_seek_stage;return words;
}
