#include "stage_reset.hpp"
#include "../program_entry/program_entry.hpp"
#include "../sprite_renderer/animation_file.hpp"
#include "../sprite_renderer/pool.hpp"
#include <cstring>
namespace th20::source::bullet {
void reset_for_stage(Controller& c){
    sprite::mark_file_animations(*program_entry::sprite_controller,program_entry::sprite_controller->files[7],false);
    // Original483ad0 byte-clears all2001 records, including the shared_ptr
    // representation, without releasing prior metadata. Preserve that ordering
    // and original MSVC x86 empty representation; this is not entity retirement.
    std::memset(c.pool,0,sizeof(c.pool));c.next_bullet=c.pool;c.pool[2000].state=5;
    std::memset(c.handles,0,sizeof(c.handles));scheduler::initialize_list(c.free);
    for(unsigned i=0;i<2000;++i){auto& b=c.pool[i];scheduler::initialize_link(b.link,reinterpret_cast<scheduler::Node*>(&b));b.index=i;scheduler::insert_after(c.free.sentinel,b.link);b.link.owner=&c.free;if(c.free.tail==&c.free.sentinel)c.free.tail=&b.link;}
    scheduler::initialize_list(c.active);c.vector_4c={0,0};c.item_counter=0;c.field_286d90=0;c.field_286d94=0;c.field_286d98=0;
}
}
