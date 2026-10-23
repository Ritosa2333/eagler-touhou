#pragma once
#include <cstdint>

namespace th08 {
inline constexpr std::uint32_t deathbomb_stimulus_version=1;
inline constexpr std::uint32_t deathbomb_stimulus_word_count=36;

struct ValidationDeathbombEmissionSpec {
    std::uint32_t sprite=0,color=0,count=1,layers=1,pattern=0;
    float x=0,y=0,z=0,angle=0,spread=0,speed=0,ending_speed=0;
};
inline ValidationDeathbombEmissionSpec validation_deathbomb_emission_spec(float x,float y,float z)noexcept{
    return{0,0,1,1,0,x,y,z,0,0,0,0};
}

inline bool validation_deathbomb_stimulus_eligible(bool scene_enabled,bool write_locked,
    bool runtime_ready,bool normal_game,bool quiet_world,bool template_ready,
    std::int32_t life_state,bool already_requested)noexcept{
    return scene_enabled&&write_locked&&runtime_ready&&normal_game&&quiet_world&&template_ready&&
        life_state==0&&!already_requested;
}

struct DeathbombStimulusReceiptV1 {
    static constexpr std::uint32_t Idle=0,Accepted=1,Refused=2;
    static constexpr std::uint32_t NoProjectile=0,Emitted=1,Collided=2,RetiredWithoutHit=3;
    static constexpr std::uint32_t NoSource=0,BulletSystemProjectile=1;

    std::uint32_t request_status=Idle,request_sequence=0,projectile_status=NoProjectile;
    std::uint32_t collision_source=NoSource,collision_sequence=0,consumer_sequence_at_collision=0;
    std::uint32_t life_state_before_collision=0,life_state_after_collision=0,collision_result=0;

    void refuse()noexcept{if(!request_sequence)request_status=Refused;}
    bool accept()noexcept{
        if(request_sequence||request_status==Accepted)return false;
        request_status=Accepted;request_sequence=1;projectile_status=Emitted;return true;
    }
    bool observe_collision(std::int32_t before,std::int32_t after,std::int32_t predead,
        std::int32_t result,std::uint32_t consumer_sequence,bool consumer_valid)noexcept{
        if(request_status!=Accepted||projectile_status!=Emitted||before!=0||after!=2||predead<=0||result!=1||!consumer_valid)return false;
        collision_source=BulletSystemProjectile;
        ++collision_sequence;
        consumer_sequence_at_collision=consumer_sequence;
        life_state_before_collision=static_cast<std::uint32_t>(before);
        life_state_after_collision=static_cast<std::uint32_t>(after);
        collision_result=static_cast<std::uint32_t>(result);
        projectile_status=Collided;
        return true;
    }
    void retire_without_hit()noexcept{
        if(projectile_status==Emitted)projectile_status=RetiredWithoutHit;
    }
};
}
