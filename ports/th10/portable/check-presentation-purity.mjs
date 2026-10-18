import {readFileSync} from 'node:fs';
import assert from 'node:assert/strict';

const read=path=>readFileSync(new URL(`../${path}`,import.meta.url),'utf8').replaceAll('\r','');
const app=read('th10_web/cpp/platform/Application.cpp');
const popups=read('th10_web/cpp/game/ScorePopups.cpp');
const bullets=read('th10_web/cpp/platform/WorldBullets.cpp');
const items=read('th10_web/cpp/platform/WorldObjects.cpp');
const lasers=read('th10_web/cpp/platform/WorldLasers.cpp');
const backgrounds=read('th10_web/cpp/platform/Backgrounds.cpp');
const startup=read('th10_web/cpp/game/StartupScreen.cpp');
const screenEffects=read('th10_web/cpp/platform/ScreenEffects.cpp');
const anim=read('th10_web/cpp/platform/AnimationEngine.cpp');

// Draw-only frames restore shared camera/render/text bookkeeping that the
// original 60 Hz draw chain is allowed to mutate.
assert.match(app,/saved_world=engine\.world,saved_ui=engine\.ui/);
assert.match(app,/saved_screen_space=engine\.screen_space,saved_fog=engine\.fog_enabled/);
assert.match(app,/saved_reserved=manager->reserved_050/);
assert.match(app,/saved_text_count=common_view->text_count/);
assert.match(app,/common_view->text_count=saved_text_count/);
assert.match(app,/manager->submitted_draws=saved_submitted/);
assert.match(app,/std::snprintf\(entry\.text,sizeof\(entry\.text\),"%\.2ffps",presentation_fps\)/);

// Embedded render scratch must be copied on presentation frames.
assert.match(popups,/if\(high_refresh::render_only\)local_animation=animation/);
assert.match(bullets,/if\(!high_refresh::render_only\|\|!high_refresh::active\)return false/);
assert.match(items,/if\(!high_refresh::render_only\|\|!high_refresh::active\)return false/);
assert.match(lasers,/if\(!high_refresh::render_only\|\|!high_refresh::active\)return false/);
assert.match(startup,/if\(!high_refresh::render_only\).*create_opening_animation.*create_loading_animation.*elapsed=wrapping_add/s);
assert.match(screenEffects,/void ScreenEffects::destroy\(ScreenEffect\* effect\)\{previous_alpha\.erase\(effect\)/);
assert.match(anim,/bool AnimationEngine::present\(AnmVm& copy,const AnmVm& source\) const/);
assert.match(anim,/auto copy=vm;present\(copy,vm\)/);

// Stage drawing is not pure in TH10: fade timers, flags, counters and object
// animation scratch are mutated by the original draw. Presentation therefore
// must render a temporary Stage/object-animation copy and suppress draw-time
// effect creation before restoring shared object flags.
assert.match(backgrounds,/Stage copy=stage;copy\.camera=presentation_camera\(stage\)/);
assert.match(backgrounds,/animations\.assign\(stage\.object_animations/);
assert.match(backgrounds,/engine\.present\(copy\.script_animations\[i\],stage\.script_animations\[i\]\)/);
assert.match(backgrounds,/if\(!high_refresh::render_only\)ScreenEffect::create/);
assert.match(backgrounds,/stage\.objects\[i\]->flags=flags\[i\]/);

console.log('TH10 presentation purity PASS: extra display frames restore shared state and draw embedded owners through copies');
