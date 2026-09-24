import test from 'node:test';
import assert from 'node:assert/strict';
import {webcrypto} from 'node:crypto';
import {configureMultiplayer,validateMultiplayerOptions,updateNetworkDiagnostics} from '../../th08_web/sdl-runtime/multiplayer-host.mjs';

const base={netplayMode:'lan',netplayUrl:'wss://relay.test/peer?room=th08mp-demo&run=1&player=0',
 netplayPlayerCount:2,netplayPlayer:0,netplaySeed:1234,netplayDifficulty:1,
 netplayLoadouts:[{character:4,shot:0},{character:11,shot:0}]};
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
 const c=fake();assert.equal(await configureMultiplayer(c,42,base,{crypto:webcrypto}),true);
 assert.deepEqual(c.words.slice(0,5),[2,2,0,1,1234]);assert.deepEqual(c.words.slice(7),[4,0,11,0,0,0]);
 assert.ok(c.words[5]||c.words[6]);assert.equal(c.connects.length,1);assert.equal(c.freed.length,2);
 const peer=fake();await configureMultiplayer(peer,42,{...base,netplayUrl:base.netplayUrl.replace('player=0','player=1'),netplayPlayer:1},{crypto:webcrypto});
 assert.deepEqual(c.words.slice(5,7),peer.words.slice(5,7));
 const next=fake();await configureMultiplayer(next,42,{...base,netplayUrl:base.netplayUrl.replace('run=1','run=2')},{crypto:webcrypto});
 assert.notDeepEqual(c.words.slice(5,7),next.words.slice(5,7));
});
test('malformed room/role/config fails before a native mutation',async()=>{
 for(const changed of [{netplaySeed:65536},{netplayDifficulty:5},{netplayPlayer:2},{netplayPlayerCount:1},
  {netplayUrl:'https://relay.test/?room=x&run=1'},{netplayUrl:'ws://relay.test/?room=x'},
  {netplayLoadouts:[{character:12,shot:0},{character:1,shot:0}]},
  {netplayLoadouts:[{character:0,shot:1},{character:1,shot:0}]},{netplaySpectator:true}]){
   const c=fake();await assert.rejects(configureMultiplayer(c,42,{...base,...changed},{crypto:webcrypto}));assert.equal(c.words.length,0);
 }
 assert.equal(validateMultiplayerOptions({}),null);
});

test('admitted spectator gets a receive-only connection and the same room identity',async()=>{
 const options={...base,netplaySpectator:true,netplaySpectatorId:'viewer_1234',netplayPlayer:-1};
 const c=fake();assert.equal(await configureMultiplayer(c,42,options,{crypto:webcrypto}),true);
 assert.equal(c.connects.length,0);assert.deepEqual(c.observers,[{url:base.netplayUrl,id:'viewer_1234'}]);
 assert.equal(c.words[2],0,'internal authoritative P1 lane is not an admitted gameplay seat');
 const player=fake();await configureMultiplayer(player,42,base,{crypto:webcrypto});
 assert.deepEqual(c.words.slice(5,7),player.words.slice(5,7));
 assert.equal(c.freed.length,3);
 for(const id of ['', 'bad', '../bad_viewer', 'a'.repeat(65)]){
  const c=fake();await assert.rejects(configureMultiplayer(c,42,{...options,netplaySpectatorId:id},{crypto:webcrypto}));
  assert.equal(c.words.length,0);
 }
 const missing=fake();delete missing.multiplayer_spectator_connect;
 await assert.rejects(configureMultiplayer(missing,42,options,{crypto:webcrypto}));assert.equal(missing.words.length,0);
});
test('diagnostics never admit input and survive native memory growth',()=>{
 const memory=new WebAssembly.Memory({initial:1});const target={};let hashes=0;
 const core={memory,multiplayer_netplay_status(){memory.grow(1);new Uint32Array(memory.buffer,128,12).set([1,1,1,60,59,59,0xffffffff,1,0,0,2,0]);return 128;},
 multiplayer_driver_status(){memory.grow(1);new Uint32Array(memory.buffer,256,16).set([1,1,0,3,12,7,30,8,90,2,999,1,1,30,30,0]);return 256;},
 multiplayer_canonical_state(){++hashes;memory.grow(1);new Uint32Array(memory.buffer,384,13).set([1,123]);return 384;}};
 updateNetworkDiagnostics(core,42,target);assert.equal(target.__eaglerNetplayLanFrame,59);assert.equal(target.__eaglerNetplayGeneration,2);
 assert.equal(target.__eaglerNetplayTransport,'rtc');assert.equal(target.__eaglerNetplayLanHashes['59'],'123');assert.equal(hashes,1);
 updateNetworkDiagnostics(core,42,target);assert.equal(hashes,1,'repeated presentations must not hash the same frame again');
});
