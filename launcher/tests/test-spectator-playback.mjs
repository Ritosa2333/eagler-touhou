import test from 'node:test';
import assert from 'node:assert/strict';
import {build} from 'esbuild';
const result = await build({entryPoints:[new URL('../src/launcher/spectator-playback.mts',import.meta.url).pathname.replace(/^\/([A-Za-z]:)/,'$1')], bundle:true, platform:'node',format:'esm',write:false});
const {parseSpectatorPlayback, spectatorPlaybackCommand, createSpectatorPlayback} = await import('data:text/javascript;base64,'+Buffer.from(result.outputFiles[0].text).toString('base64'));
const report = (extra={}) => ({version:1,run:'abc123',revision:1,min:600,position:600,received:1600,edge:1510,target:1200,
  paused:false,seeking:true,following:false,ended:false,nodes:[{id:1,kind:'stage',frame:600,stage:4,bossSlots:0},{id:2,kind:'boss',frame:1200,stage:4,bossSlots:3}],...extra});
test('reports require complete native bounds and stable restorable node identities',()=>{
  const state=parseSpectatorPlayback(report());assert.equal(state.nodes[1].frame,1200);assert.ok(Object.isFrozen(state.nodes[1]));
  for(const bad of [report({edge:1700}),report({target:500}),report({position:NaN}),report({run:'../1'}),
    report({nodes:[{id:2,kind:'boss',frame:1200,stage:4,bossSlots:0}]}),report({nodes:[report().nodes[1],report().nodes[0]]}),
    report({nodes:[report().nodes[0],report().nodes[0]]})])assert.equal(parseSpectatorPlayback(bad),null);
});
test('only native observer reports enable controls and epoch/TH09 changes retire them',()=>{
  const model=createSpectatorPlayback(),owner={epoch:7,game:'th08',launched:true,spectator:true};
  model.update({...owner,spectator:false},report());assert.equal(model.getSnapshot(),null);
  model.update(owner,report());const first=model.getSnapshot();assert.ok(first);
  model.update(owner,report({revision:0,position:700}));assert.equal(model.getSnapshot(),first);
  model.update(owner,report({revision:2,received:1599}));assert.equal(model.getSnapshot(),first);
  model.update(owner,report({revision:2,position:700}));assert.equal(model.getSnapshot().position,700);
  model.update(owner,null);assert.equal(model.getSnapshot(),null);
  model.update(owner,report());assert.ok(model.getSnapshot());
  model.update({...owner,epoch:8},null);assert.equal(model.getSnapshot(),null);
  model.update({...owner,game:'th09'},report());assert.equal(model.getSnapshot(),null);
  model.update(owner,report());model.update({...owner,launched:false},null);assert.equal(model.getSnapshot(),null);
});
test('commands bind to the current run and cannot seek outside retained confirmed history',()=>{
  const state=parseSpectatorPlayback(report());
  assert.deepEqual(spectatorPlaybackCommand(state,'seek',1200),{run:'abc123',action:'seek',frame:1200});
  for(const frame of [599,1511,NaN,Infinity,1.5])assert.equal(spectatorPlaybackCommand(state,'seek',frame),null);
  assert.deepEqual(spectatorPlaybackCommand(state,'live'),{run:'abc123',action:'live'});
  assert.equal(spectatorPlaybackCommand(null,'play'),null);
});
