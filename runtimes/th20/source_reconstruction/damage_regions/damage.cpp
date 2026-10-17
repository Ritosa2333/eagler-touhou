#include "damage.hpp"
#include "../gameplay/enemy_variables.hpp"
#include <cstring>
#include <emmintrin.h>
namespace th20::source::damage {
namespace {
template<class T>T read(const void* object,std::size_t offset){T value;std::memcpy(&value,static_cast<const std::uint8_t*>(object)+offset,sizeof(value));return value;}
template<class T>void write(void* object,std::size_t offset,T value){std::memcpy(static_cast<std::uint8_t*>(object)+offset,&value,sizeof(value));}
int add(int a,int b){return recovered::signed_bits(static_cast<unsigned>(a)+static_cast<unsigned>(b));}
int subtract(int a,int b){return recovered::signed_bits(static_cast<unsigned>(a)-static_cast<unsigned>(b));}
void* enemy_in(game_session::Context& context,std::uint32_t handle){return gameplay::find_enemy_in_list(static_cast<gameplay::EnemyController*>(context.objects_04[1])->enemies,handle);}
sprite::Vec3 midpoint(const sprite::Vec3& a,const sprite::Vec3& b){sprite::Vec3 result;const float* first=&a.x,*second=&b.x;float* out=&result.x;for(unsigned i=0;i<3;++i)out[i]=_mm_cvtss_f32(_mm_add_ss(_mm_div_ss(_mm_sub_ss(_mm_set_ss(first[i]),_mm_set_ss(second[i])),_mm_set_ss(2.f)),_mm_set_ss(second[i])));return result;}
}
bool player_frame_changed(const void* player) noexcept{return read<int>(player,0x648)!=read<int>(player,0x644);}
int player_damage_cap(const void* entity,const void* overlay,const game_session::Player& player) noexcept{
    const auto* parameters=read<const void*>(entity,0x2208);
    if(read<int>(overlay,0x54)==1)return read<int>(parameters,0x30+player.fields_00[4]*12);
    if(read<std::uint8_t>(entity,0x204c)!=0)return read<int>(parameters,0x2c+player.fields_00[4]*12);
    return read<int>(parameters,0x28+player.fields_00[5]*12);
}
int calculate_damage(HitCtrlInf& owner,const sprite::Vec3& position,const sprite::Vec2* size,float angle,float radius,std::uint32_t* hit_flag,sprite::Vec3* hit_position,int preview,std::uint32_t target,Environment& host){
    if(!player_frame_changed(owner.context->objects_04[0]))return 0;
    auto total=host.bomb_damage(*owner.context,position,size);if(hit_flag)*hit_flag=total>0?1u:0u;
    int group_used[5]{},group_base[5]{},group_effective[5]{};
    for(scheduler::Iterator iterator(owner.active.sentinel.next);iterator.current;iterator.advance()){
        auto& region=*reinterpret_cast<Region*>(iterator.current->value);
        if(region.lifetime.current==region.lifetime.previous||region.lifetime.current%region.period!=0||region.cooldown>=1||region.damage==0||!intersects(region,position,size,angle,radius))continue;
        const auto group=region.damage_group;
        if(group>0&&group<5){if(group_used[group]&&region.damage<group_base[group])continue;group_used[group]=1;total=subtract(total,group_effective[group]);group_base[group]=region.damage;}
        if(target){
            if(region.last_target==target)continue;region.last_target=target;
            auto* first_owner=game_session::context(0).objects_04[1];
            auto* enemy=first_owner?enemy_in(game_session::context(0),target):nullptr;
            if(enemy&&(region.flags&0x20)){write(enemy,0x22c,read<unsigned>(enemy,0x22c)|2u);continue;}
        }
        if(hit_flag&&(region.flags&0x10))*hit_flag=1;
        const bool expired=region.damage_limit<9999999&&region.damage_limit<=region.total_damage;
        const auto handle=region.handle;
        const auto contact=region.motion.position;
        const auto flags=region.flags;
        auto current=region.damage;
        if(!preview){region.total_damage=add(region.total_damage,region.damage);if(region.hit_callback){region.callback_target=target;const auto result=host.hit_callback(region,position,size,angle,radius);if(result>=0)current=result;}}
        // Shot hit callbacks can retire and free this region, or recycle its
        // pool slot. Finish accounting from the contact captured before dispatch.
        if(expired)if(auto* remaining=owner.find(handle))retire(*remaining);
        if(group>0&&group<5)group_effective[group]=current;
        total=add(total,current);if(hit_position)*hit_position=contact;
        if(total>0)if(auto* enemy=enemy_in(*owner.context,target)){
            const auto reward=(read<unsigned>(enemy,0x354)&0x40000000u)?recovered::signed_bits(static_cast<unsigned>(total)*10u):total;
            host.damage_reward(midpoint(contact,position),reward);
            if(flags&0x40)write(enemy,0x358,read<unsigned>(enemy,0x358)|0x10u);
        }
    }
    if(player_damage_cap(owner.context->objects_04[0],game_session::overlay_owner(0),*game_session::context(0).current_player)<total)
        total=player_damage_cap(owner.context->objects_04[0],game_session::overlay_owner(0),*game_session::context(0).current_player);
    if(!preview&&total!=0)add_score(game_session::player(0),static_cast<unsigned>(add(total/10,10)));
    return total;
}
}
