#include "enemy_drop.hpp"
#include "enemy_opcode_data.hpp"
#include "../item_system/item.hpp"
#include "../runtime_state/state.hpp"
namespace th20::source::gameplay {
namespace {
class Source final:public EnemyDropServices {
    float random_angle() override{return recovered::mul32(state::signed_unit(state::random_streams[0]),opcode_data::f_0056e0f0);}
    float random_unit() override{return state::unit(state::random_streams[0]);}
    void spawn(int player,int type,const sprite::Vec3& position) override{
        item::spawn(*item::controller(player),type,position,0xffffffffu,opcode_data::f_0056e0f8/opcode_data::f_0056c8d0,opcode_data::f_0056fd34,0,0,-1);
    }
} source;
}
EnemyDropServices& enemy_drop_services(){return source;}
}
