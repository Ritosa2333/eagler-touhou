import test from 'node:test';
import assert from 'node:assert/strict';
import {execFileSync} from 'node:child_process';
import {mkdtempSync, writeFileSync, mkdirSync, rmSync} from 'node:fs';
import {tmpdir} from 'node:os';
import {resolve, dirname} from 'node:path';
import {sourceSnapshot} from './source-snapshot.mjs';

test('test evidence follows content through staging and detects edits inside initialized submodules',()=>{
  const root = mkdtempSync(resolve(tmpdir(), 'eagler-test-snapshot-'));
  const git = (directory, args) => execFileSync('git', ['-c','user.name=Test','-c','user.email=test@example.invalid',...args],
    {cwd:directory, encoding:'utf8', windowsHide:true, stdio:['ignore','pipe','pipe']});
  try {
    git(root, ['init']);
    writeFileSync(resolve(root, 'source.cpp'), 'int value=1;\n');
    git(root, ['add','.']); git(root, ['commit','-m','initial']);
    const first = sourceSnapshot(root);
    writeFileSync(resolve(root, 'source.cpp'), 'int value=2;\n');
    const dirty = sourceSnapshot(root); assert.notEqual(dirty.sha256, first.sha256);
    git(root, ['add','.']); assert.equal(sourceSnapshot(root).sha256, dirty.sha256);
    writeFileSync(resolve(root, 'new-test.mjs'), 'throw Error("new test");\n');
    assert.notEqual(sourceSnapshot(root).sha256, dirty.sha256);
    const nested = resolve(root, 'dependency'); mkdirSync(nested); git(nested, ['init']);
    writeFileSync(resolve(nested, 'dep.cpp'), 'int dependency=1;\n');
    git(nested, ['add','.']); git(nested, ['commit','-m','dependency']);
    const pin = git(nested, ['rev-parse','HEAD']).trim();
    git(root, ['update-index','--add','--cacheinfo', `160000,${pin},dependency`]);
    const cleanDependency = sourceSnapshot(root);
    writeFileSync(resolve(nested, 'dep.cpp'), 'int dependency=2;\n');
    assert.notEqual(sourceSnapshot(root).sha256, cleanDependency.sha256);
  } finally {
    assert.equal(dirname(resolve(root)), resolve(tmpdir()));
    rmSync(root, {recursive:true, force:true});
  }
});
