#include "GameSessionResources.hpp"
#ifdef TH_ENABLE_THPRAC
#include "AudioGame.hpp"
#include "PracticeConfig.hpp"
#endif
#include <initializer_list>
namespace th10 {
// 0x417800. The retained ECL program and statistics survive a stage restart.
void reset_enemy_stage(EnemyManager& manager,const float* rate) noexcept {if(!(manager.lifetime_flags&1)){manager.lifetime.rate=rate;manager.lifetime_flags|=1;}manager.lifetime.initialize(-1);manager.head=manager.tail=nullptr;}
// 0x418b80 / 0x418a90. Continue count is stored in the last score digit.
void set_item_value(GameEconomy& game,i32 points,const float* rate) noexcept {game.item_value=points/10;if(!(game.faith_timer_flags&1)){game.faith_timer.rate=rate;game.faith_timer_flags|=1;}game.faith_timer.initialize(-1);}
void increment_continue_count(GameEconomy& game) noexcept {game.score_units=wrapping_add(game.score_units,1);if(game.score_units>=10)game.score_units=9;}
static void set_enabled(GameSystemCallbacks& object,bool enabled){if(object.update)object.update->flags=enabled?object.update->flags|2:object.update->flags&~2u;if(object.draw)object.draw->flags=enabled?object.draw->flags|2:object.draw->flags&~2u;}
// 0x4180e0. The worker starts only after the session is globally visible.
GameSession* GameSessionResources::create(i32 replay_mode,GameSessionResourceEnvironment& env){
    auto* session=env.allocate_session();if(session)std::memset(session,0,sizeof(*session));env.evict_graphics_resources();if(!session)return nullptr;
    session->replay_mode=replay_mode;*env.current=session;session->session_flags|=4;env.begin_loading(env.loader_callback);return session;
}
i32 GameSessionResources::fail(){auto& env=environment;session.session_flags|=8;env.finish_loading(false);*env.loader_stop_requested=0;*env.loader_running=1;set_enabled(*reinterpret_cast<GameSystemCallbacks*>(&session),true);return -1;}
// 0x417870. Retried games, new stages and replays use different retention paths.
i32 GameSessionResources::load(){Progress progress;i32 result;do{result=load_step(progress);}while(result>0);return result;}
i32 GameSessionResources::load_step(Progress& progress){auto& env=environment;auto& game=*env.game;
    if(progress.phase==0){session.session_flags|=4;if(*env.drawing_resource>=0||*env.updating_resource>=0){if(*env.engine_flags&0x80)return fail();env.sleep(1);return 1;}
    *env.rate=1;game.stage_frames=game.section_frames=0;
    if(*env.new_game){
#ifdef TH_ENABLE_THPRAC
        // thprac_th10.cpp:2266 (0x41798C) zeroes TrackerInfo on entering a game.
        if(env.practice){env.practice->tracker_misses=0;env.practice->tracker_bombs=0;}
#endif
        if(game.stage==7)game.difficulty=4;
        auto& character=(*env.scores)->characters[game.character*3+game.shot_type];const auto& high=character.high_scores[game.difficulty][0];game.high_score=high.score;game.high_score_units=static_cast<std::int8_t>(high.score_units);
        if(!(game.flags&8))game.score_units=0;game.score=0;set_item_value(game,50000,env.rate);
        game.lives=!(game.flags&0x10)?2:!*env.practice_lives?9:wrapping_add(*env.practice_lives,-1);game.power=game.stage==1?0:80;game.flags&=~4u;
        if(!session.replay_mode){i32 count;std::memcpy(&count,character.statistics,4);if(count<99999){count=wrapping_add(count,1);std::memcpy(character.statistics,&count,4);}}
        game.extend_index=0;game.rank=game.flags&8?-512:0;
    }else if(game.score>game.high_score)game.high_score=game.score;
    game.enemy_activity=9;
    session.update_entry=(*env.chain)->add(env.update_callback,&session,10,false,false,*env.callbacks);session.draw_entry=(*env.chain)->add(env.draw_callback,&session,4,true,false,*env.callbacks);
    std::memcpy(session.configuration,env.configuration,sizeof(session.configuration));session.stage_identifier=(*env.current_stage)->resources[0];
        ++progress.phase;return 1;
    }
    const auto create=[&](SessionObject kind){return env.create_object(kind,session.replay_mode);};
    bool success=true;
    switch(progress.phase){
    case 1:if(!(game.flags&2))success=create(SessionObject::Replay);else{env.prepare_replay();env.reload_gui();}break;
    case 2:success=create(SessionObject::Stage);break;
    case 3:if(!(game.flags&2))success=create(SessionObject::Gui);break;
    case 4:if(!(game.flags&2))success=create(SessionObject::Player);break;
    case 5:if(!(game.flags&2))success=create(SessionObject::Bullets);break;
    case 6:if(!(game.flags&2))success=create(SessionObject::Items);break;
    case 7:if(!(game.flags&2))success=create(SessionObject::Lasers);break;
    case 8:if(!(game.flags&2))success=create(SessionObject::Results);break;
    case 9:if(!(game.flags&2))success=create(SessionObject::TextOverlay);break;
    case 10:if(!(game.flags&2))success=create(SessionObject::ScorePopups);break;
    case 11:if(!(game.flags&9))success=create(SessionObject::Enemies);else reset_enemy_stage(*reinterpret_cast<EnemyManager*>(env.object(SessionObject::Enemies)),env.rate);break;
    case 12:success=create(SessionObject::Effects);break;
    case 13:success=create(SessionObject::Bomb);break;
    case 14:success=create(SessionObject::Spell);break;
    case 15:if(!(game.flags&0x20)){env.stop_music();env.load_music(0,reinterpret_cast<const char*>((*env.current_stage)->resources[4]));env.load_music(1,reinterpret_cast<const char*>((*env.current_stage)->resources[5]));}break;
    case 16:*env.rendered_time=0;*env.recorded_time=0;session.reset_timer(env.rate);break;
    case 17:if(*env.pending_upload){env.sleep(16);return 1;}break;
    default:
    if(game.section)game.section_frames=0;game.section=0;env.finish_loading(true);session.session_flags&=~4u;game.flags&=~0xbu;
    *env.loader_stop_requested=0;*env.loader_running=1;*env.menu_state=0;set_enabled(*reinterpret_cast<GameSystemCallbacks*>(&session),true);return 0;
    }
    if(!success)return fail();++progress.phase;return 1;
}
void GameSessionResources::show_loading(float x,float y){auto& env=environment;if(!*env.loading_animation)*env.loading_animation=env.create_loading_animation({x,y,0});}
// 0x417c80. Stage transitions keep the current background as the fading old
// background, and retain the player, GUI, replay buffers and projectile pools.
void GameSessionResources::shutdown(){auto& env=environment;auto& game=*env.game;
    env.save_score();game.flags&=~3u;*env.rate=1;
    switch(*env.pending_screen){
    case 10:show_loading(480,392);if(static_cast<i32>(game.reserved_040)==game.stage)game.flags|=1;break;
    case 11:show_loading(224,416);game.flags|=2;break;
    case 4:case 15:show_loading(480,392);break;
    case 13:show_loading(480,392);if(static_cast<i32>(game.reserved_040)!=game.stage){increment_continue_count(game);game.flags|=8;}game.flags|=1;break;
    default:break;
    }
    const auto destroy=[&](SessionObject kind){if(auto* object=env.object(kind))env.destroy_object(kind,object);};
    if(!(game.flags&2)){
        if(*env.pending_screen!=14&&*env.pending_screen!=15)destroy(SessionObject::Replay);
        for(auto kind:{SessionObject::Stage,SessionObject::PreviousStage,SessionObject::Results,SessionObject::Gui,SessionObject::Player,SessionObject::Bullets,SessionObject::Items,SessionObject::Lasers,SessionObject::TextOverlay,SessionObject::ScorePopups})destroy(kind);
    }else{
        env.discard_gui_stage();destroy(SessionObject::PreviousStage);env.object(SessionObject::PreviousStage)=env.object(SessionObject::Stage);
        set_enabled(*env.object(SessionObject::Results),false);set_enabled(*env.object(SessionObject::Bullets),false);
        auto& replay=*reinterpret_cast<Replay*>(env.object(SessionObject::Replay));if(replay.end_frame_entry)replay.end_frame_entry->flags&=~2u;if(replay.draw_entry)replay.draw_entry->flags&=~2u;
        auto& items=*reinterpret_cast<ItemManager*>(env.object(SessionObject::Items));std::memset(items.regular,0,sizeof(items.regular)+sizeof(items.faith));
    }
    if(!(game.flags&9))destroy(SessionObject::Enemies);else env.clear_enemies();
    for(auto kind:{SessionObject::Effects,SessionObject::Bomb,SessionObject::Spell})destroy(kind);
    (*env.chain)->remove_locked(session.update_entry,*env.callbacks);(*env.chain)->remove_locked(session.draw_entry,*env.callbacks);*env.current=nullptr;
#ifdef TH_ENABLE_THPRAC
    // th08 fix parity (everlasting BGM + practice restart): the teardown stop
    // must go through the ElBgmTest filter, exactly like the game's other BGM
    // stops. Otherwise a thprac practice restart hard-stops the song here while
    // the fresh stage-entry play stays swallowed, leaving silence. Restart
    // destinations (10 restart, 11 next, 13 retry) defer to the lock; a real
    // exit still forces the stop.
    const bool restart=*env.pending_screen==10||*env.pending_screen==11||*env.pending_screen==13;
    bool keep_locked=false;
    if(env.practice){
        if(restart)keep_locked=practice_bgm_stop(*env.practice);
        // A real exit forces the stop below, so drop the ElBgmTest lock/block
        // state here too; otherwise the stale lock would swallow the first play
        // of the same track in a later game and leave it silent.
        else{env.practice->el_bgm_lock=-1;env.practice->el_bgm_block=false;}
    }
#else
    const bool keep_locked=false;
#endif
    if(!(game.flags&0x22)&&!keep_locked)env.music_command(*env.display_flags&0x10?4:3);
    *env.menu_state=1;*env.background_color=game.flags&1?0:0xff000000;
}
}
