import test from 'node:test';
import assert from 'node:assert/strict';
import {TH08_PRACTICE_TRAINER as contract,decodeTh08PracticeEffects} from '../../tests/adaptation/adapters/titles/th08-practice-effects.mjs';
import {TH08_PRACTICE_OPTIONS} from '../../tests/adaptation/adapters/titles/th08-practice-options.mjs';
import {assertPracticeTrainerTrial,assertPracticeTrainerMatrix} from './practice-trainer-scene.mjs';

function fixture(trial){
  const e={schema:'eagler-touhou/practice-trainer-trial/1',id:trial.id,arm:{available:true,armAccepted:true,armStatus:1,requestPending:true,requestGeneration:1},
    physicalBomb:['bomb','hit-bomb','late-hit-bomb'].includes(trial.kind),stimulus:{accepted:true},samples:[]};
  for(let i=0;i<12;i++){
    const lives=trial.startLives??4,w=Array(48).fill(0);w[0]=w[1]=w[19]=w[41]=w[44]=1;w[2]=w[18]=300+i;w[4]=trial.mask;w[5]=lives;w[6]=3;w[7]=96;
    if(i&&trial.stimulus==='hit'){w[20]=301;w[21]=0;w[22]=trial.kind==='invincible'?0:2;w[23]=w[24]=1;}
    if(i&&trial.kind==='miss'){w[13]=2;w[14]=10-i;w[10]=1;}
    if(i>=3&&trial.kind==='miss'){w[8]=1;w[14]=0;w[7]=trial.expectedPower??(trial.keepPower?96:80);}
    if(i===11&&trial.kind==='miss'){w[13]=1;w[5]=trial.keepLife?lives:lives-1;}
    if(i&&['auto-bomb','hit-bomb'].includes(trial.kind)){w[10]=1;w[13]=i===1?2:3;if(i>1){w[9]=w[15]=1;w[6]=2;}}
    if(i&&trial.kind==='late-hit-bomb'){w[10]=1;w[13]=i<3?2:3;if(i>2){w[9]=w[15]=1;w[6]=1;w[16]=w[17]=2;}}
    if(i&&trial.kind==='bomb'){w[9]=w[15]=1;w[6]=trial.keepBombs?3:2;}
    const s=decodeTh08PracticeEffects(w),scene={available:true,accepted:true,acceptedBeforeFirstUpdate:true,firstTimelineWithheld:true,
      updateStarted:true,sessionWriteLocked:true,active:true,ready:true,replay:false,assisted:true,writePolicy:{initialization:'ram-score-overlay'},
      writeReceipt:{locked:true,loadScope:false,attempts:1,ramOverrides:1,discardedBytes:64,committedBytes:0,rejectedWrites:0},
      timelineCount:1,withheldTimelineUpdates:300+i,enemies:0,bullets:0,lasers:0,busy:false,stage:0,enemyFrame:300+i,controlFrame:300+i,requestGeneration:1};
    const ui=Array(128).fill(0);ui[0]=ui[1]=1;ui[110]=0x50554b31;ui[111]=1;
    if(e.physicalBomb&&i===2){ui[114]=1<<24;ui[107]=1;}
    e.samples.push({label:i===0?'before':i===11?'after':'tick',raw:{available:true,words:w},uiRaw:{available:true,words:ui},tracker:[1,2,s.deaths,s.dissolve,s.bombsUsed,s.spellCaptures,s.lastSpellCaptures],
      scene,identity:{app:1000,documentTimeOrigin:123,url:'https://localhost/runtime'}});
  }
  if(trial.options){const words=Array(1024).fill(0);for(const o of trial.options)words[o.stateWord]=o.value;e.optionsSetup={raw:{available:true,words},identity:e.samples[0].identity};}
  return e;
}
test('Mapped infinite lives consumes reserves and protects the zero-reserve boundary',()=>{
  for(const trial of TH08_PRACTICE_OPTIONS.gameplay.cases){
    const e=fixture(trial);assert.equal(assertPracticeTrainerTrial(e,{contract,trial}).complete,true);
    const bad=structuredClone(e);bad.optionsSetup.raw.words[43]=0;assert.throws(()=>assertPracticeTrainerTrial(bad,{contract,trial}));
  }
});
for(const trial of contract.cases)test('Trainer '+trial.id+' requires its actual consumer effect',()=>{
  const e=fixture(trial);assert.equal(assertPracticeTrainerTrial(e,{contract,trial}).complete,true);
  for(const change of [e=>e.samples[2].identity.app++,e=>e.samples[3].scene.writeReceipt.committedBytes++,e=>e.samples[2].tracker[4]++,
    e=>e.samples[2].raw.words[18]++,e=>e.samples[2].raw.words[4]^=1]){
    const bad=structuredClone(e);change(bad);assert.throws(()=>assertPracticeTrainerTrial(bad,{contract,trial}));
  }
  const noEffect=structuredClone(e);
  if(trial.kind==='invincible')noEffect.samples.forEach(s=>s.raw.words[24]=0);
  else if(trial.kind==='miss')noEffect.samples.forEach(s=>s.raw.words[8]=0);
  else noEffect.samples.forEach(s=>s.raw.words[9]=0);
  assert.throws(()=>assertPracticeTrainerTrial(noEffect,{contract,trial}),'A changed flag without its consumer must fail');
});

test('Trainer matrix requires all independent controls and never certifies the whole Practice inventory',()=>{
  const rows=contract.cases.map(trial=>({trainerId:trial.id,status:'passed',trainer:fixture(trial)}));
  assert.equal(assertPracticeTrainerMatrix(rows,contract).complete,false);
  assert.throws(()=>assertPracticeTrainerMatrix(rows.slice(1),contract));
  const bad=structuredClone(rows);bad[1]=bad[0];assert.throws(()=>assertPracticeTrainerMatrix(bad,contract));
  const auto=contract.cases.find(t=>t.kind==='auto-bomb'),e=fixture(auto);e.samples[3].uiRaw.words[114]|=1<<24;e.samples[3].uiRaw.words[107]=1;
  assert.throws(()=>assertPracticeTrainerTrial(e,{contract,trial:auto}),/real X key/);
});

test('Trainer positive controls distinguish each protected resource and forbid physical auto-bomb assistance',()=>{
  for(const id of ['infinite-lives','keep-power','infinite-bombs','automatic-bomb']){
    const trial=contract.cases.find(t=>t.id===id),e=fixture(trial);
    if(id==='infinite-lives')e.samples.at(-1).raw.words[5]--;
    if(id==='keep-power')e.samples[3].raw.words[7]-=16;
    if(id==='infinite-bombs')e.samples.at(-1).raw.words[6]--;
    if(id==='automatic-bomb')e.physicalBomb=true;
    assert.throws(()=>assertPracticeTrainerTrial(e,{contract,trial}),id);
  }
});
