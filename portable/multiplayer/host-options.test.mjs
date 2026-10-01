import test from 'node:test';
import assert from 'node:assert/strict';
import {webcrypto} from 'node:crypto';
import {configureMultiplayer,validateMultiplayerOptions,updateNetworkDiagnostics,networkError} from '../../th08_web/sdl-runtime/multiplayer-host.mjs';

const base={netplayMode:'lan',netplayUrl:'wss://relay.test/peer?room=th08mp-demo&run=1&player=0',
 netplayPlayerCount:2,netplayPlayer:0,netplaySeed:1234,netplayDifficulty:1,
 netplayInputDelay:4,netplayPredictionLimit:2,
 netplayLoadouts:[{character:4,shot:0},{character:11,shot:0}]};
const runtimeBuildWords=[0x11223344,0x55667788,0x99aabbcc,0xddeeff00];
const deps={crypto:webcrypto,runtimeBuildWords};
function fake(){
 let cursor=128;
 const c={memory:new WebAssembly.Memory({initial:1}),words:[],freed:[],connects:[],observers:[],allocate:n=>{const p=cursor;cursor+=n+16;return p;},deallocate:p=>c.freed.push(p),
  multiplayer_configure(app,p,n){assert.equal(app,42);c.words=Array.from(new Uint32Array(c.memory.buffer,p,n));c.memory.grow(1);return 1;},
  multiplayer_connect(app,p){assert.equal(app,42);const b=new Uint8Array(c.memory.buffer);let end=p;while(b[end])++end;c.connects.push(new TextDecoder().decode(b.subarray(p,end)));return 1;},
  multiplayer_spectator_connect(app,p,id){assert.equal(app,42);c.memory.grow(1);const b=new Uint8Array(c.memory.buffer);
   const read=q=>{let end=q;while(b[end])++end;return new TextDecoder().decode(b.subarray(q,end));};
   c.observers.push({url:read(p),id:read(id)});return 1;}};
 return c;
}
test('host maps TH08 twelve loadouts and room identity without controlling frames',async()=>{
 const c=fake();assert.equal(await configureMultiplayer(c,42,base,deps),true);
 assert.deepEqual(c.words.slice(0,5),[4,2,0,1,1234]);assert.deepEqual(c.words.slice(7,11),runtimeBuildWords);
 assert.deepEqual(c.words.slice(11),[4,0,11,0,0,0,4,2]);
 assert.ok(c.words[5]||c.words[6]);assert.equal(c.connects.length,1);assert.equal(c.freed.length,2);
 assert.equal(globalThis.__eaglerNetplayInputDelayFrames,4);
 const peer=fake();await configureMultiplayer(peer,42,{...base,netplayUrl:base.netplayUrl.replace('player=0','player=1'),netplayPlayer:1},deps);
 assert.deepEqual(c.words.slice(5,7),peer.words.slice(5,7));
 const next=fake();await configureMultiplayer(next,42,{...base,netplayUrl:base.netplayUrl.replace('run=1','run=2')},deps);
 assert.notDeepEqual(c.words.slice(5,7),next.words.slice(5,7));
});
test('malformed room/role/config fails before a native mutation',async()=>{
 for(const changed of [{netplaySeed:65536},{netplayDifficulty:5},{netplayPlayer:2},{netplayPlayerCount:1},
  {netplayInputDelay:9},{netplayPredictionLimit:0},
  {netplayUrl:'https://relay.test/?room=x&run=1'},{netplayUrl:'ws://relay.test/?room=x'},
  {netplayLoadouts:[{character:12,shot:0},{character:1,shot:0}]},
  {netplayLoadouts:[{character:0,shot:1},{character:1,shot:0}]},{netplaySpectator:true}]){
   const c=fake();await assert.rejects(configureMultiplayer(c,42,{...base,...changed},deps));assert.equal(c.words.length,0);
 }
 const noBuild=fake();await assert.rejects(configureMultiplayer(noBuild,42,base,{crypto:webcrypto}));assert.equal(noBuild.words.length,0);
 assert.equal(validateMultiplayerOptions({}),null);
});

test('admitted spectator gets a receive-only connection and the same room identity',async()=>{
 const options={...base,netplaySpectator:true,netplaySpectatorId:'viewer_1234',netplayPlayer:-1};
 const c=fake();assert.equal(await configureMultiplayer(c,42,options,deps),true);
 assert.equal(c.connects.length,0);assert.deepEqual(c.observers,[{url:base.netplayUrl,id:'viewer_1234'}]);
 assert.equal(c.words[2],0,'internal authoritative P1 lane is not an admitted gameplay seat');
 assert.equal(globalThis.__eaglerNetplayInputDelayFrames,0);
 const player=fake();await configureMultiplayer(player,42,base,deps);
 assert.deepEqual(c.words.slice(5,7),player.words.slice(5,7));
 assert.equal(c.freed.length,3);
 for(const id of ['', 'bad', '../bad_viewer', 'a'.repeat(65)]){
  const c=fake();await assert.rejects(configureMultiplayer(c,42,{...options,netplaySpectatorId:id},deps));
  assert.equal(c.words.length,0);
 }
 const missing=fake();delete missing.multiplayer_spectator_connect;
 await assert.rejects(configureMultiplayer(missing,42,options,deps));assert.equal(missing.words.length,0);
});
test('diagnostics never admit input and survive native memory growth',()=>{
 const memory=new WebAssembly.Memory({initial:1});const target={__eaglerNetplayInputDelayFrames:8};let hashes=0;
 const core={memory,multiplayer_netplay_status(){memory.grow(1);new Uint32Array(memory.buffer,128,14).set([2,1,1,60,59,59,0xffffffff,1,0,0,2,0,1,60]);return 128;},
 multiplayer_driver_status(){memory.grow(1);new Uint32Array(memory.buffer,256,16).set([1,1,0,3,12,7,30,8,90,2,999,1,1,30,30,0]);return 256;},
 multiplayer_canonical_state(){++hashes;memory.grow(1);new Uint32Array(memory.buffer,384,13).set([1,123]);return 384;}};
 updateNetworkDiagnostics(core,42,target);assert.equal(target.__eaglerNetplayLanFrame,59);assert.equal(target.__eaglerNetplayGeneration,2);
 assert.equal(target.__eaglerNetplayLanActive,true);
 assert.equal(target.__eaglerNetplayInputDelayFrames,1,'diagnostics report applied native timing, not cached host options');
 assert.equal(target.__eaglerNetplayTransport,'rtc');assert.equal(target.__eaglerNetplayLanHashes,undefined);assert.equal(hashes,0);
 updateNetworkDiagnostics(core,42,target);assert.equal(hashes,0,'published diagnostics must never calculate a world hash');
});
test('a failed channel is not advertised as active and preserves the native cause',()=>{
 const memory=new WebAssembly.Memory({initial:1}),target={};
 const reason='network channel failed: RTC peer P2 control channel closed [mode=rtc channel=4 queuedBytes=137672]';
 new Uint8Array(memory.buffer,512,reason.length+1).set(new TextEncoder().encode(reason+'\0'));
 new Uint32Array(memory.buffer,128,12).set([1,1,1,60,59,46,0xffffffff,1,0,0,0,0]);
 new Uint32Array(memory.buffer,256,16).set([1,1,1,3,12,7,30,8,90,2,999,1,1,120,98,4]);
 const core={memory,multiplayer_netplay_status:()=>128,multiplayer_driver_status:()=>256,multiplayer_network_error:()=>512};
 updateNetworkDiagnostics(core,42,target);
 assert.equal(target.__eaglerNetplayFailed,true);
 assert.equal(target.__eaglerNetplayLanActive,false);
 assert.equal(target.__eaglerNetplayError,reason);
 assert.equal(target.__eaglerNetplayChannelError,4);
 assert.equal(target.__eaglerNetplayPacketsSent,120);
 assert.equal(target.__eaglerNetplayPacketsReceived,98);
 assert.equal(target.__eaglerNetplayLanFrame,59,'a failure must not advance native frames');
});
test('native null error pointers do not decode unrelated memory at address zero',()=>{
 const memory=new WebAssembly.Memory({initial:1});
 new Uint8Array(memory.buffer,0,8).set(new TextEncoder().encode('garbage\0'));
 assert.equal(networkError({memory,multiplayer_network_error:()=>0},42),'');
});
test('read-only spectator completion no longer advertises an active session',()=>{
 const memory=new WebAssembly.Memory({initial:1}),target={};
 new Uint32Array(memory.buffer,128,12).set([1,1,1,60,59,46,0xffffffff,1,0,0,0,0]);
 new Uint32Array(memory.buffer,256,16).set([1,1,0,0,0,0,0,0,0,0,0,0,3,0,0,0]);
 new Uint32Array(memory.buffer,384,8).set([1,1,1,0,0,0,0,0]);
 updateNetworkDiagnostics({memory,multiplayer_netplay_status:()=>128,multiplayer_driver_status:()=>256,multiplayer_spectator_status:()=>384},42,target);
 assert.equal(target.__eaglerNetplayLanActive,false);
 assert.equal(target.__eaglerNetplaySpectatorFinished,true);
 assert.notEqual(target.__eaglerNetplayFailed,true,'normal observer completion is not failure');
});
