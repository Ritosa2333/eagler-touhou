import test from 'node:test';import assert from 'node:assert/strict';
import {WASI} from 'node:wasi';import {readFileSync} from 'node:fs';import {resolve} from 'node:path';
import {core,memory,root,report,sha} from './helpers.mjs';
test('TH11 presentation-only additions preserve every compared original replay state',{skip:!process.env.TH11_EQUIVALENCE_CORE},async()=>{
 const baseline=readFileSync(resolve(process.env.TH11_EQUIVALENCE_CORE)),wasi=new WASI({version:'preview1',args:[],env:{},preopens:{}}),instance=(await WebAssembly.instantiate(baseline,{wasi_snapshot_preview1:wasi.wasiImport})).instance;wasi.initialize(instance);
 const cores=[instance.exports,await core()],archive=readFileSync(resolve(root,'../[th11] 东方地灵殿 (汉化版+日文版)/th11.dat')),cases=[];
 for(const file of ['th11_ud0189.rpy','th11_ud0170.rpy']){
  const raw=readFileSync(resolve(root,'reference/replays',file)),sessions=cores.map(c=>{const d=c.allocate(archive.length),p=c.allocate(raw.length),source=c.resources_create(),s=c.game_session_create(),render=c.comp_create();memory(c,d,archive.length).set(archive);memory(c,p,raw.length).set(raw);assert.equal(c.resources_open(source,d,archive.length),1);assert.equal(c.game_session_replay(s,source,p,raw.length,0,0),1);return {c,d,p,source,s,render};});let checks=0;
  const state=({c,s})=>Buffer.concat([Buffer.from(memory(c,c.battle_data(s,0),48)),Buffer.from(memory(c,c.battle_data(s,2),128)),Buffer.from(memory(c,c.anm_env_rng(c.game_session_animations(s),0),8))]);
  try{for(;checks<110000;++checks){for(const{c,s,render}of sessions){assert.equal(c.game_session_update(s,0,0,0,0,0,0),1,file+' update '+checks);assert.equal(c.game_session_render(s,render),1);}
   const [a,b]=sessions;for(const field of[0,2,8])assert.equal(a.c.game_session_value(a.s,field),b.c.game_session_value(b.s,field),file+' field '+field+' frame '+checks);assert.deepEqual(state(b),state(a),file+' frame '+checks);
   if(a.c.game_session_value(a.s,0)!==1){++checks;break;}
  }assert.ok(checks<110000);cases.push({file,replaySha256:sha(raw),checks});console.log(file,checks);
  }finally{for(const{c,s,source,render,p,d}of sessions){c.comp_delete(render);c.game_session_delete(s);c.resources_delete(source);c.release(p);c.release(d);}}
 }
 report('gameplay-core-equivalence',{passed:true,baselineCoreSha256:sha(baseline),cases,scope:'Current core versus the immutable native-world comparison core, every tick of both full original clear replays. Phase, stage, active projectile count, all 12 economy fields, 128 bytes of player motion and gameplay RNG match exactly. Real rendering callbacks execute; new FPS overlay/rate metadata are verified separately.'});
});
