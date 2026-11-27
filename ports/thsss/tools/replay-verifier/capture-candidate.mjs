#!/usr/bin/env node
import {spawnSync} from 'node:child_process';
import {createServer} from 'node:http';
import {mkdirSync, readFileSync, statSync, writeFileSync} from 'node:fs';
import {basename, dirname, extname, resolve} from 'node:path';
import {fileURLToPath, pathToFileURL} from 'node:url';
import {comparisonIdentity} from './identity.mjs';
import {writeTrace} from './observation.mjs';

const here = dirname(fileURLToPath(import.meta.url));
const repo = resolve(here, '../..');
const args = options(process.argv.slice(2));
const corpus = JSON.parse(readFileSync(resolve(here, 'corpus.json'), 'utf8'));
const fixture = corpus.cases.find(item => item.id === args.case);
if (!fixture) throw Error('Unknown case ' + args.case);
const output = resolve(args.output || resolve(repo, 'artifacts/replay-verifier'));
mkdirSync(output, {recursive: true});
const publish = resolve(args.publish || process.env.THSSS_DIAGNOSTIC_DIR || resolve(repo, 'artifacts/replay-verifier/diagnostic'));
const moduleJs = resolve(publish, 'THSSS.Browser.js');
if (args.build === '1' || !exists(moduleJs)) {
  const build = spawnSync('dotnet', ['publish', resolve(repo, 'THSSS.Browser/THSSS.Browser.csproj'), '-c', 'Release', '-p:ThsssDiagnostic=true', `-p:PublishDir=${publish}/`, '--nologo'], {stdio: 'inherit', cwd: repo});
  if (build.status !== 0) process.exit(build.status ?? 1);
}
if (!exists(moduleJs)) throw Error('Diagnostic publish is missing ' + moduleJs);
const assets = resolve(repo, 'Assets');
const replay = resolve(repo, fixture.source);
const retail = resolve(assets, 'THSSS.exe');
const rngFile = resolve(output, `${fixture.id}.original.rng`);
const server = await serve(publish, replay, exists(rngFile) ? rngFile : '');
const chromium = chromiumPath();
const puppeteerPath = resolve(repo, '../../launcher/node_modules/puppeteer-core/lib/puppeteer/puppeteer-core.js');
const puppeteer = await import(pathToFileURL(puppeteerPath));
const browser = await puppeteer.launch({
  executablePath: chromium,
  headless: true,
  args: ['--no-sandbox', '--disable-dev-shm-usage', '--use-gl=angle', '--use-angle=swiftshader', '--enable-webgl'],
});
try {
  const page = await browser.newPage();
  const errors = [];
  page.on('pageerror', error => errors.push(String(error)));
  page.on('console', message => {
    const text = message.text();
    if (/Exception|Error|fail/i.test(text)) errors.push(text.slice(0, 500));
  });
  const replayName = 'Replay/' + basename(replay);
  const max = process.env.THSSS_VERIFY_MAX || '0';
  const url = `${server.url}/verify.html?replay=${encodeURIComponent(replayName)}&stage=${fixture.stage || 1}&max=${max}`;
  await page.goto(url, {waitUntil: 'domcontentloaded', timeout: 180000});
  await page.waitForFunction(() => document.title === 'thsss-verify-done', {timeout: 300000});
  const outcome = await page.evaluate(() => ({rows: window.__thsssVerifyRows || [], complete: window.__thsssVerifyComplete === true, error: window.__thsssVerifyError || ''}));
  if (outcome.error) throw Error(outcome.error + '\n' + errors.slice(-30).join('\n'));
  if (!outcome.rows.length) throw Error(errors.join('\n') || 'candidate produced no replay frames');
  const identity = comparisonIdentity({assets, replay, executable: retail});
  const end = {record: false, ended: true, complete: outcome.complete, reason: outcome.complete ? 'replay-complete' : 'frame-budget'};
  writeFileSync(resolve(output, `${fixture.id}.candidate.raw.jsonl`), [...outcome.rows, end].map(row => JSON.stringify(row)).join('\n') + '\n');
  writeTrace(resolve(output, `${fixture.id}.candidate.jsonl`), {
    identity,
    provider: 'thsss/diagnostic-wasm',
    provenance: {build: 'ThsssDiagnostic'},
    rows: [...outcome.rows, end],
  });
  console.log(`${fixture.id}.candidate ${outcome.rows.length} ticks`);
} finally {
  await browser.close();
  await server.close();
}

function chromiumPath() {
  if (process.env.THSSS_CHROMIUM && exists(process.env.THSSS_CHROMIUM)) return process.env.THSSS_CHROMIUM;
  for (const name of ['/usr/bin/chromium', '/usr/bin/chromium-browser', '/usr/bin/google-chrome', '/usr/bin/google-chrome-stable']) {
    if (exists(name)) return name;
  }
  const bundled = resolve(process.env.HOME || '', 'Documents/GitHub/eagler/thsss-playwright/.browsers/chromium-1255/chrome-linux64/chrome');
  if (exists(bundled)) return bundled;
  throw Error('Set THSSS_CHROMIUM to a Chrome or Chromium binary');
}

function exists(path) {
  try { return statSync(path).isFile(); }
  catch { return false; }
}

function serve(root, replayFile, rngFile) {
  const types = {'.html': 'text/html', '.js': 'text/javascript', '.mjs': 'text/javascript', '.wasm': 'application/wasm', '.json': 'application/json', '.rng': 'text/plain'};
  const harness = resolve(here, 'harness.html');
  const server = createServer((request, response) => {
    const url = new URL(request.url, 'http://127.0.0.1');
    const path = url.pathname === '/verify.html'
      ? harness
      : url.pathname === '/fixture.rpy'
        ? replayFile
      : url.pathname === '/rng-tape'
        ? rngFile
      : resolve(root, decodeURIComponent(url.pathname).replace(/^\/+/, ''));
    if (!path || (path !== replayFile && path !== rngFile && path !== harness && !path.startsWith(root + '/') && path !== resolve(root))) {
      response.writeHead(403); response.end(); return;
    }
    try {
      const body = readFileSync(path);
      response.writeHead(200, {'content-type': types[extname(path)] || 'application/octet-stream', 'cross-origin-opener-policy': 'same-origin', 'cross-origin-embedder-policy': 'require-corp'});
      response.end(body);
    } catch {
      response.writeHead(404); response.end();
    }
  });
  return new Promise(accept => {
    server.listen(0, '127.0.0.1', () => {
      const address = server.address();
      accept({url: `http://127.0.0.1:${address.port}`, close: () => new Promise(done => server.close(done))});
    });
  });
}

function options(argv) {
  const out = {};
  for (let i = 0; i < argv.length; i += 2) {
    if (!argv[i]?.startsWith('--')) continue;
    out[argv[i].slice(2)] = argv[i + 1];
  }
  if (!out.case) throw Error('--case is required');
  return out;
}
