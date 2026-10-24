import test from 'node:test';
import assert from 'node:assert/strict';
import {mkdtempSync, mkdirSync, writeFileSync, readFileSync, rmSync} from 'node:fs';
import {tmpdir} from 'node:os';
import {resolve} from 'node:path';
import {execFileSync} from 'node:child_process';
import {mergeContent, main, plan, importedHistory} from './sync.mjs';

const b = text => Buffer.from(text);
test('three-way merging preserves multiplayer edits outside the source change', () => {
  const base = 'start\n1\n2\n3\n4\n5\nend\n';
  const result = mergeContent(b(base), b(base.replace('end', 'MP')), b(base.replace('start', 'normal')));
  assert.equal(result.content.toString(), base.replace('start', 'normal').replace('end', 'MP'));
});
test('conflicts retain both owners instead of choosing a side', () => {
  assert.ok(mergeContent(b('base\n'), b('MP\n'), b('normal\n')).conflict);
  assert.ok(mergeContent(null, b('MP'), b('normal')).conflict);
  assert.ok(mergeContent(b('base'), b('MP'), null).conflict);
  assert.ok(mergeContent(b('\0base'), b('\0MP'), b('\0normal')).conflict);
});
test('additions, deletions and already-integrated changes are repeatable', () => {
  assert.equal(mergeContent(null, null, b('new')).content.toString(), 'new');
  assert.equal(mergeContent(b('old'), b('old'), null).content, null);
  assert.equal(mergeContent(b('old'), b('new'), b('new')).content.toString(), 'new');
});
function fixture(t) {
  const cwd = mkdtempSync(resolve(tmpdir(), 'eagler-sync-test-'));
  t.after(() => rmSync(cwd, {recursive: true, force: true}));
  const git = (...args) => execFileSync('git', args, {cwd, windowsHide: true, encoding: 'utf8'}).trim();
  git('init', '-q'); git('config', 'user.name', 'Test'); git('config', 'user.email', 'test@example.invalid');
  for (const path of ['launcher/config', 'docs/adaptation', 'ports/th10', 'ports/th10-mp']) mkdirSync(resolve(cwd, path), {recursive: true});
  const write = (path, value) => writeFileSync(resolve(cwd, path), value);
  write('launcher/config/workspace.json', JSON.stringify({repositories: {th10: 'ports/th10', th10mp: 'ports/th10-mp'}}));
  const baseText = 'start\n1\n2\n3\n4\n5\nend\n';
  write('ports/th10/value.cpp', baseText);
  write('ports/th10-mp/value.cpp', baseText.replace('end', 'MP'));
  git('add', '.'); git('commit', '-qm', 'base');
  const base = git('rev-parse', 'HEAD');
  main(['init', 'th10', '--base', base], cwd);
  write('ports/th10/value.cpp', baseText.replace('start', 'normal'));
  git('add', 'ports/th10'); git('commit', '-qm', 'source');
  return {cwd, git, write, base};
}
test('merge preview is read-only; apply records history without staging or committing', t => {
  const {cwd, git} = fixture(t);
  const stateFile = resolve(cwd, 'docs/adaptation/port-sync.json');
  const before = readFileSync(stateFile, 'utf8');
  main(['merge', 'th10'], cwd);
  assert.equal(readFileSync(stateFile, 'utf8'), before);
  assert.match(readFileSync(resolve(cwd, 'ports/th10-mp/value.cpp'), 'utf8'), /^start/);
  main(['merge', 'th10', '--apply', '--note', '保留多人逻辑'], cwd);
  const state = JSON.parse(readFileSync(stateFile, 'utf8'));
  assert.equal(state.pairs.th10.base, git('rev-parse', 'HEAD'));
  assert.equal(state.pairs.th10.history[0].note, '保留多人逻辑');
  assert.match(readFileSync(resolve(cwd, 'ports/th10-mp/value.cpp'), 'utf8'), /^normal[\s\S]*MP\n$/);
  assert.equal(git('diff', '--cached', '--name-only'), '');
  assert.equal(plan(cwd, state.pairs.th10, 'HEAD').entries.length, 0);
});
test('a conflicting merge leaves target files and baseline untouched', t => {
  const {cwd, write} = fixture(t);
  write('ports/th10-mp/value.cpp', 'MP conflict\n');
  const stateFile = resolve(cwd, 'docs/adaptation/port-sync.json');
  const before = readFileSync(stateFile, 'utf8');
  assert.throws(() => main(['merge', 'th10', '--apply', '--note', '同步'], cwd), /存在冲突/);
  assert.equal(readFileSync(stateFile, 'utf8'), before);
  assert.equal(readFileSync(resolve(cwd, 'ports/th10-mp/value.cpp'), 'utf8'), 'MP conflict\n');
});
test('manual integration records its decision without replacing customized multiplayer code', t => {
  const {cwd, write, git} = fixture(t);
  write('ports/th10-mp/value.cpp', 'normal with multiplayer session\n');
  main(['record', 'th10', '--from', 'HEAD', '--note', '人工合并多人会话分支'], cwd);
  assert.equal(readFileSync(resolve(cwd, 'ports/th10-mp/value.cpp'), 'utf8'), 'normal with multiplayer session\n');
  const pair = JSON.parse(readFileSync(resolve(cwd, 'docs/adaptation/port-sync.json'), 'utf8')).pairs.th10;
  assert.equal(pair.base, git('rev-parse', 'HEAD'));
  assert.equal(pair.history[0].kind, 'manual');
});
test('imported repositories retain their real heads and common ancestor', t => {
  const {cwd, git, base, write} = fixture(t);
  mkdirSync(resolve(cwd, 'docs/migration'), {recursive: true});
  const source = git('rev-parse', 'HEAD');
  write('docs/migration/move-sources-20261018.json', JSON.stringify({sources: [
    {target: 'ports/th10', head: source}, {target: 'ports/th10-mp', head: base},
  ]}));
  assert.deepEqual(importedHistory(cwd, 'ports/th10', 'ports/th10-mp'), {
    normalCommit: source, multiplayerCommit: base, mergeBases: [base],
  });
});
test('folder monitors maintain independent review cursors and merge only their scope', t => {
  const {cwd, base, git, write} = fixture(t);
  main(['watch', 'th10', '--name', 'practice', '--path', 'value.cpp', '--base', base], cwd);
  main(['watch', 'th10', '--name', 'other', '--path', 'other', '--base', base], cwd);
  write('ports/th10-mp/untouched.cpp', 'MP only\n');
  assert.throws(() => main(['merge', 'th10', '--apply', '--note', '同步'], cwd), /请选择 --watch/);
  main(['diff', 'th10', '--watch', 'practice'], cwd);
  main(['merge', 'th10', '--watch', 'practice', '--apply', '--note', '同步监控范围'], cwd);
  const pair = JSON.parse(readFileSync(resolve(cwd, 'docs/adaptation/port-sync.json'), 'utf8')).pairs.th10;
  assert.equal(pair.base, base);
  assert.equal(pair.watches.practice.base, git('rev-parse', 'HEAD'));
  assert.equal(pair.watches.other.base, base);
  assert.equal(pair.watches.other.history.length, 0);
  assert.equal(readFileSync(resolve(cwd, 'ports/th10-mp/untouched.cpp'), 'utf8'), 'MP only\n');
  assert.throws(() => main(['watch', 'th10', '--name', 'unsafe', '--path', '../th11'], cwd), /相对文件/);
});
