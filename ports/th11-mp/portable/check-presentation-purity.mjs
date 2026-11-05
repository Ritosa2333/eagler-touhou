// Draw-only purity audit for TH11. This is a source-level contract: it proves
// the non-idempotent write sites are gated by the render-only transaction and
// that the presentation sidecar never writes authoritative state. Runtime
// repeated-draw fingerprints remain a separate (not-run) Lab gate.
import assert from 'node:assert/strict';
import {readFileSync} from 'node:fs';
import {resolve} from 'node:path';
const root=resolve(import.meta.dirname,'..');
const read=path=>readFileSync(resolve(root,path),'utf8');

function body(text,signature){
  const start=text.indexOf(signature);
  assert(start>=0,'missing function '+signature);
  const open=text.indexOf('{',start);
  let depth=0;
  for(let i=open;i<text.length;++i){
    if(text[i]==='{')++depth;
    else if(text[i]==='}'&&--depth===0)return text.slice(open,i+1);
  }
  throw new Error('unterminated function '+signature);
}

const stageText=read('web/cpp/game/StageDraw.cpp');
const drawFunctions=[
  body(stageText,'bool Stage::draw_background('),
  body(stageText,'bool Stage::draw_foreground('),
  body(stageText,'bool Stage::draw_layer('),
].join('\n');
const gated=[
  /state\.effects_enabled\+\+/,
  /state\.fade_timer\.advance/,
  /state\.draw_flags\s*[|&]=/,
  /state\.tint\s*&=/,
];
const drawLines=drawFunctions.split('\n');
let checked=0;
for(let i=0;i<drawLines.length;++i){
  const line=drawLines[i];
  if(!gated.some(pattern=>pattern.test(line)))continue;
  ++checked;
  const window=drawLines.slice(Math.max(0,i-2),i+1).join('\n');
  assert(window.includes('draw_only'),'non-idempotent Stage draw write is not gated: '+line.trim());
}
assert(checked>=4,'expected at least four gated Stage write sites, saw '+checked);

const session=read('web/cpp/game/GameSession.cpp');
assert(session.includes('compositor.begin_presentation(high_refresh::alpha)'));
assert(session.includes('compositor.end_presentation()'));
assert(session.includes('struct RestoreCamera'),'camera window needs an exit guard');
assert(session.includes('presentation::begin_tick()'),'endpoints must be published at the tick boundary');

const sidecarText=read('web/cpp/game/AnmPresentation.cpp');
for(const field of ['position','script_position','child_position','rotation','scale','color','secondary_color','uv_offset'])
  assert(!new RegExp('current\\.'+field+'\\s*=').test(sidecarText),'sidecar writes authoritative '+field);

const renderer=read('web/cpp/game/AnmRenderer.cpp');
assert(renderer.includes('AnmVm smoothed;'),'renderer must draw a copy');
assert(renderer.includes('presentation::present(smoothed,vm)'));
assert(!/vm\.(position|script_position|child_position|rotation|scale|color|uv_offset)\s*=/.test(renderer),'renderer must not rewrite presentation input');

const bullets=read('web/cpp/game/BulletManager.cpp');
assert(bullets.includes('b->vm.script_position={float((double(b->position.x)+32)+192)'),'bullet draw must be recomputed from authoritative state');
const items=read('web/cpp/game/ItemManager.cpp');
assert(items.includes('vm.script_position={float((double(item.position.x)+32)+192)'),'item draw must be recomputed from authoritative state');
const player=read('web/cpp/game/PlayerFrame.cpp');
assert(player.includes('motion.body.position=screen(motion.state.position)'),'player draw must derive the scratch VM from state');

const app=read('web/cpp/sdl/Application.cpp');
const start=app.indexOf('bool presentation_draw');
const end=app.indexOf('bool return_to_title');
assert(start>0&&end>start,'presentation_draw body missing');
const presentation=app.slice(start,end);
assert(!presentation.includes('session.update'),'presentation draw must not update the session');
assert(!presentation.includes('audio'),'presentation draw must not advance audio');
assert(presentation.includes('high_refresh::begin')&&presentation.includes('high_refresh::end'));
assert(app.includes('#ifdef TH11_PRESENTATION_DIAGNOSTICS'),'diagnostic export must stay in the isolated profile');

console.log('TH11 presentation purity audit PASS ('+checked+' gated Stage writes)');
