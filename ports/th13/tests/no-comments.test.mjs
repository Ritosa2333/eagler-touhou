import test from 'node:test';
import assert from 'node:assert/strict';
import {fileURLToPath} from 'node:url';
import {scan} from '../tools/no-comments.mjs';

test('port sources carry no comments', () => {
  const findings = scan(fileURLToPath(new URL('../', import.meta.url)));
  assert.deepEqual(findings.slice(0, 40), [], `源码不写注释，规则见 docs/DEVELOPMENT.md；剩余 ${findings.length} 处`);
});
