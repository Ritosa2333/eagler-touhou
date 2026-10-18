#include "EclOwner.hpp"
namespace th11 {
// 0x45b610: argument reads precede the saved caller, including stack pops
// when the call's source and destination are the same context.
i32 call_subroutine(EclContext& target,EclContext& source,u32 skipped,EclServices& services){
    const i32 old_top=target.stack.top;
    u32 descriptor=source.instruction->argument()+4+skipped*4;
    u8* destination=target.stack.data+(old_top?old_top+8:12);
    if(!old_top)target.stack.push(EclValueType::Untyped,0);
    for(u32 index=skipped+1;index<source.instruction->parameter_count;++index,descriptor+=8,destination+=4){
        const auto* args=reinterpret_cast<const u8*>(source.instruction)+16;
        const u8 from=args[descriptor],to=args[descriptor+1];u32 bits;
        std::memcpy(&bits,args+((descriptor+4)&~3u),4);
        const bool reference=source.instruction->references&(1u<<(index&31));
        if(from=='f'||from=='g'){
            const float value=reference?source.resolve_float(float_bits(bits),services):float_bits(bits);
            bits=to=='f'?float_bits(value):u32(truncate_int(value));
        }else{
            const i32 value=reference?source.resolve_integer(signed_bits(bits),services):signed_bits(bits);
            bits=to=='f'?float_bits(float(value)):u32(value);
        }
        std::memcpy(destination,&bits,4);
    }
    if(!old_top){target.stack.top=4;target.stack.push(EclValueType::Untyped,0);target.stack.push(EclValueType::Untyped,0);}
    else{
        u32 saved=0;target.stack.pop_raw(saved);target.stack.top=old_top;
        std::memcpy(target.stack.data+old_top-4,&saved,4);
        target.stack.push(EclValueType::Untyped,float_bits(source.time));
        target.stack.push(EclValueType::Untyped,u32(reinterpret_cast<uintptr_t>(source.instruction)));
    }
    auto* owner=source.owner;auto* previous=owner->active;owner->active=&target;
    target.instruction=owner->program->find(reinterpret_cast<const char*>(source.instruction)+20);target.time=0;
    if(!target.instruction){source.instruction=nullptr;return -1;}
    owner->active=previous;return 0;
}
EclThread* EclOwner::find_thread(i32 id)noexcept{for(auto* n=&threads;n;n=n->next)if(n->value->thread_id==id)return n;return nullptr;}
void EclOwner::cancel_secondary_threads()noexcept{for(auto* n=threads.next;n;n=n->next)n->value->instruction=nullptr;}
void EclOwner::spawn_thread(i32 id,u32 skipped,EclServices& services){
    auto* context=static_cast<EclContext*>(services.allocate(sizeof(EclContext)));
    auto* node=static_cast<EclThread*>(services.allocate(sizeof(EclThread)));
    if(!context||!node)__builtin_trap();
    context->stack.top=context->stack.frame_base=0;context->thread_id=id;context->owner=this;context->time=0;context->instruction=nullptr;
    reinterpret_cast<u8*>(&context->difficulty)[0]=u8(active->difficulty);
    node->value=context;node->next=threads.next;node->previous=&threads;
    if(node->next)node->next->previous=node;threads.next=node;
    call_subroutine(*context,*active,skipped,services);
}
i32 EclOwner::update_threads(float elapsed,EclServices& services){
    bool first=true;
    for(auto* node=&threads;node;){auto* next=node->next;active=node->value;
        const i32 result=active->update(elapsed,services);if(result==-2)return -2;
        if(result){
            if(first)return -1;
            services.release(active);
            if(node->next)node->next->previous=node->previous;
            if(node->previous)node->previous->next=node->next;
            node->next=node->previous=nullptr;services.release(node);
        }
        first=false;node=next;
    }
    active=&root;return 0;
}
void EclOwner::release_threads(EclServices& services){for(auto* node=threads.next;node;){auto* next=node->next;services.release(node->value);services.release(node);node=next;}}
void EclOwner::initialize_context()noexcept{root.flags&=~1u;root.time=0;root.instruction=nullptr;root.owner=this;root.thread_id=-1;root.state=0;active=&root;threads={&root,nullptr,nullptr};}
void EclOwner::reset_threads(EclServices& services){release_threads(services);initialize_context();}
void EclOwner::select_subroutine(const char* name)noexcept{active->instruction=program->find(name);active->time=0;}
}
