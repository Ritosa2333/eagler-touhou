import assert from 'node:assert/strict';
import test from 'node:test';
import fs from 'node:fs';
import path from 'node:path';
import {fileURLToPath} from 'node:url';
import {createHash} from 'node:crypto';
import {spawnSync} from 'node:child_process';

const root = path.resolve(path.dirname(fileURLToPath(import.meta.url)), '..');

test('production shot/enemy query and accounting API is present', () => {
  for (const file of ['src/game/ShotEnemyCollisionRules.hpp', 'src/game/ShotEnemyCollisionRules.cpp'])
    assert.ok(fs.existsSync(path.join(root, file)), `missing production file ${file}`);
  const header = fs.readFileSync(path.join(root, 'src/game/ShotEnemyCollisionRules.hpp'), 'utf8');
  for (const name of ['planOrdinaryShotEnemyContact', 'planShotEnemyQuery', 'planEnemyDamageAfterCompletedQuery'])
    assert.ok(header.includes(name), `missing production API ${name}`);
});

test('original contact, query and accounting boundaries replay in WASM and UBSan without completed-hit fiction',()=>{
  const digest=value=>createHash('sha256').update(value).digest('hex');
  const raw=fs.readFileSync(path.join(root,'local/research/shot-enemy-collision-fixtures.json'));
  assert.equal(digest(raw),'298825ed7815f9f3bff29aca6d8fa2827c85ece29bcaa45d391286aa32359a73');
  const report=JSON.parse(raw);
  assert.equal(report.caseCount,190);
  for(const sample of report.birthLifeCases)assert.deepEqual(sample.lifeWords,[sample.life>>>0,sample.life>>>0]);
  for(const sample of report.ordinaryPostDamageCases){
    assert.equal(sample.stop.reason,'returned');assert.equal(sample.registers.eax,0);
    assert.equal(sample.lifeAfter[2],sample.life>>>0);assert.equal(sample.lifeAfter[4],0);
  }
  for(const sample of report.enemyGateCases){
    assert.equal(sample.flagsAfter,sample.flags&~0x400000);
    assert.equal(sample.queryRequired,(sample.flagsAfter&0x21)===0);
  }
  assert.equal(report.executable.sha256,'ba515750ece845b56640fe440592fc39ebb7bdd1ed87c541b67a19b148e767d4');
  assert.equal(report.executable.sha256After,report.executable.sha256);
  assert.equal(report.windowsGameProcessLaunched,false);
  assert.equal(digest(fs.readFileSync(path.join(root,'tools/research/shot_enemy_collision_probe.py'))),report.generatorSha256);
  for(const source of Object.values(report.code))assert.equal(digest(Buffer.from(source.bytes,'hex')),source.sha256);
  for(const source of Object.values(report.constants))assert.equal(digest(Buffer.from(source.bytes,'hex')),source.sha256);
  for(const [name,hash]of Object.entries(report.engineSha256))assert.equal(digest(fs.readFileSync(path.join(root,'tools/research',name))),hash);
  const input=[],checks=[];
  const shape=sample=>[...sample.positionBits,...sample.sizeBits];
  for(const sample of report.contactCases){
    input.push(['C',...shape(sample),sample.slotBefore,sample.kind,2,sample.callback].join(' '));
    checks.push(line=>{
      const fields=line.split(' '),values=fields.slice(0,9).map(Number);
      if(sample.kind!==0){assert.equal(values[0],3);assert.equal(fields[9],sample.slotBefore);return;}
      if(!sample.supportedGeometry){assert.equal(values[0],6);assert.equal(fields[9],sample.slotBefore);return;}
      if(sample.stop.reason==='returned')assert.deepEqual(values.slice(0,4),[1,0,0,1]);
      else if(sample.stop.address==='0x0043ec10')assert.deepEqual(values.slice(0,4),[4,0,0,0]);
      else assert.deepEqual(values.slice(0,4),[5,sample.damagePrefix,1,0]);
      assert.equal(fields[9],sample.slotAfter);
      assert.deepEqual(fields.slice(10).map(Number),sample.enemyEdgesBits);
      if(values[0]===5)assert.deepEqual(values.slice(4),[4,0x43ec8a,0x467780,0x390000,0x12345678]);
    });
  }
  for(const sample of [...report.completeMissReturns,...report.sourcePrefixes,...report.queryPrefixes]){
    input.push(['Q',...shape(sample),7,sample.sameTime?7:8,Number(!sample.nullOut),sample.outFlagBefore,
      sample.slot??-1,sample.slotBefore??'0'.repeat(240),sample.activeSource??-1,sample.inactiveSourceFlags,1].join(' '));
    checks.push(line=>{
      const fields=line.split(' '),values=fields.slice(0,11).map(Number);
      assert.equal(values[2],sample.outFlagAfter);
      assert.equal(values[3],Number(!sample.sameTime&&!sample.nullOut));
      assert.equal(values[1],sample.stop.reason==='returned'?sample.damage:0);
      if(sample.stop.reason==='returned')assert.deepEqual(values.slice(4,7),[sample.sameTime?0:256,sample.sameTime?0:2000,-1]);
      else if(sample.activeSource!==null){
        assert.equal(values[0],4);assert.equal(values[4],256);assert.equal(values[5],sample.activeSource+1);
        assert.deepEqual(values.slice(7),[6,0x43fd53,0,sample.activeSource]);
      }else{
        assert.equal(values[0],2);assert.equal(values[6],sample.slot);assert.equal(values[4],sample.slot+1);
        assert.equal(fields[13],sample.slotAfter);assert.equal(Number(fields[11]),5);
      }
    });
  }
  for(const sample of report.accountingCases){
    input.push(['H',1,sample.damage,sample.outFlag,sample.playerState,sample.bypass,sample.enemyFlags,sample.protection,
      ...sample.lifeBefore,...sample.statsBefore,1,sample.spellFlags].join(' '));
    checks.push(line=>{
      const values=line.split(' ').map(Number);
      assert.equal(values[0],sample.bypass!==0?1:sample.playerScaledDamage===0?2:4);
      assert.equal(values[1],sample.playerScaledDamage);assert.equal(values[2],sample.hpDamage);
      assert.deepEqual(values.slice(9,15),sample.lifeAfter);assert.deepEqual(values.slice(15,17),sample.statsAfter);
      if(values[0]===4)assert.deepEqual(values.slice(6,9),[8,0x41665a,0x41dea0]);
    });
  }
  for(const sample of report.hitTailCases){
    assert.equal(sample.stop.address,'0x0043ed48');
    assert.deepEqual(sample.animationPositionBits,[
      Buffer.from(sample.slotAfter,'hex').readFloatLE(0x14)+320,
      Buffer.from(sample.slotAfter,'hex').readFloatLE(0x18)+16,.1].map(value=>{const bytes=Buffer.alloc(4);bytes.writeFloatLE(value);return bytes.readUInt32LE();}));
    assert.equal(sample.animationRotationBits,sample.rotationBits);assert.equal(sample.animationFlags,4);
    input.push(`T ${sample.slotBefore} ${sample.replacementHandle}`);
    checks.push(line=>assert.equal(line,`${sample.slotAfter} 13`));
  }
  for(const sample of report.impactCases){
    input.push(`J ${sample.value} ${sample.flag}`);
    checks.push(line=>assert.equal(line,`${sample.valueAfter} ${sample.flagAfter} ${sample.stop.reason==='unsupported_call'?1:0}`));
  }
  const negative=JSON.parse(fs.readFileSync(path.join(root,'local/research/negative-motivation-fixtures.json')));
  for(const sample of negative.cases){input.push(`J ${sample.value} ${sample.flag}`);checks.push(line=>assert.equal(line,`${sample.valueAfter} ${sample.flagAfter} ${sample.stop.reason==='unsupported_call'?1:0}`));}
  const compiler=path.resolve(root,'../../work/tools/emsdk-package/emsdk-main/upstream/emscripten/emcc.py');
  const sdk=path.resolve(path.dirname(compiler),'../..'),python=path.join(sdk,'python/3.13.3_64bit/python.exe');
  const env={...process.env,EM_CONFIG:path.join(sdk,'.emscripten').replaceAll('\\','/'),
    EM_CACHE:path.resolve(root,'../../work/emscripten-cache-4.0.22').replaceAll('\\','/'),EMSDK_PYTHON:python,EMCC_CORES:'2',BINARYEN_CORES:'2'};
  fs.mkdirSync(path.join(root,'artifacts/tests'),{recursive:true});
  for(const sanitized of [false,true]){
    const output=path.join(root,`artifacts/tests/shot-enemy-collision${sanitized?'-ubsan':''}.cjs`);
    const build=spawnSync(python,[compiler,'tests/native/shot-enemy-collision.cpp','src/game/ShotEnemyCollisionRules.cpp',
      '-Isrc','-std=c++20','-O1','-Wall','-Wextra','-Werror','-fno-exceptions','-fno-fast-math','-ffp-contract=off',
      ...(sanitized?['-fsanitize=undefined','-fno-sanitize-recover=all']:[]),'-sENVIRONMENT=node','-sEXIT_RUNTIME=1','-o',output],
      {cwd:root,env,encoding:'utf8',timeout:180000});
    assert.equal(build.status,0,build.stdout+build.stderr);
    const run=spawnSync(process.execPath,[output],{cwd:root,input:input.join('\n')+'\n',encoding:'utf8',timeout:30000});
    assert.equal(run.status,0,run.stdout+run.stderr);
    const lines=run.stdout.trim().split(/\r?\n/);assert.equal(lines.length,checks.length);
    checks.forEach((check,index)=>{try{check(lines[index]);}catch(error){error.message=`case ${index}: ${error.message}`;throw error;}});
  }
});
