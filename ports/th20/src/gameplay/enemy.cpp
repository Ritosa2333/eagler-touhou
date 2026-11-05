#include "enemy.hpp"
#include "script_loader.hpp"
#include "../sprite_renderer/pool.hpp"
#include <cstring>
#include <new>
#include <stdexcept>
namespace th20::source::gameplay {
namespace {
int __cdecl update(void* self) {return static_cast<EnemyController*>(self)->update_callback();}
int __cdecl draw(void* self) {return static_cast<EnemyController*>(self)->draw_callback();}
}
EnemyController::EnemyController(EnemyServices& host):loaded_names{},services(&host) {
    construct_enemy_data(data);field_c4=field_c8=field_cc=0;timer_d0={};field_e0=0;
    for(auto& file:animation_files)file=nullptr;loader=nullptr;scheduler::initialize_list(enemies);
    field_120=field_124=field_128=0;player_index=0;context=nullptr;
    runtime::log_printf(host.log(),"initialize EnemyCtrlInf\n");advance_enemy_generation(player_index);
}
void EnemyController::clear_entities() {
    auto& sprites=services->sprites();
    for(int i=0;i<8;++i)sprite::mark_file_animations(sprites,sprites.files[i+0x19],false);
    for(scheduler::Iterator iterator(enemies.sentinel.next);iterator.current;iterator.advance()){
        auto* entity=iterator.current->value;iterator.current=nullptr;
        services->retire_entity(entity);
    }
    for(auto& handle:data.handles_44)handle=0;loaded_names.clear();disable_callbacks();
    field_cc=0;data.fields_30[1]=0;data.fields_30[0]=0;reset_enemy_counters(data);
}
EnemyController::~EnemyController() {
    runtime::log_printf(services->log(),"shutdown EnemyCtrlInf\n");clear_entities();
    scheduler::remove(services->scheduler_state(),services->scheduler_environment(),update_node);
    scheduler::remove(services->scheduler_state(),services->scheduler_environment(),draw_node);
    clear_script_cache();delete loader;loader=nullptr;
    for(int i=0;i<8;++i)sprite::unload_animation_file(services->sprites(),i+0x19);
}
int EnemyController::initialize(int index,const char* path) {
    context=&game_session::context(index);context->objects_04[1]=this;player_index=index;
    loader=new ScriptLoader(*services);loader->bind_player(index);loader->load(path);
    update_node=scheduler::register_callback(services->scheduler_state(),services->scheduler_environment(),0x24,update,this,false,false);
    draw_node=scheduler::register_callback(services->scheduler_state(),services->scheduler_environment(),0x17,draw,this,true,false);
    recovered::timer_set(data.timer_8c,0);data.field_88=99999;field_c8=0;return 0;
}
void EnemyController::reset_for_stage() {recovered::timer_set(data.timer_8c,0);scheduler::initialize_list(enemies);}
int EnemyController::update_callback() {
    if(!controller || controller->update_suppressed() || (controller->game_flags&(1u<<11)) ||
       controller->animation_frozen() || (controller->game_flags&(1u<<7)) || (controller->game_flags&(1u<<19)))return 1;
    return services->update_enemy(*this);
}
int EnemyController::draw_callback() {services->select_layer(6,player_index);services->draw_enemy_overlay();return 1;}
EnemyController* create_enemy_controller(EnemyServices& services,int index,const char* path) {
    auto* result=new EnemyController(services);
    if(result->initialize(index,path)!=0) {runtime::retire_callback_owner(result);return nullptr;}
    return result;
}
EnemyController& enemy_controller(int index) {
    auto* result=static_cast<EnemyController*>(game_session::context(index).objects_04[1]);
    if(!result)throw std::logic_error("Required EnemyController is absent");return *result;
}
void destroy_enemy_controller(int index) {
    auto& context=game_session::context(index);
    if(context.objects_04[1]) {runtime::retire_callback_owner(static_cast<EnemyController*>(context.objects_04[1]));context.objects_04[1]=nullptr;}
}
}
