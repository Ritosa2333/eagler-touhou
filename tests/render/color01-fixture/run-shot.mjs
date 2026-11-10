// Headless WebGL runner for this fixture directory. Run through
// `make shot CMD='node eagler-touhou/ports/th13/tests/render/
// color01-fixture/run-shot.mjs'`.
import {createRequire} from 'node:module';
import {createServer} from 'node:http';
import {readFile, writeFile} from 'node:fs/promises';
import {dirname, join} from 'node:path';
import {fileURLToPath} from 'node:url';

const outDir = join(dirname(fileURLToPath(import.meta.url)), 'build');
const resultPath = join(outDir, 'result.json');
const MIME = {
  '.html': 'text/html; charset=utf-8',
  '.mjs': 'text/javascript',
  '.js': 'text/javascript',
  '.wasm': 'application/wasm',
  '.json': 'application/json',
};

const server = createServer(async (req, res) => {
  try {
    const urlPath = decodeURIComponent(new URL(req.url, 'http://localhost').pathname);
    if (urlPath === '/favicon.ico') {
      res.writeHead(204).end();
      return;
    }
    const name = urlPath === '/' ? 'fixture.html' : urlPath.replace(/^\//, '');
    if (name.includes('..') || !/^[A-Za-z0-9._-]+$/.test(name)) throw Error('invalid path');
    const data = await readFile(join(outDir, name));
    const ext = '.' + name.split('.').pop();
    res.writeHead(200, {'Content-Type': MIME[ext] || 'application/octet-stream'}).end(data);
  } catch (error) {
    res.writeHead(404).end(String(error));
  }
});
await new Promise(resolveListen => server.listen(0, '127.0.0.1', resolveListen));
const url = `http://127.0.0.1:${server.address().port}/fixture.html`;

const require = createRequire(import.meta.url);
let chromium;
try {
  ({chromium} = require('playwright-core'));
} catch {
  ({chromium} = require(join(process.env.NODE_PATH ?? '/opt/shot/node_modules', 'playwright-core')));
}

const browser = await chromium.launch({
  executablePath: process.env.CHROMIUM_PATH,
  headless: true,
  args: ['--no-sandbox', '--disable-dev-shm-usage', '--use-angle=swiftshader',
         '--enable-unsafe-swiftshader', '--force-color-profile=srgb'],
});
const page = await browser.newPage({viewport: {width: 640, height: 480}});
page.setDefaultTimeout(60000);
const pageErrors = [];
page.on('pageerror', error => pageErrors.push(String(error)));
page.on('console', message => {
  if (message.type() === 'error') pageErrors.push('console: ' + message.text());
});

let exitCode = 2;
try {
  await page.goto(url);
  await page.waitForFunction(() => window.color01Ready === true, {timeout: 60000});
  const error = await page.evaluate(() => window.color01Error ?? null);
  const text = await page.evaluate(() => window.color01Result ?? null);
  if (error) throw Error(error);
  if (!text) throw Error('fixture returned no result');
  const result = JSON.parse(text);
  result.pageErrors = pageErrors;
  await writeFile(resultPath, JSON.stringify(result, null, 2) + '\n');
  console.log(JSON.stringify(result, null, 2));
  exitCode = result.pass && pageErrors.length === 0 ? 0 : 1;
} catch (error) {
  console.error('COLOR-01 fixture failed:', error);
  if (pageErrors.length) console.error(pageErrors.join('\n'));
  exitCode = 2;
} finally {
  await browser.close();
  server.close();
}
process.exit(exitCode);
