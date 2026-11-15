import test from 'node:test';
import assert from 'node:assert/strict';
import vm from 'node:vm';
import {renderLocalReplayShell} from '../tools/sync-local-replay.mjs';

test('packaging replaces an obsolete inline bridge with the canonical relocation behavior',async()=>{
  const source='throw Error("host shell must not execute in this test");\n// BEGIN GENERATED LOCAL REPLAY\nfunction obsolete(){}\n// END GENERATED LOCAL REPLAY\nconst game="th10";';
  const rendered=renderLocalReplayShell(source,'shell.mjs');
  assert.equal(renderLocalReplayShell(rendered,'shell.mjs'),rendered);
  assert.equal(renderLocalReplayShell(source.replaceAll('\n','\r\n'),'shell.mjs'),rendered);
  const start=rendered.indexOf('// BEGIN GENERATED LOCAL REPLAY'),end=rendered.indexOf('// END GENERATED LOCAL REPLAY');
  const memory=new WebAssembly.Memory({initial:1}),heap=new Uint8Array(memory.buffer),paths=[],reports=[];
  const description=new TextEncoder().encode(JSON.stringify({difficulty:3,stages:[{id:3}]}));heap.set(description,1024);
  let allocation=8192,opens=0;
  const text=p=>new TextDecoder().decode(heap.subarray(p,heap.indexOf(0,p)));
  const core={memory,eagler_replay_ready:()=>1,eagler_replay_allocate:size=>{const p=allocation;allocation+=size;return p;},eagler_replay_free(){},
    eagler_replay_inspect:()=>1024,eagler_replay_open(){opens++;return 1;},eagler_replay_step:()=>1,
    eagler_replay_state(){new Uint32Array(memory.buffer,512,5).set([456,3600,0,3,3]);return 512;},
    eagler_replay_relocate(from,to){paths.push([text(from),text(to)]);return 1;}};
  const create=vm.runInNewContext(rendered.slice(start,end)+'\ncreateNativeLocalReplay;',{
    WebAssembly,TextEncoder,TextDecoder,performance,document:{hidden:false},requestAnimationFrame:()=>1,cancelAnimationFrame(){},
  });
  const player=create({core:()=>core,module:()=>({HEAPU8:heap,FS:{readFile:()=>new Uint8Array([1,2,3])}}),
    physical:path=>'/save/'+path,stopLoop(){},onState:value=>reports.push(value)});
  try{
    await player.command({action:'open',path:'replay/a.rpy',stage:3});
    await player.command({action:'pause'});await player.command({action:'rate',value:2});
    assert.equal(reports.at(-1).fileRelocation,1);
    await player.command({action:'relocate',path:'replay/a.rpy',target:'replay/b.rpy'});
    assert.deepEqual(paths,[['/save/replay/a.rpy','/save/replay/b.rpy']]);assert.equal(opens,1);
    const state=reports.at(-1);assert.equal(state.path,'replay/b.rpy');assert.equal(state.frame,456);assert.equal(state.paused,true);assert.equal(state.rate,2);
  }finally{player.close();}
});

test('shell generation rejects truncated blocks and missing insertion anchors',()=>{
  assert.throws(()=>renderLocalReplayShell('// BEGIN GENERATED LOCAL REPLAY','shell.mjs'),/end marker/);
  assert.throws(()=>renderLocalReplayShell('let other = 1;','shell.mjs'),/insertion anchor/);
});
