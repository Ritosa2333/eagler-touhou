#include "EnemyManager.hpp"
namespace th11 {
EnemyManager::EnemyManager(EclProgram& p,EnemyEnvironment& e,EnemyCommandEnvironment& c,EnemyFrameWorld& w):program(&p),environment(e),commands(c),world(w){timer.set(0,&environment.rate);commands.manager=this;}
EnemyManager::~EnemyManager(){
    for(auto& [key,entry]:entries)entry->enemy.script.release_threads(entry->script);
    if(commands.manager==this)commands.manager=nullptr;
}
Enemy* EnemyManager::spawn(const char* name,const EnemySpawn& parameters){
    if(last_error||spawn_depth>=128){last_error=-2;return nullptr;}
    auto entry=std::make_unique<Entry>(environment,commands);auto& enemy=entry->enemy;auto& e=enemy.state;
    enemy.script.initialize_context();enemy.script.program=program;enemy.script.select_subroutine(name);
    if(!enemy.script.root.instruction){last_error=-2;return nullptr;}
    e.initialize(&enemy,&environment.rate);e.absolute.position=parameters.position;e.score=parameters.score;
    e.health=e.max_health=parameters.health;e.drops.primary=parameters.drop;e.flags|=(parameters.mirrored&1)<<15;
    enemy.script.root.difficulty=u8(1u<<(u32(environment.difficulty)&31));
    std::memcpy(e.integers,parameters.integers,48);e.damage_immunity.set(2,&environment.rate);
    e.flags|=(parameters.world_coordinates&1)<<22;if(parameters.health>999)e.flags|=0x4000000;
    // The native constructor executes the first frame before joining the list.
    // Child spawns therefore appear before their parent, and inherit RNG order.
    ++spawn_depth;const i32 result=enemy_frame(entry->script,world);--spawn_depth;
    if(result==-2||last_error){last_error=-2;if(!script_error.instruction)script_error=entry->script.error;enemy.script.release_threads(entry->script);return nullptr;}
    if(e.flags&0x80000){if(e.drops.primary==1)e.drops.primary=10;else if(e.drops.primary==4)e.drops.primary=11;}
    e.death_sound=i32(total_created&1)+2;e.death_animation=0x51;e.death_animation_file=0;
    if(e.bound_animation_file==1){switch(e.animation_script){case 5:case 50:e.death_animation=0x4e;break;case 10:case 51:e.death_animation=0x54;break;case 15:case 52:e.death_animation=0x57;break;}}
    auto* node=&e.manager_link;node->previous=last;
    if(last){node->next=last->next;if(node->next)node->next->previous=node;last->next=node;}else first=node;
    last=node;++count;++total_created;environment.enemy_count=i32(count);
    auto* result_enemy=&enemy;entries.emplace(result_enemy,std::move(entry));return result_enemy;
}
bool EnemyManager::destroy(Enemy& enemy){
    auto found=entries.find(&enemy);if(found==entries.end())return false;auto& e=enemy.state;auto& node=e.manager_link;
    if(e.deformation&&!world.release_deformation(e)){last_error=-2;return false;}
    if(first==&node)first=node.next;if(last==&node)last=node.previous;
    if(node.next)node.next->previous=node.previous;if(node.previous)node.previous->next=node.next;node.next=node.previous=nullptr;
    --count;environment.enemy_count=i32(count);
    if((e.flags&0x80000)&&u32(e.boss_slot)<8){commands.bosses[e.boss_slot]=nullptr;environment.boss=commands.bosses[0];}
    if(commands.animations)for(auto& id:e.animations){commands.animations->erase(id);id=0;}
    if(world.target==&enemy){world.target=nullptr;world.target_locked=false;}world.clear_shot_targets(&enemy);
    enemy.script.release_threads(found->second->script);entries.erase(found);return true;
}
bool EnemyManager::reset(EclProgram& next){
    while(first)if(!destroy(*first->value))return false;
    program=&next;total_created=0;last_error=0;script_error={};spawn_depth=0;
    timer.set(0,&environment.rate);commands.manager_flags=0;commands.remaining_phases=0;
    for(auto& boss:commands.bosses)boss=nullptr;
    for(auto& segment:commands.health_segments)segment={};
    environment.boss=nullptr;return true;
}
bool EnemyManager::update(){
    if(last_error)return false;
    for(auto* node=first;node;){auto* next=node->next;auto& enemy=*node->value;auto& entry=*entries.at(&enemy);
        const i32 result=enemy.state.flags&0x200000?1:enemy_frame(entry.script,world);
        if(result==-2||last_error){last_error=-2;if(!script_error.instruction)script_error=entry.script.error;return false;}
        if(result){if(!destroy(enemy))return false;}else enemy.state.flags&=~0x4000u;node=next;
    }
    timer.tick();return true;
}
    bool EnemyManager::erase_stage_enemies(bool matching_tag,u32 tag){
    for(auto* node=first;node;node=node->next){auto& e=node->value->state;
        if(((!(e.flags&0xa0)&&!(e.flags&0xc0400))||(e.flags&0x100))&&(!matching_tag||e.animation_options==tag)){
            if(e.death_animation>=0&&(!commands.animations||!commands.animations->effect_at(e.death_animation_file,e.death_animation,3,e.current.position,true))){last_error=-2;return false;}
            e.flags|=0x200000;
        }
    }timer.tick();return true;
}
bool EnemyManager::cancel_circle(Vec3 center,float radius,bool reward,bool convert){
    for(auto* node=first;node;){auto& e=node->value->state;node=node->next;
        if(!(e.flags&0x800))continue;
        const auto p=e.current.position;const float dx=float(double(p.x)-center.x),dy=float(double(p.y)-center.y);
        const float r=float(double(radius)+16),distance=float(double(dx)*dx+double(dy)*dy);
        if(!(double(distance)<=double(r)*r))continue;
        if(e.health_flags&1){e.scaled_health=wrapping_add(e.scaled_health,-99999);e.health=wrapping_add(wrapping_sub(e.scaled_health,signed_bits(u32(e.health_baseline)*7))/7,e.health_baseline);}
        else e.health=wrapping_add(e.health,-99999);
        if(double(p.x)+2>-192&&double(p.x)-2<192&&double(p.y)+2>0&&double(p.y)-2<448){
            if(convert&&!world.cancel_shot(p,normalize_angle(float(double(e.current.angle)+3.1415927410125732421875)))){last_error=-2;return false;}
            if(reward&&!world.cancel_reward(p)){last_error=-2;return false;}
        }
    }return true;
}
bool EnemyManager::cancel_beam(Vec3 center,float half_width,bool reward){
    // Native 412b10 includes both horizontal edges; its vertical edge is strict.
    for(auto* node=first;node;){auto& e=node->value->state;node=node->next;
        const auto p=e.current.position;
        if(!(e.flags&0x800)||!(double(center.x)-half_width<=p.x&&p.x<=double(center.x)+half_width&&p.y<center.y))continue;
        if(e.health_flags&1){e.scaled_health=wrapping_add(e.scaled_health,-99999);e.health=wrapping_add(wrapping_sub(e.scaled_health,signed_bits(u32(e.health_baseline)*7))/7,e.health_baseline);}
        else e.health=wrapping_add(e.health,-99999);
        if(reward&&double(p.x)+2>-192&&double(p.x)-2<192&&double(p.y)+2>0&&double(p.y)-2<448&&!world.cancel_reward(p)){last_error=-2;return false;}
    }return true;
}
i32 EnemyManager::command(EnemyState& e,EclContext& c,EnemyGlobals& g){
    const u16 op=c.instruction->opcode;
    if(op==0x149){if(!world.drop_items(e))return -2;e.drops.primary=0;return 0;}
    if(op==0x159)return erase_stage_enemies()?0:-2;
    if(op==0x173)return erase_stage_enemies(true,u32(c.integer_argument(0,g)))?0:-2;
    if(op==0x1c2){const i32 value=c.integer_argument(0,g);return world.add_score(value)&&world.score_popup(e.current.position,value)?0:-2;}
    if(op==0x150)return world.sound(c.integer_argument(0,g),e.current.position.x)?0:-2;
    const bool conditional=op==0x109||op==0x10a||op==0x10b||op==0x10c||op==0x10f;
    if(conditional&&commands.bosses[0])return 0;
    const bool relative=op==0x100||op==0x104||op==0x109||op==0x10b;
    const bool mirror=op==0x104||op==0x105||op==0x10b||op==0x10c;
    const bool world_space=op==0x10e||op==0x10f;
    if(!relative&&!mirror&&!world_space&&op!=0x101&&op!=0x10a)return -2;
    const auto* instruction=c.instruction;const u32 string_bytes=instruction->argument();
    const u32 start=20+string_bytes,argument_count=world_space?6:5;
    if(start>instruction->length||argument_count*4>instruction->length-start)return -2;
    const auto* name=reinterpret_cast<const char*>(instruction)+20;
    if(!std::memchr(name,0,string_bytes))return -2;
    const auto raw=[&](u32 index){u32 value;std::memcpy(&value,reinterpret_cast<const u8*>(instruction)+start+(index-1)*4,4);return value;};
    const auto integer=[&](u32 index){const i32 v=signed_bits(raw(index));return instruction->references&(1u<<index)?c.resolve_integer(v,g):v;};
    const auto floating=[&](u32 index){const float v=float_bits(raw(index));return instruction->references&(1u<<index)?c.resolve_float(v,g):double(v);};
    EnemySpawn value;
    const double x=floating(1);value.position.x=float(x+(relative?e.current.position.x:world_space?world.camera_position.x:0));
    const double y=floating(2);value.position.y=float(y+(relative?e.current.position.y:world_space?world.camera_position.y:0));
    if(relative){
        if(!mirror&&(e.flags&0x400000)){value.position.z=e.current.position.z;Vec3 projected;if(!world.project_spawn(value.position,projected))return -2;value.position={float(double(projected.x)-224),float(double(projected.y)-16),0};}
    }
    if(world_space){value.position.z=float(floating(3));value.world_coordinates=1;}
    const u32 skip=world_space?1:0;value.health=integer(3+skip);value.score=integer(4+skip);value.drop=integer(5+skip);value.mirrored=mirror;
    std::memcpy(value.integers,e.integers,48);return spawn(name,value)?0:-2;
}
}
