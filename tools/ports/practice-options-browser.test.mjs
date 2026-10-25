import test from 'node:test';
import assert from 'node:assert/strict';
import {readFileSync} from 'node:fs';
import {resolve} from 'node:path';
import {runNativeCpp} from '../../launcher/tests/support/native-cpp.mjs';
import {TH08_PRACTICE_OPTIONS as contract} from '../../tests/adaptation/adapters/titles/th08-practice-options.mjs';
import {assertPracticeInputOptions} from './practice-options-browser.mjs';

function fixture(){
  const e={schema:'eagler-touhou/practice-input-options/1',arm:{available:true,armAccepted:true,armStatus:1,requestPending:true,requestGeneration:1},cleanupErrors:[],trials:[]};
  for(const [j,spec] of contract.filters.entries()){
    const trial={id:spec.id};e.trials.push(trial);
    for(const [k,[name,enabled]] of [['on',true],['off',false]].entries()){
      trial[name]={samples:[]};
      for(let i=0;i<6;i++){
      const words=Array(1024).fill(0),frame=100+j*50+k*10+i;words.splice(0,8,1,1,frame,frame,0,1,0,0);
      words[16]=1;words[17]=words[18]=frame;words[19]=enabled?spec.enabledMask:spec.disabledMask;words[spec.stateWord]=Number(enabled);
      const ui=Array(128).fill(0);ui[0]=ui[1]=1;ui[2]=ui[3]=frame;ui[110]=0x50554b31;ui[111]=1;
      if(spec.key){ui[112+(spec.vk>>>5)]=1<<(spec.vk&31);if(spec.vk===90)ui[106]=1;if(spec.vk===88)ui[107]=1;}
      const scene={available:true,accepted:true,acceptedBeforeFirstUpdate:true,firstTimelineWithheld:true,updateStarted:true,sessionWriteLocked:true,
        active:true,ready:true,replay:false,assisted:true,writePolicy:{initialization:'ram-score-overlay'},
        writeReceipt:{locked:true,loadScope:false,attempts:1,ramOverrides:1,discardedBytes:64,committedBytes:0,rejectedWrites:0},
        timelineCount:1,withheldTimelineUpdates:frame,enemies:0,bullets:0,lasers:0,stage:0,enemyFrame:frame,controlFrame:frame,requestGeneration:1};
      trial[name].samples.push({raw:{available:true,words},uiRaw:{available:true,words:ui},scene,identity:{app:1000,documentTimeOrigin:123,url:'https://localhost/runtime'}});
      }
    }
  }
  return e;
}

test('Input options require native physical input and opposite consumed-input controls',()=>{
  const e=fixture();assert.equal(assertPracticeInputOptions(e,contract).complete,false);
  for(const change of [e=>e.trials[0].on.samples[3].raw.words[19]=1,e=>e.trials[0].off.samples[3].raw.words[19]=0,
    e=>e.trials[1].on.samples[2].uiRaw.words[112]=0,e=>e.trials[2].on.samples[2].scene.writeReceipt.committedBytes=1,
    e=>e.trials[3].on.samples[3].identity.app++,e=>e.trials[0].on.samples[2].raw.words[18]++,e=>e.trials.pop()]){
    const bad=structuredClone(e);change(bad);assert.throws(()=>assertPracticeInputOptions(bad,contract));
  }
});

test('Another title maps its own consumer offsets without changing the common input checks',()=>{
  const e=fixture(),fields={inGame:90,frame:91,sequence:92,current:93,life:94};
  for(const trial of e.trials)for(const part of [trial.on,trial.off])for(const row of part.samples){
    for(const key of Object.keys(fields)){row.raw.words[fields[key]]=row.raw.words[contract.consumerFields[key]];row.raw.words[contract.consumerFields[key]]=0;}
  }
  assert.equal(assertPracticeInputOptions(e,{...contract,consumerFields:fields}).complete,false);
});

test('th08 registers every native Advanced semantic and both modes expose only their actual key paths',async()=>{
  const source=readFileSync(resolve('ports/th08/web/cpp/sdl/ThpracUi.cpp'),'utf8');
  const ids=[...source.matchAll(/(?:control_(?:checkbox|button|header)|observe_control)\((\d+),/g)].map(match=>Number(match[1]));
  assert.match(source,/observe_control\(27\+i,i,selected\)/);ids.push(...contract.controls.cpuChoices);
  assert.match(source,/auto\* cpu_owner=ImGui::GetCurrentWindow\(\);const bool cpu_open=ImGui::BeginCombo/);
  assert.match(source,/observe_control\(13,state\.rsqrt_cpu,[^\n]+true,cpu_owner\)/);
  assert.deepEqual(ids.toSorted((a,b)=>a-b),Object.values(contract.controls).flat().toSorted((a,b)=>a-b));
  const advanced=source.slice(source.indexOf('void draw_advanced('),source.indexOf('\nbool pressed('));
  assert.doesNotMatch(advanced,/ImGui::(?:Checkbox|Button|CollapsingHeader|SliderInt|DragInt|InputText|RadioButton)\(/,
    'Every new native option needs a registered semantic and consumer obligation');
  for(const title of ['th08','th08-mp']){
    const ui=readFileSync(resolve(`ports/${title}/web/cpp/sdl/ThpracUi.cpp`),'utf8');assert.ok(!ui.includes('ImGui::Checkbox(label(practice_TH_ADV_DISABLE_C_KEY_SAMETIME)'));
  }
  const directory=resolve('ports/th08/web/cpp/game');
  const output=await runNativeCpp(`#include "InputController.cpp"
#include <cassert>
#include <iostream>
int main(){th08::u8 keys[256]{};keys[67]=128;assert(th08::InputController::keyboard(keys,false)==0);
 keys[88]=128;assert(th08::InputController::keyboard(keys,false)==th08::Bomb);
 keys[88]=0;keys[90]=128;assert(th08::InputController::keyboard(keys,false)==th08::Shoot);
 keys[90]=0;keys[16]=128;assert(th08::InputController::keyboard(keys,false)==th08::Focus);
 std::cout<<"actual th08 X Z Shift mappings; no C gameplay input";}`,{includes:[directory]});
  assert.match(output,/no C gameplay input/);
});
