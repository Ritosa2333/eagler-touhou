#include "EnemyVariables.hpp"
namespace th10 {
// 0x426660. Coincident positions deliberately aim straight down.
Extended EnemyEnvironment::aim_at_player(const Vec3& position) const noexcept {
    const auto x=number(player_position->x)-number(position.x);
    const auto y=number(player_position->y)-number(position.y);
    if(x==number(0.0f)&&y==number(0.0f))return number(1.5707963705062866f);
    return angle_to(y,x);
}
// Original virtual methods 0x411fc0, 0x412350. Integer and float variants
// intentionally differ for angle-only variables and the lifetime timer.
i32 EnemyVariables::integer(i32 id){
    switch(id){
    case -10000:return static_cast<i32>(environment.script_rng->next_u32());
    case -9999:return environment.script_rng->unit().truncate_int();
    case -9987:return environment.script_rng->signed_unit().truncate_int();
    case -9998:case -9989:case -9956:case -9955:return 0;
    case -9988:return enemy.lifetime.current;
    case -9961:return environment.animation(enemy.animations[0])->script_index;
    case -9953:case -9952:case -9951:case -9950:return *environment.difficulty==id+9953;
    default:return floating(id).truncate_int();
    }
}
Extended EnemyVariables::floating(i32 id){
    switch(id){
    case -10000:{
        const auto value=static_cast<i32>(environment.script_rng->next_u32());
        auto result=Extended::from_int(value);
        if(value<0)result=result+number(4294967296.0f);
        return result;
    }
    case -9999:return environment.script_rng->unit();
    case -9998:return environment.script_rng->signed_unit()*number(3.1415927410125732f);
    case -9987:return environment.script_rng->signed_unit();
    case -9997:case -9977:return number(enemy.current.position.x);
    case -9996:case -9976:return number(enemy.current.position.y);
    case -9995:case -9975:return number(enemy.absolute.position.x);
    case -9994:case -9974:return number(enemy.absolute.position.y);
    case -9993:case -9973:return number(enemy.relative.position.x);
    case -9992:case -9972:return number(enemy.relative.position.y);
    case -9991:case -9965:return number(environment.player_position->x);
    case -9990:case -9964:return number(environment.player_position->y);
    case -9989:return environment.aim_at_player(enemy.current.position);
    case -9988:return number(enemy.lifetime.fractional);
    case -9986:return Extended::from_int((enemy.flags>>16)&1);
    case -9985:case -9984:case -9983:case -9982:return Extended::from_int(enemy.integer_variables[id+9985]);
    case -9981:case -9980:case -9979:case -9978:return number(enemy.float_variables[id+9981]);
    case -9971:return number(enemy.absolute.angle);
    case -9970:return number(enemy.relative.angle);
    case -9969:return number(enemy.absolute.speed);
    case -9968:return number(enemy.relative.speed);
    case -9967:return number(enemy.absolute.radius);
    case -9966:return number(enemy.relative.radius);
    case -9963:return number(environment.boss->state.current.position.x);
    case -9962:return number(environment.boss->state.current.position.y);
    case -9960:return Extended::from_int(*environment.rank);
    case -9959:return Extended::from_int(*environment.difficulty);
    case -9958:return angle_to(number(enemy.current.velocity.y),number(enemy.current.velocity.x));
    case -9957:return number(1.0f);
    case -9956:return environment.aim_at_player(enemy.absolute.position);
    case -9955:return environment.aim_at_player(enemy.relative.position);
    case -9954:return Extended::from_int(enemy.health);
    case -9953:case -9952:case -9951:case -9950:return Extended::from_int(*environment.difficulty==id+9953);
    default:return number(0.0f);
    }
}
// 0x412300 / 0x4126d0: only the enemy's four local slots are writable.
i32* EnemyVariables::integer_reference(i32 id){return id>=-9985&&id<=-9982?enemy.integer_variables+(id+9985):nullptr;}
float* EnemyVariables::float_reference(i32 id){return id>=-9981&&id<=-9978?enemy.float_variables+(id+9981):nullptr;}
}
