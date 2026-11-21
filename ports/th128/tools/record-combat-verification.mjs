import assert from 'node:assert/strict';import fs from 'node:fs';import path from 'node:path';import {createHash} from 'node:crypto';import {fileURLToPath} from 'node:url';
const root=path.resolve(path.dirname(fileURLToPath(import.meta.url)),'..'),work=path.resolve(root,'../../work');
const json=relative=>JSON.parse(fs.readFileSync(path.join(root,relative),'utf8')),log=name=>fs.readFileSync(path.join(work,name),'utf8');
const hash=relative=>createHash('sha256').update(fs.readFileSync(path.join(root,relative))).digest('hex');
const build=json('site/runtime/build.json'),mismatches=[];for(const [name,digest] of Object.entries(build.sourceFiles))if(hash(name)!==digest)mismatches.push(name);assert.deepEqual(mismatches,[]);
for(const [name,record] of Object.entries(build.artifacts))assert.equal(hash(path.join('site',path.dirname(build.entry),name)),record.sha256);
const nodeLog=log('combat-stage-node.log'),number=label=>Number([...nodeLog.matchAll(new RegExp(`(?:^|\\n)[^\\n]*?\\b${label} (\\d+)(?=\\r?\\n|$)`,'g'))].at(-1)?.[1]);
const node={tests:number('tests'),passed:number('pass'),failed:number('fail'),skipped:number('skipped')};assert.ok(node.tests>=180&&node.passed===node.tests&&node.failed===0&&node.skipped===0);
const pythonLog=log('combat-stage-python.log'),pythonCount=Number(pythonLog.match(/Ran (\d+) tests/)?.[1]);assert.ok(pythonCount>=354&&/\r?\nOK\r?\n?$/.test(pythonLog)&&!pythonLog.includes('... skipped '));
const visualLog=log('combat-stage-visual-probes.log');assert.match(visualLog,/Ran 3 tests/);assert.match(visualLog,/\r?\nOK\r?\n?$/);
const scripts=['resolution-focus.cjs','runtime-recovery.cjs','boss-ring-smoke.cjs','z-freeze-experience.cjs','z-freeze-propagation.cjs',
 'z-stage-lifecycle.cjs','stage-experience.cjs','preview-smoke.cjs','geometry526-framebuffer.cjs','loading-regressions.cjs'];
const browserLogs=log('combat-delivery-browser.log')+log('combat-final-browser.log');for(const script of scripts)assert.ok(browserLogs.includes(`PASS ${script}`));assert.ok(!browserLogs.includes('FAIL '));
const combat=json('docs/combat-stability-browser.json');assert.deepEqual(combat.results.map(result=>result.mode),['c','z-taps','c-up-left']);
for(const result of combat.results){assert.deepEqual(result.errors,[]);let previous=-1;for(const sample of result.trace){
 assert.equal(sample.world.error,'');assert.equal(sample.world.drawError,'');assert.equal(sample.runtime.paused,false);assert.ok(sample.world.snapshot.frame>previous);previous=sample.world.snapshot.frame;
 assert.deepEqual(sample.runtime.renderOutput,{width:1280,height:960,logicalWidth:640,logicalHeight:480});
}assert.ok(result.trace.at(-1).world.snapshot.enemiesDefeated>0);}
const resolution=json('docs/resolution-focus-browser.json');assert.deepEqual(resolution.results.map(result=>result.width),[1440,390]);
for(const result of resolution.results){assert.equal(result.glError,0);assert.deepEqual(result.errors,[]);assert.deepEqual(result.failed,[]);
 assert.equal(result.marker.length,2);assert.ok(result.marker.every(draw=>draw.sprite===444&&draw.layer===16));
 assert.ok(result.captureSizes.some(size=>size.join(',')==='1280,960')&&result.captureSizes.some(size=>size.join(',')==='640,480'));}
const framebuffer=json('docs/geometry526-framebuffer-1280-browser.json');for(const result of framebuffer.results){
 assert.equal(result.runtime,build.generation);assert.equal(result.renderWidth,1280);assert.equal(result.renderHeight,960);assert.equal(result.maximum,0);assert.equal(result.differences,0);assert.equal(result.gpuError,0);
}
const bosses=json('docs/boss-ring-browser.json');for(const result of bosses.results){assert.ok(result.maxTyped4>0);assert.equal(result.stopped.snapshot.frame,3373);
 assert.equal(result.stopped.error,'enemy ANM child scheduling is unproved for this topology');assert.deepEqual(result.errors,[]);assert.deepEqual(result.failed,[]);}
const assets=json('docs/combat-stage-assets-verification.json');for(const result of Object.values(assets.variants))assert.equal(result.originalUnchanged,true);
assert.equal(json('docs/z-fire-runtime.json').generation,build.generation);assert.deepEqual(json('docs/runtime-recovery-browser.json').errors,[]);
const report={date:'2026-10-30',execution:'main-thread-only',subagentDelegationPermitted:false,checkpoint:'combat-resolution-focus-boss-ring',verificationStatus:'bounded-checkpoint-verified',
 runtimeGeneration:build.generation,sourceFilesCheckedAgainstBuild:Object.keys(build.sourceFiles).length,sourceMismatches:mismatches,
 artifactFingerprints:Object.fromEntries(['site/play.mjs','site/play.html','site/play.css','site/runtime/renderer.mjs','site/runtime/timing.mjs','tools/th128_assets/bullet_profiles.py'].map(name=>[name,hash(name)])),
 nodeCppWasm:node,python:{tests:pythonCount,passed:pythonCount,failed:0,skipped:0},visualProbePython:{tests:3,passed:3,failed:0,skipped:0},
 fixedCombat:{nativeShotMotionCases:22,rootCause:'old exponent-separation guard rejected original tiny lateral velocity after hit-speed change24to3 at nonzeroX',
  additionalCause:'83ms presentation spike exceeded oldfour-step pause rule',timing:'at most4 unit ticks per presentation; retain remaining backlog; gaps above250ms still visibly pause',
  liveModes:combat.results.map(result=>({mode:result.mode,frame:result.trace.at(-1).world.snapshot.frame,enemiesDefeated:result.trace.at(-1).world.snapshot.enemiesDefeated})),
  recovery:'in-canvas pause/fault reason, resume when appropriate and in-page restart proven; no page reload needed'},
 resolution:{defaultRenderOutput:[1280,960],logicalCoordinates:[640,480],selectable:[[640,480],[1280,960]],sourceTexturesUpscaled:false,
  note:'GPU renders to requested output; source art remains original resolution, display downscales responsively if browser is smaller.',
  nativeFramebufferMetadata:[640,480],physicalCapture:[1280,960],identityStripRoundtrip:{maximumChannelDifference:0,differingChannels:0},manualFullscreenInteractionVerified:false},
 focusIndicator:{bank:0,rootScript:80,childScript:79,sprite:444,factoryLayer:12,authoredLayer:16,nativePixelOffset:[320,16],changesCollisionRadius:false,
  focusGate:'original manager/counter/Shift or Z-held>=12 rule; normal moving state only',retirement:'actual IRQ1 children, not synthesized marker'},
 stageProgress:{previousBoundary:{frame:3292,sub:'MBoss1At2',offset:296,opcode:524},
  implemented:['original524 emitter spawn radius actually consumed in radial bullet birth','actual typed4 profile/script39 and startup interrupt',
   'ETEX1 burst unit lifecycle through real animation/bullet ownership','original448 difficulty-ranked ECL-context clock subtraction'],
  currentBoundary:bosses.results.map(result=>({width:result.width,frame:result.stopped.snapshot.frame,error:result.stopped.error,maxTyped4:result.maxTyped4})),
  next:'original bullet123 attached children121/122 and custom procedural geometry, then later Boss phase flow'},
 browser:{scripts:[...scripts,'combat-stability.cjs'],passed:11,extraPhysicalFramebuffer2xRun:true,loadingFailureAndTimeoutCases:7},
 originalAndPreparedAssets:assets,fullGameComplete:false,originalExecutableLaunched:false,originalFilesWritten:false,
 residualScope:['Boss attack effects/phase continuation are still incomplete; first-stage baseline stops visibly at3373.',
  'Area-result panel, shot-level/motivation notice callbacks, X Perfect Freeze, lasers and all-route progression remain incomplete.',
  'Original Windows GPU whole-frame comparison and full initialization equivalence are not claimed.',
  '1280x960 output does not add new high-resolution source textures; fullscreen still depends on user/browser permission.'],
 evidence:['combat-live-repro.json','combat-stability-browser.json','resolution-focus-browser.json','runtime-recovery-browser.json','boss-ring-browser.json',
  'geometry526-framebuffer-1280-browser.json','z-freeze-propagation-browser.json','z-stage-lifecycle-browser.json','combat-stage-assets-verification.json']};
for(const name of ['combat-focus-stage-verification.json','main-thread-verification.json'])fs.writeFileSync(path.join(root,'docs',name),JSON.stringify(report,null,2)+'\n');
console.log(JSON.stringify({generation:build.generation,sourceFiles:report.sourceFilesCheckedAgainstBuild,node,python:pythonCount,browser:report.browser.passed,currentBoundary:report.stageProgress.currentBoundary}));
