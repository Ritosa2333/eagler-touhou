import fs from 'node:fs/promises';
import path from 'node:path';
import {pathToFileURL} from 'node:url';
import {createHash} from 'node:crypto';
import assert from 'node:assert/strict';
import {parseEclIncludes} from '../../site/runtime/ecl-includes.mjs';
import {nativeReplayToFile,OwnedReplayPlayback,OwnedReplayStore} from '../../site/runtime/sound.mjs';
import {readTouchReplay} from '../../site/runtime/touch-replay.mjs';
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
const generation=world().flow.worldGeneration,auxiliary=[],projections=[];let previous=0,frame=0;
function projection(){const p=m._th128_test_practice_projection();return [...new Uint32Array(m.HEAPU8.slice(p,p+60).buffer)];}
while(auxiliary.length<3){
 assert(ticks<20000,'original ordinary MSG auxiliary upper20000ticks');
 const context=JSON.parse(m.UTF8ToString(m._th128_world_replay_playback_state()));assert.equal(context.auxiliaryKnown,true);
 const observed=world();
 const threats=observed.bullets.filter(b=>b.y>330);
 const boss=observed.entities.filter(e=>e.active&&e.life>1000).sort((a,b)=>b.life-a.life)[0];
 let best=null;
 for(const y of [392,416,432])for(let x=-176;x<=176;x+=16){let clearance=10000;for(const b of threats){for(const t of [0,1,2,3,4])clearance=Math.min(clearance,(x-b.x-b.velocityX*t)**2+(y-b.y-b.velocityY*t)**2);}
  const score=(clearance<500?clearance-10000:1000)-Math.abs(x-(boss?.x??0))*2+y/100; if(!best||score>best.score)best={x,y,score};}
 const held=512|(frame%4===2?1:0)|(context.auxiliaryLive&&frame%4===0?32:0);
 ok(m._th128_world_replay_clock(10+(frame+1)/60,60));ok(m._th128_world_input(held,held&~previous,previous&~held));
 if(frame>=4)ok(m._th128_world_touch_motion(2,Math.round(best.x*128),Math.round(best.y*128),generation));
 if(context.auxiliaryLive)auxiliary.push([frame,held,held&~previous,previous&~held]);previous=held;
 ++ticks;const code=m._th128_world_tick();if(code!==1){await fs.writeFile(path.join(out,'first-observed-world.json'),JSON.stringify({Factories,Worlds,ticks,frame,auxiliary,code,state:world()},null,2)+'\n',{flag:'wx'});}ok(code);ok(m._th128_world_records_present(10+(frame+1)/60));projections.push(projection());++frame;
}
const size=m._th128_test_ordinary_replay_size(),p=m._th128_test_ordinary_replay_data(),raw=m.HEAPU8.slice(p,p+size),file=nativeReplayToFile(raw),decoded=decodeReplay(file),plan=readTouchReplay(decoded)?.[0];
await fs.writeFile(path.join(out,'ordinary-aux-native.bin'),raw,{flag:'wx'});await fs.writeFile(path.join(out,'ordinary-aux.rpy'),file,{flag:'wx'});
await fs.writeFile(path.join(out,'observed-native-history.json'),JSON.stringify({Factories,Worlds,ticks,auxiliary,mainFrames:decoded.stages[0].numFrames,worldFrames:plan?.worldFrames,events:plan?.events,WorldBoundary:world().frame},null,2)+'\n',{flag:'wx'});
assert.deepEqual(plan?.auxiliary,auxiliary,'original ordinary MSG/route-choice skipped input history must survive native RPY USER');
const playback=new OwnedReplayPlayback(file,9);await setup(session,0,playback);playback.bindNativeStage(m);
for(let i=0;i<projections.length;i++){assert(ticks<40000);++ticks;ok(m._th128_world_tick());assert.deepEqual(projection(),projections[i],'ordinary auxiliary actual native15projection frame '+i);}
assert.equal(playback.snapshot().complete,true);playback.cancel();m._th128_world_reset();
console.log(JSON.stringify({passed:true,Factories,Worlds,ticks,recordedWorldFrames:frame,auxiliaryFrames:auxiliary.length,mainFrames:decoded.stages[0].numFrames}));
