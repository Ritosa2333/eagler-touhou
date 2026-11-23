// 固定一个无独立 Git 来源的作品源码快照，并生成迁入映射。
// 用法：node tools/ports/generate-delivery-snapshot.mjs --game=th16 --date=20261123 [--check]
// 快照只收 git 已跟踪文件；生成前要求该作品目录没有未提交改动。
import {execFileSync, spawnSync} from 'node:child_process';
import {createHash} from 'node:crypto';
import {existsSync, mkdirSync, readFileSync, writeFileSync} from 'node:fs';
import {dirname, join, relative, resolve, sep} from 'node:path';
import {fileURLToPath} from 'node:url';

const args = process.argv.slice(2);
const option = name => args.find(arg => arg.startsWith(`--${name}=`))?.slice(name.length + 3);
const game = option('game');
const date = option('date');
const check = args.includes('--check');
if (!/^th\d+$/.test(game ?? '') || !/^\d{8}$/.test(date ?? '') || args.some(arg => arg !== '--check' && arg !== `--game=${game}` && arg !== `--date=${date}`)) {
  throw Error('Usage: node tools/ports/generate-delivery-snapshot.mjs --game=th16 --date=20261123 [--check]');
}
const root = resolve(dirname(fileURLToPath(import.meta.url)), '../..');
const directory = `ports/${game}`;
for (const extra of [[], ['--cached']]) {
  const result = spawnSync('git', ['diff', ...extra, '--quiet', '--', directory], {cwd: root});
  if (result.status !== 0) throw Error(`${directory} has uncommitted changes; commit the source before snapshotting it`);
}
const names = execFileSync('git', ['ls-files', '-z', '--', directory], {cwd: root, encoding: 'utf8'})
  .split('\0').filter(Boolean).sort();
if (names.length === 0) throw Error(`No tracked source files under ${directory}`);
const hash = bytes => createHash('sha256').update(bytes).digest('hex');
const checksums = {};
const files = names.map(path => {
  const bytes = readFileSync(join(root, path));
  const source = relative(directory, path).split(sep).join('/');
  checksums[source] = hash(bytes);
  return {source, destination: path, bytes: bytes.length, sha256: checksums[source]};
});
const archive = `archive/${game}-development-${date}`;
const manifestPath = `${archive}/FILE-SHA256.json`;
const migrationPath = `docs/migration/${game}-eagler-${date}.json`;
const json = value => JSON.stringify(value, null, 2) + '\n';
const manifest = json(checksums);
const migration = json({
  schema: 'eagler-touhou/source-snapshot/1',
  game,
  source: `${game.toUpperCase()}-local-development-${date}`,
  sourceCommit: null,
  sourceKind: 'development-delivery-without-git',
  destination: directory,
  files,
  sourceManifest: {path: manifestPath, sha256: hash(manifest)},
});
const title = game.toUpperCase();
const readme = `# ${title} 源码交付来源\n\n` +
  `\`FILE-SHA256.json\` 固定 ${date.slice(0, 4)}-${date.slice(4, 6)}-${date.slice(6)} 的本地源码身份。` +
  `迁入映射见 \`../../docs/migration/${game}-eagler-${date}.json\`，当前开发入口为 \`../../ports/${game}/README.md\`。` +
  '本记录不包含原版 DATA、音乐、字体或预编译 Runtime。\n\n' +
  `重新生成或核对：\`node tools/ports/generate-delivery-snapshot.mjs --game=${game} --date=${date}\`（加 \`--check\` 只核对，不写文件）。\n`;
const outputs = new Map([[manifestPath, manifest], [migrationPath, migration], [`${archive}/README.md`, readme]]);
let failed = false;
for (const [path, content] of outputs) {
  if (check) {
    if (!existsSync(join(root, path)) || readFileSync(join(root, path), 'utf8') !== content) {
      console.error(`out of date: ${path}`);
      failed = true;
    }
  } else {
    mkdirSync(dirname(join(root, path)), {recursive: true});
    writeFileSync(join(root, path), content);
    console.log(`${path}: ${path === migrationPath ? `${files.length} files` : 'written'}`);
  }
}
if (failed) process.exitCode = 1;
else if (check) console.log(`${migrationPath}: ${files.length} files, hashes up to date`);
