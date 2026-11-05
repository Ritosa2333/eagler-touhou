// Connect a previously checked native prefix to a rebuilt C++ core. Compare
// every field in world-replays.test.mjs, not just the replay's final score.
import test from 'node:test';import assert from 'node:assert/strict';
import {readFileSync} from 'node:fs';import {resolve} from 'node:path';
import {gunzipSync} from 'node:zlib';import {WASI} from 'node:wasi';
import {core,memory,root,report,sha,target} from './helpers.mjs';

test('TH11 rebuilt core preserves the complete previously verified native prefix',{skip:!process.env.TH11_NATIVE_PREFIX},async()=>{
 const dir=resolve(process.env.TH11_NATIVE_PREFIX),saved=JSON.parse(readFileSync(resolve(dir,'state.json'))),baselinePath=resolve(process.env.TH11_PREFIX_CORE),baseline=readFileSync(baselinePath);
 const raw=readFileSync(resolve(root,'reference/replays/th11_ud0189.rpy')),archive=readFileSync(resolve(root,'../[th11] 东方地灵殿 (汉化版+日文版)/th11.dat'));
 assert.equal(saved.identity.coreSha256,sha(baseline));assert.equal(saved.identity.replaySha256,sha(raw));assert.equal(saved.identity.targetSha256,target.sha256);assert.equal(saved.identity.stageDrawClocks,1);
 assert.equal(saved.checks,saved.nextFrame);assert.equal(saved.activeTicks,saved.checks);assert.equal(saved.firstInactiveFrame,null);
 const wasi=new WASI({version:'preview1',args:[],env:{},preopens:{}}),instance=(await WebAssembly.instantiate(baseline,{wasi_snapshot_preview1:wasi.wasiImport})).instance;wasi.initialize(instance);
 const cores=[instance.exports,await core()];
 const runs=cores.map(c=>{const p=c.allocate(raw.length),r=c.replay_create();memory(c,p,raw.length).set(raw);assert.equal(c.replay_open(r,p,raw.length),1);const data=c.allocate(archive.length),source=c.resources_create(),session=c.game_session_create();memory(c,data,archive.length).set(archive);assert.equal(c.resources_open(source,data,archive.length),1);assert.equal(c.game_session_replay(session,source,p,raw.length,1,1),1);const render=c.comp_create();return {c,p,r,data,source,session,render};});
 assert.deepEqual(Object.fromEntries(Object.entries(runs[0]).filter(([k])=>k!=='c')),saved.pointers,'baseline allocation order matches the original checkpoint');
 const snapshot=({c,session})=>{
  const dv=new DataView(c.memory.buffer),parts=[],read=p=>dv.getUint32(p,true),take=(p,n)=>parts.push(Buffer.from(memory(c,p,n))),word=n=>{const b=Buffer.alloc(4);b.writeUInt32LE(n>>>0);parts.push(b);};
  for(const field of[0,2,8])word(c.game_session_value(session,field));
  take(c.battle_data(session,0),48);take(c.battle_data(session,2),128);take(c.anm_env_rng(c.game_session_animations(session),0),8);
  const life=c.battle_data(session,1);for(const[off,n]of[[0,4],[64,12],[76,36],[112,24],[136,72]])take(life+off,n);
  const active=c.game_session_transition_value(session,2);word(active);if(active){const stage=c.game_session_stage_data(session,0);for(const off of[0x38,0x3c,0x40,0x2ff0,0x2ff8,0x3028])take(stage+off,4);}
  let enemy=c.game_session_enemy_state(session,0),count=0;while(enemy){word(1);for(const[off,n]of[[0,0xe0],[0x104,0x50],[0x14ec,0x1c],[0x1580,0x14]])take(enemy+off,n);const next=read(enemy+0x16c);enemy=next?read(next)+0x103c:0;assert.ok(++count<4096,'bounded enemy list');}word(0);
  const items=c.battle_items(session);for(let i=0;i<2198;++i){const p=items+i*0x478,state=read(p+0x464);word(state);if(state){word(i);take(p+0x434,0x28);take(p+0x460,0x18);}}
  return Buffer.concat(parts);
 };
 let checks=0;
 try{for(;checks<saved.nextFrame;++checks){
  for(const {c,session,render}of runs){assert.equal(c.game_session_update(session,0,0,0,0,0,0),1,'update '+checks);assert.equal(c.game_session_render(session,render),1,'draw '+checks);}
  assert.deepEqual(snapshot(runs[1]),snapshot(runs[0]),'complete world prefix frame '+checks);
  if(checks%12000===0)console.log('prefix compared',checks);
 }
 const final=snapshot(runs[0]),old=runs[0].c,bytes=gunzipSync(readFileSync(resolve(dir,'cpp.gz')));if(old.memory.buffer.byteLength<bytes.length)old.memory.grow((bytes.length-old.memory.buffer.byteLength)/65536);new Uint8Array(old.memory.buffer).set(bytes);
 assert.deepEqual(snapshot(runs[0]),final,'recreated prefix ends at the verified native checkpoint state');
 report('world-prefix-equivalence',{passed:true,checks,replaySha256:sha(raw),baselineCoreSha256:sha(baseline),checkpoint:{path:dir,metadataSha256:sha(readFileSync(resolve(dir,'state.json'))),nativeSha256:sha(readFileSync(resolve(dir,'native.gz'))),cppSha256:sha(readFileSync(resolve(dir,'cpp.gz'))),nextFrame:saved.nextFrame},scope:'Every field checked by the original native-world prefix is compared between the immutable old core and current core at every frame: economy, player lifecycle/motion, RNG, projectile count, enemy lists/fields, active item fields, stage clocks and lifecycle. The old reconstructed endpoint is also matched to the saved original-comparison checkpoint. This proves the prefix only; a contiguous current-core native tail is required for a complete campaign proof.'});
 }finally{for(const{c,p,r,data,source,session,render}of runs){c.comp_delete(render);c.game_session_delete(session);c.resources_delete(source);c.release(data);c.replay_delete(r);c.release(p);}}
});
