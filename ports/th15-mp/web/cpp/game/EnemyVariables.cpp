#include "EnemyVariables.hpp"
#include <cmath>
namespace th15 {
namespace {
float direction(const Vec3& velocity)noexcept{return float(std::atan2(double(velocity.y),double(velocity.x)));}
float toward(const Vec3& from,const Vec3& target)noexcept{const float x=float(target.x-from.x),y=float(target.y-from.y);if(x==0&&y==0)return 1.57079637050628662109375f;return float(std::atan2(double(y),double(x)));}
float distance(const Vec3& a,const Vec3& b)noexcept{const float x=float(a.x-b.x),y=float(a.y-b.y);return float(std::sqrt(double(float(float(x*x)+float(y*y)))));}
}
bool EnemyVariables::numeric(i32 index,float& value)noexcept{
    switch(index){
        case 3:case 23:value=enemy.motion.position.x;break;case 4:case 24:value=enemy.motion.position.y;break;
        case 5:case 25:value=enemy.absolute.position.x;break;case 6:case 26:value=enemy.absolute.position.y;break;
        case 7:case 27:value=enemy.relative.position.x;break;case 8:case 28:value=enemy.relative.position.y;break;
        case 9:case 35:value=world.player_position.x;break;case 10:case 36:value=world.player_position.y;break;
        case 19:case 20:case 21:case 22:value=enemy.float_registers[index-19];break;
        case 29:value=enemy.absolute.angle;break;case 30:value=enemy.relative.angle;break;
        case 31:value=enemy.absolute.speed;break;case 32:value=enemy.relative.speed;break;
        case 33:value=enemy.absolute.radius;break;case 34:value=enemy.relative.radius;break;
        case 42:value=direction(enemy.motion.velocity);break;case 56:value=distance(enemy.motion.position,world.player_position);break;
        case 61:case 62:case 63:case 64:value=world.boss?world.boss->float_registers[index-61]:0;break;
        case 65:case 66:case 67:case 68:value=enemy.temporary_registers[index-65];break;
        case 78:case 79:case 80:case 81:case 82:case 83:case 84:case 85:value=world.float_registers[index-78];break;
        case 89:value=world.boss?direction(world.boss->motion.velocity):0;break;case 90:value=world.boss?world.boss->absolute.speed:0;break;
        default:return false;
    }return true;
}
bool EnemyVariables::integer(i32 id,i32& value){const i32 index=wrapping_add(id,10000);float number;if(numeric(index,number)){value=truncate_int(number);return true;}value=0;switch(index){
    case 0:value=signed_bits(random.next32()&0x7fffffff);break;case 1:value=truncate_int(random.unit());break;case 13:value=truncate_int(random.signed_unit());break;
    case 12:value=enemy.age_timer.current;break;case 14:value=(enemy.flags>>24)&1;break;
    case 15:case 16:case 17:case 18:value=enemy.integer_registers[index-15];break;
    case 37:case 38:if(!world.boss)return false;value=truncate_int(index==37?world.boss->motion.position.x:world.boss->motion.position.y);break;
    case 39:value=i16(enemy.animation_frame);break;case 40:value=world.rank;break;case 41:value=world.difficulty;break;case 43:value=1;break;case 46:value=enemy.life;break;
    case 47:case 48:case 49:case 50:value=world.difficulty==index-47;break;
    case 51:case 52:case 53:value=world.counters[index-51];break;case 54:value=world.enemy_count;break;case 55:value=wrapping_add(world.mode,world.submode);break;
    case 57:case 58:case 59:case 60:value=world.boss?world.boss->integer_registers[index-57]:0;break;
    case 69:value=signed_bits(world.total);break;case 70:value=world.state;break;case 73:value=world.spell_state==0&&world.player_state!=0;break;
    case 74:case 75:case 76:case 77:value=world.integer_registers[index-74];break;
    case 86:value=signed_bits(enemy.id);break;case 91:value=signed_bits(enemy.parent_id);break;case 92:value=world.active_count();break;
    case 93:value=world.counter1;break;case 94:value=(enemy.flags>>19)&1;break;case 95:value=world.current_chapter;break;case 96:value=world.counter3;break;
    }return true;
}
bool EnemyVariables::floating(i32 id,float& value){const i32 index=wrapping_add(id,10000);if(numeric(index,value))return true;value=0;switch(index){
    case 0:value=float(random.next32()&0x7fffffff);break;case 1:value=random.unit();break;case 2:value=float(random.signed_unit()*3.1415927410125732421875f);break;case 13:value=random.signed_unit();break;
    case 11:value=toward(enemy.motion.position,world.player_position);break;case 12:value=enemy.age_timer.fractional;break;
    case 37:value=world.boss?world.boss->motion.position.x:0;break;case 38:value=world.boss?world.boss->motion.position.y:128;break;
    case 44:value=toward(enemy.absolute.position,world.player_position);break;case 45:value=toward(enemy.relative.position,world.player_position);break;
    case 86:value=float(enemy.id);break;case 91:value=float(enemy.parent_id);break;case 69:value=float(world.total);break;
    case 39:case 96:break;
    default:{i32 integer_value;if(!integer(id,integer_value))return false;value=float(integer_value);break;}
    }return true;
}
i32* EnemyVariables::integer_destination(i32 id){const i32 index=wrapping_add(id,10000);if(index>=15&&index<=18)return &enemy.integer_registers[index-15];if(index>=51&&index<=53)return &world.counters[index-51];if(index>=57&&index<=60)return world.boss?&world.boss->integer_registers[index-57]:&enemy.integer_registers[index-57];if(index>=74&&index<=77)return &world.integer_registers[index-74];return nullptr;}
float* EnemyVariables::float_destination(i32 id){const i32 index=wrapping_add(id,10000);switch(index){case 5:return &enemy.absolute.position.x;case 6:return &enemy.absolute.position.y;case 7:return &enemy.relative.position.x;case 8:return &enemy.relative.position.y;default:break;}if(index>=19&&index<=22)return &enemy.float_registers[index-19];if(index>=61&&index<=64)return world.boss?&world.boss->float_registers[index-61]:&enemy.float_registers[index-61];if(index>=65&&index<=68)return &enemy.temporary_registers[index-65];if(index>=78&&index<=85)return &world.float_registers[index-78];return nullptr;}
}
