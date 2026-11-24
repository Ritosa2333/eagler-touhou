import assert from 'node:assert/strict';
import fs from 'node:fs';
import path from 'node:path';
import {fileURLToPath} from 'node:url';
import {createHash} from 'node:crypto';

const root=path.resolve(path.dirname(fileURLToPath(import.meta.url)),'..'),work=path.resolve(root,'../../work');
const read=relative=>fs.readFileSync(path.join(root,relative),'utf8');
const json=relative=>JSON.parse(read(relative));
const hash=name=>createHash('sha256').update(fs.readFileSync(name)).digest('hex');
const build=json('site/runtime/build.json');
for(const [name,digest] of Object.entries(build.sourceFiles))assert.equal(hash(path.join(root,name)),digest,`runtime source mismatch: ${name}`);
for(const [name,artifact] of Object.entries(build.artifacts))assert.equal(hash(path.join(root,'site',path.dirname(build.entry),name)),artifact.sha256);
const nodeLog=fs.readFileSync(path.join(work,'all-routes-node-final.log'),'utf8');
const number=label=>Number([...nodeLog.matchAll(new RegExp(`(?:^|\\n)[^\\n]*?\\b${label} (\\d+)(?=\\r?\\n|$)`,'g'))].at(-1)?.[1]);
const node={tests:number('tests'),passed:number('pass'),failed:number('fail'),skipped:number('skipped')};
assert.equal(node.tests,188);assert.equal(node.passed,node.tests);assert.equal(node.failed,0);assert.equal(node.skipped,0);
const pythonLog=fs.readFileSync(path.join(work,'all-routes-python-delivery.log'),'utf8');
const python={tests:Number(pythonLog.match(/Ran (\d+) tests/)?.[1]),allPassed:/\r?\nOK\r?\n?$/.test(pythonLog),skipped:/\.\.\. skipped /.test(pythonLog)};
assert.equal(python.tests,372);assert.equal(python.allPassed,true);assert.equal(python.skipped,false);
const visualLog=fs.readFileSync(path.join(work,'all-routes-visual.log'),'utf8');assert.match(visualLog,/Ran 3 tests/);assert.match(visualLog,/\r?\nOK\r?\n?$/);
const route=json('docs/route-gameplay-browser.json');assert.deepEqual(route.errors,[]);
assert.equal(route.stopped.frame,8050);assert.equal(route.stopped.spawns,141);assert.equal(route.stopped.enemiesDefeated,132);assert.equal(route.stopped.deathsCommitted,2);
assert.match(route.error,/sub=main offset=644 opcode=418/);
const player=json('docs/player-render-order-browser.json');
for(const result of player.results){
  assert.deepEqual(result.errors,[]);assert.deepEqual(result.failed,[]);
  const outline=result.full.draws.find(draw=>draw.id===4000001),body=result.full.draws.find(draw=>draw.id===4000000);
  assert.equal(outline.sprite,body.sprite+79);assert.equal(outline.color,0xc0ffff00);assert.equal(outline.priority,25);
  assert.ok(result.field.draws.some(draw=>draw.drawPass==='hostile-bullets'&&draw.priority===32));
}
const logFiles=['checkpoint-preview.log','checkpoint-combat-stability.log','checkpoint-player-render.log','all-routes-resolution.log',
  'all-routes-freeze-propagation.log','checkpoint-z-lifecycle.log','checkpoint-framebuffer.log','checkpoint-stage-background.log',
  'checkpoint-recovery-rerun.log','checkpoint-loading.log','route-gameplay-third.log'];
const browserEvidence=[];
for(const name of logFiles){
  const text=fs.readFileSync(path.join(work,name),'utf8');assert.ok(text.trim(),name);assert.doesNotMatch(text,/AssertionError|TimeoutError|TypeError|ReferenceError|^FAIL /m,name);
  browserEvidence.push({log:name,sha256:hash(path.join(work,name))});
}
assert.match(fs.readFileSync(path.join(work,'checkpoint-recovery-rerun.log'),'utf8'),/fresh-stage restart passed/);
const framebuffer=JSON.parse(fs.readFileSync(path.join(work,'checkpoint-framebuffer.log'),'utf8').trim());
for(const sample of framebuffer){assert.equal(sample.runtime,build.generation);assert.equal(sample.differences,0);assert.equal(sample.maximum,0);assert.equal(sample.gpuError,0);}
const assets=json('docs/all-routes-assets-checkpoint.json');
for(const record of Object.values(assets.variants)){assert.equal(record.originalUnchanged,true);assert.equal(record.rawHashesVerified,199);assert.equal(record.pngHashesVerified,305);}
const messages=json('docs/stage-message-inventory.json');assert.equal(messages.sources.length,32);assert.equal(messages.dialogueGameplayImplemented,false);
assert.match(fs.readFileSync(path.join(work,'stage-messages-independent.log'),'utf8'),/Ran 4 tests/);
const report={schema:'th128-all-routes-development-checkpoint/1',createdAt:new Date().toISOString(),runtimeGeneration:build.generation,
  verifiedSourceFiles:Object.keys(build.sourceFiles).length,node,python,visualProbeTests:3,browserEvidence,framebuffer,
  progress:{stage:'st_a1_1.ecl',difficulty:1,seed:34772,frame:8050,scriptBirths:141,defeated:132,deaths:2,
    midbossPhasesExecuted:['MBoss1','MBoss2','MBoss3','MBossDead'],nextBoundary:route.error,
    endBossCompleted:false,firstStageCompleted:false,allRoutesCompleted:false},
  render:{hostilePriority:32,playerPriority:25,frozenBulletPriority:24,outlineSpriteOffset:79,fullPowerColor:0xc0ffff00,
    chargeRingPriority:27,iceReleaseBodyBelowPlayer:true,desktopAndNarrowVerified:true},
  messages:{decodedResources:32,independentThmsgTextMatch:true,executionImplemented:false},
  resources:{report:'all-routes-assets-checkpoint.json',originalUnchanged:true},
  limitations:['MSG418/419 portraits, text, audio, waiting and scene callbacks are not yet implemented.',
    'No end-Boss fight or A/B/C three-stage chain is accepted as complete.',
    'Active spell-card finalization, lasers, GameOver continuation and some callbacks remain explicit unsupported paths.',
    'No full native GPU frame-equivalence claim.'],
  verificationNote:'An earlier concurrent runtime-recovery launch timed out loading assets; the isolated rerun passed. Historical logs are not substituted for the named fresh checkpoint logs.'};
const serialized=JSON.stringify(report,null,2)+'\n';
fs.writeFileSync(path.join(root,'docs/all-routes-checkpoint-verification.json'),serialized);
fs.writeFileSync(path.join(root,'docs/main-thread-verification.json'),serialized);
console.log(JSON.stringify({runtime:build.generation,sources:report.verifiedSourceFiles,node,python,visualProbes:3,browserScripts:browserEvidence.length,frame:8050,allStagesComplete:false}));
