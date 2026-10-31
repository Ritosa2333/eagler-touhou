import test from 'node:test';
import assert from 'node:assert/strict';
import {TitleScenes} from '../src/title-scenes.mjs';

test('cooperative story clear uses title ranking then multiplayer replay slots',()=>{
  const model=new TitleScenes({profile:{data:{lastName:'PILOT'},rankings:()=>[]}});
  const result={mode:'multiplayer',character:2,shot:1,difficulty:1,score:123456,canSaveReplay:true,clear:true,stage:6};
  model.finishMultiplayer(result);
  assert.equal(model.state.scene,'name');assert.equal(model.state.ranking[0].score,123456);
  assert.equal(model.state.nameContext,'title');assert.equal(model.state.afterName,'replay-save');
  model.completeName();assert.equal(model.state.scene,'replay-save');
  assert.equal(model.state.choices.length,25);assert.equal(model.state.result.mode,'multiplayer');
  assert.equal(model.drainActions().find(a=>a.type==='saveName').result.mode,'multiplayer');
});

test('original 25-slot replay menu retains a separate multiplayer slot namespace',()=>{
  const model=new TitleScenes({corpus:[{id:'th12_mp_11.rpy',name:'TEAM'},{id:'th12_mp_import_1.rpy',name:'IMPORT'}]});
  model.openReplaySave({mode:'multiplayer',canSaveReplay:true},'TEAM','pause');
  assert.equal(model.state.choices.length,25);assert.equal(model.state.replayRecords[10].name,'TEAM');
  assert.equal(model.state.replayRecords[25].name,'IMPORT');
  model.state.age=20;assert(model.choose(24));model.state.name='TEAM';model.completeName();
  const save=model.drainActions().find(a=>a.type==='saveReplay');assert.equal(save.slot,25);assert.equal(save.name,'TEAM');
});

test('cooperative game over and Extra retain their native in-game result controls',()=>{
 for(const result of [{clear:false,stage:3},{clear:true,stage:7,difficulty:4}]){
  const model=new TitleScenes({profile:{data:{lastName:'PILOT'},rankings:()=>[]}});
  model.finishMultiplayer({mode:'multiplayer',character:2,shot:1,difficulty:1,score:123456,canSaveReplay:true,...result});
  model.completeName();assert.equal(model.state.scene,'result-menu');
  assert.equal(model.state.choices.find(c=>c.id==='retry').disabled,true);
  assert.equal(model.state.choices.find(c=>c.id==='save-exit').disabled,false);
 }
});
