#include "BackgroundFlow.hpp"
namespace th08 {
namespace {
constexpr const char* animations[]={"stg1bg.anm","stg2bg.anm","stg3bg.anm","stg4abg.anm","stg4abg.anm","stg5bg.anm","stg6bg.anm","stg7bg.anm","stg8bg.anm"};
constexpr const char* stages[]={"stage1.std","stage2.std","stage3.std","stage4a.std","stage4b.std","stage5.std","stage6.std","stage7.std","stage8.std"};
constexpr const char* practice[]={"stage1_s.std","stage2_s.std","stage3_s.std","stage4a_s.std","stage4b_s.std","stage5_s.std","stage6_s.std","stage7_s.std","stage8_s.std"};
}
bool BackgroundFlow::setup(){
    auto& s=script.state;const i32 stage=script.context.stage;
    s.time.set(0);s.instruction_index=0;s.position={};s.spell_state=0;s.fog_duration=0;
    if(stage<0||stage>=9||!context.text)return false;
    s.animation=context.keep_resources?resources.get(4):resources.load(4,animations[stage]);if(!s.animation)return false;
    std::vector<u8> bytes;
    if(context.keep_resources){const auto* header=reinterpret_cast<const u8*>(script.program.header());if(!header)return false;bytes.assign(header,header+script.program.bytes_size());}
    else bytes=resources.stage((script.context.practice?practice:stages)[stage]);
    if(!script.load(bytes.data(),bytes.size(),*s.animation,*context.text))return false;
    script.reset_camera();return true;
}
void BackgroundFlow::release(){if(!context.keep_resources)resources.release(4);script.release(context.keep_resources);}
bool BackgroundFlow::attach(Chain& owner,i32 stage){
    detach();chain=&owner;auto* data=context.keep_resources?script.program.header():nullptr;std::memset(&script.state,0,sizeof(script.state));script.state.stage_data=data;script.state.stage=stage;
    calculation.set_callback([](void* p){return static_cast<BackgroundFlow*>(p)->script.update();});calculation.argument=this;
    calculation.added=[](void* p){return static_cast<BackgroundFlow*>(p)->setup()?0:-1;};calculation.deleted=[](void* p){static_cast<BackgroundFlow*>(p)->release();return 0;};
    if(owner.add(&calculation,8))return false;
    high.set_callback([](void* p){return static_cast<BackgroundFlow*>(p)->view.high();});high.argument=this;owner.add(&high,6,true);
    low.set_callback([](void* p){return static_cast<BackgroundFlow*>(p)->view.low();});low.argument=this;owner.add(&low,7,true);return true;
}
void BackgroundFlow::detach(){if(chain){chain->cut(&calculation);chain->cut(&high);chain->cut(&low);chain=nullptr;}}
}
