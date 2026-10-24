import test from 'node:test';
import assert from 'node:assert/strict';
import {TitleScenes,TITLE_CHOICES,NAME_CHARACTERS} from '../src/title-scenes.mjs';

const frames=(model,n,mask=0)=>{for(let i=0;i<n;i++)model.tick(mask);};
const active=(options={})=>{const model=new TitleScenes(options);frames(model,191);model.drainActions();return model;};
const press=(model,mask)=>{model.tick(0);model.tick(mask);model.tick(0);};
const choose=(model,index,delay=7)=>{frames(model,delay);assert.equal(model.choose(index),true);};
const actions=(model,type)=>model.drainActions().filter(a=>a.type===type);

test('retail title has eight entries, no fabricated manual root, and input unlocks after intro',()=>{
  const model=new TitleScenes();assert.deepEqual(model.state.choices.map(c=>c.label),TITLE_CHOICES);
  assert.equal(model.choose(0),false);frames(model,190);assert.equal(model.ready(),false);model.tick(0);assert.equal(model.ready(),true);
  assert.equal(model.state.events[0].selectionInterrupt,17);
});

test('menu keyboard repeats at held frame26 then every8, skipping locked Extra',()=>{
  const model=active();model.tick(32);assert.equal(model.state.selection,2);
  frames(model,24,32);assert.equal(model.state.selection,2);
  model.tick(32);assert.equal(model.state.selection,3);
  frames(model,7,32);assert.equal(model.state.selection,3);
  model.tick(32);assert.equal(model.state.selection,4);
  assert.equal(model.state.events[0].selectionInterrupt,11);
});

test('Start keeps chosen settings and waits before handing gameplay to runtime',()=>{
  const model=active();model.choose(0);frames(model,19);assert.equal(model.state.scene,'title');model.tick(0);
  assert.equal(model.state.scene,'difficulty');choose(model,3);choose(model,2);choose(model,1);
  assert.equal(model.state.scene,'shot');assert.equal(actions(model,'start').length,0);frames(model,40);
  assert.deepEqual(actions(model,'start'),[{type:'start',mode:'story',difficulty:3,character:2,shot:1,stage:1}]);
});

test('Extra allows only cleared character/equipment combinations',()=>{
  const profile={canExtra:(c,s)=>c===2&&s===1};const model=active({profile});
  assert.equal(model.state.choices[1].disabled,false);model.choose(1);frames(model,20);
  assert.equal(model.state.scene,'character');assert.equal(model.choose(0),false);choose(model,2);
  assert.equal(model.state.scene,'shot');assert.equal(model.state.selection,1);assert.equal(model.choose(0),false);choose(model,1);frames(model,40);
  assert.deepEqual(actions(model,'start'),[{type:'start',mode:'extra',difficulty:4,character:2,shot:1,stage:7}]);
});

test('Practice consults selected loadout and difficulty unlocks and starts the chosen single stage',()=>{
  const requests=[];const profile={canPractice:(c,s,d,stage)=>{requests.push([c,s,d,stage]);return stage===1||stage===4;}};
  const model=active({profile});model.choose(2);frames(model,20);choose(model,2);choose(model,1);choose(model,0);
  assert.equal(model.state.scene,'practice');assert.equal(model.choose(5),false);choose(model,3);frames(model,40);
  assert.ok(requests.some(v=>v.join(',')==='1,0,2,4'));
  assert.deepEqual(actions(model,'start'),[{type:'start',mode:'practice',difficulty:2,character:1,shot:0,stage:4}]);
});

test('Demo uses original reverse file order at1800 idle frames and Z/X/Esc/Enter to exit',()=>{
  const model=new TitleScenes();model.drainActions();frames(model,1799);assert.equal(model.state.scene,'title');model.tick(0);
  assert.deepEqual(actions(model,'demo'),[{type:'demo',id:'demo2.rpy'}]);
  model.tick(16);assert.equal(model.state.scene,'demo');model.tick(1);assert.equal(model.state.scene,'title');model.tick(0);
  frames(model,1799);assert.deepEqual(actions(model,'demo'),[{type:'demo',id:'demo1.rpy'}]);
  frames(model,3540);assert.deepEqual(actions(model,'demoFade'),[{type:'demoFade',frames:60}]);frames(model,60);assert.equal(model.state.scene,'title');
  frames(model,1800);assert.deepEqual(actions(model,'demo'),[{type:'demo',id:'demo0.rpy'}]);
});

test('idle title activity resets demo clock and Esc selects Quit before leaving',()=>{
  const model=active();frames(model,1500);model.tick(16);assert.equal(model.idleFrames,0);frames(model,1700);assert.equal(model.state.scene,'title');
  press(model,256);assert.equal(model.state.selection,7);press(model,256);frames(model,20);assert.equal(model.state.scene,'quit');
  choose(model,0);assert.equal(model.state.scene,'title');
});

test('Replay excludes internal demos and retains selected stage/id after transition',()=>{
  const corpus=[{id:'demo0.rpy',provenance:'built-in-demo',stages:[{number:4}]},{id:'th12_11.rpy',name:'ATB',stages:[{number:1},{number:6}]}];
  const model=active({corpus});model.choose(3);frames(model,20);assert.equal(model.state.choices.length,1);choose(model,0);choose(model,1);frames(model,40);
  assert.deepEqual(actions(model,'playReplay'),[{type:'playReplay',id:'th12_11.rpy',stage:6}]);
});

test('native options volumes are bounded5-percent steps and defaults restore100/80',()=>{
  const model=active();model.choose(6);frames(model,20);frames(model,7);for(let i=0;i<25;++i)press(model,64);
  assert.equal(model.settings.bgmVolume,0);model.select(1);for(let i=0;i<25;++i)press(model,128);assert.equal(model.settings.seVolume,100);
  model.choose(3);assert.equal(model.settings.bgmVolume,100);assert.equal(model.settings.seVolume,80);
  model.choose(2);frames(model,7);model.choose(0);model.key('KeyC',true);assert.equal(model.settings.keyMap.shoot,'KeyC');
  model.choose(6);assert.equal(model.state.scene,'options');model.key('F1',true);assert.equal(model.state.scene,'manual');press(model,256);assert.equal(model.state.scene,'options');
});

test('Pause is safe for replay, continue restrictions preserve mode, and scene transitions emit runtime commands',()=>{
  const model=active();model.openPause({mode:'replay',stage:6});assert.deepEqual(model.state.choices.map(c=>c.id),['resume','title','stop-replay']);
  model.choose(0);assert.deepEqual(actions(model,'resume'),[{type:'resume'}]);
  model.openContinue({mode:'story',continues:2,stage:4});model.choose(0);assert.equal(actions(model,'continue')[0].stage,4);
  model.openContinue({mode:'extra'});assert.equal(model.state.scene,'result');
  model.openContinue({mode:'story',continues:9});assert.equal(model.state.scene,'continue');
});

test('native pause exposes save-exit, confirms destructive run choices, and forbids continued saves',()=>{
  const model=active();model.openPause({mode:'story',stage:3,canSaveReplay:true});
  assert.deepEqual(model.state.choices.map(c=>c.id),['resume','title','save-exit','retry']);
  model.choose(3);assert.equal(model.state.scene,'pause-confirm');assert.equal(model.state.selection,1);
  model.choose(1);assert.equal(model.state.scene,'pause');assert.equal(model.state.selection,3);
  model.choose(3);model.choose(0);assert.equal(actions(model,'retry')[0].stage,3);
  model.openPause({mode:'story',canSaveReplay:false});assert.equal(model.choose(2),false);
  model.openPause({mode:'story',canSaveReplay:true});model.choose(2);model.choose(0);assert.equal(actions(model,'saveExit').length,1);
});

test('Manual restores result/name/replay-stage payload instead of discarding in-progress flow',()=>{
  const model=active();model.finish({kind:'clear',score:12345,canSaveReplay:true});frames(model,30);
  model.key('F1');assert.equal(model.state.scene,'manual');model.cancel();assert.equal(model.state.result.score,12345);
  model.choose(0);assert.equal(model.state.scene,'name');model.key('F1');model.cancel();assert.equal(model.state.name,'PLAYER');
  model.choose(90);assert.equal(actions(model,'saveName')[0].result.score,12345);
  const replay={id:'th12_11.rpy',stages:[{number:1},{number:6}]};model.enter('replay-stage',1,{replay});frames(model,7);
  model.key('F1');model.cancel();assert.equal(model.state.replay.id,replay.id);assert.equal(model.state.selection,1);
  model.choose(1);frames(model,40);assert.equal(actions(model,'playReplay')[0].stage,6);
});

test('result name entry uses retail88-character grid,8-character limit and25 replay slots',()=>{
  assert.equal(NAME_CHARACTERS.length,88);const model=active({profile:{data:{lastName:'ATB'}}});
  model.finish({kind:'clear',score:123456,canSaveReplay:true});frames(model,30);model.choose(0);
  assert.equal(model.state.scene,'name');assert.equal(model.state.choices.length,91);assert.equal(model.state.name,'ATB');
  for(let i=0;i<8;++i)model.choose(0);assert.equal(model.state.name.length,8);model.choose(90);
  assert.equal(actions(model,'saveName')[0].name,'ATBAAAAA');frames(model,7);assert.equal(model.state.choices.length,26);model.choose(24);
  const save=actions(model,'saveReplay')[0];assert.equal(save.slot,25);assert.equal(save.result.score,123456);assert.equal(model.state.scene,'title');
});

test('exiting unsavable replay/demo does not offer score entry or mutate game state',()=>{
  const model=active();model.finish({mode:'replay',kind:'replay'});assert.equal(model.state.scene,'title');
  model.finish({kind:'gameover',continues:1,canSaveReplay:false});frames(model,30);model.choose(0);assert.equal(model.state.scene,'title');
  assert.equal(actions(model,'saveName').length,0);
});
