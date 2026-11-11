const { readdir, readFile } = require('node:fs/promises');
const { join, resolve } = require('node:path');
const assert = require('node:assert/strict');
const create = require(resolve(process.argv[2]));
const root = resolve(process.argv[3]);
const ui = resolve(process.argv[4]);
(async () => {
  const m = await create(); m.FS.mkdir('/content'); m.FS.mkdir('/user'); m.FS.mkdir('/user/REPLAY');
  async function copy(dir, target) {
    for (const entry of await readdir(dir, {withFileTypes:true})) {
      const dst = `${target}/${entry.name}`;
      if (entry.isDirectory()) {m.FS.mkdir(dst); await copy(join(dir,entry.name),dst);}
      else if (entry.isFile()) m.FS.writeFile(dst,await readFile(join(dir,entry.name)));
    }
  }
  await copy(root,'/content'); m.FS.mkdir('/content/PODDUI'); await copy(ui,'/content/PODDUI');
  const created = m._th03_classic_create(); assert.equal(created,1,`New Classic create: ${created}`);
  assert.ok(m._th03_classic_pixels());
  let frames = 0, sound = false, point = false;
  for (let i=0; i<900; i++) {
    const input = i % 60 < 5 ? 0x20 : 0;
    assert.equal(m._th03_classic_step(input,input,0),0,`tick ${i}`);
    if (m._th03_classic_point_x() >= 0 && m._th03_classic_point_y() >= 0) point = true;
    const count=m._th03_classic_audio_frames(); frames+=count;
    const pointer=m._th03_classic_audio() >>> 2;
    if (m.HEAP32.subarray(pointer,pointer+count*2).some(value => value !== 0)) sound=true;
  }
  assert.ok(frames>0,'audio frames'); assert.ok(sound,'nonzero audio'); assert.ok(m._th03_classic_pixels());
  assert.ok(point, 'player position after entering a match');
  m._th03_classic_destroy(); console.log('New Classic: match, player position, video and audio passed');
})().catch(error=>{console.error(error);process.exitCode=1;});
