import {test} from 'vitest';
import assert from 'node:assert/strict';
import {execFileSync} from 'node:child_process';
import {mkdtemp, readFile, rm, writeFile, access} from 'node:fs/promises';
import {resolve} from 'node:path';
import {fileURLToPath} from 'node:url';
import {checkJavaScriptSyntax} from '../../scripts/check-javascript-syntax.mjs';

const project = fileURLToPath(new URL('../../', import.meta.url));
test('syntax gate accepts modules and CommonJS while reporting source locations', () => {
  checkJavaScriptSyntax('import {readFile} from "node:fs"; await readFile(import.meta.url);', 'tool.mjs');
  checkJavaScriptSyntax('#!/usr/bin/env node\nmodule.exports = 1; return;', 'tool.cjs');
  assert.throws(() => checkJavaScriptSyntax('const invalid = ;', 'broken.mjs'), /broken\.mjs:.*\(1:16\)/);
});
test('syntax CLI never evaluates tools, even ones that write files or throw', async () => {
  const work = await mkdtemp(resolve(project, '.cache/syntax-check-'));
  const source = resolve(work, 'publication.mjs'), marker = resolve(work, 'executed.txt');
  try {
    await writeFile(source, `import {writeFileSync} from 'node:fs'; writeFileSync(${JSON.stringify(marker)},'executed'); throw new Error('Must never run');`);
    const result = execFileSync(process.execPath, ['scripts/check-javascript-syntax.mjs', source], {cwd: project, encoding: 'utf8'});
    assert.match(result, /PASS \(1 files\)/); await assert.rejects(access(marker));
    await writeFile(source, 'export const broken = ;');
    assert.throws(() => execFileSync(process.execPath, ['scripts/check-javascript-syntax.mjs', source], {cwd: project, stdio: 'pipe'}), error => error.status === 1 && /publication\.mjs/.test(error.stderr.toString()));
    assert.equal(await readFile(source, 'utf8'), 'export const broken = ;');
  } finally {await rm(work, {recursive: true, force: true});}
});
