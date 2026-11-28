import assert from 'node:assert/strict';
import test from 'node:test';
import fs from 'node:fs';
import path from 'node:path';
import {fileURLToPath,pathToFileURL} from 'node:url';
const root=path.resolve(path.dirname(fileURLToPath(import.meta.url)),'..');
async function freshWorld(protection=65535){
 const build=JSON.parse(fs.readFileSync(path.join(root,'site/runtime/build.json'))),{default:create}=await import(pathToFileURL(path.join(root,'site',build.entry)).href);
 const module=await create({print(){},printErr(){}}),local=path.join(root,'local'),catalog=JSON.parse(fs.readFileSync(path.join(local,'catalog.json')));
 const directory=path.dirname(path.join(local,catalog.variants.ja.manifest));
 const allocate=bytes=>{const pointer=module._malloc(bytes.length);module.HEAPU8.set(bytes,pointer);return pointer;};
 const call=(name,bytes,...prefix)=>{const pointer=allocate(bytes);const result=module[name](...prefix,pointer,bytes.length);module._free(pointer);return result;};
 const raw=name=>fs.readFileSync(path.join(directory,'raw',name));
 const error=()=>module.UTF8ToString(module._th128_world_error()),state=()=>JSON.parse(module.UTF8ToString(module._th128_world_state()));
 for(const name of ['st_a1_1.ecl','default.ecl','st_a1_1b.ecl']){
  const encoded=Buffer.from(name),np=allocate(encoded),bytes=raw(name),dp=allocate(bytes);
  assert.equal(module._th128_world_load(np,encoded.length,dp,bytes.length),1,error());module._free(np);module._free(dp);
 }
 const encoded=Buffer.from('st_a1_1.ecl'),np=allocate(encoded);assert.equal(module._th128_world_start(np,encoded.length,1,34772),1,error());module._free(np);
 for(const [bank,name] of [[1,'enemy.anm'],[2,'enemy_ll.anm'],[3,'boss01.anm']])assert.equal(call('_th128_world_load_enemy_anm',raw(name),bank),1,error());
 const profile=fs.readFileSync(path.join(local,'runtime/bullet-profile-8.bin')),bullet=raw('bullet.anm'),pp=allocate(profile),bp=allocate(bullet);
 assert.equal(module._th128_world_enable_bullets(pp,profile.length,bp,bullet.length,0,400),1,error());module._free(pp);module._free(bp);
 for(const type of [18,20])assert.equal(call('_th128_world_add_bullet_profile',fs.readFileSync(path.join(local,`runtime/bullet-profile-${type}.bin`)),type),1,error());
 const shot=raw('pl00a.sht'),player=raw('pl00.anm'),sp=allocate(shot),ap=allocate(player),flags=allocate(Buffer.alloc(8000));
 assert.equal(module._th128_world_enable_bullet_updates(sp,shot.length),1,error());assert.equal(module._th128_world_enable_graze(0,10000,0),1,error());
 assert.equal(module._th128_world_enable_player_context(ap,player.length,1,0,protection,0,0,0,0,0,0),1,error());
 assert.equal(module._th128_world_enable_player_motion(0,51200),1,error());
 assert.equal(module._th128_world_enable_player_shots(sp,shot.length,ap,player.length,flags,2000,0,1,0,0,0,0),1,error());
 module._free(sp);module._free(ap);module._free(flags);
 assert.equal(module._th128_world_enable_shot_hits(100000,0,0,0,0),1,error());assert.equal(module._th128_world_enable_rapid_shot(-1),1,error());
 assert.equal(module._th128_world_enable_player_death(-1),1,error());assert.equal(call('_th128_world_enable_perfect_hud',raw('front.anm')),1,error());
 const control=allocate(Buffer.alloc(32));assert.equal(module._th128_world_enable_player_respawn(1,0,control,8),1,error());module._free(control);
 assert.equal(call('_th128_world_load_score_font',raw('ascii.anm')),1,error());assert.equal(module._th128_world_enable_head_power(),1,error());
 const masks=JSON.parse(fs.readFileSync(path.join(directory,'animations/ice-masks.json'))),ice=raw('ice.anm');
 const maskBytes=Buffer.concat(masks.profiles.map(profile=>Buffer.from(profile.hex,'hex'))),ip=allocate(ice),mp=allocate(maskBytes);
 assert.equal(module._th128_world_input(1,1,0),0,'Z remains opt-in until every owning service is ready');
 assert.equal(module._th128_world_enable_z_fire(ip,ice.length,mp,maskBytes.length-1),0);
 assert.equal(module._th128_world_enable_z_fire(ip,ice.length,mp,maskBytes.length),1,error());
 assert.equal(module._th128_world_enable_z_fire(ip,ice.length,mp,maskBytes.length),0,'cannot replace a live owning host');module._free(ip);module._free(mp);
 const tick=count=>{for(let index=0;index<count;++index)assert.equal(module._th128_world_tick(),1,error());return state();};
 const draws=()=>JSON.parse(module.UTF8ToString(module._th128_world_bullet_draws()));
 return {module,error,state,tick,draws};
}
test('published WASM Z input owns charge/ice visuals, nearby real bullet transfer and complete shatter accounting',async()=>{
 const {module,error,state,tick,draws}=await freshWorld();assert.equal(typeof module._th128_world_sound_operation,'function');
 assert.equal(module._th128_world_input(1,1,0),1,error());const tapped=tick(1);assert.equal(tapped.heldZ,1);assert.equal(tapped.playerShots.length,2);
 assert.ok(draws().some(draw=>draw.bank===36),'original ice30 charge visuals must be bound on first held unit');
 assert.equal(module._th128_world_input(0,0,1),1,error());assert.equal(tick(1).ice.busy,0);
 assert.equal(module._th128_world_input(1,1,0),1,error());const charged=tick(45);assert.equal(charged.heldZ,45);assert.equal(charged.shotCycle,-1);
 const head=draws().filter(draw=>draw.id>=12000000&&draw.id<12000100);assert.deepEqual(head.map(draw=>draw.sprite),[259,258,258,269]);assert.equal(head[0].sx,2);
 let contact;
 for(let count=0;count<600;++count){
  const snapshot=state(),eligible=snapshot.bullets.filter(bullet=>bullet.type===8&&(bullet.flags&1));
  eligible.sort((first,second)=>Math.hypot(first.x-snapshot.playerX,first.y-snapshot.playerY)-Math.hypot(second.x-snapshot.playerX,second.y-snapshot.playerY));
  const nearest=eligible[0];if(nearest&&Math.hypot(nearest.x-snapshot.playerX,nearest.y-snapshot.playerY)<40){contact=nearest;break;}
  let holding=1;if(nearest){const dx=nearest.x-snapshot.playerX,dy=nearest.y-snapshot.playerY;
   if(dx>10)holding|=128;else if(dx< -10)holding|=64;if(dy>10)holding|=32;else if(dy< -10)holding|=16;}
  assert.equal(module._th128_world_input(holding,0,0),1,error());tick(1);
 }
 assert.ok(contact,'the actual stage must supply a nearby freeze-eligible bullet');
 assert.equal(module._th128_world_input(0,0,1),1,error());const released=tick(1);
 assert.equal(released.powerRaw,0);assert.equal(released.heldZ,-40);assert.equal(released.ice.busy,1);assert.ok(released.ice.converted>0);
 assert.ok(!released.bullets.some(bullet=>bullet.id===contact.id));assert.ok(draws().some(draw=>draw.id>=13100000&&draw.layer===13&&draw.bank===0));
 const readBefore=state(),drawBefore=draws();assert.deepEqual(draws(),drawBefore);assert.deepEqual(state(),readBefore);
 let complete=state();for(let count=0;count<130&&!complete.ice.completions;++count)complete=tick(1);
 assert.equal(complete.ice.completions,1);assert.equal(complete.ice.busy,0);assert.ok(complete.ice.shattered>0);
 assert.ok(complete.ice.pixels>0);assert.ok(complete.lastFrozenArea>0);assert.ok(complete.freezeBonus>0);assert.ok(complete.perfectCharge>0);
 assert.ok(complete.displayRandomCalls>0);assert.equal(complete.deathsCommitted,0,'test-only explicit high protection avoids hit-window ambiguity');
 assert.ok(Array.from({length:module._th128_world_sound_count()},(_,index)=>module._th128_world_sound_operation(index)).includes(1));
 fs.writeFileSync(path.join(root,'docs/z-fire-runtime.json'),JSON.stringify({
  generation:JSON.parse(fs.readFileSync(path.join(root,'site/runtime/build.json'))).generation,
  context:{variant:'ja',difficulty:1,seed:34772,protection:65535,scope:'explicit high protection for isolated actual-stage contact test, not the browser initial state'},
  tapped,charged,contact,released,complete},null,2)+'\n');
});

test('holding Z through a real hit stops native charge sound and continues the original death/respawn window',async()=>{
 const {module,error,state,tick}=await freshWorld(120);
 assert.equal(module._th128_world_input(1,1,0),1,error());
 let overlap=false,committed=false;
 for(let frame=0;frame<1300;++frame){
  const current=tick(1);if(current.playerState===4)overlap=true;
  if(current.deathsCommitted){committed=true;break;}
 }
 assert.ok(overlap&&committed,'an actual native bullet hit must execute charge-stop then death commit');
 assert.equal(state().heldZ,0);assert.equal(module._th128_world_input(0,0,1),1,error());
 for(let frame=0;frame<200&&state().playerState!==1;++frame)tick(1);
 assert.equal(state().playerState,1);assert.ok(state().respawnsCommitted>0);
});
