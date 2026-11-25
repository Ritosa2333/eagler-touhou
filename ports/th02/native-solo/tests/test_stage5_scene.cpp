#include "encounter_scene.hpp"
#include "music_room.hpp"
#include <iostream>
#include <string>

namespace {
using namespace soew;
using DialoguePhase = StageDialogue::Phase;
using BattlePhase = MimaCore::Phase;
using ContinueChoice = EncounterScene::ContinueChoice;
using TransitionPhase = StageTransition::Phase;

constexpr std::uint32_t seed = 1234;
constexpr unsigned frame_budget = 125000;
unsigned checks = 0;

void check(bool condition, const char *message) {
    ++checks;
    if (!condition) throw std::runtime_error(message);
}

void check_running(const EncounterScene &scene) {
    if (scene.runtime.blocked) throw std::runtime_error("Stage 5 encounter runtime blocked");
    if (const char *reason = scene.mima.blocked_reason()) throw std::runtime_error(reason);
    if (scene.weapon_blocked) throw std::runtime_error("Stage 5 weapon runtime blocked");
}

void dialogue_step(EncounterScene &scene) {
    const bool press = scene.dialogue.phase == DialoguePhase::Text ||
        (scene.dialogue.phase == DialoguePhase::KeyDelay && scene.dialogue.key_delay.waiting_press);
    scene.tick(press ? Shot : 0, press);
}

void play_step(EncounterScene &scene) {
    // Leave actual survival and collision code running, but isolate each run
    // from accidental misses. The scripted Continue removes this protection.
    scene.life.invincibility = 250;
    std::uint16_t input = 0;
    if (scene.mima_enabled) {
        const int target = scene.mima.left + 64;
        input = Shot;
        if (scene.player.x + 8 < target - 4) input |= Right;
        else if (scene.player.x + 8 > target + 4) input |= Left;
        if (scene.player.y > 256) input |= Up;
    }
    scene.tick(input);
}

void accept_continue(EncounterScene &scene) {
    const auto phase = scene.mima.phase;
    const auto form2 = scene.mima.form2;
    const unsigned credit_before = scene.continues_used;
    const unsigned misses_before = scene.life.total_misses;
    // Script exactly one hit with no spares; death animation and Game Over
    // still run through PlayerLife and EncounterScene::tick.
    scene.life.lives = 0;
    scene.life.invincibility = 0;
    scene.life.register_hit();
    for (unsigned i = 0; i < 24 && !scene.life.game_over; ++i) scene.tick(0);
    check(scene.life.game_over && scene.life.total_misses == misses_before + 1,
          "Scripted hit did not reach Game Over");
    check(scene.mima.phase == phase && scene.mima.form2 == form2,
          "Scripted miss did not stay in the same boss form");

    const auto ticks = scene.ticks;
    const auto page = scene.runtime.page;
    const auto scroll_pixels = scene.runtime.scroll.display_pixels;
    const auto phase_frame = scene.mima.phase_frame;
    const auto rounds = scene.mima.rounds;
    const auto damage = scene.mima.damage_taken;
    const auto score = scene.score_core.score;
    scene.tick(Shot, true);
    check(scene.continue_choice == ContinueChoice::Release, "Continue did not open on its release gate");
    scene.tick(Shot, true);
    scene.tick(0, true);
    check(scene.continue_choice == ContinueChoice::Release && scene.continues_used == credit_before,
          "Held input bypassed the Continue release gate");
    scene.tick(0, false);
    check(scene.continue_choice == ContinueChoice::Choose && scene.continue_yes,
          "Continue did not open with Yes selected");
    scene.tick(Down);
    check(!scene.continue_yes, "Down did not select No");
    scene.tick(Up);
    scene.tick(Shot, true);
    check(scene.continues_used == credit_before + 1 && !scene.life.game_over && !scene.continue_prompt(),
          "Accepting Continue did not resume with exactly one credit used");
    check(scene.life.lives == scene.run_start_lives && scene.life.bombs == scene.run_start_bombs &&
          scene.life.power == 1 && scene.life.invincibility == 200,
          "Continue did not restore starting stock and protection");
    check(scene.score_core.score == 0 && scene.highest_run_score >= score,
          "Continue did not reset score while preserving the run best");
    check(scene.ticks == ticks && scene.runtime.page == page && scene.runtime.scroll.display_pixels == scroll_pixels &&
          scene.mima.form2 == form2 && scene.mima.phase == phase && scene.mima.phase_frame == phase_frame &&
          scene.mima.rounds == rounds && scene.mima.damage_taken == damage,
          "Continue reset or advanced the frozen encounter");
    check(scene.transition.phase == TransitionPhase::Entry, "Continue did not schedule the resume fade");
    for (unsigned i = 0; i < StageTransition::fade_frames; ++i) scene.tick(0);
    check(scene.transition.phase == TransitionPhase::Battle && scene.ticks == ticks &&
          scene.mima.phase_frame == phase_frame, "Continue fade did not keep the encounter frozen");
}

void check_hit_art(const std::filesystem::path &root){
    auto scene=EncounterScene::load(root,4,0,1,2,3);
    scene.enable_stage5_preview();
    scene.mima.left=143; // Exercise the shifted source mask, not just byte-aligned artwork.
    constexpr std::uint32_t untouched=0xfedcba;
    std::vector<std::uint32_t> pixels(640*400);
    const auto rgb=[&](unsigned index){const auto c=scene.background.palette[index];return (unsigned(c[0])<<16)|(unsigned(c[1])<<8)|c[2];};
    const auto verify=[&](bool winged_hint,bool colored){
        std::fill(pixels.begin(),pixels.end(),untouched);
        scene.draw_mima(pixels,winged_hint);
        const auto &art=*(scene.mima.form2&&!winged_hint?scene.stage5_final_art:scene.stage5_winged_art);
        for(unsigned part=0;part<3;++part){
            const auto &g=*art.sprites.at(scene.mima.pattern+part);
            for(unsigned y=0;y<g.height;++y)for(unsigned x=0;x<g.width;++x){
                const auto i=g.pixels[y*g.width+x];
                const auto at=(scene.mima.top+int(y))*640+scene.mima.left+part*48+x;
                check(pixels[at]==(i==g.transparent?untouched:rgb(colored?4u:i)),"Mima body put mask did not match artwork");
            }
        }
        check(pixels[(scene.mima.top-1)*640+scene.mima.left]==untouched,"Mima mask touched outside body");
    };
    verify(false,false);
    unsigned damage=0;
    MimaCore::Hooks h;h.damage=[&](int,int,int,int){return damage;};h.random8=[](){return 0u;};
    MimaCore::Player p;
    for(unsigned frame=1;frame<=6;++frame){
        damage=frame==3?1u:0u;
        p.stage_frame=frame;scene.mima.update(h,p);
        check(scene.mima.body_flash==(frame==6)&&scene.mima.damage_taken==0,"a real locked hit starts its own four-sample latch");
        verify(false,frame==6);
    }
    verify(true,false); // Dialogue's winged stand-in has no battle latch.
    scene.mima.event=MimaCore::Event::Form1Defeated;verify(false,false);
    check(scene.mima.resolve_form1(h,0).enter_form2,"form 2 art switch setup");
    damage=1;
    for(unsigned frame=1;frame<=4;++frame){
        p.stage_frame=frame;scene.mima.update(h,p);
        check(scene.mima.body_flash==(frame==4),"form 2 shares the hit latch");
        scene.reduced_effects=true;verify(false,frame==4);
    }
    scene.mima.event=MimaCore::Event::FinalDefeated;verify(false,false);
}

void check_dialogue_tones(const std::filesystem::path &root){
    auto s=EncounterScene::load(root,4,0,1,2,3);
    s.enable_stage5_preview();
    s.dialogue_snapshot.resize(640*400);
    s.dialogue.postboss=true;
    s.dialogue.current=54;
    s.mima.event=MimaCore::Event::FinalDefeated;
    s.mima_delay=10;s.mima_delay_frame=1;
    std::vector<std::uint32_t> normal(640*400),reduced(normal.size());
    s.render(normal);
    s.reduced_effects=true;s.render(reduced);
    check(normal[0]==0xffffff&&reduced[0]==0x444444,"Reduced Mima defeat pulse uses tone 125 rather than 200");
    s.mima_delay_frame=2;
    s.render(reduced);check(reduced[0]==0,"non-flash dialogue frame keeps normal tone");
    s.dialogue.postboss=false;
    s.dialogue.current=StageDialogue::stage5_winged;
    s.dialogue.phase=DialoguePhase::IntroAppearance;
    s.appearance_frame=18;s.mima_delay=0;
    s.reduced_effects=false;s.render(normal);
    s.reduced_effects=true;s.render(reduced);
    check(normal[0]==0xffffff&&reduced[0]==0x444444,"winged reveal uses the same dialogue-only tone clamp");
    check(s.appearance_frame==18&&s.mima_delay_frame==2&&s.mima_tone==100&&s.ticks==0,"rendering effects cannot advance dialogue or alter battle tone");
}

enum class Route { Good, EarlierContinue, FinalContinue };

void run(const std::filesystem::path &root, Route route, const char *label) {
    auto scene = EncounterScene::load(root, 4, 0, 1, 2, 3);
    scene.enable_effects(seed);
    scene.combat_enabled = scene.survival_enabled = scene.transitions_enabled = scene.reduced_effects = true;
    scene.enable_stage5_preview();
    check(scene.stage5_preview && !scene.stage5_entry_only && !scene.mima_enabled,
          "Stage 5 did not enable its preview without skipping to Mima");
    check(music_files.at(gameplay_music_track(scene.stage, false, false)) == "STAGE4.M",
          "Stage 5 did not map to the native stage track");
    if (route == Route::EarlierContinue) accept_continue(scene);

    for (unsigned i = 0; i < 3000 && !scene.preboss_started; ++i) {
        play_step(scene);
        check_running(scene);
    }
    check(scene.preboss_started && scene.runtime.scroll.done && scene.scroll.done && scene.ticks > 1000,
          "Stage 5 did not reach the pre-boss dialogue by scrolling its own map");
    check(scene.runtime.spawns > 0 && scene.bullet_core.spawned > 0,
          "Stage 5 map had no live enemy or projectile encounters");
    check(scene.dialogue.current == 0 && !scene.mima_enabled && !scene.mima_music && !scene.mima_music_stopped &&
          music_files.at(gameplay_music_track(scene.stage, scene.preboss_started, false)) == "STAGE4.M",
          "Boss track started before the pre-boss dialogue reached the winged reveal");
    const auto map_ticks = scene.ticks;
    std::vector<std::uint32_t> surface(640 * 400);
    scene.render(surface);

    unsigned appearances = 0;
    bool saw_boss_track = false;
    for (unsigned i = 0; i < 2000 && !scene.mima_enabled; ++i) {
        if (scene.dialogue.phase == DialoguePhase::IntroAppearance && scene.appearance_frame == 0) {
            ++appearances;
            check(!scene.mima_music, "Boss track started before an appearance interlude");
        }
        dialogue_step(scene);
        if (scene.mima_music && !saw_boss_track) {
            check(scene.dialogue.current == StageDialogue::stage5_winged,
                  "Boss track flag appeared outside the winged dialogue record");
            saw_boss_track = true;
            scene.render(surface);
        }
    }
    check(scene.mima_enabled && scene.dialogue.preboss_complete() && !scene.dialogue.active() &&
          scene.transition.phase == TransitionPhase::Battle && scene.ticks == map_ticks &&
          appearances == 2 && saw_boss_track && scene.mima_music,
          "Stage 5 dialogue did not freeze scrolling and bind the first form after both reveals");
    scene.render(surface);

    const auto boss_start_bullets = scene.bullet_core.spawned;
    const auto boss_start_shots = scene.shots.fired;
    bool saw_form1_dialogue = false, saw_final_dialogue = false, continued_in_final = false;
    unsigned damage_windows = 0;
    for (unsigned i = 0; i < frame_budget && !scene.stage5_result; ++i) {
        check_running(scene);
        if (scene.dialogue.active() || scene.mima_delay) {
            if (scene.mima.event == MimaCore::Event::Form1Defeated && !saw_form1_dialogue) {
                saw_form1_dialogue = true;
                check(scene.dialogue.current == (route == Route::EarlierContinue ? 44u : 49u),
                      "First-form defeat opened the wrong Continue dialogue branch");
                scene.render(surface);
            }
            if (scene.mima.event == MimaCore::Event::FinalDefeated && !saw_final_dialogue) {
                saw_final_dialogue = true;
                check(scene.dialogue.current == StageDialogue::stage5_final_start && !scene.mima_music &&
                      scene.mima_music_stopped, "Final defeat did not stop the boss track and open final dialogue");
                scene.render(surface);
            }
            dialogue_step(scene);
        } else if (route == Route::FinalContinue && !continued_in_final && scene.mima.phase == BattlePhase::Final &&
                   scene.mima.rounds >= 1) {
            accept_continue(scene);
            continued_in_final = true;
            check(scene.mima_music && !scene.mima_music_stopped, "Continue in form 2 stopped boss music");
        } else {
            const auto damage = scene.mima.damage_taken;
            play_step(scene);
            if (scene.mima.damage_taken > damage) damage_windows |= scene.mima.form2 ? 2u : 1u;
        }
        if (scene.life.game_over) throw std::runtime_error("Unexpected Game Over in protected test gameplay");
    }
    check_running(scene);
    check(scene.stage5_result.has_value(), "Stage 5 did not reach its result within the test frame budget");
    const bool reached_final = route != Route::EarlierContinue;
    const auto result = *scene.stage5_result;
    check(saw_form1_dialogue && saw_final_dialogue == reached_final && scene.mima.form2 == reached_final &&
          continued_in_final == (route == Route::FinalContinue), "Stage 5 traversed the wrong boss forms");
    check(result.ending && !result.enter_form2 && result.good_ending == (route == Route::Good) &&
          result.bonus == (reached_final ? 100000u : 50000u), "Unexpected Stage 5 ending or clear bonus");
    check(scene.continues_used == (route == Route::Good ? 0u : 1u) && scene.score_core.score >= result.bonus,
          "Stage 5 result did not retain live run credits or award its score bonus");
    check(scene.bullet_core.spawned > boss_start_bullets && scene.shots.fired > boss_start_shots &&
          damage_windows == (reached_final ? 3u : 1u), "Live projectiles did not reach the expected boss damage windows");
    const auto tick_count = scene.ticks;
    const auto score = scene.score_core.score;
    scene.tick(Shot, true);
    scene.render(surface);
    check(scene.ticks == tick_count && scene.score_core.score == score && scene.stage5_result.has_value(),
          "Stage 5 result did not freeze gameplay or awarded the bonus twice");
    std::cout << label << ": scroll_ticks=" << map_ticks << " gameplay_ticks=" << tick_count
              << " boss_bullets=" << scene.bullet_core.spawned - boss_start_bullets
              << " damage_windows=" << damage_windows << " continues=" << scene.continues_used
              << " bonus=" << result.bonus << '\n';
}
} // namespace

int wmain(int argc, wchar_t **argv) {
    if (argc != 2) {
        std::cerr << "Usage: soew-stage5-scene-tests <assets>\n";
        return 2;
    }
    try {
        const std::filesystem::path root = argv[1];
        check_hit_art(root);
        check_dialogue_tones(root);
        run(root, Route::Good, "good");
        run(root, Route::EarlierContinue, "bad-form1");
        run(root, Route::FinalContinue, "bad-form2");
        std::cout << "stage5_scene_checks=" << checks << " seed=" << seed << " failures=0\n";
        return 0;
    } catch (const std::exception &error) {
        std::cerr << "Stage 5 scene check " << checks << " failed: " << error.what() << '\n';
        return 1;
    }
}
