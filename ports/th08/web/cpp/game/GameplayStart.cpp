#include "GameplayStart.hpp"
#include "ItemPool.hpp"
namespace th08 {
void GameplayStart::initialize_rank(){
    constexpr i32 ranks[5][3]{{10,8,16},{10,8,16},{8,8,12},{8,8,12},{16,15,16}};
    const auto& r=ranks[game.difficulty];session.rank.value=r[0];session.rank.minimum=r[1];session.rank.maximum=r[2];
}
void GameplayStart::initialize_score(){
    auto& s=session;actions.save_score();std::memset(s.records,0,sizeof(s.records));
    for(u32 i=0;i<spell_count;i++){auto& r=s.records[i];r.header={fourcc('C','A','T','K'),sizeof(r),sizeof(r),3,0,0};r.number=u16(i);}
    const auto bytes=actions.read_score();ScoreFile score;score.decode(bytes.data(),bytes.size());
    s.numbers.high_score=score.high_score(game.shot,game.difficulty,s.numbers.high_score_retries);score.spells(s.records);score.clears(s.clears);score.practice(s.practices);
    if(game.game_flags&1){auto& p=s.practices[game.shot];s.numbers.high_score=u32(p.high_scores[game.stage][game.difficulty]);p.attempts[game.stage][game.difficulty]=wrapping_add(p.attempts[game.stage][game.difficulty],1);p.used=1;}
    std::memcpy(s.previous_records,s.records,sizeof(s.records));s.history=HighScore{};s.history.character=u8(game.shot);s.history.difficulty=u8(game.difficulty);s.history.config=s.display_config;control.frames=0;
}
bool GameplayStart::before_player(bool initial){
    if(game.stage>=9||game.difficulty>=5||game.shot<0||game.shot>=12||((game.game_flags&0x4000)&&(game.current_spell<0||game.current_spell>=222)))return false;
    auto& s=session;auto& n=s.numbers;control.load_frames=0;control.stage_mask=u16(1u<<game.stage);s.stage_copy=i32(game.stage);game.difficulty_mask=game.difficulty<4?1u<<game.difficulty:15;game.game_flags&=~1024u;
    fresh=initial||(game.game_flags&0x4001)||game.difficulty>=4;
    if(fresh){
        // The original allocates an unused random-sized decoy. Preserve its RNG
        // consumption without keeping an allocation that has no game behavior.
        s.random.bounded32(0xffff);s.config=GameConfiguration{};n=GameGlobals{};s.values.initialize_integrity();s.config=s.display_config;
        n.gauge=0;n.clock_time=game.stage==8?6:0;if(game.difficulty>=4)s.config.lives=2;if(game.game_flags&1)s.config.lives=8;
    }else{n.display_score=n.score;n.score_increment=0;s.values.set_deaths_stage(0);s.values.set_bombs_stage(0);}
    return true;
}
void GameplayStart::after_player(float initial_bombs){
    auto& s=session;auto& n=s.numbers;const auto flags=game.game_flags;
    if(fresh){
        if(!(flags&8)){s.values.set_lives(s.config.lives);s.values.set_bombs(Scalar::truncate(initial_bombs));}
        menu.arcade_origin={32,16};menu.arcade_size={384,448};s.values.set_power(0);s.stall_frames=0;
        n.display_score=n.score=0;n.score_increment=0;n.high_score=100000;n.retries=0;n.graze=0;n.points=0;
        if(game.difficulty>=4||(flags&0x4001))s.config.slow_mode=0;
        constexpr i32 point_values[]{60000,100000,200000,300000,300000};n.point_value=point_values[game.difficulty];n.point_extends=0;point_item_extend_threshold(n,game.difficulty);initialize_score();initialize_rank();
        n.deaths=0;s.values.set_deaths_stage(0);n.bombs_used=0;s.values.set_bombs_stage(0);n.captured_spells=0;s.practice.tracker_last_spell_captures=0;s.practice.tracker_dissolve_count=0;
        if(!(flags&0x4008)){
            if(!s.config.slow_mode){auto increase=[](u32& value){if(value<999999)++value;};for(const u32 difficulty:{game.difficulty,6u}){auto& p=s.statistics.counts[difficulty];increase(p.total);increase(p.characters[game.shot]);if(menu.supervisor_state==10)increase(p.restarts);if(flags&1)increase(p.practices);}}
        }else s.config.slow_mode=0;
    }
    s.rank.fraction=0;n.points_stage=n.graze_stage=0;menu.pause_state=0;game.game_flags&=~0x2180u;n.gauge_copy=n.gauge;n.time_orbs=n.total_time_orbs=0;
    // A fifth difficulty indexes the next row in the original contiguous table.
    // Extra at the final row reads the first rank entry (10).
    constexpr i32 quotas[]{2000,2500,2700,3000,6500,7200,7200,7200,7500,8500,8800,8800,9999,9999,9999,9999,7500,8500,8500,8500,9999,9999,9999,9999,0,0,0,0,0,0,0,0,0,0,0,0,10};
    n.last_spell_requirement=game.game_flags&0x4000?0:quotas[game.stage*4+game.difficulty];
    if(game.game_flags&1)s.values.set_power(game.game_flags&0x4000?(game.current_spell<2?30:game.current_spell<13?80:128):game.stage==0?0:game.stage==1?112:128);
    if(game.game_flags&8)initialize_rank();
}
void GameplayStart::after_replay(){auto& s=session;if(game.game_flags&8){const u16 seed=s.random.seed;s.values.update_integrity();s.random.seed=seed;}s.replay_seed=s.random.seed;}
void GameplayStart::after_resources(bool keep,const char (&songs)[3][128],i32& background_state){
    const bool spell=game.game_flags&0x4000;const i32 id=game.current_spell;
    if(spell&&((game.stage==6&&(id<119||id>122))||(game.stage==7&&(id<147||id>150))||(game.stage==8&&(id<191||id>193)&&id!=213)))background_state=2;
    if(!keep){if(spell){const auto& music=spell_music(id);if(music.last_spell>=0)actions.preload_music(0,music.path);}else{actions.preload_music(0,songs[0]);for(i32 i=1;i<3;i++)if(songs[i][0]!=' ')actions.preload_music(i,songs[i]);}}
    menu.show_retry=game.stage_completion=0;game.game_flags|=4;control.start_music=keep&&spell&&!spell_music(id).pause_in_practice?2:1;
    session.numbers.score=0;game.game_flags&=~16u;control.sticky_input=false;
}
}
