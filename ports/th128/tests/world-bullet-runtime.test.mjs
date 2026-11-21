import assert from 'node:assert/strict';
import test from 'node:test';
import fs from 'node:fs';
import path from 'node:path';
import {fileURLToPath, pathToFileURL} from 'node:url';

const root = path.resolve(path.dirname(fileURLToPath(import.meta.url)), '..');

for(const [initialAnimationSlot,hidden] of [[0,false],[1,false],[0,true]])test(`WASM 501 constructs actual bullet ANM with enemy slot${initialAnimationSlot}${hidden?' hidden':''}`, async () => {
  const build = JSON.parse(fs.readFileSync(path.join(root, 'site/runtime/build.json'), 'utf8'));
  const {default: createRuntime} = await import(pathToFileURL(path.join(root, 'site', build.entry)).href);
  const module = await createRuntime({print: () => {}, printErr: () => {}});
  assert.equal(typeof module._th128_world_enable_bullets, 'function', 'real bullet ANM host is not exported');
  assert.equal(typeof module._th128_world_load_enemy_anm, 'function', 'real enemy appearance bounds loader is missing');
  const local = path.join(root, 'local');
  const catalog = JSON.parse(fs.readFileSync(path.join(local, 'catalog.json'), 'utf8'));
  const directory = path.dirname(path.join(local, catalog.variants.ja.manifest));
  const allocate = bytes => {
    const pointer = module._malloc(bytes.length);
    module.HEAPU8.set(bytes, pointer);
    return pointer;
  };
  for (const name of ['st_a1_1.ecl', 'default.ecl', 'st_a1_1b.ecl']) {
    const nameBytes = Buffer.from(name);
    const bytes = fs.readFileSync(path.join(directory, 'raw', name));
    if((initialAnimationSlot===1||hidden)&&name==='st_a1_1.ecl'){
      const metadata=JSON.parse(fs.readFileSync(path.join(directory,'ecl/st_a1_1.json'),'utf8'));
      const girl=metadata.subroutines.find(sub=>sub.name==='GirlA1b');
      const command=girl.commands.find(command=>command.opcode===262);
      const offset=girl.offset+16+command.offset+16;
      assert.equal(bytes.readInt32LE(offset),0);bytes.writeInt32LE(initialAnimationSlot,offset);
      if(hidden){
        const flag=girl.commands.find(command=>command.opcode===402);
        bytes.writeUInt32LE(32,girl.offset+16+flag.offset+16);
      }
    }
    const namePointer = allocate(nameBytes);
    const dataPointer = allocate(bytes);
    assert.equal(module._th128_world_load(namePointer, nameBytes.length, dataPointer, bytes.length), 1);
    module._free(namePointer);
    module._free(dataPointer);
  }
  const name = Buffer.from('st_a1_1.ecl');
  const namePointer = allocate(name);
  assert.equal(module._th128_world_start(namePointer, name.length, 1, 34772), 1);
  module._free(namePointer);
  const enemyAnimation=fs.readFileSync(path.join(directory,'raw/enemy.anm'));
  const enemyPointer=allocate(enemyAnimation);
  assert.equal(module._th128_world_load_enemy_anm(1,enemyPointer,enemyAnimation.length),1);
  module._free(enemyPointer);
  const profile = fs.readFileSync(path.join(local, 'runtime/bullet-profile-8.bin'));
  const animation = fs.readFileSync(path.join(directory, 'raw/bullet.anm'));
  const profilePointer = allocate(profile);
  const animationPointer = allocate(animation);
  assert.equal(module._th128_world_enable_bullets(profilePointer, profile.length, animationPointer,
    animation.length, 0, 400), 1, module.UTF8ToString(module._th128_world_error()));
  assert.equal(module._th128_world_enable_bullets(profilePointer, profile.length, animationPointer,
    animation.length, 0, -400), 0, 'reconfiguration after local constructor execution must reject');
  module._free(profilePointer);
  module._free(animationPointer);
  let firstEnemyChecked=false;
  for (let frame = 0; frame < 300; ++frame){
    if(!module._th128_world_tick())break;
    if(!firstEnemyChecked&&JSON.parse(module.UTF8ToString(module._th128_world_state())).spawns>0){
      const firstEnemy=JSON.parse(module.UTF8ToString(module._th128_world_bullet_draws())).find(record=>record.bank===1);
      if(hidden){assert.equal(firstEnemy,undefined,'existing ANM receives actual ECL hidden-bit operation');firstEnemyChecked=true;continue;}
      assert.equal(firstEnemy.visualTicks,2,'newly created primary enemy ANM receives native priority30 pass after priority21 birth');
      const physical=JSON.parse(module.UTF8ToString(module._th128_world_state())).entities.find(entity=>!entity.root);
      assert.ok(Math.abs(firstEnemy.x-320-physical.x)<.001&&Math.abs(firstEnemy.y-16-physical.y)<.001,
        'every owned enemy ANM slot receives its physical world position');
      firstEnemyChecked=true;
    }
  }
  const state = JSON.parse(module.UTF8ToString(module._th128_world_state()));
  assert.equal(state.bullets.length, 1);
  assert.equal(state.positionIntegrated, true);
  const bullet = state.bullets[0];
  assert.equal(bullet.type, 8);
  assert.equal(bullet.color, 10);
  assert.equal(bullet.script, 43);
  assert.equal(bullet.motionState, 2);
  assert.equal(bullet.sprite, 328);
  assert.equal(bullet.integer0, 0);
  assert.equal(bullet.visualTicks, 2);
  assert.ok(state.enemyAnimationCount>=1,'actual enemy ANM host must be active');
  assert.ok(bullet.velocityY > 0, 'rejected player target must not change later emission');
  assert.ok(state.entities.find(entity=>!entity.root).y > -32);
  assert.match(module.UTF8ToString(module._th128_world_error()), /bullet update/);
  const draws = JSON.parse(module.UTF8ToString(module._th128_world_bullet_draws()));
  assert.ok(draws.length>=(hidden?1:2));
  assert.equal(draws.some(record=>record.bank===1),!hidden,'ECL controls existing enemy ANM visibility');
  assert.equal(draws[0].sprite, 328);
  assert.equal(draws[0].bank, 0);
  assert.ok(draws[0].width > 0 && draws[0].height > 0);
  assert.ok(Number.isFinite(draws[0].x) && Number.isFinite(draws[0].y));
  assert.ok(Math.abs(draws[0].x - bullet.x - 320) < .01,
    'TH128 bullet draw offset is 128+192, not the TH12 viewport origin');
  assert.equal(module._th128_world_enable_bullets(module.HEAPU8.byteLength - 1, 288, 0, 0, 0, 400), 0);
  assert.equal(JSON.parse(module.UTF8ToString(module._th128_world_state())).bullets.length, 1);
});

test('malformed ANM scale returns valid JSON and an explicit presentation error instead of nan tokens',async()=>{
  const build=JSON.parse(fs.readFileSync(path.join(root,'site/runtime/build.json'),'utf8'));
  const {default:createRuntime}=await import(pathToFileURL(path.join(root,'site',build.entry)).href);
  const module=await createRuntime({print:()=>{},printErr:()=>{}});
  assert.equal(typeof module._th128_world_bullet_draw_error,'function','presentation diagnostics are missing');
  const local=path.join(root,'local');
  const catalog=JSON.parse(fs.readFileSync(path.join(local,'catalog.json'),'utf8'));
  const directory=path.dirname(path.join(local,catalog.variants.ja.manifest));
  const allocate=bytes=>{const pointer=module._malloc(bytes.length);module.HEAPU8.set(bytes,pointer);return pointer;};
  for(const name of ['st_a1_1.ecl','default.ecl','st_a1_1b.ecl']){
    const nameBytes=Buffer.from(name),bytes=fs.readFileSync(path.join(directory,'raw',name));
    const namePointer=allocate(nameBytes),dataPointer=allocate(bytes);
    assert.equal(module._th128_world_load(namePointer,nameBytes.length,dataPointer,bytes.length),1);
    module._free(namePointer);module._free(dataPointer);
  }
  const name=Buffer.from('st_a1_1.ecl'),namePointer=allocate(name);
  assert.equal(module._th128_world_start(namePointer,name.length,1,34772),1);module._free(namePointer);
  const enemy=fs.readFileSync(path.join(directory,'raw/enemy.anm')),enemyPointer=allocate(enemy);
  assert.equal(module._th128_world_load_enemy_anm(1,enemyPointer,enemy.length),1);module._free(enemyPointer);
  const animation=fs.readFileSync(path.join(directory,'raw/bullet.anm'));
  assert.equal(animation.readInt16LE(19156+176),50);
  animation.writeUInt32LE(0x7fc00000,19156+176+8);
  animation.writeUInt32LE(0x7fc00000,19156+176+12);
  const profile=fs.readFileSync(path.join(local,'runtime/bullet-profile-8.bin'));
  const profilePointer=allocate(profile),animationPointer=allocate(animation);
  assert.equal(module._th128_world_enable_bullets(profilePointer,profile.length,animationPointer,animation.length,0,400),1);
  module._free(profilePointer);module._free(animationPointer);
  for(let tick=0;tick<600;++tick)if(!module._th128_world_tick())break;
  const output=module.UTF8ToString(module._th128_world_bullet_draws());
  assert.deepEqual(JSON.parse(output),[]);
  assert.match(module.UTF8ToString(module._th128_world_bullet_draw_error()),/non-finite/i);
  assert.equal(JSON.parse(module.UTF8ToString(module._th128_world_state())).bullets.length,1);
});

test('actual stage updates multiple real bullets and ANMs, stopping at unsupported gameplay instead of first501',async()=>{
  const build=JSON.parse(fs.readFileSync(path.join(root,'site/runtime/build.json'),'utf8'));
  const {default:createRuntime}=await import(pathToFileURL(path.join(root,'site',build.entry)).href);
  const module=await createRuntime({print:()=>{},printErr:()=>{}});
  assert.equal(typeof module._th128_world_enable_bullet_updates,'function','real bullet update runtime is not exported');
  const local=path.join(root,'local'),catalog=JSON.parse(fs.readFileSync(path.join(local,'catalog.json'),'utf8'));
  const directory=path.dirname(path.join(local,catalog.variants.ja.manifest));
  const allocate=bytes=>{const pointer=module._malloc(bytes.length);module.HEAPU8.set(bytes,pointer);return pointer;};
  for(const name of ['st_a1_1.ecl','default.ecl','st_a1_1b.ecl']){
    const n=Buffer.from(name),data=fs.readFileSync(path.join(directory,'raw',name)),np=allocate(n),dp=allocate(data);
    assert.equal(module._th128_world_load(np,n.length,dp,data.length),1);module._free(np);module._free(dp);
  }
  const n=Buffer.from('st_a1_1.ecl'),np=allocate(n);
  assert.equal(module._th128_world_start(np,n.length,1,34772),1);module._free(np);
  const enemy=fs.readFileSync(path.join(directory,'raw/enemy.anm')),ep=allocate(enemy);
  assert.equal(module._th128_world_load_enemy_anm(1,ep,enemy.length),1);module._free(ep);
  const profile=fs.readFileSync(path.join(local,'runtime/bullet-profile-8.bin')),
    animation=fs.readFileSync(path.join(directory,'raw/bullet.anm')),pp=allocate(profile),ap=allocate(animation);
  assert.equal(module._th128_world_enable_bullets(pp,profile.length,ap,animation.length,0,400),1);module._free(pp);module._free(ap);
  const shot=fs.readFileSync(path.join(directory,'raw/pl00a.sht')),sp=allocate(shot);
  assert.equal(module._th128_world_enable_bullet_updates(sp,shot.length),1,module.UTF8ToString(module._th128_world_error()));module._free(sp);
  assert.equal(typeof module._th128_world_enable_graze,'function','real graze scalar/effect host is not exported');
  assert.equal(module._th128_world_enable_graze(0,10000,0x1234),1);
  assert.equal(typeof module._th128_world_enable_player_context,'function','explicit player-contact context is not exported');
  const playerAnimation=fs.readFileSync(path.join(directory,'raw/pl00.anm')),playerPointer=allocate(playerAnimation);
  assert.equal(module._th128_world_enable_player_context(playerPointer,playerAnimation.length,1,0,120,0,0,0,0,0,0),1,
    module.UTF8ToString(module._th128_world_error()));module._free(playerPointer);
  assert.equal(typeof module._th128_world_enable_player_motion,'function','normal-state player movement is not integrated');
  assert.equal(module._th128_world_enable_player_motion(0,51200),1);
  assert.equal(module._th128_world_input(128,128,0),1);
  for(let step=0;step<10;++step)assert.equal(module._th128_world_tick(),1);
  const moved=JSON.parse(module.UTF8ToString(module._th128_world_state()));
  assert.equal(moved.playerX,45);assert.equal(moved.playerY,400);assert.equal(moved.playerAnimation,3);
  assert.equal(module._th128_world_input(64,64,128),1);
  for(let step=0;step<10;++step)assert.equal(module._th128_world_tick(),1);
  assert.equal(module._th128_world_input(0,0,64),1);
  assert.equal(JSON.parse(module.UTF8ToString(module._th128_world_state())).playerX,0);
  let grazeAnimationChecked=false;
  for(let tick=0;tick<6000;++tick){
    if(!module._th128_world_tick())break;
    if(!grazeAnimationChecked&&JSON.parse(module.UTF8ToString(module._th128_world_state())).grazeCount>0){
      const effect=JSON.parse(module.UTF8ToString(module._th128_world_bullet_draws())).find(record=>record.script===175);
      assert.equal(effect.visualTicks,2,'graze effect registers at primary tail and ticks in later priority30 phase');
      assert.equal(effect.effectSystem,'unified-primary','graze/hit/cancellation roots must share one primary-tail scheduler');
      grazeAnimationChecked=true;
    }
  }
  assert.equal(grazeAnimationChecked,true);
  const state=JSON.parse(module.UTF8ToString(module._th128_world_state()));
  assert.ok(state.bullets.length>=2);
  assert.ok(state.bullets.some(bullet=>bullet.age>=20&&bullet.visualTicks>=22));
  assert.ok(state.grazeCount>0);
  assert.ok(state.bullets.some(bullet=>(bullet.flags&4)!==0));
  assert.equal(state.positionIntegrated,true);
  const reason=module.UTF8ToString(module._th128_world_error());
  assert.doesNotMatch(reason,/opcode=501|bullet update collision\/animation\/ETEX scheduling is not implemented/);
  assert.notEqual(reason,'','later unsupported gameplay must still fail stop');
  assert.match(reason,/player death commit/);
  assert.equal(state.playerState,4);assert.equal(state.playerWindow,8);
  assert.ok(state.rngCalls>=256);
  const drawText=module.UTF8ToString(module._th128_world_bullet_draws());
  assert.equal(module.UTF8ToString(module._th128_world_bullet_draw_error()),'');
  assert.ok(JSON.parse(drawText).length>=2);
  assert.ok(JSON.parse(drawText).some(record=>record.bank===7));
  console.log(`UPDATED world frame=${state.frame} bullets=${state.bullets.length} stop=${reason}`);
  module._th128_world_reset();
  for(const file of ['st_a1_1.ecl','default.ecl','st_a1_1b.ecl']){
    const name=Buffer.from(file),data=fs.readFileSync(path.join(directory,'raw',file));
    if(file==='st_a1_1.ecl'){
      const metadata=JSON.parse(fs.readFileSync(path.join(directory,'ecl/st_a1_1.json'),'utf8'));
      const main=metadata.subroutines.find(sub=>sub.name==='main');
      assert.equal(data.readUInt16LE(main.offset+16+20+4),402);
      data.writeUInt16LE(900,main.offset+16+20+4);
    }
    const namePtr=allocate(name),dataPtr=allocate(data);
    assert.equal(module._th128_world_load(namePtr,name.length,dataPtr,data.length),1);
    module._free(namePtr);module._free(dataPtr);
  }
  const restart=allocate(n);assert.equal(module._th128_world_start(restart,n.length,1,34772),1);module._free(restart);
  const badEnemy=allocate(enemy);assert.equal(module._th128_world_load_enemy_anm(1,badEnemy,enemy.length),1);module._free(badEnemy);
  const badProfile=allocate(profile),badAnimation=allocate(animation);
  assert.equal(module._th128_world_enable_bullets(badProfile,profile.length,badAnimation,animation.length,0,400),0);
  module._free(badProfile);module._free(badAnimation);
  const faulted=module.UTF8ToString(module._th128_world_state());
  assert.equal(module._th128_world_enable_graze(9,2900,0x1234),0,'faulted world cannot accept graze configuration');
  assert.equal(module.UTF8ToString(module._th128_world_state()),faulted);
});
