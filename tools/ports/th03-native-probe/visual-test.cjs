const { readdir, readFile } = require('node:fs/promises');
const { join, resolve } = require('node:path');
const assert = require('node:assert/strict');
const create = require(resolve(process.argv[2]));
const root = resolve(process.argv[3]);
(async () => {
  const m = await create();
  m.FS.mkdir('/content');
  async function copy(dir, target) {
    for (const entry of await readdir(dir, { withFileTypes: true })) {
      const dst = `${target}/${entry.name}`;
      if (entry.isDirectory()) { m.FS.mkdir(dst); await copy(join(dir, entry.name), dst); }
      else m.FS.writeFile(dst, await readFile(join(dir, entry.name)));
    }
  }
  await copy(root, '/content');
  const created = m._th03_visual_create();
  assert.equal(created, 1, `visual create: ${created}`);
  const pointer = m._th03_visual_pixels();
  assert.ok(pointer, 'initial surface');
  for (let i = 0; i < 120; i++) assert.equal(m._th03_visual_step(0x20, 0), 0, `tick ${i}`);
  assert.ok(m._th03_visual_pixels(), 'advanced surface');
  assert.ok(m.HEAPU8.subarray(pointer, pointer + 640 * 400 * 4).some(value => value !== 0));
  m._th03_visual_destroy();
  console.log('TH03 native visual: resources, 120 ticks and surfaces passed');
})().catch(error => { console.error(error); process.exitCode = 1; });
