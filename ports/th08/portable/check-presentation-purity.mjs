import {readFileSync} from 'node:fs';
import assert from 'node:assert/strict';
import {verifyBrowserRuntimePresent} from './presentation-method-component.mjs';

const read=path=>readFileSync(new URL(`../${path}`,import.meta.url),'utf8').replaceAll('\r','');

// Presentation sidecars must stay outside original TH08 ABI/state layouts.
for(const path of [
  'web/cpp/game/AnmLayout.hpp',
  'web/cpp/game/BulletState.hpp',
  'web/cpp/game/ItemPool.hpp',
  'web/cpp/game/EclVm.hpp',
  'web/cpp/game/GuiState.hpp',
  'web/cpp/game/PlayerMovement.hpp',
]) assert.doesNotMatch(read(path),/presentation_(?:previous|state|valid|alpha|render)/i,`${path} must remain authoritative/ABI-only`);

const app=read('web/cpp/game/GameApplication.cpp');
assert.match(app,/if\(presentation_only\)\{ascii\.state\.string_count=string_count/);
assert.match(app,/if\(!presentation_only\).*platform\.process_sounds\(\)/s);
assert.match(app,/if\(!presentation_only&&value<=0\)/);
assert.match(app,/if\(!presentation_only\)failed\|=invalid\(\)/);
assert.match(app,/ascii\.state\.color=ascii_color/);

const control=read('web/cpp/game/GameplayControl.cpp');
assert.match(control,/if\(!presentation::render_only&&menu\.pause_state\)menu\.pause_state=2/);

const gui=read('web/cpp/game/GuiView.cpp');
assert.match(gui,/if\(!presentation::render_only\)gui\.flags\.lives=gui\.flags\.bombs/);
assert.match(gui,/if\(!presentation::render_only&&gui\.previous_spell_seconds!=gui\.spell_seconds\)/);
assert.match(gui,/if\(!presentation::render_only&&!context\.paused/);

const items=read('web/cpp/game/ItemPool.cpp');
assert.match(items,/if\(!presentation::render_only\).*p->onscreen/s);
assert.match(items,/if\(presentation::render_only\)\{copy=source;[^\n]*before\.visual\.apply\(source,copy,presentation::world_alpha\);vm=&copy;\}/);

const background=read('web/cpp/game/BackgroundObjects.cpp');
assert.match(background,/if\(!presentation::render_only\)object\.flags\|=2/);
assert.match(background,/presentation::render_only.*presentation_vms/s);
const backgroundView=read('web/cpp/game/BackgroundView.cpp');
const backgroundScript=read('web/cpp/game/BackgroundScript.cpp');
assert.match(backgroundView,/saved_camera=s\.camera;s\.camera=script\.presentation_camera\(\);camera_override=true/);
assert.match(backgroundView,/if\(camera_override\)\{s\.camera=saved_camera;camera_override=false;\}/);
assert.match(backgroundScript,/presentation_previous_camera=s\.camera;presentation_camera_valid=true/);

const menus=read('web/cpp/game/UiMenus.cpp');
assert.match(menus,/snapshot_pause\(\)/);
assert.match(menus,/snapshot_retry\(\)/);
assert.match(menus,/presentation::render_only/);
assert.match(menus,/presentation::lerp\(before\.pos\.x,source\.pos\.x\)/);

const enemy=read('web/cpp/game/EnemySystem.cpp');
assert.match(enemy,/if\(!presentation::render_only\)failed\|=!ok/);

const bullets=read('web/cpp/game/BulletDrawing.cpp');
assert.match(bullets,/if\(presentation::render_only\)\{copy=source;vm=&copy;\}/);
assert.match(bullets,/if\(presentation::render_only\)\{body_copy=\*body;body=&body_copy;[\s\S]*?if\(smooth_laser\)[^;]*\.apply\(l\.animation\[0\],\*body,presentation::world_alpha,[^;]*\);[\s\S]*?\}/);

const player=read('web/cpp/game/PlayerSimulation.cpp');
assert.match(player,/if\(!presentation::render_only\)synchronize_shots\(\)/);
assert.match(player,/if\(presentation::render_only\)\{failed=failed_before;shots\.failure=shot_failure_before;\}/);

const title=read('web/cpp/game/TitleView.cpp');
assert.match(title,/saved_name_positions/);
assert.match(title,/if\(presentation::render_only\).*spellCardNameVms\[i\]\.pos=saved_name_positions\[i\]/s);

const spell=read('web/cpp/game/SpellDrawing.cpp');
assert.match(spell,/std::array<AnmVm,14> presented/);
assert.match(spell,/return presentation::render_only\?presented\[i\]:v\[i\]/);
assert.doesNotMatch(spell,/if\(presentation::render_only\).*v\[\d+\]\s*=/s);

const spellBackground=read('web/cpp/game/SpellBackground.cpp');
assert.match(spellBackground,/AnmVm presented_a,presented_b/);
assert.match(spellBackground,/if\(presentation::render_only\).*ap=&presented_a;bp=&presented_b/s);

assert.match(backgroundView,/if\(presentation::render_only\)\{auto vm=presentation_spell_vm/);

const loading=read('web/cpp/game/LoadingScreen.cpp');
assert.match(loading,/if\(!presentation::render_only\)phase=wrapping_add/);
assert.match(loading,/if\(presentation::render_only\)\{ascii\.state\.color=saved_color/);

const effects=read('web/cpp/game/EffectSystem.cpp');
assert.match(effects,/EffectState copy=\*e/);
assert.match(effects,/copy\.vertices=vertices\.data\(\)/);
assert.match(effects,/if\(presentation::render_only\).*EffectState copy=\*e/s);

const runtime=read('web/cpp/platform/BrowserRuntime.cpp');
await verifyBrowserRuntimePresent(runtime);

console.log('TH08 presentation source contract + Web present method component PASS');
