import assert from 'node:assert/strict';
import test from 'node:test';
import fs from 'node:fs';
import path from 'node:path';
import {fileURLToPath,pathToFileURL} from 'node:url';
const root=path.resolve(path.dirname(fileURLToPath(import.meta.url)),'..');
for(const resolveHits of [false,true])test(`real world shot lifecycle and ${resolveHits?'completed hit damage':'explicit unresolved hit'} preserve native boundaries`,async()=>{
  const build=JSON.parse(fs.readFileSync(path.join(root,'site/runtime/build.json')));
  const {default:createRuntime}=await import(pathToFileURL(path.join(root,'site',build.entry)).href);
  const module=await createRuntime({print:()=>{},printErr:()=>{}});
  assert.equal(typeof module._th128_world_enable_player_shots,'function','player shots are not exported');
  const local=path.join(root,'local'),catalog=JSON.parse(fs.readFileSync(path.join(local,'catalog.json'))),
    directory=path.dirname(path.join(local,catalog.variants.ja.manifest));
  const allocate=bytes=>{const pointer=module._malloc(bytes.length);module.HEAPU8.set(bytes,pointer);return pointer;};
  for(const name of ['st_a1_1.ecl','default.ecl','st_a1_1b.ecl']){
    const label=Buffer.from(name),bytes=fs.readFileSync(path.join(directory,'raw',name)),np=allocate(label),dp=allocate(bytes);
    assert.equal(module._th128_world_load(np,label.length,dp,bytes.length),1);module._free(np);module._free(dp);
  }
  const name=Buffer.from('st_a1_1.ecl'),np=allocate(name);
  assert.equal(module._th128_world_start(np,name.length,1,34772),1);module._free(np);
  for(const [bank,file]of [[1,'enemy.anm'],[2,'enemy_ll.anm'],[3,'boss01.anm']]){
    const enemy=fs.readFileSync(path.join(directory,'raw',file)),ep=allocate(enemy);
    assert.equal(module._th128_world_load_enemy_anm(bank,ep,enemy.length),1,`actual ANIM bank${bank}/${file}: ${module.UTF8ToString(module._th128_world_error())}`);module._free(ep);
  }
  const profile=fs.readFileSync(path.join(local,'runtime/bullet-profile-8.bin')),bullet=fs.readFileSync(path.join(directory,'raw/bullet.anm')),
    pp=allocate(profile),bp=allocate(bullet);
  assert.equal(module._th128_world_enable_bullets(pp,profile.length,bp,bullet.length,0,400),1);module._free(pp);module._free(bp);
  if(resolveHits){
    assert.equal(typeof module._th128_world_add_bullet_profile,'function','typed profile ownership export is missing');
    const profile20=fs.readFileSync(path.join(local,'runtime/bullet-profile-20.bin')),p20=allocate(profile20);
    assert.equal(module._th128_world_add_bullet_profile(20,p20,profile20.length),1);module._free(p20);
    const profile18=fs.readFileSync(path.join(local,'runtime/bullet-profile-18.bin')),p18=allocate(profile18);
    assert.equal(module._th128_world_add_bullet_profile(18,p18,profile18.length),1);
    const saved=module.HEAPU8[p18];module.HEAPU8[p18]=43;
    assert.equal(module._th128_world_add_bullet_profile(18,p18,profile18.length),0,'invalid typed replacement cannot replace retained profile');
    module.HEAPU8[p18]=saved;module._free(p18);
  }
  const shot=fs.readFileSync(path.join(directory,'raw/pl00a.sht')),animation=fs.readFileSync(path.join(directory,'raw/pl00.anm')),
    sp=allocate(shot),ap=allocate(animation),sources=allocate(Buffer.alloc(2000*4));
  assert.equal(module._th128_world_enable_bullet_updates(sp,shot.length),1);
  assert.equal(module._th128_world_enable_graze(0,10000,0x1234),1);
  assert.equal(module._th128_world_enable_player_context(ap,animation.length,1,0,120,0,0,0,0,0,0),1);
  assert.equal(module._th128_world_enable_player_motion(0,51200),1);
  assert.equal(module._th128_world_enable_player_shots(sp,shot.length,ap,animation.length,sources,2000,0,1,0,0,0,0),1,module.UTF8ToString(module._th128_world_error()));
  if(resolveHits){
    assert.equal(typeof module._th128_world_enable_shot_hits,'function');
    assert.equal(module._th128_world_enable_shot_hits(100000,0,0,0,0),1,module.UTF8ToString(module._th128_world_error()));
    assert.equal(module._th128_world_enable_rapid_shot(-1),1);
    assert.equal(module._th128_world_enable_player_death(-1),1);
    assert.equal(typeof module._th128_world_enable_perfect_hud,'function','real Perfect Freeze front HUD export is missing');
    const front=fs.readFileSync(path.join(directory,'raw/front.anm')),frontPointer=allocate(front);
    assert.equal(module._th128_world_enable_perfect_hud(frontPointer,front.length),1);module._free(frontPointer);
    const font=fs.readFileSync(path.join(directory,'raw/ascii.anm')),fontPointer=allocate(font);
    assert.equal(module._th128_world_load_score_font(fontPointer,font.length),1);module._free(fontPointer);
    assert.equal(typeof module._th128_world_enable_player_respawn,'function','genuine respawn context export is missing');
    const controlTargets=allocate(Buffer.alloc(8*4));
    assert.equal(module._th128_world_enable_player_respawn(1,0,controlTargets,8),1);module._free(controlTargets);
    assert.equal(module._th128_world_input(512,512,0),1);
  }
  module._free(sp);module._free(ap);module._free(sources);
  const state=()=>JSON.parse(module.UTF8ToString(module._th128_world_state()));
  if(!resolveHits)assert.equal(module._th128_world_emit_player_shots(0),1);
  else assert.equal(module._th128_world_tick(),1);
  const born=state();assert.equal(born.playerShots.length,2);assert.equal(born.playerShots[0].script,5);
  assert.equal(module._th128_world_input(1,1,0),0,'unimplemented full Z dispatcher must remain explicit');
  assert.equal(module._th128_world_tick(),1,module.UTF8ToString(module._th128_world_error()));
  const next=state();assert.equal(next.playerShots[0].age,born.playerShots[0].age+1);assert.equal(next.playerShots[0].y,born.playerShots[0].y-24);
  assert.equal(next.playerShots[0].visualTicks,born.playerShots[0].visualTicks+1);
  const draw=JSON.parse(module.UTF8ToString(module._th128_world_bullet_draws())).filter(record=>record.id>=5000000&&record.id<6000000);
  assert.equal(draw.length,2);assert.equal(draw[0].bank,7);assert.equal(draw[0].mode,1);
  if(resolveHits)assert.equal(module._th128_world_input(0,0,512),1);
  for(let tick=0;tick<45;tick++)assert.equal(module._th128_world_tick(),1,module.UTF8ToString(module._th128_world_error()));
  assert.equal(state().playerShots.length,0);
  if(resolveHits){assert.equal(module._th128_world_input(512,512,0),1);}else{
    assert.equal(module._th128_world_emit_player_shots(0),1);assert.equal(state().playerShots[0].slot,0);
  }
  let stopped=false;
  let deathObserved=false,type18Observed=false,type20Observed=false;
  for(let tick=0;tick<360;tick++){
    if(!resolveHits&&tick%15===0){assert.equal(module._th128_world_emit_player_shots(0),1);}
    if(!module._th128_world_tick()){stopped=true;break;}
    if(resolveHits&&state().enemiesDefeated>0){deathObserved=true;break;}
  }
  if(resolveHits){assert.equal(deathObserved,true);assert.equal(stopped,false);}else{
    assert.equal(stopped,true);assert.match(module.UTF8ToString(module._th128_world_error()),/shot hit.*0x43ec8a/);
  }
  assert.ok(state().playerShots.some(value=>value.hitMarker===1));
  if(resolveHits){
    assert.ok(state().entities.some(value=>!value.root&&value.life<0),'real completed hit query subtracts actual native HP');
    assert.ok(state().damageStats[1]>0);assert.equal(state().motivationRaw,100000);
    assert.ok(state().scoreRaw>0,'completed genuine shot query must run its native score tail');
    assert.ok(state().playerShots.some(value=>value.script===6&&value.active===2));
    assert.ok(state().items.length>0&&state().entities.some(value=>!value.root&&!value.active));
    assert.ok(state().effectCount>=10);
    assert.equal(module._th128_world_input(0,0,512),1);
    const respawnTrace=[];let normalObserved=false;
    for(let tick=0;tick<6000;tick++){
      if(!module._th128_world_tick())break;
      const current=state();
      assert.equal(current.perfectCharge,Math.min(30000,current.deathsCommitted*2500),'each completed death awards the original2500 charge');
      if(current.bullets.some(value=>value.type===18)){type18Observed=true;assert.ok(current.bullets.filter(value=>value.type===18).every(value=>value.script===72));}
      if(current.bullets.some(value=>value.type===20)){type20Observed=true;assert.ok(current.bullets.filter(value=>value.type===20).every(value=>value.script===74));}
      if(tick%30===0){
        const records=JSON.parse(module.UTF8ToString(module._th128_world_bullet_draws()));
        assert.equal(new Set(records.map(value=>value.id)).size,records.length,'live score glyphs, charge HUD, children and gameplay nodes need disjoint presentation IDs');
      }
      if(current.deathsCommitted>0&&respawnTrace.length<130)respawnTrace.push({frame:current.frame,state:current.playerState,
        timer:current.playerWindow,protection:current.playerProtection,y:current.playerY,fields:current.damageFieldCount,
        shotCycle:current.shotCycle,perfectCharge:current.perfectCharge,
        cancelled:current.bullets.filter(value=>value.motionState===3).length});
      if(current.respawnsCommitted>0&&current.playerState===1)normalObserved=true;
    }
    assert.ok(state().deathsCommitted>=1,'actual first-hit window executes death commit');
    assert.equal(respawnTrace[0].perfectCharge,2500);assert.ok(state().motivationRaw<=90002);
    assert.ok(state().respawnsCommitted>=1,'actual first death must execute respawn field and return to normal movement');
    assert.ok(state().playerRespawnControl.mode04===2);
    assert.deepEqual(state().playerRespawnControl.pending,[2,2,7,3,3,3,3,3]);
    assert.equal(normalObserved,true,'entry motion must actually restore normal player state');
    assert.ok(respawnTrace.some(value=>value.fields===1&&value.state===0&&value.y===480));
    assert.ok(respawnTrace.some(value=>value.state===0&&value.cancelled>0),'entry invokes real bullet cancellation IRQ/effects');
    assert.ok(respawnTrace.filter(value=>value.state===0).every(value=>value.shotCycle===-1),
      'entry clears movement deltas, not the independent rapid-fire timer; released C must not restart shooting');
    assert.doesNotMatch(module.UTF8ToString(module._th128_world_error()),/respawn.*0x43df90/);
    assert.ok(state().frame>721,'real rectangular enemy appearance must pass the old721 boundary');
    assert.equal(type18Observed,true,'real stage must emit original typed18/ANM72 bullets past840');
    assert.ok(state().frame>840,'actual typed18 emission must pass the old840 boundary');
    assert.ok(state().frame>1045,'genuine additional item drops must pass the old1045 boundary');
    const stopped20=state().bullets.filter(value=>value.type===20);
    assert.ok(type20Observed||stopped20.length>0,`real type20/ANM74 must emit at1380; frame=${state().frame} error=${module.UTF8ToString(module._th128_world_error())}`);
    assert.ok(stopped20.every(value=>value.script===74));
    assert.ok(state().frame>1405,'genuine type16 update must pass the old1405 continuation boundary');
    assert.ok(state().frame>1678,'actual fourth-death threshold must execute front47 notice and charge-child update');
    assert.ok(state().frame>1730,`type4 must pass the old1730 boundary; actual=${state().frame} ${module.UTF8ToString(module._th128_world_error())}`);
    assert.equal(state().frame,3170,'real stage reaches MBoss, not an invented replacement encounter');
    assert.match(module.UTF8ToString(module._th128_world_error()),/offset=708 opcode=526: unregistered host opcode 526/);
    assert.deepEqual(state().bossGauges.map(value=>({boss:value.boss,slot:value.slot,color:value.color})),
      [{boss:0,slot:0,color:0xffff8080},{boss:0,slot:1,color:0xff800000}]);
    assert.ok(Math.abs(state().bossGauges[0].ratio-Math.fround(6300/10900))<.000001);
    assert.ok(Math.abs(state().bossGauges[1].ratio-Math.fround(2000/10900))<.000001);
    const registeredBoss=state().entities.find(value=>value.active&&(value.flags&0x1000000));
    assert.ok(registeredBoss,'original412 must commit a real boss before its secondary ANM requests');
    assert.equal(registeredBoss.life,10900,'real Normal411 sets the original boss HP words');
    assert.equal(registeredBoss.bank0,2,'actual MBoss primary uses native second include, not an enemy1 substitute');
    assert.equal(registeredBoss.phase0.threshold,6300,'the actual Normal branch retains its own414 threshold and jumps past the harder branch');
    assert.equal(registeredBoss.phase0.limit,1800);
    const bossDraws=JSON.parse(module.UTF8ToString(module._th128_world_bullet_draws()));
    assert.equal(new Set(bossDraws.map(value=>value.id)).size,bossDraws.length,'roots and attached enemy children have distinct draw identities');
    const bossAura=bossDraws.find(value=>value.id===2000000+registeredBoss.id*16+1);
    assert.ok(bossAura&&bossAura.bank===0&&bossAura.sprite===454,'actual259/99 aura must use bullet bank0, not enemy/boss sprite substitution');
    const body=bossDraws.find(value=>value.id===2000000+registeredBoss.id*16);
    assert.ok(body&&body.bank===2&&body.width===64,'actual enemy_ll primary is loaded and rendered from its own atlas');
    const child=bossDraws.find(value=>value.id>=8000000&&value.script===186);
    assert.ok(child&&child.bank===0&&child.sprite===472&&Math.abs(child.x-body.x)<=4.001&&Math.abs(child.y-body.y)<=.001,
      'actual186 child has authored sprite and parent pose exactly once, not a flat or doubled translation');
    if(stopped20.length){
      const draws=JSON.parse(module.UTF8ToString(module._th128_world_bullet_draws()));
      for(const bullet of stopped20){
        const draw=draws.find(value=>value.id===bullet.id);assert.ok(draw);
        assert.ok((draw.flags&0x40000000)!==0,JSON.stringify({id:bullet.id,script:bullet.script,flags:draw.flags,rotation:draw.rotation,
          vx:bullet.velocityX,vy:bullet.velocityY,visualTicks:bullet.visualTicks}));
        const expected=Math.fround(Math.atan2(bullet.velocityY,bullet.velocityX)+Math.fround(Math.PI/2));
        assert.ok(Math.abs(draw.rotation-expected)<.000002||Math.abs(draw.rotation-expected+Math.fround(2*Math.PI))<.000002,
          'type20 uses genuine motion angle plus native half-pi at draw time');
      }
    }
    fs.writeFileSync(path.join(root,'artifacts/tests/respawn-world.json'),JSON.stringify({runtime:build.generation,frame:state().frame,
      error:module.UTF8ToString(module._th128_world_error()),deaths:state().deathsCommitted,respawns:state().respawnsCommitted,
      playerState:state().playerState,normalObserved,trace:respawnTrace},null,2)+'\n');
  }else assert.ok(state().entities.some(value=>!value.root&&value.life===20),'positive prefix is not completed HP damage');
  if(!resolveHits){const retained=JSON.stringify(state());assert.equal(module._th128_world_tick(),0);assert.equal(JSON.stringify(state()),retained);}
  module._th128_world_reset();assert.equal(state().playerShots.length,0);
});
