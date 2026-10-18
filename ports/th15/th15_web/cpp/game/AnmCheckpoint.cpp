#include "AnmCheckpoint.hpp"
#include "AnmFile.hpp"
namespace th15 {
AnmCheckpoint::Entry* AnmCheckpoint::capture_tree(const AnmVm& source,const AnmRegistry& registry,Entry* parent){
    auto entry=std::make_unique<Entry>();auto* out=entry.get();const u32 slot=entries.size()<0x1fff?u32(entries.size()):0x1fff;
    generation=(generation+1)&0x3ffff;if(!generation)generation=1;out->handle=0x80000000u|(generation<<13)|slot;
    out->vm.copy_checkpoint(source);out->vm.visual.render_flags|=0x4000000;out->parent=parent;
    if(parent){parent->children.push_back(out);out->vm.creation_parent=&parent->vm;out->vm.rotation_parent=parent->vm.rotation_parent;}
    entries.push_back(std::move(entry));
    if(out->vm.geometry.gather)for(auto& handle:out->vm.geometry.gather->handles)handle=capture(registry.find(handle),registry);
    order.insert(order.begin(),out);
    for(auto* child:registry.children(source))capture_tree(*child,registry,out);
    return out;
}
u32 AnmCheckpoint::capture(const AnmVm* source,const AnmRegistry& registry){return source?capture_tree(*source,registry,nullptr)->handle:0;}
const AnmVm* AnmCheckpoint::find(u32 handle)const noexcept{
    if(!handle)return nullptr;const u32 slot=handle&0x1fff;if(slot<0x1fff)return slot<entries.size()?&entries[slot]->vm:nullptr;
    for(const auto* entry:order)if(entry->handle==handle)return &entry->vm;return nullptr;
}
u32 AnmCheckpoint::child_handle(u32 handle,u32 index)const noexcept{const auto* vm=find(handle);if(!vm)return 0;for(const auto& entry:entries)if(&entry->vm==vm)return index<entry->children.size()?entry->children[index]->handle:0;return 0;}
u32 AnmCheckpoint::restore_tree(const Entry& source,AnmRegistry& registry,AnmVm* parent){
    auto* vm=registry.allocate();vm->copy_checkpoint(source.vm);vm->visual.render_flags&=~0x4000000u;
    if(vm->geometry.gather)for(auto& handle:vm->geometry.gather->handles){const u32 saved=handle;handle=restore(saved,registry);if(saved&&!handle){registry.discard(*vm);return 0;}}
    const u32 handle=registry.submit(*vm,vm->ordering);if(!handle){error=registry.error;registry.discard(*vm);return 0;}
    if(vm->ordering&4)vm->visual.render_flags&=~0xc000u;
    if(parent){vm->creation_parent=parent;vm->rotation_parent=parent->rotation_parent?parent->rotation_parent:parent;if(!registry.attach(*vm,*parent)){error=registry.error;registry.destroy_tree(*vm);return 0;}}
    for(const auto* child:source.children)if(!restore_tree(*child,registry,vm)){registry.destroy_tree(*vm);return 0;}
    return handle;
}
u32 AnmCheckpoint::restore(u32 handle,AnmRegistry& registry){if(!handle)return 0;const auto* vm=find(handle);if(!vm){error="Checkpoint animation unavailable";return 0;}for(const auto& entry:entries)if(&entry->vm==vm)return restore_tree(*entry,registry,nullptr);return 0;}
namespace {u32 file_word(const u8* p,u32 at){u32 v;std::memcpy(&v,p+at,4);return v;}void file_put(u8* p,u32 at,u32 v){std::memcpy(p+at,&v,4);}}
bool AnmCheckpoint::write_entry(const Entry& e,const AnmManager& manager,std::vector<u8>& output,bool sibling,u32 depth){
    if(depth>64){error="Checkpoint animation tree nesting exceeds limit";return false;}
    AnmFile codec;AnmFile::Block block;if(!codec.write(e.vm,manager,block)){error=codec.error;return false;}
    file_put(block.data(),0x544,e.handle);file_put(block.data(),0x588,0);file_put(block.data(),0x598,0);
    const size_t at=output.size();output.insert(output.end(),block.begin(),block.end());
    const auto particle=[&](u32 handle,std::vector<u8>& out)->bool{const auto* v=find(handle);if(!v)return false;for(const auto& entry:entries)if(&entry->vm==v)return write_entry(*entry,manager,out,false,depth+1);return false;};
    if(!codec.write_geometry(e.vm,manager,output,particle)){if(error.empty())error=codec.error;output.resize(at);return false;}
    // 4899f0 advances its serialized parent cursor to each child after
    // attaching it. Preserve that file topology, including an existing child
    // list on the preceding node. Stored links are presence/offset metadata;
    // the reader never dereferences persisted process addresses.
    size_t parent=at;
    for(const auto* child:e.children){const size_t start=output.size();if(!write_entry(*child,manager,output,false,depth+1)){output.resize(at);return false;}
        u32 first=file_word(output.data()+parent,0x598);
        if(!first)file_put(output.data()+parent,0x598,u32(start+1));
        else {size_t last=first-1;while(const u32 next=file_word(output.data()+last,0x588))last=next-1;file_put(output.data()+last,0x588,u32(start+1));}
        parent=start;
    }
    return true;
}
bool AnmCheckpoint::write_tree(u32 handle,const AnmManager& manager,std::vector<u8>& output){
    error.clear();const auto* vm=find(handle);if(!vm){error="Checkpoint animation tree is unavailable";return false;}
    for(const auto& entry:entries)if(&entry->vm==vm)return write_entry(*entry,manager,output,false,0);return false;
}
AnmCheckpoint::Entry* AnmCheckpoint::read_entry(const u8* bytes,u32 size,u32& consumed,AnmManager& manager,Entry* parent,u32 depth){
    consumed=0;if(depth>64||size<0x608||entries.size()>65536){error="Checkpoint animation tree is truncated or exceeds limits";return nullptr;}
    AnmFile codec;auto next=std::make_unique<Entry>();auto* e=next.get();
    if(!codec.read(bytes,size,e->vm,manager)){error=codec.error;return nullptr;}
    const u32 slot=entries.size()<0x1fff?u32(entries.size()):0x1fff;generation=(generation+1)&0x3ffff;if(!generation)generation=1;
    e->handle=0x80000000u|(generation<<13)|slot;e->parent=parent;
    e->vm.timer.set(e->vm.timer.current);e->vm.age.set(e->vm.age.current);e->vm.visual.render_flags|=0x4000000;
    if(parent){parent->children.push_back(e);e->vm.creation_parent=&parent->vm;e->vm.rotation_parent=parent->vm.rotation_parent;}
    entries.push_back(std::move(next));u32 geometry=0;
    const auto particle=[&](const u8* p,u32 n,u32& used)->u32{auto* value=read_entry(p,n,used,manager,nullptr,depth+1);return value?value->handle:0;};
    if(!codec.read_geometry(bytes,bytes+0x608,size-0x608,e->vm,manager,geometry,particle)){if(error.empty())error=codec.error;return nullptr;}
    consumed=0x608+geometry;order.insert(order.begin(),e);
    if(file_word(bytes,0x598)){
        bool more=true;while(more){if(size-consumed<0x608){error="Truncated checkpoint child animation";return nullptr;}
            const auto* child=bytes+consumed;more=file_word(child,0x588)!=0;u32 used=0;if(!read_entry(child,size-consumed,used,manager,e,depth+1))return nullptr;consumed+=used;
        }
    }
    return e;
}
u32 AnmCheckpoint::read_tree(const u8* bytes,u32 size,u32& consumed,AnmManager& manager){
    error.clear();const size_t count=entries.size(),previous_order=order.size();const u32 old_generation=generation;
    auto* root=read_entry(bytes,size,consumed,manager,nullptr,0);if(root)return root->handle;
    order.erase(order.begin(),order.begin()+(order.size()-previous_order));entries.resize(count);generation=old_generation;consumed=0;return 0;
}

}
