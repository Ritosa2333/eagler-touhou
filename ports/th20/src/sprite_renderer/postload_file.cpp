#include "texture_load.hpp"
#include "binding.hpp"
#include "anm_vm.hpp"
#include <atomic>
namespace th20::source::sprite {
AnimationFile* postload_animation_file(AnimationFile& file,TextureContext& context,runtime::Log& log) {
    auto stage=std::atomic_ref<std::uint32_t>(file.fields_5c[0]);
    auto* header=reinterpret_cast<AnmHeader*>(file.bytes);
    std::uint32_t index=0,texture=0;std::int32_t sprite=0,script=0;bool processed=false;
    for(;;) {
        if(index==stage.load()-1u) {
            if(postload_animation_entry(file,texture,sprite,script,header,context,log)<0){stage.store(0);return nullptr;}
            processed=true;
        }
        sprite+=header->sprite_count;script+=header->script_count;++texture;
        if(!header->next_offset)break;
        header=reinterpret_cast<AnmHeader*>(reinterpret_cast<std::uint8_t*>(header)+header->next_offset);++index;
        if(index==stage.load()||processed){stage.fetch_add(1);return &file;}
    }
    for(std::uint32_t i=0;i<file.script_count;++i) {
        auto& animation=file.templates[i];reset_animation_state(animation);clear_animation_suffix(animation);
        initialize_animation_script(file,animation,static_cast<std::int32_t>(i));
        th20::recovered::timer_set(animation.timer_4c8,-1);th20::recovered::timer_set(animation.timer_4d8,-1);
        execute_animation(animation);
    }
    stage.store(0);return &file;
}
}
