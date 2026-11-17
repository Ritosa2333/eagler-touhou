const { createServer } = require('node:http');
const { readFile } = require('node:fs/promises');
const { spawn } = require('node:child_process');
const { join } = require('node:path');
const directory = process.argv[2];
const chrome = process.argv[3];
if (!directory || !chrome) throw Error('Usage: node browser-test.cjs <build-directory> <chrome>');
const html = `<!doctype html><title>TH03 WASM test</title><body>pending<script src="/th03-core.js"></script><script>
createTh03Core().then(m => {
  if (!m._th03_create(34, 1, 18)) throw Error('create');
  for (let i = 0; i < 48; i++) if (m._th03_step(0x41, 0x104) !== 0) throw Error('step');
  if (!m._th03_save()) throw Error('save');
  const before = m._th03_hash();
  m._th03_step(0x82, 0x104);
  if (!m._th03_restore() || m._th03_hash() !== before) throw Error('restore');
  document.body.textContent = 'TH03_BROWSER_PASS';
}).catch(e => { document.body.textContent = 'TH03_BROWSER_FAIL ' + e; });
</script>`;
const server = createServer(async (req, res) => {
  try {
    if (req.url === '/') return res.end(html);
    if (!['/th03-core.js', '/th03-core.wasm'].includes(req.url)) { res.writeHead(404).end(); return; }
    res.setHeader('Content-Type', req.url.endsWith('.wasm') ? 'application/wasm' : 'text/javascript');
    res.end(await readFile(join(directory, req.url.slice(1))));
  } catch { res.writeHead(500).end(); }
});
server.listen(0, '127.0.0.1', () => {
  const browser = spawn(chrome, ['--headless', '--disable-gpu', '--no-first-run', '--virtual-time-budget=15000', '--dump-dom', `http://127.0.0.1:${server.address().port}/`]);
  let output = '', errors = '';
  browser.stdout.on('data', chunk => output += chunk);
  browser.stderr.on('data', chunk => errors += chunk);
  const timer = setTimeout(() => browser.kill(), 30000);
  browser.on('error', error => { console.error(error); clearTimeout(timer); server.close(); process.exitCode = 1; });
  browser.on('close', code => {
    clearTimeout(timer); server.close();
    if (code !== 0 || !output.includes('TH03_BROWSER_PASS')) {
      console.error('Browser WASM test failed', code, output, errors); process.exitCode = 1;
    } else console.log('TH03 WASM bridge: browser execution passed');
  });
});
