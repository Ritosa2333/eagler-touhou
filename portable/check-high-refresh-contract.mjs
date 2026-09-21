import {readFileSync} from 'node:fs';
import assert from 'node:assert/strict';

const read=path=>readFileSync(new URL(`../${path}`,import.meta.url),'utf8').replaceAll('\r','');
const host=read('th08_web/cpp/sdl/GameHost.cpp');
const app=read('th08_web/cpp/game/GameApplication.cpp');
const rendererH=read('portable/sdl/Renderer.hpp');
const renderer=read('portable/sdl/Renderer.cpp');
const presentation=read('th08_web/cpp/game/Presentation.hpp');
const ascii=read('th08_web/cpp/game/AsciiManager.cpp');
const background=read('th08_web/cpp/game/BackgroundView.cpp');
const backgroundObjects=read('th08_web/cpp/game/BackgroundObjects.cpp');
const player=read('th08_web/cpp/game/PlayerSimulation.cpp');
const bullets=read('th08_web/cpp/game/BulletDrawing.cpp');
const effects=read('th08_web/cpp/game/EffectSystem.cpp');
const effectGeometry=read('th08_web/cpp/game/EffectGeometry.hpp');
const scene=read('th08_web/cpp/game/GameplayScene.cpp');
const spell=read('th08_web/cpp/game/SpellUpdate.cpp');
const spellDrawing=read('th08_web/cpp/game/SpellDrawing.cpp');
const guiController=read('th08_web/cpp/game/GuiController.cpp');
const spellBackground=read('th08_web/cpp/game/SpellBackground.cpp');
const backgroundView=read('th08_web/cpp/game/BackgroundView.cpp');
const backgroundScript=read('th08_web/cpp/game/BackgroundScript.cpp');
const shell=read('th08_web/sdl-runtime/shell.mjs');
const labController=read('portable/presentation-lab/controller.mjs');
const labServer=read('portable/presentation-lab/serve.mjs');
const labStart=read('portable/presentation-lab/start-lab.ps1');
const buildScript=read('portable/build.mjs');
const packageScript=read('portable/package-eagler.mjs');
const commonController=read('third_party/eagler-common/testkit/presentation-lab/controller-core.mjs');

// Diagnostic orchestration is a pinned common testkit dependency. The
// production shell must not know Lab hotkeys or expose a runtime toggle.
assert.match(labController,/import \{PresentationLabControllerCore\}/);
assert.match(labController,/third_party\/eagler-common\/testkit\/presentation-lab\/controller-core\.mjs/);
assert.match(labServer,/third_party\/eagler-common\/testkit\/presentation-lab/);
assert.match(commonController,/class PresentationLabControllerCore/);
assert.doesNotMatch(shell,/presentation-mark|gameGeneration.*presentation-lab/);
assert.match(buildScript,/profile=presentationLab\?'presentation-lab':'sdl3'/);
assert.match(buildScript,/diagnostic:presentationLab/);
assert.match(packageScript,/build\.diagnostic.*presentationLab/);
assert.match(packageScript,/Production build contains diagnostic export/);
assert.match(labStart,/package-eagler\.mjs --presentation-lab/);
assert.match(labServer,/artifacts\/presentation-lab\/runtime/);

// Fixed game clock: one rAF callback may execute zero or one fixed tick. Late
// callbacks skip expired 60 Hz deadlines instead of replaying catch-up ticks.
assert.match(host,/int tick\(\).*runtime->step\(false\)/s);
assert.match(host,/const bool tick_due=cadence\.advance\(delta\)!=0/);
assert.match(host,/if\(tick_due&&!result\)/);
assert.doesNotMatch(host,/for\(unsigned i=0;i<ticks/);
assert.match(host,/elapsed\+=touhou::sdl::FrameCadence::interval;result=tick\(\)/);
assert.match(host,/th08_limit_presentation_to_60/);
assert.match(host,/const bool limit60=th08_limit_presentation_to_60\(\)!=0/);
assert.match(host,/const bool ready=interpolation_ready\(\)&&!limit60/);

// Every real 60 Hz tick still executes TH08's authoritative draw once. High
// refresh only hides that swap and follows it with a presentation-only draw.
assert.match(host,/runtime->app\.draw\(1\.0f,false,false\)/);
assert.match(host,/runtime->app\.draw\(presentation_alpha,interpolate,true,!frozen\)/);
assert.match(host,/presentation_gate\.advance\(high,tick_due\)/);
assert.doesNotMatch(host,/interpolate=.*fast/);
assert.match(host,/const bool frozen=.*paused.*retrying.*pause_state.*show_retry/s);
const frameTick=host.indexOf('result=tick();');
const frameDraw=host.indexOf('runtime->app.draw(1.0f,false,false)',frameTick);
const frameFault=host.indexOf('runtime->status(2)||runtime->status(4)',frameDraw);
const frameCount=host.indexOf('++frames',frameDraw);
const frameAudio=host.indexOf('runtime->audio_tick',frameCount);
assert(frameTick>=0&&frameTick<frameDraw&&frameDraw<frameFault&&frameFault<frameCount&&frameCount<frameAudio,'rAF fixed tick order must remain update -> draw/fault -> frames -> audio');
const manual=host.indexOf('EX("sdl_loop_tick")');
const manualTick=host.indexOf('result=tick()',manual),manualDraw=host.indexOf('runtime->app.draw(1.0f,false,false)',manualTick),manualFault=host.indexOf('runtime->status(2)||runtime->status(4)',manualDraw),manualCount=host.indexOf('++frames',manualDraw),manualAudio=host.indexOf('runtime->audio_tick',manualCount);
assert(manual>=0&&manualTick<manualDraw&&manualDraw<manualFault&&manualFault<manualCount&&manualCount<manualAudio,'manual fixed tick order must remain update -> draw/fault -> frames -> audio');

// Extra display frames must not advance original FPS/replay bookkeeping or
// consume sound commands. They may only redraw the already-produced state.
assert.match(app,/if\(presentation::render_only\)a\.statistics\.draw_text\(\);else a\.statistics\.calculate\(true\)/);
assert.match(app,/if\(!presentation_only\).*recording\.input\.timing_level=.*platform\.process_sounds\(\)/s);
assert.match(app,/if\(presentation_only\)\{ascii\.state\.string_count=string_count/);
assert.match(app,/if\(!presentation_only&&value<=0\)/);
assert.match(app,/if\(!presentation_only\)failed\|=invalid\(\)/);

// The old GPU command-replay/quad-matching experiment is forbidden. Owner
// state prev/current interpolation is the only high-refresh mechanism.
for(const source of [rendererH,renderer]){
  assert.doesNotMatch(source,/PresentationCommand|presentation_record|presentation_tag|sdl_interpolate|interpolatedBatches|presentationReplays/);
}
assert.match(presentation,/render_only/);
assert.match(presentation,/previous\+\(current-previous\)\*alpha/);
assert.match(presentation,/world_alpha/);
assert.match(presentation,/lerp_world/);
assert.match(app,/presentation::begin\(presentation_alpha,presentation_active,presentation_only,world_interpolate\)/);

// Score popups own their world position outside AnmVm. Their continuous rise
// must publish a previous endpoint while sprite-age changes remain discrete.
assert.match(ascii,/score_popup_previous\[i\]=\{p\.position,p\.timer\.current,p\.in_use,p\.characters\}/);
assert.match(ascii,/presentation::lerp_world\(before\.position\.y,p\.position\.y\)/);
assert.match(ascii,/direct_sprite\(small,p\.text\[i\]\+\(p\.timer\.current<52\?0:p\.timer\.current<56\?11:21\)\)/);
assert.match(background,/presentation::lerp_world\(float\(s\.spell_frames-1\),float\(s\.spell_frames\)\)/);
assert.match(player,/state\.life\.state==1\|\|state\.life\.state==2/);
assert.match(player,/presentation_previous_script==current\.scriptIndex/);
assert.match(player,/animation\.scale=\{presentation::lerp_world/);
assert.match(player,/animation\.color1\.a=u8\(std::clamp\(presentation::lerp_world/);
assert.match(bullets,/p\.scale_x=vm\.scale\.x/);
assert.match(bullets,/p\.scale_y=vm\.scale\.y/);
assert.match(bullets,/p\.state==l\.state/);
assert.match(bullets,/p\.script==l\.animation\[0\]\.scriptIndex/);
assert.match(bullets,/p\.script==source\.scriptIndex/);
assert.match(bullets,/p\.state==b\.state&&p\.script==source\.scriptIndex&&b\.active_time\.current>=p\.age/);
assert.doesNotMatch(bullets,/p\.script==source\.scriptIndex&&p\.sprite==source\.activeSpriteIndex/,
  'An ordinary sprite animation must not suppress continuous bullet movement');
assert.match(bullets,/scale_x=presentation::lerp_world\(p\.scale_x,scale_x\)/);
assert.match(bullets,/scale_y=presentation::lerp_world\(p\.scale_y,scale_y\)/);

// Shared EffectState can be mutated by owners that run before EffectSystem's
// priority-13 calculation. The spell-card boss ring is the concrete case:
// SpellSystem rotates/follows it at priority 12. Capture the previous endpoint
// at the GameplayScene tick boundary, not after SpellSystem has already moved
// it, so ring center/angle/radius presentation interpolation has real endpoints.
assert.match(spell,/effect->angle=add_angle\(effect->angle/);
assert.match(effects,/draw\.angle=angle\(before\.angle,source\.angle\)/);
assert.match(effects,/draw\.center=\{presentation::lerp_world/);
const prepare=scene.slice(scene.indexOf('bool GameplayScene::prepare_frame('),scene.indexOf('bool GameplayScene::update('));
assert.match(prepare,/effect_system\.snapshot_presentation\(\);spell_drawing\.snapshot_presentation\(\);background_view\.snapshot_spell_presentation\(\);ascii\.snapshot_presentation\(ascii_context\);/);
assert(prepare.indexOf('effect_system.snapshot_presentation()')<prepare.indexOf('update_practice('),
  'The shared SDL application chain must snapshot before input/cheat/owner updates');
assert.match(app,/game\.prepare_frame\(supervisor\.input\.current/);
assert.equal((scene.match(/effect_system\.snapshot_presentation\(\)/g)||[]).length,1,
  'Standalone and application-owned chains must share one snapshot boundary');
assert.doesNotMatch(effects,/JobResult EffectSystem::update\(\)\{\s*snapshot_presentation\(\)/);
// Custom effect geometry has three discrete topologies. Continuous fields may
// only be blended when the whole path stays in one topology and vertex layout.
assert.match(effectGeometry,/interpolation_preserves_topology/);
assert.match(effectGeometry,/before_segments==current_segments&&before==current/);
assert.match(effectGeometry,/\(before_height>0\)==\(current_height>0\)/);
assert.match(effects,/before\.frequency=e\.frequency;before\.segments=e\.segments/);
assert.match(effects,/if\(!EffectGeometry::interpolation_preserves_topology\([^)]+\)\)return;/);

// Spell-card UI/background ANM VMs remain authoritative 60 Hz owners. High
// refresh draws copies sampled at the scene tick boundary and never executes
// ANM a second time. Lifecycle changes (script/visibility/time reset)
// snap instead of blending unrelated animation phases.
assert.match(spellDrawing,/before\.scriptIndex!=source\.scriptIndex\|\|before\.visible!=source\.visible/);
assert.match(spellDrawing,/source\.currentTimeInScript\.current<before\.currentTimeInScript\.current/);
assert.match(spellDrawing,/draw\.rotation=\{angle\(/);
assert.match(spellDrawing,/sample\.authored_uv_fields\(source\)/);
assert.match(spellDrawing,/presentation::render_only\?presented\[i\]:v\[i\]/);
assert.match(spellDrawing,/presentation_previous_panel_color=state\.spell_panel_color/);
assert.match(spellDrawing,/presentation::lerp\(float\(before\),float\(current\)\)/);
assert.doesNotMatch(spellDrawing,/anm\.execute|executor\.execute/);
assert.match(guiController,/&source==&display\.clock/);
assert.match(guiController,/owner_fields\|=presentation::VisualSample::Opacity/);
assert.match(backgroundView,/snapshot_spell_presentation/);
assert.match(backgroundView,/draw\.rotation=\{background_angle\(/);
assert.match(backgroundView,/presentation::render_only\).*presentation_spell_vm/s);
assert.match(backgroundObjects,/before\.authored_uv_fields\(raw\)/);
// Stage-script coordinate rebases are atomic discontinuities. In particular,
// stage 1's 511.5-unit wrap must never pass through the generic 512-unit
// proximity guard and create an intermediate camera view.
assert.match(backgroundScript,/case 5:if\(s\.jumped\).*presentation_camera_rebased=true/);
assert.match(backgroundScript,/if\(presentation_camera_rebased\)return result/);
assert(backgroundScript.indexOf('if(presentation_camera_rebased)return result')<backgroundScript.indexOf('const auto close='),
  'Authored camera rebases must snap before any distance-based interpolation decision');
assert.match(spellBackground,/view\.presentation_spell_vm\(0\)/);
assert.match(spellBackground,/view\.presentation_spell_vm\(1\)/);
assert.match(spellBackground,/effects\.presentation_copy\(\*effects\.group\(9\)\)/);
assert.match(spellBackground,/effects\.presentation_copy\(\*effects\.group\(10\)\)/);

// Gameplay ASCII overlays have cross-owner inputs too. Boss markers are moved
// by EnemySimulation before AsciiManager's own VM tick, while the humanity
// gauge and Mystia blindness are published from gameplay/ECL owners. Snapshot
// them at the scene boundary so their continuous geometry is presentable.
assert.match(ascii,/void AsciiManager::snapshot_presentation\(const AsciiContext& c\)/);
assert.match(ascii,/presentation_state\.boss_markers\[i\]=state\.boss_markers\[i\]\.pos/);
assert.match(ascii,/presentation_state\.gauge_vm\.capture\(state\.gauge\)/);
assert.match(ascii,/presentation_state\.cursor_vm\.apply\(s\.cursor,cursor_copy,presentation::world_alpha,fields\)/);
assert.match(ascii,/presentation_state\.human_icon_vm\.apply\(s\.human_icon,human_copy,presentation::world_alpha,fields\)/);
assert.match(ascii,/presentation_state\.youkai_icon_vm\.apply\(s\.youkai_icon,youkai_copy,presentation::world_alpha,fields\)/);
assert.match(ascii,/presentation_state\.gauge=c\.gauge/);
assert.match(ascii,/presentation_state\.blindness_radius=state\.blindness_radius/);
const asciiTick=ascii.slice(ascii.indexOf('void AsciiManager::tick_vms'),ascii.indexOf('bool AsciiManager::add_string'));
assert.doesNotMatch(asciiTick,/presentation_state\.boss_markers/);
assert.match(ascii,/presented_gauge=presentation::lerp_world/);
assert.match(ascii,/number\(presented_gauge\).*number\(112\)/);
assert.match(ascii,/presented_blindness_radius=presentation::lerp_world/);

console.log('TH08 high-refresh contract PASS: fixed 60 Hz simulation + owner-side presentation-only interpolation');
