#include "GameSession.hpp"
#include "HudTh14Text.hpp"
#include "Th14LifeFragments.hpp"

#if PORT_TITLE == 14
#include <eagler/gameplay/CooperativeRules.hpp>
#include <cstdio>

namespace th11 {
namespace {
constexpr u32 skip=0x200;
}

bool GameBattle::setup_coop(const std::array<Th14CoopLoadout,3>& loadouts,u32 seats,u32 viewer,bool challenge,bool observer,bool keep_bullets_on_miss){
    if(seats<2||seats>3||viewer>=seats||!initialized||
       loadouts[0].character!=enemy_environment.character||loadouts[0].subtype!=enemy_environment.subtype)
        return unavailable("invalid TH14 co-op setup");
    coop_seats=seats;coop_viewer=viewer;coop_challenge=challenge;coop_observer=observer;
    coop_keep_bullets_on_miss=keep_bullets_on_miss;
    if(!hud.layout_coop(seats,challenge))return unavailable("TH14 multiplayer HUD setup");
    player->shots.coop_draw_file=20;
    player->motion.th14_coop_draw_tag=20;
    // The local seat already has options when cooperative setup takes over.
    for(const auto& option:player->motion.options)
        for(const auto id:{option.animation,option.auxiliary})
            if(auto* vm=animations.find(id))vm->reserved_3a0=20;
    if(challenge){economy.lives=0;economy.life_fragments=0;economy.th14_bomb_fragments=0;}
    economy.th14_initial_bombs=challenge?0:std::max(0,economy.th14_bombs-1);
    economy.th14_bombs=economy.th14_initial_bombs;
    economy.th14_small_power=i32(seats);
    for(u32 seat=1;seat<seats;++seat){
        const auto [character,subtype]=loadouts[seat];
        auto& mate=teammates[seat-1];mate=std::make_unique<Teammate>();
        if(!resources.load_coop_player(character,subtype,mate->animation,mate->shot)){
            error=resources.error;return false;
        }
        mate->economy=economy;
        auto& stock=mate->economy;
        stock.power_step=100;
        stock.max_power=mate->shot.header.max_power_level*100;
        stock.power=std::clamp(economy.power,0,stock.max_power);
        mate->player=std::make_unique<PlayerFrame>(mate->shot,mate->animation,resources.core.bullet,animations,stock,*this,9,6);
        auto& p=*mate->player;
        p.shots.coop_draw_file=u16(20+seat);
        p.motion.th14_coop_draw_tag=u8(20+seat);
        p.input=player_input;p.input.movement.character=character;p.input.movement.subtype=subtype;
        mate->bomb=std::make_unique<BombController>(animations,mate->animation,p,*this,9,&resources.core.text);
        mate->bomb->selection=character*2+subtype;
        CoopActor actor(*this,seat);
        if(!p.initialize())return unavailable("TH14 teammate initialization");
        p.state.minimum_point_value=1'000'000;
        p.motion.state.x=(seat==1?-24:24)*128;
        p.motion.state.position.x=float(p.motion.state.x)/128;
        if(!p.start_stage())return unavailable("TH14 teammate stage reset");
    }
    enemy_environment.th14_aim=th14_enemy_aim;enemy_environment.th14_aim_user=this;
    for(u32 seat=0;seat<seats;++seat){
        CoopActor actor(*this,seat);const auto& stock=pilot_economy(seat);
        display_lives(stock.lives,stock.life_fragments);
        bombs_changed(stock.th14_bombs,stock.th14_bomb_fragments);
    }
    assign_coop_items();
    return true;
}

bool GameBattle::coop_bomb_available(u32 seat)const{
    if(coop_challenge||!pilot_live(seat)||!enemies.count||(dialogue&&dialogue->active))return false;
    const auto* own=pilot_bomb(seat);
    if(!own)return false;
    if(own->selection==4)return !own->state.active;
    if(coop_bomb_invincibility.current>0)return false;
    if(own->selection==3&&coop_marisa_b_cooldown[seat].current>0)return false;
    return !own->state.active;
}

void GameBattle::th14_bomb_invincibility(i32 frames){
    if(coop_seats==1)return;
    if(coop_bomb_invincibility.current<frames){
        coop_bomb_invincibility.set(frames,&animations.rate);
        coop_bomb_sakuya_b=pilot_bomb(coop_actor)->selection==5;
    }
}

bool GameBattle::th14_knife_slowdown(Enemy& enemy,float delta){
    auto& e=enemy.state;
    if(coop_seats==1){
        shot_th14_sakuya_slowdown(e,delta,spell_flags,spell_id);
        return true;
    }
    auto& slowdown=e.th14_coop_knife_slowdown[coop_actor];
    slowdown=std::max(0.f,slowdown+delta);
    return true;
}

float GameBattle::th14_effective_knife_slowdown(const EnemyState& e)const{
    if(coop_seats==1)return e.th14_knife_slowdown;
    if(coop_knife_owner<0)return 0.f;
    const float slowdown=e.th14_coop_knife_slowdown[u32(coop_knife_owner)];
    if(!(e.flags&0x800000))return std::min(slowdown,.9f);
    if((spell_flags&1)&&(spell_id==0x77||spell_id==0x71||(spell_id>=0x60&&spell_id<=0x63)))return 0.f;
    return std::min(slowdown/6.f,.33f);
}

bool GameBattle::update_coop(){
    ShotWorld::enemies=enemies.first;
    if(coop_bomb_invincibility.current>0)coop_bomb_invincibility.advance(-1);
    for(auto& timer:coop_marisa_b_cooldown)if(timer.current>0)timer.advance(-1);
    const bool talking=dialogue&&dialogue->active;
    bool skip_held=false;
    std::array<u32,3> pressed_buttons{};
    for(u32 seat=0;seat<coop_seats;++seat){
        auto& p=pilot(seat);
        const auto& input=coop_input.seats[seat];
        const u32 held=input.held,pressed=held&~coop_held[seat];
        pressed_buttons[seat]=pressed;
        coop_held[seat]=held;skip_held|=(held&skip)!=0;
        if(!pilot_live(seat)){
            auto& motion=p.motion.state;
            ghosts[seat].advance(motion.x,motion.y);
            motion.position={float(motion.x)/128,float(motion.y)/128,0};
            continue;
        }
        auto& lane=p.input;
        lane.movement.held=held;lane.movement.pressed=pressed;
        lane.movement.touch_mode=input.touch_mode;lane.movement.touch_x=input.touch_x;lane.movement.touch_y=input.touch_y;
        lane.movement.bomb=talking;lane.movement.enemies=enemies.count!=0;
        lane.movement.enemy_manager=enemies.owner_present();
        const auto* bomb=pilot_bomb(seat);
        lane.special_available=coop_bomb_available(seat);
        lane.special_active=bomb?bomb->state.active:0;
        lane.shooting_blocked=(completion.state.hud_flags&0x10)!=0;
        p.spell={spell_flags,spell_elapsed,spell_bonus};
        CoopActor actor(*this,seat);
        if(!seat&&before_player_update)before_player_update(*this);
        if(!p.update())return unavailable("TH14 co-op player update");
        spell_flags=p.spell.flags;spell_bonus=p.spell.bonus;
    }
    coop_skip=skip_held?std::min(coop_skip+1,60u):0;
    for(u32 seat=0;seat<coop_seats;++seat)if(pilot_live(seat)&&pilot_bomb(seat)){
        CoopActor actor(*this,seat);
        auto& p=pilot(seat);
        p.spell={spell_flags,spell_elapsed,spell_bonus};
        if(!pilot_bomb(seat)->update())return unavailable("TH14 co-op Bomb update");
        spell_flags=p.spell.flags;spell_bonus=p.spell.bonus;
    }
    for(u32 seat=0;seat<coop_seats;++seat){
        if(!pilot_live(seat))continue;
        auto& timer=pilot(seat).state.invincibility;
        if(timer.current<coop_bomb_invincibility.current)timer=coop_bomb_invincibility;
    }
    if(!coop_transfers(pressed_buttons))return false;
    coop_knife_owner=-1;
    for(u32 seat=0;seat<coop_seats;++seat)
        if(pilot_live(seat)&&pilot(seat).input.movement.character==2&&
           (coop_knife_owner<0||pilot_economy(seat).power>pilot_economy(u32(coop_knife_owner)).power))
            coop_knife_owner=i32(seat);
    return true;
}

bool GameBattle::start_coop_stage(){
    power_taps={};
    for(u32 seat=0;seat<coop_seats;++seat){
        CoopActor actor(*this,seat);
        if(seat){
            pilot_bomb(seat)->clear_coop();
            pilot(seat).shots.clear_coop_th14();
        }
        if(!pilot_live(seat)){
            if(!coop_revive(seat,120))return false;
        }else if(!pilot(seat).start_stage())return false;
    }
    return true;
}

bool GameBattle::coop_game_over(bool){
    const u32 seat=coop_actor;
    if(!pilot_live(seat))return true;
    const u32 seed=u32(animations.visual_rng.seed_value())^(0x9e3779b9u*(seat+1))^frame;
    ghosts[seat]={seed?seed:1};
    for(auto* node=enemies.first;node;node=node->next)
        node->value->state.th14_coop_knife_slowdown[seat]=0;
    life_taps[seat]=power_taps[seat]=0;
    auto& p=pilot(seat);
    if(auto* b=pilot_bomb(seat)){
        if(b->selection==5&&b->state.active&&!th14_bomb_freeze(false))return false;
        b->clear_coop();
    }
    p.motion.clear_focus();
    p.shots.clear_coop_th14();
    for(u32 other=0;other<coop_seats;++other)if(pilot_live(other))return true;
    game_over_requested=true;events.push_back({BattleEventKind::GameOver,i32(replaying)});
    return !on_game_over||on_game_over(replaying);
}

bool GameBattle::coop_revive(u32 seat,i32 invulnerability){
    CoopActor actor(*this,seat);
    auto& p=pilot(seat);auto& e=pilot_economy(seat);
    e.lives=std::max(0,e.lives);e.power=e.max_power/2;e.th14_bombs=e.th14_initial_bombs;
    if(!p.motion.reset_body()||!p.start_stage())return false;
    p.state.invincibility.set(invulnerability,&animations.rate);
    ghosts[seat]={};life_taps[seat]=power_taps[seat]=0;
    return bombs_changed(e.th14_bombs,e.th14_bomb_fragments);
}

bool GameBattle::coop_transfers(const std::array<u32,3>& pressed_buttons){
    namespace rules=eagler::cooperative;
    const bool allow_power=stage_active&&!transitioning&&!(dialogue&&dialogue->active)&&!(completion.state.hud_flags&0x10);
    if(!allow_power)power_taps={};
    for(u32 seat=0;seat<coop_seats;++seat){
        if(!pilot_live(seat)||pilot(seat).state.life_state!=1){life_taps[seat]=power_taps[seat]=0;continue;}
        auto& e=pilot_economy(seat);
        const u32 pressed=pressed_buttons[seat];
        const auto distance=[&](u32 other){
            return th14_dist_sq(pilot(seat).motion.state.position,pilot(other).motion.state.position);
        };
        const auto near=[&](u32 other){return seat!=other&&distance(other)<=400.f;};
        i32 life=-1;
        if(e.lives>0&&!(coop_held[seat]&1))for(u32 other=0;other<coop_seats;++other){
            if(!near(other)||(pilot_live(other)&&(pilot(other).state.life_state!=1||pilot_economy(other).lives>=8)))continue;
            if(life<0||(!pilot_live(other)&&pilot_live(u32(life)))||
               (pilot_live(other)==pilot_live(u32(life))&&distance(other)<distance(u32(life))))life=i32(other);
        }
        if(rules::AdvanceLifeTransfer(life_taps[seat],bool(pressed&8),life>=0)){
            const auto other=u32(life);
            if(!pilot_live(other)){if(!coop_revive(other,280))return false;}
            else{
                auto& target=pilot_economy(other);++target.lives;
                hud.display_coop_stock(other,target.lives,target.life_fragments,false);
            }
            --e.lives;
            hud.display_coop_stock(seat,e.lives,e.life_fragments,false);
            if(!sound(17,0,false))return false;
        }
        if(!allow_power)continue;
        const i32 step=e.power_step;
        if(pressed&8)power_taps[seat]=0;
        i32 power=-1;
        if(step>0&&e.power>=2*step)for(u32 other=0;other<coop_seats;++other){
            if(!near(other)||!pilot_live(other)||pilot(other).state.life_state!=1||
               pilot_economy(other).power>=pilot_economy(other).max_power)continue;
            if(power<0||distance(other)<distance(u32(power)))power=i32(other);
        }
        if(rules::AdvanceLifeTransfer(power_taps[seat],bool(pressed&1),power>=0)){
            const u32 other=u32(power);auto& target=pilot_economy(other);
            const i32 amount=std::min(step,target.max_power-target.power);
            e.power-=amount;target.power+=amount;
            if(!pilot(seat).rebuild_th14_options()||!pilot(other).rebuild_th14_options()||!sound(13,0,false))return false;
        }
    }
    return true;
}

void GameBattle::assign_coop_items(){
    items.th14_pilot_count=coop_seats;
    for(u32 seat=0;seat<coop_seats;++seat){
        const auto& p=pilot(seat);auto& target=items.th14_pilots[seat];
        p.copy_item_state(target);target.character=p.input.movement.character;
        target.force_attract=dialogue&&dialogue->active;
        auto* b=pilot_bomb(seat);
        target.bomb_attract=b&&b->state.active==1&&b->state.elapsed.current<60;
        items.th14_pilot_live[seat]=pilot_live(seat);
    }
}

bool GameBattle::coop_collect(ItemState& item,bool& convert){
    const u32 collector=items.th14_collector;
    if(coop_challenge&&item.type>=4&&item.type<=7)return true;
    const i32 bomb_pieces=th14_coop_bomb_pieces(coop_seats);
    if(item.type==4||item.type==5||item.type==7){
        for(u32 seat=0;seat<coop_seats;++seat){
            CoopActor actor(*this,seat);
            ItemRewards rewards(pilot_economy(seat),*this,economy);
            if(item.type==4&&!rewards.th14_life_fragment(th14_coop_life_pieces,bomb_pieces))return false;
            if(item.type==5&&!rewards.th14_life())return false;
            if(item.type==7&&!rewards.th14_bomb())return false;
        }
        return true;
    }
    auto& e=pilot_economy(collector);
    const auto& p=items.th14_pilots[collector];
    CoopActor actor(*this,collector);ItemRewards rewards(e,*this,economy);
    if(item.type==6)return rewards.th14_bomb_fragment(bomb_pieces);
    if(item.th14_combo_owner<0)return rewards.th14_collect_with_combo(item,convert,&p,nullptr);
    const auto owner=u32(item.th14_combo_owner);
    return rewards.th14_collect_with_combo(item,convert,&p,&items.th14_pilots[owner],&pilot_economy(owner).th14_combo);
}

bool GameBattle::item_th14_combo_settle_seat(u32 seat){
    if(coop_seats==1)return item_th14_combo_settle();
    CoopActor actor(*this,seat);
    auto& stock=pilot_economy(seat);
    if(const auto count=stock.th14_combo.count;count>=20)++stock.th14_practice_collections[std::min(count/10-2,4)];
    ItemRewards rewards(stock,*this,economy);
    return rewards.th14_combo_settle(dialogue&&dialogue->active);
}

Vec3 GameBattle::th14_aim(Vec3 origin,Vec3 solo)const{
    if(coop_seats==1)return solo;
    Vec3 target{0,400,0};float best=0;bool have=false;
    for(u32 seat=0;seat<coop_seats;++seat)if(pilot_live(seat)&&pilot(seat).state.life_state!=2){
        const auto position=pilot(seat).motion.state.position;
        const float distance=th14_dist_sq(origin,position);
        if(!have||distance<best){target=position;best=distance;have=true;}
    }
    return target;
}
Vec3 GameBattle::th14_enemy_aim(Vec3 origin,void* game){
    return static_cast<GameBattle*>(game)->th14_aim(origin,{});
}

i32 GameBattle::th14_collision_all(Vec3 origin,const std::function<PlayerCollisionResult(const PlayerFrame&)>& test,bool probe){
    bool hit=false,grazed=false;float graze_distance=0;
    for(u32 seat=0;seat<coop_seats;++seat){
        if(!pilot_live(seat)||pilot(seat).state.life_state==2)continue;
        const auto result=test(pilot(seat));
        if(result.kind==CollisionKind::Hit)hit=true;
        if(result.kind==CollisionKind::Graze){
            const float d=th14_dist_sq(origin,pilot(seat).motion.state.position);
            if(!grazed||d<graze_distance){grazed=true;graze_distance=d;coop_grazer=seat;}
        }
        if(!probe&&result.trigger_hit){
            CoopActor actor(*this,seat);
            if(apply_collision(result)<0)return -2;
            LaserWorld::player=pilot(seat).motion.state.position;
        }
    }
    return i32(hit?CollisionKind::Hit:grazed?CollisionKind::Graze:CollisionKind::None);
}

bool GameBattle::coop_damage(const EnemyState& e,Vec3 position,const Vec2* size,float radius,float angle,i32& out,bool probe,bool* contact){
    const bool boss=(e.flags&0x40800000u)!=0;
    const auto ratios=th14_coop_damage_ratios(coop_seats,coop_bomb_invincibility.current>0,coop_bomb_sakuya_b,
        boss,spell_flags&1,resources.stage_number,spell_id,e.boss_slot);
    i32 total=0;u32 live=0;if(contact)*contact=false;
    for(u32 seat=0;seat<coop_seats;++seat)if(pilot_live(seat)){
        ++live;CoopActor actor(*this,seat);
        auto& p=pilot(seat);const auto& timer=p.state.state_timer;
        i32 damage=0;bool bomb_hit=false;
        if(!p.shots.damage_th14(position,size,radius,angle,timer.previous!=timer.current,pilot_economy(seat),damage,
            u32(e.instance_id),u32(e.instance_id),e.script_owner,probe,
            contact?&bomb_hit:nullptr,ratios.bomb,ratios.shot))return false;
        if(p.state.life_state==0||p.state.life_state==2)damage/=5;
        total=wrapping_add(total,damage);if(contact)*contact|=bomb_hit&&damage>0;
    }
    u32 remainder=e.th14_coop_damage_remainder;
    out=boss?th14_boss_damage(total,live,remainder):total;
    if(!probe)e.th14_coop_damage_remainder=remainder;
    return true;
}

bool GameBattle::reset_coop_player_scales(){
    for(u32 seat=0;seat<coop_seats;++seat){
        auto& p=pilot(seat);
        if(!(p.th14_scale.flags&0x10))continue;
        CoopActor actor(*this,seat);
        if(pilot_live(seat)){if(!p.set_th14_scale(1.f))return false;}
        else p.reset_th14_scale();
    }
    return true;
}

bool GameBattle::draw_coop_players(AnmRenderer& renderer){
    for(u32 seat=0;seat<coop_seats;++seat){
        auto& p=pilot(seat);
        if(pilot_live(seat)){if(!p.draw(renderer,coop_observer||seat==coop_viewer))return false;}
        else{
            const auto old=p.state.life_state;
            p.state.life_state=1;
            const bool drawn=p.draw(renderer,false);
            p.state.life_state=old;
            if(!drawn)return false;
        }
    }
    return true;
}
bool GameBattle::player_effect(Vec3 p,i32 script){
    auto* vm=animations.create(resources.core.effect,script,8,u32(-1),false,false,&p);
    if(vm&&coop_seats>1&&(script==27||script==28))vm->reserved_3a0=coop_miss_draw_tag+coop_actor;
    return vm!=nullptr;
}
u8 GameBattle::coop_sprite_opacity(const void* context,const AnmVm& vm){
    const auto& battle=*static_cast<const GameBattle*>(context);
    auto* root=&vm;
    for(;;root=root->parent){
        if(battle.coop_challenge&&root->reserved_3a0==Hud::coop_life_stock_tag)return 0;
        if(!root->parent)break;
    }
    if(battle.coop_observer)return 255;
    const auto local=battle.pilot(battle.coop_viewer).motion.state.position;
    const auto opacity=[&](u32 seat)->u8 {
        if(seat==battle.coop_viewer)return battle.pilot_live(seat)?255:112;
        if(!battle.pilot_live(seat))return 88;
        return th14_dist_sq(local,battle.pilot(seat).motion.state.position)<2304.f?72:160;
    };
    // The tags are set on the ANM roots, including effects retiring after Shift release.
    const auto tag=root->reserved_3a0;
    if(tag>=coop_miss_draw_tag&&tag<coop_miss_draw_tag+battle.coop_seats)
        return tag-coop_miss_draw_tag==battle.coop_viewer?255:64;
    if(tag>=24&&tag<24+battle.coop_seats)return tag==24+battle.coop_viewer?255:0;
    if(tag>=20&&tag<20+battle.coop_seats)return opacity(tag-20);
    for(u32 seat=0;seat<battle.coop_seats;++seat)
        if(root==&battle.pilot(seat).motion.body)return opacity(seat);
    return 255;
}

bool GameBattle::queue_coop_hud(){
    AsciiStyle style;style.font=4;style.shadow=true;
    const auto* first=hud.coop_icon(0,0);
    const u32 alpha=first?first->color&0xff000000u:0;
    const auto text=[&](float x,float y,const char* value){return ascii.add(value,{x,y,0},style);};
    char number[64];
    AsciiStyle transfer;transfer.font=1;transfer.shadow=true;transfer.horizontal=0;
    for(u32 seat=0;seat<coop_seats;++seat){
        const auto lives=eagler::cooperative::TapCount(life_taps[seat]);
        const auto power=eagler::cooperative::TapCount(power_taps[seat]);
        if(lives<3&&power<3)continue;
        const auto pos=pilot(seat).motion.state.position;
        transfer.color=lives>=3?0xffffa0c0u:0xffffa070u;
        std::snprintf(number,sizeof(number),"%c %u/%u",lives>=3?'L':'P',
            lives>=3?lives:power,eagler::cooperative::TransferTaps);
        if(!ascii.add(number,{pos.x+224.f,pos.y-8.f,0},transfer))return false;
    }
    style.horizontal=2;style.color=alpha|0x808080;
    th14_score_number(number,u32(hud.score.high_value()),hud.score.high_continues);
    if(!text(620,44,number))return false;
    style.color=alpha|0xffffff;
    th14_score_number(number,u32(hud.score.displayed_value()),hud.score.continues_value());
    if(!text(620,66,number))return false;
    for(u32 seat=0;seat<coop_seats;++seat){
        const auto& e=pilot_economy(seat);
        const auto* icon=hud.coop_icon(seat,0);
        const u32 opacity=icon?icon->color&0xff000000u:alpha;
        const float life=hud.life_row(seat),bomb=hud.bomb_row(seat),power=hud.power_row(seat);
        style.horizontal=1;style.font=1;style.scale={1,1};
        style.color=opacity|0xffffffu;
        if(coop_challenge&&!text(438,life+3,"MISS"))return false;
        style.color=opacity|(seat==coop_viewer?0xffff80u:0xffffffu);
        std::snprintf(number,sizeof(number),"%uP",seat+1);
        if(!text(491,life+3,number))return false;
        style.font=4;style.scale={.6f,.6f};style.color=opacity|0xffffffu;
        if(coop_challenge){
            style.horizontal=2;style.scale={1,1};
            th14_grouped_number(number,e.th14_practice_misses);
            if(!text(620,life+8,number))return false;
            style.horizontal=1;style.scale={.6f,.6f};
        }else if(!pilot_live(seat)){
            style.font=2;
            if(!text(580,life+22,"GHOST"))return false;
            style.font=4;
        }else{
            std::snprintf(number,sizeof(number),"%d/%d",e.life_fragments,th14_coop_life_pieces);
            if(!text(580,life+22,number))return false;
        }
        std::snprintf(number,sizeof(number),"%d/%d",e.th14_bomb_fragments,th14_coop_bomb_pieces(coop_seats));
        if(!text(580,bomb+22,number))return false;
        style.scale={1,1};style.color=opacity|0xff8030;
        std::snprintf(number,sizeof(number),"%d.",e.power/e.power_step);
        if(!text(540,power-2,number))return false;
        style.scale={.6f,.6f};
        std::snprintf(number,sizeof(number),"%02d",e.power%e.power_step*100/e.power_step);
        if(!text(560,power+5,number))return false;
        style.scale={1,1};
        std::snprintf(number,sizeof(number),"/%d.",e.max_power/e.power_step);
        if(!text(574,power-2,number))return false;
        style.scale={.6f,.6f};
        if(!text(606,power+5,"00"))return false;
    }
    style.color=alpha|0x40c0ff;style.scale={1,1};style.horizontal=2;
    const auto points=economy.point_value/100;
    th14_grouped_number(number,u32(points-points%10));
    if(!text(620,hud.point_row()-2,number))return false;
    style.color=alpha|0xffffff;
    th14_grouped_number(number,u32(economy.graze));
    return text(620,hud.graze_row()-2,number);
}
}
#endif
