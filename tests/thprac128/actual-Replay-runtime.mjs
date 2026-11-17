// Actual freshly built Factory + actual native API + real JS session/adapter. No GPU/browser/physical audio/storage.
import fs from'node:fs';import assert from'node:assert/strict';import{createHash}from'node:crypto';import{pathToFileURL}from'node:url';
import{openTh128Dat}from'../../site/runtime/th128-dat.mjs';import{parseEclIncludes}from'../../site/runtime/ecl-includes.mjs';
import{createThprac128Session,prepareThprac128Native,readThprac128Native,thprac128RuntimeSupported}from'../../site/runtime/thprac128.mjs';
import{createThprac128Recorder}from'../../site/runtime/thprac128-recording.mjs';
import{readThprac128Replay,Thprac128ReplayPlayback}from'../../site/runtime/thprac128-replay.mjs';
import{OwnedReplayStore}from'../../site/runtime/sound.mjs';
import{createReplayIDB}from'../launcher-session/replay-idb.mjs';
const [coreHome,output]=process.argv.slice(2),W='D:/Codex-th128-owned-20261108/menu-lifecycle-fixes-20261113';
const sha=b=>createHash('sha256').update(b).digest('hex');const raw=fs.readFileSync(W+'/Web-delivery-20261116/resource-package-dat-ogg-v2/extracted/games/th128/th128.dat');assert.equal(sha(raw),'38be25a5f1b0bf5e88dd7f66e34e295338d6bb968794a1ff587337f46b3db045');
const dat=openTh128Dat(new Uint8Array(raw)),cache=new Map(),asset=name=>{if(!cache.has(name))cache.set(name,dat.read(name));return cache.get(name);};
const profile=fs.readFileSync(W+'/Web-delivery-20261116/resource-package-v1/extracted/games/th128/assets/runtime/bullet-profile-8.bin');assert.equal(sha(profile),'7427ad4aa6459742ffaa628c7a8e303029a9692b7b3ffbdd5b7f9a3207f0d80e');
const core=await import(pathToFileURL(coreHome+'/core.mjs')),wasmBinary=fs.readFileSync(coreHome+'/core.wasm');let checks=0,factories=0,starts=0;
function check(value,label){assert(value,label);checks++;}
const c={name:'A1stage3chapter2',params:{stage:2,section:10302,score:1234567899,motivation:54321,perfect_freeze:12345,ice_power:6789,ice_area:800},rank:1,root:'st_a1_3.ecl',message:'st_a1_3.msg',group:6};
const evidence=[],identity={coreSHA256:sha(wasmBinary),resourceSHA256:sha(raw)};let recordedFile=null,projection=null;
for(const mode of ['record','playback']){
 const log=[];factories++;const m=await core.default({wasmBinary,print:s=>log.push(s),printErr:s=>log.push(s)});check(thprac128RuntimeSupported(m),'actual new core exports both native consumers');
 const error=()=>m.UTF8ToString(m._th128_world_error()),mem=b=>{const p=m._malloc(b.byteLength);check(p>0,'actual native allocation');m.HEAPU8.set(b,p);return p;};
 const blobs=(names,fn)=>{const arrays=names.map(n=>asset(n)),pointers=arrays.map(mem);try{return fn(pointers,arrays);}finally{for(const p of pointers)m._free(p);}};
 const call=(name,args)=>check(m[name](...args)===1,name+': '+error());
 m._th128_world_reset();for(const n of dat.names.filter(n=>n.endsWith('.ecl')).sort()){const name=new TextEncoder().encode(n),b=asset(n),np=mem(name),bp=mem(b);try{call('_th128_world_load',[np,name.length,bp,b.length]);}finally{m._free(np);m._free(bp);}}
 const s=createThprac128Session(c.params,c.rank);prepareThprac128Native(m,s);check(JSON.parse(m.UTF8ToString(m._th128_world_thprac128_state())).active===false,'pending not falsely published as active');
 const rootName=new TextEncoder().encode(c.root),rootPointer=mem(rootName);try{starts++;call('_th128_world_start',[rootPointer,rootName.length,c.rank,34772]);}finally{m._free(rootPointer);}
 const includes=parseEclIncludes(asset(c.root)).includes;for(const[bank,name]of includes.ANIM.entries())blobs([name],(p,b)=>call('_th128_world_load_enemy_anm',[bank+1,p[0],b[0].length]));
 blobs(['text.anm'],(p,b)=>call('_th128_world_enable_geometry526',[p[0],b[0].length]));
 blobs(['bullet.anm'],(p,b)=>{const pp=mem(profile);try{call('_th128_world_enable_bullets',[pp,profile.length,p[0],b[0].length,0,400]);}finally{m._free(pp);}});
 const before=JSON.parse(m.UTF8ToString(m._th128_world_state()));check(before.frame===0&&before.spawns===0,'real native resources install before practice ECL creates enemies');
 blobs(['pl00a.sht'],(p,b)=>call('_th128_world_enable_bullet_updates',[p[0],b[0].length]));call('_th128_world_enable_graze',[0,s.params.ice_power,0x1234]);
 blobs(['pl00.anm'],(p,b)=>call('_th128_world_enable_player_context',[p[0],b[0].length,1,0,120,0,0,0,0,0,s.params.perfect_freeze]));call('_th128_world_enable_player_motion',[0,51200]);
 blobs(['pl00a.sht','pl00.anm'],(p,b)=>{const flags=mem(new Uint8Array(8000));try{call('_th128_world_enable_player_shots',[p[0],b[0].length,p[1],b[1].length,flags,2000,s.params.ice_area,c.rank,0,0,0,0]);}finally{m._free(flags);}});
 call('_th128_world_enable_shot_hits',[s.params.motivation,0,0,0,0]);const receipt=readThprac128Native(m,s);check(receipt.nonNormal&&receipt.contexts===15,'real JS receipt consumer accepts actual five native resources');
 const native=JSON.parse(m.UTF8ToString(m._th128_world_state()));check(native.flow.playerShotGroupOwner.group===c.group&&native.flow.playerShotGroupOwner.activeHandles>0,'real native SHT/ANM group');
 call('_th128_world_enable_rapid_shot',[-1]);call('_th128_world_enable_player_death',[-1]);blobs(['front.anm'],(p,b)=>call('_th128_world_enable_perfect_hud',[p[0],b[0].length]));
 blobs([c.message,'text.anm','pl00.anm',includes.ANIM[2]],(p,b)=>call('_th128_world_enable_messages',p.flatMap((ptr,i)=>[ptr,b[i].length])));
 check(m._th128_world_records_restore(0,0)===0,'practice never acquires normal record owner');
 const project=s=>({frame:s.frame,spawns:s.spawns,rngCalls:s.rngCalls,playerX:s.playerX,playerY:s.playerY,scoreRaw:s.scoreRaw,motivationRaw:s.motivationRaw,perfectCharge:s.perfectCharge,powerRaw:s.powerRaw,cumulativeFrozenArea:s.cumulativeFrozenArea,playerAnimation:s.playerAnimation,playerWindow:s.playerWindow});
 if(mode==='record'){
  const owner=createThprac128Recorder(m,s,identity,{seconds:10,isCurrent:()=>true}),packets=[[16,16,0],[16,0,0],[0,0,16]];
  for(let i=0;i<packets.length;i++){owner.clock(10+(i+1)/60,60);call('_th128_world_input',packets[i]);call('_th128_world_tick',[]);}
  const native=JSON.parse(m.UTF8ToString(m._th128_world_state()));projection=project(native);check(projection.frame===3&&Number.isFinite(projection.playerY)&&projection.playerY<400,'actual three movement ticks');
  recordedFile=owner.capture(1281268800n,new TextEncoder().encode('TEST    '));const metadata=readThprac128Replay(recordedFile,identity);check(metadata.frames===3&&metadata.stage===3&&metadata.session.params.section===10302,'actual native RPY metadata roundtrip');owner.close();
  const idb=createReplayIDB(),store=new OwnedReplayStore(idb),saved=await store.saveThprac(7,recordedFile,{expectedDigest:null,isCurrent:()=>true});check(saved.strictDurability&&saved.strictFullReadback,'actual file strict synthetic transaction readback');store.close();const reopened=new OwnedReplayStore(idb);recordedFile=await reopened.export(7);check(sha(recordedFile)===saved.digest,'stored full RPY reopens unchanged');reopened.close();
  evidence.push({mode,receipt,projection,recordFrames:3,fileBytes:recordedFile.length,fileSHA256:sha(recordedFile),metadata});
 }else{
  const owner=new Thprac128ReplayPlayback(recordedFile,7,identity);owner.bindNativeStage(m);check(m._th128_world_input(16,16,0)===0&&m._th128_world_set_debug_infinite_respawn(1)===0,'actual native playLock rejects live inputs and assist toggle');
  for(let i=0;i<3;i++)call('_th128_world_tick',[]);const replayState=owner.snapshot(),actual=project(JSON.parse(m.UTF8ToString(m._th128_world_state())));assert.deepEqual(actual,projection);checks++;check(replayState.complete&&replayState.consumed===3,'actual native playback completed exact recorded history');owner.cancel();
  evidence.push({mode,receipt,projection:actual,playback:replayState,sameProjection:true});
 }
 check(log.length===0,'actual native runtime produced no diagnostics');
}
const result={passed:true,checks,Factories:factories,WorldStarts:starts,actualTicks:6,fullCoreCompilations:0,browser:0,GPU:0,storage:'synthetic-strict-IDB',physicalAudio:false,originalDAT:{bytes:raw.length,sha256:sha(raw)},evidence};fs.writeFileSync(output,JSON.stringify(result,null,2)+'\n',{flag:'wx'});console.log(JSON.stringify({passed:true,checks,Factories:factories,WorldStarts:starts,browser:0,nativeCompiles:0}));
