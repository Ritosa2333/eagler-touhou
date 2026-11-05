#include "EclThreads.hpp"
namespace th15 {
EclThreadsSnapshot EclThreads::snapshot()const{EclThreadsSnapshot out;out.main=main;out.children.reserve(children.size());for(const auto& child:children)out.children.push_back(*child);return out;}
bool EclThreads::restore(const EclThreadsSnapshot& source){
    error.clear();if(main.program!=source.main.program){error="ECL checkpoint program changed";return false;}
    auto* variables=main.variables;auto* commands=main.commands;auto* visual=main.visual_rng;const auto* clock=main.interpolation_clock;
    const auto rebind=[&](EclContext& target,const EclContext& input){target=input;target.variables=variables;target.commands=commands;target.visual_rng=visual;target.interpolation_clock=clock;target.threads=this;};
    rebind(main,source.main);children.clear();for(const auto& child:source.children){auto out=std::make_unique<EclContext>();rebind(*out,child);children.push_back(std::move(out));}return true;
}
bool EclThreads::start(const char* name,u8 difficulty){
    if(!main.program){error="ECL program unavailable";return false;}const i32 id=main.program->find_index(name);
    children.clear();main.difficulty=difficulty;main.stack_pointer=main.local_base=0;
    // Original lookup 48f340 returns -1 for the intentional absent routine
    // name "0". The next 48ca80 update terminates the context normally.
    if(id<0){main.routine=-1;main.instruction_offset=0;main.time=0;main.code=nullptr;main.code_size=0;main.instruction=nullptr;return true;}
    return main.bind(*main.program,id);
}
bool EclThreads::restart(const char* name,u8 difficulty){error.clear();main.error.clear();main.thread_id=-1;main.thread_state=0;main.thread_flags&=~1u;for(auto& curve:main.interpolators)curve.duration=0;main.interpolation_targets.fill(nullptr);return start(name,difficulty);}
bool EclThreads::spawn(EclContext& source,i32 id,u32 skipped_arguments){auto context=std::make_unique<EclContext>();context->thread_id=id;context->difficulty=source.difficulty;context->variables=source.variables;context->commands=source.commands;context->visual_rng=source.visual_rng;context->threads=this;context->interpolation_rate=source.interpolation_rate;context->interpolation_clock=source.interpolation_clock;if(!source.call_into(*context,skipped_arguments)){error=source.error;return false;}children.push_front(std::move(context));return true;}
EclContext* EclThreads::find(i32 id){if(main.thread_id==id)return &main;for(auto& child:children)if(child->thread_id==id)return child.get();return nullptr;}
void EclThreads::terminate_children(){for(auto& child:children)child->routine=child->instruction_offset=-1;}
EclContext* EclThreads::at(u32 index)noexcept{if(!index)return &main;for(auto& child:children)if(--index==0)return child.get();return nullptr;}
int EclThreads::step(float elapsed,float interpolation_rate){auto next=children.begin();main.interpolation_rate=interpolation_rate;const int result=main.step(elapsed);if(result){error=main.error;return result;}while(next!=children.end()){auto current=next++;(*current)->interpolation_rate=interpolation_rate;const int child_result=(*current)->step(elapsed);if(child_result==-2){error=(*current)->error;return -2;}if(child_result)children.erase(current);}return 0;}
}
