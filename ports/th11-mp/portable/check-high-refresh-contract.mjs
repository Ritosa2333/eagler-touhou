// Source contract for TH11 high-refresh scheduling. Pure text assertions so
// the gate runs without emcc, a browser or game data.
import assert from 'node:assert/strict';
import {readFileSync} from 'node:fs';
import {resolve} from 'node:path';
const root=resolve(import.meta.dirname,'..');
const read=path=>readFileSync(resolve(root,path),'utf8');

const cadence=read('portable/sdl/FrameCadence.hpp');
assert(cadence.includes('interpolation_alpha'),'FrameCadence must expose interpolation_alpha');
assert(/unsigned advance\(double seconds\)/.test(cadence),'FrameCadence::advance missing');
assert(cadence.includes('return 1;'),'FrameCadence must service at most one tick per callback');
assert(!/std::min\(4u/.test(cadence),'TH11 catch-up scheduling must not be restored');

const presentation=read('portable/sdl/PresentationCadence.hpp');
assert(presentation.includes('struct PresentationCadence'));
assert(presentation.includes('struct PresentationGate'));
assert(presentation.includes('fast_sample'));

const app=read('web/cpp/sdl/Application.cpp');
assert(app.includes('touhou::sdl::PresentationCadence presentation'));
assert(app.includes('touhou::sdl::PresentationGate presentation_gate'));
assert(app.includes('presentation_gate.advance(high,ticks!=0)'),'presentation gate must require a real tick');
assert(app.includes('presentation_draw(cadence.interpolation_alpha()'),'display frames must draw with the residual alpha');
assert(app.includes('limitPresentationTo60'),'60 Hz presentation kill switch missing');
assert(app.includes('bool interpolation_ready()'),'screen/freeze whitelist missing');
assert(app.includes('presentation_fps')&&app.includes('frame_statistics.label(shown)'),'the readout must follow presented frames, not logic ticks');
assert(app.includes('th11::browser::ThpracUi::simulation_interval'),'thprac custom speed must disable interpolation');

const loopStart=app.indexOf('void th11_loop_start(){');
const loopEnd=app.indexOf('void th11_touch_stick');
assert(loopStart>0&&loopEnd>loopStart,'loop body not found');
const loop=app.slice(loopStart,loopEnd);
const drawStart=loop.indexOf('presentation_draw');
assert(drawStart>0,'loop does not call presentation_draw');
// MP and ordinary configurations each own a loop branch. Check the shared
// draw transaction itself, rather than treating the later #else as draw-only.
const transactionStart=app.indexOf('bool presentation_draw(');
const transactionEnd=app.indexOf('bool return_to_title()',transactionStart);
assert(transactionStart>0&&transactionEnd>transactionStart,'display transaction not found');
const drawOnly=app.slice(transactionStart,transactionEnd);
assert(!drawOnly.includes('sample_and_tick'),'draw-only frames must not run simulation');
assert(!drawOnly.includes('session.update'),'draw-only frames must not update the session');
assert(!drawOnly.includes('playback.tick'),'draw-only frames must not consume Replay');
assert(app.includes('multiplayer_presentation.observe('),'MP display must observe completed native frames');
assert(app.includes('app.rollback_revisions'),'MP display must fence corrected history');

const drawSplit=app.indexOf('bool draw_session()');
const drawEnd=app.indexOf('bool presentation_draw');
assert(drawSplit>0&&drawEnd>drawSplit,'tick draw must be reusable by both paths');
const sessionDraw=app.slice(drawSplit,drawEnd);
assert(sessionDraw.includes('session.draw'),'semantic draw must go through GameSession::draw');

const renderer=read('web/cpp/game/AnmRenderer.cpp');
assert(renderer.includes('#include "AnmPresentation.hpp"'),'renderer must use the ANM sidecar');
assert(renderer.includes('presentation::present(smoothed,vm)'),'renderer must interpolate the draw copy');
assert(renderer.includes('continuous_position'),'interpolated world sprites must keep subpixel coordinates');

const sidecar=read('web/cpp/game/AnmPresentation.cpp');
assert(sidecar.includes('before.id!=current.id'),'ANM lifecycle identity gate missing');
assert(sidecar.includes('current.timer.current<before.timer'),'ANM timer rewind gate missing');
assert(sidecar.includes('high_refresh::position_limit'),'position teleport/rebase gate missing');
assert(sidecar.includes('before.scale.x*current.scale.x>=0'),'scale reflection gate missing');
assert(!/current\.(position|script_position|child_position|rotation|scale|color|uv_offset)=/.test(sidecar),'sidecar must never write authoritative VM state');

const session=read('web/cpp/game/GameSession.cpp');
assert(session.includes('presentation::begin_tick()'),'fixed tick must publish presentation endpoints');
assert(session.includes('compositor.begin_presentation'),'camera presentation window missing');
assert(session.includes('RestoreCamera'),'camera window must restore authoritative state on every exit');
assert(session.includes('renderer.continuous=(pass.kind==SceneDrawKind::Animation&&pass.layer<=19)'),'world pass classification missing');

const stage=read('web/cpp/game/StageDraw.cpp');
assert(stage.includes('const bool draw_only=high_refresh::render_only'),'stage draw purity gate missing');
assert(stage.includes('state.effects_enabled&&!draw_only'),'effects_enabled must not advance on draw-only frames');
assert(stage.includes('state.fade_timer.current>0&&!draw_only'),'stage fade timer must not advance on draw-only frames');
assert(stage.includes('display_camera(high_refresh::alpha)'),'stage camera must expose interpolated endpoints');
assert(stage.includes('camera_discontinuous(previous_camera,state.camera)'),'camera rebases must snap presentation');
assert(stage.includes('camera_discontinuity'),'authored hard camera sets must invalidate interpolation');
assert(session.includes('high_refresh::position_limit'),'scene passes must lower the continuous-position limit');

console.log('TH11 high-refresh source contract PASS');
