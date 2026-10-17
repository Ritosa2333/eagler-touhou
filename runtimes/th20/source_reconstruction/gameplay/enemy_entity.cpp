#include "enemy_entity.hpp"
#include "enemy_variables.hpp"
#include <new>
#include <cstring>
namespace th20::source::gameplay {
void ScriptManager::reset(){
    main.time=0;main.instruction_offset=main.subroutine=-1;main.async_id=-1;main.manager=this;
    main.flags&=~1u;main.field_2c=0;main.interpolators.clear();current=&main;
    main.stack.pointer=main.stack.frame_base=0;main.stack.words.clear();main.stack.words.reserve(0x100); //49c220 reserves capacity; logical size remains zero.
    scheduler::initialize_link(runtimes,reinterpret_cast<scheduler::Node*>(&main));
}
void ScriptManager::clear_async(){
    auto* link=runtimes.next;while(link){auto* next=link->next;auto* task_runtime=reinterpret_cast<EnemyRuntime*>(link->value);
        if(task_runtime){task_runtime->~EnemyRuntime();runtime::release_bytes(task_runtime);}runtime::release_bytes(link);link=next;
    }
    //Original leaves the sentinel's next pointer unchanged; reset immediately
    //reconstructs it at the ordinary clear+reset callers.
}
ScriptManager::~ScriptManager(){clear_async();}
void ScriptManager::select_script(const char* name){current->subroutine=loader->find(name);current->instruction_offset=0;current->time=0;}
Enemy::Enemy():field_70(0),state{},spawn_parameters{},callback{},player_index(0),context(nullptr){
    scheduler::initialize_link(controller_link,reinterpret_cast<scheduler::Node*>(this));construct_spawn_parameters(spawn_parameters);
    scheduler::initialize_list(children);parent_link={};
}
void Enemy::select_context(int index,game_session::Session& session) noexcept{
    player_index=index;context=&session.contexts[index];state.fields_2c8[8]=index;state.fields_2c8[9]=reinterpret_cast<std::uint32_t>(context);
}
void Enemy::initialize(int index,const char* name,game_session::Session& session){
    reset();select_context(index,session);state.entity=this;state.initialize();scheduler::initialize_list(children);children.sentinel.value=reinterpret_cast<scheduler::Node*>(this);
    scheduler::initialize_link(parent_link,reinterpret_cast<scheduler::Node*>(this));callback=nullptr;
    state.identifier=current_enemy_generation;(void)advance_enemy_generation(static_cast<EnemyController*>(context->objects_04[1])->player_index);
    loader=static_cast<EnemyController*>(context->objects_04[1])->loader;select_script(name);
}
Enemy* spawn_enemy(EnemyController& owner,const char* name,const SpawnParameters& parameters,Enemy* parent){
    void* storage=runtime::allocate_bytes(sizeof(Enemy));if(!storage)throw std::bad_alloc();std::memset(storage,0,sizeof(Enemy));
    auto* entity=new(storage)Enemy;entity->initialize(owner.player_index,name,game_session::session);
    if(parent)scheduler::append(parent->children,entity->parent_link);
    apply_enemy_spawn(entity,parameters,game_session::session,enemy_frame_services());
    scheduler::insert_after(owner.enemies.sentinel,entity->controller_link);entity->controller_link.owner=&owner.enemies;
    if(owner.enemies.tail==&owner.enemies.sentinel)owner.enemies.tail=&entity->controller_link;
    ++owner.field_124;return entity;
}
void retire_enemy(Enemy* entity){if(entity){entity->~Enemy();runtime::release_bytes(entity);}}
Enemy::~Enemy(){
    for(scheduler::Iterator iterator(children.sentinel.next);iterator.current;iterator.advance())reinterpret_cast<Enemy*>(iterator.current->value)->state.fields_2c8[1]|=0x200u;
    scheduler::unlink(parent_link);auto& owner=*static_cast<EnemyController*>(context->objects_04[1]);
    if(owner.field_120==reinterpret_cast<std::uint32_t>(&controller_link))owner.field_120=reinterpret_cast<std::uint32_t>(controller_link.next);
    scheduler::unlink(controller_link);if(!(state.fields_2c8[1]&0x40000u))--owner.field_124;
    if(!(state.fields_2c8[1]&0x40000u)){
        if(state.fields_2c8[1]&0x80u)owner.data.handles_44[state.fields_250[8]]=0;
        for(auto& animation:state.animations)enemy_lifecycle_services().delete_animation(animation.handle);
    }
    auto*& mesh_owner=*reinterpret_cast<EnemyMeshOwner**>(&state.fields_2c8[3]);
    if(mesh_owner){enemy_lifecycle_services().destroy_mesh(mesh_owner->mesh);mesh_owner->mesh=nullptr;runtime::release_bytes(mesh_owner);mesh_owner=nullptr;}
}
int Enemy::execute_opcode(){return unrecovered::execute_enemy_opcode_0048c010(state);}
std::int32_t Enemy::read_integer(std::int32_t variable){return unrecovered::read_enemy_integer_0049abc0(*this,variable);}
float Enemy::read_float(std::int32_t variable){return unrecovered::read_enemy_float_004995d0(*this,variable);}
std::uint32_t* Enemy::integer_destination(std::int32_t variable){return enemy_integer_destination(this,variable);}
std::uint32_t* Enemy::float_destination(std::int32_t variable){return enemy_float_destination(this,variable);}
}
