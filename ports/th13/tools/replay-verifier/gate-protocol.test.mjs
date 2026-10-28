import test from 'node:test';
import assert from 'node:assert/strict';
import {spawnSync} from 'node:child_process';
import {parseComparison, classify, checkIdentity} from './gate-protocol.mjs';
const result = (first = null) => {
  const status = first === null ? 0 : 1;
  const value = {schema: 'th13/compare/v1', kind: 'ops', first_divergence_frame: first,
    window: 10, expected_frames: 10, ops: 1, exit_code: status, qualified: status === 0,
    ignored: [], ulp: 0, error: null};
  return {status, stdout: 'RESULT_JSON ' + JSON.stringify(value) + '\n' +
    `RESULT first_divergence_frame=${first ?? 'none'} window=10 ops=1\n`};
};
test('strict process + unique mutually consistent result + explicit window', () => {
  assert.equal(parseComparison(result(), 10).first, 'none');
  assert.equal(parseComparison(result(2), 10).first, '2');
  for (const bad of [ {...result(), status: 1}, {...result(), status: null},
    {...result(), signal: 'SIGTERM'}, {...result(), error: Error('spawn')},
    {...result(), stdout: result().stdout.repeat(2)},
    {...result(), stdout: result().stdout + 'RESULT ignored=x\n'},
    {...result(), stdout: result().stdout.replace('"ulp":0', '"ulp":1')},
    {...result(), stdout: result().stdout.replace('"ignored":[]', '"ignored":["x"]')},
    {...result(), stdout: result().stdout.replace('"qualified":true', '"qualified":false')}]) {
    assert.throws(() => parseComparison(bad, 10));
  }
  assert.throws(() => parseComparison(result(), 11));
});
test('unbaselined and held are not fidelity passes; no regression acceptance', () => {
  assert.equal(classify({first: 'none'}, null), 'UNBASELINED');
  assert.equal(classify({first: '4'}, {first_divergence_frame: '3'}), 'BASELINE_HELD');
  assert.equal(classify({first: '2'}, {first_divergence_frame: '3'}), 'REGRESSION');
  assert.equal(classify({first: '2'}, {first_divergence_frame: 'none'}), 'REGRESSION');
  assert.equal(classify({first: 'none'}, {first_divergence_frame: '3'}), 'EXACT_MATCH');
});
test('corpus/oracle/replay/case identity cannot be silently replaced', () => {
  const identity = {corpusSha256: 'a'.repeat(64), oracleSha256: 'b'.repeat(64), caseId: 'case', replaySha256: 'c'.repeat(64)};
  const fixture = {id: 'case', kind: 'replay', replaySha256: identity.replaySha256};
  assert.throws(() => checkIdentity(null, {...fixture, replaySha256: null}, identity));
  assert.throws(() => checkIdentity(null, fixture, {...identity, oracleSha256: 'unknown'}));
  assert.equal(checkIdentity({identity}, fixture, identity), true);
  assert.throws(() => checkIdentity({}, fixture, identity));
  for (const key of Object.keys(identity)) {
    assert.throws(() => checkIdentity({identity: {...identity, [key]: 'changed'}}, fixture, identity));
  }
});
test('ordinary gate rejects accept-golden and unknown options before data access', () => {
  for (const arg of ['--accept-golden', '--typo']) {
    const run = spawnSync(process.execPath, [new URL('./run-gate.mjs', import.meta.url).pathname, arg], {encoding: 'utf8'});
    assert.notEqual(run.status, 0);
    assert.match(run.stderr, arg === '--accept-golden' ? /separate audited operation/ : /Unknown option/);
  }
});
