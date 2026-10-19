#include "enemy_opcode_misc.hpp"
#include "enemy_variables.hpp"
#include "../special_state/special.hpp"
namespace th20::source::gameplay {
namespace {
struct Host final:EnemyMiscOpcodeServices {
    Enemy* find(EnemyController& c,unsigned id) override{return static_cast<Enemy*>(find_enemy_in_list(c.enemies,id));}
    Enemy* selected(EnemyController& c,unsigned index) override{return static_cast<Enemy*>(selected_enemy(&c,index));}
    void select_script(Enemy& target,const char* name) override{target.clear_async();target.reset();target.select_script(name);}
    void attach_stone(unsigned enemy,int color) override{special_state::attach(*special_state::controller,enemy,color,special_state::environment());}
};
}
EnemyMiscOpcodeServices& enemy_misc_opcode_services(){static Host host;return host;}
EnemyOpcodeResult execute_enemy_misc_opcode(EnemyOpcodeReader& r){return execute_enemy_misc_opcode(r,enemy_misc_opcode_services());}
}
