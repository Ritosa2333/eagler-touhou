#include "EclProgram.hpp"
namespace th10 {
// 0x40c710. Context initialization is a separate operation.
void EclOwner::construct(void* type_table) noexcept {original_virtual_table=type_table;root.stack.top=root.stack.frame_base=0;}
// 0x450470. Names are sorted bytewise, independent of locale.
EclInstruction* EclProgram::find(const char* name) const noexcept {
    i32 low=0,high=subroutine_count-1;
    while(low<=high){
        const i32 middle=low+(high-low)/2;
        const auto& sub=sorted_subroutines[middle];
        const i32 order=std::strcmp(name,sub.name);
        if(!order)return reinterpret_cast<EclInstruction*>(const_cast<u8*>(sub.header)+16);
        if(order<0)high=middle-1;else low=middle+1;
    }
    return nullptr;
}
// 0x44df70. Copy arguments before saving the caller: argument reads may pop
// the source stack, including when source and target are the same context.
i32 call_subroutine(EclContext& target,EclContext& source,u32 skipped,EclServices& services) {
    const i32 old_top=target.stack.top;
    u32 descriptor=source.instruction->argument(0)+4+skipped*4;
    u8* argument_destination=target.stack.data+(old_top?old_top+8:12);
    if(!old_top){const u32 zero=0;target.stack.push(EclValueType::Untyped,&zero,4);}
    for(u32 index=skipped+1;index<source.instruction->parameter_count;++index,descriptor+=8,argument_destination+=4){
        const u8* args=reinterpret_cast<const u8*>(source.instruction)+16;
        const u8 from=args[descriptor],to=args[descriptor+1];
        u32 bits;std::memcpy(&bits,args+((descriptor+4)&~3u),4);
        if(from=='f'||from=='g'){
            float input;std::memcpy(&input,&bits,4);
            const auto value=source.resolve_float(index,input,services);
            if(to=='f'){const float result=value.to_float();std::memcpy(argument_destination,&result,4);}
            else{const i32 result=value.truncate_int();std::memcpy(argument_destination,&result,4);}
        }else{
            const i32 value=source.resolve_integer(index,static_cast<i32>(bits),services);
            if(to=='f'){const float result=Extended::from_int(value).to_float();std::memcpy(argument_destination,&result,4);}
            else std::memcpy(argument_destination,&value,4);
        }
    }
    if(!old_top){
        const u32 zero=0;target.stack.top=4;
        target.stack.push(EclValueType::Untyped,&zero,4);
        target.stack.push(EclValueType::Untyped,&zero,4);
    }else{
        u32 saved_frame=0;target.stack.pop(EclValueType::Untyped,&saved_frame,4);
        target.stack.top=old_top;std::memcpy(target.stack.data+old_top-4,&saved_frame,4);
        target.stack.push(EclValueType::Untyped,&source.time,4);
        target.stack.push(EclValueType::Untyped,&source.instruction,4);
    }
    auto* owner=source.owner;
    auto* previous=owner->active;owner->active=&target;
    target.instruction=owner->program->find(reinterpret_cast<const char*>(source.instruction)+20);
    target.time=0;
    if(!target.instruction){source.instruction=nullptr;return -1;}
    owner->active=previous;
    return 0;
}
// 0x450160.
ListNode<EclContext>* EclOwner::find_thread(i32 id) noexcept {
    for(auto* node=&threads;node;node=node->next)if(node->value->thread_id==id)return node;
    return nullptr;
}
// 0x450190.
void EclOwner::cancel_secondary_threads() noexcept {
    for(auto* node=threads.next;node;node=node->next)node->value->instruction=nullptr;
}
// 0x4500d0. The original initializes only these fields; allocator contents in
// other fields remain untouched until the corresponding script initializes them.
void EclOwner::spawn_thread(i32 id,u32 skipped,EclServices& services) {
    auto* context=static_cast<EclContext*>(services.allocate(sizeof(EclContext)));
    auto* node=static_cast<ListNode<EclContext>*>(services.allocate(sizeof(ListNode<EclContext>)));
    if(!context||!node)__builtin_trap();
    context->stack.top=context->stack.frame_base=0;
    context->thread_id=id;context->owner=this;context->time=0;context->instruction=nullptr;
    reinterpret_cast<u8*>(&context->difficulty)[0]=static_cast<u8>(active->difficulty);
    node->initialize(context);
    node->next=threads.next;if(node->next)node->next->previous=node;
    threads.next=node;node->previous=&threads;
    call_subroutine(*context,*active,skipped,services);
}
// 0x44fd10. Save the next node before executing; newly spawned threads begin
// on the following pass. The root terminating also terminates this owner.
i32 EclOwner::update_threads(float elapsed,EclServices& services) {
    bool first=true;
    for(auto* node=&threads;node;){
        auto* next=node->next;active=node->value;
        if(active->update(elapsed,services)){
            if(first)return -1;
            services.release(active);node->unlink();services.release(node);
        }
        first=false;node=next;
    }
    active=&root;return 0;
}
// 0x40c6e0. The list links are retained until initialization or destruction.
void EclOwner::release_threads(EclServices& services){
    for(auto* node=threads.next;node;){auto* next=node->next;services.release(node->value);services.release(node);node=next;}
}
// 0x40c730. Stack storage is deliberately retained across a phase.
void EclOwner::initialize_context() noexcept {
    root.flags&=~1u;root.time=0;root.instruction=nullptr;root.owner=this;root.thread_id=-1;root.state_1018=0;
    active=&root;threads.initialize(&root);
}
void EclOwner::reset_threads(EclServices& services){release_threads(services);initialize_context();}
// 0x450700.
void EclOwner::select_subroutine(const char* name) noexcept {active->instruction=program->find(name);active->time=0;}
}
