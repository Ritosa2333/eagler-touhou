#pragma once
#include "stage_sprites.hpp"
#include "enemy_path_runtime.hpp"
#include "bullet_core.hpp"
#include "player_shots.hpp"
#include "enemy_damage.hpp"
#include "player_life.hpp"
#include "spark_core.hpp"
#include "item_core.hpp"
#include "score_core.hpp"
#include "pickup_core.hpp"
#include "bomb_renderer.hpp"
#include "midboss1_core.hpp"
#include "midboss2_core.hpp"
#include "midboss3_core.hpp"
#include "midboss4_core.hpp"
#include "extra_midboss_core.hpp"
#include "marisa_core.hpp"
#include "sigma_core.hpp"
#include "mima_core.hpp"
#include "stones_core.hpp"
#include "meira_core.hpp"
#include "rika_core.hpp"
#include "laser_core.hpp"
#include "dialogue_renderer.hpp"
#include "locale.hpp"
#include "sound_effects.hpp"
#include "stage_transition.hpp"
#include <map>
namespace soew {
// Visible original-art development slice, NOT a playable original stage.
// Original shots, life, pickup/score/sparks and optional Bomb combat phases.
// No Continue/Boss/midboss; sound requests are still recorded only.
// Tile-changing paths stop explicitly until the real consumer is wired.
struct EncounterScene:StageScene {
    StageSprites bank;
    std::filesystem::path assets_root;
    Language language=Language::Japanese;
    StageDialogue load_dialogue(unsigned id) const {
        auto result=StageDialogue::decode(StageScene::read(locale_asset(assets_root,language,"STAGE"+std::to_string(id)+".TXT")));
        result.configure(id);
        return result;
    }
    EnemyPathRuntime runtime{0,1234};
    BulletCore bullet_core;
    std::uint64_t sound_requests=0,hit_checks=0;
    SoundEffects sound;
    std::function<void(unsigned)> sound_output;
    std::size_t consumed_shot_sounds=0,consumed_bomb_sounds=0,consumed_pickup_sounds=0;
    unsigned observed_extend_sounds=0;
    void request_sound(unsigned id){++sound_requests;sound.request(id);}
    void update_sound(){if(auto id=sound.update();id&&sound_output)sound_output(*id);}
    unsigned rendered_page=0;
    bool reduced_effects=false;
    static constexpr bool stage4_overlay_enabled(int id,unsigned page,bool map_end,bool reduced){
        return id==3&&page==0&&!map_end&&!reduced;
    }
    bool combat_enabled=false,survival_enabled=false;
    bool preparation_only=false,practice_mode=false,practice_completed=false,practice_show_dialogue=false;
    bool preboss_ready=false;
    PlayerLife life;
    bool effects_enabled=false;
    bool bombing_for_spawn=false;
    SparkCore sparks;
    ItemCore items;
    ScoreCore score_core;
    PickupCore pickups;
    PointnumCore numbers;
    bool pickup_enabled=false;
    bool weapon_blocked=false,bomb_enabled=false;
    BombCore bomb;
    std::optional<BombRenderer> bomb_art;
    bool midboss_enabled=false;
    Midboss1Core midboss;
    bool midboss2_enabled=false;
    Midboss2Core midboss2;
    bool midboss3_enabled=false;
    Midboss3Core midboss3;
    Midboss4Core midboss4;
    bool extra_road_enabled=false;
    ExtraMidbossCore extra_midboss;
    StageTileRing extra_tiles;
    void enable_extra_road(){
        if(stage!=5||rank!=4||ticks||runtime.ticks||runtime.blocked||extra_road_enabled)
            throw std::runtime_error("Extra road requires a fresh rank-4 stage 5 scene");
        extra_tiles.reset(background);
        extra_road_enabled=true;
    }
    SigmaCore sigma;
    ExtraSettlement extra_settlement;
    std::optional<StageSprites> sigma_art;
    bool sigma_enabled=false,sigma_music=false;
    void enable_extra(){
        auto art=bank.extra_art(assets_root);
        auto text=load_dialogue(5);
        auto renderer=DialogueRenderer::load(assets_root);
        enable_extra_road();sigma_art=std::move(art);dialogue=std::move(text);dialogue_art=std::move(renderer);
        preboss_enabled=transitions_enabled=true;
    }
    MarisaCore marisa;
    bool marisa_enabled=false;
    std::optional<StageSprites> stage4_intro_art,stage4_boss_art;
    bool marisa_music=false,stage5_entry_only=false;
    bool stage5_preview=false,mima_enabled=false,mima_music=false,mima_music_stopped=false;
    MimaCore mima;
    std::optional<StageSprites> stage5_intro_art,stage5_winged_art,stage5_final_art;
    std::optional<MimaCore::Result> stage5_result;
    enum class MimaPaint { Dot, Line, Circle, Fill };
    struct MimaDraw { MimaPaint kind; int x,y,a,b; unsigned color; };
    std::vector<MimaDraw> mima_draws;
    unsigned mima_tone=100,mima_delay=0,mima_delay_frame=0;
    std::array<std::uint8_t,3> mima_palette0{};
    void bind_marisa_battle(){
        if(stage!=3||!runtime.scroll.done||runtime.blocked||!stage4_boss_art)
            throw std::runtime_error("Invalid Marisa battle handoff");
        marisa=MarisaCore{};marisa_enabled=true;
        midboss4.active=midboss4.visible=false;
        for(auto &enemy:runtime.enemies.slots)if(enemy.raw[0xe]==1)enemy.raw[0xe]=2;
        death_patterns.fill(std::nullopt);
    }
    bool stones_enabled=false;
    StonesCore stones;
    std::map<unsigned,unsigned> stones_stripe;
    void bind_stones_battle(){
        if(stage!=2||!runtime.scroll.done||stones_enabled)throw std::runtime_error("Invalid Stones test binding");
        stones.init();stones_stripe.clear();lasers=LaserCore{};
        for(auto &enemy:runtime.enemies.slots)if(enemy.raw[0xe])enemy.raw[0xe]=2;
        stones_enabled=true;
    }
    bool meira_enabled=false,meira_appearance=false;
    unsigned appearance_frame=0;
    MeiraCore meira;
    MeiraCore::Inherited meira_handoff;
    bool rika_enabled=false;
    RikaCore rika;
    bool transitions_enabled=false,stage2_preview_boundary=false,stage3_preview_boundary=false;
    StageTransition transition;
    unsigned run_start_lives=2,run_start_bombs=3;
    unsigned continues_used=0;
    std::uint32_t highest_run_score=0;
    enum class ContinueChoice { Inactive, Release, Choose, Declined };
    ContinueChoice continue_choice=ContinueChoice::Inactive;
    bool continue_yes=true;
    std::uint32_t resident_frame=0;
    std::string transition_error;
    LaserCore lasers;
    std::uint32_t rika_stage_frame=0;
    bool preboss_enabled=false,preboss_started=false,normal_rika_handoff=false;
    StageDialogue dialogue;
    std::optional<DialogueRenderer> dialogue_art;
    RikaCore::Inherited rika_handoff_context{0,0,0,0,0,0};
    std::vector<std::uint32_t> dialogue_snapshot;
    std::map<unsigned,unsigned> track_tiles; // Virtual map row*24+column -> tile.
    std::size_t consumed_bomb_sparks=0;
    std::size_t consumed_life_effects=0;
    PlayerShots shots;
    EnemyDamage damage;
    std::vector<EnemyKillRequest> kills_recorded;
    std::array<std::optional<unsigned>,25> death_patterns;
private:
    HitboxMode hitbox_mode_=HitboxMode::Off;
    CollisionFrame collision_frame_;
    bool hitbox_battle_visible()const{
        return survival_enabled&&!life.miss_active&&!preparation_only&&!practice_completed&&!stage2_preview_boundary&&!stage3_preview_boundary&&
            !stage5_entry_only&&!stage5_result&&!mima_delay&&!dialogue.active()&&!life.game_over&&
            (!transitions_enabled||!transition.active())&&extra_settlement.phase==ExtraSettlement::Phase::Inactive&&
            !runtime.blocked&&!weapon_blocked&&!sigma.blocked&&!mima.blocked_reason()&&transition_error.empty();
    }
    bool sigma_view_shift()const{return sigma_enabled&&sigma.shake&&!reduced_effects;}
    struct DeathPresentation {unsigned slowdown=1,tone=100,shift=0;};
    template<class Effects>
    DeathPresentation death_presentation(const Effects &effects,unsigned count)const{
        DeathPresentation out;
        for(unsigned i=0;i<count;++i){
            const auto frame=effects[i].frame;
            if(frame<32||frame>=58)continue;
            const auto age=frame-32;
            // sub_FFF8 writes each of these independently, in request order.
            out.slowdown=age<=20?3:age<24?2:1;
            if(!reduced_effects){
                if(age<=8)out.tone=100+70*(age&1);
                if(age<=20)out.shift=16*(age&1);
            }
        }
        return out;
    }
    bool battle_presentation()const{
        return !dialogue.active()&&(!transitions_enabled||transition.phase==StageTransition::Phase::Battle);
    }
    DeathPresentation boss_death_presentation()const{
        if(!battle_presentation())return {};
        if(marisa_enabled&&marisa.defeated)return death_presentation(marisa.death_effects,marisa.death_effect_count);
        if(stones_enabled)return death_presentation(stones.death_effects,stones.death_effect_count);
        return {};
    }
public:
    unsigned presentation_slowdown()const{
        if(sigma_enabled&&!sigma.done)return sigma.slowdown;
        return boss_death_presentation().slowdown;
    }
    bool marisa_terminal_frame()const{
        return marisa_enabled&&marisa.terminal_death_frame()&&battle_presentation();
    }
    bool stones_terminal_frame()const{
        return stones_enabled&&stones.terminal_death_frame()&&battle_presentation();
    }
    bool boss_terminal_frame()const{return marisa_terminal_frame()||stones_terminal_frame();}
    void invalidate_hitboxes(){collision_frame_.scene=nullptr;}
    void set_hitbox_mode(HitboxMode mode){
        if(mode==hitbox_mode_)return;
        hitbox_mode_=mode;invalidate_hitboxes();
    }
    const CollisionFrame *collision_snapshot()const{
        if(hitbox_mode_==HitboxMode::Off||collision_frame_.scene!=this||collision_frame_.completed_tick!=ticks||
           collision_frame_.page!=rendered_page||!hitbox_battle_visible())return nullptr;
        return &collision_frame_;
    }
    void render_hitboxes(std::span<std::uint32_t> pixels)const{
        if(const auto *display=collision_snapshot())draw_collision_frame(pixels,*display,reduced_effects,-int(boss_death_presentation().shift));
    }
    static EncounterScene load(const std::filesystem::path &root,int id,int shot,int selected_rank,int spare,int stock,Language language=Language::Japanese){
        EncounterScene out;out.language=language;static_cast<StageScene&>(out)=StageScene::load(root,id,shot,selected_rank,spare,stock);
        out.run_start_lives=unsigned(spare);out.run_start_bombs=unsigned(stock);
        out.assets_root=root;out.bank=StageSprites::load(root,id);out.runtime=EnemyPathRuntime(id,1234);out.bullet_core=BulletCore(selected_rank);
        out.life=PlayerLife(static_cast<unsigned>(spare),static_cast<unsigned>(stock));out.life.draw=out.life.current;out.life.pattern=0;out.bomb.stage_used=0;out.bombing_for_spawn=false;
        if(out.bank.next<=128)throw std::runtime_error("No original encounter sprites");
        // Original template auto-fire cadence scales at loader time. Preserve
        // original extraction; apply signed16 changes to scene-local copy only.
        for(auto &enemy:out.script.enemies){int cadence=enemy_s16(unsigned(enemy.raw[26])|(unsigned(enemy.raw[27])<<8));if(selected_rank==0)cadence=enemy_s16(unsigned(cadence*2));else if(selected_rank==2||selected_rank==3)cadence=cadence>=0?cadence/2:-((-cadence+1)/2);enemy.raw[26]=static_cast<std::uint8_t>(cadence);enemy.raw[27]=static_cast<std::uint8_t>(unsigned(cadence)>>8);}
        return out;
    }
    // Diagnostic battle binding ONLY; normal map-end dialogue is not skipped.
    void bind_rika_battle(const RikaCore::Inherited &old){
        if(stage!=0||unsigned(runtime.scroll.line)!=old.scroll_line)throw std::runtime_error("Rika context must match frozen scene scroll");
        rika.init(unsigned(rank),old);rika_stage_frame=0;rika_enabled=true;midboss_enabled=false;
        for(auto &e:runtime.enemies.slots)if(e.raw[0xe]==1)e.raw[0xe]=2;
    }
    void bind_meira_battle(){if(stage!=1)throw std::runtime_error("Invalid Meira stage");meira.init(unsigned(rank),meira_handoff);meira_enabled=true;midboss2_enabled=false;bullet_core.special.u3=2;for(auto&e:runtime.enemies.slots)if(e.raw[0xe]==1)e.raw[0xe]=2;}
    bool continue_prompt()const{return continue_choice==ContinueChoice::Release||continue_choice==ContinueChoice::Choose;}
    void update_continue(std::uint16_t input,bool held){
        // th02_main.asm:800–815,1306–1328,2237–2345. Resume in the
        // existing scene; only a successfully accepted credit belongs to the run.
        if(continue_choice==ContinueChoice::Inactive){
            continue_yes=true;
            continue_choice=(rank<4&&continues_used<3)?ContinueChoice::Release:ContinueChoice::Declined;
            return;
        }
        if(continue_choice==ContinueChoice::Release){if(!input&&!held)continue_choice=ContinueChoice::Choose;return;}
        if(continue_choice!=ContinueChoice::Choose)return;
        if(input&Up)continue_yes=true;
        if(input&Down)continue_yes=false;
        if(input&Cancel){continue_choice=ContinueChoice::Declined;return;}
        if(!(input&(Shot|OK)))return;
        if(!continue_yes){continue_choice=ContinueChoice::Declined;return;}
        ++continues_used;
        highest_run_score=std::max(highest_run_score,score_core.score);
        score_core.score=score_core.delta=score_core.transferred=0;
        life.lives=std::uint8_t(run_start_lives);life.bombs=std::uint8_t(run_start_bombs);
        lives=life.lives;bombs=life.bombs;
        life.power=1;pickups.power=1;pickups.overflow=10;
        items.bigpower_override=unsigned(stage)%5+2;
        life.hit=life.miss_active=life.game_over=life.bomb_invincible=false;
        life.miss_frame=0;life.invincibility=200;
        life.positions={PlayerLife::Point{},PlayerLife::Point{}};
        life.current=life.draw=life.positions[0];life.pattern=0;
        player=PlayerMotion(player.shot);bullet_core.clear(runtime.page);
        if(bomb.active)bomb.finish();
        continue_choice=ContinueChoice::Inactive;
        transition.phase=StageTransition::Phase::Entry;transition.frame=0;
    }
    void start_postboss(){
        dialogue_snapshot.clear();dialogue_snapshot.resize(640*400);render(dialogue_snapshot,false);
        if(!practice_mode){
            unsigned stock=life.lives;score_core.grant_dialogue_bonus(stock);life.lives=std::uint8_t(stock);lives=int(stock);
        }
        dialogue.continues_used=continues_used;
        dialogue.start_postboss();transition.phase=StageTransition::Phase::Postboss;
    }
    void finish_postboss(bool key){
        transition.bonus=StageBonus::regular(rank,life.playperf,bomb.stage_used,life.stage_misses,run_start_bombs,run_start_lives,pickups.points_collected);
        score_core.add(transition.bonus.internal_bonus);unsigned stock=life.lives;score_core.grant_dialogue_bonus(stock);life.lives=std::uint8_t(stock);lives=int(stock);
        sound.current=255;sound.frame=0;transition.bonus_ready(key);
    }
    EncounterScene next_stage(bool strict_stage4=false)const{
        if(stage>3||(stage==3&&(!marisa_enabled||!marisa.cleared))||transition.phase!=StageTransition::Phase::Ready)throw std::runtime_error("Invalid stage transition request");
        auto next=load(assets_root,stage+1,player.shot,rank,life.lives,life.bombs,language);
        next.combat_enabled=combat_enabled;next.survival_enabled=survival_enabled;next.pickup_enabled=pickup_enabled;
        next.reduced_effects=reduced_effects;next.set_hitbox_mode(hitbox_mode_);
        next.continues_used=continues_used;next.highest_run_score=highest_run_score;
        next.run_start_lives=run_start_lives;next.run_start_bombs=run_start_bombs;next.resident_frame=resident_frame;
        next.life.start_bombs=life.start_bombs;next.life.playperf=life.playperf;next.life.power=life.power;next.life.total_misses=life.total_misses;
        next.score_core=score_core;next.score_core.stage_extend_init();next.observed_extend_sounds=next.score_core.sound8;
        next.pickups.power=pickups.power;next.pickups.overflow=pickups.overflow;next.pickups.collect_skill=pickups.collect_skill;next.pickups.item_skill=pickups.item_skill;
        next.items.item_skill=items.item_skill;next.items.drop_cycle=items.drop_cycle;next.items.cycle=items.cycle;next.items.bigpower_override=items.bigpower_override;
        if(effects_enabled){IRand rng(resident_frame);next.sparks.initialize(rng,false);next.runtime.random.fill(rng);next.items.initialize(next.runtime.random);next.effects_enabled=true;}
        if(bomb_enabled){next.enable_bombing();next.bomb.total_used=bomb.total_used;}
        next.sound.enabled=sound.enabled;next.sound_output=sound_output;
        if(next.stage==1){next.midboss2_enabled=true;next.enable_preboss();}
        if(next.stage==2){next.midboss3_enabled=true;next.enable_preboss();}
        if(next.stage==3&&!strict_stage4&&next.script.known_stage4_short_timeline)
            next.script.enable_stage4_compatibility();
        if(next.stage==4)next.stage5_entry_only=true;
        next.transitions_enabled=true;next.transition.phase=StageTransition::Phase::Entry;next.transition.frame=0;
        return next;
    }
    void enable_stage5_preview(){
        if(stage!=4||practice_mode)throw std::runtime_error("Invalid Stage 5 preview scene");
        if(stage5_preview)return;
        auto intro=bank.stage5_art(assets_root,StageSprites::Stage5Art::Intro);
        auto winged=bank.stage5_art(assets_root,StageSprites::Stage5Art::Winged);
        auto final=bank.stage5_art(assets_root,StageSprites::Stage5Art::Final);
        auto text=load_dialogue(4);
        auto portrait=DialogueRenderer::load(assets_root);
        stage5_intro_art=std::move(intro);stage5_winged_art=std::move(winged);stage5_final_art=std::move(final);
        dialogue=std::move(text);dialogue_art=std::move(portrait);
        mima_palette0=background.palette[0];mima_draws.reserve(820);
        preboss_enabled=stage5_preview=true;stage5_entry_only=false;
    }
    void bind_mima_battle(){
        if(stage!=4||!stage5_preview||!runtime.scroll.done||runtime.blocked||!dialogue.preboss_complete()||dialogue.active())
            throw std::runtime_error("Invalid Mima battle handoff");
        mima=MimaCore{};mima_enabled=true;
        for(auto &enemy:runtime.enemies.slots)if(enemy.raw[0xe]==1)enemy.raw[0xe]=2;
        death_patterns.fill(std::nullopt);
        if(combat_enabled)shots.slots={}; // sub_D376 clears live shot slots.
        transition.phase=StageTransition::Phase::Battle;
    }
    void start_mima_defeat(){
        const bool final=mima.event==MimaCore::Event::FinalDefeated;
        if(!final){
            if(combat_enabled)shots.slots={};
            if(!practice_mode&&effects_enabled){
                unsigned stock=life.lives;score_core.grant_dialogue_bonus(stock);life.lives=std::uint8_t(stock);lives=int(stock);
            }
            mima_tone=100; // dialog_pre restores the normal palette tone.
        }else{mima_music=false;mima_music_stopped=true;}
        dialogue_snapshot.resize(640*400);render(dialogue_snapshot);
        dialogue.start_stage5_defeat(continues_used,final);
        transition.phase=StageTransition::Phase::Postboss;
    }
    void complete_mima_dialogue(){
        MimaCore::Hooks h;h.random8=[&]{return unsigned(runtime.random.next8());};
        const auto result=mima.event==MimaCore::Event::Form1Defeated?mima.resolve_form1(h,continues_used):mima.resolve_final(continues_used);
        if(result.ending&&practice_mode){
            practice_completed=true;
        }else if(result.ending){
            score_core.score+=result.bonus;
            if(score_core.score>score_core.hiscore)score_core.hiscore=score_core.score;
            highest_run_score=std::max(highest_run_score,score_core.score);
            stage5_result=result;
        }else if(result.enter_form2){
            mima_draws.clear();dialogue_snapshot.clear();transition.phase=StageTransition::Phase::Battle;
        }
    }
    void force_mima_sound(unsigned id){++sound_requests;if(auto play=sound.force(id);play&&sound_output)sound_output(*play);}
    void tick_mima_delay(){
        update_sound();
        ++mima_delay_frame;
        if(mima.event==MimaCore::Event::FinalDefeated&&
            ((dialogue.current==54&&mima_delay_frame==1)||(dialogue.current==55&&mima_delay_frame==31)||
             (dialogue.current==57&&(mima_delay_frame==1||mima_delay_frame==21||mima_delay_frame==41))))force_mima_sound(5);
        if(mima_delay_frame>=mima_delay){mima_delay=mima_delay_frame=0;if(!dialogue.active())complete_mima_dialogue();}
    }
    void enable_preboss(){
        if(stage>3)throw std::runtime_error("Unsupported preboss stage");
        if(stage==3){
            stage4_intro_art=bank.stage4_art(assets_root,StageSprites::Stage4Art::MimaIntro);
            stage4_boss_art=bank.stage4_art(assets_root,StageSprites::Stage4Art::Marisa);
        }
        dialogue=load_dialogue(unsigned(stage));dialogue_art=DialogueRenderer::load(assets_root);preboss_enabled=true;
    }
    void start_preboss(){
        if(!preboss_enabled||preboss_started||!runtime.scroll.done||rika_enabled||meira_enabled||runtime.blocked||weapon_blocked||(survival_enabled&&life.game_over))return;
        // Capture existing sprites/bullets/terrain once; blocking source dialog
        // does not advance gameplay or re-render simulation underneath the box.
        if(!preparation_only){
            dialogue_snapshot.resize(640*400);
            if(stage5_preview||sigma_art){
                // th02_main.asm:22396–22475 clears both pages and redraws the player.
                scroll.line=runtime.scroll.line=0;
                std::fill(dialogue_snapshot.begin(),dialogue_snapshot.end(),stage5_rgb(background.palette[0]));
                draw_mima_dialogue_player(dialogue_snapshot);
            }else render(dialogue_snapshot);
        }
        if(effects_enabled&&!(practice_mode&&practice_show_dialogue)){
            unsigned stock=life.lives;score_core.grant_dialogue_bonus(stock);life.lives=std::uint8_t(stock);lives=int(stock);
        }
        rika_handoff_context.phase_frame=midboss.phase_frame;rika_handoff_context.scroll_line=unsigned(runtime.scroll.line);
        meira_handoff.phase_frame=midboss2.phase_frame;
        dialogue.continues_used=continues_used;
        preboss_started=true;dialogue.start_preboss();
    }
    void enable_midboss(){if(stage!=0)throw std::runtime_error("Only stage1 midboss supported");midboss_enabled=true;}
    void enable_bombing(){bomb.init(static_cast<unsigned>(player.shot));bomb_art=BombRenderer::load(assets_root,static_cast<unsigned>(player.shot));bomb_enabled=true;}
    void enable_effects(std::uint32_t seed=1234){
        // Reproducible development init: IRand seeds64 spark defaults BEFORE
        // filling ring; item init then consumes one ring byte. Other original
        // initialization users/clock seed are NOT yet modeled, no parity claim.
        IRand rng(seed);sparks.initialize(rng,false);runtime.random.fill(rng);items.initialize(runtime.random);effects_enabled=true;
    }
    void tick(std::uint16_t input,bool dialogue_group_key=false,bool dialogue_fast_forward=false){
        if(paused)return;
        if(hitbox_mode_!=HitboxMode::Off)invalidate_hitboxes();
        if(practice_completed)return;
        if(extra_settlement.terminal())return;
        if(extra_settlement.showing_bonus()||extra_settlement.phase==ExtraSettlement::Phase::GameOver){extra_settlement.tick(dialogue_group_key);return;}
        if(sigma_art&&life.game_over&&!sigma.done){extra_settlement.lose(dialogue_group_key);return;}
        if(life.game_over&&transitions_enabled&&!sigma_art&&survival_enabled&&!practice_mode){
            update_continue(input,dialogue_group_key);return;
        }
        if(practice_mode&&!practice_show_dialogue&&!preparation_only&&((rika_enabled&&rika.done)||(meira_enabled&&meira.done))){practice_completed=true;return;}
        if(transitions_enabled&&stage==2&&!midboss3_enabled&&runtime.scroll.progress>=0x67){stage3_preview_boundary=true;return;}
        if(transitions_enabled&&stage==1&&!midboss2_enabled&&!meira_enabled&&runtime.scroll.progress>=0x50){stage2_preview_boundary=true;return;}
        if(transitions_enabled&&transition.active()&&transition.phase!=StageTransition::Phase::Postboss){transition.tick(dialogue_group_key);return;}
        if(stage5_entry_only||stage5_result)return;
        if(mima_delay){tick_mima_delay();return;}
        if(stage==3&&runtime.scroll.done&&!runtime.blocked&&!preboss_enabled&&!practice_mode&&!weapon_blocked&&!life.game_over)enable_preboss();
        if(preboss_enabled&&dialogue.active()){
            if(dialogue.phase==StageDialogue::Phase::IntroAppearance){
                if(stage==3){
                    update_sound();
                    // The first interlude waits 10 updates, then runs the 150-step
                    // reveal and ten 3-update tone steps (ASM 22269–22389, 22494–22498).
                    const bool marisa_intro=dialogue.current==StageDialogue::stage4_generic_end;
                    if(++appearance_frame==11&&!marisa_intro)request_sound(9);
                    if(appearance_frame>=(marisa_intro?10u:190u)){
                        if(marisa_intro){marisa_music=true;request_sound(10);}
                        dialogue.continue_after_appearance();appearance_frame=0;
                    }
                }else if(stage==4){
                    update_sound();
                    const bool winged=dialogue.current==StageDialogue::stage5_winged;
                    ++appearance_frame;
                    if(!winged&&appearance_frame==11)request_sound(9);
                    if(winged&&appearance_frame==18){request_sound(10);update_sound();}
                    // White out: 1 + 17 VSYNCs. White in: 1 + 17*3 VSYNCs.
                    if(appearance_frame>=(winged?70u:190u)){
                        if(winged)mima_music=true;
                        dialogue.continue_after_appearance();appearance_frame=0;
                    }
                }else if(stage==5){
                    update_sound();
                    if(++appearance_frame==StageDialogue::extra_appearance_peak){
                        bullet_core.clear(runtime.page);shots.slots={};sigma_music=true;
                    }
                    if(appearance_frame>=StageDialogue::extra_appearance_end){dialogue.continue_after_appearance();appearance_frame=0;}
                }else{++appearance_frame;if(appearance_frame==16)meira_appearance=true;if(appearance_frame>=32)dialogue.continue_after_appearance();}
                return;
            }
            if(stage==4){
                update_sound();const unsigned before=dialogue.current;
                const bool complete=dialogue.tick(input!=0,dialogue_group_key,dialogue_fast_forward);
                if(mima.event==MimaCore::Event::FinalDefeated&&dialogue.current!=before){
                    // dialog.cpp:551–587: five flashes, with 10/30/20/20/20/20/20 VSYNC delays.
                    constexpr std::array<unsigned,4> delays={12,52,20,118};
                    mima_delay=delays.at(dialogue.current-54);
                    mima_delay_frame=0;
                }else if(complete){
                    if(!dialogue.postboss)bind_mima_battle();
                    else if(mima.event==MimaCore::Event::Form1Defeated&&continues_used){mima_music=false;mima_music_stopped=true;mima_delay=221;mima_delay_frame=0;}
                    else complete_mima_dialogue();
                }
                return;
            }
            if(dialogue.tick(input!=0,dialogue_group_key,dialogue_fast_forward)){
                if(dialogue.postboss&&practice_mode){
                    practice_completed=true;
                }else if(stage==5){
                    if(dialogue.postboss){
                        const auto b=ExtraBonus::calculate(life.stage_misses,bomb.stage_used,sigma.sigma_frame,pickups.points_collected);
                        if(extra_settlement.begin(b,dialogue_group_key)){
                            // The EX overlay uses the same bonus grant as the original:
                            // add the pending delta, process an extend, then freeze scoring.
                            score_core.add(b.internal_bonus);
                            unsigned stock=life.lives;
                            score_core.grant_dialogue_bonus(stock);
                            life.lives=std::uint8_t(stock);lives=int(stock);
                        }
                    }else{sigma.init();sigma_enabled=true;for(auto &e:runtime.enemies.slots)e.raw[0xe]=0;death_patterns.fill(std::nullopt);}
                }else if(dialogue.postboss)finish_postboss(dialogue_group_key);
                else if(stage==3)bind_marisa_battle();
                else if(stage==2)bind_stones_battle();
                else if(stage==1)bind_meira_battle();else{bind_rika_battle(rika_handoff_context);normal_rika_handoff=true;}
            }
            return;
        }
        if(transitions_enabled&&marisa_enabled&&marisa.cleared&&!life.game_over&&transition.phase==StageTransition::Phase::Battle){start_postboss();return;}
        if(transitions_enabled&&stones_enabled&&stones.done()&&!life.game_over&&transition.phase==StageTransition::Phase::Battle){start_postboss();return;}
        if(transitions_enabled&&meira_enabled&&meira.done&&!life.game_over&&transition.phase==StageTransition::Phase::Battle){start_postboss();return;}
        if(transitions_enabled&&rika_enabled&&rika.done&&!life.game_over&&transition.phase==StageTransition::Phase::Battle){start_postboss();return;}
        if(preboss_enabled&&runtime.scroll.done&&!rika_enabled&&!meira_enabled&&!preboss_started){start_preboss();return;}
        if(sigma_enabled&&sigma.done){if(practice_mode&&!practice_show_dialogue)practice_completed=true;else start_postboss();return;}
        if(paused||runtime.blocked||(runtime.scroll.done&&!rika_enabled&&!meira_enabled&&!stones_enabled&&!marisa_enabled&&!mima_enabled&&!sigma_enabled)|| (marisa_enabled&&marisa.cleared)||(stones_enabled&&stones.done())||(rika_enabled&&rika.done)||(meira_enabled&&meira.done)||(survival_enabled&&life.game_over)||weapon_blocked||mima.blocked_reason()||sigma.blocked)return;
        if(mima_enabled&&mima.event!=MimaCore::Event::None){start_mima_defeat();return;}
        update_sound();
        const std::uint32_t logic_frame=rika_enabled?rika_stage_frame:static_cast<std::uint32_t>(ticks);
        const auto page=runtime.page;
        auto *display=hitbox_mode_!=HitboxMode::Off&&hitbox_battle_visible()?&collision_frame_:nullptr;
        if(display)display->begin(hitbox_mode_,logic_frame,page);
        rendered_page=page;bullet_core.begin_frame(page);
        if(midboss_enabled)midboss.begin_frame(page);
        if(midboss2_enabled)midboss2.begin_frame(page);
        if(midboss3_enabled)midboss3.begin_frame(page);
        if(meira_enabled)meira.begin_frame(page);
        if(rika_enabled)rika.begin_frame(page);
        if(sigma_enabled)sigma.begin_frame(page);
        if(rika_enabled||stones_enabled||stage==3||sigma_enabled)lasers.begin_frame(logic_frame);
        const bool suppress_midboss_formations=(midboss_enabled&&midboss.active)||(midboss2_enabled&&midboss2.active)||(midboss3_enabled&&midboss3.active);
        const unsigned midboss_progress=runtime.scroll.progress;
        if(stage==3)midboss4.begin_frame(page);
        const bool extra_road_frame=extra_road_enabled&&!runtime.scroll.done;
        if(!marisa_enabled&&(midboss_enabled||midboss2_enabled||midboss3_enabled||stage==3||extra_road_frame)){runtime.prepare_frame(script,suppress_midboss_formations||midboss4.active||(extra_road_frame&&extra_midboss.active));if(runtime.blocked)return;}
        if(extra_road_frame)extra_midboss.trigger(midboss_progress);
        if(stage==3)midboss4.trigger(midboss_progress);
        // Source formation dispatch precedes the trigger equality check.
        if(midboss_enabled)midboss.trigger(runtime.scroll.progress);
        if(midboss2_enabled)midboss2.trigger(runtime.scroll.progress);
        if(midboss3_enabled)midboss3.trigger(runtime.scroll.progress);
        if(combat_enabled)shots.begin_frame(page);
        if(effects_enabled){items.begin_frame(page);numbers.begin_frame(page);sparks.begin_frame(page);}
        for(auto &pattern:death_patterns)pattern.reset();
        bullet_core.playperf=life.playperf;
        EnemyEffectSink sink;
        sink.bullet=[&](auto request){return bullet_core.add(request,page,player.x,player.y,runtime.random,bomb_enabled&&bomb.active);};
        sink.sound=[&](unsigned id){request_sound(id);return true;};
        const char *tile_error=nullptr;
        if(extra_road_frame)sink.tile=[&](const EnemyTileRequest &r){
            if((tile_error=extra_tiles.write_error(background,r.left,r.vram_y,r.image)))return false;
            extra_tiles.set_and_put_both(background,r.left,r.vram_y,r.image);return true;
        };
        // Original invalidation carries pages, but does NOT refresh cached
        // player_topleft while a miss blocks input. Preserve that phase lag.
        bool accepts_input=true;
        if(survival_enabled){life.begin_frame(page);accepts_input=life.input_allowed();if(accepts_input){player.x=life.positions[page].x;player.y=life.positions[page].y;}}
        player.back_page=page;
        if(accepts_input){player.tick(input);if(survival_enabled)life.moved(page,player.x,player.y);}
        if(survival_enabled){player.x=life.current.x;player.y=life.current.y;}
        EnemyPathRuntime::CombatHooks combat;
        if(combat_enabled){
            shots.random=&runtime.random;shots.shot_type=static_cast<unsigned>(player.shot);shots.level=pickup_enabled?pickups.level():0;
            if(accepts_input){shots.option_left=player.x-16-player.last_dx;shots.option_top=player.y+16-player.last_dy;shots.input((input&Shot)!=0,page,player.x,player.y,input);}
            shots.target_x=-1;shots.target_y=-1;
            shots.random=nullptr;
            if(bomb_enabled){
                if(accepts_input&&(input&Bomb)&&bomb.start(life.bombs,static_cast<unsigned>(player.shot)))life.bomb_invincible=true;
                if(bomb.clear_requested){bullet_core.clear(page);bomb.clear_requested=false;}
                bombing_for_spawn=bomb.active;
                bomb.update(runtime.random);
                if(bomb.clear_requested){bullet_core.clear(page);bomb.clear_requested=false;}
                if(bomb.finished){life.bomb_invincible=false;life.invincibility=70;bombing_for_spawn=false;}
                if(effects_enabled)while(consumed_bomb_sparks<bomb.spark_requests.size()){const auto &r=bomb.spark_requests[consumed_bomb_sparks++];sparks.add(r.x,r.y,r.speed,r.count,r.sprite,page);}
            }
            while(consumed_shot_sounds<shots.sound_requests){++consumed_shot_sounds;request_sound(1);}
            while(consumed_bomb_sounds<bomb.sounds.size())request_sound(bomb.sounds[consumed_bomb_sounds++]);
            combat.damage=[&](unsigned,auto &enemy,const auto &type){const int left=enemy_s16(enemy.word(page*4)),top=enemy_s16(enemy.word(page*4+2));const auto hp=unsigned(type.raw[22])|(unsigned(type.raw[23])<<8);if(hp==65535)return false;
                if(top<360&&top>shots.target_y-8&&top>0){shots.target_x=left+8;shots.target_y=top+8;}
                if(survival_enabled&&!type.raw[24]&&!enemy.raw[0x23]){
                    const auto bounds=CollisionBounds::open(left-16,top-16,left+16,top+16);
                    const CollisionPoint point{life.positions[page^1].x,player.y};
                    if(display)display->hazard(CollisionSource::Road,bounds,CollisionSample::FrontXCurrentY,point);
                    if(bounds.contains(point)){enemy.word(0xa,0);enemy.raw[0x10]=1;life.register_hit();}
                }
                auto dealt=shots.damage(page,left,top,32,32,[&](const auto &s){if(effects_enabled)sparks.add(s.left,s.top,int(s.speed),int(s.count),s.as_sprite,page);});if(bomb_enabled)dealt+=bomb.damage(static_cast<std::uint32_t>(ticks));if(effects_enabled)score_core.add_hit_lowword(static_cast<std::uint16_t>(dealt));if(auto killed=damage.apply(enemy,type,dealt,page)){request_sound(3);kills_recorded.push_back(*killed);if(effects_enabled){if(killed->drop>1)items.add(left+12,top,killed->drop-2,life.lives);else items.semirandom(left+12,top,runtime.random,life.lives);score_core.add(killed->score);}if(rank==3){EnemyBulletRequest revenge;const auto &r=type.raw;revenge.left=enemy_s16(unsigned(left)+(unsigned(r[4])|(unsigned(r[5])<<8)));revenge.top=enemy_s16(unsigned(top)+(unsigned(r[6])|(unsigned(r[7])<<8)));revenge.group=0x19;revenge.speed=60;bullet_core.add(revenge,page,player.x,player.y,runtime.random,bombing_for_spawn);}}else if(dealt&&!enemy.raw[0x22])request_sound(4);return dealt>0&&hp!=65534;};
            combat.death=[&](unsigned slot,auto &enemy,const auto &type){const auto pattern=EnemyDamage::death_pattern(enemy,type);if(effects_enabled&&enemy.word(0xa)==0)sparks.add(enemy_s16(enemy.word(page*4))+16,enemy_s16(enemy.word(page*4+2))+16,20,8,false,page);EnemyDamage::death_tick(enemy);if(enemy.raw[0xe]==1)death_patterns[slot]=pattern;};
        }
        combat.suppress_formations=suppress_midboss_formations;
        const auto scroll_attempt_before=runtime.scroll.attempted_pixels;
        if(!rika_enabled&&!meira_enabled&&!stones_enabled&&!marisa_enabled&&!mima_enabled&&!sigma_enabled)runtime.tick(script,player.x,player.y,&sink,combat_enabled?&combat:nullptr,extra_road_frame);
        else {runtime.page^=1;++runtime.ticks;}
        if(!extra_road_frame)scroll=runtime.scroll;
        encounter_observer=runtime.dispatch;observed_requests=runtime.spawns;
        if(runtime.blocked){if(tile_error)runtime.blocked->result.reason=tile_error;return;}
        if(extra_road_frame)extra_midboss.update(runtime.scroll.line,sink);
        if(midboss_enabled&&midboss.active){
            Midboss1Core::Hooks h;
            h.bullet=[&](const auto &r){if(!bullet_core.add(r,page,player.x,player.y,runtime.random,bomb_enabled&&bomb.active))throw std::runtime_error("Unsupported midboss bullet");};
            h.damage=[&](int x,int y,int w,int height){auto value=shots.damage(page,x,y,w,height,[&](const auto &s){if(effects_enabled)sparks.add(s.left,s.top,int(s.speed),int(s.count),s.as_sprite,page);});if(bomb_enabled)value+=bomb.damage(static_cast<std::uint32_t>(ticks));if(effects_enabled)score_core.add_hit_lowword(static_cast<std::uint16_t>(value));return value;};
            h.score=[&](unsigned value){if(effects_enabled)score_core.add(value);};h.sound=[&](unsigned id){request_sound(id);};
            h.spark=[&](const auto &s){if(effects_enabled)sparks.add(s.x,s.y,s.speed,s.count,s.sprite,page);};
            h.tile=[&](int x,int vram_y,unsigned image){if(image>=background.tiles.size())throw std::runtime_error("Midboss track tile out of range");const unsigned screen_y=unsigned((vram_y-int(scroll.line)+400)%400);const unsigned virtual_row=(383-screen_y+scroll.display_pixels)/16;track_tiles[virtual_row*24+unsigned((x-32)/16)]=image;};
            midboss.update(page,scroll.line,midboss_progress,h);shots.target_x=midboss.target_x;shots.target_y=midboss.target_y;
        }
        if(midboss2_enabled&&midboss2.active){
            Midboss2Core::Hooks h;
            h.bullet=[&](const auto &r){if(!bullet_core.add(r,page,player.x,player.y,runtime.random,bomb_enabled&&bomb.active))throw std::runtime_error("Unsupported midboss2 bullet");};
            h.damage=[&](int x,int y,int w,int height){unsigned value=shots.damage(page,x,y,w,height,[&](const auto&s){if(effects_enabled)sparks.add(s.left,s.top,int(s.speed),int(s.count),s.as_sprite,page);});if(bomb_enabled)value+=bomb.damage(logic_frame);if(effects_enabled)score_core.add_hit_lowword(std::uint16_t(value));return value;};
            h.score=[&](unsigned n){if(effects_enabled)score_core.add(n);};h.sound=[&](unsigned n){request_sound(n);};h.spark=[&](const auto&s){if(effects_enabled)sparks.add(s.x,s.y,s.speed,s.count,s.sprite,page);};
            midboss2.update(page,scroll.line,h);shots.target_x=midboss2.target_x;shots.target_y=midboss2.target_y;
        }
        if(midboss3_enabled&&midboss3.active){
            Midboss3Core::Hooks h;
            h.bullet=[&](const auto &r){if(!bullet_core.add(r,page,player.x,player.y,runtime.random,bomb_enabled&&bomb.active))throw std::runtime_error("Unsupported midboss3 bullet");};
            h.damage=[&](int x,int y,int w,int height){unsigned value=shots.damage(page,x,y,w,height,[&](const auto &p){if(effects_enabled)sparks.add(p.left,p.top,int(p.speed),int(p.count),p.as_sprite,page);});if(bomb_enabled)value+=bomb.damage(logic_frame);if(effects_enabled)score_core.add_hit_lowword(std::uint16_t(value));return value;};
            h.score=[&](unsigned n){if(effects_enabled)score_core.add(n);};h.sound=[&](unsigned n){request_sound(n);};h.spark=[&](const auto &p){if(effects_enabled)sparks.add(p.x,p.y,p.speed,p.count,p.sprite,page);};
            midboss3.update(page,scroll.line,runtime.scroll.attempted_pixels-scroll_attempt_before,unsigned(rank),h);shots.target_x=midboss3.target_x;shots.target_y=midboss3.target_y;
        }
        if(marisa_enabled){
            MarisaCore::Hooks h;h.collision_display=display;
            h.bullet=[&](const auto &r){if(!bullet_core.add(r,page,player.x,player.y,runtime.random,bomb_enabled&&bomb.active))throw std::runtime_error("Unsupported Marisa bullet");};
            const auto damage_query=[&](int x,int y,int w,int height,unsigned mask){
                if(!combat_enabled)return 0u;
                auto n=shots.damage(page,x,y,w,height,[&](const auto &p){if(effects_enabled)sparks.add(p.left,p.top,p.speed,p.count,p.as_sprite,page);});
                if(bomb_enabled)n+=bomb.damage(logic_frame,mask);
                if(effects_enabled)score_core.add_hit_lowword(std::uint16_t(n));
                return n;
            };
            h.damage=[&](int x,int y,int w,int height){return damage_query(x,y,w,height,1);};
            h.orb_damage=[&](int x,int y,int w,int height){return damage_query(x,y,w,height,3);};
            h.random8=[&]{return unsigned(runtime.random.next8());};
            h.random16=[&]{return unsigned(runtime.random.next16());};
            h.drift=[&](unsigned angle,unsigned speed,unsigned frames){bullet_core.special={int(angle),int(speed),int(frames)};};
            h.item=[&](int x,int y,unsigned type){if(effects_enabled)items.add(x,y,type,life.lives);};
            h.sound=[&](unsigned n){request_sound(n);};
            h.score=[&](unsigned n){if(effects_enabled)score_core.add(n);};
            h.spark=[&](const auto &p){if(effects_enabled)sparks.add(p.x,p.y,p.speed,p.count,p.sprite,page);};
            h.player_hit=[&]{if(survival_enabled)life.register_hit();};
            h.invincibility=[&](unsigned n){life.invincibility=std::uint8_t(n);};
            const auto front=life.positions[page^1];
            marisa.update(h,{survival_enabled?front.x:player.x,survival_enabled?front.y:player.y,pickups.power,player.x,unsigned(rank)});
            shots.target_x=marisa.target_x;shots.target_y=marisa.target_y;
        }
        if(sigma_enabled){
            SigmaCore::Hooks h;h.collision_display=display;
            h.bullet=[&](const auto &r){if(!sink.bullet(r))sigma.blocked="sigma: unsupported projectile";};
            h.damage=[&](int x,int y,int w,int height){
                if(!combat_enabled)return 0u;
                auto n=shots.damage(page,x,y,w,height,[&](const auto &p){if(effects_enabled)sparks.add(p.left,p.top,p.speed,p.count,p.as_sprite,page);});
                if(bomb_enabled)n+=bomb.damage(logic_frame);
                if(effects_enabled)score_core.add_hit_lowword(std::uint16_t(n));
                return n;
            };
            h.score=[&](unsigned n){score_core.score+=n;};
            h.sound=[&](unsigned n){request_sound(n);};
            h.turns_max=[&](unsigned n){bullet_core.special.u3=int(n);};
            h.randring2_next=[&]{return unsigned(runtime.random.next8());};
            h.player_hit=[&]{if(survival_enabled)life.register_hit();};
            h.laser=[&](int x,int y,int frames,unsigned pat,unsigned delay){lasers.initial_delay=delay;lasers.add(x,y,frames,pat,[&](unsigned id){request_sound(id);});};
            h.player_x=player.x;h.player_y=player.y;
            const auto front=life.positions[page^1];h.front_player_x=survival_enabled?front.x:player.x;h.front_player_y=survival_enabled?front.y:player.y;
            h.reduced=reduced_effects;
            sigma.update(page,logic_frame,h);
            shots.target_x=sigma.left[page]+60;shots.target_y=sigma.top[page]+60;
            if(sigma.done){if(practice_mode&&!practice_show_dialogue)practice_completed=true;else start_postboss();return;}
        }
        if(mima_enabled){
            MimaCore::Hooks h;h.collision_display=display;
            h.bullet=[&](const auto &r){if(!bullet_core.add(r,page,player.x,player.y,runtime.random,bomb_enabled&&bomb.active))throw std::runtime_error("Unsupported Mima bullet");};
            h.damage=[&](int x,int y,int w,int height){
                if(!combat_enabled)return 0u;
                auto n=shots.damage(page,x,y,w,height,[&](const auto &p){if(effects_enabled)sparks.add(p.left,p.top,p.speed,p.count,p.as_sprite,page);});
                if(bomb_enabled)n+=bomb.damage(logic_frame);
                if(effects_enabled)score_core.add_hit_lowword(std::uint16_t(n));
                return n;
            };
            h.random8=[&]{return unsigned(runtime.random.next8());};
            h.item=[&](int x,int y,unsigned type){if(effects_enabled)items.add(x,y,type,life.lives);};
            h.sound=[&](unsigned id){request_sound(id);};
            h.score=[&](unsigned n){score_core.score+=n;};
            h.player_hit=[&]{if(survival_enabled)life.register_hit();};
            h.clear_bullets=[&]{bullet_core.clear(page);};
            h.invincibility=[&](unsigned n){life.invincibility=std::uint8_t(n);};
            h.stack_multiplier=[&](unsigned n){bullet_core.stack=(rank==2||rank==3?1u:0u)*n;};
            h.turns_max=[&](unsigned n){bullet_core.special.u3=int(n);};
            h.dot=[&](int x,int y,unsigned size,unsigned c){mima_draws.push_back({MimaPaint::Dot,x,y,int(size),0,c});};
            h.line=[&](int x,int y,int x2,int y2,unsigned c){mima_draws.push_back({MimaPaint::Line,x,y,x2,y2,c});};
            h.circle=[&](int x,int y,unsigned r,unsigned c){mima_draws.push_back({MimaPaint::Circle,x,y,int(r),0,c});};
            h.fill_circle=[&](int x,int y,unsigned r,unsigned c){mima_draws.push_back({MimaPaint::Fill,x,y,int(r),0,c});};
            h.palette0=[&](unsigned r,unsigned g,unsigned b){mima_palette0={std::uint8_t(r),std::uint8_t(g),std::uint8_t(b)};};
            h.tone=[&](unsigned n){mima_tone=n;};
            mima_draws.clear();mima.render_background(h,reduced_effects);
            const auto front=life.positions[page^1];
            mima.update(h,{survival_enabled?front.x:player.x,survival_enabled?front.y:player.y,player.x,player.y,unsigned(rank),logic_frame,reduced_effects,page});
            shots.target_x=mima.target_x;shots.target_y=mima.target_y;
        }
        if(stage==3&&!marisa_enabled&&midboss4.active){
            Midboss4Core::Hooks h;
            h.bullet=[&](const auto &r){if(!bullet_core.add(r,page,player.x,player.y,runtime.random,bomb_enabled&&bomb.active))throw std::runtime_error("Unsupported midboss4 bullet");};
            h.damage=[&](int x,int y,int w,int height){
                if(!combat_enabled)return 0u;
                auto n=shots.damage(page,x,y,w,height,[&](const auto &p){if(effects_enabled)sparks.add(p.left,p.top,p.speed,p.count,p.as_sprite,page);});
                if(bomb_enabled)n+=bomb.damage(logic_frame);
                if(effects_enabled)score_core.add_hit_lowword(std::uint16_t(n));
                return n;
            };
            h.score=[&](unsigned n){if(effects_enabled)score_core.add(n);};
            h.sound=[&](unsigned n){request_sound(n);};
            h.spark=[&](const auto &p){if(effects_enabled)sparks.add(p.x,p.y,p.speed,p.count,p.sprite,page);};
            h.item=[&](int x,int y,unsigned type){if(effects_enabled)items.add(x,y,type,life.lives);};
            h.random8=[&]{return unsigned(runtime.random.next8());};
            h.laser=[&](int x,int y,unsigned frames,unsigned pat,unsigned delay){lasers.initial_delay=delay;lasers.add(x,y,int(frames),pat,[&](unsigned id){request_sound(id);});};
            midboss4.update(page,scroll.line,unsigned(rank),h);
            shots.target_x=midboss4.target_x;shots.target_y=midboss4.target_y;
        }
        if(stones_enabled){
            StonesCore::Hooks h;
            h.bullet=[&](const auto &r){if(!bullet_core.add(r,page,player.x,player.y,runtime.random,bomb_enabled&&bomb.active))throw std::runtime_error("Unsupported Stones bullet");};
            h.damage=[&](int x,int y,int w,int height){
                if(!combat_enabled)return 0u;
                unsigned n=shots.damage(page,x,y,w,height,[&](const auto &p){if(effects_enabled)sparks.add(p.left,p.top,p.speed,p.count,p.as_sprite,page);});
                if(bomb_enabled)n+=bomb.damage(logic_frame);
                if(effects_enabled)score_core.add_hit_lowword(std::uint16_t(n));
                return n;
            };
            h.score=[&](unsigned n){if(effects_enabled)score_core.add(n);};
            h.sound=[&](unsigned id){request_sound(id);};
            h.spark=[&](const auto &p){if(effects_enabled)sparks.add(p.x,p.y,p.speed,p.count,p.sprite,page);};
            h.item=[&](int x,int y,unsigned type){if(effects_enabled)items.add(x,y,type,life.lives);};
            h.invincibility=[&](unsigned n){life.invincibility=std::uint8_t(n);};
            h.random8=[&]{return unsigned(runtime.random.next8());};
            h.laser=[&](int x,int y,int frames,unsigned pat,unsigned delay){lasers.initial_delay=delay;lasers.add(x,y,frames,pat,[&](unsigned id){request_sound(id);});};
            h.bullet_stack_multiplier=[&](unsigned n){bullet_core.stack=(rank==2||rank==3?1u:0u)*n;};
            h.stripe_tile=[&](int x,int y,unsigned pat){
                if(x<32||x>=416||(x-32)%16||pat>=background.tiles.size())throw std::runtime_error("Invalid Stones stripe tile");
                const int row=((y+int(scroll.line))%400)/16;
                stones_stripe[unsigned(row*24+(x-32)/16)]=pat;
            };
            stones.update(unsigned(rank),player.x,player.y,h);
            shots.target_x=stones.target_x;shots.target_y=stones.target_y;
        }
        if(meira_enabled){
            MeiraCore::Hooks h;h.collision_display=display;
            h.bullet=[&](const auto&r){if(!bullet_core.add(r,page,player.x,player.y,runtime.random,bomb_enabled&&bomb.active))throw std::runtime_error("Unsupported Meira bullet");};
            h.damage=[&](int x,int y,int w,int height){unsigned v=shots.damage(page,x,y,w,height,[&](const auto&s){if(effects_enabled)sparks.add(s.left,s.top,int(s.speed),int(s.count),s.as_sprite,page);});if(bomb_enabled)v+=bomb.damage(logic_frame);if(effects_enabled)score_core.add_hit_lowword(std::uint16_t(v));return v;};
            h.explosion=[&](int,int,int frame){if(frame==1)request_sound(18);};
            h.hit=[&](){if(survival_enabled)life.register_hit();};h.score=[&](unsigned n){if(effects_enabled)score_core.add(n);};h.sound=[&](unsigned n){request_sound(n);};h.invincibility=[&](unsigned n){life.invincibility=std::uint8_t(n);};
            h.spark=[&](int x,int y,int speed,int count,bool sprite){if(effects_enabled)sparks.add(x,y,speed,count,sprite,page);};
            h.random8=[&](){return unsigned(runtime.random.next8());};h.random16=[&](){return unsigned(runtime.random.next16());};h.special_turns=[&](unsigned n){bullet_core.special.u3=int(n);};
            meira.update(page,player.x,player.y,scroll.line,h);shots.target_x=meira.target_x;shots.target_y=meira.target_y;
        }
        if(rika_enabled){
            RikaCore::Hooks h;
            h.bullet=[&](const auto &r){if(!bullet_core.add(r,page,player.x,player.y,runtime.random,bomb_enabled&&bomb.active))throw std::runtime_error("Unsupported Rika bullet");};
            h.damage=[&](int x,int y,int w,int height){unsigned value=shots.damage(page,x,y,w,height,[&](const auto &s){if(effects_enabled)sparks.add(s.left,s.top,int(s.speed),int(s.count),s.as_sprite,page);});if(bomb_enabled)value+=bomb.damage(logic_frame);if(effects_enabled)score_core.add_hit_lowword(std::uint16_t(value));return value;};
            h.laser=[&](int x,int y,int frames,unsigned pat,unsigned delay){lasers.initial_delay=delay;lasers.add(x,y,frames,pat,[&](unsigned id){request_sound(id);});};
            h.score=[&](unsigned n){if(effects_enabled)score_core.add(n);};h.sound=[&](unsigned id){request_sound(id);};h.invincibility=[&](unsigned n){life.invincibility=std::uint8_t(n);};
            h.spark=[&](int x,int y,int speed,int count,bool sprite){if(effects_enabled)sparks.add(x,y,speed,count,sprite,page);};
            rika.update(page,logic_frame,h);shots.target_x=rika.target_x;shots.target_y=rika.target_y;
        }
        if(survival_enabled)life.player_phase_begin();
        if(combat_enabled)shots.update(page,logic_frame);
        if(survival_enabled){
            const unsigned normal=player.pose==PlayerMotion::LeanLeft?2:player.pose==PlayerMotion::LeanRight?3:0;
            life.player_phase_end(page,normal);player.x=life.current.x;player.y=life.current.y;
            lives=life.lives;bombs=life.bombs;bullet_core.playperf=life.playperf;
        }
        if(effects_enabled){
            // Consume effects in original player-handler order before item step.
            while(consumed_life_effects<life.effects.size()){const auto &effect=life.effects[consumed_life_effects++];
                if(effect.kind==PlayerLife::Sound)request_sound(unsigned(effect.value));
                else if(effect.kind==PlayerLife::Sparks)sparks.add(effect.x,effect.y,effect.value,effect.count,effect.as_sprite,page);
                else if(effect.kind==PlayerLife::ScoreCommit)score_core.commit();
                else if(effect.kind==PlayerLife::MissItems)items.miss_add(effect.x,effect.y,life.current.x,effect.gameover,runtime.random);
                else if(effect.kind==PlayerLife::PowerChanged&&pickup_enabled){constexpr std::array<unsigned,10> reset={1,1,4,8,16,24,32,40,52,64};pickups.power=reset.at(pickups.level());life.power=static_cast<std::uint8_t>(pickups.power);}
            }
            pickups.begin_frame();pickups.item_skill=items.item_skill;numbers.rank=static_cast<unsigned>(rank);
            items.update(page,player.x,player.y,life.hit,[&](unsigned type,int left,int top){
                if(!pickup_enabled)return false;
                unsigned stock=life.lives,bomb_stock=life.bombs;
                pickups.item_skill=items.item_skill;
                pickups.collect(type,left,top,stock,bomb_stock,numbers);items.item_skill=pickups.item_skill;
                life.lives=static_cast<std::uint8_t>(stock);life.bombs=static_cast<std::uint8_t>(bomb_stock);life.power=static_cast<std::uint8_t>(pickups.power);
                return true;
            });
            // Missing upgraded weapons halt AFTER this tick's remaining phases,
            // before any next-frame emission at an unsupported firepower level.
            if(pickup_enabled){score_core.add(pickups.score_for_rank(static_cast<unsigned>(rank)));shots.level=pickups.level();if(!shots.supports_level())weapon_blocked=true;}
            numbers.update(page);
        }
        if((rika_enabled||stones_enabled||stage==3||sigma_enabled)&&lasers.update(logic_frame,player.x,player.y,life.hit,[&](unsigned id){request_sound(id);},display)){++hit_checks;if(survival_enabled)life.register_hit();}
        if(bullet_core.update(page,player.x,player.y,logic_frame,display)){++hit_checks;if(survival_enabled)life.register_hit();}
        if(effects_enabled){sparks.update(page);unsigned stock=life.lives,bomb_stock=life.bombs;score_core.update(stock,bomb_stock);life.lives=static_cast<std::uint8_t>(stock);life.bombs=static_cast<std::uint8_t>(bomb_stock);lives=int(stock);bombs=int(bomb_stock);if(life.game_over)score_core.commit();}
        while(consumed_pickup_sounds<pickups.sounds.size())request_sound(pickups.sounds[consumed_pickup_sounds++]);
        while(observed_extend_sounds<score_core.sound8){++observed_extend_sounds;request_sound(8);}
        if(extra_road_frame){
            runtime.finish_deferred_scroll(static_cast<unsigned>(script.map.size()),[&](bool reload){
                const auto &s=runtime.scroll;
                extra_tiles.scroll_both(background,s.line,unsigned(s.tile_line),s.top_row,reload);
            });
            extra_tiles.flip(page);
            scroll=runtime.scroll;
        }
        ++resident_frame;++ticks;if(rika_enabled)++rika_stage_frame;
        if(survival_enabled){life.advance_playperf(rika_enabled?rika_stage_frame:std::uint32_t(ticks),unsigned(rank));bullet_core.playperf=life.playperf;}
        if(preboss_enabled&&runtime.scroll.done&&!rika_enabled&&!meira_enabled&&!preboss_started)start_preboss();
        if(display&&hitbox_battle_visible())display->publish(this,ticks,sigma_enabled&&sigma.shake);
    }
    static void clear_marisa_playfield(std::span<std::uint32_t> pixels,int bottom){
        for(int y=16;y<bottom;++y)std::fill_n(pixels.begin()+y*640+32,384,0u);
    }
    // master.lib cutline.asm and grcg_line.asm: clip endpoints before using
    // the left-to-right 16-bit fractional line stepper, centered at 0x8000.
    template<class Put>
    static void marisa_orb_line(std::array<int,2> a,std::array<int,2> b,const Put &put){
        const auto outcode=[](const auto &p){return (p[0]<32?1:p[0]>416?2:0)|(p[1]<0?4:p[1]>399?8:0);};
        const auto cut=[&](auto &p,const auto &q){
            if(p[0]<32||p[0]>416){
                const int x=p[0]<32?32:416;
                p[1]=q[1]+(x-q[0])*(p[1]-q[1])/(p[0]-q[0]);p[0]=x;
            }
            if(p[1]<0||p[1]>399){
                const int y=p[1]<0?0:399;
                if(p[1]==q[1])return false;
                p[0]=q[0]+(y-q[1])*(p[0]-q[0])/(p[1]-q[1]);p[1]=y;
            }
            return !outcode(p);
        };
        if((outcode(a)&outcode(b))||!cut(a,b)||!cut(b,a))return;
        if(a[0]>b[0])std::swap(a,b);
        const int dx=b[0]-a[0],dy=std::abs(b[1]-a[1]),sy=b[1]<a[1]?-1:1;
        const int major=std::max(dx,dy),minor=std::min(dx,dy),step=major?(minor<<16)/major:0;
        for(int i=0,frac=0x8000;i<=major;++i){
            put(a[0]+(dx>=dy?i:frac>>16),a[1]+sy*(dy>=dx?i:frac>>16));frac+=step;
        }
    }
    template<class Effects,class Dot,class Tiny>
    static void draw_death_effects(const Effects &effects,unsigned count,const Dot &dot,const Tiny &tiny){
        // sub_FFF8 / sub_1C287. Each caller keeps its own ordered requests
        // and maps screen positions onto its own displayed VRAM page.
        constexpr std::array<unsigned,5> masks={0x00,0x80,0xC0,0xE0,0xF0}; // MAIN.EXE DS:08EE
        for(unsigned i=0;i<count;++i){
            const auto &e=effects[i];
            if(e.frame<30){
                const int radius=240-8*int(e.frame),size=(radius>>6)+1;
                for(unsigned a=0;a<256;a+=2){
                    const auto v=enemy_vector(a,radius);
                    const int x=e.x+7+v[0],y=e.y+v[1];
                    if(x<=24||x>=416||y<=8||y>=384)continue;
                    for(int dy=0;dy<size;++dy)for(int dx=0;dx<8;++dx)
                        if(masks[size]&(0x80u>>dx))dot(x+dx,y,dy,e.frame&1?15u:4u);
                }
            }else if(e.frame>=32&&e.frame<56){
                const unsigned age=e.frame-32;
                const int radius=16*int(age);
                for(unsigned a=age;a<256+age;a+=4){
                    const int x=e.x+enemy_shift8(radius*enemy_sin(a+64));
                    const int y=e.y+enemy_shift8(radius*enemy_sin(a+e.angle));
                    if(x>16&&x<416&&y>0&&y<384)tiny(88+age/6,x,y);
                }
            }
        }
    }
    void draw_marisa(std::span<std::uint32_t> pixels,bool terminal=false,unsigned shift=0)const{
        if(!stage4_boss_art||(marisa.cleared&&!terminal))return;
        const auto &art=*stage4_boss_art;
        const auto put=[&](int x,int y,std::uint32_t rgb){
            if(x>=32&&x<416+int(shift)&&y>=16&&y<384)pixels[y*640+x]=rgb;
        };
        using Paint=StageSprites::Paint;
        const auto sprite=[&](unsigned id,int left,int top,int scale=1,bool tiny=false,Paint paint=Paint::Artwork){
            const auto &image=*art.sprites.at(id);
            for(unsigned y=0;y<image.height;++y)for(unsigned x=0;x<image.width;++x){
                const auto index=image.pixels[y*image.width+x];
                // super_zoom skips color 0; normal/tiny blits use the put mask.
                if(index==(scale>1?0:image.transparent))continue;
                const auto rgb=stage5_rgb(background.palette[StageSprites::paint_index(paint,index)]);
                for(int dy=0;dy<scale;++dy)for(int dx=0;dx<scale;++dx){
                    const int px=left+int(x)*scale+dx,py=top+int(y)*scale+dy;
                    if(tiny)put(px,py%400,rgb); // Marisa resets the VRAM scroll origin to 0.
                    else if(px<=416)put(px,py,rgb);
                }
            }
        };
        // Fresh frame, not EGC erasure of the previous page.
        draw_death_effects(marisa.death_effects,marisa.death_effect_count,
            [&](int x,int y,int dy,unsigned index){put(x,y+dy,stage5_rgb(background.palette[index]));},
            [&](unsigned id,int x,int y){sprite(id,x,y,1,true);});
        if(terminal||marisa.death_zoom)sprite(marisa.death_pattern,marisa.left,marisa.top,3);
        else{
            // Opening dialogue and defeat draw ordinary cels, not the live latch.
            const auto paint=marisa.body_flash&&!marisa.defeated&&!dialogue.active()?Paint::Color4Hit:Paint::Artwork;
            sprite(marisa.pattern,marisa.left,marisa.top,1,false,paint);
            sprite(marisa.pattern+1,marisa.left+48,marisa.top,1,false,paint);
        }
        if(terminal)return;
        for(const auto &o:marisa.orbs)if(o.active&&o.dying)sprite(o.death_pattern,o.x,o.y);
        std::array<std::array<int,2>,4> live{};unsigned count=0;
        for(const auto &o:marisa.orbs)if(o.active&&!o.dying)live[count++]={o.x+16,o.y+16};
        const auto line_rgb=stage5_rgb(background.palette[3]);
        for(unsigned i=0;i<count;++i)
            marisa_orb_line(live[i],live[(i+1)%count],[&](int x,int y){put(x,y,line_rgb);});
        for(unsigned i=0;i<marisa.orbs.size();++i){const auto &o=marisa.orbs[i];
            if(o.active&&!o.dying&&(!o.hit_flash||(o.x>0&&o.x<416&&o.y>=0&&o.y<=368)))
                sprite(138+i,o.x,o.y,1,false,o.hit_flash?Paint::AllPlanesHit:Paint::Artwork);
        }
    }
    static std::uint32_t stage5_rgb(const std::array<std::uint8_t,3> &c){
        return (unsigned(c[0])<<16)|(unsigned(c[1])<<8)|c[2];
    }
    static void paint_mima(std::span<std::uint32_t> pixels,const MimaDraw &d,std::uint32_t color,int bottom=384){
        const auto put=[&](int x,int y){if(x>=32&&x<416&&y>=16&&y<bottom)pixels[y*640+x]=color;};
        if(d.kind==MimaPaint::Dot){
            for(int y=d.y;y<d.y+d.a;++y)for(int x=d.x;x<d.x+d.a;++x)put(x,y);
        }else if(d.kind==MimaPaint::Line){
            int x=d.x,y=d.y;const int dx=std::abs(d.a-x),dy=-std::abs(d.b-y),sx=x<d.a?1:-1,sy=y<d.b?1:-1;
            int error=dx+dy;
            for(;;){put(x,y);if(x==d.a&&y==d.b)break;const int e=error*2;if(e>=dy){error+=dy;x+=sx;}if(e<=dx){error+=dx;y+=sy;}}
        }else if(d.kind==MimaPaint::Circle){
            int x=d.a,y=0,error=1-x;
            while(x>=y){
                for(int i:{-1,1})for(int j:{-1,1}){put(d.x+i*x,d.y+j*y);put(d.x+i*y,d.y+j*x);}
                ++y;if(error<0)error+=2*y+1;else{--x;error+=2*(y-x)+1;}
            }
        }else{
            int x=d.a;
            for(int y=0;y<=d.a;++y){
                while(x*x+y*y>d.a*d.a)--x;
                for(int px=std::max(32,d.x-x);px<=std::min(415,d.x+x);++px){put(px,d.y-y);if(y)put(px,d.y+y);}
            }
        }
    }
    static void apply_mima_tone(std::span<std::uint32_t> pixels,unsigned tone){
        if(tone==100)return;
        tone=std::min(tone,200u);
        // master.lib palette_show writes four DAC bits per component.
        for(auto &rgb:pixels){
            std::uint32_t toned=0;
            for(unsigned shift:{0u,8u,16u}){
                const auto src=(rgb>>(shift+4))&15u;
                const unsigned out=tone>100?15-(15-src)*(200-tone)/100:src*tone/100;
                toned|=(out*17)<<shift;
            }
            rgb=toned;
        }
    }
    void draw_mima(std::span<std::uint32_t> pixels,bool winged_hint=false)const{
        const auto &art=*(mima.form2&&!winged_hint?stage5_final_art:stage5_winged_art);
        using Paint=StageSprites::Paint;
        // ASM 18408–18508: three battle put masks. Form 1 redraws ordinary
        // art for dialogue; the final defeat's back-page paint never flips.
        const auto paint=!winged_hint&&mima.body_flash&&mima.event==MimaCore::Event::None?Paint::Color4Hit:Paint::Artwork;
        for(unsigned part=0;part<3;++part)
            art.put(pixels,background,mima.pattern+part,mima.left+int(part)*48,mima.top,paint);
        if(!winged_hint)for(const auto &o:mima.orbs)
            if(o.active)stage5_winged_art->put(pixels,background,o.pattern,o.x,o.y);
    }
    void draw_mima_dialogue_player(std::span<std::uint32_t> pixels)const{
        bank.put(pixels,background,0,player.x,player.y);
        if(pickup_enabled&&pickups.power>=8){
            bank.put(pixels,background,72,player.x-16,player.y+16);
            bank.put(pixels,background,72,player.x+32,player.y+16);
        }
    }
    void draw_mima_dialogue(std::span<std::uint32_t> pixels)const{
        unsigned tone=100;
        if(!dialogue.postboss&&dialogue.current>=StageDialogue::stage5_intro_end){
            const bool winged=dialogue.current>=StageDialogue::stage5_winged;
            const bool revealing=dialogue.phase==StageDialogue::Phase::IntroAppearance;
            const bool show_winged=winged&&(!revealing||appearance_frame>=18);
            if(!winged&&revealing&&appearance_frame<160){
                if(appearance_frame>=11){
                    const unsigned r=161-appearance_frame;
                    if(r<150){
                        paint_mima(pixels,{MimaPaint::Fill,224,144,150,0,0},stage5_rgb(background.palette[0]),320);
                        for(unsigned prev=150;prev>r;--prev){
                            paint_mima(pixels,{MimaPaint::Circle,224,144,int(prev%50)*4,0,0},stage5_rgb(background.palette[0]),320);
                            paint_mima(pixels,{MimaPaint::Circle,224,144,int(prev%30)*7,0,0},stage5_rgb(background.palette[0]),320);
                        }
                    }
                    paint_mima(pixels,{MimaPaint::Fill,224,144,int(r),0,10},stage5_rgb(background.palette[10]),320);
                    paint_mima(pixels,{MimaPaint::Circle,224,144,int(r%50)*4,0,13},stage5_rgb(background.palette[13]),320);
                    paint_mima(pixels,{MimaPaint::Circle,224,144,int(r%30)*7,0,15},stage5_rgb(background.palette[15]),320);
                    tone=100+(150-r)/3*2;
                }
            }else{
                for(int y=16;y<320;++y)std::fill_n(pixels.begin()+y*640+32,384,stage5_rgb(background.palette[0]));
                if(show_winged)draw_mima(pixels,true);
                else for(unsigned row=0;row<8;++row)for(unsigned col=0;col<8;++col)
                    stage5_intro_art->put(pixels,background,128+row*8+col,96+int(col)*32,int(row)*32);
                draw_mima_dialogue_player(pixels);
                if(revealing)tone=winged?(appearance_frame<18?100+6*(appearance_frame?appearance_frame-1:0):200-6*((appearance_frame-18)/3))
                    :200-10*((appearance_frame-160)/3);
            }
        }
        if(dialogue_art)dialogue_art->render(pixels,background,bank,dialogue);
        if(mima_delay){
            const unsigned f=mima_delay_frame;
            if(mima.event==MimaCore::Event::Form1Defeated)tone=100+std::min(100u,(f?f-1:0)/10*6);
            else if((dialogue.current==54&&f==1)||(dialogue.current==55&&f==31)||(dialogue.current==57&&(f==1||f==21||f==41)))tone=200;
            else if(dialogue.current==57&&f>61)tone=100+std::min(100u,(f-61)/3*6);
        }
        // Keep the dialogue cue, but reduce its white-out intensity only.
        if(reduced_effects&&tone>100)tone=100+(tone-100)/4;
        apply_mima_tone(pixels,tone);
    }
    void draw_sigma(std::span<std::uint32_t> pixels)const{
        using P=SigmaCore::Paint;
        const auto &art=*sigma_art;
        for(std::size_t i=0;i<=sigma.paints.size();++i){
            if(sigma.visible&&i==sigma.body_paint_index){
                const int x=sigma.left[rendered_page],y=sigma.top[rendered_page];
                art.put(pixels,background,sigma.draw_pattern,x,y);
                art.put(pixels,background,sigma.draw_pattern+1,x+64,y);
            }
            if(i==sigma.paints.size())break;
            const auto &p=sigma.paints[i];
            if(p.kind==P::Sprite){art.put(pixels,background,unsigned(p.a),p.x,p.y);continue;}
            const auto color=stage5_rgb(background.palette.at(p.color));
            if(p.kind==P::Box){
                for(int y=std::max(16,p.y);y<=std::min(383,p.b);++y)
                    for(int x=std::max(32,p.x);x<=std::min(415,p.a);++x)pixels[y*640+x]=color;
            }else paint_mima(pixels,{p.kind==P::Dot?MimaPaint::Dot:p.kind==P::Circle?MimaPaint::Circle:MimaPaint::Fill,p.x,p.y,p.a,p.b,p.color},color);
        }
    }
    bool scrolling_background()const{return !scroll.done&&!marisa_enabled&&!sigma_enabled&&!mima_enabled;}
    void render(std::span<std::uint32_t> pixels,bool show_boss_terminal=true,bool smooth_background=false,unsigned subframe=scroll_subpixels)const{
        render_frame(pixels,show_boss_terminal,smooth_background,subframe);
    }
private:
    void draw_stones(std::span<std::uint32_t> pixels,bool show_effects,unsigned shift)const{
        using Paint=StageSprites::Paint;
        const int line=scroll.line;
        // Stage 3 keeps the frozen split-scroll origin. Convert each source
        // row to VRAM, respecting super_put / sub_3E6A vs. super_roll_put.
        const auto dot=[&](int x,int vram_y,unsigned index,bool rolls){
            if(vram_y>=400){if(!rolls)return;vram_y-=400;}
            const int screen_y=(vram_y-line+400)%400;
            if(x>=32&&x<416+int(shift)&&screen_y>=16&&screen_y<384)
                pixels[screen_y*640+x]=stage5_rgb(background.palette[index]);
        };
        const auto cel=[&](unsigned id,int x,int y,bool rolls,Paint paint=Paint::Artwork){
            const auto &slot=bank.sprites.at(id);
            if(!slot)throw std::runtime_error("Missing Stones sprite");
            const auto &image=*slot;
            const int vram_y=(y+line)%400;
            for(unsigned dy=0;dy<image.height;++dy)for(unsigned dx=0;dx<image.width;++dx){
                const auto index=image.pixels[dy*image.width+dx];
                if(index!=image.transparent)dot(x+int(dx),vram_y+int(dy),StageSprites::paint_index(paint,index),rolls);
            }
        };
        // stones_116EC draws living stones before the kill-animation pass.
        for(const auto &d:stones.draw)if(d.visible&&!d.dying)
            cel(d.pattern,d.x,d.y,true,d.flash?Paint::AllPlanesHit:Paint::Artwork);
        for(unsigned i=0;i<stones.draw.size()-1;++i){
            const auto &d=stones.draw[i];
            if(d.visible&&d.dying)cel(d.pattern,d.x,d.y,false);
        }
        if(!show_effects)return;
        // The original invalidates the two-frame-old ring via EGC tile copies.
        // This surface already contains a freshly reconstructed terrain frame.
        draw_death_effects(stones.death_effects,stones.death_effect_count,
            [&](int x,int y,int dy,unsigned index){dot(x,(y+line)%400+dy,index,false);},
            [&](unsigned id,int x,int y){cel(id,x,y,true);});
        const auto &north=stones.draw.back();
        if(north.visible&&north.dying)cel(north.pattern,north.x,north.y,stones.death_effects.front().frame<32);
    }
    void render_frame(std::span<std::uint32_t> pixels,bool show_boss_terminal,bool smooth_background,unsigned subframe)const{
        if(extra_settlement.showing_bonus()&&!dialogue_snapshot.empty()){
            std::copy(dialogue_snapshot.begin(),dialogue_snapshot.end(),pixels.begin());return;
        }
        if(stage5_result||(stage==4&&(dialogue.active()||mima_delay))){
            if(pixels.size()!=dialogue_snapshot.size())throw std::runtime_error("Invalid Stage 5 dialogue surface");
            std::copy(dialogue_snapshot.begin(),dialogue_snapshot.end(),pixels.begin());
            if(!stage5_result)draw_mima_dialogue(pixels);
            return;
        }
        if(transitions_enabled&&transition.phase!=StageTransition::Phase::Battle&&transition.phase!=StageTransition::Phase::Postboss&&transition.phase!=StageTransition::Phase::Entry&&!dialogue_snapshot.empty()){std::copy(dialogue_snapshot.begin(),dialogue_snapshot.end(),pixels.begin());return;}
        if(dialogue.active()&&dialogue_art){if(pixels.size()!=dialogue_snapshot.size())throw std::runtime_error("Invalid dialogue surface");std::copy(dialogue_snapshot.begin(),dialogue_snapshot.end(),pixels.begin());if(stage==1&&meira_appearance&&!dialogue.postboss)bank.put(pixels,background,141,192,48);if(stage==3&&!dialogue.postboss){
            if(marisa_music){
                clear_marisa_playfield(pixels,320);
                bank.put(pixels,background,0,player.x,player.y);
                draw_marisa(pixels);
            }
            else if(dialogue.current>=StageDialogue::stage4_intro_end&&stage4_intro_art){
                const bool revealing=dialogue.phase==StageDialogue::Phase::IntroAppearance&&appearance_frame<160;
                if(!revealing){
                    for(int y=16;y<320;++y)std::fill_n(pixels.begin()+y*640+32,384,0u);
                    for(unsigned row=0;row<8;++row)for(unsigned col=0;col<8;++col)
                        stage4_intro_art->put(pixels,background,128+row*8+col,96+int(col)*32,int(row)*32);
                }else if(appearance_frame>=10){
                    const int radius=160-int(appearance_frame);
                    const auto c=background.palette[10];const auto rgb=(unsigned(c[0])<<16)|(unsigned(c[1])<<8)|c[2];
                    for(int y=16;y<320;++y)for(int x=32;x<416;++x)
                        if((x-224)*(x-224)+(y-144)*(y-144)<=radius*radius)pixels[y*640+x]=rgb;
                }
            }
        }else if(stage==5&&!dialogue.postboss&&sigma_music&&sigma_art){
            for(int y=16;y<384;++y)std::fill_n(pixels.begin()+y*640+32,384,stage5_rgb(background.palette[0]));
            sigma_art->put(pixels,background,128,160,48);
            sigma_art->put(pixels,background,129,224,48);
            draw_mima_dialogue_player(pixels);
        }dialogue_art->render(pixels,background,bank,dialogue);return;}
        const unsigned lag=smooth_background&&scrolling_background()?background_scroll_lag(scroll,subframe):0;
        const int terrain_bottom=384+scroll_sample_rows(lag);
        if(extra_road_enabled)extra_tiles.render(pixels,background,terrain_bottom);
        else StageScene::render(pixels,false,stage4_overlay_enabled(stage,rendered_page,runtime.scroll.done,reduced_effects),terrain_bottom);
        // Boss backgrounds replace the terrain before any battle sprites.
        if(marisa_enabled)clear_marisa_playfield(pixels,384);
        if(sigma_enabled)
            for(int y=16;y<384;++y)std::fill_n(pixels.begin()+y*640+32,384,stage5_rgb(background.palette[0]));
        if(mima_enabled){
            const auto color=[&](unsigned c){return stage5_rgb(c?background.palette.at(c):mima_palette0);};
            std::fill(pixels.begin(),pixels.end(),color(0));
            for(int y=0;y<400;++y)std::fill_n(pixels.begin()+y*640+416,224,color(11));
            for(const auto &d:mima_draws)paint_mima(pixels,d,color(d.color));
        }
        if(stones_enabled)for(const auto &[key,pat]:stones_stripe){
            const int x=32+int(key%24)*16, vram_y=int(key/24)*16;
            for(int dy=0;dy<16;++dy){
                const int y=(vram_y+dy-int(scroll.line)+400)%400;
                if(y<16||y>=terrain_bottom)continue;
                for(int dx=0;dx<16;++dx){const auto c=background.palette[background.tiles[pat][dy*16+dx]];pixels[y*640+x+dx]=(unsigned(c[0])<<16)|(unsigned(c[1])<<8)|c[2];}
            }
        }
        if(!track_tiles.empty())for(const auto &[key,tile]:track_tiles){const unsigned virtual_row=key/24,col=key%24;const int top=368+int(scroll.display_pixels)-int(virtual_row*16);if(top>=terrain_bottom||top+16<=16)continue;for(int y=0;y<16;++y)for(unsigned x=0;x<16;++x){int py=top+y;if(py<16||py>=terrain_bottom)continue;const auto c=background.palette[background.tiles[tile][y*16+x]];pixels[py*640+32+col*16+x]=(unsigned(c[0])<<16)|(unsigned(c[1])<<8)|c[2];}}
        blend_stage_background(pixels,lag);
        if(bomb_enabled&&bomb_art)bomb_art->render(pixels,background,bomb);
        // th02_main.asm:1655–1657 draws Bomb before the boss update and its paints.
        if(sigma_enabled)draw_sigma(pixels);
        for(const auto &s:runtime.enemies.slots){if(s.raw[0xe]!=1||s.raw[0x10])continue;const auto type=s.word(0xc);const auto &raw=script.enemies.at(type).raw;const unsigned base=unsigned(raw[8])|(unsigned(raw[9])<<8);if(!base)continue;const unsigned pattern=s.word(0x14)==2?s.word(0x12):base+s.word(0x12);bank.put(pixels,background,pattern,enemy_s16(s.word(rendered_page*4)),enemy_s16(s.word(rendered_page*4+2)));}
        if(combat_enabled){
            for(unsigned i=0;i<25;++i)if(death_patterns[i]){const auto &s=runtime.enemies.slots[i];bank.put(pixels,background,*death_patterns[i],enemy_s16(s.word(rendered_page*4)),enemy_s16(s.word(rendered_page*4+2)));}
        }
        if(midboss_enabled&&midboss.visible){if(!midboss.dying){if(!midboss.hit_flash)bank.put(pixels,background,midboss.pattern,midboss.left[rendered_page],midboss.top[rendered_page]);else{const auto &image=*bank.sprites.at(148);const auto rgb=background.palette[15];for(unsigned y=0;y<image.height;++y)for(unsigned x=0;x<image.width;++x)if(image.pixels[y*image.width+x]!=image.transparent){const int px=midboss.left[rendered_page]+int(x),py=midboss.top[rendered_page]+int(y);if(px>=32&&px<416&&py>=16&&py<384)pixels[py*640+px]=(unsigned(rgb[0])<<16)|(unsigned(rgb[1])<<8)|rgb[2];}}}else{const auto &image=*bank.sprites.at(midboss.death_pattern);for(unsigned y=0;y<image.height;++y)for(unsigned x=0;x<image.width;++x){const auto c=image.pixels[y*image.width+x];if(c==0)continue;for(unsigned dy=0;dy<3;++dy)for(unsigned dx=0;dx<3;++dx){const int px=midboss.left[rendered_page]-16+int(x*3+dx),py=midboss.top[rendered_page]+int(y*3+dy);if(px<32||px>=416||py<16||py>=384)continue;const auto rgb=background.palette[c];pixels[py*640+px]=(unsigned(rgb[0])<<16)|(unsigned(rgb[1])<<8)|rgb[2];}}}}
        if(midboss2_enabled&&midboss2.visible){if(!midboss2.dying){if(!midboss2.hit_flash)bank.put(pixels,background,midboss2.pattern,midboss2.left[rendered_page],midboss2.top[rendered_page]);else{const auto &image=*bank.sprites.at(136);const auto color=background.palette[15];for(unsigned y=0;y<image.height;++y)for(unsigned x=0;x<image.width;++x)if(image.pixels[y*image.width+x]!=image.transparent){const int px=midboss2.left[rendered_page]+int(x),py=midboss2.top[rendered_page]+int(y);if(px>=32&&px<416&&py>=16&&py<384)pixels[py*640+px]=(unsigned(color[0])<<16)|(unsigned(color[1])<<8)|color[2];}}}else{const auto &image=*bank.sprites.at(midboss2.death_pattern);for(unsigned y=0;y<image.height;++y)for(unsigned x=0;x<image.width;++x){auto c=image.pixels[y*image.width+x];if(!c)continue;const auto color=background.palette[c];for(unsigned dy=0;dy<2;++dy)for(unsigned dx=0;dx<2;++dx){int px=midboss2.left[rendered_page]+int(x*2+dx),py=midboss2.top[rendered_page]+int(y*2+dy);if(px>=32&&px<416&&py>=16&&py<384)pixels[py*640+px]=(unsigned(color[0])<<16)|(unsigned(color[1])<<8)|color[2];}}}}
        if(midboss3_enabled){for(const auto &a:midboss3.actor){if(!a.visible)continue;const int left=a.left[rendered_page],top=a.top[rendered_page];if(a.flag==Midboss3Core::Flag::Dying){bank.put(pixels,background,a.death_pattern,left,top);bank.put(pixels,background,a.death_pattern,left+32,top);}else if(!a.hit_flash)bank.put(pixels,background,midboss3.pattern,left,top);else{if(midboss3.pattern>=bank.sprites.size()||!bank.sprites[midboss3.pattern])throw std::runtime_error("Midboss3 sprite unavailable");const auto &image=*bank.sprites[midboss3.pattern];const auto color=background.palette[15];for(unsigned y=0;y<image.height;++y)for(unsigned x=0;x<image.width;++x)if(image.pixels[y*image.width+x]!=image.transparent){const int px=left+int(x),py=top+int(y);if(px>=32&&px<416&&py>=16&&py<384)pixels[py*640+px]=(unsigned(color[0])<<16)|(unsigned(color[1])<<8)|color[2];}}}}
        if(stage==3&&midboss4.visible){
            const auto &slot=bank.sprites.at(midboss4.pattern);
            if(!slot)throw std::runtime_error("Midboss4 sprite unavailable");
            const auto &image=*slot;
            const int scale=midboss4.dying&&!midboss4.hit_flash?2:1;
            for(unsigned y=0;y<image.height;++y)for(unsigned x=0;x<image.width;++x){
                const auto source=image.pixels[y*image.width+x];if(source==image.transparent)continue;
                const auto c=background.palette[midboss4.hit_flash?15:source];
                const auto rgb=(unsigned(c[0])<<16)|(unsigned(c[1])<<8)|c[2];
                for(int dy=0;dy<scale;++dy)for(int dx=0;dx<scale;++dx){
                    const int px=midboss4.x+int(x)*scale+dx;
                    const int py=(midboss4.y+int(y)*scale+dy+400)%400;
                    if(px>=32&&px<416&&py>=16&&py<384)pixels[py*640+px]=rgb;
                }
            }
        }
        const bool marisa_terminal=show_boss_terminal&&marisa_terminal_frame();
        const bool stones_effects=show_boss_terminal||!stones_terminal_frame();
        const auto death_fx=(!marisa.cleared||marisa_terminal)&&stones_effects?boss_death_presentation():DeathPresentation{};
        if(marisa_enabled)draw_marisa(pixels,marisa_terminal,death_fx.shift);
        if(mima_enabled)draw_mima(pixels);
        if(stones_enabled)draw_stones(pixels,stones_effects,death_fx.shift);
        if(meira_enabled){
            auto unroll=[&](int y){int result=y-int(scroll.line);if(result<0)result+=400;return result;};
            auto put=[&](unsigned pat,int x,int y,unsigned scale,bool flash){const auto &image=*bank.sprites.at(pat);for(unsigned py=0;py<image.height;++py)for(unsigned px=0;px<image.width;++px){unsigned c=image.pixels[py*image.width+px];if(c==image.transparent)continue;const auto rgb=background.palette[flash?15:c];for(unsigned dy=0;dy<scale;++dy)for(unsigned dx=0;dx<scale;++dx){int xx=x+int(px*scale+dx),yy=y+int(py*scale+dy);if(xx>=32&&xx<416&&yy>=16&&yy<384)pixels[yy*640+xx]=(unsigned(rgb[0])<<16)|(unsigned(rgb[1])<<8)|rgb[2];}}};
            for(unsigned i=0;i<meira.clone_draw_count;++i)put(147,meira.clone_draws[i].left,unroll(meira.clone_draws[i].top),1,true);
            if(meira.visible)put(meira.death_zoom?meira.death_pattern:meira.draw_pattern,meira.left[rendered_page],unroll(meira.render_y),meira.death_zoom?2:1,meira.hit_flash);
            for(unsigned i=0;i<meira.effect_draw_count;++i){const auto &e=meira.effect_draws[i];if(!e.tiny)put(e.pattern,e.left,unroll(e.top),1,false);else put(e.pattern,e.left,unroll(e.top),1,false);}
        }
        if(rika_enabled&&rika.visible){
            if(!rika.death_zoom){bank.put(pixels,background,rika.draw_pattern,rika.cached_x,48);if(rika.hit_flash){const auto &image=*bank.sprites.at(rika.draw_pattern);const auto c=background.palette[15];for(unsigned y=0;y<image.height;++y)for(unsigned x=0;x<image.width;++x)if(image.pixels[y*image.width+x]!=image.transparent){const int px=rika.cached_x+int(x),py=48+int(y);if(px>=32&&px<416&&py>=16&&py<384)pixels[py*640+px]=(unsigned(c[0])<<16)|(unsigned(c[1])<<8)|c[2];}}}
            else{const auto &image=*bank.sprites.at(rika.death_pattern);for(unsigned y=0;y<image.height;++y)for(unsigned x=0;x<image.width;++x){const auto c=image.pixels[y*image.width+x];if(!c)continue;for(unsigned dy=0;dy<2;++dy)for(unsigned dx=0;dx<2;++dx){const int px=rika.cached_x+int(x*2+dx),py=64+int(y*2+dy);if(px>=32&&px<416&&py>=16&&py<384){const auto rgb=background.palette[c];pixels[py*640+px]=(unsigned(rgb[0])<<16)|(unsigned(rgb[1])<<8)|rgb[2];}}}}
        }
        if(combat_enabled)for(const auto &s:shots.slots)if(s.flag==1)bank.put(pixels,background,shots.pattern_for(s),PlayerShots::pixel(s.position[rendered_page][0]),PlayerShots::pixel(s.position[rendered_page][1]));
        if(survival_enabled){if(life.pattern)bank.put(pixels,background,*life.pattern,life.draw.x,life.draw.y);}
        else bank.put(pixels,background,player.pose==PlayerMotion::LeanLeft?2:player.pose==PlayerMotion::LeanRight?3:0,player.x,player.y);
        if(pickup_enabled&&pickups.power>=8&&(!survival_enabled||(!life.hit&&!life.miss_active&&!life.game_over))){const unsigned pat=72+static_cast<unsigned>(player.shot);bank.put(pixels,background,pat,shots.option_left,shots.option_top);bank.put(pixels,background,pat,shots.option_left+48,shots.option_top);}
        if(effects_enabled){
            constexpr std::array<unsigned,5> item_patterns={68,69,70,71,87};
            for(const auto &item:items.slots)if(item.flag==1)bank.put(pixels,background,item_patterns.at(item.type),item.left[rendered_page],ItemCore::pixel(item.top[rendered_page]));
        }
        if(effects_enabled){
            // Literal thirteen 8x8 cels from ReC98/th02/sprites/pointnum.bmp.
            constexpr std::array<std::array<unsigned,13>,8> rows={{{0,0,0,6,0,0,0,0,0,0,0,0,0},{56,0,12,126,126,126,24,32,4,16,0,24,0},{68,0,60,0,90,8,126,126,36,62,36,24,0},{130,6,0,6,126,16,0,32,34,36,24,0,0},{130,126,0,60,66,124,0,32,66,38,24,255,0},{130,0,6,0,66,36,36,60,65,74,36,0,0},{68,0,126,3,126,68,66,0,0,142,0,24,0},{56,0,0,255,0,255,0,0,0,0,0,24,0}}};
            const auto color=background.palette[15];const auto rgb=(unsigned(color[0])<<16)|(unsigned(color[1])<<8)|color[2];
            for(const auto &n:numbers.slots)if(n.flag==1){const auto glyphs=numbers.glyphs(n.points);for(unsigned digit=0;digit<7;++digit){if(glyphs[digit]>=13)throw std::runtime_error("Original point-number glyph out of range");for(unsigned y=0;y<8;++y)for(unsigned x=0;x<8;++x)if(rows[y][glyphs[digit]]&(128>>x)){const int px=n.left+int(digit*8+x),py=n.top[rendered_page]+int(y);if(px>=32&&px<416&&py>=16&&py<384)pixels[py*640+px]=rgb;}}}
        }
        if(rika_enabled||stones_enabled||stage==3||sigma_enabled)for(const auto &l:lasers.slots)if(l.visible){if(l.charging)bank.put(pixels,background,l.draw_pattern,l.x-8,l.y-8);else{if(l.draw_state<5)bank.put(pixels,background,91+l.draw_state,l.x,l.y);for(int y=l.y+8;y<384;y+=16)bank.put(pixels,background,l.base_pattern+l.draw_state,l.x,y);}}
        constexpr std::array<unsigned,8> pellet={0x3c,0x7e,0xff,0xff,0xff,0xff,0x7e,0x3c}; // th02/sprites/pellet.bmp
        for(const auto &b:bullet_core.bullets){if(b.flag!=1)continue;const int left=BulletCore::pixel(b.position[rendered_page][0]),top=BulletCore::pixel(b.position[rendered_page][1]);
            if(b.kind==EnemyBulletRequest::Bullet16){bank.put(pixels,background,b.pattern,left,top);continue;}
            for(unsigned y=0;y<8;++y)for(unsigned x=0;x<8;++x)if(pellet[y]&(128>>x)){int px=left+int(x),py=top+int(y);if(px>=32&&px<416&&py>=16&&py<384){auto c=background.palette[15];pixels[py*640+px]=(unsigned(c[0])<<16)|(unsigned(c[1])<<8)|c[2];}}
        }
        if(effects_enabled){
            // Exact eight cels from ReC98/th02/sprites/sparks.bmp (64x8,1bpp).
            constexpr std::array<std::array<unsigned,8>,8> rows={{{0x10,0x40,0,0,0x40,0,0,0x3c},{0x10,0x21,0x42,0,0x21,0x42,0,0x42},{0x10,0x12,0x24,0x24,0x12,0x24,0x24,0x81},{0x1f,0x1c,0x18,0x18,0x1c,0x18,0x18,0x99},{0xf8,0x38,0x18,0x18,0x38,0x18,0x18,0x99},{8,0x48,0x24,0x24,0x48,0x24,0x24,0x81},{8,0x84,0x42,0,0x84,0x42,0,0x42},{8,2,0,0,2,0,0,0x3c}}};
            auto color=background.palette[14];const auto rgb=(unsigned(color[0])<<16)|(unsigned(color[1])<<8)|color[2];
            for(const auto &s:sparks.slots)if(s.flag==1){const int left=SparkCore::pixel(s.position[rendered_page][0]),top=SparkCore::pixel(s.position[rendered_page][1]);for(unsigned y=0;y<(s.sprite?8u:1u);++y)for(unsigned x=0;x<(s.sprite?8u:1u);++x)if(!s.sprite||(rows[y][s.age%8]&(128>>x))){const int px=left+int(x),py=top+int(y);if(px>=32&&px<416&&py>=16&&py<384)pixels[py*640+px]=rgb;}}
        }
        if(mima_enabled)apply_mima_tone(pixels,mima_tone);
        if(death_fx.shift)for(int y=16;y<384;++y){
            auto first=pixels.begin()+y*640+32;
            std::copy(first+death_fx.shift,first+384+death_fx.shift,first);
            std::fill_n(first+384,death_fx.shift,0u);
        }
        if(death_fx.tone!=100)apply_mima_tone(pixels,death_fx.tone);
        // Approximate Sigma's one-word GDC offset without moving host HUD or collisions.
        if(sigma_view_shift())for(int y=16;y<384;++y){
            auto first=pixels.begin()+y*640+32;
            std::rotate(first,first+16,first+384);
        }
    }
};
}
