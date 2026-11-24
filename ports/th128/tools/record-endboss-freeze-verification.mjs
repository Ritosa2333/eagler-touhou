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
const nodeLog=log('endboss-mask-node.log');
const metric=name=>Number(nodeLog.match(new RegExp(`^ℹ ${name} (\\d+)`,'m'))?.[1]);
const node={tests:metric('tests'),passed:metric('pass'),failed:metric('fail'),skipped:metric('skipped')};
assert.ok(node.tests>=207);assert.equal(node.passed,node.tests);assert.equal(node.failed,0);assert.equal(node.skipped,0);
const pythonLog=log('endboss-mask-python-release.log');
const python={tests:Number(pythonLog.match(/Ran (\d+) tests/)?.[1]),passed:/\r?\nOK\r?\n?$/.test(pythonLog),skipped:/\.\.\. skipped /.test(pythonLog)};
assert.ok(python.tests>=374);assert.equal(python.passed,true);assert.equal(python.skipped,false);
const scripts=['boss-hud','geometry526-framebuffer','resolution-focus','player-render-order','runtime-recovery','z-freeze-experience','large-freeze','large-freeze-ja','gameplay'];
const browserEvidence=scripts.map(script=>{
  const name=`endboss-mask-${script}.log`,text=log(name);assert.ok(text.trim(),name);
  assert.doesNotMatch(text,/AssertionError|TimeoutError|TypeError|ReferenceError|^FAIL /m,name);
  return {script,logSha256:hash(path.join(work,name))};
});
for(const sample of JSON.parse(log('endboss-mask-geometry526-framebuffer.log').trim())){
  assert.equal(sample.runtime,build.generation);assert.equal(sample.differences,0);assert.equal(sample.maximum,0);assert.equal(sample.gpuError,0);
}
const freeze=read('docs/large-ball-freeze-browser.json');assert.deepEqual(freeze.errors,[]);
assert.ok(freeze.converted.large.length>=2);assert.ok(freeze.converted.large.every(bullet=>bullet.type===18&&bullet.policy===0&&bullet.sprite===371));
assert.ok(freeze.completed.converted>=39);assert.equal(freeze.completed.shattered,freeze.completed.converted);assert.equal(freeze.completed.completions,1);assert.ok(freeze.completed.pixels>1000);
const japaneseFreeze=read('docs/large-ball-freeze-browser-ja.json');assert.equal(japaneseFreeze.variant,'ja');assert.deepEqual(japaneseFreeze.errors,[]);
assert.ok(japaneseFreeze.converted.large.some(bullet=>bullet.type===18&&bullet.policy===0&&bullet.sprite===371));
assert.equal(japaneseFreeze.completed.shattered,japaneseFreeze.completed.converted);assert.ok(japaneseFreeze.completed.converted>=38);assert.equal(japaneseFreeze.completed.completions,1);
const route=read('docs/route-gameplay-browser.json');assert.deepEqual(route.errors,[]);assert.equal(route.error,'');
assert.deepEqual(route.bossHudCheck.resolution,[1280,960]);assert.ok(route.bossHudCheck.ringVertices.every(count=>count===76));
const phases=route.stopped.phaseTransitions.filter(phase=>phase.target.startsWith('Boss'));
assert.deepEqual(phases.map(phase=>phase.target),['BossCard1','Boss2','BossCard2','BossDead']);
assert.ok(phases.every(phase=>phase.timedOut));assert.equal(route.stopped.frame,19180);
const boss=route.stopped.entities.find(entity=>entity.id===phases.at(-1).entity);
assert.ok(boss);assert.equal(boss.active,false);assert.equal(boss.life,0);assert.equal(boss.animation0,-1);
assert.ok(!route.stopped.entities.some(entity=>entity.active&&(entity.flags&0x1000000)));
assert.ok(route.stopped.bossGauges.every(gauge=>gauge.boss===0));assert.equal(route.stopped.spell.active,false);assert.equal(route.stopped.spell.captured,false);
assert.equal(route.stopped.ice.converted,1016);assert.equal(route.stopped.ice.shattered,1016);assert.equal(route.stopped.ice.completions,50);
const eligibility=read('local/research/large-freeze-fixtures.json');assert.equal(eligibility.cases.length,24);assert.equal(eligibility.apiReturnsAssigned,false);
const mask=read('local/research/ice-mask-fixtures.json');assert.equal(mask.caseCount,108);assert.equal(mask.windowsGameProcessLaunched,false);assert.equal(mask.executable.unchanged,true);
const assets=read('docs/endboss-freeze-assets-verification.json');for(const variant of Object.values(assets.variants))assert.equal(variant.originalUnchanged,true);
const backup=JSON.parse(log('endboss-mask-backup-hash.json').replace(/^\uFEFF/,''));
assert.equal(backup.Hash.toLowerCase(),'8ac15bd416cd85f170249fd9d56c6450a6335d477af798e6c5573050ddd1223b');
const evidence=['large-ball-freeze-browser.json','large-ball-freeze-browser-ja.json','route-gameplay-browser.json','endboss-freeze-assets-verification.json','boss-hud-browser.json'];
const fingerprints=Object.fromEntries(['site/play.mjs','site/play.html','site/index.html','site/runtime/draw.mjs','site/runtime/renderer.mjs',
  'local/research/large-freeze-fixtures.json','local/research/ice-mask-fixtures.json',...evidence.map(name=>`docs/${name}`)]
  .map(file=>[file,hash(path.join(root,file))]));
const report={schema:'th128-first-stage-endboss-freeze/1',createdAt:new Date().toISOString(),runtimeGeneration:build.generation,
  verifiedSourceFiles:Object.keys(build.sourceFiles).length,node,python,browserEvidence,fingerprints,
  output:{width:1280,height:960,lowResolutionOptionRemoved:true,originalLogicalCoordinatesRetained:true},
  freeze:{nativeEligibilityCases:24,nativeMaskCases:108,actualLargeBallTest:freeze,japaneseLargeBallTest:japaneseFreeze,appearanceAndFreezePolicyAreIndependent:true,
    originalImmunityAndYoungBulletProtectionPreserved:true,originalBitmapsAndCloneAnimationsUsed:true,tinyCoordinatesNotClamped:true},
  progress:{stage:'st_a1_1.ecl',difficulty:1,seed:34772,frame:route.stopped.frame,scriptBirths:route.stopped.spawns,
    defeated:route.stopped.enemiesDefeated,deaths:route.stopped.deathsCommitted,endBossCompleted:true,endBossRetired:true,
    phaseTransitions:phases,completionMethod:'original-phase-timeouts-with-real-player-damage',spellCaptureClaimed:false,
    boss:{id:boss.id,life:boss.life,active:boss.active,animation0:boss.animation0},freeze:route.stopped.ice,
    cumulativeFrozenArea:route.stopped.cumulativeFrozenArea,firstStageSettlementCompleted:false,allRoutesCompleted:false},
  assets:{originalUnchanged:true,ja:assets.variants.ja,zh:assets.variants.zh},
  backup:{path:'D:/TH128-full-project-backup-20261030-212208.zip',sha256:backup.Hash.toLowerCase(),unchanged:true},
  limitations:['A/B/C full three-stage chains, post-Boss route selection and settlement are not complete.',
    'This run completed all four Boss phases through their original timeout transitions; it did not capture the spells.',
    'X Perfect Freeze activation, laser instances and the original sliding freeze-area result panel remain unfinished.',
    'Full native initialization and whole-frame GPU equivalence are not verified; dynamic text uses Canvas system fonts.',
    'The Chinese asset variant uses the pinned Japanese executable ice bitmaps; Chinese executable table equivalence is unverified.'],
  originalGameProcessLaunched:false,debugHpChangesUsed:false,invulnerabilityAdded:false,scriptPhasesSkipped:false,mainThreadOnly:true};
for(const file of ['docs/endboss-freeze-verification.json','docs/main-thread-verification.json'])
  fs.writeFileSync(path.join(root,file),JSON.stringify(report,null,2)+'\n');
console.log(JSON.stringify({runtime:build.generation,sources:report.verifiedSourceFiles,node,python,browserScripts:scripts.length,
  bossRetired:report.progress.endBossRetired,frame:report.progress.frame,frozenLargeBalls:freeze.converted.large.length,backupUnchanged:true}));
