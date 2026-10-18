import test from 'node:test';
import assert from 'node:assert/strict';
import {build} from 'esbuild';
const built=await build({entryPoints:[new URL('../../src/launcher/replay-description.mts',import.meta.url).pathname.replace(/^\/([A-Z]:)/,'$1')],bundle:true,platform:'node',format:'esm',write:false});
const {parseReplayDescription}=await import('data:text/javascript;base64,'+Buffer.from(built.outputFiles[0].text).toString('base64'));
test('native CP932 names and original numeric metadata retain stage identity and missing per-stage scores',()=>{
  const value=parseReplayDescription({nameBytes:[0x82,0xa0,0,32],spellNameBytes:[0x82,0xa2,0],character:2,shot:1,difficulty:5,score:123456780,slowdown:0.123,date:'07/19',stages:[{id:8,label:'PHANTASM',score:123456780,frames:500},{id:3,score:null}]});
  assert.equal(value.spellName,'い');
  assert.equal(value.name,'あ');assert.equal(value.score,123456780);assert.equal(value.slowdown,0.123);assert.equal(value.stages[0].id,8);assert.equal(value.stages[1].score,null);
});
test('invalid, duplicate or guessed stage metadata cannot become selectable',()=>{
  for(const stages of [[],[{id:1,score:-1}],[{id:1,score:0},{id:1,score:2}],[{id:1,score:0,frames:NaN}],[{id:1.5,score:0}]])assert.throws(()=>parseReplayDescription({character:0,difficulty:1,stages}));
});
