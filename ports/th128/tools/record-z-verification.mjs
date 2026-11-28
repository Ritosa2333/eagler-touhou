import assert from 'node:assert/strict';
import fs from 'node:fs';
import path from 'node:path';
import {createHash} from 'node:crypto';
import {fileURLToPath} from 'node:url';
const root=path.resolve(path.dirname(fileURLToPath(import.meta.url)),'..'),work=path.resolve(root,'../../work');
const json=relative=>JSON.parse(fs.readFileSync(path.join(root,relative),'utf8'));
const log=name=>fs.readFileSync(path.join(work,name),'utf8');
const hash=relative=>createHash('sha256').update(fs.readFileSync(path.join(root,relative))).digest('hex');
const build=json('site/runtime/build.json'),mismatches=[];
for(const [name,expected] of Object.entries(build.sourceFiles))if(hash(name)!==expected)mismatches.push(name);
assert.deepEqual(mismatches,[],'published WASM sources must match actual delivery');
for(const [name,record] of Object.entries(build.artifacts))assert.equal(hash(path.join('site',path.dirname(build.entry),name)),record.sha256);
const nodeLog=log('z-verified-node-tests.log');
const finalNumber=(text,label)=>Number([...text.matchAll(new RegExp(`(?:^|\\n)[^\\n]*?\\b${label} (\\d+)(?=\\r?\\n|$)`,'g'))].at(-1)?.[1]);
const node={tests:finalNumber(nodeLog,'tests'),passed:finalNumber(nodeLog,'pass'),failed:finalNumber(nodeLog,'fail'),skipped:finalNumber(nodeLog,'skipped')};
assert.ok(node.tests>=173&&node.passed===node.tests&&node.failed===0&&node.skipped===0,'full Node/C++/WASM suite must finish green');
const pythonLog=log('z-final-python-tests.log'),pythonCount=Number(pythonLog.match(/Ran (\d+) tests/)?.[1]);
assert.ok(pythonCount>=349&&/\r?\nOK\r?\n?$/.test(pythonLog)&&!pythonLog.includes('... skipped '));
const visualLog=log('z-visual-probe-tests.log');assert.match(visualLog,/Ran 3 tests/);assert.match(visualLog,/\r?\nOK\r?\n?$/);
const browserLog=log('z-verified-browser.log'),browserScripts=['z-freeze-experience.cjs','z-freeze-propagation.cjs','stage-experience.cjs',
 'enemy312-smoke.cjs','preview-smoke.cjs','player-shot-smoke.cjs','combat-smoke.cjs','emission-audio-race.cjs','loading-regressions.cjs'];
for(const name of browserScripts)assert.ok(browserLog.includes(`PASS ${name}`));assert.ok(!browserLog.includes('FAIL '));
assert.match(log('z-stage-lifecycle-browser.log'),/Japanese\/Chinese Z release, music, reset\/restart, title return and live resource switch passed/);
const experience=json('docs/z-freeze-browser.json'),propagation=json('docs/z-freeze-propagation-browser.json');
assert.deepEqual(experience.results.map(result=>result.width),[1280,390]);assert.deepEqual(propagation.errors,[]);assert.deepEqual(propagation.failed,[]);
assert.equal(propagation.glError,0);assert.ok(propagation.complete.snapshot.ice.converted>0&&propagation.complete.snapshot.ice.completions===1);
const runtimeProof=json('docs/z-fire-runtime.json');assert.equal(runtimeProof.generation,build.generation);
const lifecycle=json('docs/z-stage-lifecycle-browser.json');assert.deepEqual(lifecycle.errors,[]);assert.deepEqual(lifecycle.failed,[]);
const assets=json('docs/z-assets-verification.json');for(const variant of Object.values(assets.variants))assert.equal(variant.originalUnchanged,true);
const bosses=json('docs/enemy312-browser-smoke.json');for(const result of bosses.results)assert.equal(result.runtime,build.generation);
const fingerprints=Object.fromEntries(['site/play.mjs','site/play.html','site/runtime/sound.mjs','site/runtime/renderer.mjs','site/runtime/background.mjs',
 'tools/th128_assets/ice_masks.py','tools/th128_assets/pipeline.py'].map(name=>[name,hash(name)]));
const report={date:'2026-10-30',execution:'main-thread-only',subagentDelegationPermitted:false,checkpoint:'z-freeze-stage-experience',
 verificationStatus:'bounded-checkpoint-verified',runtimeGeneration:build.generation,sourceFilesCheckedAgainstBuild:Object.keys(build.sourceFiles).length,
 sourceMismatches:mismatches,artifactFingerprints:fingerprints,nodeCppWasm:node,
 python:{tests:pythonCount,passed:pythonCount,failed:0,skipped:0},visualProbePython:{tests:3,passed:3,failed:0,skipped:0},
 browser:{scripts:[...browserScripts,'z-stage-lifecycle.cjs'],passed:10,desktopAnd390px:true,originalStageMusic:'th128_00.wav',
  headGlyphs:[258,259,260,261,262,263,264,265,266,267,269],freezeReleaseHoldFrames:45,freezeReleaseMinimumRaw:3000,
  actualStagePropagation:{frame:propagation.frozen.snapshot.frame,...propagation.complete.snapshot.ice,areaPercent:propagation.complete.snapshot.lastFrozenArea,bonus:propagation.complete.snapshot.freezeBonus},
  headlessConsoleErrors:0,headlessHttpErrors:0,webGlError:0,resourceLoadingFailureAndTimeoutCases:7},
 originalAndPreparedAssets:assets,stageBoundary:bosses.results.map(({width,stopped})=>({width,frame:stopped.snapshot.frame,error:stopped.error})),
 fullGameComplete:false,originalExecutableLaunched:false,originalFilesWritten:false,
 residualScope:['Full original initialization and random seed identity are not yet verified.',
  'Area score/reward numbers are implemented; original sliding area-result panel remains pending.',
  'Shot-level increase and motivation-notice callbacks still stop explicitly instead of being skipped.',
  'X Perfect Freeze activation, laser freezing, later Boss524/type4 and all-route progression are incomplete.',
  'STD/ANM camera motion, perspective/depth/fog and nonblank pixels are tested; no original GPU whole-frame equivalence is claimed.',
  'Chinese resource variant currently uses the pinned Japanese executable ice bitmaps; Chinese executable table equivalence is unverified.'],
 evidence:['z-freeze-browser.json','z-freeze-propagation-browser.json','z-stage-lifecycle-browser.json','z-fire-runtime.json',
  'stage-experience-browser.json','preview8128-browser-smoke.json','loading-browser-regressions.json','enemy312-browser-smoke.json','z-assets-verification.json']};
for(const file of ['z-freeze-verification.json','main-thread-verification.json'])fs.writeFileSync(path.join(root,'docs',file),JSON.stringify(report,null,2)+'\n');
console.log(JSON.stringify({generation:report.runtimeGeneration,sourceFiles:report.sourceFilesCheckedAgainstBuild,node,python:pythonCount,browserScripts:10,freeze:report.browser.actualStagePropagation}));
