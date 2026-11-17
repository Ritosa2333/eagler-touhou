import fs from 'node:fs/promises';
import path from 'node:path';
import {pathToFileURL} from 'node:url';
import {createHash} from 'node:crypto';
import assert from 'node:assert/strict';
import {parseEclIncludes} from '../../site/runtime/ecl-includes.mjs';
import {nativeReplayToFile,OwnedReplayPlayback,OwnedReplayStore} from '../../site/runtime/sound.mjs';
import {decodeReplay} from '../../site/runtime/replay.mjs';
import {createReplayIDB} from '../launcher-session/replay-idb.mjs';
const [component,original,catalogPath,out]=process.argv.slice(2);
const catalog=JSON.parse(await fs.readFile(catalogPath,'utf8'));
const sha=b=>createHash('sha256').update(b).digest('hex');
let Factories=0,Worlds=0,ticks=0;
const mod=await import(pathToFileURL(component).href);assert(++Factories<=1);
const wasm=new Uint8Array(await fs.readFile(component.replace(/\.mjs$/,'.wasm')));
const m=await mod.default({wasmBinary:wasm,print(){},printErr(s){throw Error(s);}});
const coreSHA256=sha(wasm),world=()=>JSON.parse(m.UTF8ToString(m._th128_world_state()));
const check=(v,why)=>assert(v,why),ok=v=>{assert.equal(v,1,m.UTF8ToString(m._th128_world_error()));return v;};
const mem=(arrays,fn)=>{const a=arrays.map(v=>v instanceof Uint8Array?v:new Uint8Array(v.buffer,v.byteOffset,v.byteLength)),p=a.map(b=>{const n=m._malloc(b.length);m.HEAPU8.set(b,n);return n;});try{return fn(p,a.map(b=>b.length));}finally{p.forEach(n=>m._free(n));}};
const one=(n,b,tail=[])=>mem([b],(p,z)=>ok(m[n](p[0],z[0],...tail)));
async function setup(session,token=0,playback=null,progress=0){const stage=catalog.stages.find(s=>s.id===session.params.stage+1),receipts=new Map(),read=async n=>{const b=new Uint8Array(await fs.readFile(path.join(original,n)));receipts.set(n,sha(b));return b;};m._th128_world_reset();for(const n of new TextDecoder().decode(await read('files.txt')).trim().split(/\r?\n/)){const b=await read(n),name=new TextEncoder().encode(n);mem([name,b],(p,z)=>ok(m._th128_world_load(p[0],z[0],p[1],z[1])));}
 if(token&&session.practice)ok(m._th128_world_prepare_thprac_carry(token));const seed=playback?.stageInitial().seed??34772;check(++Worlds<=3,'actualWorldbudget4');one('_th128_world_start',new TextEncoder().encode(stage.ecl),[session.difficulty,seed]);playback?.prepareNativeStage?.(m,{naturalCarry:!!token});if(token){mem([new TextEncoder().encode(stage.ecl)],(p,z)=>ok(m._th128_world_bind_stage_carry_target(token,p[0],z[0],session.difficulty)));ok(m._th128_world_restore_stage_carry_random(token,stage.id,progress,session.difficulty));}
 const banks=parseEclIncludes(await read(stage.ecl)).includes.ANIM;for(const[i,n]of banks.entries()){const b=await read(n);mem([b],(p,z)=>ok(m._th128_world_load_enemy_anm(i+1,p[0],z[0])));}const [bullet,profile,text,player,shot,front,font,card,boss,ice,masks,msg]=await Promise.all(['bullet.anm','bullet-profile-8.bin','text.anm','pl00.anm','pl00a.sht','front.anm','ascii.anm','card.anm',banks[2],'ice.anm','ice-masks.bin',stage.message].map(read));
 one('_th128_world_enable_geometry526',text);mem([profile,bullet],(p,z)=>ok(m._th128_world_enable_bullets(p[0],z[0],p[1],z[1],0,400)));one('_th128_world_enable_bullet_updates',shot);ok(m._th128_world_enable_graze(0,session.params.ice_power,0));one('_th128_world_enable_player_context',player,[1,0,0,0,0,0,0,0,session.params.perfect_freeze]);ok(m._th128_world_enable_player_motion(0,51200));mem([shot,player,new Uint32Array(2000)],(p,z)=>ok(m._th128_world_enable_player_shots(p[0],z[0],p[1],z[1],p[2],2000,session.params.ice_area,session.difficulty,0,0,0,0)));ok(m._th128_world_enable_shot_hits(session.params.motivation,0,0,0,0));ok(m._th128_world_enable_rapid_shot(-1));ok(m._th128_world_enable_player_death(-1));one('_th128_world_enable_perfect_hud',front);one('_th128_world_load_score_font',font);mem([bullet,boss,text,font,card,player],(p,z)=>ok(m._th128_world_enable_spells(...p.flatMap((v,i)=>[v,z[i]]))));mem([new Uint32Array(8)],p=>ok(m._th128_world_enable_player_respawn(1,0,p[0],8)));mem([ice,masks],(p,z)=>ok(m._th128_world_enable_z_fire(p[0],z[0],p[1],z[1])));for(const type of new TextDecoder().decode(await read('profiles.txt')).trim().split(/\r?\n/).map(Number)){const b=await read('profile-'+type+'.bin');mem([b],(p,z)=>ok(m._th128_world_add_bullet_profile(type,p[0],z[0])));}mem([msg,text,player,boss],(p,z)=>ok(m._th128_world_enable_messages(...p.flatMap((v,i)=>[v,z[i]]))));one('_th128_world_enable_message_front',front);if([1,6,11].includes(stage.id))one('_th128_world_enable_route_choice',front,[stage.id,0,0]);one('_th128_world_enable_stage_transition',front,[stage.id,token?progress:0,0,0,1,0]);if(token){ok(m._th128_world_restore_stage_carry(token));ok(m._th128_world_acknowledge_stage_transition(token));}return{coreSHA256,resourceSHA256:sha(Buffer.from(JSON.stringify([...receipts].sort((a,b)=>a[0].localeCompare(b[0])))))};}
// Fixed isolated plaintext profile: valid original CR/ST/CD block headers, no personal data.
const profile=new Uint8Array(52232),pv=new DataView(profile.buffer);
for(let i=0;i<8;i++){pv.setUint32(i*1644,0x35243,true);pv.setUint32(i*1644+8,1644,true);}
pv.setUint32(13152,0x25453,true);pv.setUint32(13160,1068,true);pv.setUint32(14220,0x14443,true);pv.setUint32(14228,38012,true);
const session={practice:false,difficulty:0,params:{stage:0,motivation:100000,perfect_freeze:0,ice_power:0,ice_area:0}};
await setup(session);one('_th128_world_records_restore',profile);ok(m._th128_world_replay_begin(34772,10));
const rows=[...Array.from({length:4},()=>({motion:[0,0,0],held:0,want:[0,51200]})),
 {motion:[1,1280,51200],held:0,want:[576,51200]},
 {motion:[3,32767,0],held:0,want:[1152,51200]},
 {motion:[2,-23552,4096],held:128,want:[1728,51200]},
 {motion:[2,1728,51200],held:0,want:[1728,51200]},
 {motion:[2,1856,51200],held:0,want:[1856,51200]},
 {motion:[2,8192,51200],held:0,want:[8192,51200]},
 {motion:[0,0,0],held:0,want:[8192,51200]},
 {motion:[3,-32767,0],held:8,want:[7936,51200]},
 {motion:[2,-23552,4096],held:128,want:[8512,51200]},
 {motion:[0,0,0],held:0,want:[8512,51200]}];
let previous=0;const generation=world().flow.worldGeneration,projections=[];
const projection=()=>{const p=m._th128_test_practice_projection();return [...new Uint32Array(m.HEAPU8.slice(p,p+60).buffer)];};
function tick(){assert(ticks<200,'actualnative tick budget');++ticks;ok(m._th128_world_tick());}
for(let i=0;i<rows.length;i++){
 const r=rows[i];ok(m._th128_world_replay_clock(10+(i+1)/60,60));
 ok(m._th128_world_input(r.held,r.held&~previous,previous&~r.held));previous=r.held;
 ok(m._th128_world_touch_motion(...r.motion,generation));tick();
 const p=projection();assert.deepEqual(p.slice(5,7),r.want,'actual ordinary PlayerHitHost movement '+i);projections.push(p);
 if(i===6||i===7){assert.equal(world().normalRecord.assisted,false,'keyboard priority/stationary command is not actual touch movement');assert.equal(world().normalRecord.scoreEligible,true);}
 if(i===8){assert.equal(world().normalRecord.assisted,true,'fixed upstream marks ANY consumed unlimited displacement');assert.equal(world().normalRecord.scoreEligible,false);}
 ok(m._th128_world_records_present(10+(i+1)/60));
}
const receipt=world();assert.equal(receipt.debug.runTainted,false);assert.equal(receipt.thprac?.configured??false,false);
assert.equal(receipt.normalRecord.assisted,true,'actual successful unlimited displacement must retire normal score/unlock eligibility');
assert.equal(receipt.normalRecord.scoreEligible,false);
// Readonly test adapter captures the actual original NativeRunReplay; it cannot modify gameplay state.
const size=m._th128_test_ordinary_replay_size(),ptr=m._th128_test_ordinary_replay_data();assert(size>148);
const raw=m.HEAPU8.slice(ptr,ptr+size),file=nativeReplayToFile(raw),decoded=decodeReplay(file);
assert.equal(decoded.metadata.slowdown,100,'upstream actual assisted movement slowdown policy');
assert.equal(decoded.userBlocks.some(b=>b.kind===0x31484354),true);
const profileBytes=m._th128_world_records_prepare(1281268800,0,11);assert.equal(profileBytes,52232,m.UTF8ToString(m._th128_world_error()));
assert.deepEqual(m.HEAPU8.slice(m._th128_world_records_data(),m._th128_world_records_data()+profileBytes),profile,'assisted run cannot commit any normal score/spell/unlock counters');
const db=createReplayIDB(),store=new OwnedReplayStore(db);await store.save(9,raw,{expectedDigest:null});store.close();
const reopened=new OwnedReplayStore(db),stored=await reopened.export(9);assert.deepEqual(stored,file);reopened.close();
const playback=new OwnedReplayPlayback(stored,9);await setup(session,0,playback);playback.bindNativeStage(m);
const replayGeneration=world().flow.worldGeneration;
assert.equal(m._th128_world_touch_motion(2,0,51200,replayGeneration),0,'actual native playLock rejects live input');
for(let i=0;i<rows.length;i++){tick();assert.deepEqual(projection(),projections[i],'ordinary touch native Replay 15projections frame '+i);}
assert.equal(playback.snapshot().complete,true);playback.cancel();
// A fresh limited/free run retains genuine normal eligibility and records real counters.
await setup(session);one('_th128_world_records_restore',profile);ok(m._th128_world_replay_begin(34772,20));
const fresh=world().flow.worldGeneration;
for(let i=0;i<65;i++){ok(m._th128_world_replay_clock(20+(i+1)/60,60));ok(m._th128_world_touch_motion(i<4?0:1,i<4?0:1280,i<4?0:51200,fresh));tick();ok(m._th128_world_records_present(20+(i+1)/60));}
assert.equal(world().normalRecord.assisted,false);assert.equal(world().normalRecord.scoreEligible,true);
assert.equal(m._th128_world_records_prepare(1281268800,0,22),52232,m.UTF8ToString(m._th128_world_error()));assert.notDeepEqual(m.HEAPU8.slice(m._th128_world_records_data(),m._th128_world_records_data()+52232),profile);
m._th128_world_reset();assert.equal(m._th128_world_touch_motion(1,0,51200,fresh),0);
await fs.writeFile(path.join(out,'ordinary-touch.rpy'),file,{flag:'wx'});
const result={passed:true,Factories,Worlds,ticks,projectionWords:15,ordinaryRunAssisted:true,normalQualificationPreserved:true,profileUnchangedOnAssisted:true,RPYBytes:file.length,RPYSHA256:sha(file),strictSyntheticIDB:true,wholeProduction:false};
await fs.writeFile(path.join(out,'ordinary-touch-evidence.json'),JSON.stringify(result,null,2)+'\n',{flag:'wx'});console.log(JSON.stringify(result));
