#pragma once
#include "encounter_scene.hpp"
#include "practice_targets.hpp"
#include <memory>
#include <optional>
#include <utility>

namespace soew {
// Resolve an attempt once before dispatching scene preparation. The stored
// recipe remains unchanged so a retry can select again using the same mode.
template<class Generator>
std::uint32_t choose_practice_seed(const PracticeSpec &recipe,std::optional<std::uint32_t> prior_seed,Generator &&generate){
    if(recipe.seed_mode!=PracticeSeedMode::Random)return recipe.seed;
    const auto candidate=static_cast<std::uint32_t>(std::forward<Generator>(generate)());
    // Scene seeding uses the low byte of IRand's output (state bits 16..23).
    // Seeds differing only in the high byte would repeat the same random ring.
    constexpr std::uint32_t effective_mask=0x00ffffffu;
    return prior_seed&&((candidate^*prior_seed)&effective_mask)==0?candidate^1u:candidate;
}
class PracticeBuilder {
public:
    enum class Status { Preparing, Ready, Failed, Cancelled };
    static constexpr unsigned maximum_updates=50000;
    PracticeBuilder(const std::filesystem::path &root,PracticeSpec recipe,
                    std::optional<PracticeCheckpoint> checkpoint=std::nullopt,
                    std::optional<std::uint32_t> concrete_seed=std::nullopt,bool reduced_effects=false,
                    Language language=Language::Japanese,PracticeStartRule rule=PracticeStartRule::Calibrated)
        : recipe_(recipe),checkpoint_(checkpoint),
          seed_(recipe.seed_mode==PracticeSeedMode::Random?concrete_seed.value_or(recipe.seed):recipe.seed) {
        try {
            if(auto error=validate_practice(recipe_);!error.empty()){fail(error);return;}
            if(recipe_.seed_mode==PracticeSeedMode::Random&&!concrete_seed){fail("Random attempt seed not resolved");return;}
            const auto caps=practice_capabilities();
            if(!caps.allows(recipe_.stage,recipe_.section)||!caps.allows_phase(recipe_.stage,recipe_.boss_phase)){
                fail("Practice target unavailable");return;
            }
            if(recipe_.section==PracticeSection::Chapter){
                if(!checkpoint_||rule==PracticeStartRule::LegacyV4)checkpoint_=practice_checkpoint(recipe_,rule);
                if(!checkpoint_||checkpoint_->stage!=recipe_.stage||checkpoint_->chapter_id!=recipe_.chapter_id){
                    fail("Chapter checkpoint unavailable");return;
                }
            }
            // Current inventory may exceed original option-menu stock. Keep the
            // legal run-start baseline separate from current practice inventory.
            scene_=std::make_unique<EncounterScene>(EncounterScene::load(root,recipe_.stage,recipe_.shot,recipe_.rank,std::min(recipe_.lives,4),recipe_.respawn_bombs,language));
            auto &s=*scene_;
            s.preparation_only=true;
            s.reduced_effects=reduced_effects;
            s.combat_enabled=s.survival_enabled=s.pickup_enabled=true;
            s.enable_effects(seed_);s.enable_bombing();
            switch(recipe_.stage){
            case 0:s.enable_midboss();s.enable_preboss();break;
            case 1:s.midboss2_enabled=true;s.enable_preboss();break;
            case 2:s.midboss3_enabled=true;s.enable_preboss();break;
            case 3:
                if(s.script.known_stage4_short_timeline)s.script.enable_stage4_compatibility();
                s.enable_preboss();break;
            case 4:s.enable_stage5_preview();break;
            case 5:s.enable_extra();break;
            }
            s.practice_mode=true;s.transitions_enabled=true;
            s.practice_show_dialogue=recipe_.dialogue==PracticeDialogue::Show;
            initialize_target();finish();
        }catch(const std::exception &e){fail(e.what());}
    }
    PracticeBuilder(const PracticeBuilder&)=delete;
    PracticeBuilder& operator=(const PracticeBuilder&)=delete;
    PracticeBuilder(PracticeBuilder&&)=default;
    PracticeBuilder& operator=(PracticeBuilder&&)=default;
    Status status()const{return status_;}
    const std::string &error()const{return error_;}
    unsigned prepared_ticks()const{return 0;}
    const PracticeSpec &recipe()const{return recipe_;}
    std::uint32_t seed()const{return seed_;}
    const EncounterScene *scene()const{return scene_.get();}
    Status advance(unsigned =256){return status_;}
    void cancel(){if(status_==Status::Preparing||status_==Status::Ready){scene_.reset();status_=Status::Cancelled;}}
    std::unique_ptr<EncounterScene> release(){
        if(status_!=Status::Ready||!scene_)throw std::runtime_error("Practice scene is not ready");
        return std::move(scene_);
    }
private:
    PracticeSpec recipe_;
    std::optional<PracticeCheckpoint> checkpoint_;
    std::uint32_t seed_;
    std::unique_ptr<EncounterScene> scene_;
    Status status_=Status::Preparing;
    std::string error_;
    void fail(std::string error){error_=std::move(error);status_=Status::Failed;scene_.reset();}
    void initialize_scroll(unsigned progress,bool done=false){
        auto &s=*scene_;
        const auto count=s.background.map.size();
        if(count<3||count>320||progress>2*(8*count-24))throw std::runtime_error("Practice scroll progress outside map");
        if(!progress)return; // Chapter 1 uses the ordinary fresh-stage initializer.
        s.seek_practice_ambient(progress);
        auto &scroll=s.runtime.scroll;
        scroll=StageScroll(recipe_.stage);
        scroll.progress=progress;scroll.top_row=23+(progress+1)/2;
        const auto remainder=(progress%50)*8;
        scroll.line=remainder?int(400-remainder):0;
        scroll.tile_line=(progress&1)?8:0;
        scroll.pending_progress=false;scroll.done=done;scroll.cycle=0;
        // Native rendering samples the map directly instead of a VRAM tile ring.
        // This is ONLY a visual map offset, including the odd-progress half row;
        // it is not naturally elapsed pixels or a simulation-frame estimate.
        scroll.display_pixels=16*(scroll.top_row-23)-unsigned(scroll.tile_line);
        scroll.attempted_pixels=0;
        for(unsigned row=scroll.top_row-24;row<=scroll.top_row;++row){
            const auto section=s.background.map.at(row/8);
            if(section>=16)throw std::runtime_error("Practice map section outside original tile bank");
            for(unsigned col=0;col<24;++col){
                const auto tile=s.background.sections.at((section*8+row%8)*24+col);
                if(tile>=s.background.tiles.size())throw std::runtime_error("Practice map tile unavailable");
            }
        }
        // The Extra asset has stale out-of-order tail rows. The original
        // dispatch cannot consume them out of order; never sort or skip those rows.
        s.runtime.dispatch.cursor=std::size_t(std::find_if(s.script.formations.begin(),s.script.formations.end(),
            [progress](const auto &formation){return formation.trigger>progress;})-s.script.formations.begin());
        s.runtime.page=0;s.runtime.frame_prepared=false;
        s.scroll=scroll;s.encounter_observer=s.runtime.dispatch;
        if(s.extra_road_enabled){
            // Start both pages with the direct-load tile ring (Azinn MAIN 0x27996).
            auto &tiles=s.extra_tiles;
            tiles.display_line=scroll.line;
            for(unsigned i=0;i<StageTileRing::rows;++i){
                const unsigned row=(unsigned(scroll.line/16)+i)%StageTileRing::rows;
                auto &entry=tiles.ring[row];entry.map_row=int(scroll.top_row-i);
                for(unsigned col=0;col<StageTileRing::columns;++col){
                    entry.cells[col]=StageTileRing::map_tile(s.background,unsigned(entry.map_row),col);
                    tiles.restore_cell(s.background,0,row,col);
                    tiles.restore_cell(s.background,1,row,col);
                }
            }
            tiles.new_row_cache=tiles.ring[unsigned(scroll.line/16)].cells;
        }
    }
    void initialize_boss(){
        auto &s=*scene_;
        if(recipe_.stage==0){
            // Azinn MAIN file 0x27D64..0x27E58, actual-byte contract.
            s.rika_handoff_context={192,0,0,0,0,unsigned(s.runtime.scroll.line)};
            s.bind_rika_battle(s.rika_handoff_context);
            s.rika.damage_taken=std::uint16_t((recipe_.boss_phase>0?recipe_.boss_phase-1:0)*700);
            s.rika.left.fill(192);s.rika.cached_x=192;
            s.rika.render_y=(s.runtime.scroll.line+48)%400;
            s.rika.phase_frame=s.rika.death_frame=s.rika.attack_frame=0;
            s.rika.state=0;s.rika.sweep_angle=0;
            s.rika.pattern=s.rika.draw_pattern=150;
            s.rika.dying=false;
        }else if(recipe_.stage==1){
            // Azinn MAIN file 0x2A0B1..0x2A261. These values deliberately
            // replace vanilla inherited state; no earlier encounter is replayed.
            s.meira_handoff=MeiraCore::Inherited{};
            s.meira_handoff.origin_x=s.meira_handoff.destination_x=192;
            s.meira_handoff.origin_y=s.meira_handoff.destination_y=48;
            for(auto &page:s.meira_handoff.clones)for(auto &point:page)point={192,48};
            s.bind_meira_battle();
            const unsigned phase=recipe_.boss_phase>0?unsigned(recipe_.boss_phase-1):0;
            constexpr std::array<unsigned,3> damage{0,701,1501};
            s.meira.damage_taken=std::uint16_t(damage[phase]);
            s.meira.state=phase;s.meira.attack_index=0;
            s.meira.left.fill(192);s.meira.top.fill(48);
            s.meira.phase_frame=s.meira.death_frame=0;
            s.meira.pattern=s.meira.draw_pattern=141;
            s.meira.special_angle=0;s.meira.special_turns_max=2;
            s.meira.dying=false;s.meira.clones_enabled=false;
        }else if(recipe_.stage==2){
            s.bind_stones_battle();
            const auto chapter=recipe_.boss_phase;
            if(chapter){
                using P=StonesCore::Phase;using F=StonesCore::Flag;
                auto &b=s.stones;
                constexpr std::array phases={P::Inner,P::Outer,P::NorthOpening,P::NorthSecondAttacks,P::NorthFinalAttacks};
                b.phase=phases.at(chapter-1);
                for(unsigned i=0;i<4;++i){
                    const bool active=chapter==1?i<2:chapter==2&&i>=2;
                    const bool removed=chapter>=3||(chapter==2&&i<2);
                    b.stones[i].flag=active?F::Active:removed?F::Removed:F::Dormant;
                    if(active)b.stones[i].pattern=i<2?155:159;
                }
                if(chapter>=4)b.stones[4].damage=500;
                if(chapter==5){
                    b.stones[4].flag=F::Active;b.stones[4].pattern=163;
                    const unsigned row=unsigned((96+s.scroll.line)%400)/16;
                    for(unsigned col=0;col<24;++col)s.stones_stripe[row*24+col]=43;
                }
            }
        }else if(recipe_.stage==3){
            s.bind_marisa_battle();s.marisa_music=true;
            if(recipe_.boss_phase>1){
                s.marisa.rounds=unsigned(recipe_.boss_phase-1);
                s.marisa.body_damage_enabled=s.marisa.rounds>=2;
                s.marisa.phase=MarisaCore::Phase::DeployOrbs;
            }
        }else if(recipe_.stage==4){
            // Use the source scene's normal binding invariants after advancing
            // only the dialogue completion marker; no dialogue ticks are run.
            s.dialogue.current=s.dialogue.pre_count;s.dialogue.phase=StageDialogue::Phase::Complete;
            s.bind_mima_battle();s.mima_music=true;
            if(recipe_.boss_phase){
                MimaCore::Hooks h;
                h.random8=[&]{return unsigned(s.runtime.random.next8());};
                h.palette0=[&](unsigned r,unsigned g,unsigned b){s.mima_palette0={std::uint8_t(r),std::uint8_t(g),std::uint8_t(b)};};
                s.mima.start_practice_phase(unsigned(recipe_.boss_phase),h);
            }
        }else{
            s.sigma.init();s.sigma_enabled=s.sigma_music=true;
            if(recipe_.boss_phase)s.sigma.phase=SigmaCore::cycles.at(recipe_.boss_phase-1).phase;
        }
        s.midboss.active=s.midboss.present=s.midboss.visible=false;
        s.midboss2.active=s.midboss2.present=s.midboss2.visible=false;
        // Common boss wrapper resets stage time, then selects cycle 255.
        s.runtime.scroll.cycle=255;s.scroll=s.runtime.scroll;
    }
    void initialize_target(){
        auto &s=*scene_;
        s.transition.opening_caption=recipe_.section==PracticeSection::StageStart;
        switch(recipe_.section){
        case PracticeSection::StageStart:break;
        case PracticeSection::Chapter:
            initialize_scroll(checkpoint_->progress);
            if(checkpoint_->dispatch_on_first_tick){
                auto &scroll=s.runtime.scroll;
                auto &cursor=s.runtime.dispatch.cursor;
                if(!cursor||s.script.formations[cursor-1].trigger!=scroll.progress)
                    throw std::runtime_error("Practice closing-wave formation unavailable");
                --cursor;
                // Hand off immediately after the 8-pixel crossing, before the next
                // playable frame consumes the event. No enemy or bullet is synthesized.
                scroll.pending_progress=true;
                scroll.cycle=std::uint8_t((scroll.progress*8/scroll.speed-1)*scroll.interval+1);
                s.scroll=scroll;s.encounter_observer=s.runtime.dispatch;
            }
            if(checkpoint_->formation_cursor&&s.runtime.dispatch.cursor!=*checkpoint_->formation_cursor)
                throw std::runtime_error("Practice formations do not match verified chapter");
            break;
        case PracticeSection::Midboss:{
            constexpr std::array<unsigned,practice_stage_count> progress={116,80,103,Midboss4Core::triggers[0],0,ExtraMidbossCore::trigger_progress};
            initialize_scroll(progress[recipe_.stage]);
            break;
        }
        case PracticeSection::Midboss2:initialize_scroll(Midboss4Core::triggers[1]);break;
        case PracticeSection::Boss:
            initialize_scroll(unsigned((s.background.map.size()*8-24)*2),true);
            if(recipe_.dialogue==PracticeDialogue::Show)s.preboss_ready=true;
            else {initialize_boss();s.preboss_started=true;}
            break;
        }
        if(recipe_.section==PracticeSection::Chapter||recipe_.section==PracticeSection::Midboss||recipe_.section==PracticeSection::Midboss2){
            const unsigned progress=s.runtime.scroll.progress;
            // Retire only past actors; keep the fresh inherited context for boss dialogue.
            if(recipe_.stage==0&&progress>116)s.midboss.active=s.midboss.present=s.midboss.visible=false;
            if(recipe_.stage==1&&progress>80)s.midboss2.active=s.midboss2.present=s.midboss2.visible=false;
            if(recipe_.stage==3)s.midboss4.entries=unsigned(std::count_if(Midboss4Core::triggers.begin(),Midboss4Core::triggers.end(),[&](unsigned p){return p<progress;}));
            if(recipe_.stage==5&&progress>ExtraMidbossCore::trigger_progress)s.extra_midboss.started=s.extra_midboss.finished=true;
        }
    }
    void finish(){
        auto &s=*scene_;
        s.life.playperf=recipe_.dynamic_rank;s.bullet_core.playperf=recipe_.dynamic_rank;
        s.life.lives=static_cast<std::uint8_t>(recipe_.lives);
        s.life.bombs=static_cast<std::uint8_t>(recipe_.bombs);
        s.life.start_bombs=static_cast<std::uint8_t>(recipe_.respawn_bombs);
        s.life.power=static_cast<std::uint8_t>(recipe_.power);s.pickups.power=unsigned(recipe_.power);
        s.lives=recipe_.lives;s.bombs=recipe_.bombs;
        s.shots.shot_type=unsigned(recipe_.shot);s.shots.level=s.pickups.level();
        if(!s.shots.supports_level()){fail("Practice power requires an unsupported weapon");return;}
        s.life.invincibility=0;s.life.bomb_invincible=false;
        s.life.draw=s.life.current;s.life.pattern=0;
        s.shots.option_left=s.player.x-16-s.player.last_dx;s.shots.option_top=s.player.y+16-s.player.last_dy;
        s.sound.current=255;s.sound.frame=0;s.sound_output={};
        s.consumed_shot_sounds=s.shots.sound_requests;
        s.consumed_bomb_sounds=s.bomb.sounds.size();s.consumed_pickup_sounds=s.pickups.sounds.size();
        s.observed_extend_sounds=s.score_core.sound8;s.consumed_life_effects=s.life.effects.size();
        s.consumed_bomb_sparks=s.bomb.spark_requests.size();
        s.preparation_only=false;
        status_=Status::Ready;
    }
};

class PracticeSession {
public:
    enum class Result { Running, Success, Failure };
    PracticeSpec recipe;
    explicit PracticeSession(PracticeSpec spec,std::optional<PracticeCheckpoint> =std::nullopt,
                             std::optional<std::uint32_t> concrete_seed=std::nullopt)
        :recipe(spec),seed_(spec.seed_mode==PracticeSeedMode::Random?concrete_seed.value_or(spec.seed):spec.seed){}
    std::uint32_t seed()const{return seed_;}
    std::uint32_t samples()const{return samples_;}
    Result result()const{return result_;}
    const std::string &error()const{return error_;}
    bool simulation_error()const{return result_==Result::Failure&&!normal_loss_;}
    void tick(EncounterScene &s,std::uint16_t input,bool group_key=false,bool dialogue_fast_forward=false){
        if(result_!=Result::Running||s.paused)return;
        ++samples_;
        s.practice_mode=true;
        s.practice_show_dialogue=recipe.dialogue==PracticeDialogue::Show;
        if(failed(s)){stop(s,Result::Failure);return;}
        if(completed(s)){stop(s,Result::Success);return;}
        try{
            if(!s.practice_show_dialogue&&s.mima_enabled&&s.mima.event==MimaCore::Event::Form1Defeated){
                // This is an inter-form scene, not an ending. The native phase
                // handoff remains intact, without the original dialogue score grant.
                MimaCore::Hooks hooks;hooks.random8=[&]{return unsigned(s.runtime.random.next8());};
                if(!s.mima.resolve_form1(hooks,0).enter_form2)throw std::runtime_error("Mima practice form handoff failed");
                s.clear_shots();s.mima_tone=100;s.mima_draws.clear();s.dialogue_snapshot.clear();
                return;
            }
            s.tick(input,group_key,dialogue_fast_forward);
        }catch(const std::exception &e){error_=e.what();stop(s,Result::Failure);return;}
        if(failed(s)){stop(s,Result::Failure);return;}
        if(completed(s))stop(s,Result::Success);
    }
private:
    std::uint32_t seed_,samples_=0;
    Result result_=Result::Running;
    bool normal_loss_=false;
    std::string error_;
    bool failed(const EncounterScene &s){
        if(s.runtime.blocked)error_="Unsupported enemy path";
        else if(s.weapon_blocked)error_="Unsupported weapon";
        else if(!s.transition_error.empty())error_=s.transition_error;
        else if(s.mima_enabled&&s.mima.blocked_reason())error_=s.mima.blocked_reason();
        else if(s.sigma_enabled&&s.sigma.blocked)error_=s.sigma.blocked;
        else if(s.life.game_over){normal_loss_=true;error_="Game over";}
        return !error_.empty();
    }
    bool completed(const EncounterScene &s)const{
        return s.practice_completed||(!s.practice_show_dialogue&&(
            (s.rika_enabled&&s.rika.done)||(s.meira_enabled&&s.meira.done)||
            (s.stones_enabled&&s.stones.done())||(s.marisa_enabled&&s.marisa.cleared)||
            (s.mima_enabled&&s.mima.event==MimaCore::Event::FinalDefeated)||(s.sigma_enabled&&s.sigma.done)));
    }
    void stop(EncounterScene &s,Result result){result_=result;s.practice_completed=true;s.sound.current=255;s.sound.frame=0;}
};
}
