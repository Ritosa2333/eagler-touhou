import test from 'node:test';
import assert from 'node:assert/strict';
import {createDiagnosticsRegistry,DIAGNOSTIC_LANE_CHECKS,PAIRED_PRESENTATION_CHECKS,pairedPresentationCapability,
  partitionDiagnosticLanes,resolveDiagnosticProfile} from './diagnostics.mjs';
import {PAIRED_CADENCE_DIFFERENCE_GROUPS} from './diagnostics.mjs';

test('diagnostic profile declares build provenance and snapshot-free Lab selection',()=>{
  const profile=resolveDiagnosticProfile('th08','normal');
  assert.ok(profile);
  assert.equal(profile.buildIdentitySchema,'th08/presentation-lab-build/1');
  assert.equal(profile.wasmFile,'th08-sdl.wasm');
  assert.equal(profile.runtimeProfile,'presentation-lab');
  assert.equal(profile.buildReceipt,'ports/th08/web/artifacts/presentation-lab/build.json');
  assert.equal(profile.sourceRoot,'ports/th08');
  assert.equal(profile.presentationDirectory,'ports/th08/portable/presentation-lab');
  assert.equal(profile.envNames.snapshot,'TH08_LAB_SNAPSHOT');
  assert.deepEqual(profile.lanes,['presentation','quick','daily']);
  assert.equal(profile.presentation.pairedReplay.schema,'eagler-presentation/replay-pair-capability/3');
  assert.equal(profile.presentation.pairedReplay.fixture,'demo0');
  assert.equal(profile.presentation.pairedReplay.inputFile,'demo0.rpy');
  assert.deepEqual(profile.presentation.pairedReplay.checks,PAIRED_PRESENTATION_CHECKS);
  assert.deepEqual(profile.presentation.pairedReplay.cadencesHz,[60,120,144]);
  assert.equal(profile.presentation.pairedReplay.expectedAuthorityGroups,17);
  assert.deepEqual(profile.presentation.pairedReplay.cadenceDifferenceGroups,PAIRED_CADENCE_DIFFERENCE_GROUPS);
});

test('diagnostic profile binds quick and daily captures to explicit input identities',()=>{
  const profile=resolveDiagnosticProfile('th08','normal');
  assert.equal(profile.quickInputs.length,profile.quickFixtureCount);
  assert.deepEqual(profile.quickInputs.map(item=>item.index),[0,1,2,3]);
  assert.deepEqual(profile.quickInputs.map(item=>item.id),['demo0','demo1','demo2','demo3']);
  assert.ok(profile.quickInputs.every(item=>item.file===`${item.id}.rpy`));
  assert.ok(profile.quickInputs.every(item=>item.captureFile===`${item.id}.json`));
  assert.equal(profile.quickCaptureSchema,'th08/current-replay-capture/v2');
  assert.deepEqual(new Set(profile.dailyFixtures.map(item=>item.replayKey)),
    new Set(profile.dailyGateReplayOptions.map(item=>item.replayKey)));
  assert.deepEqual(profile.dailyFixtures.map(item=>item.id),['lunatic','extra']);
  assert.ok(profile.dailyFixtures.every(item=>item.captureFile===`${item.id}.json`));
  assert.equal(profile.dailyCaptureSchema,'th08/current-replay-tick-capture/v2');
});

test('diagnostic lane check map stays limited to operations with structured evidence',()=>{
  assert.deepEqual(DIAGNOSTIC_LANE_CHECKS.presentation,['DISPLAY-03']);
  assert.deepEqual(DIAGNOSTIC_LANE_CHECKS.quick,['VERIFY-01','VERIFY-03','VERIFY-04','VERIFY-06','VERIFY-08']);
  assert.deepEqual(DIAGNOSTIC_LANE_CHECKS.daily,['VERIFY-02','VERIFY-03','VERIFY-04','VERIFY-06','VERIFY-08']);
  const profile=resolveDiagnosticProfile('th08','normal');
  assert.ok(profile.presentation.partialChecks.includes('DISPLAY-08'));
  assert.ok(profile.quickPartialChecks.includes('VERIFY-08'));
  assert.ok(profile.presentation.partialChecks.includes('REPLAY-07'));
  assert.ok(profile.presentation.partialChecks.includes('VERIFY-07'));
  assert.deepEqual(PAIRED_PRESENTATION_CHECKS,['DISPLAY-01','DISPLAY-02','DISPLAY-03','DISPLAY-04','DISPLAY-05',
    'DISPLAY-06','DISPLAY-07','DISPLAY-08','REPLAY-07','AUDIO-06','VERIFY-07']);
});

test('diagnostic registry validates title profiles and explicitly partitions unsupported lanes',()=>{
  const th08=resolveDiagnosticProfile('th08','normal');
  const next={...structuredClone(th08),game:'th06',lanes:['presentation']};
  for(const key of ['quickCapture','quickCaptureSchema','quickGate','quickFixtureCount','quickInputs','quickPartialChecks',
    'dailyCapture','dailyCaptureSchema','dailyGate','dailyGateReplayOptions','dailyFixtures','dailyPartialChecks','goldenManifest'])delete next[key];
  delete next.laneScope.quick;delete next.laneScope.daily;delete next.laneScope.otherVerification;
  delete next.decisionInputs.quick;delete next.decisionInputs.daily;
  delete next.presentation.pairedReplay;
  const registry=createDiagnosticsRegistry([next]);
  assert.equal(registry.resolve('th06','normal').game,'th06');
  assert.equal(registry.resolve('th06','multiplayer'),null);
  assert.deepEqual(partitionDiagnosticLanes(registry.resolve('th06','normal'),['presentation','quick','daily']),
    {ready:['presentation'],blocked:['quick','daily']});
  assert.deepEqual(partitionDiagnosticLanes(null,['presentation','quick']),{ready:[],blocked:['presentation','quick']});
  assert.deepEqual(pairedPresentationCapability(registry.resolve('th06','normal')),
    {status:'blocked',reason:'th06/normal has no paired Replay presentation adapter'});
  assert.equal(pairedPresentationCapability(th08).status,'ready');
});

test('diagnostic registry rejects missing lane declarations, incomplete observations and duplicate profiles',()=>{
  const profile=structuredClone(resolveDiagnosticProfile('th08','normal'));
  const noLanes={...profile};delete noLanes.lanes;
  assert.throws(()=>createDiagnosticsRegistry([noLanes]),/explicitly declare supported lanes/);
  const missingQuick={...profile,quickInputs:[]};
  assert.throws(()=>createDiagnosticsRegistry([missingQuick]),/quickInputs/);
  assert.throws(()=>createDiagnosticsRegistry([profile,structuredClone(profile)]),/Duplicate diagnostic profile/);
  const badInput={...profile,decisionInputs:{...profile.decisionInputs,quick:['../untracked.json']}};
  assert.throws(()=>createDiagnosticsRegistry([badInput]),/safe repository-relative path/);
  const undeclared={...profile,lanes:['presentation']};
  assert.throws(()=>createDiagnosticsRegistry([undeclared]),/configuration without declaring that lane/);
  const badPair={...profile,presentation:{...profile.presentation,pairedReplay:{...profile.presentation.pairedReplay,inputFile:'another.rpy'}}};
  assert.throws(()=>createDiagnosticsRegistry([badPair]),/bounded tick count/);
  const broadPair={...profile,presentation:{...profile.presentation,pairedReplay:{...profile.presentation.pairedReplay,
    cadenceDifferenceGroups:[...profile.presentation.pairedReplay.cadenceDifferenceGroups,'determinism-and-game-values']}}};
  assert.throws(()=>createDiagnosticsRegistry([broadPair]),/only the two measured non-authority cadence groups/);
  const missingCadence={...profile,presentation:{...profile.presentation,pairedReplay:{...profile.presentation.pairedReplay,cadencesHz:[60,120]}}};
  assert.throws(()=>createDiagnosticsRegistry([missingCadence]),/60, 120 and 144 Hz/);
  const missingAuthorityCount={...profile,presentation:{...profile.presentation,pairedReplay:{...profile.presentation.pairedReplay,expectedAuthorityGroups:undefined}}};
  assert.throws(()=>createDiagnosticsRegistry([missingAuthorityCount]),/expected authority-group count/);
});
