import {readFileSync} from 'node:fs';
import assert from 'node:assert/strict';

const read=path=>readFileSync(new URL(`../${path}`,import.meta.url),'utf8').replaceAll('\r','');
const host=read('th10_web/cpp/sdl/ApplicationHost.cpp');
const app=read('th10_web/cpp/platform/Application.cpp');
const appFrames=read('th10_web/cpp/platform/ApplicationFrames.cpp');
const anim=read('th10_web/cpp/platform/AnimationEngine.cpp');
const worldPlayer=read('th10_web/cpp/platform/WorldPlayer.cpp');
const bullets=read('th10_web/cpp/game/BulletFrame.cpp');
const items=read('th10_web/cpp/game/ItemDraw.cpp');
const lasers=read('th10_web/cpp/game/LaserFrame.cpp');
const high=read('th10_web/cpp/game/HighRefresh.hpp');
const startup=read('th10_web/cpp/game/StartupScreen.cpp');
const effects=read('th10_web/cpp/platform/ScreenEffects.cpp');
const backgrounds=read('th10_web/cpp/platform/Backgrounds.cpp');
const popupGame=read('th10_web/cpp/game/ScorePopups.cpp');
const popupPlatform=read('th10_web/cpp/platform/WorldPopups.cpp');
const resultsDraw=read('th10_web/cpp/game/ResultsDraw.cpp');
const guiDraw=read('th10_web/cpp/game/GuiDraw.cpp');
const hud=read('th10_web/cpp/platform/Hud.cpp');

// rAF may schedule extra presentation frames, but authoritative simulation is
// at most one Application::step(true) per callback. Late callbacks skip expired
// original deadlines instead of replaying multiple catch-up ticks.
assert.match(host,/const bool tick_due=cadence\.advance\(delta\)!=0/);
assert.match(host,/if\(tick_due\)\{sdl_native_input\(application\);result=application->step\(true\);\}/);
assert.doesNotMatch(host,/for\(unsigned i=0;i<ticks/);
assert.match(host,/sdl_defer\(1\)/);
assert.match(host,/frame_alpha=interpolate\?float\(cadence\.interpolation_alpha\(\)\):1\.0f/);
assert.match(host,/application->presentation_draw\(frame_alpha,interpolate,!frozen\)/);
assert.match(host,/session_flags&0x74/);
assert.match(host,/screen==1\|\|screen==4\|\|screen==7\|\|screen==14/);
assert.match(startup,/if\(!high_refresh::render_only\).*opening_ready.*elapsed=wrapping_add/s);
assert.match(effects,/previous_alpha\[&effect\]=effect\.alpha/);
assert.match(effects,/high_refresh::lerp\(float\(found->second\),float\(effect\.alpha\)\)/);

// Extra display frames replay only the draw chain. They must not enter
// engine.update_all(), FrameStatistics::draw(), or Presentation::submit().
const presentationDraw=app.slice(app.indexOf('bool Application::presentation_draw'),app.indexOf('void Application::presentation_frame'));
assert.match(presentationDraw,/high_refresh::begin\(alpha,interpolate,true,world_interpolate\)/);
assert.match(presentationDraw,/engine\.draw_all\(\)/);
assert.doesNotMatch(presentationDraw,/update_all\(|clock\.step|\.submit\(\)/);
assert.match(app,/if\(high_refresh::render_only\).*presentation_fps/s);
assert.match(app,/statistics->draw\(rates\)/);

// The original HUD assumed a two-digit FPS value at x=590. High refresh can
// make the label wider, so keep its right edge fixed inside the 640px viewport.
assert.match(appFrames,/std::strlen\(output\)/);
assert.match(appFrames,/if\(length>7\)adjusted\.x-=float\(\(length-7\)\*7\)/);

// Generic ANM presentation is field-aware: position is lifecycle-gated, while
// scale/rotation/color/UV may only smooth fields that the authored ANM is
// already advancing continuously through an interpolator or velocity.
assert.match(anim,/snapshot_presentation\(\)/);
assert.match(anim,/copy\.position=mix/);
assert.match(anim,/copy\.script_position=mix/);
assert.match(anim,/copy\.child_position=mix/);
assert.match(anim,/continuous_fields\(const AnmVm& vm\)/);
assert.match(anim,/rotation_interpolation\.duration>0\|\|vm\.angular_velocity/);
assert.match(anim,/scale_interpolation\.duration>0\|\|vm\.scale_velocity/);
assert.match(anim,/color_interpolation\.duration>0/);
assert.match(anim,/alpha_interpolation\.duration>0/);
assert.match(anim,/presentation_previous\.try_emplace\(&vm,presentation_sample\(vm\)\)/);
assert.match(anim,/before\.visible!=\(source\.flags&3u\)/);
assert.match(anim,/source\.script_timer\.current<before\.script_time/);
assert.match(anim,/before\.sprite_index==source\.sprite_index/);
assert.match(anim,/continuous&64/);
assert.match(backgrounds,/engine\.present\(copy\.script_animations\[i\],stage\.script_animations\[i\]\)/);
assert.match(backgrounds,/engine\.present\(animations\[i\],stage\.object_animations\[i\]\)/);

// Owner-side non-ANM motion still needs explicit endpoints. Score popups keep
// discrete sprite-age thresholds but smooth their continuously moving position
// (and the authored first-eight-frame spacing), while Results keeps state gates
// discrete and only smooths the direct text slide coordinate.
assert.match(popupPlatform,/popup_presentation\[i\]=\{p\.position,p\.elapsed\.fractional,p\.elapsed\.current,p\.active,p\.length\}/);
assert.match(popupPlatform,/high_refresh::lerp_world\(before\.position\.y,popup\.position\.y\)/);
assert.match(popupGame,/env\.presentation\(popup,position,elapsed\)/);
assert.match(resultsDraw,/displayed_elapsed\(const Timer& timer\)/);
assert.match(resultsDraw,/number\(displayed_elapsed\(elapsed\)\)/);
assert.match(guiDraw,/presentation_timer\(env\.game->faith_timer\)/);
assert.match(guiDraw,/env\.presentation_boss_health\(displayed_boss_health\)/);
assert.match(hud,/previous_boss_health=gui\.displayed_boss_health/);
assert.match(hud,/owner\.actors\.enemies->bosses\[0\]!=owner\.previous_boss/);

// Embedded gameplay owners use copies/sidecars rather than writing
// presentation values back into authoritative ABI state.
assert.match(worldPlayer,/Player copy;Player\* draw=&player/);
assert.match(worldPlayer,/player_presentation\.state==player\.state/);
assert.match(bullets,/AnmVm copy;AnmVm\* vm=&source/);
assert.match(items,/AnmVm copy;AnmVm\* vm=&animation/);
assert.match(lasers,/AnmVm beam_copy,tip_copy/);
assert.match(high,/render_only/);
assert.match(high,/previous\+\(current-previous\)\*alpha/);
assert.match(high,/world_alpha/);
assert.match(high,/lerp_world/);

console.log('TH10 high-refresh contract PASS: fixed 60 Hz authority + draw-only owner/ANM presentation');
