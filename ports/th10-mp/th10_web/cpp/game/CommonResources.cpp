#include "CommonResources.hpp"
#include <initializer_list>
namespace th10 {
// 0x401000. The final zero-fill also erases the preliminary vtable and VM
// constructors. These are the only values left by the original constructor.
void CommonResources::initialize(CommonResources** current) noexcept {std::memset(this,0,sizeof(*this));flags=2;color=0xffffffff;scale={1,1};character_width=9;*current=this;}
// 0x401110.
i32 CommonResources::start(CommonResourceEnvironment& env){
    effects=env.load_animations(2,env.effects_name);if(!effects){env.report_error();return -1;}
    text_animations=env.load_animations(0,env.text_name);if(!text_animations){env.report_error();return -1;}
    capture=env.load_animations(3,env.capture_name);if(!capture){env.report_error();return -1;}
    update_entry=(*env.chain)->add(env.update_callback,this,4,false,false,*env.callbacks);
    draw_entry=(*env.chain)->add(env.draw_callback,this,48,true,false,*env.callbacks);
    early_draw_entry=(*env.chain)->add(env.early_draw_callback,this,38,true,false,*env.callbacks);
    auto* file=effects;characters.initialize();characters.animation_file=file;env.bind_sprite(*file,characters,0);
    file=effects;small_characters.initialize();small_characters.animation_file=file;env.bind_sprite(*file,small_characters,98);return 0;
}
// 0x401260. Reload each slot after the release callback before deleting its
// allocation, preserving the original ownership order during shutdown.
void CommonResources::shutdown(CommonResourceEnvironment& env){
    virtual_table=env.virtual_table;(*env.chain)->remove_locked(update_entry,*env.callbacks);(*env.chain)->remove_locked(draw_entry,*env.callbacks);(*env.chain)->remove_locked(early_draw_entry,*env.callbacks);
    for(i32 slot:{2,0,3})if(env.slots[slot]){env.release_animations(*env.slots[slot]);env.delete_object(env.slots[slot]);env.slots[slot]=nullptr;}
    *env.current=nullptr;
    if(small_characters.geometry)env.free_geometry(small_characters.geometry);small_characters.geometry=nullptr;
    if(characters.geometry)env.free_geometry(characters.geometry);characters.geometry=nullptr;
}
// 0x401440.
CommonResources* CommonResources::create(CommonResourceEnvironment& env){auto* value=env.allocate();if(!value)return nullptr;value->initialize(env.current);if(value->start(env)){value->shutdown(env);env.delete_object(value);return nullptr;}return value;}
// 0x4014d0 / 0x4014f0.
i32 CommonResources::update() noexcept {text_count=early_text_count=0;frames=wrapping_add(frames,1);return 1;}
// 0x41ff00, used once the loading screen hands control to the title screen.
void CommonResources::enable() noexcept {update_entry->flags|=2;draw_entry->flags|=2;early_draw_entry->flags|=2;}
// 0x401530 / 0x4015c0. The early queue intentionally leaves the shadow field
// untouched. Unused bytes after the string terminator also remain unchanged.
void CommonResources::queue(const char* value,const Vec3& position,bool early) noexcept {
    auto& count=early?early_text_count:text_count;if(count>=(early?64:256))return;
    auto& entry=(early?early_text:text)[count++];std::strcpy(entry.text,value);entry.position=position;entry.color=color;entry.scale=scale;entry.camera=camera;entry.font=0;if(!early)entry.shadow=shadow;
}
// 0x401690 also marks the last item when an already-full queue rejects text.
void CommonResources::mark_small() noexcept {if(text_count>0)text[text_count-1].font=1;}
// 0x401760 / 0x401a50. Both queues share the first embedded VM. Pixel alignment
// on the early queue is disabled when the horizontal scale differs from one.
i32 CommonResources::draw(bool early,AsciiRenderEnvironment& env){
    auto& vm=characters;vm.flags=(vm.flags&0xffd7ffff)|0x140001;i32 previous_camera=1;
    auto& count=early?early_text_count:text_count;auto* entries=early?early_text:text;
    for(i32 index=0;index<count;++index){auto& entry=entries[index];vm.script_position=entry.position;vm.scale=entry.scale;vm.flags|=8;
        const bool small=!early&&entry.font==1;const float advance=small?7:(Extended::from_int(character_width)*number(entry.scale.x)).to_float(),line_height=small?9:14;
        if(previous_camera!=entry.camera){previous_camera=entry.camera;env.select_camera(previous_camera!=0);}
        for(const auto* ch=reinterpret_cast<const u8*>(entry.text);*ch;++ch){
            if(*ch==10){vm.script_position.y=(number(line_height)*number(entry.scale.y)+number(vm.script_position.y)).to_float();vm.script_position.x=entry.position.x;continue;}
            if(*ch!=32){
                const i32 sprite=static_cast<i32>(static_cast<u32>(*ch)-32+(early?0:static_cast<u32>(entry.font)*98));vm.sprite=effects->sprites+sprite;
                if(!early){vm.sprite_size={vm.sprite->width,vm.sprite->height};vm.flags|=8;
                    if(entry.shadow){vm.color=(entry.color>>25)<<24;vm.script_position.x=Scalar::add(vm.script_position.x,2);vm.script_position.y=Scalar::add(vm.script_position.y,2);env.draw_character(vm,true);vm.script_position.x=Scalar::sub(vm.script_position.x,2);vm.script_position.y=Scalar::sub(vm.script_position.y,2);}
                }
                vm.color=entry.color;env.draw_character(vm,!early||number(vm.scale.x)==number(1));
            }
            vm.script_position.x=Scalar::add(advance,vm.script_position.x);
        }
    }
    if(previous_camera!=0)env.select_camera(false);return 1;
}
}
