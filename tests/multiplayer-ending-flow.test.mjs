import test from 'node:test';
import assert from 'node:assert/strict';
import {readFileSync} from 'node:fs';
import {runInNewContext} from 'node:vm';
import {TitleScenes} from '../src/title-scenes.mjs';

const source=readFileSync(new URL('../site/game.mjs',import.meta.url),'utf8');
for(const status of ['ending','result','playing'])test(`cooperative ${status} routes through the appropriate clear flow`,async()=>{
 const calls=[],model=new TitleScenes({profile:{data:{lastName:'TEAM'},rankings:()=>[]}}),story={character:2,shot:1};
 const context={multiplayer:{state:()=>({stage:6,score:123456}),readOnly:false},session:{character:0,shot:0,difficulty:1,storyLoadout:story},model,
  stopEffects(){},pauseAudio(){},clearKeys(){},readJSON:()=>({status,totalFrames:3600}),profile:{data:{lastName:'TEAM'}},
  playEnding:(result,loadout)=>{assert.equal(loadout,story);assert(result.clear);calls.push('ending');},returnMultiplayerRoom(){}};
 const from=source.indexOf('async function finishMultiplayerRun('),to=source.indexOf('\nasync function multiplayerStage(',from);
 await runInNewContext(source.slice(from,to)+';finishMultiplayerRun()',context);
 if(status==='ending'){assert.deepEqual(calls,['ending']);assert.equal(model.state.scene,'title');}
 else {assert.equal(calls.length,0);assert.equal(model.state.scene,'name');assert.equal(context.pendingResult.clear,status==='result');}
});

test('ending input keeps advancing with the cooperative runtime still active',()=>{
 let frames=0,networkTicks=0;const model=new TitleScenes({profile:{data:{lastName:'TEAM'},rankings:()=>[]}});
 const timeline={message:{id:11},finished:false,tick(){if(++frames===3)this.finished=true;}};
 const seen=[],context={roomPreview:false,fatalError:false,ready:true,busy:false,ending:timeline,multiplayer:{active:true,tick(){networkTicks++;}},multiplayerResult:true,
  updatePracticeUI(){},input:()=>({ui:1,held:0,pressed:1}),padInput:()=>({value:0}),touch:{sample:()=>({held:0,pressed:0})},touchState:()=>({}),previousPadUI:0,
  profile:{recordEndingSeen:id=>seen.push(id),save:async()=>{}},model,presentation:{tick(){}},fail:error=>{throw error;},
  pendingResult:{mode:'multiplayer',character:2,shot:1,difficulty:1,score:123456,clear:true,canSaveReplay:true}};
 const from=source.indexOf('function tick(){'),to=source.indexOf('\n  if(multiplayer?.active){',from);
 const tick=runInNewContext(source.slice(from,to)+'\n};tick',context);
 tick();tick();assert.equal(context.ending,timeline);assert.notEqual(model.state.scene,'name');tick();
 assert.equal(context.ending,null);assert.deepEqual(seen,[11]);assert.equal(model.state.scene,'name');assert.equal(model.state.nameContext,'title');model.completeName();assert.equal(model.state.scene,'replay-save');assert.equal(model.state.result.mode,'multiplayer');assert.equal(networkTicks,0);
});

