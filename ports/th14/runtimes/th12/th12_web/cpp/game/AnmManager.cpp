#include "AnmManager.hpp"
#include <cstdlib>
namespace th11 {
namespace {
// A draw failure names the layer but nothing about who put an animation there, and children inherit
// their parent's layer (see spawn() below), so the creator cannot be recovered from the live VM.
// Recording every create keeps that provenance.
struct CreationRecord{i32 script;u32 layer;u32 file_id;const void* resource;u32 depth;u32 id;};
constexpr u32 kCreationCapacity=4096;
CreationRecord g_creations[kCreationCapacity]{};
u32 g_creation_count=0;
}
u32 anm_creation_count()noexcept{
#if defined(TH14_NATIVE_CRITICAL_SECTIONS)
    th14_critical_sections::ContainerGuard guard(th14_critical_sections::containers);
#endif
return g_creation_count;}
i32 anm_creation_value(u32 index,u32 kind){
#if defined(TH14_NATIVE_CRITICAL_SECTIONS)
    th14_critical_sections::ContainerGuard guard(th14_critical_sections::containers);
#endif

    if(index>=g_creation_count||g_creation_count-index>kCreationCapacity)return -1;
    const auto& r=g_creations[index%kCreationCapacity];
    switch(kind){
    case 0:return r.script;
    case 1:return i32(r.layer);
    case 2:return i32(r.file_id);
    case 3:return r.resource?1:0;
    case 4:return i32(r.depth);
    case 5:return i32(r.id);
    default:return -1;
    }
}
AnmManager::~AnmManager(){
#if defined(TH14_NATIVE_CRITICAL_SECTIONS)
    th14_critical_sections::ContainerGuard guard(th14_critical_sections::containers);
#endif
for(auto& [vm,size]:geometry_sizes){std::free(vm->geometry);vm->geometry=nullptr;}}
#if PORT_TITLE == 14
bool AnmManager::prepare_resource(AnmResource& file,u16 file_id){
    if(file.templates)return true;
    auto prepared=std::make_shared<AnmTemplatesTh14>();prepared->scripts.resize(file.scripts.size());
    for(u32 i=0;i<file.scripts.size();++i){
        auto& vm=prepared->scripts[i];vm.bind_script(file,i,file_id,&rate);vm.flags2|=0x4000;
        vm.timer.set(-1,&rate);vm.age.set(-1,&rate);
        const int status=vm.update(*this);
        if(status<0){record_failure(vm);release_geometry(vm);return false;}
        // Native geometry/callback storage belongs to the uncopied suffix.
        release_geometry(vm);
    }
    file.templates=std::move(prepared);return true;
}
#endif
// The newest record carrying this id is the one the live animation came from; the ring is walked
// backwards because an id is never reused while its animation lives.
bool anm_creation_of(u32 id,i32& script,u32& layer,u32& file){
#if defined(TH14_NATIVE_CRITICAL_SECTIONS)
    th14_critical_sections::ContainerGuard guard(th14_critical_sections::containers);
#endif

    const u32 count=anm_creation_count()<kCreationCapacity?anm_creation_count():kCreationCapacity;
    for(u32 back=0;back<count;++back){
        const u32 index=anm_creation_count()-1-back;
        if(g_creations[index%kCreationCapacity].id!=id)continue;
        const auto& r=g_creations[index%kCreationCapacity];
        script=r.script;layer=r.layer;file=r.file_id;return true;
    }
    return false;
}
AnmVm* AnmManager::find(u32 id)noexcept{
#if defined(TH14_NATIVE_CRITICAL_SECTIONS)
    th14_critical_sections::ContainerGuard guard(th14_critical_sections::containers);
#endif
auto it=active.find(id);return it==active.end()?nullptr:it->second;}
// What the interpreter left behind at a failure: the command, how far into the script it was, the
// commands that led there, and which animation it was. Both the creation walk and the per-frame
// overlay walk reach it, and a report from either names the same fields.
void AnmManager::record_failure(const AnmVm& vm){
    last_error=u32(u16(vm.instruction?vm.instruction->opcode:-1));
    last_error_offset=u32(reinterpret_cast<const u8*>(vm.instruction)-reinterpret_cast<const u8*>(vm.script_begin));
    last_error_length=vm.instruction?u32(vm.instruction->length):0;
    last_error_time=vm.instruction?i32(vm.instruction->time):0;
    last_error_references=vm.instruction?u32(vm.instruction->references):0;
    last_error_vm=vm.id;last_error_layer=vm.layer;
    last_error_file=u32(vm.file_index);last_error_script=u32(i32(vm.script_index));
    // The loader's hash of this script, against the bytes the vm's own pointer holds: a difference
    // says a loaded script was written after the load, or that the pointer holds another buffer.
    last_script_address=u32(uintptr_t(vm.script_begin));
    if(vm.resource&&vm.script_index>=0&&u32(vm.script_index)<vm.resource->scripts.size()){
        const AnmScript& script=vm.resource->scripts[u32(vm.script_index)];
        last_script_size=u32(script.bytes.size());
        last_script_hash_load=script.load_hash;
        last_resource_address=u32(uintptr_t(script.bytes.data()));
        last_script_hash_now=anm_bytes_hash(reinterpret_cast<const u8*>(vm.script_begin),
                                          last_script_size);
    }
    // The window starts at the failing command when it is too early in the script for a word
    // before, and never leaves the script's own bytes.
    {
        u32 start=last_error_offset>=4?last_error_offset-4:0;
        last_bytes_offset=start;last_bytes_count=0;
        const auto* base=reinterpret_cast<const u8*>(vm.script_begin);
        while(last_bytes_count<sizeof last_bytes&&start+last_bytes_count<last_error_offset+20)
            last_bytes[last_bytes_count++]=base[start+last_bytes_count];
    }
    // The ring belongs to the VM that filled it: sixteen animations run each frame, and a global
    // ring held whichever of them dispatched last rather than this one's history (section 381).
    last_path_count=vm.trace_count<anm_trace_size?vm.trace_count:anm_trace_size;
    for(u32 i=0;i<last_path_count;++i)last_path[i]=vm.trace[(vm.trace_count-last_path_count+i)&(anm_trace_size-1)];
}
std::string AnmManager::last_error_text()const{
    // Both positions print in the same shape and the same base: the head names the command that
    // failed, the ring names how the walk got there, and its last entry is that same position.
    // TH13's dialogue emitter wrote hex and this one wrote the ring in decimal, so a trace read
    // across rounds compared 292 with 0x0124 without a way to see they were one place.
    const auto offset_text=[](u32 offset){
        std::string out;
        for(i32 shift=12;shift>=0;shift-=4)out+="0123456789abcdef"[(offset>>shift)&15];
        return out;
    };
    std::string text="ANM opcode "+std::to_string(last_error)+" at +0x"+offset_text(last_error_offset);
    // The instruction as memory holds it, not only its number: the file's own four fields are the
    // other half of the comparison, and a zero length is what makes an update spin.
    text+=" (len "+std::to_string(last_error_length)+" time "+std::to_string(last_error_time)+" refs 0x"+offset_text(last_error_references&0xffffu)+")";
    text+=" (vm "+std::to_string(last_error_vm)+", layer "+std::to_string(last_error_layer)
          +", file "+std::to_string(last_error_file)+", script "+std::to_string(last_error_script)+")";
    // The same bytes the interpreter read, so the title's file can be compared with them directly.
    text+=" bytes +0x"+offset_text(last_bytes_offset)+":";
    for(u32 i=0;i<last_bytes_count;++i){text+=" ";const u8 byte=last_bytes[i];text+="0123456789abcdef"[byte>>4];text+="0123456789abcdef"[byte&15];}
    // The load's own hash of the bound script beside the hash of the bytes its pointer holds now.
    text+=" script 0x"+offset_text(last_script_address)+" of "+std::to_string(last_script_size)+" bytes"
          +", the resource's 0x"+offset_text(last_resource_address)
          +"; hash 0x"+offset_text(last_script_hash_load)+" at load, 0x"+offset_text(last_script_hash_now)
          +" now";
    if(last_script_hash_load!=last_script_hash_now)text+=" [the loaded script changed]";
    if(last_script_address!=last_resource_address)text+=" [the vm points away from the resource]";
    for(u32 i=0;i<last_path_count;++i){text+=" 0x"+offset_text(last_path[i].offset);text+=":";text+=std::to_string(last_path[i].opcode);
        // How far the walk could step at that position: a zero here is where a path stopped moving.
        text+="(len ";text+=std::to_string(last_path[i].length);text+=")";}
    return text;
}
AnmVm* AnmManager::spawn_effect(AnmVm& source,i32 script){return source.resource?create(*source.resource,script,source.file_index,0):nullptr;}
AnmVm* AnmManager::create(AnmResource& file,i32 script,u16 file_id,u32 layer,bool overlay,bool prepend,const Vec3* initial_position,const AnmVm* detached_source,AnmVm* attached_parent,bool th14_playfield_layer,const float* initial_rotation,u32 th16_initial_flags2){
#if defined(TH14_NATIVE_CRITICAL_SECTIONS)
    th14_critical_sections::AnmGuard guard(th14_critical_sections::anm);
#endif

    // The failure fields describe *this* creation: a range or depth failure returns before the update,
    // and without this the report would carry the opcode and position of whatever failed before it.
    last_error=0;last_error_offset=0;last_path_count=0;last_error_vm=0;last_error_layer=0;
    last_error_length=0;last_error_time=0;last_error_references=0;
    last_error_file=0;last_error_script=0;last_bytes_offset=0;last_bytes_count=0;
    last_script_hash_load=0;last_script_hash_now=0;last_script_address=0;last_resource_address=0;
    last_script_size=0;
    if(script<0||u32(script)>=file.scripts.size()||spawn_depth>=128)return nullptr;
    // The record of this call, kept by index: the update below runs this animation's own script, and
    // a command in it may create a child, which writes a record of its own and moves the counter.
    // Stamping the newest record at the end would then put this creation's id on the child.
    u32 record_index;
    AnmVm* vm;
    {
#if defined(TH14_NATIVE_CRITICAL_SECTIONS)
    th14_critical_sections::ContainerGuard guard(th14_critical_sections::containers);
#endif
    record_index=g_creation_count%kCreationCapacity;
    {auto& r=g_creations[record_index];r={script,layer,file_id,&file,spawn_depth,0u};++g_creation_count;}
    if(available.empty()){storage.push_back(std::make_unique<AnmVm>());vm=storage.back().get();if constexpr(kPortTitle==16)vm->layer=14;vm->initialize();}else{vm=available.back();available.pop_back();}
#if PORT_TITLE == 14
    if(th14_native_slots.find(vm)==th14_native_slots.end())th14_native_slots.emplace(vm,th14_unused_slots?--th14_unused_slots:8191);
#endif
    }
    // Retail TH16 4063dc skips the layer override for negative arguments.
    // The resource's default layer is retained; -1 is not a draw-list index.
    if constexpr(kPortTitle==16){if(signed_bits(layer)>=0)vm->layer=layer;}
    else if constexpr(kPortTitle!=14)vm->layer=layer;
#if PORT_TITLE == 14
    vm->position=initial_position?*initial_position:Vec3{};vm->copy_script(file,script,file_id,&rate);
    if(!attached_parent&&signed_bits(layer)>=0)vm->layer=layer;
#else
    if(initial_position){vm->position=*initial_position;vm->bind_script(file,script,file_id,&rate);}
    else{vm->position=vm->script_position=vm->child_position={};vm->flags|=0x40000000;
    vm->rectangle_columns=vm->rectangle_rows=16;vm->script_index=i16(script);vm->file_index=file_id;vm->resource=&file;
    vm->script_begin=vm->instruction=reinterpret_cast<AnmInstruction*>(file.scripts[script].bytes.data());vm->flags&=~0x601u;vm->timer.set(0,&rate);}
#endif
    if constexpr(kPortTitle==13||kPortTitle==16)if(detached_source){vm->position=detached_source->position;vm->rotation=detached_source->rotation;vm->child_position=detached_source->script_position;}
    if constexpr(kPortTitle==14){
        //47ee90 supplies its layer/0x200 before40af00, whose template
        // prefix replaces them. The detached and ordinary factories set
        // their layer and0x200 after the copy.
        if(!attached_parent)vm->flags2|=0x200;
        if(initial_rotation)vm->rotation.z=*initial_rotation;
        // Native creation helpers establish the origin before the first ANM
        // update; that update may explicitly override it.
        if(th14_playfield_layer)vm->flags2=(vm->flags2&~0x40000u)|0x20000;
        if(const auto* source=attached_parent?attached_parent:detached_source){
            vm->flags2=(vm->flags2&~0x1000000u)|(source->flags2&0x1000000u);
            // 47ef18 binds the parent before the child's first script update.
            vm->parent=attached_parent;
            if(!attached_parent){vm->position=source->position;vm->rotation=source->rotation;vm->child_position=source->script_position;}
        }
    }
#if PORT_TITLE == 16
    if(const auto* source=attached_parent?attached_parent:detached_source){
        vm->parent=attached_parent;vm->th16_transform_parent=attached_parent;
        vm->flags2=(vm->flags2&~0x2000000u)|(source->flags2&0x2000000u);
        if(detached_source)vm->flags2|=0x400u;
    }
#endif
    if constexpr(kPortTitle==16){
        vm->flags2|=th16_initial_flags2;
        if(th16_initial_flags2){if(initial_rotation)vm->rotation.z=*initial_rotation;if(signed_bits(layer)>=0&&layer<=23)vm->flags2=(vm->flags2&~0x80000u)|0x40000;}
    }
#if PORT_TITLE == 14
    if(before_initial_update&&!before_initial_update(*vm)){destroy(*vm);return nullptr;}
#endif
    ++spawn_depth;const int status=vm->update(*this);--spawn_depth;
    if(status<0){record_failure(*vm);destroy(*vm);return nullptr;}
    {
#if defined(TH14_NATIVE_CRITICAL_SECTIONS)
    th14_critical_sections::ContainerGuard guard(th14_critical_sections::containers);
#endif
    auto& node=vm->registry;node.value=vm;node.next=node.previous=nullptr;const u32 list=overlay?1:0;
    if(prepend){node.next=heads[list];if(heads[list])heads[list]->previous=&node;else tails[list]=&node;heads[list]=&node;}
    else{node.previous=tails[list];if(tails[list])tails[list]->next=&node;else heads[list]=&node;tails[list]=&node;}
#if PORT_TITLE == 14
    // 47e890 publishes after the initial script update (and its children).
    // Its19-bit serial skips zero; the low13 bits retain the pool slot.
    next_id=(next_id+1)&0x7ffffu;if(!next_id)++next_id;
    vm->id=(next_id<<13)|(th14_native_slots.at(vm)&8191u);
#else
    if(++next_id==0)++next_id;vm->id=next_id;
#endif
    active.emplace(vm->id,vm);
    g_creations[record_index].id=vm->id;
    }
    return vm;
}
AnmVm* AnmManager::spawn(AnmVm& parent,i32 script,u32 op){if(!parent.resource)return nullptr;const bool detached=op==95||op==97;return create(*parent.resource,script,parent.file_index,parent.layer,op==90||op==92,op==91||op==92,nullptr,(kPortTitle==13||kPortTitle==14||kPortTitle==16)&&detached?&parent:nullptr,(kPortTitle==14||kPortTitle==16)&&!detached?&parent:nullptr);}
void AnmManager::destroy(AnmVm& vm){
#if defined(TH14_NATIVE_CRITICAL_SECTIONS)
    th14_critical_sections::ContainerGuard guard(th14_critical_sections::containers);
#endif

#if PORT_TITLE == 14
    if(vm.th13_effect==7&&vm.geometry)anm_th14_particles_destroy(vm,*this);
#endif
    // TH14 47ebfa walks the root's entire flattened child chain, including
    // grandchildren, and marks flags2 bit20 before unlinking the root.
    // Unlinking the root below detaches that chain before its VM is recycled.
    if constexpr(kPortTitle==13||kPortTitle==14)if(!vm.child.previous)for(auto* node=vm.child.next;node;node=node->next){node->value->parent=nullptr;if constexpr(kPortTitle==14)node->value->flags2|=0x20;else node->value->flags|=0x4000000;}
    // A destroyed VM may still be referenced by the previous draw traversal.
    // Remove it there before initialize() resets its id and next pointer.
    for(auto& head:layers){auto** link=&head;while(*link){if(*link==&vm){*link=vm.draw_next;break;}link=&(*link)->draw_next;}}
    for(u32 i=0;i<2;++i){if(heads[i]==&vm.registry)heads[i]=vm.registry.next;if(tails[i]==&vm.registry)tails[i]=vm.registry.previous;}
    auto unlink=[](AnmVm::Node& node){if(node.next)node.next->previous=node.previous;if(node.previous)node.previous->next=node.next;node.next=node.previous=nullptr;};
    unlink(vm.registry);unlink(vm.child);active.erase(vm.id);if(auto it=geometry_sizes.find(&vm);it!=geometry_sizes.end()){std::free(vm.geometry);vm.geometry=nullptr;geometry_sizes.erase(it);}
#if PORT_TITLE == 14
    const auto slot=th14_native_slots.find(&vm);
    if(slot!=th14_native_slots.end()&&slot->second==8191){
        // 47ecd4 frees the heap VM instead of inserting it in the pool.
        th14_native_slots.erase(slot);
        for(auto i=storage.begin();i!=storage.end();++i)if(i->get()==&vm){storage.erase(i);break;}
        return;
    }
#endif
    vm.initialize();available.push_back(&vm);
}
void AnmManager::clear(u32 keep_handle,u32 keep_family){
#if defined(TH14_NATIVE_CRITICAL_SECTIONS)
    th14_critical_sections::ContainerGuard guard(th14_critical_sections::containers);
#endif

    // Walk ownership, not draw layers: newly created VMs are not on a draw
    // layer until update() has run, and deleted layer heads are not owners.
    std::vector<u32> retained;
    if(keep_family){
        retained.push_back(keep_family);
        if(auto* root=find(keep_family))
            for(auto* node=root->child.next;node;node=node->next)retained.push_back(node->value->id);
    }
    const auto keep=[&](u32 id){
        if(id==keep_handle)return true;
        for(u32 retained_id:retained)if(id==retained_id)return true;
        return false;
    };
    layers.fill(nullptr);
#if PORT_TITLE == 14
    // Preserve registry order when returning slots; hash iteration would
    // make the next allocation history depend on the host container.
    for(u32 list=0;list<2;++list){auto* node=heads[list];while(node){auto* next=node->next;if(!keep(node->value->id))destroy(*node->value);node=next;}}
#endif
    for(auto i=active.begin();i!=active.end();){auto* vm=i->second;++i;if(!keep(vm->id))destroy(*vm);}
    last_error=0;
}
void AnmManager::retire_resource(const AnmResource& resource){
#if defined(TH14_NATIVE_CRITICAL_SECTIONS)
    th14_critical_sections::ContainerGuard guard(th14_critical_sections::containers);
#endif

    // Deferred deletion matches 4564c0: an overlay callback and its children
    // may still occur later in the current tick. Keep the resource alive until
    // both ANM registries have consumed the deletion markers.
    for(auto& [id,vm]:active)if(vm->resource==&resource){
        if constexpr(kPortTitle==14)vm->flags2|=0x20;
        else vm->flags|=0x4000000;
    }
}
bool AnmManager::references(const AnmResource& resource)const noexcept{
#if defined(TH14_NATIVE_CRITICAL_SECTIONS)
    th14_critical_sections::ContainerGuard guard(th14_critical_sections::containers);
#endif

    for(const auto& entry:active)if(entry.second->resource==&resource)return true;
    return false;
}
bool AnmManager::update(bool overlay){
#if PORT_TITLE == 14
    return update(overlay,VmBoundaryCallbackTh14{});
}
bool AnmManager::update(bool overlay,const std::function<bool()>& after_vm){
    VmBoundaryCallbackTh14 boundary;
    if(after_vm)boundary=[&](const AnmVm&,VmBoundaryTh14 point){return point==VmBoundaryTh14::BeforeCall||after_vm();};
    return update(overlay,boundary);
}
bool AnmManager::update(bool overlay,const VmBoundaryCallbackTh14& boundary){
#if defined(TH14_CPP_STAGE_DECODE)
    if(!overlay&&th14_control.normal_anm_blocked())return true;
#endif
#if defined(TH14_NATIVE_CRITICAL_SECTIONS)
    th14_critical_sections::AnmGuard guard(th14_critical_sections::anm);
#endif

#endif
    constexpr u32 overlay_first=kPortTitle==16?36:kPortTitle==14?34:kPortTitle==13?31:29;
    const u32 first=overlay?overlay_first:0,last=overlay?overlay_first+(kPortTitle==16?7:2):overlay_first;decltype(layers) end{};for(u32 i=first;i<last;++i)layers[i]=nullptr;
    auto* cursor=heads[overlay?1:0];while(cursor){auto& vm=*cursor->value;cursor=cursor->next;
        if((vm.flags&0x4000000)||(kPortTitle==14&&(vm.flags2&0x20))){destroy(vm);continue;}
        if constexpr(kPortTitle==16){const u32 state=vm.flags2&0x60;if(state==0x20){destroy(vm);continue;}if(state)continue;}
#if PORT_TITLE == 14
        // Original47e65b/47e73b precede the callback inside46fe50 at46ff1f.
        if(boundary&&!boundary(vm,VmBoundaryTh14::BeforeCall))return false;
#endif
        if(vm.before_update)vm.before_update(vm);
        const int status=vm.update(*this);
        if(status<0){record_failure(vm);return false;}
#if PORT_TITLE == 14
        // Retail47e660 is a real worker interleave after the VM call, before
        // retirement and draw-list publication. The callback owns its clock
        // and readiness checks; no VM state or fixed delay is supplied here.
        if(boundary&&!boundary(vm,VmBoundaryTh14::AfterReturn))return false;
#endif
        if(status){destroy(vm);continue;}
        if constexpr(kPortTitle==16){
            if(overlay){if(vm.layer>=24&&vm.layer<=31)vm.layer+=12;else if(vm.layer<36||vm.layer>42)vm.layer=38;}
            else if(vm.layer>=36&&vm.layer<=42)vm.layer-=12;
            // Native overlay layer 31 becomes 43, outside its seven slots.
            // Reject that out-of-bounds request rather than corrupting memory.
            if(vm.layer>=last){last_error=-1;last_error_layer=vm.layer;return false;}
        }else if(overlay&&vm.layer!=overlay_first&&vm.layer!=overlay_first+1)vm.layer=overlay_first;
        if(vm.layer>=first&&vm.layer<last){if(end[vm.layer])end[vm.layer]->draw_next=&vm;else layers[vm.layer]=&vm;end[vm.layer]=&vm;vm.draw_next=nullptr;}
    }
#if PORT_TITLE == 14
    if(overlay)th14_native_stack.controller_overlay_tail();
#endif
    return true;
}
bool AnmManager::allocate_geometry(AnmVm& vm,u32 bytes){
#if defined(TH14_NATIVE_CRITICAL_SECTIONS)
    th14_critical_sections::ContainerGuard guard(th14_critical_sections::containers);
#endif

    if(!bytes||bytes>16*1024*1024)return false;void* p=std::calloc(1,bytes);if(!p)return false;
    if(geometry_sizes.find(&vm)!=geometry_sizes.end())std::free(vm.geometry);vm.geometry=p;geometry_sizes[&vm]=bytes;return true;
}
void AnmManager::release_geometry(AnmVm& vm){
#if defined(TH14_NATIVE_CRITICAL_SECTIONS)
    th14_critical_sections::ContainerGuard guard(th14_critical_sections::containers);
#endif
auto it=geometry_sizes.find(&vm);if(it!=geometry_sizes.end()){std::free(vm.geometry);vm.geometry=nullptr;geometry_sizes.erase(it);}}
bool AnmManager::update_geometry(AnmVm& vm){
#if defined(TH14_NATIVE_CRITICAL_SECTIONS)
    const u32 bytes=geometry_bytes(vm);if(!bytes)return false;
#if PORT_TITLE == 14
    if(vm.drawing_mode()==23||vm.drawing_mode()==24)return anm_world_ring_vertices(vm,static_cast<AnmWorldVertex*>(vm.geometry),bytes/sizeof(AnmWorldVertex));
#endif
    return anm_ring_vertices(vm,static_cast<AnmVertex*>(vm.geometry),bytes/sizeof(AnmVertex),render_space);
#else
    auto it=geometry_sizes.find(&vm);if(it==geometry_sizes.end())return false;
#if PORT_TITLE == 14
    if(vm.drawing_mode()==23||vm.drawing_mode()==24)return anm_world_ring_vertices(vm,static_cast<AnmWorldVertex*>(vm.geometry),it->second/sizeof(AnmWorldVertex));
#endif
    return anm_ring_vertices(vm,static_cast<AnmVertex*>(vm.geometry),it->second/sizeof(AnmVertex),render_space);
#endif
}
bool AnmManager::change_draw_mode(AnmVm& vm){if(!allocate_geometry(vm,sizeof(AnmRipple)))return false;anm_ripple_initialize(vm,*static_cast<AnmRipple*>(vm.geometry),script_rng);return true;}
}

