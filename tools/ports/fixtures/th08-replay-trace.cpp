#include "GameValues.hpp"
#include <cassert>
#include <cstdio>
#include <cstring>
using th08::u8;
using th08::u32;
static_assert(offsetof(th08::GameGlobals,deaths)==0x64&&offsetof(th08::GameGlobals,deaths_stage)==0x68);
static_assert(offsetof(th08::GameGlobals,lives)==0x74&&offsetof(th08::GameGlobals,bombs)==0x80);
static_assert(offsetof(th08::GameGlobals,bombs_used)==0x84&&offsetof(th08::GameGlobals,bombs_used_stage)==0x88);
static_assert(offsetof(th08::GameGlobals,power)==0x98);

// Only the object graph is a fixture. The export body and GameGlobals layout
// are the production sources selected by the test.
struct Clock { struct {u32 frames=0;} state; };
struct Player { struct {struct {struct {float x=0,y=0;} position;} movement;} motion;struct {int state=0;} life; };
struct BrowserRuntime {
    struct Application {
        struct Game {
            struct {struct {u32 frame=0;} stream;struct {u32 current=0;} input;} playback;
            struct {u32 stage=0,game_flags=0;} globals;
            Clock enemies,control;Player player_state;
        } game;
        struct {struct {u32 seed=0,calls=0;} random;th08::GameGlobals numbers;} session;
    } app;
};
#define EX(name)
#include "production-trace.inc"

int main(){
    BrowserRuntime runtime;auto& app=runtime.app;
    app.game.playback.stream.frame=101;app.game.globals.stage=7;
    app.game.enemies.state.frames=203;app.game.control.state.frames=307;app.game.globals.game_flags=12;
    app.session.random.seed=401;app.session.random.calls=509;
    app.game.player_state.motion.movement.position={-123.25f,456.5f};app.game.player_state.life.state=3;
    app.game.playback.input.current=0x81;
    auto* numbers=reinterpret_cast<u8*>(&app.session.numbers);
    for(unsigned index=0;index<228;++index)numbers[index]=u8(index*17+3);
    const auto* words=browser_trace(&runtime);const auto* coordinates=reinterpret_cast<const float*>(words);
    if(words[2]!=203)return 19;
    assert(words[0]==101&&words[1]==7&&words[2]==203&&words[3]==307&&words[4]==12);
    assert(words[5]==401&&words[6]==509&&coordinates[7]==-123.25f&&coordinates[8]==456.5f);
    assert(words[9]==3&&words[10]==0x81);
    const auto* raw=reinterpret_cast<const u8*>(words);
    for(unsigned index=0;index<228;++index)assert(raw[44+index]==u8(index*17+3));
    for(unsigned index=0;index<272;++index)std::printf("%02x",unsigned(raw[index]));
    std::printf("\n");
}
