#include "EclRoutes.hpp"
#include "EnemyManager.hpp"
#include "BulletManager.hpp"
#include "LaserManager.hpp"
#include "Dialogue.hpp"
#include "SpellController.hpp"
#include "ScreenShake.hpp"
namespace th11 {
EnemyManager::EnemyManager(EclProgram& p,EnemyEnvironment& e,EnemyCommandEnvironment& c,EnemyFrameWorld& w):program(&p),environment(e),commands(c),world(w){timer.set(0,&environment.rate);
#if PORT_TITLE == 14
    environment.th14_enemy_list=&first;
    commands.th14_publish_owner(this);
#else
    commands.manager=this;
#endif
}
EnemyManager::~EnemyManager(){
    for(auto& [key,entry]:entries)entry->enemy.script.release_threads(entry->script);
#if PORT_TITLE == 14
    if(commands.th14_owner()==this)commands.th14_publish_owner(nullptr);
    if(environment.th14_enemy_list==&first)environment.th14_enemy_list=nullptr;
#else
    if(commands.manager==this)commands.manager=nullptr;
#endif
}
Enemy* EnemyManager::spawn(const char* name,const EnemySpawn& parameters){
#if PORT_TITLE == 16
    const auto failed_spawn=[&](i32 reason){if(!th16_spawn_failure){th16_spawn_failure=reason;std::snprintf(th16_failed_spawn,sizeof th16_failed_spawn,"%s",name?name:"");}};
#endif
    if(last_error||spawn_depth>=128){
#if PORT_TITLE == 16
        failed_spawn(last_error?1:2);
#endif
        last_error=-2;return nullptr;}
    auto entry=std::make_unique<Entry>(environment,commands);auto& enemy=entry->enemy;auto& e=enemy.state;
    enemy.script.initialize_context();enemy.script.program=program;enemy.script.select_subroutine(name);
    if(!enemy.script.root.instruction){
#if PORT_TITLE == 16
        failed_spawn(3);
#endif
        last_error=-2;return nullptr;}
    e.initialize(&enemy,&environment.rate);e.absolute.position=parameters.position;e.score=parameters.score;
    e.health=e.max_health=parameters.health;e.drops.primary=parameters.drop;if constexpr(kPortTitle!=16)e.flags|=(parameters.mirrored&1)<<(kPortTitle==14?19:15);
    enemy.script.root.difficulty=u8(1u<<(u32(environment.difficulty)&31));
    std::memcpy(e.integers,parameters.integers,48);e.damage_immunity.set(2,&environment.rate);
    if constexpr(kPortTitle!=16){e.flags|=(parameters.world_coordinates&1)<<(kPortTitle==14?26:22);if(parameters.health>999)e.flags|=kPortTitle==14?0x40000000:0x4000000;}
    if constexpr(kPortTitle==16)enemy_th16_bind_spawn(e,parameters,&environment.rate,commands.stage_section);
#if PORT_TITLE == 14 || PORT_TITLE == 16
    // TH14 422da3 / TH16 41b85f assign and advance the constructor ID.
    e.instance_id=i32(total_created);if(!++total_created)++total_created;
    e.parent_instance_id=parameters.parent_instance_id;
#endif
    // 412990 runs the constructor first, and the constructor executes the new enemy's first frame. Only after
    // that does it take the instance id from the manager's counter at iVar2+0x74 and store it in enemy[0x27b4],
    // and only after that does it advance the counter. A child spawned during the parent's first frame therefore
    // takes the lower id and the parent the next one, so ids stay distinct. Assigning the id before the first
    // frame instead gave parent and child the same value, and enemy_exists compares exactly this field, so any
    // wave waiting for id 0 to disappear could never see it go.
    // The native constructor executes the first frame before joining the list.
    // Child spawns therefore appear before their parent, and inherit RNG order.
    ++spawn_depth;const i32 result=[&](){
#if PORT_TITLE == 14
        auto* stack=commands.animations?&commands.animations->native_stack():nullptr;
        NativeStackTh14::CounterScope constructor_scope(stack?&stack->enemy_spawn_depth:nullptr);
#endif
        return enemy_frame(entry->script,world);
    }();--spawn_depth;
    if constexpr(kPortTitle!=14&&kPortTitle!=16)e.instance_id=i32(total_created);
    if(result==-2||last_error){
#if PORT_TITLE == 16
        failed_spawn(result==-2?4:5);
#endif
        last_error=-2;if(!script_error.instruction)script_error=entry->script.error;enemy.script.release_threads(entry->script);return nullptr;}
#if PORT_TITLE == 14
    enemy_th14_death_metadata(e);
#elif PORT_TITLE == 16
    enemy_th16_death_metadata(e);
#else
    if(e.flags&0x80000){if(e.drops.primary==1)e.drops.primary=10;else if(e.drops.primary==4)e.drops.primary=11;}
    // 412990 computes this from the same counter read as the id and adds three, not two.
    e.death_sound=i32((kPortTitle==14?u32(e.instance_id):total_created)&1)+3;e.death_animation=0x51;e.death_animation_file=0;
    if(e.bound_animation_file==1){switch(e.animation_script){case 5:case 50:e.death_animation=0x4e;break;case 10:case 51:e.death_animation=0x54;break;case 15:case 52:e.death_animation=0x57;break;}}
#endif
    auto* node=&e.manager_link;node->previous=last;
    if(last){node->next=last->next;if(node->next)node->next->previous=node;last->next=node;}else first=node;
    last=node;++count;if constexpr(kPortTitle!=14&&kPortTitle!=16)++total_created;environment.enemy_count=i32(count);
    auto* result_enemy=&enemy;entries.emplace(result_enemy,std::move(entry));return result_enemy;
}
bool EnemyManager::enemy_exists(i32 id)const noexcept{
    // TH12's FUN_00412500 walks DAT_004b43dc+0x68 comparing each enemy's +0x27b4; the port keeps the
    // same chain through EnemyState::manager_link and the same id in EnemyState::instance_id.
    for(const EnemyLink* node=first;node;node=node->next){
        const Enemy* enemy=reinterpret_cast<const Enemy*>(reinterpret_cast<const u8*>(node)-(offsetof(EnemyState,manager_link)+offsetof(Enemy,state)));
        if(enemy->state.instance_id==id)return true;
    }
    return false;
}
EnemyState* EnemyManager::th16_boss(u32 slot)const noexcept{
#if PORT_TITLE == 16
    if(slot>=4||!commands.th16_boss_ids[slot])return nullptr;
    EnemyState* found=nullptr;
    // Retail417580 returns the last visited enemy for an unmatched nonzero ID.
    for(auto* node=first;node;node=node->next){found=&node->value->state;if(found->instance_id==commands.th16_boss_ids[slot])break;}
    return found;
#else
    return nullptr;
#endif
}
bool EnemyManager::destroy(Enemy& enemy){
    auto found=entries.find(&enemy);if(found==entries.end())return false;auto& e=enemy.state;auto& node=e.manager_link;
    if(e.deformation&&!world.release_deformation(e)){last_error=-2;return false;}
    if(first==&node)first=node.next;if(last==&node)last=node.previous;
    if(node.next)node.next->previous=node.previous;if(node.previous)node.previous->next=node.next;node.next=node.previous=nullptr;
    --count;environment.enemy_count=i32(count);
    if constexpr(kPortTitle!=16)if((e.flags&(kPortTitle==14?0x800000:0x80000))&&u32(e.boss_slot)<commands.boss_capacity){commands.bosses[e.boss_slot]=nullptr;environment.boss=commands.bosses[0];}
#if PORT_TITLE == 16
    if((e.flags&0x800000)&&u32(e.boss_slot)<4){commands.th16_boss_ids[e.boss_slot]=0;commands.bosses[e.boss_slot]=nullptr;environment.boss=th16_boss(0);}
#endif
    if(commands.animations)for(auto& id:e.animations){commands.animations->erase(id);id=0;}
#if PORT_TITLE == 14
    if(commands.animations)for(auto& id:e.additional_animations){commands.animations->erase(id);id=0;}
#endif
    if(world.target==&enemy){world.target=nullptr;world.target_locked=false;}world.clear_shot_targets(&enemy);
    enemy.script.release_threads(found->second->script);entries.erase(found);return true;
}
bool EnemyManager::reset(EclProgram& next){
    while(first)if(!destroy(*first->value))return false;
    program=&next;total_created=kPortTitle==14?1u:0u;last_error=0;script_error={};spawn_depth=0;
    timer.set(0,&environment.rate);commands.manager_flags=0;commands.remaining_phases=0;
    for(auto& boss:commands.bosses)boss=nullptr;
#if PORT_TITLE == 16
    th16_spawn_failure=0;th16_failed_spawn[0]=0;
    for(auto& id:commands.th16_boss_ids)id=0;
    for(auto& row:commands.th16_health_segments)for(auto& segment:row)segment={};
#endif
    for(auto& segment:commands.health_segments)segment={};
#if PORT_TITLE == 14
    for(auto& row:commands.th14_health_segments)for(auto& segment:row)segment={};
#endif
    environment.boss=nullptr;return true;
}
#if PORT_TITLE == 14
bool EnemyManager::retire(){
    while(first)if(!destroy(*first->value))return false;
    first=last=nullptr;count=0;total_created=0;program=nullptr;
    environment.enemy_count=0;environment.boss=nullptr;
    for(auto& boss:commands.bosses)boss=nullptr;
    if(commands.th14_owner()==this)commands.th14_publish_owner(nullptr);
    if(environment.th14_enemy_list==&first)environment.th14_enemy_list=nullptr;
    return true;
}
bool EnemyManager::initialize_empty(EclProgram& next){
    if(owner_present()||count||first||last||!entries.empty())return false;
    // These projections were retired on Main before starting the worker.
    // Other Main callbacks may still read them with no owner. Do not issue
    // redundant same-value writes while constructing the private replacement.
    if(environment.enemy_count||environment.boss)return false;
    for(const auto* boss:commands.bosses)if(boss)return false;
    // Retail4228b7..4228ed clears the private manager, publishes its pointer,
    // then reserves ID zero. HUD+158 and HUD+1b8 are different owners: this
    // constructor must not clear remaining phases or the health marks.
    program=&next;total_created=0;last_error=0;script_error={};spawn_depth=0;
    timer.previous=timer.current=0;timer.fractional=0;timer.rate=nullptr;timer.flags=0;
    spawn_capacity=0;commands.manager_flags=0;
    environment.th14_enemy_list=&first;
    commands.th14_publish_owner(this);total_created=1;
    return true;
}
bool EnemyManager::finish_stage_owner(){
    if(!owner_present()||(timer.flags&1))return false;
    // Retail42244f..4224a8 follows the ECL loader and callback registration.
    // ECL/player activation remains disabled until Timeline Ready is acquired.
    timer.set(0,&environment.rate);spawn_capacity=99999;return true;
}
#endif
bool EnemyManager::update(){
    if(last_error)return false;
#if PORT_TITLE == 14
    // Other native calls can overwrite the constructor's stack between
    // manager passes. Only the proved current-pass producer is known here.
    if(commands.animations){auto& stack=commands.animations->native_stack();stack.spawn_scratch_known=false;stack.spawn_scratch_mask=0;}
#endif
    for(auto* node=first;node;){auto* next=node->next;auto& enemy=*node->value;auto& entry=*entries.at(&enemy);
        const i32 result=enemy.state.flags&((kPortTitle==14||kPortTitle==16)?0x2000000:0x200000)?1:enemy_frame(entry.script,world);
        if(result==-2||last_error){
#if PORT_TITLE == 16
            th16_frame_failure[0]=1;th16_frame_failure[1]=enemy.state.instance_id;th16_frame_failure[2]=i32(enemy.state.tick_callback);th16_frame_failure[3]=i32(enemy.state.damage_callback);th16_frame_failure[4]=i32(enemy.state.collision_callback);
            th16_frame_failure[5]=result;th16_frame_failure[6]=last_error;
            const auto& e=enemy.state;th16_frame_failure[7]=e.absolute.flags;th16_frame_failure[8]=e.relative.flags;th16_frame_failure[9]=e.absolute_angle.duration;th16_frame_failure[10]=e.relative_angle.duration;th16_frame_failure[11]=e.absolute_radius.duration;th16_frame_failure[12]=e.relative_radius.duration;th16_frame_failure[13]=e.absolute_ellipse.duration;th16_frame_failure[14]=e.relative_ellipse.duration;
#endif
            last_error=-2;if(!script_error.instruction)script_error=entry.script.error;return false;}
        if(result){if(!destroy(enemy))return false;}else enemy.state.flags&=~u32((kPortTitle==14||kPortTitle==16)?0x40000:0x4000);node=next;
    }
    timer.tick();return true;
}
    bool EnemyManager::erase_stage_enemies(bool matching_tag,u32 tag){
    if constexpr(kPortTitle==16){
        if(matching_tag)return false;
        for(auto* node=first;node;){auto& e=node->value->state;node=node->next;
            if(!enemy_th16_clear_drops(e))continue;
            EnemyScriptServices services(e,environment,commands);
            if(!commands.animations||enemy_death(e,*commands.animations,world,&services)==-2){last_error=-2;if(services.error.instruction)script_error=services.error;return false;}
            e.flags|=0x2000000;
        }
        timer.tick();return true;
    }
    if constexpr(kPortTitle==14){
        if(matching_tag)return false;
        for(auto* node=first;node;){auto& e=node->value->state;node=node->next;
            if(!enemy_th14_clear_drops(e))continue;
            EnemyScriptServices services(e,environment,commands);
            if(!commands.animations||enemy_death(e,*commands.animations,world,&services)==-2){last_error=-2;if(services.error.instruction)script_error=services.error;return false;}
            e.flags|=0x2000000;
        }
        timer.tick();return true;
    }
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
    if constexpr(kPortTitle==16){
        // Own422550 ->4319e0 returns immediately for an empty laser list.
        // Live-object virtual cancellation still needs the TH16 consumers.
#if PORT_TITLE == 16
        if(op==545)return commands.lasers&&commands.lasers->reset_th16()?0:-2;
        if(op==800){
            // Own422966 /41a9c0 /41a4f0 /41a5a0 /474890.
            const i32 id=c.integer_argument(0,g);Enemy* target=nullptr;
            if(id)for(auto* node=first;node;node=node->next){target=node->value;if(target->state.instance_id==id)break;}
            if(!target)return 0;
            const auto* instruction=c.instruction;if(instruction->length<=24)return -2;
            const char* name=reinterpret_cast<const char*>(instruction)+24;
            if(!std::memchr(name,0,instruction->length-24)||!target->script.program)return -2;
            EnemyScriptServices services(target->state,environment,commands);
            target->script.reset_threads(services);target->script.select_subroutine(name);
            return 0;
        }
#endif
        if(op==516)return world.sound(c.integer_argument(0,g),e.current.position.x)?0:-2; // Own421668.
#if PORT_TITLE == 16
        if(op==517){
            const i32 to=c.integer_argument(2,g),from=c.integer_argument(1,g),duration=c.integer_argument(0,g);
            if(!commands.shakes)return -2;commands.shakes->start(duration,from,to,&environment.rate);return 0;
        }
        if(op==509){
            if((commands.th16_run_flags&0x30)==0x20)return 0;
            if(!enemy_th16_time_drop_decay(e))return -2;
            return world.drop_items_now_th16(e)?0:-2;
        }
        if(op==300||op==301||op==304||op==305||(op>=309&&op<=312)||op==321){
            // Retail41dd58 conditional lookup; 423050 compares signed counts.
            if(op>=309&&op<=312&&th16_boss(0))return 0;
            if(signed_bits(count)>=signed_bits(spawn_capacity))return 0;
            EnemySpawn value;const char* name=nullptr;if(!enemy_th16_spawn_parameters(e,c,g,value,name))return -2;
            return spawn(name,value)?0:-2;
        }
        if(op==502||op==503){
            // Retail420493/4204cb: flags, then all sixteen animation handles.
            const u32 bits=u32(c.integer_argument(0,g));e.flags=op==502?e.flags|bits:e.flags&~bits;
            const bool visible=!(e.flags&0x20),update=op==502?!visible:visible;
            if(update){if(!commands.animation_visibility)return -2;for(u32 slot=0;slot<16;++slot)commands.animation_visibility(e.animation_handle(slot),visible,commands.animation_user);}
            return 0;
        }
        if(op==512){
            const i32 slot=c.integer_argument(0,g);commands.manager_flags&=~1u;
            if(slot<0){
                if(e.flags&0x800000){if(u32(e.boss_slot)>=4)return -2;commands.th16_boss_ids[e.boss_slot]=0;commands.bosses[e.boss_slot]=nullptr;}
                e.flags&=~0x800000u;
            }else{
                if(u32(slot)>=4)return -2;
                e.flags|=0x800000;commands.th16_boss_ids[slot]=e.instance_id;commands.bosses[slot]=&e;e.boss_slot=slot;
            }
            environment.boss=th16_boss(0);return 0;
        }
        if(op==520)return th16_boss(0)||th16_boss(1)||th16_boss(2)?-1:0;
        if(op==525)return erase_stage_enemies()?0:-2;
#endif
        // Retail 4216b3: starter, bullet cancel, laser cancel, stage-enemy
        // erase. Cancellation algorithms remain the shared implementation.
        if(op==518){
            if(!commands.dialogue||!commands.bullets||!commands.lasers||!commands.cancellation)return -2;
            if(!commands.dialogue->start_dialogue(c.integer_argument(0,g)))return -2;
            if(!commands.bullets->cancel_all(false,*commands.cancellation))return -2;
            if(!commands.lasers->cancel_all(false,false))return -2;
            return erase_stage_enemies()?0:-2;
        }
    }
    if constexpr(kPortTitle==14){
        if(op==432||op==433){
            // 4271eb/4271fc -> actual 421aa0 lookup. An unmatched nonzero
            // ID returns the last visited entry in retail, not null.
            const i32 id=c.integer_argument(0,g);Enemy* found=nullptr;
            if(id)for(auto* node=first;node;node=node->next){found=node->value;if(found->state.instance_id==id)break;}
            if(!found)return -2; // Retail dereferences null for an invalid empty lookup.
            (op==432?e.absolute:e.relative).position=found->state.current.position;
            return 0; // No immediate coordinate combine or mode/velocity reset.
        }
        if(op==516)return world.sound(c.integer_argument(0,g),e.current.position.x)?0:-2;
        if(op==517){
            // 4287e7 -> 46d1c0 mode 1: read end, start, duration in that order.
            const i32 to=c.integer_argument(2,g),from=c.integer_argument(1,g),duration=c.integer_argument(0,g);
            if(!commands.shakes)return -2;
            commands.shakes->start(duration,from,to,&environment.rate);return 0;
        }
#if PORT_TITLE == 14
        if(op==802){
            // 429a9e walks three boss owners, excluding the caller's slot.
            // A qualifying health interrupt resets/selects its root without
            // executing the new subroutine or consuming the interrupt.
            const u32 index=u32(c.integer_argument(0,g));if(index>=8)return -2;
            for(u32 slot=0;slot<3;++slot){auto* target=commands.bosses[slot];if(i32(slot)==e.boss_slot||!target)continue;
                const auto& phase=target->interrupts[index];if(phase.health<0||!phase.health_script)continue;
                auto* owner=target->script_owner;if(!owner||!owner->script.program)return -2;
                EnemyScriptServices services(*target,environment,commands);owner->script.reset_threads(services);
                owner->script.root.stack.top=owner->script.root.stack.frame_base=0;owner->script.select_subroutine(phase.health_script);
            }return 0;
        }
        if(op==509){
            // 4277fd -> 421560; spell practice skips this request entirely.
            if((commands.th14_run_flags&0x30)==0x20)return 0;
            return world.drop_items(e)?0:-2;
        }
        if(op==545){
            return commands.lasers&&commands.lasers->reset_th14()?0:-2;
        }
        if(op==515){e.damage_immunity.set(c.integer_argument(0,g),&environment.rate);return 0;}
        if(op==613){
            if(!commands.bullets||!commands.lasers)return -2;
            return commands.bullets->cancel_all_th14()&&commands.lasers->cancel_all(true,false)?0:-2;
        }
        if(op==518){
            if(!commands.dialogue||!commands.bullets||!commands.lasers)return -2;
            const i32 id=c.integer_argument(0,g);
            if(!commands.dialogue->start_dialogue(id))return -2;
            if(!commands.bullets->cancel_all_th14())return -2;
            if(!commands.lasers->cancel_all(false,false))return -2;
            return erase_stage_enemies()?0:-2;
        }
#endif
        if(op==523){
            // 4289ea -> 41d450: an inactive spell takes the immediate return.
            if(world.spell_flags&1){
                if constexpr(kPortTitle==14){if(!commands.spells||!commands.spells->end())return -2;}
                else return -2;
            }
            e.health_flags&=~1u;return 0;
        }
#if PORT_TITLE == 14
        if(op==615||op==616){
            const float radius=float(c.float_argument(0,g));
            if(!commands.bullets||!commands.lasers)return -2;
            if(!commands.bullets->cancel_circle_th14(e.current.position,radius,op==615))return -2;
            return commands.lasers->cancel_circle(e.current.position,radius,op==615?1u:0u,true)<0?-2:0;
        }
#endif
        if(op==525){
            return erase_stage_enemies()?0:-2;
        }
        if(op==300||op==301||op==304||op==305||(op>=309&&op<=312)||op==321){
            if(op>=309&&op<=312&&commands.bosses[0])return 0;
            if(count>=spawn_capacity)return 0;
            EnemySpawn value;const char* name=nullptr;if(!enemy_th14_spawn_parameters(e,c,g,value,name))return -2;
            return spawn(name,value)?0:-2;
        }
        return -2;
    }
    // 0x149 is deliberately absent: TH12 gives it the interpolator body shared with 0x131 and
    // 0x14b, which enemy_movement_command implements. TH11's drop handler of the same number was
    // dead here because the movement gate runs first.
    if(op==0x159)return erase_stage_enemies()?0:-2;
    if(op==0x173)return erase_stage_enemies(true,u32(c.integer_argument(0,g)))?0:-2;
    if(op==0x1c2){const i32 value=c.integer_argument(0,g);return world.add_score(value)&&world.score_popup(e.current.position,value)?0:-2;}
    if(op==0x150)return world.sound(c.integer_argument(0,g),e.current.position.x)?0:-2;
    // TH12's 0x1a0 is a sound instruction, not a laser one. FUN_00453e20 looks up a
    // 16-bit field in the 0x14-stride table at 0x4ae5aa (about 56 entries, ending
    // just before the 49-pointer sound table at 0x4aea50), then queues a value into
    // the 12-channel table at DAT_004cf508, each channel carrying a count at
    // DAT_004cf538 and up to 128 values at DAT_004cf568. The consumer FUN_00453fa0
    // walks those channels each frame and makes COM calls at vtable+0x24 and +0x48
    // behind LeaveCriticalSection - DirectSound buffer control. The sound id arrives
    // as the first ECL argument (FUN_0041a900(0)'s low dword).
    //
    // The port dispatches by id through the host's sound layer and does not model the
    // per-channel value queue or the per-frame buffer updates, which are audio mixing
    // detail the host does not expose.
    if(op==0x1a0)return world.sound(c.integer_argument(0,g),e.current.position.x)?0:-2;
    const bool conditional=op==0x109||op==0x10a||op==0x10b||op==0x10c||op==0x10f;
    if(conditional&&commands.bosses[0])return 0;
    // TH12's 0x100 and 0x118 are one shared case body in FUN_004154d0
    // (decomp_out/FUN_004154d0_004154d0.c, switchD_00415546_caseD_100), so 402's
    // successor 280 is the same relative spawn with signature mSSSSS.
    const bool relative=op==0x100||ecl_named_spawn_relative(op)||op==0x104||op==0x109||op==0x10b;
    const bool mirror=op==0x104||op==0x105||op==0x10b||op==0x10c;
    const bool world_space=op==0x10e||op==0x10f;
    if(!relative&&!mirror&&!world_space&&op!=0x101&&op!=0x10a)return -2;
    // TH12 computes the first argument's position as a WORD index, not as a byte offset:
    // switchD_00415546_caseD_101 (257) and its sibling caseD_100 do
    //     iVar10 = (arg0 + 4 + ((arg0+4) >> 31 & 3)) >> 2;
    //     fVar23 = FUN_0041a9b0(1, pfVar11[iVar10 + 4]);
    // where arg0 is the name's byte length and pfVar11 walks the instruction as words, so the two
    // floats sit at absolute argument iVar10 and iVar10+1 - i.e. at the first word boundary at or
    // after the name. The port used to start at 20 + string_bytes, which lands mid-word whenever the
    // name length is not a multiple of four. Rounding up is what TH12 does; the two agree exactly
    // when string_bytes % 4 == 0, so this only changes the unaligned case.
    //
    // Round 50's comment here claimed the integer arguments were read at absolute indices 3, 4, 5
    // while the floats were word-relative, and warned that the whole family's health/score/drop
    // might come from the wrong words. That was wrong, and the disassembly of the same case shows it.
    //
    // FUN_0041a970(this, param_1) takes its RAW VALUE IN EAX, not as an argument:
    //     this_00 = script->active;
    //     if ((1 << (param_1 & 0x1f) & instruction->references) != 0)
    //         return FUN_00468f70(this_00, in_EAX);      // resolve the value held in EAX
    //     return CONCAT44(this_00, in_EAX);              // else the raw value itself
    // and the call site loads that value from the instruction:
    //     mov eax,[edi + esi*4 + 0x18]   ; instruction + 24 + esi*4 = arg[iVar10 + 2]
    //     push 3 / call 0x41a970          ; reference bit 3
    // so all five arguments are relative to the same iVar10: the floats at arg[iVar10 + 0] and
    // arg[iVar10 + 1] carrying bits 1 and 2, the integers at arg[iVar10 + 2], [+3], [+4] carrying
    // bits 3, 4 and 5. The port's floating(1)/floating(2) and integer(3+skip)/(4+skip)/(5+skip)
    // match that exactly, both in the bit tested and in the word read: with start = 20 + align4(sb)
    // the port's raw(3) is instruction + 28 + sb, and TH12's is 16 + 4*iVar10 + 8 = 28 + sb.
    //
    // This is the third time a decompiler artifact has misled a reading - a float literal that was
    // really a register in round 45, extraout_ECX before that, and now in_EAX here. When an
    // accessor's parameter list does not explain where its value comes from, read the call site's
    // disassembly before drawing a conclusion.
    const auto* instruction=c.instruction;const u32 string_bytes=instruction->argument();
    const u32 start=20+((string_bytes+3u)&~3u),argument_count=world_space?6:5;
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
