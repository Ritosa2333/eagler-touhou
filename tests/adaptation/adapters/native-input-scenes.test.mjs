import test from 'node:test';
import assert from 'node:assert/strict';
import {readDeathbombState,requestDeathbombHit} from './deathbomb.mjs';
import {controlRunBoundary} from './run-boundaries.mjs';

const runtime={global:'__nativeSceneFixture',core:'core',module:'Module',app:'app'};
function withRuntime(words,fn){
  const memory={buffer:new ArrayBuffer(4096)},pointer=16;
  const core={memory,state:()=>pointer,hit:app=>Number(app===73),arm:app=>Number(app===73),
    transition:(...args)=>{core.request=args;return 1;}};
  globalThis[runtime.global]={core,Module:{},app:73};
  try{return fn({core,pointer,put:()=>new words.constructor(memory.buffer,pointer,words.length).set(words)});}
  finally{delete globalThis[runtime.global];}
}
const death={schema:'eagler-touhou/stg-deathbomb/1',wordCount:36,stateExport:'state',hitExport:'hit',runtime};
const boundary={stateExport:'state',armExport:'arm',transitionExport:'transition',runtime,
  gameScene:2,resultScene:6,nonNormalFlags:0x400b,sources:{resultLag:'actual Result field'}};

test('Deathbomb reads the tagged collision and next consumer window without changing native memory',()=>{
  const words=new Uint32Array(36);words[0]=words[1]=words[2]=words[3]=words[12]=1;
  words[8]=words[9]=1;words[10]=words[11]=43;words[13]=2;words[15]=1;words[16]=1;
  words[22]=3;words[24]=2;words[31]=0;words[32]=2;words[33]=1;
  withRuntime(words,({core,put})=>{
    put();const before=Buffer.from(core.memory.buffer).toString('hex');
    const state=readDeathbombState({game:'new-title',variant:'normal',contract:death});
    assert.equal(state.collision.consumerSequence,43);assert.equal(state.life.remainingTicks,1);
    assert.equal(state.life.deathbomb,true);assert.equal(state.inventory.bombs,3);
    assert.equal(Buffer.from(core.memory.buffer).toString('hex'),before);
    assert.deepEqual(requestDeathbombHit({contract:death}),{accepted:true});
  });
});

test('Deathbomb rejects a malformed ABI or pointer rather than inventing a window',()=>{
  const words=new Uint32Array(36);words[0]=words[1]=1;
  withRuntime(words,({core,put})=>{
    words[35]=1;put();assert.throws(()=>readDeathbombState({contract:death}),/ABI/);
    core.state=()=>4090;assert.throws(()=>readDeathbombState({contract:death}),/address/);
    delete core.state;assert.equal(readDeathbombState({contract:death}).available,false);
  });
  assert.equal(readDeathbombState({game:'new-title',variant:'normal'}).available,false);
});

test('Ordinary run reads native run/Result identities and sends exact observed transition inputs',()=>{
  const values=new Float64Array([1,1,1,9,2,1,0,0,4,0,1,192,400,0,0,0,0]);
  withRuntime(values,({core,put})=>{
    put();const state=controlRunBoundary({game:'new-title',variant:'normal',action:'read',contract:boundary});
    assert.equal(state.runId,9);assert.equal(state.scene,'gameplay');assert.equal(state.resultLag,null);
    assert.equal(state.cheatMovementUsed,true);assert.equal(state.normal,true);
    const result=controlRunBoundary({action:'stage',runId:9,stage:0,contract:boundary});
    assert.equal(result.accepted,true);assert.deepEqual(core.request,[73,1,9,0]);
    values[4]=6;values[5]=0;values[13]=1;values[14]=100;put();
    assert.deepEqual(controlRunBoundary({action:'read',contract:boundary}).resultLag,
      {available:true,value:100,source:'actual Result field'});
  });
});

test('Ordinary run excludes Replay/practice and rejects missing identities or malformed observations',()=>{
  const values=new Float64Array([1,1,1,9,2,1,0,0,12,0,0,192,400,0,0,0,0]);
  withRuntime(values,({core,put})=>{
    put();const state=controlRunBoundary({action:'read',contract:boundary});
    assert.equal(state.normal,false);assert.equal(state.replay,true);
    assert.throws(()=>controlRunBoundary({action:'stage',contract:boundary}),/observed native run/);
    values[2]=2;put();assert.throws(()=>controlRunBoundary({action:'read',contract:boundary}),/boolean/);
    delete core.state;assert.equal(controlRunBoundary({action:'read',contract:boundary}).available,false);
  });
});
