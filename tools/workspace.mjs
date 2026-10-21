#!/usr/bin/env node
import { readFileSync } from 'node:fs';
import { resolve, dirname } from 'node:path';
import { fileURLToPath } from 'node:url';
import { spawnSync } from 'node:child_process';

const root = resolve(dirname(fileURLToPath(import.meta.url)), '..');
const workspace = JSON.parse(readFileSync(resolve(root, 'launcher/config/workspace.json'), 'utf8'));
const builds = JSON.parse(readFileSync(resolve(root, 'launcher/config/runtime-builds.json'), 'utf8'));

export function selectedPaths(game, variant = 'normal') {
  if (game === 'frontend') return ['launcher', 'common/contracts', 'tools', 'docs'];
  const entry = builds.games[game];
  if (!entry) throw new Error(`未知作品：${game}。运行 node tools/workspace.mjs list 查看作品。`);
  const variants = variant === 'both' ? ['normal', 'multiplayer'].filter(name => entry.variants[name]) : [variant];
  const owners = variants.map(name => {
    const definition = entry.variants[name];
    if (!definition || !['normal', 'multiplayer'].includes(name)) throw new Error(`${game} 没有 ${name} 构建。`);
    return workspace.repositories[definition.workspaceRepository ?? entry.workspaceRepository];
  });
  return [...new Set(['launcher', 'common', 'tools', 'docs', 'tests', ...owners])];
}

function runGit(args, options = {}) {
  const result = spawnSync('git', args, { cwd: root, encoding: 'utf8', ...options });
  if (result.error) throw result.error;
  return result;
}

function main(args) {
  const [command = 'help', game = 'frontend', ...flags] = args;
  const variantFlags = flags.filter(flag => flag.startsWith('--variant='));
  if (flags.some(flag => !flag.startsWith('--variant=')) || variantFlags.length > 1) throw new Error('参数：--variant=normal|multiplayer|both');
  const variant = variantFlags[0]?.slice('--variant='.length) ?? 'normal';
  if (!['normal', 'multiplayer', 'both'].includes(variant)) throw new Error('参数：--variant=normal|multiplayer|both');
  if (command === 'list') {
    console.log('作品\t单人源码\tMP 源码');
    for (const [id, entry] of Object.entries(builds.games)) {
      const path = name => entry.variants[name] ? workspace.repositories[entry.variants[name].workspaceRepository ?? entry.workspaceRepository] : '—';
      console.log(`${id}\t${path('normal')}\t${path('multiplayer')}`);
    }
  } else if (command === 'paths' || command === 'select') {
    const paths = selectedPaths(game, variant);
    if (command === 'paths') { console.log(paths.join('\n')); return; }
    const sparse = runGit(['config', '--bool', 'core.sparseCheckout']);
    if (sparse.status > 1) throw new Error(sparse.stderr.trim());
    if (sparse.stdout.trim() === 'true') {
      const result = runGit(['sparse-checkout', 'add', '--', ...paths], { stdio: 'inherit' });
      if (result.status !== 0) throw new Error('检出未完成；查看上方 Git 信息。');
    } else console.log('当前为完整检出，所需目录已包含在工作区。');
    console.log(`所需目录：${paths.join(', ')}`);
    const guides = paths.filter(path => path.startsWith('ports/')).map(path => `${path}/README.md`);
    console.log(game === 'frontend' ? '下一步：npm --prefix launcher ci --ignore-scripts，然后 npm run dev。' : `下一步：阅读 ${guides.join('、')}。`);
  } else if (command === 'help' || command === '--help') {
    console.log('node tools/workspace.mjs list\nnode tools/workspace.mjs paths th10 --variant=both\nnode tools/workspace.mjs select th10 --variant=both\nnode tools/workspace.mjs select frontend\n\nselect 为稀疏检出增加所需目录，保留已选择的组件。');
  } else throw new Error(`未知命令：${command}。运行 node tools/workspace.mjs help。`);
}

if (process.argv[1] && resolve(process.argv[1]) === fileURLToPath(import.meta.url)) {
  try { main(process.argv.slice(2)); } catch (error) { console.error(error.message); process.exitCode = 1; }
}
