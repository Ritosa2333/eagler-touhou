import test from 'node:test';
import assert from 'node:assert/strict';
import {spawnSync} from 'node:child_process';
import {mkdtemp,readFile,rm} from 'node:fs/promises';
import {dirname,join,resolve} from 'node:path';
import {tmpdir} from 'node:os';
import {PRODUCT_GAMES} from '../../common/contracts/product-catalog.mjs';
import {WORKSPACE_REPOSITORIES} from '../../launcher/lib/workspace-layout.mjs';
import {classifyTouchSourceOwner,productTouchMatrix,requireTouchSourceCoverage,runTouchNativeInvariant,sourceOwners} from './touch-native-invariants.mjs';

const root=resolve(import.meta.dirname,'../..');
const cli=resolve(root,'tools/ports/touch-native-invariants.mjs');

async function withOutput(action){
  const output=await mkdtemp(join(tmpdir(),'touch-native-cli-'));
  try{return await action(output);}
  finally{
    if(dirname(resolve(output))!==resolve(tmpdir()))throw new Error(`Unexpected CLI test directory: ${output}`);
    await rm(output,{recursive:true,force:true});
  }
}

test('the touch matrix follows every non-fighting product and its declared multiplayer variant',()=>{
  const matrix=productTouchMatrix();
  const supportedGames=Object.entries(PRODUCT_GAMES)
    .filter(([,product])=>product.inputProfile!=='fighting').map(([game])=>game).sort();
  assert.deepEqual([...new Set(matrix.map(item=>item.game))].sort(),supportedGames);
  for(const [game,product] of Object.entries(PRODUCT_GAMES)){
    const entries=matrix.filter(item=>item.game===game);
    if(product.inputProfile==='fighting'){
      assert.deepEqual(entries,[],`${game} fighting profile is excluded from STG touch movement`);
      continue;
    }
    assert.equal(entries.filter(item=>item.variant==='normal').length,1,`${game} normal variant`);
    assert.equal(entries.some(item=>item.variant==='multiplayer'),Boolean(product.multiplayerRuntime),`${game} multiplayer variant`);
  }
});

test('a newly cataloged source owner is enumerated and blocks until production components exist',()=>{
  const game='th99';
  const products={...PRODUCT_GAMES,[game]:{inputProfile:'stg',multiplayerRuntime:true}};
  const repositories={...WORKSPACE_REPOSITORIES,[`${game}mp`]:`ports/${game}-mp`};
  const matrix=productTouchMatrix(products,repositories);
  assert.deepEqual(matrix.filter(item=>item.game===game).map(({variant,sourceOwner})=>({variant,sourceOwner})),[
    {variant:'normal',sourceOwner:game},
    {variant:'multiplayer',sourceOwner:`${game}mp`},
  ]);
  const owners=sourceOwners(matrix).filter(owner=>owner.sourceOwner===game||owner.sourceOwner===`${game}mp`);
  const unresolved=owners.map(owner=>classifyTouchSourceOwner(owner,{controllerExists:false,motionExists:false}));
  assert.ok(unresolved.every(owner=>owner.status==='incomplete'));
  assert.throws(()=>requireTouchSourceCoverage(unresolved),/th99, th99mp/);
});

test('only complete th06/th07 legacy references may remain reference-only',()=>{
  for(const game of ['th06','th07']){
    const owner={sourceOwner:game,variants:['normal']};
    const legacy=classifyTouchSourceOwner(owner,{controllerExists:false,motionExists:false});
    assert.equal(legacy.status,'reference-only');
    assert.ok(legacy.reference.length>0);
    assert.equal(requireTouchSourceCoverage([legacy,{sourceOwner:'th09',status:'production-fixture'}]).length,2);
    const partial=classifyTouchSourceOwner(owner,{controllerExists:true,motionExists:false});
    assert.equal(partial.status,'incomplete',`${game} partial portable components must not pass by legacy reference`);
    assert.throws(()=>requireTouchSourceCoverage([partial]),new RegExp(game));
  }
});

test('CLI requires --output and rejects unknown options before validation',()=>{
  const missing=spawnSync(process.execPath,[cli],{cwd:root,encoding:'utf8',timeout:10000});
  assert.notEqual(missing.status,0);
  assert.match(missing.stderr,/Required: --output=report-directory/);
  const unknown=spawnSync(process.execPath,[cli,'--output=unused','--typo'],{cwd:root,encoding:'utf8',timeout:10000});
  assert.notEqual(unknown.status,0);
  assert.match(unknown.stderr,/Unknown option: --typo/);
});

test('the public fixture uses the runner-provided CXX without replacing it',async()=>withOutput(async output=>{
  const missingCompiler=join(output,process.platform==='win32'?'missing-cxx.exe':'missing-cxx');
  const child=spawnSync(process.execPath,[cli,`--output=${output}`],{cwd:root,encoding:'utf8',timeout:30000,
    env:{...process.env,CXX:missingCompiler},maxBuffer:1024*1024});
  assert.equal(child.error,undefined,child.error?.message);
  assert.notEqual(child.status,0,child.stdout);
  const report=JSON.parse(await readFile(resolve(output,'report.json'),'utf8'));
  assert.equal(report.status,'failed');
  assert.ok(report.failure.includes(missingCompiler),report.failure);
}));

test('failed compile or invariant validation is sealed as a failed component report',async()=>withOutput(async output=>{
  const report=await runTouchNativeInvariant({output,validate:async()=>{throw new Error('injected native validation failure');}});
  assert.equal(report.status,'failed');
  assert.equal(report.passed,false);
  assert.equal(report.componentStatus,'failed');
  assert.equal(report.completeRuntimeCoverage,false);
  assert.equal(report.runtimeCoverageStatus,'incomplete');
  assert.match(report.failure,/injected native validation failure/);
  const sealed=JSON.parse(await readFile(resolve(output,'report.json'),'utf8'));
  assert.equal(sealed.status,'failed');
  assert.equal(sealed.passed,false);
  assert.equal(sealed.componentStatus,'failed');
  assert.ok(Date.parse(sealed.finishedAt));
}));

test('successful component reports keep full Runtime coverage incomplete',async()=>withOutput(async output=>{
  const evidence={status:'production-components-verified-runtime-coverage-incomplete',productVariantCount:productTouchMatrix().length,
    nativeComponentOwners:[{sourceOwner:'fixture'}],mutationProofs:['fixture-mutation'],completeRuntimeCoverage:false};
  const report=await runTouchNativeInvariant({output,validate:async()=>evidence});
  assert.equal(report.status,'passed');
  assert.equal(report.passed,true);
  assert.equal(report.componentStatus,'passed');
  assert.equal(report.componentEvidenceStatus,evidence.status);
  assert.equal(report.completeRuntimeCoverage,false);
  assert.equal(report.runtimeCoverageStatus,'incomplete');
  const sealed=JSON.parse(await readFile(resolve(output,'report.json'),'utf8'));
  assert.equal(sealed.status,'passed');
  assert.equal(sealed.passed,true);
  assert.equal(sealed.componentStatus,'passed');
  assert.equal(sealed.completeRuntimeCoverage,false);
  assert.equal(sealed.runtimeCoverageStatus,'incomplete');
  assert.ok(Date.parse(sealed.finishedAt));
}));
