import test from 'node:test';
import assert from 'node:assert/strict';
import fs from 'node:fs';
import path from 'node:path';
import {fileURLToPath} from 'node:url';
import {spawnSync} from 'node:child_process';

const root=path.resolve(path.dirname(fileURLToPath(import.meta.url)),'..');
test('all16 original route time-limit inventories preserve authored414 values without claiming completed fights',()=>{
  const result=spawnSync(process.execPath,['tools/inventory-boss-timers.mjs'],{cwd:root,encoding:'utf8'});assert.equal(result.status,0,result.stdout+result.stderr);
  const inventory=JSON.parse(fs.readFileSync(path.join(root,'docs/boss-timer-inventory.json')));assert.equal(inventory.routes.length,16);
  assert.equal(inventory.logicTicksPerSecond,60);assert.ok(inventory.routes.every(route=>route.gameplayComplete===false));
  const first=inventory.routes.find(route=>route.stage==='st_a1_1.ecl');assert.ok(first);
  for(const [subroutine,frames] of [['MBoss',1800],['Boss1',3000],['BossCard1',2280],['BossCard2',3000]]){
    assert.ok(first.phases.some(phase=>phase.subroutine===subroutine&&phase.timeLimitFrames===frames&&phase.timeLimitSeconds===frames/60));
  }
  const catalog=JSON.parse(fs.readFileSync(path.join(root,'local/catalog.json'))),directory=path.dirname(path.join(root,'local',catalog.variants.ja.manifest));
  for(const route of inventory.routes)for(const phase of route.phases){
    const program=JSON.parse(fs.readFileSync(path.join(directory,'ecl',phase.source.replace(/\.ecl$/,'.json'))));
    const command=program.subroutines.find(subroutine=>subroutine.name===phase.subroutine).commands.find(command=>command.offset===phase.offset);
    assert.equal(command.opcode,414);if(phase.literalTime)assert.equal(phase.timeLimitFrames,Buffer.from(command.raw,'hex').readInt32LE(8));
  }
});
