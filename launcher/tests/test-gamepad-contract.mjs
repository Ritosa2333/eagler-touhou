import {test} from 'vitest';
import assert from 'node:assert/strict';
import {spawnSync} from 'node:child_process';
import {fileURLToPath} from 'node:url';
import {nativeNodeExecutable} from './support/node-runtime.mjs';

test('shared native gamepad mapping', () => {
  const script=fileURLToPath(new URL('../../tools/browser/gamepad-input.test.mjs',import.meta.url));
  const result=spawnSync(nativeNodeExecutable(),['--test',script],{encoding:'utf8',windowsHide:true});
  assert.equal(result.status,0,`${result.stdout}\n${result.stderr}`);
});
