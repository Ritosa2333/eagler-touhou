import test from 'node:test';
import assert from 'node:assert/strict';
import {createHash} from 'node:crypto';
import {link,mkdtemp,mkdir,readFile,rm,symlink,unlink,writeFile} from 'node:fs/promises';
import {tmpdir} from 'node:os';
import {dirname,resolve} from 'node:path';
import {directorySnapshot,loadOracleSuite,preflightOracleInputs} from './oracle-suite.mjs';

const sha256=bytes=>createHash('sha256').update(bytes).digest('hex');

async function fixture(t,{candidateBytes=Buffer.from('current candidate'),baselineBytes=Buffer.from('accepted baseline')}={}) {
  const root=await mkdtemp(resolve(tmpdir(),'eagler-oracle-identity-'));
  t.after(()=>rm(root,{recursive:true,force:true}));
  const candidatePath=resolve(root,'artifacts/cpp/game-core-test.wasm'),baselinePath=resolve(root,'private/baseline-core.wasm'),
    referencePath=resolve(root,'reference/native/accepted-core.wasm'),receiptPath=resolve(root,'.local/oracle-inputs.json');
  await mkdir(dirname(candidatePath),{recursive:true});await mkdir(dirname(baselinePath),{recursive:true});
  await mkdir(dirname(referencePath),{recursive:true});await mkdir(dirname(receiptPath),{recursive:true});
  await writeFile(candidatePath,candidateBytes);await writeFile(baselinePath,baselineBytes);await writeFile(referencePath,baselineBytes);
  const receipt={schema:'eagler-touhou/oracle-private-inputs/1',owner:'th11',inputs:{
    baseline:{kind:'file',path:'private/baseline-core.wasm',sha256:sha256(baselineBytes),
      referenceSource:{path:'reference/native/accepted-core.wasm',sha256:sha256(baselineBytes),
        provenance:'Independent historical build recorded for this fixture'}},
  }};
  await writeFile(receiptPath,JSON.stringify(receipt));
  const spec={id:'baseline-core',kind:'receipt',receiptId:'baseline',inputKind:'file',pathEnv:'TH11_EQUIVALENCE_CORE',
    distinctFromCandidate:'artifacts/cpp/game-core-test.wasm',referenceIdentity:{kind:'accepted-reference-file'}};
  const environment={EAGLER_ORACLE_INPUT_RECEIPT:receiptPath};
  return {root,candidatePath,baselinePath,referencePath,receiptPath,receipt,spec,environment,
    check:required=>preflightOracleInputs(root,{owner:'th11'},required??[spec],environment)};
}

test('TH11 core-equivalence accepts a distinct accepted reference source and reports both byte identities',async t=>{
  const f=await fixture(t,{candidateBytes:Buffer.from('same historical core'),baselineBytes:Buffer.from('same historical core')});
  const result=await f.check();
  assert.equal(result.status,'ready',JSON.stringify(result));
  assert.deepEqual(result.inputs[0].referenceIdentity,{kind:'accepted-reference-file',path:f.referencePath,
    sha256:sha256(Buffer.from('same historical core')),provenance:'Independent historical build recorded for this fixture',receiptPath:f.receiptPath,
    receiptSha256:sha256(await readFile(f.receiptPath))});
  assert.deepEqual(result.inputs[0].candidateComparison,{path:f.candidatePath,
    sha256:sha256(Buffer.from('same historical core')),sameSha256:true,referenceIdentityBound:true});
});

test('TH11 baseline without an accepted source record is blocked, including a same-byte copy',async t=>{
  const f=await fixture(t,{candidateBytes:Buffer.from('identical bytes'),baselineBytes:Buffer.from('identical bytes')});
  delete f.receipt.inputs.baseline.referenceSource;
  await writeFile(f.receiptPath,JSON.stringify(f.receipt));
  const result=await f.check();
  assert.equal(result.status,'blocked');
  assert.match(result.blocked[0].reason,/lacks accepted reference source identity/);

  f.receipt.inputs.baseline.referenceSource={path:'reference/native/accepted-core.wasm',
    sha256:sha256(Buffer.from('identical bytes')),provenance:'   '};
  await writeFile(f.receiptPath,JSON.stringify(f.receipt));
  const unreviewable=await f.check();
  assert.equal(unreviewable.status,'blocked');
  assert.match(unreviewable.blocked[0].reason,/lacks accepted reference source identity/);

  f.receipt.inputs.baseline.referenceSource={path:'reference/native/accepted-core.wasm',
    sha256:'0'.repeat(64),provenance:'Independent historical build recorded for this fixture'};
  await writeFile(f.receiptPath,JSON.stringify(f.receipt));
  const sourceHashMismatch=await f.check();
  assert.equal(sourceHashMismatch.status,'blocked');
  assert.match(sourceHashMismatch.blocked[0].reason,/Accepted reference source hash mismatch/);
});

test('TH11 baseline rejects candidate paths, realpath aliases and hardlinks',async t=>{
  const f=await fixture(t);
  f.receipt.inputs.baseline.path='artifacts/cpp/game-core-test.wasm';
  f.receipt.inputs.baseline.sha256=sha256(await readFile(f.candidatePath));
  f.receipt.inputs.baseline.referenceSource.sha256=f.receipt.inputs.baseline.sha256;
  await writeFile(f.referencePath,await readFile(f.candidatePath));
  await writeFile(f.receiptPath,JSON.stringify(f.receipt));
  const samePath=await f.check();
  assert.equal(samePath.status,'blocked');
  assert.match(samePath.blocked[0].reason,/aliases the candidate artifact/);

  const aliasDirectory=resolve(f.root,'private/candidate-alias');
  await symlink(dirname(f.candidatePath),aliasDirectory,'junction');
  f.receipt.inputs.baseline.path='private/candidate-alias/game-core-test.wasm';
  await writeFile(f.receiptPath,JSON.stringify(f.receipt));
  const alias=await f.check();
  assert.equal(alias.status,'blocked');
  assert.match(alias.blocked[0].reason,/aliases the candidate artifact/);

  const hardlinkPath=resolve(f.root,'private/candidate-hardlink.wasm');
  await link(f.candidatePath,hardlinkPath);
  f.receipt.inputs.baseline.path='private/candidate-hardlink.wasm';
  await writeFile(f.receiptPath,JSON.stringify(f.receipt));
  const hardlink=await f.check();
  assert.equal(hardlink.status,'blocked');
  assert.match(hardlink.blocked[0].reason,/aliases the candidate artifact/);
});

test('TH11 accepted source identity must be a separate file from the candidate',async t=>{
  const bytes=Buffer.from('same candidate and reference bytes'),f=await fixture(t,{candidateBytes:bytes,baselineBytes:bytes});
  await unlink(f.referencePath);
  await link(f.candidatePath,f.referencePath);
  const result=await f.check();
  assert.equal(result.status,'blocked');
  assert.match(result.blocked[0].reason,/Reference source aliases the candidate artifact/);
});

test('TH11 prefix baseline identity is tied to the verified native-prefix state field',async t=>{
  const f=await fixture(t,{candidateBytes:Buffer.from('same old core'),baselineBytes:Buffer.from('same old core')});
  const prefixPath=resolve(f.root,'private/native-prefix'),statePath=resolve(prefixPath,'state.json');
  await mkdir(prefixPath,{recursive:true});
  await writeFile(statePath,JSON.stringify({identity:{coreSha256:sha256(Buffer.from('same old core'))}}));
  const prefix=await directorySnapshot(prefixPath);
  f.receipt.inputs['native-prefix-state']={kind:'directory',path:'private/native-prefix',sha256:prefix.sha256};
  f.receipt.inputs['prefix-core']={kind:'file',path:'private/baseline-core.wasm',sha256:sha256(Buffer.from('same old core'))};
  await writeFile(f.receiptPath,JSON.stringify(f.receipt));
  const prefixSpec={id:'prefix-baseline-core',kind:'receipt',receiptId:'prefix-core',inputKind:'file',pathEnv:'TH11_PREFIX_CORE',
    distinctFromCandidate:'artifacts/cpp/game-core-test.wasm',
    referenceIdentity:{kind:'receipt-json-field',receiptId:'native-prefix-state',file:'state.json',jsonPath:'identity.coreSha256'}};
  const directorySpec={id:'native-prefix-state',kind:'receipt',receiptId:'native-prefix-state',inputKind:'directory',pathEnv:'TH11_NATIVE_PREFIX'};
  const result=await f.check([prefixSpec,directorySpec]);
  assert.equal(result.status,'ready',JSON.stringify(result));
  assert.equal(result.inputs[0].candidateComparison.sameSha256,true);
  assert.equal(result.inputs[0].referenceIdentity.sha256,sha256(Buffer.from('same old core')));

  await writeFile(statePath,JSON.stringify({identity:{coreSha256:sha256(Buffer.from('different core'))}}));
  const changedTree=await directorySnapshot(prefixPath);
  f.receipt.inputs['native-prefix-state'].sha256=changedTree.sha256;
  await writeFile(f.receiptPath,JSON.stringify(f.receipt));
  const wrongSource=await f.check([prefixSpec,directorySpec]);
  assert.equal(wrongSource.status,'blocked');
  assert.match(wrongSource.blocked[0].reason,/Reference identity field does not match/);
});

test('registered TH11 and TH15 oracle groups name their actual reference consumers',async()=>{
  for(const owner of ['th11','th11-mp']) {
    const suite=await loadOracleSuite(resolve('ports',owner,'web'));
    const group=(id)=>suite.groups.find(item=>item.id===id);
    for(const [groupId,inputId,kind] of [
      ['core-equivalence-dat-replays','baseline-core','accepted-reference-file'],
      ['dat-replays-world-prefix-equivalence','prefix-baseline-core','receipt-json-field'],
    ]) {
      const input=group(groupId).requiredInputs.find(item=>item.id===inputId);
      assert.equal(input.distinctFromCandidate,'artifacts/cpp/game-core-test.wasm');
      assert.equal(input.referenceIdentity.kind,kind);
    }
  }
  for(const owner of ['th15','th15-mp']) {
    const suite=await loadOracleSuite(resolve('ports',owner,'web'));
    const battle=suite.candidateGroups.find(group=>group.id==='battle-callback-order');
    assert.deepEqual(battle.tests,['battle-callback-order.test.mjs']);
    assert.deepEqual(battle.requiredInputs,[{id:'original-executable',kind:'target-executable'}]);
    assert.ok(!suite.candidateGroups.find(group=>group.id==='core').tests.includes('battle-callback-order.test.mjs'));
  }
});
