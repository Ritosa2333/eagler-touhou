#include "environment.hpp"
#include "../runtime_core/runtime_core.hpp"
#include <cstring>
#include <new>
namespace th20::source::special_state {
Controller* controller=nullptr;
Controller* create(){
    auto* value=static_cast<Controller*>(::operator new(sizeof(Controller),std::nothrow));
    if(value){std::memset(value,0,sizeof(*value));construct(*value);}controller=value;return value;
}
void destroy(Controller& owner,Environment& host){
    // Advance while the current allocation is alive. Original frees first and
    // then touches the former link's observer; no game-visible callback occurs
    // between these operations, so retain its free order without a C++ UAF.
    for(scheduler::Iterator it(owner.entries.sentinel.next);it.current;){auto* value=reinterpret_cast<Entry*>(it.current->value);it.advance();host.free_entry(*value);}
}
void release(){if(controller){destroy(*controller,environment());std::lock_guard<std::recursive_mutex> lock(runtime::shared_locks().slot(1));::operator delete(controller);}controller=nullptr;}
void retire(Entry& value,Environment& host){scheduler::unlink(value.link);host.delete_animation(value.animation_handle);host.free_entry(value);}
Entry* attach(Controller& owner,unsigned enemy_handle,int selected,Environment& host){
    auto* entry=static_cast<Entry*>(::operator new(sizeof(Entry),std::nothrow));if(!entry)return nullptr;
    std::memset(entry,0,sizeof(*entry));construct(*entry);scheduler::append(owner.entries,entry->link);host.initialize_entry(*entry,enemy_handle,selected);return entry;
}
void update(Controller& owner){update(owner,environment());}
void draw(Controller& owner){draw(owner,environment());}
}
