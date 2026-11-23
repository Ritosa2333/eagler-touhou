import assert from 'node:assert/strict';
import {createHash} from 'node:crypto';
import {readFile} from 'node:fs/promises';
import {pathToFileURL} from 'node:url';
import {parseEclIncludes} from '../../th128/site/runtime/ecl-includes.mjs';
import {openTh128Dat} from '../../th128/site/runtime/th128-dat.mjs';
import {loadOriginalType33ForWorld} from '../../th128/site/runtime/original-bullet-type33.mjs';
import {createMultiplayerRules} from '../../th128/site/runtime/multiplayer-rules.mjs';
import {createMultiplayerSessionOwner} from '../../th128/site/runtime/multiplayer-session.mjs';
import {createMultiplayerRound} from '../runtime/round.mjs';
import {bindNativeRollbackWorld} from '../runtime/native-world.mjs';

const read=async path=>new Uint8Array(await readFile(path));
const sha=bytes=>createHash('sha256').update(bytes).digest('hex');
const [pinFile]=process.argv.slice(2);
const config=JSON.parse(await readFile(pinFile,'utf8'));
const [[core,wasm],[dat,atlas],[stagedProfiles,iceMasks,baseBullet,type9,type32]]=await Promise.all([
  Promise.all([read(config.core),read(config.wasm)]),Promise.all([read(config.dat),read(config.atlas)]),
  Promise.all(['profiles','iceMasks','baseBullet','type9','type32'].map(key=>read(config[key]))),
]);
const decoder=new TextDecoder(),encoder=new TextEncoder(),seed=34772,rank=0;
assert.equal(sha(core),config.coreSHA256);assert.equal(sha(wasm),config.wasmSHA256);assert.equal(sha(dat),config.datSHA256);
const factory=(await import(pathToFileURL(config.core))).default,archive=openTh128Dat(dat);
const stage=JSON.parse(decoder.decode(atlas)).stages.find(row=>row.id===3);
assert.equal(stage.ecl,'st_a1_3.ecl');assert.equal(stage.message,'st_a1_3.msg');
const ecls=new Map();function add(name){if(ecls.has(name))return;const bytes=archive.read(name);ecls.set(name,bytes);for(const child of parseEclIncludes(bytes).includes.ECLI)add(child);}
add(stage.ecl);
const names=parseEclIncludes(ecls.get(stage.ecl)).includes.ANIM;
const [bullet,text,player,shot,front,font,card,boss,ice,msg]=['bullet.anm','text.anm','pl00.anm','pl00a.sht','front.anm','ascii.anm','card.anm',names[2],'ice.anm',stage.message].map(name=>archive.read(name));
const knownProfiles=await Promise.all(decoder.decode(stagedProfiles).trim().split(/\r?\n/).map(async line=>{const type=Number(line);return [type,await read(config.bullets[type])];}));
for(const [type,body] of [[9,type9],[32,type32]]){
  const row=JSON.parse(decoder.decode(body)),bytes=Buffer.from(row.hex,'hex');assert.equal(row.type,type);assert.equal(bytes.length,288);assert.equal(sha(bytes),row.sha256);knownProfiles.push([type,bytes]);
}
const rules=createMultiplayerRules({schema:'th128-multiplayer-rules/1',matchId:'core127-native-rollback',epoch:1,mode:'parallel-stage',
  route:'A1-3',seed,coreSHA256:config.wasmSHA256,resourceSHA256:config.datSHA256,inputDelay:0,rollbackWindow:2});
const counts={factories:0,worldStarts:0,ticks:0};
const total=132,lateFrame=120;

async function createWorld(){
  assert.ok(++counts.factories<=2);
  const m=await factory({wasmBinary:wasm,print:()=>{},printErr:message=>{throw Error(message);}});
  const ok=value=>assert.equal(value,1,m.UTF8ToString(m._th128_world_error()));
  const alloc=bytes=>{const ptr=m._malloc(bytes.length);assert.ok(ptr);m.HEAPU8.set(bytes,ptr);return ptr;};
  function bind(name,arrays,head=[],tail=[]){
    const pointers=[];
    try{const args=arrays.flatMap(bytes=>{const ptr=alloc(bytes);pointers.push(ptr);return [ptr,bytes.length];});ok(m[`_th128_world_${name}`](...head,...args,...tail));}
    finally{for(const ptr of pointers)m._free(ptr);}
  }
  m._th128_world_reset();
  for(const [name,bytes] of ecls)bind('load',[encoder.encode(name),bytes]);
  assert.ok(++counts.worldStarts<=2);
  bind('start',[encoder.encode(stage.ecl)],[],[rank,seed]);
  names.forEach((name,index)=>bind('load_enemy_anm',[archive.read(name)],[index+1]));
  bind('enable_geometry526',[text]);bind('enable_bullets',[baseBullet,bullet],[],[0,400]);bind('enable_bullet_updates',[shot]);
  ok(m._th128_world_enable_graze(0,10000,0x1234));
  bind('enable_player_context',[player],[],[1,0,120,0,0,0,0,0,0]);ok(m._th128_world_enable_player_motion(0,51200));
  const groups=alloc(new Uint8Array(8000));
  try{bind('enable_player_shots',[shot,player],[],[groups,2000,0,rank,0,0,0,0]);}finally{m._free(groups);}
  ok(m._th128_world_enable_shot_hits(100000,0,0,0,0));ok(m._th128_world_enable_rapid_shot(-1));ok(m._th128_world_enable_player_death(-1));
  bind('enable_perfect_hud',[front]);bind('load_score_font',[font]);bind('enable_spells',[bullet,boss,text,font,card,player]);
  const respawn=alloc(new Uint8Array(32));try{ok(m._th128_world_enable_player_respawn(1,0,respawn,8));}finally{m._free(respawn);}
  ok(m._th128_world_enable_head_power());ok(m._th128_world_enable_focus_indicator());bind('enable_z_fire',[ice,iceMasks]);
  for(const [type,bytes] of knownProfiles)bind('add_bullet_profile',[bytes],[type]);
  await loadOriginalType33ForWorld({module:m,memory:alloc});
  bind('enable_messages',[msg,text,player,boss]);bind('enable_message_front',[front]);
  const engine=bindNativeRollbackWorld({module:m,seat:1,checkpointSlots:2,maxCheckpointBytes:134217728,beforeTick(){assert.ok(++counts.ticks<=2*total+2);}});
  return {engine,state:()=>JSON.parse(m.UTF8ToString(m._th128_world_state())),
    draws:()=>m.UTF8ToString(m._th128_world_bullet_draws()),heapBytes:m.HEAPU8.length};
}
async function setup(){
  const world=await createWorld(),session=await createMultiplayerSessionOwner({rules,allowedMask:763,localPlayer:0});
  const confirmed=[];
  const round=createMultiplayerRound({session,localPlayer:0,engine:world.engine,sendInput(){},onConfirmed:row=>confirmed.push(row),maxFrames:total});
  const deliver=(frame,held)=>round.receiveInput({matchId:rules.matchId,epoch:rules.epoch,player:1,frame,held},1);
  return {...world,round,session,deliver,confirmed};
}
const reference=await setup(),delayed=await setup();
const heldAt=frame=>frame===lateFrame?128:0;
try{
  for(let frame=0;frame<total;frame++){
    reference.deliver(frame,heldAt(frame));assert.equal(reference.round.tick(0).advanced,true);
    if(frame!==lateFrame)delayed.deliver(frame,heldAt(frame));
    assert.equal(delayed.round.tick(0).advanced,true);
    if(frame===lateFrame+1){
      const wrong=delayed.state(),right=reference.state();assert.equal(wrong.frame,right.frame);
      assert.notEqual(wrong.playerX,right.playerX,'late input must diverge in the real native player before rewinding');
      assert.equal(delayed.confirmed.at(-1).frame,lateFrame-1);
      delayed.deliver(lateFrame,heldAt(lateFrame));delayed.round.flush();
      assert.deepEqual(delayed.state(),reference.state(),'all public native gameplay diagnostics after correction');
      assert.equal(delayed.draws(),reference.draws(),'original native draw records after correction');
      console.log(JSON.stringify({correctionFrame:wrong.frame,beforePlayerX:wrong.playerX,correctedPlayerX:delayed.state().playerX}));
    }
  }
  assert.equal(counts.ticks,2*total+2);assert.equal(delayed.round.status().replayedFrames,2);
  assert.equal(delayed.state().frame,total);assert.deepEqual(delayed.state(),reference.state());
  assert.equal(delayed.draws(),reference.draws());assert.deepEqual(delayed.confirmed,reference.confirmed);
  assert.equal(delayed.confirmed.length,total);
  const snapshot=delayed.state();assert.equal(snapshot.normalRecord.bound,false);assert.equal(snapshot.normalRecord.scoreEligible,false);
  const report={case:'Core127 TH128 multiplayer rollback differential',stage:stage.ecl,seed,rank,
    counts,confirmed:total,corrected:delayed.round.status().replayedFrames,heapBytes:delayed.heapBytes,
    frame:total,playerX:snapshot.playerX,playerY:snapshot.playerY,stateSHA256:sha(JSON.stringify(snapshot)),drawSHA256:sha(delayed.draws()),
    browser:0,network:0,audioDevices:0,profileWrites:0};
  console.log(JSON.stringify(report));
}finally{for(const owner of [delayed,reference]){owner.round.close();owner.session.close();owner.engine.close();}}
