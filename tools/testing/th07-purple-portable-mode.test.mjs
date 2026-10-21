import test from 'node:test';
import assert from 'node:assert/strict';
import {spawnSync} from 'node:child_process';
import {resolve} from 'node:path';
import {fileURLToPath} from 'node:url';

const script = resolve(fileURLToPath(new URL('../../ports/th07/tests/test-purple-portable.mjs', import.meta.url)));

function run(args, env = {}) {
  return spawnSync(process.execPath, [script, ...args], {
    encoding: 'utf8',
    env: {...process.env, EMSDK: '', TH07_RETAIL_DAT: '', ...env},
    windowsHide: true,
  });
}

test('TH07 portable entry defaults to full coverage and blocks without retail data', () => {
  const result = run([]);
  assert.equal(result.status, 2);
  assert.match(result.stderr, /requires TH07_RETAIL_DAT/);
  assert.doesNotMatch(result.stderr, /Set EMSDK/);
});

test('TH07 explicit unit mode bypasses retail data, while retail mode requires it', () => {
  const unit = run(['--mode=unit']);
  assert.equal(unit.status, 2);
  assert.match(unit.stderr, /Set EMSDK explicitly/);
  assert.doesNotMatch(unit.stderr, /requires TH07_RETAIL_DAT/);

  const retail = run(['--mode=retail']);
  assert.equal(retail.status, 2);
  assert.match(retail.stderr, /requires TH07_RETAIL_DAT/);
});

test('TH07 portable entry rejects unknown modes before compilation', () => {
  const result = run(['--mode=fast']);
  assert.equal(result.status, 2);
  assert.match(result.stderr, /Mode must be unit, retail, or all/);
});
