#include "option_frame.hpp"
#include <bit>
#include <cstring>
namespace th20::source::player_entity {
namespace {
int shot_script(const Player& player,unsigned offset,int variant){int value;std::memcpy(&value,static_cast<const std::uint8_t*>(player.shot_data)+offset+variant*4,4);return value;}
int signed32(std::uint32_t value){return std::bit_cast<int>(value);}
int smooth_step(int target,int current,std::uint32_t factor){return signed32((std::uint32_t(target)-std::uint32_t(current))*factor)/100;}
}
int update_option(Player& player,Option& option,OptionFrameServices& env){
    if(!option.state)return 0;auto& power=env.power();auto& callbacks=env.callbacks();
    auto& cached_focus=reinterpret_cast<std::uint8_t*>(option.fields_f4)[0x10];
    if(cached_focus!=bool(power.global_player().focused_204c)){
        const int amount=clamped_power(*player.context->current_player);const int maximum=clamped_maximum_power(*player.context->current_player);
        if(amount>=maximum){power.delete_animation(option.handle_e0);option.handle_e0=power.spawn(*player.animation_file,shot_script(player,0xa8,power.script_variant()),-1,0);env.execute_interrupt(option.handle_e0,7);}
        power.delete_animation(option.handle_dc);option.handle_dc=power.spawn(*player.animation_file,shot_script(player,0x88,power.script_variant()),14,0);env.execute_interrupt(option.handle_dc,7);
        cached_focus=power.global_player().focused_204c!=0;
    }
    if(!(player.entity_flags&2u)){
        if(!(option.fields_f4[3]&2u)){
            const bool focused=player.focused_204c!=0;option.vector_70=add_fixed_coordinates(player.fixed_position,option.offsets_80[focused?1:0]);
#ifdef TH_SDL3
            // Stored option callbacks are __fastcall (Option*,void* dummy);
            // wasm lowers conventions away, so the dummy is a real argument.
            const auto callback=option.fields_f4[focused?10:11];if(callback)reinterpret_cast<void(*)(Option*,void*)>(callback)(&option,nullptr);
#else
            const auto callback=option.fields_f4[focused?10:11];if(callback)reinterpret_cast<void(__thiscall*)(Option*)>(callback)(&option);
#endif
        }
    }else{
        option.vector_70=player.fixed_position;
        if(signed32(player.fields_20e4[1])>29){option.state=0;callbacks.interrupt(option.handle_dc,1);callbacks.interrupt(option.handle_e0,1);return 0;}
    }
    if(!option.fields_f4[2]){
        if(signed32(player.fields_20e4[0])>29){const Fixed2 delta{smooth_step(option.vector_70.x,option.vector_78.x,player.fields_20e4[0]),smooth_step(option.vector_70.y,option.vector_78.y,player.fields_20e4[0])};
            if(delta.x==0&&delta.y==0)option.vector_78=option.vector_70;else option.vector_78=add_fixed_coordinates(option.vector_78,delta);
        }
    }else{--option.fields_f4[2];option.vector_78=option.vector_70;}
    const sprite::Vec3 position{float(option.vector_78.x)/128.0f,float(option.vector_78.y)/128.0f,0};
    if(auto* animation=callbacks.find_animation(option.handle_dc))animation->vector_5bc=position;if(auto* animation=callbacks.find_animation(option.handle_e0))animation->vector_5bc=position;
    option.previous_position=position;return 0;
}
}
