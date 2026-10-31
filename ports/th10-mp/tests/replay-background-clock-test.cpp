#include "../web/cpp/multiplayer/ReplayBackground.hpp"
#include <cassert>
#include <cstdio>
#include <vector>
using namespace th10;
u32 bits(float f){u32 n;std::memcpy(&n,&f,4);return n;}
void op(std::vector<u32>& bytes,i32 time,std::int16_t code,std::initializer_list<u32> args){
    bytes.push_back(u32(time));bytes.push_back(u32(u16(code))|(u32(8+args.size()*4)<<16));
    bytes.insert(bytes.end(),args);
}
struct Environment final:StageEnvironment{
    float speed=1;u32 color=0;Camera camera{};bool chapter=false;
    explicit Environment(bool fresh){chapter=fresh;rate=&speed;background_color=&color;world=&camera;}
    i32 update_animation(AnmVm&)override{return 0;}
    void initialize_animation(AnmFile&,AnmVm&,i32)override{}
    void normalize(Vec3&,const Vec3&)override{}
    void camera_position_interpolated(Stage& s)override{multiplayer::AlignStage4ReplayCamera(s,chapter);}
};
void setup(Stage& s,std::vector<u32>& stream,Environment& e,i32 number=4){
    s.initialize();s.stage_number=number;s.instruction=s.script_begin=reinterpret_cast<StageInstruction*>(stream.data());
    s.script_timer.rate=e.rate;s.script_timer.initialize(-1);
}
int main(){
    std::vector<u32> script;
    op(script,0,3,{90,0,bits(90),bits(300),bits(-300)});
    op(script,25,13,{0xffabcdef});op(script,1000000,0,{});
    Stage full{},direct{},broken{};Environment live(false),replay(true),old(false);
    setup(full,script,live);setup(direct,script,replay);setup(broken,script,old);
    for(unsigned n=0;n<29;++n)assert(full.update_script(live)==0);
    assert(broken.update_script(old)==0);assert(full.script_timer.current!=broken.script_timer.current);
    const Stage initial=direct;assert(direct.update_script(replay)==0);const Stage once=direct;
    direct=initial;assert(direct.update_script(replay)==0);
    assert(direct.script_timer.current==once.script_timer.current&&!std::memcmp(&direct.camera.position,&once.camera.position,sizeof(Vec3)));
    for(unsigned n=0;n<120;++n){
        assert(full.update_script(live)==0);if(n)assert(direct.update_script(replay)==0);
        assert(full.script_timer.current==direct.script_timer.current);
        assert(full.position_interpolation.timer.current==direct.position_interpolation.timer.current);
        assert(!std::memcmp(&full.camera.position,&direct.camera.position,sizeof(Vec3)));
        assert(full.instruction==direct.instruction&&live.color==replay.color);
    }
    for(i32 stage:{1,2,3,5,6,7}){Stage other{};Environment e(true);setup(other,script,e,stage);assert(other.update_script(e)==0);assert(other.script_timer.current==1);}
    Stage normal{},late{};Environment untouched(false),late_env(true);setup(normal,script,untouched);setup(late,script,late_env);
    late.script_timer.current=1;late.script_timer.fractional=1;assert(late.update_script(late_env)==0&&late.script_timer.current==2);
    assert(normal.update_script(untouched)==0&&normal.script_timer.current==1);
    std::puts("PASS TH10 chapter background: native 29-frame transition vs direct chapter for 120 ticks; rewind; stage and normal-play guards");
}
