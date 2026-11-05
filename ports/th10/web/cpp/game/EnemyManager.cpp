#include "EnemyManager.hpp"
namespace th10 {
// Constructor in 0x40d6b0. The active-system pointer is owned by the host.
void EnemyManager::initialize() noexcept {__builtin_memset(this,0,sizeof(*this));flags=2;}
// 0x40cfb0. Initial ECL execution precedes list insertion and can spawn more
// enemies. Read the manager tail and spawn counter after that execution.
Enemy* EnemyManager::spawn(const char* subroutine,const EnemySpawnParameters& parameters,EnemyManagerEnvironment& env){
    auto* enemy=env.allocate_enemy();if(!enemy)__builtin_trap();enemy->initialize(*program,subroutine,env.rate,env.enemy_type_table);auto& e=enemy->state;
    e.absolute.position=parameters.position;e.score=parameters.score;e.health=parameters.health;e.drops.kind=parameters.drop_kind;
    e.flags=(e.flags&~0x800u)|((static_cast<u32>(parameters.mirror)<<11)&0x800);reinterpret_cast<u8*>(&enemy->script.root.difficulty)[0]=static_cast<u8>(1u<<(static_cast<u32>(*env.difficulty)&31));
    __builtin_memcpy(e.integer_variables,parameters.integer_variables,sizeof(e.integer_variables));__builtin_memcpy(e.float_variables,parameters.float_variables,sizeof(e.float_variables));
    if(!(e.damage_immunity_flags&1)){e.damage_immunity.rate=env.rate;e.damage_immunity_flags|=1;}
    e.damage_immunity.previous=1;e.damage_immunity.current=2;e.damage_immunity.fractional=2;
    e.flags=(e.flags&~0x40000u)|((parameters.flags<<18)&0x40000);env.update_enemy(*enemy);
    if(e.flags&0x8000){if(e.drops.kind==1)e.drops.kind=10;else if(e.drops.kind==4)e.drops.kind=11;}
    e.death_sound=(spawn_count&1)+2;e.death_animation=0x167;
    if(e.bound_animation_file==1)switch(e.animation_script){
        case 5:case 0x19:case 0x32:e.death_animation=0x164;break;
        case 10:case 0x1e:case 0x33:e.death_animation=0x16a;break;
        case 15:case 0x23:case 0x34:e.death_animation=0x16d;break;
        default:break;
    }
    e.death_animation_file=0;if(!head)head=&e.manager_node;else e.manager_node.insert_after(*tail);
    tail=&e.manager_node;count=wrapping_add(count,1);++spawn_count;return enemy;
}
// 0x40d750. The cached next node keeps newly appended enemies for the next
// pass; nested spawns already execute their initialization frame in spawn().
i32 EnemyManager::update(EnemyManagerEnvironment& env){
    for(auto* node=head;node;){auto* next=node->next;
        if(!(node->value->state.flags&0x20000)&&env.update_enemy(*node->value)==0)node->value->state.flags&=~0x400u;
        else if(node->value)env.destroy_enemy(*node->value);node=next;
    }
    lifetime.tick();return 1;
}
// 0x40e6a0. Clearing marks eligible enemies for deletion, without drops. Its
// timer tick is intentional even when called between the manager's updates.
void EnemyManager::clear_enemies(EnemyManagerEnvironment& env){
    for(auto* node=head;node;){auto& e=node->value->state;node=node->next;
        if(!(e.flags&0xc050)||(e.flags&0x80)){if(e.death_animation>=0)env.spawn_death_effect(e);e.flags|=0x20000;}
    }
    lifetime.tick();
}
}
