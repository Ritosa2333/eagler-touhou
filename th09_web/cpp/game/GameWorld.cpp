#include "GameWorld.hpp"
#include "MusicCatalog.hpp"
#include <algorithm>
namespace th09 {
struct GameWorld::Services:BattlePresentation,HudPresentation,MatchRuleActions,StageSelectionActions,ScreenEffectOutput {
    GameWorld& g;
    struct Backdrop:BackgroundServices {
        Services& root;i32 side;Backdrop(Services& r,i32 s):root(r),side(s){}
        void start_animation(AnmVm& vm,bool boss,i32 script)override{auto& g=root.g;if(!g.resources.start(boss?character_animation(1-side):AnimationFile::background,vm,script,!boss))g.fail("Background animation");}
        void advance_animation(AnmVm& vm)override{root.g.resources.advance(vm);}
    } backdrops[2]{{*this,0},{*this,1}};
    struct Messages:DialogueServices {
        Services& root;explicit Messages(Services& r):root(r){}
        AnimationFile file(DialogueResource r){switch(r){case DialogueResource::ascii:return AnimationFile::text;case DialogueResource::left_portrait:return AnimationFile::left_character;case DialogueResource::right_portrait:return AnimationFile::right_character;default:return AnimationFile::extra_portraits;}}
        void start_animation(AnmVm& vm,DialogueResource r,i32 script)override{if(!root.g.resources.start(file(r),vm,script,false))root.g.fail("Dialogue animation");}
        void set_sprite(AnmVm& vm,DialogueResource r,i32 id)override{if(!root.g.resources.sprite(file(r),vm,id))root.g.fail("Dialogue sprite");}
        void advance_animation(AnmVm& vm)override{root.g.resources.advance(vm);}
        void draw_animation(AnmVm& vm,bool right)override{root.g.output.animation(vm,right?BattleSprite::mirrored:BattleSprite::unrotated);}
        void text(AnmVm& vm,u32 color,u32 shadow,const std::string& value)override{root.g.output.text(vm,value.c_str(),color,shadow);}
        void music(i32 track)override{if(const auto* music=th09::music_track(track))root.g.music_track=music->title;root.g.output.music(track);}
        void fade_music()override{root.g.output.fade_music();}
        void show_results()override{root.sync_scene();root.g.scene->show_results();root.publish_scene();}
        void transition(DialogueTransition value)override{auto& g=root.g;g.transition_pending=true;g.transition=value==DialogueTransition::next_stage?SceneTransition::next_stage:value==DialogueTransition::game_over?SceneTransition::game_over:SceneTransition::match_complete;}
        void white_transition()override{root.g.screen_effects.create({5,300,0xffffff,0,0,35,2});}
        void enter_player(i32 side)override{auto& p=*root.g.battle->fields[side].player;p.control.player_state=1;p.control.protection.reset();}
        void show_huds()override{for(auto& hud:root.g.huds)if(hud)hud->ready();}
        void background_setting(i32 side,i32 value)override{root.background_setting(side,value);}
        void draw_panel(const AttackColorVertex* p,u32 n)override{root.g.output.colored(nullptr,p,n,BattleGeometry::strip,false);}
    } messages{*this};
    struct Scene:MatchSceneServices {
        Services& root;explicit Scene(Services& r):root(r){}
        void start_animation(AnmVm& vm,OverlayResource f,i32 script)override{if(!root.g.resources.start(f==OverlayResource::front?AnimationFile::front:AnimationFile::ascii,vm,script))root.g.fail("Scene animation");}
        void set_sprite(AnmVm& vm,i32 sprite)override{if(!root.g.resources.sprite(AnimationFile::front,vm,sprite))root.g.fail("Scene sprite");}
        void advance_animation(AnmVm& vm)override{root.g.resources.advance(vm);}
        void draw_animation(AnmVm& vm)override{root.g.output.animation(vm,BattleSprite::unrotated);}
        void begin_view(i32 side)override{root.g.output.begin_field(side);}
        void outline(const AttackColorVertex* p,u32 count)override{for(u32 n=0;n<count;++n)root.g.output.colored(nullptr,p+n,2,BattleGeometry::lines,false);}
        void number(const Vec3& p,i32 width,i32 value)override{root.g.output.number(p,width,value);}
        void clock(const Vec3& p,i32 stage,i32 min,i32 sec)override{root.g.output.clock(p,stage,min,sec);}
        i32 dialogue_state()const override{return root.g.dialogue->id;}
        void update_dialogue()override{auto& g=root.g;g.dialogue->update(g.combined_input);if(g.dialogue->invalid)g.fail("Dialogue script");}
        void draw_dialogue()override{root.g.dialogue->draw();}
        void begin_dialogue(i32 script,i32 flip)override{if(!root.g.dialogue->begin(script,flip))root.g.fail("Dialogue entry");}
        void victory_dialogue(i32 side)override{auto& g=root.g;if(!g.dialogue->begin_victory(side,g.configuration.selection.characters[1-side],g.world.random))g.fail("Victory dialogue");}
        void play_sound(i32 id,i32 pan)override{root.g.output.sound(id,pan);}
        void reset_attack_timer(i32 side)override{root.g.battle->fields[side].attacks->time.reset();}
        void clear_round_hazards(i32 side)override{root.g.battle->clear_hazards(side);}
        void flush_combo(i32 side)override{root.g.battle->flush_combo(side);}
        void background_transition(i32 side,i32 state,i32 frames)override{root.background_transition(side,state,frames);}
        void fade(i32 type,i32 duration,u32 color,i32 side)override{root.g.screen_effects.create({type,duration,color,0,0,35,side});}
        void fade_hud(i32 side)override{root.g.huds[side]->close();}
        void restart_round()override{root.g.reset_round();}
        void transition(SceneTransition value)override{root.g.transition=value;root.g.transition_pending=true;}
        void record_defeat(i32 character)override{root.g.output.defeated(character);}
    } scene{*this};
    explicit Services(GameWorld& game):g(game){}
    void sync_scene(){
        scene.game_flags=g.battle?g.battle->state.flags:g.configuration.game_flags;scene.route=g.configuration.selection.route;scene.opponent_character=g.configuration.selection.characters[1];scene.starting_extra_lives=g.configuration.starting_extra_lives;scene.music_track=g.music_track;
        for(i32 s=0;s<2;++s){scene.geometry[s]=g.battle?g.battle->state.geometry[s]:PlayfieldGeometry{u32(16+320*s),16,{-144,0},288};if(!g.battle||!g.battle->fields[s].player)continue;auto& f=g.battle->fields[s];auto& p=*f.player;auto& summary=scene.players[s];summary.health=p.motion.health;summary.best_combo=p.combo_state.best_hits;summary.spells=f.spells;summary.bosses=f.bosses;summary.counters=f.counters;summary.position=p.motion.position;}
        if(g.battle)scene.cpu_level=g.battle->fields[1].cpu_level;
    }
    void publish_scene(){if(!g.battle)return;auto& b=*g.battle;b.state.flags=scene.game_flags;b.state.scene_phase=g.scene->phase;b.state.ending_frames=g.scene->ending_frames;b.state.rewards_blocked=g.scene->rewards_blocked;b.state.dialogue=g.dialogue->id;b.fields[1].cpu_level=scene.cpu_level;}
    HudFrame hud_frame(i32 side){const auto& f=g.battle->fields[side];const auto& p=*f.player;HudFrame frame;frame.timing=g.timing;frame.geometry=g.battle->state.geometry[side];frame.player=p.motion.position;frame.combo=p.combo_state;frame.score=g.rules.scores[side];frame.health=p.motion.health;frame.spell_level=p.attack_levels[0];frame.boss_level=p.attack_levels[1];frame.wins=scene.wins[side];frame.charge=p.control.charge;frame.available=p.control.available;frame.versus=g.rules.mode==GameMode::versus;return frame;}
    void sound(i32 id,i32 pan)override{g.output.sound(id,pan);}
    void positioned_sound(i32 side,i32 id,float x)override{g.output.positioned_sound(side,id,x);}
    void begin_field(i32 side)override{g.output.begin_field(side);}
    void animation(AnmVm& vm,BattleSprite mode)override{g.output.animation(vm,mode);}
    void colored(const AnmVm* vm,const AttackColorVertex* p,u32 n,BattleGeometry mode,bool additive)override{g.output.colored(vm,p,n,mode,additive);}
    void textured(const AnmVm& vm,const AttackTextureVertex* p,u32 n,BattleGeometry mode)override{g.output.textured(vm,p,n,mode);}
    void text(AnmVm& vm,const char* s,u32 color,u32 shadow)override{g.output.text(vm,s,color,shadow);}
    void score_popup(i32 side,const Vec3& p,i32 value,u32 color)override{g.output.score_popup(side,p,value,color);}
    void notice(i32 side,BattleNotice kind,i32 value)override{if(!g.huds[side])return;auto& h=*g.huds[side];switch(kind){case BattleNotice::begin_charge:h.begin_charge();break;case BattleNotice::end_charge:h.end_charge();break;case BattleNotice::charge_level:h.charge_level(value);break;case BattleNotice::critical_health:h.health_notice(0);break;case BattleNotice::begin_survival:h.begin_survival(value);break;case BattleNotice::survival_time:h.survival_time(value);break;case BattleNotice::survival_expired:h.survival_expired();break;case BattleNotice::reward:h.health_notice(1);break;}}
    void damage_flash(i32 side,i32 duration,u32 color)override{g.scene->flash_frames[side]=duration;g.scene->flash_colors[side]=color;}
    void end_round(i32 winner)override{sync_scene();g.scene->end_round(winner);publish_scene();}
    void finish_match()override{g.battle->state.hide_players=true;}
    void attack_position(i32 side,const Vec3& p)override{if(g.scene){sync_scene();g.scene->boss_position(side,p);}}
    void boss_indicator(i32,i32 slot,i32 state)override{if(slot>=0&&slot<4)g.markers[slot].state=state;}
    void boss_indicator_position(i32,i32 slot,const Vec3& p)override{if(slot>=0&&slot<4)g.markers[slot].position=p;}
    void background_setting(i32 side,i32 value)override{if(g.backgrounds[side])g.backgrounds[side]->requested_label=value;}
    void background_transition(i32 side,i32 state,i32 frames)override{if(g.backgrounds[side]){g.backgrounds[side]->transition_state=state;g.backgrounds[side]->transition_frames=frames;}}
    void reset_background(i32 side)override{g.backgrounds[side]->end_boss();}
    void boss_background(i32 side)override{if(g.backgrounds[side])g.backgrounds[side]->begin_boss(character_resources(g.configuration.selection.characters[1-side])->boss_background);}
    void portrait(i32 side,u32 layer,i32 script)override{g.huds[side]->portrait(layer,script,character_animation(side));}
    void begin_hud(i32 side)override{g.output.begin_field(side);}
    void hud_animation(AnmVm& vm,bool rotated)override{g.output.animation(vm,rotated?BattleSprite::rotated:BattleSprite::unrotated);}
    void hud_triangle(const AttackColorVertex* p)override{g.output.colored(nullptr,p,3,BattleGeometry::strip,false);}
    void reward_enemy(i32 side,i32 kind)override{g.battle->reward_enemy(side,kind);}
    void play_sound(i32 id,i32 pan)override{sound(id,pan);}
    void reward_notification(i32 side)override{notice(side,BattleNotice::reward,0);}
    void encounter(i32 character)override{g.output.encountered(character);}
    void shake(i32 side,float x,float y)override{g.output.shake(side,x,y);}
    void rectangle(float l,float t,float r,float b,u32 color,bool full)override{g.output.rectangle(l,t,r,b,color,full);}
};
MatchSceneServices& GameWorld::scene_services(){return services->scene;}
GameWorld::GameWorld(EclWorldState& w,GameResources& r,AnmExecutor& a,WorldPresentation& p):services(std::make_unique<Services>(*this)),world(w),resources(r),animations(a),output(p),rules(w,*services),screen_effects(w.random,*services){}
GameWorld::~GameWorld()=default;
bool GameWorld::fail(const char* message){if(error.empty())error=message;return false;}
bool GameWorld::initialize(const WorldConfiguration& config,bool choose_stage,const std::array<ScoreCounter,2>* scores){
    if(ready)return fail("World already initialized");configuration=config;auto& selection=configuration.selection;
    if(choose_stage&&!StageSelection::select(selection,world.random,*services))return fail("Stage selection");
    if(!resources.load_common()||!resources.load_match(selection.background,selection.characters,configuration.alternate,selection.stage==8&&selection.characters[0]==6))return fail("World resources");
    std::vector<u8> data;for(i32 s=0;s<2;++s){const auto* profile=character_resources(selection.characters[s]);if(!profile)return fail("Character selection");const char* file=selection.mode==GameMode::versus?profile->versus_messages:profile->story_messages;if(!resources.read(file,data)||!messages[s].load(data.data(),data.size()))return fail("Message resource");services->messages.characters[s]={profile->attack_portrait,profile->dialogue_portrait,profile->dialogue_sprite};}
    rules.initialize(selection.difficulty,selection.mode,selection.stage);rules.progress.round=selection.round;rules.scores[0].lives=selection.lives;if(scores)rules.scores=*scores;
    services->messages.mode=selection.mode;services->messages.stage_music=selection.route.music;services->scene.wins_required=selection.mode==GameMode::versus?2:1;
    dialogue=std::make_unique<Dialogue>(services->messages);dialogue->set_resources(messages[0],&messages[1]);scene=std::make_unique<MatchScene>(rules,services->scene);services->sync_scene();scene->initialize();
    battle=std::make_unique<GameBattle>(world,rules,resources,animations,*services);battle->configure(selection.characters[0],selection.characters[1]);battle->state.flags=configuration.game_flags;
    const auto* background=background_resources(selection.background);if(!background||!resources.read(background->model,data))return fail("Stage resource");
    for(i32 side=0;side<2;++side){backgrounds[side]=std::make_unique<Background>(services->backdrops[side]);if(!backgrounds[side]->load(data.data(),data.size(),selection.background))return fail("Stage model");
        if(!battle->initialize_field(side,configuration.controllers[side],configuration.health[side]))return fail(battle->error.c_str());battle->fields[side].cpu_level=selection.cpu_levels[side];
        battle->state.automatic_focus[side]=configuration.controllers[side]?false:configuration.automatic_focus[side];huds[side]=std::make_unique<HeadsUpDisplay>(resources,*services,side);huds[side]->initialize(selection.mode==GameMode::versus);
    }
    if(!battle->initialize_shared())return fail(battle->error.c_str());for(i32 s=0;s<2;++s)std::copy_n(rules.attack_levels[s],2,battle->fields[s].player->attack_levels);
    sync();ready=error.empty()&&!animations.invalid;return ready;
}
void GameWorld::sync(){
    animations.timing=timing;services->messages.timing=timing;for(auto& b:services->backdrops)b.timing=timing;
    if(battle){battle->state.timing=timing;battle->state.dialogue=dialogue->id;battle->sync_players();}services->sync_scene();services->publish_scene();
}
void GameWorld::reset_round(){
    markers={};battle->state.extra_damage=0;battle->state.script_extra_time=0;battle->state.hide_players=false;scene->reset_round();battle->attack_queue->clear();
    for(i32 s=0;s<2;++s){backgrounds[s]->end_boss();battle->reset_field(s,configuration.health[s]);huds[s]->open();}
    battle->cross_effects->clear();rules.restart_round(rules.progress.round);sync();
}
bool GameWorld::update(u16 left,u16 right,u16 menu){
    if(!ready||!error.empty())return false;if(paused||transition_pending)return true;
    combined_input.advance(menu);battle->fields[0].player->input.advance(left);battle->fields[1].player->input.advance(right);
    for(i32 s=0;s<2;++s)battle->fields[s].player->input.update_auto_focus(battle->state.automatic_focus[s]);return simulate();
}
void GameWorld::copy_inputs(GameInput (&inputs)[3])const{for(i32 s=0;s<2;++s)inputs[s]=battle->fields[s].player->input;inputs[2]=combined_input;}
bool GameWorld::update_prepared(const GameInput (&inputs)[3]){if(!ready||!error.empty())return false;if(paused||transition_pending)return true;for(i32 s=0;s<2;++s)battle->fields[s].player->input=inputs[s];combined_input=inputs[2];return simulate();}
void GameWorld::continue_game(){
    transition_pending=false;paused=false;rules.scores[0].displayed=rules.scores[0].points=rules.scores[0].increment=0;rules.scores[1].displayed=rules.scores[1].points=rules.scores[1].increment=0;
    battle->state.flags|=0x2000;reset_round();services->scene.wins[0]=services->scene.wins[1]=0;rules.scores[0].lives=float(configuration.starting_extra_lives)+2;configuration.selection.continued=1;battle->state.flags|=0x4000;sync();
}
bool GameWorld::simulate(){
    sync();
    for(i32 s=0;s<2;++s)std::copy_n(battle->fields[s].player->attack_levels,2,rules.attack_levels[s]);
    rules.update(dialogue->blocks_gameplay(),battle->state.flags,battle->fields[0].script.flags);
    ScreenEffectContext effects;effects.timing=timing;effects.game_flags=battle->state.flags;effects.active_frames=rules.progress.active_frames;effects.game_over=battle->state.hide_players;
    for(i32 s=0;s<2;++s)effects.field_flags[s]=battle->fields[s].script.flags;screen_effects.update(effects);
    for(i32 s=0;s<2;++s)std::copy_n(rules.attack_levels[s],2,battle->fields[s].player->attack_levels);
    battle->sync_players();
    for(i32 s=0;s<2;++s)backgrounds[s]->update(battle->state.flags,battle->fields[s].script.flags);
    for(i32 s=0;s<2;++s)if(!battle->update_enemies(s))return fail(battle->error.c_str());
    for(i32 s=0;s<2;++s)if(!battle->update_bullets(s))return fail(battle->error.c_str());
    if(!battle->update_attacks())return fail(battle->error.c_str());for(i32 s=0;s<2;++s)battle->update_player(s);for(i32 s=0;s<2;++s)battle->update_controller(s);for(i32 s=0;s<3;++s)battle->update_effects(s);
    services->sync_scene();scene->update();services->publish_scene();
    for(i32 s=0;s<2;++s)huds[s]->update(services->hud_frame(s));
    if(animations.invalid)return fail("World animation");for(auto& b:backgrounds)if(b->invalid)return fail("World background");return error.empty();
}
void GameWorld::draw(){
    if(!ready)return;for(i32 s=0;s<2;++s)output.background(*backgrounds[s],*backgrounds[0],s,false);for(i32 s=0;s<2;++s)output.background(*backgrounds[s],*backgrounds[0],s,true);
    for(i32 s=0;s<2;++s)battle->draw_enemies(s,false);for(i32 s=0;s<2;++s)battle->draw_player(s,false);for(i32 s=0;s<2;++s)battle->draw_player(s,true);battle->draw_attacks(false);
    for(i32 s=0;s<2;++s)battle->draw_enemies(s,true);for(i32 s=0;s<2;++s)battle->draw_effects(s,0);for(i32 s=0;s<2;++s)battle->draw_bullets(s);output.draw_score_popups();for(i32 s=0;s<2;++s)battle->draw_controller(s);
    for(i32 s=0;s<2;++s)huds[s]->draw(services->hud_frame(s));services->sync_scene();scene->draw();battle->draw_attacks(true);battle->draw_effects(2,0);output.draw_overlay();screen_effects.draw(35);
}
}
