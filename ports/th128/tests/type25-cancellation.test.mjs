import assert from 'node:assert/strict';
import test from 'node:test';
import fs from 'node:fs';
import path from 'node:path';
import {createHash} from 'node:crypto';
import {spawnSync} from 'node:child_process';
import {fileURLToPath} from 'node:url';

const root=path.resolve(path.dirname(fileURLToPath(import.meta.url)),'..');
const compiler=path.resolve(root,'../../work/tools/emsdk-package/emsdk-main/upstream/emscripten/emcc.py');
const sdk=path.resolve(path.dirname(compiler),'../..');
const python=path.join(sdk,'python/3.13.3_64bit/python.exe');
const output=path.join(root,'artifacts/tests/type25-cancellation.cjs');
const digest=bytes=>createHash('sha256').update(bytes).digest('hex');

test('type25 cancellation spawns real 371/370 family with strict rejection, RNG, drift and cleanup',
  {skip:!fs.existsSync(compiler)},()=>{
    const catalog=JSON.parse(fs.readFileSync(path.join(root,'local/catalog.json'),'utf8'));
    const variant=path.dirname(path.resolve(root,'local',catalog.variants.ja.manifest));
    const bulletPath=path.join(variant,'raw/bullet.anm');
    const raw=fs.readFileSync(bulletPath);
    const fixturePath=path.join(root,'local/research/type25-cancellation-fixtures.json');
    const fixture=JSON.parse(fs.readFileSync(fixturePath,'utf8'));
    assert.equal(fixture.schema,'th128-type25-cancellation-fixtures/1');
    assert.equal(fixture.bulletAnm.sha256,digest(raw));
    assert.equal(fixture.metadata.sha256,
      digest(fs.readFileSync(path.join(variant,fixture.metadata.path))));
    assert.deepEqual(fixture.profile,{type:25,script:102,secondaryScript:103,selector:8,
      cancellationScript:371,freezePolicy:1});
    assert.deepEqual(fixture.links,[{parent:371,child:370},{parent:371,child:370},
      {parent:371,child:370}]);
    assert.deepEqual(fixture.siblingRoots,{identicalInstructionFlow:[375,381],
      childTargets:{371:370,375:374,381:380},childSignaturesDiffer:true});
    assert.deepEqual(fixture.programs['371'].children,[370,370,370]);
    assert.deepEqual(fixture.programs['370'].children,[]);
    assert.deepEqual(fixture.programs['371'].rngReads.map(read=>read.builtin),[10022,10022,10022]);
    assert.equal(fixture.programs['370'].rngReads.length,7);
    assert.equal(fixture.programs['371'].rngReads.length+3*fixture.programs['370'].rngReads.length,24);
    assert.deepEqual(fixture.sprites.map(sprite=>sprite.index),
      [580,581,582,583,584,585,586,587]);
    for(const sprite of fixture.sprites){
      assert.equal(sprite.entry,7);
      assert.equal(sprite.width,32);
      assert.equal(sprite.height,32);
      assert.equal(sprite.texture,'textures/bullet-007.png');
    }
    assert.deepEqual(fixture.programs['370'].layers,[15]);
    assert.deepEqual(fixture.programs['370'].sprites,
      [580,581,582,583,584,585,586,587]);
    assert.ok(fixture.programs['371'].commands.some(command=>command.op===26));
    assert.ok(fixture.programs['371'].commands.some(command=>command.op===75&&command.words[0]===40));
    assert.ok(fixture.programs['370'].commands.some(command=>command.op===1&&command.time===40));
    for(const program of Object.values(fixture.programs))
      for(const command of program.commands)
        assert.ok(![4,5].includes(command.op)&&!(command.op>=28&&command.op<=39),
          `unproven loop opcode ${command.op}`);
    assert.deepEqual(fixture.childScaleCommands,[
      {command:6,mask:12,duration:20,mode:4,targetVariables:[10004,10004]},
      {command:27,mask:12,duration:20,mode:4,targetVariables:[10004,10004]}]);
    assert.deepEqual(fixture.lifecycle,{root:371,child:370,children:3,maxReservation:4,
      rootWaitFrames:40,childEndFrame:40,
      zeroRng:{rootClockAfterBind:-39,childClockAfterBind:1,birthRngCalls:21,
        totalRngCalls:24,retirePass:40},
      rootSpawns:'three op88 child370 with op26/op75 random delays',
      rootTerminates:'op75 40 then op1',childTerminates:'op1 at time 40'});
    assert.deepEqual(fixture.unsupportedSiblingRoots,[369,373,377,379]);

    fs.mkdirSync(path.dirname(output),{recursive:true});
    const environment={...process.env,EM_CONFIG:path.join(sdk,'.emscripten').replaceAll('\\','/'),
      EM_CACHE:path.resolve(root,'../../work/emscripten-cache-4.0.22').replaceAll('\\','/'),
      EMSDK_PYTHON:python,PYTHONUTF8:'1',EMCC_CORES:'2',BINARYEN_CORES:'2'};
    const build=spawnSync(python,[compiler,'tests/native/type25-cancellation.cpp',
      'src/game/CancellationEffects.cpp','src/animation/AnmLogic.cpp','src/animation/AnmScene.cpp',
      '-Isrc','-std=c++20','-O1','-Wall','-Wextra','-Werror','-fno-fast-math','-ffp-contract=off',
      '-fsanitize=undefined','-fno-sanitize-recover=all','-sENVIRONMENT=node','-sEXIT_RUNTIME=1',
      '-sNODERAWFS=1','-sALLOW_MEMORY_GROWTH=1','-sINITIAL_MEMORY=67108864','-o',output],
    {cwd:root,encoding:'utf8',env:environment,timeout:180000});
    assert.equal(build.status,0,build.stdout+build.stderr);
    const run=spawnSync(process.execPath,[output,bulletPath],{cwd:root,encoding:'utf8',timeout:60000});
    assert.equal(run.status,0,run.stdout+run.stderr);
    assert.match(run.stdout,/type25 cancellation 371\/370 actual \+ strict synthetic cases passed/);
  });
