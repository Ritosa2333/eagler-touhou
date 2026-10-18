import assert from 'node:assert/strict';
import {test} from 'node:test';
import {selectedPortGames} from '../../launcher/tests/support/port-selection.mjs';
import {STATUS_SCHEMA,checkStates,retainedVerification,validateStatusReport} from './status.mjs';
import {targets,dependencyRevision,sourceIdentity} from './inputs.mjs';
import {mkdtempSync,writeFileSync,rmSync} from 'node:fs';
import {tmpdir} from 'node:os';
import {resolve,dirname} from 'node:path';

test('source identity normalizes line endings and preserves non-UTF-8 source bytes',()=>{
  const directory=mkdtempSync(resolve(tmpdir(),'eagler-source-identity-')),file=resolve(directory,'source.mjs');
  try {
    writeFileSync(file,'line\n');const lf=sourceIdentity([file]);
    writeFileSync(file,'line\r\n');assert.equal(sourceIdentity([file]),lf);
    writeFileSync(file,Buffer.from([0xff]));const first=sourceIdentity([file]);
    writeFileSync(file,Buffer.from([0xfe]));assert.notEqual(sourceIdentity([file]),first);
  } finally {
    assert.equal(dirname(directory),resolve(tmpdir()));rmSync(directory,{recursive:true,force:true});
  }
});
import {adaptationMarkdown,preserveManualEvidence,MANUAL_EVIDENCE} from '../docs/new-adaptation.mjs';

test('regenerating source status preserves hand-written runtime acceptance records',()=>{
  const generated=adaptationMarkdown({game:'th20',directory:'ports/th20',variants:['normal']});
  const notes=`${MANUAL_EVIDENCE}\n\nDISPLAY-04: device, build identity, steps and evidence link\n`;
  const updated=preserveManualEvidence(generated,`old generated table\n${notes}`);
  assert.ok(updated.includes('| BUILD-01 |'));
  assert.equal(updated.slice(updated.indexOf(MANUAL_EVIDENCE)),notes);
  assert.equal(preserveManualEvidence(generated,''),generated);
});

test('single-game inspection is explicit and ordinary tests retain full coverage',()=>{
  assert.deepEqual(selectedPortGames(['th06','th07'],{}),['th06','th07']);
  assert.deepEqual(selectedPortGames(['th06','th07'],{EAGLER_PORT_CHECK_GAMES:'["th07"]'}),['th07']);
  for (const value of ['[]','["th99"]','"th06"','null'])
    assert.throws(()=>selectedPortGames(['th06'],{EAGLER_PORT_CHECK_GAMES:value}));
  assert.throws(()=>targets({game:'th99'}));
  assert.throws(()=>targets({game:'th20',variant:'multiplayer'}));
});

test('failed evidence wins, partial checks cannot complete an acceptance item',()=>{
  const state=gates=>checkStates({port:{game:'th08'},variant:'normal',checks:[{id:'RUNTIME-06'}],gates,notApplicableFor:()=>null})['RUNTIME-06'].status;
  const gate={id:'option-adapter',checks:['RUNTIME-06'],status:'passed'};
  assert.equal(state([{...gate,complete:false}]),'待核对');
  assert.equal(state([gate]),'已实现');
  assert.equal(state([gate,{...gate,id:'native-endpoint',status:'failed'}]),'检查失败');
});

test('executed build evidence survives regeneration and expires when its source changes',()=>{
  const gate={id:'compiled-runtime',checks:['BUILD-03'],level:'build',status:'passed',complete:true};
  const previous={sourceDigest:'a'.repeat(64),gates:[gate]};
  assert.deepEqual(retainedVerification(previous,'a'.repeat(64)),[gate]);
  assert.deepEqual(retainedVerification(previous,'b'.repeat(64)),[]);
  const state=checkStates({port:{game:'th08'},variant:'normal',checks:[{id:'BUILD-03'}],gates:[gate],notApplicableFor:()=>null});
  assert.equal(state['BUILD-03'].status,'已验证');
});

test('dependency pins and local edits invalidate evidence while clean checkout state stays portable',()=>{
  const pin='a'.repeat(40),path='ports/th06/vendored/SDL';
  const absent=dependencyRevision(path,pin,null),clean=dependencyRevision(path,pin,{revision:pin,dirty:false});
  assert.deepEqual(absent,clean);
  const digest=dependency=>sourceIdentity([],{dependencies:[dependency]});
  assert.notEqual(digest(clean),digest(dependencyRevision(path,'b'.repeat(40),null)));
  assert.notEqual(digest(clean),digest(dependencyRevision(path,pin,{revision:pin,dirty:true,sha256:'c'.repeat(64)})));
  assert.throws(()=>dependencyRevision(path,pin,{revision:'b'.repeat(40)}),/differs/);
});

test('reports reject invented targets, duplicate gates and nonexistent acceptance items',()=>{
  const target={game:'th08',variant:'normal',sourceDigest:'a'.repeat(64),gates:[
    {id:'native',checks:['INPUT-04'],level:'source-contract',status:'passed'}]};
  const check=targets=>validateStatusReport({schema:STATUS_SCHEMA,targets},{expectedTargets:[target],checkIds:new Set(['INPUT-04'])});
  assert.deepEqual(check([target]),[]);
  assert.ok(check([{...target,game:'th99'}]).some(issue=>issue.includes('未登记')));
  assert.ok(check([{...target,gates:[...target.gates,...target.gates]}]).some(issue=>issue.includes('重复')));
  assert.ok(check([{...target,gates:[{...target.gates[0],checks:['INPUT-99']}]}]).some(issue=>issue.includes('未登记适配')));
  assert.ok(check([{...target,sourceDigest:null}]).some(issue=>issue.includes('源码身份')));
});
