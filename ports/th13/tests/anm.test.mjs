import test from 'node:test';
import assert from 'node:assert/strict';
import {spawnSync} from 'node:child_process';
import {resolve,dirname} from 'node:path';
import {fileURLToPath} from 'node:url';
const port=resolve(dirname(fileURLToPath(import.meta.url)),'..');
test('ANM native suites are included in the unified Node entry',{timeout:600000},()=>{
  const run=spawnSync('bash',[resolve(port,'tests/anm/run.sh')],{encoding:'utf8'});
  assert.equal(run.status,0,run.stdout+run.stderr);
  assert.match(run.stdout,/ok /);
});
