const assert = require('node:assert/strict');
const createModule = require(process.argv[2]);

(async () => {
  const wasm = await createModule();
  assert.equal(wasm._th03_restore(), 0);
  assert.equal(wasm._th03_create(34, 1, 18), 1);
  for (let i = 0; i < 48; i++) assert.equal(wasm._th03_step(i & 1 ? 0x41 : 0x82, 0x104), 0);
  assert.equal(wasm._th03_save(), 1);
  const frame = wasm._th03_frame();
  const hash = wasm._th03_hash();
  for (let i = 0; i < 30; i++) assert.equal(wasm._th03_step(0x41, 0x104), 0);
  const advancedFrame = wasm._th03_frame();
  const advancedHash = wasm._th03_hash();
  assert.equal(wasm._th03_restore(), 1);
  assert.equal(wasm._th03_frame(), frame);
  assert.equal(wasm._th03_hash(), hash);
  for (let i = 0; i < 30; i++) assert.equal(wasm._th03_step(0x41, 0x104), 0);
  assert.equal(wasm._th03_frame(), advancedFrame);
  assert.equal(wasm._th03_hash(), advancedHash);
  wasm._th03_destroy();
  assert.equal(wasm._th03_step(0, 0), -1);
  console.log('TH03 WASM bridge: deterministic rollback passed');
})().catch(error => { console.error(error); process.exitCode = 1; });
