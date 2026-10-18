// Actual portable C++ gesture behavior; no JS mapping or game world is mocked here.
// The unchanged legacy fixture includes the production TouchController.hpp.
#include "../../../portable/input/touch-bomb-check.cpp"
#include <cstdio>
#include <cstdlib>

namespace {
const char* group="";
unsigned groups=0,assertions=0;
void begin(const char* name){group=name;++groups;}
void check(bool ok,const char* expression,int line){
    ++assertions;
    if(!ok){std::fprintf(stderr,"touch-controller [%s] line %d: %s\n",group,line,expression);std::exit(1);}
}
#define CHECK(x) check(bool(x),#x,__LINE__)
struct Fixture {
    TouchController touch;
    TouchState state;
    std::uint64_t now=0;
    Fixture(){
        state.context=1;state.instance=1;state.ready=true;
        state.x=192;state.y=384;state.fast=4;state.slow=2;
        touch.sample(state,now++,false,false);
    }
    TouchSample sample(bool keySlow=false,bool arrows=false){return touch.sample(state,now++,keySlow,arrows);}
    void pointer(int type,int id,float x,float y,bool keySlow=false){touch.pointer(type,id,x,y,now++,state,keySlow);}
};
TouchSample actions(Fixture& f,bool bomb,bool escape,bool shot=false,bool focus=false){
    auto s=f.sample();
    CHECK(s.keys[88]==bomb);CHECK(s.keys[27]==escape);
    CHECK(s.keys[90]==shot);CHECK(s.keys[16]==focus);
    return s;
}
void serialPulses(){
    begin("serial repeat, refresh, independent channels, jump and uint32 wrap");
    Fixture f;auto& t=f.touch;
    t.controls(false,false,0,0,0,0);actions(f,false,false);
    t.controls(false,false,11,21,0,0);
    for(int n=0;n<4;++n){t.controls(false,false,11,21,0,0);actions(f,n<3,n<3);}
    t.controls(false,false,12,22,0,0);actions(f,true,true);
    t.controls(false,false,13,22,0,0); // Refresh only bomb after one consumed sample.
    actions(f,true,true);actions(f,true,true);actions(f,true,false);actions(f,false,false);
    t.controls(false,false,13,23,0,0);actions(f,false,true);
    t.controls(false,false,13,24,0,0); // Escape independently refreshes to three.
    actions(f,false,true);actions(f,false,true);actions(f,false,true);actions(f,false,false);
    for(std::uint32_t serial:{17u,0x10000000u,3u,0xffffffffu,0u}){
        t.controls(false,false,serial,24,0,0);
        for(int n=0;n<4;++n){t.controls(false,false,serial,24,0,0);actions(f,n<3,false);}
    }
    // The second channel has the same arbitrary-serial contract, including wrap.
    for(std::uint32_t serial:{100u,4u,0xffffffffu,0u}){
        t.controls(false,false,0,serial,0,0);
        for(int n=0;n<4;++n){t.controls(false,false,0,serial,0,0);actions(f,false,n<3);}
    }
}
void cancelKeepsControls(){
    begin("cancel preserves accepted pulse remainder, fire, focus and stick");
    Fixture f;auto& t=f.touch;t.mode=3;t.controls(true,true,41,91,32767,-32767);
    auto s=actions(f,true,true,true,true);
    CHECK(s.motion==1&&s.stick&&s.x==194&&s.y==382);
    t.cancel();
    for(int n=0;n<3;++n){
        s=actions(f,n<2,n<2,true,true);
        CHECK(!t.active());CHECK(s.motion==1&&s.stick&&s.x==194&&s.y==382);
    }
}
void neutralKeepsPulse(){
    begin("same mapped neutral clears drag, axes and focus without replaying actions");
    for(int mode:{0,3}){
        Fixture f;auto& t=f.touch;t.mode=mode;t.controls(true,true,41,91,32767,-32767);
        if(mode==0){f.pointer(0,1,.5f,.5f);f.pointer(1,1,.625f,.5f);}
        auto s=actions(f,true,true,true,true);
        CHECK(s.motion==1&&s.stick==(mode==3));
        CHECK(mode==3||(s.x==232&&s.y==384&&t.active()));
        // These are the real native methods reached by the existing cancel and
        // controls ABIs; Application supplies zero axes on every controls call.
        t.cancel();t.controls(true,false,41,91,0,0);
        CHECK(!t.active()&&t.stick_x==0&&t.stick_y==0);
        f.pointer(1,1,.75f,.625f); // A stale MOVE cannot revive cancelled drag.
        for(int n=0;n<4;++n){
            t.controls(true,false,41,91,0,0);
            s=actions(f,n<2,n<2,true,false);
            CHECK(!s.motion&&!s.stick&&!t.active());
            CHECK(!s.keys[37]&&!s.keys[38]&&!s.keys[39]&&!s.keys[40]);
        }
    }
}
void resetKeepsSerial(){
    begin("reset clears pulses but retains private serial across repeated neutral");
    Fixture f;auto& t=f.touch;t.mode=3;t.controls(true,true,73,91,32767,-32767);
    actions(f,true,true,true,true);t.reset();
    auto s=actions(f,false,false,true,true); // Reset is not an axis/options reset.
    CHECK(s.motion==1&&s.stick&&s.x==194&&s.y==382);
    for(int n=0;n<3;++n){
        t.reset();t.controls(true,false,73,91,0,0);
        s=actions(f,false,false,true,false);CHECK(!s.motion&&!t.active());
    }
    t.controls(true,false,74,92,0,0);
    for(int n=0;n<4;++n)actions(f,n<3,n<3,true,false);
    t.reset();t.controls(true,false,74,92,0,0);actions(f,false,false,true,false);
}
void pointerPulsesKeepSerial(){
    begin("double tap and four fingers change pulses, not controls serial");
    for(bool bombGesture:{true,false}){
        Fixture f;auto& t=f.touch;t.controls(false,false,19,29,0,0);
        for(int n=0;n<4;++n)actions(f,n<3,n<3);
        if(bombGesture){
            t.double_tap=true;
            f.pointer(0,1,.5f,.5f);f.pointer(2,1,.5f,.5f);f.pointer(0,2,.5f,.5f);
        }else{
            for(int id=1;id<=4;++id)f.pointer(0,id,.5f,.5f);
            CHECK(!t.active()); // Four fingers internally cancel after the pulse.
        }
        for(int n=0;n<4;++n){
            t.controls(false,false,19,29,0,0);
            auto s=actions(f,bombGesture&&n<3,!bombGesture&&n<3);CHECK(!s.motion);
        }
        t.reset();t.controls(false,false,19,29,0,0);actions(f,false,false);
    }
}
void confirmCancelAndReset(){
    begin("menu confirm pulse survives cancel and is cleared by reset");
    Fixture f;auto& t=f.touch;f.state.context=0;f.sample();
    f.pointer(0,1,.5f,.5f);f.pointer(2,1,.5f,.5f);
    actions(f,false,false,true);CHECK(!t.fire);
    t.cancel();t.controls(false,false,0,0,0,0);
    actions(f,false,false,true);actions(f,false,false,true);actions(f,false,false);
    f.pointer(0,2,.5f,.5f);f.pointer(2,2,.5f,.5f);
    t.reset();t.controls(false,false,0,0,0,0);
    for(int n=0;n<4;++n)actions(f,false,false);
}
void joystickAndButtons(){
    begin("free joystick, slow modifiers, digital axes, clamping and fire");
    Fixture f;auto& t=f.touch;t.mode=3;t.controls(true,false,0,0,32767,-16383.5f);
    auto s=actions(f,false,false,true,false);
    CHECK(s.motion==1&&s.stick&&s.x==196&&s.y==382);
    t.controls(true,true,0,0,32767,-16383.5f);
    s=actions(f,false,false,true,true);CHECK(s.motion==1&&s.stick&&s.x==194&&s.y==383);
    t.controls(true,false,0,0,32767,-16383.5f);s=f.sample(true);
    CHECK(s.motion==1&&s.stick&&s.x==194&&s.y==383&&s.keys[90]&&!s.keys[16]);
    t.controls(false,false,0,0,65534,-65534);s=actions(f,false,false);
    CHECK(t.stick_x==1&&t.stick_y==-1&&s.x==196&&s.y==380);
    t.mode=2;t.controls(false,false,0,0,8191.75f,-8191.75f);s=actions(f,false,false);
    CHECK(!s.motion&&!s.keys[37]&&!s.keys[38]&&!s.keys[39]&&!s.keys[40]); // Exactly .25 is neutral.
    t.controls(true,false,0,0,32767,-32767);s=actions(f,false,false,true);
    CHECK(!s.motion&&!s.keys[37]&&s.keys[38]&&s.keys[39]&&!s.keys[40]);
    t.cancel();t.controls(true,false,0,0,0,0);s=actions(f,false,false,true);
    CHECK(!s.motion&&!s.keys[37]&&!s.keys[38]&&!s.keys[39]&&!s.keys[40]);
    // These are TouchController's held Z bits. Application owns auto-fire 3/3.
}
void dragAndNewDown(){
    begin("drag target, turn, slow scaling, unlimited, cancel and fresh DOWN");
    Fixture f;auto& t=f.touch;f.state.y=240;t.sensitivity=1.5f;
    f.pointer(0,1,.5f,.5f);f.pointer(1,1,.625f,.625f);
    auto s=actions(f,false,false);CHECK(s.motion==1&&!s.stick&&s.x==312&&s.y==330);
    f.pointer(1,1,.5f,.5f);s=actions(f,false,false);CHECK(s.x==192&&s.y==240);
    t.controls(true,true,0,0,0,0);f.pointer(1,1,.625f,.625f);
    s=actions(f,false,false,true,true);CHECK(s.motion==1&&s.x==252&&s.y==285);
    t.controls(true,false,0,0,0,0);f.pointer(1,1,.5f,.5f,true);
    s=actions(f,false,false,true,false);CHECK(s.x==192&&s.y==240);
    t.mode=1;t.unlimited=true;t.controls(false,true,0,0,0,0);
    f.pointer(1,1,.625f,.625f,true);
    s=actions(f,false,false,false,true);CHECK(s.motion==2&&!s.stick&&s.x==312&&s.y==330);
    t.cancel();t.controls(false,false,0,0,0,0);
    f.pointer(1,1,.875f,.75f);s=actions(f,false,false);CHECK(!s.motion&&!t.active());
    f.pointer(0,2,.5f,.5f);f.pointer(1,2,.625f,.5f);
    s=actions(f,false,false);CHECK(t.active()&&s.motion==2&&!s.stick&&s.x==312&&s.y==240);
    s=f.sample(false,true);CHECK(!s.motion&&!t.active());
    f.pointer(1,2,.75f,.5f);s=actions(f,false,false);CHECK(!s.motion&&!t.active());
}
}
int main(){
    begin("unchanged legacy deathbomb and context contracts");
    const int legacy=verify();
    if(legacy)std::fprintf(stderr,"legacy touch-bomb verify returned %d\n",legacy);
    CHECK(legacy==0);
    serialPulses();cancelKeepsControls();neutralKeepsPulse();resetKeepsSerial();
    pointerPulsesKeepSerial();confirmCancelAndReset();joystickAndButtons();dragAndNewDown();
    std::printf("{\"passed\":true,\"groups\":%u,\"assertions\":%u,\"legacyVerify\":%d,"
        "\"scope\":\"Production C++ TouchController component; not managed bridge, world, phone or performance acceptance\"}\n",
        groups,assertions,legacy);
    return 0;
}
