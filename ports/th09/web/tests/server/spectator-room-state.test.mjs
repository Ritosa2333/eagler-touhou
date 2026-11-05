import test from 'node:test';
import assert from 'node:assert/strict';
import {readFile} from 'node:fs/promises';
import vm from 'node:vm';
import {SharedNetplay} from '../../sdl-runtime/shared-netplay.mjs';
import {SpectatorTape} from '../../sdl-runtime/spectator-tape.mjs';

const source=await readFile(new URL('../../sdl-runtime/managed.mjs',import.meta.url),'utf8');
const body=source.slice(source.indexOf('async function command('),source.indexOf('async function dispatchCommand('));
test('actual managed command accepts validated spectator room snapshots and preserves unknown-command rejection',async()=>{
  const calls=[];let current=true;
  const c=vm.createContext({receiveLifetime:()=>0,assertLifetime:()=>{if(!current)throw Error('superseded');},netplay:{roomState:(...args)=>calls.push(args)}});
  vm.runInContext(body+';globalThis.invoke=command;',c);
  await c.invoke({command:'spectator-room-state',runId:4,waiting:false});
  await c.invoke({command:'spectator-room-state',runId:5,waiting:true});
  assert.deepEqual(calls,[[4,false],[5,true]]);
  for(const m of [{runId:-1,waiting:true},{runId:1.5,waiting:false},{runId:2**32,waiting:false},{runId:4,waiting:1}])
    await assert.rejects(c.invoke({command:'spectator-room-state',...m}),/状态无效/);
  await assert.rejects(c.invoke({command:'unrecognized'}),/不支持的操作：unrecognized/);
  current=false;await assert.rejects(c.invoke({command:'spectator-room-state',runId:5,waiting:false}),/superseded/);
  assert.equal(calls.length,2);
});
function viewer(){
  const memory=new ArrayBuffer(1024),calls=[];let played=2;
  const core={HEAPU32:new Uint32Array(memory),_th09_spectator_frame:()=>played,_th09_spectator_playback_info:()=>64,_th09_rollback_info:()=>0,
    _th09_peer_close:()=>calls.push('peer-close'),_th09_spectator_end:()=>calls.push('native-end')};
  const n=new SharedNetplay(core,{onResult:()=>calls.push('result'),onClose:()=>calls.push('close')});
  Object.assign(n,{connected:true,active:true,spectator:true,timelineExperiment:true,prepared:true,spectatorFrame:10,spectatorEnded:false,
    options:{netplayUrl:'https://example.test/?run=4'},spectatorTape:new SpectatorTape()});
  return {n,core,calls,play:frame=>{played=frame;}};
}
test('room snapshots do not interrupt live playback, stale runs, players or paused/queued history',()=>{
  const previous=globalThis.location;globalThis.location={href:'https://example.test/'};
  try{
    const {n,core,calls,play}=viewer();
    n.roomState(3,true);n.roomState(4,false);assert.ok(!n.roomSessionRetired);
    n.roomState(4,true);n.frame();assert.deepEqual(calls,[],'a room snapshot never invents an end packet');
    n.spectatorEnded=true;play(10);core.HEAPU32[21]=1;n.frame();assert.deepEqual(calls,[],'pause retains the complete tail');
    n.spectatorTape.append(new Uint8Array(74));n.frame();assert.equal(n.spectatorTape.begins,1);assert.deepEqual(calls,[]);
    const player=viewer();player.n.spectator=false;player.n.roomState(5,true);assert.ok(!player.n.roomSessionRetired);
  }finally{globalThis.location=previous;}
});
for(const next of [{run:4,waiting:true},{run:5,waiting:false}])test(`finished viewer retires only after the final tail (run ${next.run})`,()=>{
  const previous=globalThis.location;globalThis.location={href:'https://example.test/'};
  try{
    const {n,calls,play}=viewer();n.spectatorEnded=true;n.roomState(next.run,next.waiting);n.frame();assert.deepEqual(calls,[]);
    play(10);n.frame();assert.deepEqual(calls,['result','peer-close','native-end','close']);assert.equal(n.connected,false);
  }finally{globalThis.location=previous;}
});
