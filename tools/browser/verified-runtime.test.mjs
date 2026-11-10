import assert from 'node:assert/strict';
import {createServer} from 'node:http';
import test from 'node:test';
import {launchBrowser} from './launch.mjs';
import {verifyRuntimeDelivery} from './verified-runtime.mjs';

test('receipts identify the script and WASM actually executed after a cross-origin redirect', async t => {
  const wasm = Uint8Array.from([0,97,115,109,1,0,0,0,1,5,1,96,0,1,127,3,2,1,0,
    7,7,1,3,97,110,115,0,0,10,6,1,4,0,65,42,11]);
  const script = Buffer.from(`export async function run(){
    const {instance}=await WebAssembly.instantiateStreaming(fetch(new URL('th09.wasm',import.meta.url)));
    return instance.exports.ans();
  }`);
  let mode = 'matching';
  const serve = async handler => {
    const server = createServer((req, res) => {
      res.setHeader('Access-Control-Allow-Origin', '*');
      res.setHeader('Cache-Control', 'no-store');
      handler(req, res);
    });
    await new Promise(resolve => server.listen(0, '127.0.0.1', resolve));
    t.after(() => new Promise(resolve => { server.close(resolve); server.closeAllConnections(); }));
    return `http://127.0.0.1:${server.address().port}`;
  };
  const remote = await serve((req, res) => {
    if (req.url === '/runtime/th09.mjs') {
      res.setHeader('Content-Type', 'text/javascript');
      res.end(mode === 'changed-script' ? Buffer.concat([script, Buffer.from('\n// changed')]) : script);
    } else if (req.url === '/runtime/th09.wasm') {
      res.setHeader('Content-Type', 'application/wasm');
      const bytes = wasm.slice();
      if (mode === 'changed-wasm') bytes[bytes.length - 2] = 43;
      res.end(bytes);
    } else res.writeHead(404).end();
  });
  const local = await serve((req, res) => {
    if (req.url === '/probe.mjs') res.writeHead(302, {location: remote + '/runtime/th09.mjs'}).end();
    else {
      res.setHeader('Content-Type', 'text/html');
      res.end(`<script type="module">const module=await import('/probe.mjs');
        if(!location.search)window.answer=await module.run();window.ready=true;</script>`);
    }
  });
  const browser = await launchBrowser();
  t.after(() => browser.close());
  for (const scenario of ['matching', 'changed-wasm', 'changed-script', 'missing-wasm']) {
    await t.test(scenario, async () => {
      mode = scenario;
      const page = await browser.newPage();
      try {
        const delivery = await verifyRuntimeDelivery(page, {
          scriptUrl: local + '/probe.mjs', scriptBytes: script, wasmName: 'th09.wasm', wasmBytes: wasm,
        });
        await page.goto(local + (scenario === 'missing-wasm' ? '/?skip=1' : '/'));
        await page.waitForFunction(() => window.ready === true);
        if (scenario === 'matching') {
          await delivery.assertComplete();
          assert.equal(await page.evaluate(() => window.answer), 42);
          assert.deepEqual(delivery.receipts.map(receipt => receipt.url).sort(),
            [remote + '/runtime/th09.mjs', remote + '/runtime/th09.wasm'].sort());
        } else if (scenario === 'changed-wasm') {
          assert.equal(await page.evaluate(() => window.answer), 43, 'the mismatched binary really executed');
          await assert.rejects(delivery.assertComplete(), /wasm response does not match/);
        } else if (scenario === 'changed-script') {
          assert.equal(await page.evaluate(() => window.answer), 42);
          await assert.rejects(delivery.assertComplete(), /script response does not match/);
        } else await assert.rejects(delivery.assertComplete(), /Missing verified wasm response/);
      } finally { await page.close(); }
    });
  }
});
