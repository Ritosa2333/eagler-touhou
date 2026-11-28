#!/usr/bin/env node
import {spawn, spawnSync} from 'node:child_process';
import {copyFileSync, mkdirSync, readFileSync, writeFileSync} from 'node:fs';
import {dirname, resolve} from 'node:path';
import {fileURLToPath} from 'node:url';
import {comparisonIdentity} from './identity.mjs';
import {loadRaw, writeTrace} from './observation.mjs';

const here = dirname(fileURLToPath(import.meta.url));
const repo = resolve(here, '../..');
const args = options(process.argv.slice(2));
const corpus = JSON.parse(readFileSync(resolve(here, 'corpus.json'), 'utf8'));
const fixture = corpus.cases.find(item => item.id === args.case);
if (!fixture) throw Error('Unknown case ' + args.case);
const output = resolve(args.output || resolve(repo, 'artifacts/replay-verifier'));
mkdirSync(output, {recursive: true});
const assets = resolve(repo, 'Assets');
const retail = resolve(assets, 'THSSS.exe');
const replay = resolve(repo, fixture.source);
const stage = String(fixture.stage || 1);
const rawPath = resolve(output, `${fixture.id}.original.raw.jsonl`);
const rngPath = resolve(output, `${fixture.id}.original.rng`);
const tracePath = resolve(output, `${fixture.id}.original.jsonl`);
const work = resolve(repo, 'artifacts/replay-verifier/original');
mkdirSync(work, {recursive: true});

const rewrite = spawnSync('dotnet', ['build', resolve(here, 'original/Rewrite/Rewrite.csproj'), '-c', 'Release', '--nologo'], {stdio: 'inherit'});
if (rewrite.status !== 0) process.exit(rewrite.status ?? 1);
const rewriteDll = resolve(here, 'original/Rewrite/bin/Release/net10.0/Rewrite.dll');
const observeDll = resolve(work, 'THSSS.Observe.dll');
const csc = buildObserver(observeDll);
if (csc !== 0) process.exit(csc);
const rewritten = resolve(assets, 'THSSS.verify.exe');
const rewrittenRun = spawnSync('dotnet', [rewriteDll, retail, observeDll, rewritten], {stdio: 'inherit'});
if (rewrittenRun.status !== 0) process.exit(rewrittenRun.status ?? 1);
copyFileSync(observeDll, resolve(assets, 'THSSS.Observe.dll'));
writeFileSync(resolve(assets, 'THSSS.verify.cfg'), `REPLAY=${winePath(replay)}\nSTAGE=${stage}\nOUT=${winePath(rawPath)}\nRNG=${winePath(rngPath)}\nMAX=${process.env.THSSS_VERIFY_MAX || '0'}\n`);
ensureWindowed(resolve(assets, 'Setting.INI'));

const proton = process.env.THSSS_PROTON || '/usr/share/steam/compatibilitytools.d/proton-cachyos-slr/proton';
const env = {
  ...process.env,
  STEAM_COMPAT_CLIENT_INSTALL_PATH: process.env.STEAM_COMPAT_CLIENT_INSTALL_PATH || `${process.env.HOME}/.local/share/Steam`,
  STEAM_COMPAT_DATA_PATH: process.env.STEAM_COMPAT_DATA_PATH || `${process.env.HOME}/.local/share/Steam/steamapps/compatdata/thsss-local`,
  LANG: 'zh_CN.UTF-8',
  LC_ALL: 'zh_CN.UTF-8',
  LOCPATH: process.env.LOCPATH || `${process.env.HOME}/.local/lib/locale`,
  DISPLAY: process.env.DISPLAY || ':0',
  WINEDLLOVERRIDES: 'd3dx9_43=n;d3dcompiler_43=n;xaudio2_7=n',
  THSSS_VERIFY_REPLAY: winePath(replay),
  THSSS_VERIFY_STAGE: stage,
  THSSS_VERIFY_OUT: winePath(rawPath),
  THSSS_VERIFY_MAX: process.env.THSSS_VERIFY_MAX || '0',
};
const child = spawn(proton, ['run', rewritten], {cwd: assets, env, stdio: ['ignore', 'inherit', 'inherit']});
const timer = setTimeout(() => child.kill('SIGTERM'), Number(process.env.THSSS_VERIFY_TIMEOUT_MS || 180000));
const code = await new Promise(accept => child.on('exit', accept));
clearTimeout(timer);
const rows = loadRaw(rawPath);
const end = rows.find(row => row.ended === true);
const recorded = rows.filter(row => row.record).length;
if (!end || !recorded) {
  console.error('original capture incomplete', end || {code});
  process.exit(code || 1);
}
const identity = comparisonIdentity({assets, replay, executable: retail});
writeTrace(tracePath, {
  identity,
  provider: 'thsss/retail-proton-inject',
  provenance: {injector: 'THSSS.Observe', rewrite: 'MainProcess.UpdateData'},
  rows,
});
console.log(`${fixture.id}.original ${recorded} ticks ${end.reason}`);
if (!end.complete) process.exit(end.reason === 'frame-budget' ? 0 : (code || 2));

function buildObserver(output) {
  const proton = process.env.THSSS_PROTON || '/usr/share/steam/compatibilitytools.d/proton-cachyos-slr/proton';
  const sources = [resolve(here, 'observe/VerifySnapshot.cs'), resolve(here, 'observe/VerifyHost.cs')].map(winePath);
  const env = {
    ...process.env,
    STEAM_COMPAT_CLIENT_INSTALL_PATH: process.env.STEAM_COMPAT_CLIENT_INSTALL_PATH || `${process.env.HOME}/.local/share/Steam`,
    STEAM_COMPAT_DATA_PATH: process.env.STEAM_COMPAT_DATA_PATH || `${process.env.HOME}/.local/share/Steam/steamapps/compatdata/thsss-local`,
  };
  const run = spawnSync(proton, ['run', 'C:\\windows\\Microsoft.NET\\Framework\\v4.0.30319\\csc.exe', '/nologo', '/t:library', '/define:THSSS_OBSERVE', '/r:System.Drawing.dll', `/out:${winePath(output)}`, ...sources], {env, stdio: 'inherit'});
  return run.status ?? 1;
}

function winePath(path) {
  return 'Z:' + path.replaceAll('/', '\\');
}

function ensureWindowed(path) {
  let text = '';
  try { text = readFileSync(path).toString('latin1'); } catch { text = ''; }
  if (!text.includes('[DirectX]')) {
    text = `[DirectX]\nFullWindow=0\nAntiAlias=0\nVerticalSync=0\n[Mode]\nPresentInterval=1\nWindowSize=0\n${text}`;
  }
  text = text.replace(/FullWindow=\d+/, 'FullWindow=0').replace(/PresentInterval=\d+/, 'PresentInterval=1');
  if (!text.endsWith('\n')) text += '\n';
  writeFileSync(path, Buffer.from(text, 'latin1'));
}

function options(argv) {
  const out = {};
  for (let i = 0; i < argv.length; i += 2) out[argv[i].replace(/^--/, '')] = argv[i + 1];
  if (!out.case) throw Error('--case is required');
  return out;
}
