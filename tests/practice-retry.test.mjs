import test from 'node:test';import assert from 'node:assert/strict';
import {TitleScenes} from '../src/title-scenes.mjs';
const frames=(model,n)=>{for(let i=0;i<n;i++)model.tick({held:0,pressed:0});};
const active=options=>{const model=new TitleScenes(options);frames(model,200);model.drainActions();return model;};
const actions=(model,type)=>model.drainActions().filter(a=>a.type===type);
for(const params of [
  {mode:0,stage:3,section:0,phase:0},
  {mode:1,stage:4,section:10501,phase:0,power:250,life:4,score:12340,ventra_1:2},
  {mode:1,stage:6,section:0,phase:0,power:400,life:8},
])for(const retry of ['pause-confirm','quick-retry','result-menu'])test(`${retry} retains practice parameters for stage ${params.stage+1}`,()=>{
  const model=active({practiceEnabled:true}),difficulty=params.stage===6?4:2;
  model.run={mode:'practice',character:1,shot:1,difficulty,stage:params.stage+1,practice:{...params}};
  const context={mode:'practice',practice:true,character:1,shot:1,difficulty,stage:params.stage+1,canSaveReplay:true};
  if(retry==='result-menu'){model.enter('result-menu',2,{context,readyAt:0});model.confirm();}
  else {model.openPause(context);frames(model,12);if(retry==='quick-retry'){assert.equal(model.quickRetry(),true);frames(model,12);}else{model.choose(3);frames(model,30);model.choose(0);frames(model,30);}}
  const command=actions(model,'retry')[0];assert.ok(command);
  assert.deepEqual(command.practice,params);assert.equal(command.stage,params.stage+1);assert.equal(command.difficulty,difficulty);
  assert.notEqual(command.practice,params,'retry copies the selected parameter object');
});

