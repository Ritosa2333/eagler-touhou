const { createServer } = require('node:http');
const { readdir, readFile } = require('node:fs/promises');
const { spawn } = require('node:child_process');
const { join, resolve } = require('node:path');
const build = resolve(process.argv[2]);
const content = resolve(process.argv[3]);
const chrome = process.argv[4];
if (!chrome) throw Error('Usage: node visual-browser-test.cjs <build-directory> <content-directory> <chrome>');
async function list(dir, prefix = '') {
  const result = [];
  for (const e of await readdir(dir, { withFileTypes: true })) {
    const path = prefix ? `${prefix}/${e.name}` : e.name;
    if (e.isDirectory()) result.push(...await list(join(dir, e.name), path));
    else if (e.isFile()) result.push(path);
  }
  return result;
}
(async () => {
  const files = await list(content);
  const allowed = new Set(files);
  const html = `<!doctype html><canvas width="640" height="400"></canvas><div id="result">pending</div><script src="/visual.js"></script><script>
  (async () => {
    const m = await createTh03Visual(); m.FS.mkdir('/content');
    for (const path of ${JSON.stringify(files)}) {
      const parts = path.split('/'); let dir = '/content';
      for (const part of parts.slice(0, -1)) { dir += '/' + part; if (!m.FS.analyzePath(dir).exists) m.FS.mkdir(dir); }
      const response = await fetch('/content/' + path.split('/').map(encodeURIComponent).join('/'));
      if (!response.ok) throw Error('resource ' + path);
      m.FS.writeFile('/content/' + path, new Uint8Array(await response.arrayBuffer()));
    }
    const result = m._th03_visual_create(); if (result !== 1) throw Error('create ' + result);
    for (let i=0; i<120; i++) if (m._th03_visual_step(0x20, 0) !== 0) throw Error('step ' + i);
    const pointer = m._th03_visual_pixels(); if (!pointer) throw Error('pixels');
    const pixels = new Uint8ClampedArray(m.HEAPU8.slice(pointer, pointer + 640*400*4));
    document.querySelector('canvas').getContext('2d').putImageData(new ImageData(pixels,640,400),0,0);
    if (!pixels.some((value, i) => i % 4 !== 3 && value !== 0)) throw Error('blank surface');
    document.querySelector('#result').textContent = 'TH03_VISUAL_PASS';
  })().catch(e => document.querySelector('#result').textContent = 'TH03_VISUAL_FAIL ' + e);
  </script>`;
  const server = createServer(async (req, res) => {
    try {
      if (req.url === '/') return res.end(html);
      if (req.url === '/visual.js' || req.url === '/visual.wasm') {
        res.setHeader('Content-Type', req.url.endsWith('.wasm') ? 'application/wasm' : 'text/javascript');
        return res.end(await readFile(join(build, req.url.slice(1))));
      }
      const path = decodeURIComponent(req.url.slice('/content/'.length));
      if (!req.url.startsWith('/content/') || !allowed.has(path)) return res.writeHead(404).end();
      res.end(await readFile(join(content, path)));
    } catch { res.writeHead(500).end(); }
  });
  server.listen(0, '127.0.0.1', () => {
    const browser = spawn(chrome, ['--headless', '--disable-gpu', '--no-first-run', '--virtual-time-budget=30000', '--dump-dom', `http://127.0.0.1:${server.address().port}/`]);
    let output = '', errors = '';
    browser.stdout.on('data', c => output += c); browser.stderr.on('data', c => errors += c);
    const timer = setTimeout(() => browser.kill(), 60000);
    browser.on('error', e => { console.error(e); clearTimeout(timer); server.close(); process.exitCode = 1; });
    browser.on('close', code => {
      clearTimeout(timer); server.close();
      if (code !== 0 || !output.includes('TH03_VISUAL_PASS')) { console.error('Browser visual test failed', code, output, errors); process.exitCode = 1; }
      else console.log('TH03 native visual: browser resources and canvas passed');
    });
  });
})().catch(e => { console.error(e); process.exitCode = 1; });
