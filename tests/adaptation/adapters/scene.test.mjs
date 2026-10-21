import test from 'node:test';
import assert from 'node:assert/strict';
import {readFileSync} from 'node:fs';
import {resolve} from 'node:path';
import {runNativeCpp} from '../../../launcher/tests/support/native-cpp.mjs';
import {assertControlledScene,assertControlledSceneStimulus,assertControlledScenePauseBoundary,assertControlledSceneArmed,controlValidationScene,validationSceneRequest} from './scene.mjs';

const receipt={available:true,accepted:true,armStatus:2,requestGeneration:1,requestPending:false,
  acceptedBeforeFirstUpdate:true,firstTimelineWithheld:true,updateStarted:true,sessionWriteLocked:true,
  writeReceipt:{locked:true,loadScope:false,attempts:1,ramOverrides:1,discardedBytes:64,committedBytes:0,rejectedWrites:0},
  writePolicy:{initialization:'ram-score-overlay',source:'th08 initialize_score'},
  active:true,eligible:true,ready:true,busy:false,replay:false,assisted:true,
  enemies:0,bullets:0,lasers:0,timelineCount:1,stage:0,controlFrame:100,enemyFrame:98,withheldTimelineUpdates:20};

test('declared projectile and dialogue phases do not relax the movement-world invariant',()=>{
  const next={...receipt,controlFrame:101,enemyFrame:99,withheldTimelineUpdates:21,bullets:1};
  assert.equal(assertControlledSceneStimulus(next,receipt,{kind:'single-projectile'}),next);
  assert.throws(()=>assertControlledScene(next,receipt),/live hazards/);
  for(const patch of [{bullets:2},{enemies:1},{lasers:1},{busy:true}])
    assert.throws(()=>assertControlledSceneStimulus({...next,...patch},receipt,{kind:'single-projectile'}),/hazards|modal/);
  const modal={...receipt,busy:true};
  assert.equal(assertControlledSceneStimulus(modal,receipt,{kind:'dialogue'}),modal);
  assert.throws(()=>assertControlledScene(modal,receipt),/modal/);
  assert.throws(()=>assertControlledSceneStimulus({...modal,writeReceipt:{...receipt.writeReceipt,committedBytes:1}},receipt,{kind:'dialogue'}),/zero-commit/);
  assert.throws(()=>assertControlledSceneStimulus(receipt,receipt,{kind:'anything'}),/kind/);
});

test('a controlled scene requires accepted-before-timeline, write-lock and live hazard receipts',()=>{
  const next={...receipt,controlFrame:101,enemyFrame:99,withheldTimelineUpdates:21};
  assert.equal(assertControlledScene(next,receipt),next);
  for(const hazard of ['enemies','bullets','lasers'])assert.throws(()=>assertControlledScene({...next,[hazard]:1},receipt),/live hazards/);
  for(const field of ['controlFrame','enemyFrame','withheldTimelineUpdates'])
    assert.throws(()=>assertControlledScene({...next,[field]:receipt[field]},receipt),/logic tick|withhold/);
  for(const field of ['acceptedBeforeFirstUpdate','firstTimelineWithheld','updateStarted','sessionWriteLocked'])
    assert.throws(()=>assertControlledScene({...next,[field]:false}),/pre-timeline acceptance receipt/);
  for(const patch of [{locked:false},{loadScope:true},{committedBytes:1}])
    assert.throws(()=>assertControlledScene({...next,writeReceipt:{...next.writeReceipt,...patch}}),/zero-commit disposable-write receipt/);
  for(const patch of [{attempts:0},{ramOverrides:0},{discardedBytes:0}])
    assert.throws(()=>assertControlledScene({...next,writeReceipt:{...next.writeReceipt,...patch}}),/RAM score overlay receipt/);
  assert.equal(assertControlledScene({...next,writePolicy:{initialization:'none',source:'next world load'},writeReceipt:
    {locked:true,loadScope:false,attempts:0,ramOverrides:0,discardedBytes:0,committedBytes:0,rejectedWrites:0}}).active,true);
  assert.throws(()=>assertControlledScene({...next,accepted:false}),/request was rejected/);
  assert.throws(()=>assertControlledScene({...next,replay:true}),/pre-timeline acceptance receipt/);
  assert.throws(()=>assertControlledScene({...next,stage:1},receipt),/changed stage/);
  assert.throws(()=>assertControlledScene({...next,timelineCount:0}),/no original ECL/);
  assert.throws(()=>assertControlledScene({...next,withheldTimelineUpdates:0}),/has not withheld/);
  assert.throws(()=>assertControlledSceneArmed({available:true,armAccepted:false,armStatus:1,requestPending:true}),/not armed/);
  assert.equal(controlValidationScene(validationSceneRequest({game:'th08',variant:'multiplayer'})).available,false);
});

test('the serialized title adapter arms one pending request and decodes the unchanged snapshot ABI',()=>{
  const previous=globalThis.__th08Runtime;
  try{
    const memory={buffer:new ArrayBuffer(160)},calls=[];
    const snapshot=()=>new Uint32Array(memory.buffer,16,16);
    const lifecycle=()=>new Uint32Array(memory.buffer,88,8);
    const writeState=()=>new Uint32Array(memory.buffer,120,8);
    snapshot().set([1,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0]);
    lifecycle().set([1,0,0,0,0,0,0,0]);
    writeState().set([1,0,0,0,0,0,0,0]);
    globalThis.__th08Runtime={app:319,core:{memory,
      validation_scene_state(app){assert.equal(app,319);return 16;},
      validation_scene_arm_state(app){assert.equal(app,319);return 88;},
      validation_scene_write_state_v1(app){assert.equal(app,319);return 120;},
      validation_scene_arm(app){calls.push(['arm',app]);lifecycle().set([1,1,1,1,0,0,0,0]);return 1;}}};
    const request=validationSceneRequest({game:'th08',variant:'normal'});
    const arm=controlValidationScene({...request,action:'arm'});
    assert.equal(assertControlledSceneArmed(arm),arm);
    assert.deepEqual({armAccepted:arm.armAccepted,status:arm.armStatus,pending:arm.requestPending,
      active:arm.active,ready:arm.ready},{armAccepted:true,status:1,pending:true,active:false,ready:false});
    assert.deepEqual(calls,[['arm',319]]);

    snapshot().set([1,1,1,1,0,0,0,0,0,2,70,64,1,1,1,0]);
    lifecycle().set([1,2,1,0,1,1,1,1]);
    writeState().set([1,1,0,1,1,64,0,0]);
    const active=controlValidationScene(request);
    assert.equal(active.accepted,true);assert.equal(active.acceptedBeforeFirstUpdate,true);
    assert.equal(active.firstTimelineWithheld,true);assert.equal(active.sessionWriteLocked,true);
    assert.equal(assertControlledScene(active),active);
    for(const index of [1,2,3,4,5,14]){
      const prior=snapshot()[index];snapshot()[index]=2;
      assert.throws(()=>controlValidationScene(request),/flag must be 0 or 1/,
        `snapshot boolean word ${index} cannot use nonzero truthiness`);
      snapshot()[index]=prior;
    }
    for(const index of [3,4,5,6,7]){
      const prior=lifecycle()[index];lifecycle()[index]=2;
      assert.throws(()=>controlValidationScene(request),/flag must be 0 or 1/,
        `lifecycle boolean word ${index} cannot use nonzero truthiness`);
      lifecycle()[index]=prior;
    }
    for(const index of [1,2]){
      const prior=writeState()[index];writeState()[index]=2;
      assert.throws(()=>controlValidationScene(request),/flag must be 0 or 1/,
        `write receipt boolean word ${index} cannot use nonzero truthiness`);
      writeState()[index]=prior;
    }
    lifecycle()[1]=3;
    assert.throws(()=>assertControlledScene(controlValidationScene(request)),/request was rejected/);
    lifecycle()[1]=2;
    globalThis.__th08Runtime.core.validation_scene_state=()=>158;
    assert.throws(()=>controlValidationScene(request),/outside native memory/);
    globalThis.__th08Runtime.core.validation_scene_state=()=>16;
    globalThis.__th08Runtime.core.validation_scene_write_state_v1=()=>158;
    assert.throws(()=>controlValidationScene(request),/outside native memory/);
  }finally{
    if(previous===undefined)delete globalThis.__th08Runtime;else globalThis.__th08Runtime=previous;
  }
});

test('native pause entry and exit freeze the world without relaxing acceptance or write ownership',()=>{
  const paused={...receipt,busy:true,eligible:false,controlFrame:receipt.controlFrame+1};
  assert.equal(assertControlledScenePauseBoundary(paused,receipt,{before:{active:false},after:{active:true}}),paused);
  const resumed={...paused,busy:false,controlFrame:paused.controlFrame+1,enemyFrame:paused.enemyFrame+1,
    withheldTimelineUpdates:paused.withheldTimelineUpdates+1};
  assert.equal(assertControlledScene(resumed,paused),resumed);
  assert.throws(()=>assertControlledScenePauseBoundary(resumed,paused,{before:{active:true},after:{active:false}}),/independent native pause/);
  assert.throws(()=>assertControlledScene(paused),/modal owner/,'movement keeps its strict modal gate');
  assert.throws(()=>assertControlledScenePauseBoundary(paused,receipt,{before:{active:false},after:{active:false}}),/independent native pause/);
  for(const key of ['enemyFrame','withheldTimelineUpdates','controlFrame'])
    assert.throws(()=>assertControlledScenePauseBoundary({...paused,[key]:paused[key]+1},receipt,{after:{active:true}}));
  for(const patch of [{enemies:1},{acceptedBeforeFirstUpdate:false},{ready:false},
    {writeReceipt:{...receipt.writeReceipt,committedBytes:1}}])
    assert.throws(()=>assertControlledScenePauseBoundary({...paused,...patch},receipt,{after:{active:true}}));
});

test('the production gate accepts only a fresh normal world before its first timeline and reset restores defaults',async()=>{
  const output=await runNativeCpp(`#include "ValidationScene.hpp"
#include <cassert>
#include <initializer_list>
#include <iostream>
int main(){
  constexpr unsigned normal=0,practice=1,demo=2,replay=8,game_state=0x60,spell_practice=0x4000;
  assert(th08::validation_scene_request_eligible(true,true,normal));
  assert(!th08::validation_scene_request_eligible(false,true,normal));
  assert(!th08::validation_scene_request_eligible(true,false,normal));
  for(unsigned flags:{practice,demo,replay,game_state,spell_practice})assert(!th08::validation_scene_request_eligible(true,true,flags));
  th08::ValidationSceneGate ordinary;
  assert(!ordinary.enabled()&&!ordinary.update_started()&&ordinary.allow_timeline()&&!ordinary.first_timeline_withheld());
  th08::ValidationSceneGate gate;
  assert(!gate.begin_before_first_update(false,false,false,0,0,0));
  assert(!gate.begin_before_first_update(true,true,false,0,0,0));
  assert(!gate.begin_before_first_update(true,false,true,0,0,0));
  assert(!gate.begin_before_first_update(true,false,false,1,0,0));
  assert(!gate.begin_before_first_update(true,false,false,0,1,0));
  assert(!gate.begin_before_first_update(true,false,false,0,0,1));
  assert(gate.begin_before_first_update(true,false,false,0,0,0));
  assert(gate.accepted_before_first_update()&&!gate.update_started());
  gate.note_update_started();assert(gate.update_started());
  assert(!gate.allow_timeline()&&gate.first_timeline_withheld()&&gate.withheld_updates()==1);
  for(int tick=0;tick<599;++tick){gate.note_update_started();assert(!gate.allow_timeline());}
  assert(gate.withheld_updates()==600);
  gate=th08::ValidationSceneGate{};
  assert(!gate.enabled()&&!gate.update_started()&&gate.withheld_updates()==0&&gate.allow_timeline());
  gate.note_update_started();assert(gate.allow_timeline()&&!gate.first_timeline_withheld());
  assert(gate.begin(true,false,false,0,0,0));
  assert(!gate.accepted_before_first_update()&&!gate.allow_timeline()&&!gate.first_timeline_withheld());
  gate=th08::ValidationSceneGate{};gate.note_update_started();
  assert(!gate.begin_before_first_update(true,false,false,0,0,0));
  std::cout<<"normal route only; pre-first-timeline acceptance; reset restores ordinary execution";
}`,{includes:[resolve(import.meta.dirname,'../../../ports/th08/web/cpp/game')]});
  assert.match(output,/pre-first-timeline acceptance/);
});

test('the Deathbomb projectile stimulus is a fixture-gated one-shot and records only a real death-window collision',async()=>{
  const output=await runNativeCpp(`#include "DeathbombStimulus.hpp"
#include <cassert>
#include <iostream>
int main(){
  const auto emission=th08::validation_deathbomb_emission_spec(192,384,.2f);
  assert(emission.sprite==0&&emission.color==0&&emission.count==1&&emission.layers==1&&emission.pattern==0);
  assert(emission.x==192&&emission.y==384&&emission.z==.2f);
  assert(emission.speed==0&&emission.ending_speed==0&&emission.spread==0&&emission.angle==0);
  assert(th08::deathbomb_stimulus_version==1&&th08::deathbomb_stimulus_word_count==36);
  const auto allowed=[](bool scene,bool lock,bool ready,bool normal,bool quiet,bool type,int life,bool requested){
    return th08::validation_deathbomb_stimulus_eligible(scene,lock,ready,normal,quiet,type,life,requested);
  };
  assert(allowed(true,true,true,true,true,true,0,false));
  assert(!allowed(false,true,true,true,true,true,0,false));
  assert(!allowed(true,false,true,true,true,true,0,false));
  assert(!allowed(true,true,false,true,true,true,0,false));
  assert(!allowed(true,true,true,false,true,true,0,false));
  assert(!allowed(true,true,true,true,false,true,0,false));
  assert(!allowed(true,true,true,true,true,false,0,false));
  assert(!allowed(true,true,true,true,true,true,3,false));
  assert(!allowed(true,true,true,true,true,true,0,true));

  th08::DeathbombStimulusReceiptV1 hit;
  hit.refuse();assert(hit.request_status==th08::DeathbombStimulusReceiptV1::Refused&&hit.request_sequence==0);
  assert(hit.accept()&&hit.request_status==th08::DeathbombStimulusReceiptV1::Accepted&&hit.request_sequence==1);
  assert(!hit.accept());
  assert(!hit.observe_collision(0,2,8,1,12,false));
  assert(!hit.observe_collision(0,2,8,2,12,true));
  assert(!hit.observe_collision(0,0,8,1,12,true));
  assert(!hit.observe_collision(0,2,0,1,12,true));
  assert(hit.observe_collision(0,2,8,1,12,true));
  assert(hit.collision_source==th08::DeathbombStimulusReceiptV1::BulletSystemProjectile);
  assert(hit.collision_sequence==1&&hit.consumer_sequence_at_collision==12);
  assert(hit.life_state_before_collision==0&&hit.life_state_after_collision==2&&hit.collision_result==1);
  assert(hit.projectile_status==th08::DeathbombStimulusReceiptV1::Collided);
  hit.retire_without_hit();assert(hit.projectile_status==th08::DeathbombStimulusReceiptV1::Collided);

  th08::DeathbombStimulusReceiptV1 miss;assert(miss.accept());miss.retire_without_hit();
  assert(miss.projectile_status==th08::DeathbombStimulusReceiptV1::RetiredWithoutHit);
  std::cout<<"one real-template projectile; locked fixture only; collision receipt requires completed PlayerLife death window";
}` ,{includes:[resolve(import.meta.dirname,'../../../ports/th08/web/cpp/game')]});
  assert.match(output,/one real-template projectile; locked fixture only/);

  const bulletSystem=readFileSync(resolve(import.meta.dirname,'../../../ports/th08/web/cpp/game/BulletSystem.cpp'),'utf8');
  const bulletCreation=readFileSync(resolve(import.meta.dirname,'../../../ports/th08/web/cpp/game/BulletCreation.cpp'),'utf8');
  const exports=readFileSync(resolve(import.meta.dirname,'../../../ports/th08/web/cpp/platform/BrowserExports.cpp'),'utf8');
  assert.match(bulletSystem,/validation_deathbomb_emission_spec\(position\.x,position\.y,position\.z\)/);
  assert.match(bulletSystem,/emission\.sprite=i16\(spec\.sprite\).*emission\.count=i16\(spec\.count\)/s);
  assert.match(bulletSystem,/emit\(emission\)/,'the fixture calls the production BulletSystem::emit path');
  assert.match(bulletCreation,/e\.type=&state\.templates\.types\[e\.sprite\]/,'the emitted sprite resolves through the production loaded template table');
  assert.match(bulletSystem,/candidate->sprites\.animation\[0\]\.loadedSprite!=state\.templates\.types\[0\]\.animation\[0\]\.loadedSprite/,
    'the request receipt requires the created bullet to carry the expected loaded sprite');
  assert.match(bulletSystem,/kind==2&&deathbomb_projectile==&bullet/,'the receipt is bound to the emitted slot in the ordinary projectile collision callback');
  assert.match(bulletSystem,/observe_collision\(before,state\.life\.state,state\.life\.predead_count,result,input\.sequence,input\.valid\)/);
  assert.match(exports,/EX\("validation_deathbomb_hit_v1"\)/);
  assert.match(exports,/EX\("validation_deathbomb_state_v1"\)/);
  assert.match(exports,/receipt\.locked&&!receipt\.load_scope&&receipt\.committed_bytes==0/);
  assert.doesNotMatch(exports,/out\[35\]\s*=/,'word 35 remains the reserved zero word in the 36-word ABI');
});

test('every gameplay load opens a write scope and the production policy supports initial load and restart',async()=>{
  const gameApplication=readFileSync(resolve(import.meta.dirname,'../../../ports/th08/web/cpp/game/GameApplication.cpp'),'utf8');
  const enterGameStart=gameApplication.indexOf('bool GameApplication::enter_game(){');
  const enterGameEnd=gameApplication.indexOf('\nResultContext GameApplication::result_context()',enterGameStart);
  assert.notEqual(enterGameStart,-1,'GameApplication::enter_game is present');
  assert.notEqual(enterGameEnd,-1,'enter_game ends before result_context');
  const enterGame=gameApplication.slice(enterGameStart,enterGameEnd);
  assert.equal((enterGame.match(/game\.load\(/g)||[]).length,1,'the production entry has one load boundary');
  assert.match(enterGame,/platform\.validation_scene_load_scope\(true\);\s*const bool loaded=game\.load\(prepared,true\);\s*platform\.validation_scene_load_scope\(false\);/,
    'every title load and restart uses the same scoped write boundary');
  assert.doesNotMatch(enterGame,/if\s*\(\s*validation_requested\s*\)\s*platform\.validation_scene_load_scope/,
    'write scope does not depend on a one-time validation request');

  const output=await runNativeCpp(`#include "DisposableWritePolicy.hpp"
#include <cassert>
#include <iostream>
int main(){
  th08::DisposableWritePolicy policy;int ram_puts=0,file_saves=0;bool world_accepted=false;
  const auto write=[&](unsigned bytes,bool score){return policy.write(bytes,score,
    [&]{++ram_puts;++file_saves;return true;},[&]{++ram_puts;return true;});};
  const auto load=[&](unsigned bytes,bool score){
    policy.load_scope(true);const bool loaded=write(bytes,score);policy.load_scope(false);return loaded;
  };
  assert(load(12,true)&&ram_puts==1&&file_saves==1);
  policy.arm();assert(policy.receipt().locked&&policy.receipt().attempts==0);
  assert(!write(9,true)&&ram_puts==1&&file_saves==1);
  assert(!world_accepted&&load(64,true));
  assert(ram_puts==2&&file_saves==1);
  assert(!world_accepted&&load(72,true));
  assert(ram_puts==3&&file_saves==1);
  world_accepted=true;
  assert(!write(28,true));
  policy.load_scope(true);assert(!write(16,false));policy.load_scope(false);
  const auto& receipt=policy.receipt();
  assert(receipt.locked&&!receipt.load_scope&&receipt.attempts==5&&receipt.ram_overrides==2);
  assert(receipt.discarded_bytes==136&&receipt.committed_bytes==0&&receipt.rejected_writes==3);
  assert(file_saves==1&&ram_puts==3);
  std::cout<<"initial load and restart use RAM score overlay; no armed-session FileDevice save";
}`,{includes:[resolve(import.meta.dirname,'../../../ports/th08/web/cpp/platform')]});
  assert.match(output,/initial load and restart use RAM score overlay/);
});
