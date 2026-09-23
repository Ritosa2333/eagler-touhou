#include "../platform/Application.hpp"
#include <algorithm>

using namespace th10;
extern "C" __attribute__((export_name("multiplayer_audio_status")))
const u32* multiplayer_audio_status(browser::Application* app){
    static u32 words[9]{};std::fill(words,words+9,0);
    if(!app||!app->world)return words;const auto& events=app->world->audio_events;
    words[0]=1;words[1]=events.NextCommit();words[2]=events.EffectsCommitted();
    words[3]=events.MusicCommitted();words[4]=events.StopsCommitted();
    words[5]=events.PendingFrames();words[6]=events.IsOpen();words[7]=events.Failed();
    words[8]=events.Digest();return words;
}
