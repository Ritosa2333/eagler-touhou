#include "../../cpp/game/KeyboardInput.hpp"
#include "../../cpp/game/GameInput.hpp"
#include <cstdio>
#include <cstdlib>
#include <initializer_list>
#include <utility>

#define CHECK(x) do { if(!(x)){std::fprintf(stderr,"%s:%d: %s\n",__FILE__,__LINE__,#x);std::abort();} } while(0)
using namespace th09;

struct Keyboard {
    bool keys[256]{};
    KeyboardInputBuffer shots;
    GameInput game;
    void event(u32 key,bool down){keys[key]=down;shots.event(key,down);}
    u16 capture(bool active=true,u32 device=2){shots.sample(keys,active);const auto input=shots.input(keys,device);game.advance(input);return input;}
    void clear(){for(auto& key:keys)key=false;shots.clear();}
};

int main(){
    // Main-site failure: a 27.5 ms Z press completed inside a 62.6 ms
    // capture gap. Both DOM events arrived, but the next frame held zero.
    Keyboard short_press;
    CHECK(short_press.capture()==0);
    short_press.event(90,true);short_press.event(90,false);
    CHECK(short_press.capture()==1);CHECK(short_press.game.pressed==1);
    CHECK(short_press.capture()==0);CHECK(short_press.game.released==1);
    for(int i=0;i<20;++i)CHECK(short_press.capture()==0);

    // Ordinary holds and releases have no additional capture delay. Keyboard
    // repeat must not create a release/press or restart charging.
    Keyboard held;
    held.event(90,true);CHECK(held.capture()==1);CHECK(held.game.pressed==1);
    for(int i=0;i<20;++i){held.event(90,true);CHECK(held.capture()==1);CHECK(!held.game.pressed);}
    held.event(90,false);CHECK(held.capture()==0);CHECK(held.game.released==1);

    // A release and repress between captures must reach GameInput as separate
    // edges, rather than merging into a continuous charge.
    held.event(90,true);CHECK(held.capture()==1);
    held.event(90,false);held.event(90,true);
    CHECK(held.capture()==0);CHECK(held.game.released==1);
    CHECK(held.capture()==1);CHECK(held.game.pressed==1);
    CHECK(held.capture()==1);CHECK(!held.game.pressed);
    held.event(90,false);CHECK(held.capture()==0);

    // Bound the tail after a stall. Bursts faster than the logical sampling
    // rate coalesce to one stroke; the physical released state always wins.
    Keyboard burst;
    for(int i=0;i<1001;++i){burst.event(90,true);burst.event(90,false);}
    CHECK(burst.capture()==1);CHECK(burst.capture()==0);CHECK(burst.capture()==0);

    // The right keyboard's down arrow is a fire key. The same complete stroke
    // is also retained for full-keyboard movement, without leaking device bits.
    Keyboard directions;
    directions.event(40,true);directions.event(40,false);
    CHECK(directions.capture(true,4)==1);CHECK(directions.shots.input(directions.keys,2)==32);
    CHECK(directions.capture(true,4)==0);
    directions.event(37,true);directions.event(88,true);directions.event(16,true);
    CHECK(directions.capture()==(64|2|4));
    directions.event(37,false);directions.event(88,false);directions.event(16,false);
    CHECK(directions.capture()==0);
    // All movement layouts, including diagonals, retain a complete stroke
    // exactly once. No gameplay movement is synthesized in menus.
    for(const auto binding:{std::pair<u32,u32>{37,64},{38,16},{39,128},{40,32},{97,96},{98,32},{99,160},{100,64},{102,128},{103,80},{104,16},{105,144}}){
        Keyboard tap;tap.event(binding.first,true);tap.event(binding.first,false);
        CHECK(tap.capture()==binding.second);CHECK(tap.capture()==0);CHECK(tap.capture()==0);
    }
    for(const auto binding:{std::pair<u32,u32>{66,32},{70,64},{72,128},{78,160},{82,80},{84,16},{86,96},{89,144}}){
        Keyboard tap;tap.event(binding.first,true);tap.event(binding.first,false);
        CHECK((tap.capture(true,3)&0xf0)==binding.second);CHECK(tap.capture(true,3)==0);
    }
    // Ordinary release is not queued, including OS key repeat while held.
    Keyboard move;move.event(39,true);CHECK(move.capture()==128);
    for(int i=0;i<20;++i){move.event(39,true);CHECK(move.capture()==128);}
    move.event(39,true);move.event(39,false);CHECK(move.capture()==0);CHECK(move.capture()==0);
    // Sub-frame bursts coalesce; opposing keys keep the game's original
    // simultaneous-input semantics rather than replaying an old route later.
    for(int i=0;i<1001;++i){move.event(37,true);move.event(37,false);}
    move.event(39,true);move.event(39,false);CHECK(move.capture()==(64|128));CHECK(move.capture()==0);
    move.event(37,true);move.event(37,false);move.clear();CHECK(move.capture()==0);
    for(u32 device=0;device<5;++device){
        directions.event(90,true);directions.event(90,false);directions.event(40,true);directions.event(40,false);
        CHECK(directions.capture(false,device)==keyboard_input(directions.keys,device));
        CHECK(directions.capture(true,device)==0);
    }

    // Focus loss, pause and a new game discard pending actions. Held state is
    // still accepted immediately when transitioning from menus into gameplay.
    Keyboard lifecycle;
    lifecycle.event(90,true);lifecycle.event(90,false);lifecycle.clear();CHECK(lifecycle.capture()==0);
    lifecycle.event(90,true);CHECK(lifecycle.capture(false)==1);CHECK(lifecycle.capture()==1);
    lifecycle.clear();CHECK(lifecycle.capture()==0);
    lifecycle.event(90,true);lifecycle.event(90,false);CHECK(lifecycle.capture(false)==0);CHECK(lifecycle.capture()==0);

    // The older SDL polling path has no DOM edge feed and keeps its behavior.
    Keyboard polled;polled.keys[90]=true;CHECK(polled.capture()==1);polled.keys[90]=false;CHECK(polled.capture()==0);
    std::puts("Keyboard input capture: shot/repress, direction taps, immediate hold/release, bounded tail, device mapping and clear PASS");
}
