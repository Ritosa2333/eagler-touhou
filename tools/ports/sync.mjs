import {execFileSync, spawnSync} from 'node:child_process';
import {readFileSync, writeFileSync, existsSync, mkdirSync, mkdtempSync, rmSync, lstatSync} from 'node:fs';
import {resolve, relative, dirname, isAbsolute} from 'node:path';
import {tmpdir} from 'node:os';
import {fileURLToPath} from 'node:url';

const root = resolve(import.meta.dirname, '../..');
const statePath = 'docs/adaptation/port-sync.json';
function git(cwd, args, options = {}) {
  return execFileSync('git', args, {cwd, windowsHide: true, maxBuffer: 32 * 1024 * 1024, ...options});
}
function text(cwd, args) { return git(cwd, args, {encoding: 'utf8'}).trim(); }
function commit(cwd, ref) { return text(cwd, ['rev-parse', '--verify', ref + '^{commit}']); }
function ancestor(cwd, base, source) {
  const result = spawnSync('git', ['merge-base', '--is-ancestor', base, source], {cwd, windowsHide: true, encoding: 'utf8'});
  if (result.status !== 0) throw Error(`来源未继承同步基线 ${base.slice(0, 12)}，请核对作品与分支祖先`);
}
function blob(cwd, ref, path) {
  const result = spawnSync('git', ['show', `${ref}:${path}`], {cwd, windowsHide: true, maxBuffer: 32 * 1024 * 1024});
  if (result.status !== 0) throw Error(`无法读取 ${ref}:${path}`);
  return result.stdout;
}
const equal = (a, b) => a === null ? b === null : b !== null && a.equals(b);

export function importedHistory(cwd, normal, multiplayer) {
  const importsPath = resolve(cwd, 'docs/migration/import-sources.json');
  const movesPath = resolve(cwd, 'docs/migration/move-sources-20261018.json');
  const imports = existsSync(importsPath) ? JSON.parse(readFileSync(importsPath, 'utf8')).imports : [];
  const moves = existsSync(movesPath) ? JSON.parse(readFileSync(movesPath, 'utf8')).sources : [];
  const source = path => moves.find(row => row.target === path)?.head ?? imports.find(row => row.target === path.replace('ports/', 'runtimes/'))?.head;
  const a = source(normal), b = source(multiplayer);
  if (!a || !b) return undefined;
  const normalCommit = commit(cwd, a), multiplayerCommit = commit(cwd, b);
  // Import parents retain the real repository history. Never substitute the
  // enclosing launcher's merge-base for the title's original branch ancestry.
  for (const ref of [normalCommit, multiplayerCommit]) git(cwd, ['merge-base', '--is-ancestor', ref, 'HEAD']);
  const bases = spawnSync('git', ['merge-base', '--all', normalCommit, multiplayerCommit], {cwd, windowsHide: true, encoding: 'utf8'});
  if (![0, 1].includes(bases.status)) throw Error(bases.stderr || '无法读取原仓库祖先');
  return {normalCommit, multiplayerCommit, mergeBases: bases.stdout.trim().split('\n').filter(Boolean)};
}

// Source, base and destination are separate owners, as in a branch merge.
export function mergeContent(base, ours, theirs) {
  if (equal(ours, theirs) || equal(base, theirs)) return {content: ours};
  if (equal(base, ours)) return {content: theirs};
  if (base === null || ours === null || theirs === null) return {conflict: 'add/delete'};
  if ([base, ours, theirs].some(value => value.includes(0))) return {conflict: 'binary'};
  const temporary = mkdtempSync(resolve(tmpdir(), 'eagler-port-merge-'));
  try {
    const paths = ['multiplayer', 'base', 'normal'].map(name => resolve(temporary, name));
    [ours, base, theirs].forEach((value, index) => writeFileSync(paths[index], value));
    const result = spawnSync('git', ['merge-file', '-p', ...paths], {windowsHide: true, maxBuffer: 32 * 1024 * 1024});
    if (result.error) throw result.error;
    if (result.status === 0) return {content: result.stdout};
    if (result.status > 0 && result.status < 128) return {conflict: 'content'};
    throw Error(result.stderr.toString() || '三方合并失败');
  } finally { rmSync(temporary, {recursive: true, force: true}); }
}

export function plan(cwd, pair, from, manual = false) {
  const source = commit(cwd, from);
  ancestor(cwd, pair.base, source);
  const paths = pair.paths?.map(path => `${pair.normal}/${path}`) ?? [pair.normal];
  const changes = git(cwd, ['diff', '--no-renames', '--name-status', '-z', pair.base, source, '--', ...paths]).toString().split('\0');
  const entries = [];
  for (let index = 0; index < changes.length - 1; index += 2) {
    const status = changes[index], path = changes[index + 1];
    if (!manual && !['A', 'M', 'D'].includes(status)) throw Error(`需要人工处理的文件类型：${status} ${path}`);
    if (!path.startsWith(pair.normal + '/')) throw Error('源目录不匹配');
    const suffix = path.slice(pair.normal.length + 1);
    const destination = pair.multiplayer + '/' + suffix;
    if (manual) { entries.push({path: destination}); continue; }
    const absolute = resolve(cwd, destination);
    if (relative(cwd, absolute).startsWith('..') || isAbsolute(relative(cwd, absolute)) || absolute === resolve(cwd)) throw Error('目标路径越界');
    if (relative(resolve(cwd, pair.multiplayer), absolute).startsWith('..')) throw Error('目标路径越界');
    for (let current = absolute; current !== resolve(cwd); current = dirname(current)) {
      if (lstatSync(current, {throwIfNoEntry: false})?.isSymbolicLink()) throw Error(`需要人工处理的目标链接：${destination}`);
    }
    // Keep symlinks, executable mode changes and submodules for explicit review.
    for (const ref of [pair.base, source]) {
      const mode = text(cwd, ['ls-tree', ref, '--', path]).split(' ')[0];
      if (mode && mode !== '100644') throw Error(`需要人工处理的文件模式：${path}`);
    }
    const before = existsSync(absolute) ? readFileSync(absolute) : null;
    const base = status === 'A' ? null : blob(cwd, pair.base, path);
    const incoming = status === 'D' ? null : blob(cwd, source, path);
    entries.push({path: destination, before, ...mergeContent(base, before, incoming)});
  }
  return {source, entries};
}

export function main(args = process.argv.slice(2), cwd = root) {
  const [command = 'status', game, ...rest] = args;
  const option = name => { const index = rest.indexOf(name); return index < 0 ? undefined : rest[index + 1]; };
  const path = resolve(cwd, statePath);
  const state = existsSync(path) ? JSON.parse(readFileSync(path, 'utf8')) : {schema: 'eagler-touhou/port-sync/1', pairs: {}};
  const save = () => writeFileSync(path, JSON.stringify(state, null, 2) + '\n');
  const layout = JSON.parse(readFileSync(resolve(cwd, 'launcher/config/workspace.json'), 'utf8')).repositories;
  if (command === 'init') {
    if (!game || !layout[game] || !layout[game + 'mp']) throw Error('请选择具有独立 MP 目录的作品');
    if (state.pairs[game]) throw Error('该作品已有同步基线');
    if (!option('--base')) throw Error('init 需要显式 --base，选择本轮普通版修改之前的基线');
    state.pairs[game] = {normal: layout[game], multiplayer: layout[game + 'mp'], origin: importedHistory(cwd, layout[game], layout[game + 'mp']), base: commit(cwd, option('--base')), history: []};
    save(); return;
  }
  const games = game ? [game] : Object.keys(state.pairs);
  for (const title of games) {
    const pair = state.pairs[title];
    if (!pair) throw Error(`${title} 尚未初始化，请使用 init --base`);
    if (command === 'watch') {
      const name = option('--name');
      if (!name || !/^[a-z0-9_-]+$/i.test(name)) throw Error('监控名称使用字母、数字、下划线或连字符');
      if (pair.watches?.[name]) throw Error('该监控已存在，保留既有接收进度');
      const paths = rest.flatMap((value, index) => value === '--path' ? [rest[index + 1]] : []);
      if (!paths.length) throw Error('至少指定一个 --path，路径相对普通版目录');
      for (const path of paths) {
        if (!path || path.startsWith('-') || path.includes('\\') || path.startsWith('/') || path.includes(':') || path.split('/').some(part => part === '..' || part === '.')) throw Error('监控路径必须是目录内的相对文件或文件夹');
      }
      pair.watches ??= {};
      pair.watches[name] = {paths, base: commit(cwd, option('--base') ?? pair.base), history: []};
      save(); continue;
    }
    const watchName = option('--watch');
    const watch = watchName ? pair.watches?.[watchName] : undefined;
    if (watchName && !watch) throw Error(`未知监控：${watchName}`);
    const progress = watch ?? pair;
    const scoped = {...pair, ...progress};
    if (command === 'log') {
      if (rest.includes('--original')) {
        if (!pair.origin) throw Error('尚无成对原仓库导入记录');
        console.log(text(cwd, ['log', '--graph', '--oneline', '--boundary', '--max-count=80', pair.origin.normalCommit, pair.origin.multiplayerCommit]));
      } else console.log(JSON.stringify({game: title, watch: watchName, origin: pair.origin, base: progress.base, paths: watch?.paths, history: progress.history, watches: watch ? undefined : pair.watches}, null, 2));
      continue;
    }
    if (command === 'status') {
      console.log(`${title} | ${pair.normal} -> ${pair.multiplayer}`);
      if (pair.origin) console.log(`原仓库 | normal ${pair.origin.normalCommit.slice(0, 12)} | MP ${pair.origin.multiplayerCommit.slice(0, 12)} | 共同祖先 ${pair.origin.mergeBases.map(ref => ref.slice(0, 12)).join(', ') || '无，分别保留原历史'}`);
      const scopes = watch ? [[watchName, watch]] : rest.includes('--all') ? [] : Object.entries(pair.watches ?? {});
      if (!scopes.length) scopes.push(['all', pair]);
      for (const [name, entry] of scopes) {
        ancestor(cwd, entry.base, 'HEAD');
        const paths = entry.paths?.map(path => `${pair.normal}/${path}`) ?? [pair.normal];
        console.log(`监控 ${name} | 已处理基线 ${entry.base.slice(0, 12)} | ${entry.paths?.join(', ') ?? '整个普通版目录'}`);
        console.log(text(cwd, ['log', '--oneline', '--name-status', `${entry.base}..HEAD`, '--', ...paths]) || '基线之后无相关提交');
        console.log(text(cwd, ['diff', '--name-only', 'HEAD', '--', ...paths]) || '监控范围工作区无修改');
        const added = text(cwd, ['ls-files', '--others', '--exclude-standard', '--', ...paths]);
        if (added) console.log(`未提交新增文件：\n${added}`);
      }
      continue;
    }
    if (command === 'diff') {
      const paths = progress.paths?.map(path => `${pair.normal}/${path}`) ?? [pair.normal];
      const from = commit(cwd, option('--from') ?? 'HEAD');
      ancestor(cwd, progress.base, from);
      console.log(text(cwd, ['diff', progress.base, from, '--', ...paths]));
      continue;
    }
    if (!['merge', 'record'].includes(command) || !game) throw Error('命令：init/watch/status/diff/log/merge/record <game>');
    if (pair.watches && !watch && !rest.includes('--all')) throw Error('请选择 --watch <name>；同步整个目录显式使用 --all');
    const note = option('--note');
    if ((rest.includes('--apply') || command === 'record') && !note) throw Error('登记同步需要 --note 记录多人版的处理');
    const result = plan(cwd, scoped, option('--from') ?? 'HEAD', command === 'record');
    if (command === 'record') {
      progress.history.push({kind: 'manual', base: progress.base, source: result.source, targetHead: commit(cwd, 'HEAD'), paths: result.entries.map(entry => entry.path), note});
      progress.base = result.source;
      save();
      console.log(`${title} | 已登记人工同步 | ${result.source.slice(0, 12)}`);
      continue;
    }
    const conflicts = result.entries.filter(entry => entry.conflict);
    const changed = result.entries.filter(entry => !entry.conflict && !equal(entry.before, entry.content));
    console.log(JSON.stringify({game: title, watch: watchName, source: result.source, changes: changed.map(entry => entry.path), conflicts: conflicts.map(({path, conflict}) => ({path, conflict}))}, null, 2));
    if (!rest.includes('--apply')) continue;
    if (conflicts.length) throw Error('存在冲突，目标文件和同步基线保持原状；人工完成对应修改后重试');
    // Reject a stale plan before writing any file. The index and commits stay caller-owned.
    for (const entry of changed) {
      const destination = resolve(cwd, entry.path);
      const current = existsSync(destination) ? readFileSync(destination) : null;
      if (!equal(current, entry.before)) throw Error(`规划后文件已改变：${entry.path}`);
    }
    for (const entry of changed) {
      const destination = resolve(cwd, entry.path);
      if (entry.content === null) rmSync(destination);
      else { mkdirSync(dirname(destination), {recursive: true}); writeFileSync(destination, entry.content); }
    }
    progress.history.push({base: progress.base, source: result.source, targetHead: commit(cwd, 'HEAD'), paths: result.entries.map(entry => entry.path), note});
    progress.base = result.source;
    save();
  }
}

if (process.argv[1] && resolve(process.argv[1]) === fileURLToPath(import.meta.url)) {
  try { main(); } catch (error) { console.error(error.message); process.exitCode = 1; }
}
