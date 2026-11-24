import fs from 'node:fs';
import path from 'node:path';
import assert from 'node:assert/strict';
import {fileURLToPath} from 'node:url';
import {createHash} from 'node:crypto';

const root=path.resolve(path.dirname(fileURLToPath(import.meta.url)),'..');
const work=path.resolve(root,'../../work');
const read=file=>JSON.parse(fs.readFileSync(path.join(root,file),'utf8'));
const hash=file=>createHash('sha256').update(fs.readFileSync(file)).digest('hex');
const log=name=>fs.readFileSync(path.join(work,name),'utf8');
const build=read('site/runtime/build.json');
for(const [file,digest] of Object.entries(build.sourceFiles))assert.equal(hash(path.join(root,file)),digest,file);
for(const [file,record] of Object.entries(build.artifacts))assert.equal(hash(path.join(root,'site',path.dirname(build.entry),file)),record.sha256,file);
const nodeLog=log('boss-hud-release-node.log');
const metric=name=>Number(nodeLog.match(new RegExp(`^ℹ ${name} (\\d+)`,'m'))?.[1]);
const node={tests:metric('tests'),passed:metric('pass'),failed:metric('fail'),skipped:metric('skipped')};
assert.ok(node.tests>=196);assert.equal(node.passed,node.tests);assert.equal(node.failed,0);assert.equal(node.skipped,0);
const pythonLog=log('boss-hud-release-python.log');
const python={tests:Number(pythonLog.match(/Ran (\d+) tests/)?.[1]),passed:/\r?\nOK\r?\n?$/.test(pythonLog),skipped:/\.\.\. skipped /.test(pythonLog)};
assert.ok(python.tests>=374);assert.equal(python.passed,true);assert.equal(python.skipped,false);
const scripts=['boss-hud','geometry526-framebuffer','resolution-focus','player-render-order','runtime-recovery','z-freeze-experience'];
const browserEvidence=scripts.map(script=>{
  const name=`boss-hud-release-${script}.log`,text=log(name);assert.ok(text.trim(),name);
  assert.doesNotMatch(text,/AssertionError|TimeoutError|TypeError|ReferenceError|^FAIL /m,name);
  return {script,logSha256:hash(path.join(work,name))};
});
for(const sample of JSON.parse(log('boss-hud-release-geometry526-framebuffer.log').trim())){
  assert.equal(sample.runtime,build.generation);assert.equal(sample.differences,0);assert.equal(sample.maximum,0);assert.equal(sample.gpuError,0);
}
const browser=read('docs/boss-hud-browser.json');
for(const sample of browser.results){
  assert.deepEqual(sample.output.canvas,[1280,960]);assert.deepEqual(sample.output.buffer,[1280,960]);assert.equal(sample.output.selector,false);
  assert.equal(sample.output.error,0);assert.deepEqual(sample.errors,[]);assert.ok(sample.hud.timerVisible);
}
const route=read('docs/route-gameplay-browser.json');assert.deepEqual(route.errors,[]);
assert.ok(route.stopped.frame>8050&&route.stopped.spawns>=142);assert.ok(route.bossHudCheck?.hud?.timerVisible);
assert.deepEqual(route.bossHudCheck.resolution,[1280,960]);assert.ok(route.bossHudCheck.ringVertices.every(count=>count===76));
const timings=read('docs/boss-timer-inventory.json');assert.equal(timings.routes.length,16);assert.ok(timings.routes.every(route=>!route.gameplayComplete));
const report={schema:'th128-boss-hud-checkpoint/1',createdAt:new Date().toISOString(),runtimeGeneration:build.generation,
  verifiedSourceFiles:Object.keys(build.sourceFiles).length,node,python,browserEvidence,
  output:{width:1280,height:960,lowResolutionOptionRemoved:true,originalLogicalCoordinatesRetained:true},
  bossHud:{originalScripts:[81,162,163,164,165],ringVertexPairs:38,nativeCpuBuffersBitMatched:true,originalPhaseCountdown:true,
    pauseDoesNotAdvance:true,oldPhaseMarkersClearedOnOwnerRemoval:true,actualEndBoss:route.bossHudCheck},
  originalTimerInventory:{routes:16,authored414Records:timings.routes.reduce((total,route)=>total+route.phases.length,0),gameplayCompletionClaimed:false},
  progress:{stage:'st_a1_1.ecl',difficulty:1,seed:34772,frame:route.stopped.frame,scriptBirths:route.stopped.spawns,
    defeated:route.stopped.enemiesDefeated,deaths:route.stopped.deathsCommitted,nextBoundary:route.error,
    preBossMessageExecuted:true,endBossBattleEntered:true,endBossCompleted:false,firstStageCompleted:false,allRoutesCompleted:false},
  nativeEvidence:{fixture:'local/research/boss-hud-fixtures.json',sha256:hash(path.join(root,'local/research/boss-hud-fixtures.json'))},
  limitations:['A/B/C full three-stage chains and end-Boss completion are not verified.',
    'Active spell settlement, lasers, some stage callbacks and complete original initialization remain unfinished.',
    'No full native GPU-frame equivalence claim; dialogue text is not original GDI pixel rendering.'],
  originalGameProcessLaunched:false,mainThreadOnly:true};
const bytes=JSON.stringify(report,null,2)+'\n';
fs.writeFileSync(path.join(root,'docs/boss-hud-verification.json'),bytes);
fs.writeFileSync(path.join(root,'docs/main-thread-verification.json'),bytes);
console.log(JSON.stringify({runtime:build.generation,sources:report.verifiedSourceFiles,node,python,browserScripts:scripts.length+1,progress:report.progress}));
