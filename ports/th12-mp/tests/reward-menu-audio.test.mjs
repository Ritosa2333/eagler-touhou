import test from 'node:test';
import assert from 'node:assert/strict';
import {TitleScenes} from '../src/title-scenes.mjs';
test('cleared story and Extra score flows request score music before ranked or assisted results',()=>{
  for(const mode of ['story','extra'])for(const ranked of [true,false]){
    const model=new TitleScenes();model.drainActions();model.run={mode,character:0,shot:0,difficulty:1};
    model.finish({mode,clear:true,ranked,score:1000,canSaveReplay:true});
    const actions=model.drainActions();assert.deepEqual(actions.filter(a=>a.type==='menuMusic'),[{type:'menuMusic',track:'th10_17',restart:true}]);
    if(mode==='extra')assert.ok(actions.findIndex(a=>a.type==='pause')<actions.findIndex(a=>a.type==='menuMusic'));
    assert.equal(model.state.scene,'name');
    assert.equal(model.state.ranked,ranked);
  }
});

test('unranked story and Extra clears show the ranking without inserting or saving the run',()=>{
  for(const mode of ['story','extra']){
    const rows=[{score:900,name:'EXISTING'}];
    const model=new TitleScenes({profile:{data:{lastName:'PLAYER'},rankings:()=>rows}});
    model.drainActions();model.run={mode,character:0,shot:0,difficulty:1};
    model.finish({clear:true,ranked:false,score:1000,canSaveReplay:true});
    assert.equal(model.state.scene,'name');assert.equal(model.state.nameKind,'score');
    assert.equal(model.state.ranked,false);assert.equal(model.state.rank,-1);
    assert.deepEqual(model.state.ranking,rows);
    model.completeName();assert.equal(model.drainActions().filter(a=>a.type==='saveName').length,0);
    assert.equal(model.state.scene,mode==='extra'?'result-menu':'replay-save');
    assert.deepEqual(rows,[{score:900,name:'EXISTING'}]);
  }
});
test('Replay name confirmation emits native save sound18 instead of ordinary menu sound7',()=>{
  for(const nameContext of ['title','pause']){
    const model=new TitleScenes();model.enter('name',90,{nameKind:'replay',nameContext,name:'PLAYER',slot:2,result:{canSaveReplay:true},readyAt:0});model.drainActions();
    model.confirm();const actions=model.drainActions();
    assert.deepEqual(actions.filter(a=>a.type==='sound'),[{type:'sound',id:18}]);assert.equal(actions.filter(a=>a.type==='saveReplay').length,1);assert.equal(model.state.scene,'replay-save');
  }
});
