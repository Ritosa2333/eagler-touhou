#include "hud.hpp"
#include "../sprite_renderer/binding.hpp"
#include "../sprite_renderer/named_spawn.hpp"
#include "../sprite_renderer/render_mesh.hpp"
#include "../effect_system/effect.hpp"
namespace th20::source::hud {
namespace s=sprite;
namespace {
std::uint32_t spawn(s::AnimationFile& file,const char* name,int script){return s::spawn_named_animation(environment::sprites(),file,name,script);}
void assign(std::uint32_t handle,int sprite){auto& c=environment::sprites();if(auto* a=s::find_animation(c,handle))s::assign_animation_sprite(s::script_file(c,*a),*a,sprite);}
}
void notify(FrontInf& owner,int type,int value){
    auto& c=environment::sprites();
    if(type==0){
        s::request_animation_deletion(c,owner.notice_handles[0]);owner.notice_handles[0]=spawn(*owner.front_file,"front",49);
        int remainder=value,place=10000000;bool significant=false;
        for(unsigned i=0;i<8;++i){s::request_animation_deletion(c,owner.score_handles[i]);owner.score_handles[i]=spawn(environment::notice_file(),nullptr,i+4);const int digit=remainder/place;if(digit)significant=true;assign(owner.score_handles[i],digit+239);
            if(auto* a=s::find_animation(c,owner.score_handles[i])){if(significant)effects::enable_animation_tree(*a);else s::hide_animation_tree(*a);}remainder%=place;place/=10;
        }
        s::request_animation_deletion(c,owner.score_handles[8]);if(value>999999){owner.score_handles[8]=spawn(environment::notice_file(),nullptr,12);assign(owner.score_handles[8],253);}
        s::request_animation_deletion(c,owner.score_handles[9]);if(value>999){owner.score_handles[9]=spawn(environment::notice_file(),nullptr,13);assign(owner.score_handles[9],253);}
        owner.fields_11c[7]=1;owner.handles_f8[7]=spawn(*owner.front_file,"front",84);
    }else if(type==1){s::request_animation_deletion(c,owner.notice_handles[0]);owner.notice_handles[0]=spawn(*owner.front_file,"front",50);owner.fields_11c[7]=1;owner.handles_f8[7]=spawn(*owner.front_file,"front",84);}
    else if(type>=2&&type<=4){s::request_animation_deletion(c,owner.notice_handles[1]);owner.notice_handles[1]=spawn(*owner.front_file,"front",type+49);}
    else if(type==6){s::request_animation_deletion(c,owner.notice_handles[0]);owner.notice_handles[0]=spawn(*owner.front_file,"front",54);}
}
}
