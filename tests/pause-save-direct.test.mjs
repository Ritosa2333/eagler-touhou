import test from 'node:test';
import assert from 'node:assert/strict';
import fs from 'node:fs';
import vm from 'node:vm';
import {TitleScenes} from '../src/title-scenes.mjs';

test('pause Save Replay seals the run and goes directly to the pause-context save page',()=>{
 const source=fs.readFileSync(new URL('../site/game.mjs',import.meta.url),'utf8');
 const start=source.indexOf('function completeRun('),end=source.indexOf('\nfunction beginEnding(',start);
 const model=new TitleScenes();model.enter('pause-confirm',0,{readyAt:0});
 let sealed=0,cleared=0;
 model.finish=()=>assert.fail('Save Replay must not enter death/score/continue results');
 const context={session:{recorder:{finishStage:()=>sealed++}},pendingResult:null,
  resultData:()=>({mode:'start',clear:false,canSaveReplay:true}),recordClear(){},stopEffects(){},saveProfile(){},
  profile:{data:{lastName:'PLAYER'}},model,clearKeys:()=>cleared++};
 vm.runInNewContext(source.slice(start,end)+'\ncompleteRun(false,{saveOnly:true});',context);
 assert.equal(sealed,1);assert.equal(cleared,1);assert.equal(model.state.scene,'replay-save');
 assert.equal(model.state.nameContext,'pause');assert.equal(model.state.name,'PLAYER');
 assert.match(source,/action\.type==='saveExit'\)\{stopMusic\(\);completeRun\(false,\{saveOnly:true\}\);\}/);
});
