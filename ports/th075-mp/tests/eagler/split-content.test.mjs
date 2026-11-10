import test from 'node:test';
import assert from 'node:assert/strict';
import {createHash} from 'node:crypto';
import {mkdtemp, mkdir, readFile, rm, writeFile} from 'node:fs/promises';
import {tmpdir} from 'node:os';
import {join, basename} from 'node:path';
import {execFileSync} from 'node:child_process';
import {fileURLToPath} from 'node:url';
import {createServer} from 'node:http';
import {CONTENT_SCHEMA} from '../../portable/content-bundle.mjs';
import {openSplitContent} from '../../portable/split-content.mjs';

const hash = bytes => createHash('sha256').update(bytes).digest('hex');

test('TH075 verifies redirected download blobs and rejects redirected indexes', async t => {
  const font = '/fonts/original-glyphs.bin';
  const catalog = '/music/catalog.bin';
  const payload = Buffer.from('font');
  const files = [{path: font, bytes: payload.length, sha256: hash(payload)},
    {path: catalog, bytes: 1, sha256: hash('x')}];
  const index = JSON.stringify({schema: 'eagler-touhou/th075-multiplayer-assets/1', files});
  const indexHash = hash(index), indexPath = '/' + indexHash + '.json';
  let corrupt = false, redirectIndex = false;
  async function serve(handler) {
    const server = createServer(handler);
    await new Promise(resolve => server.listen(0, '127.0.0.1', resolve));
    t.after(() => new Promise(resolve => server.close(resolve)));
    return 'http://127.0.0.1:' + server.address().port;
  }
  const downloads = await serve((req, res) => {
    res.end(req.url === indexPath ? index : corrupt ? 'fake' : payload);
  });
  const launcher = await serve((req, res) => {
    if (req.url === indexPath && !redirectIndex) return res.end(index);
    res.writeHead(307, {Location: downloads + req.url}).end();
  });
  const content = await openSplitContent(launcher + indexPath, indexHash);
  assert.deepEqual(Buffer.from(await content.read(font)), payload);
  corrupt = true;
  await assert.rejects(content.read(font), /Corrupt TH075 multiplayer asset/);
  redirectIndex = true;
  await assert.rejects(openSplitContent(launcher + indexPath, indexHash), TypeError);
});

test('TH075 multiplayer downloads only resources requested by the Runtime', async () => {
  const root = await mkdtemp(join(tmpdir(), 'th075-split-'));
  try {
    const output = join(root, 'output');
    await mkdir(join(root, 'music'));
    const font = '/fonts/original-glyphs.bin';
    const catalog = '/music/catalog.bin';
    const shared = '/resources/base/data/system/battle.dat.z';
    const reimu = '/resources/base/data/character/00.dat.z';
    const marisa = '/resources/base/data/character/01.dat.z';
    const unused = '/resources/base/data/character/02.dat.z';
    const stage = '/resources/base/data/background/BG00a.dat.z';
    const story = '/resources/base/data/character/reimu/stage1.dat.z';
    const tracks = ['00a.ogg', '00b.ogg', 'sys99_ed.ogg'];
    const blobs = [font, catalog, shared, reimu, marisa, unused, stage, story].map(path => Buffer.from(path));
    let offset = 0;
    const files = blobs.map(bytes => {
      const file = {path: bytes.toString(), offset, bytes: bytes.length, sha256: hash(bytes)};
      offset += bytes.length;
      return file;
    });
    const index = Buffer.from(JSON.stringify({schema: CONTENT_SCHEMA, files}));
    const header = Buffer.alloc(12);
    header.write('E75DATA1');
    header.writeUInt32LE(index.length, 8);
    await writeFile(join(root, 'th075.data'), Buffer.concat([header, index, ...blobs]));
    for (const name of tracks) await writeFile(join(root, 'music', name), '/music/' + name);
    const script = fileURLToPath(new URL('../../portable/prepare-split-content.mjs', import.meta.url));
    const release = JSON.parse(execFileSync(process.execPath, [script, '--prepared=' + root, '--output=' + output]));
    const requests = [];
    let corrupt = false;
    const fetchAsset = async url => {
      const file = basename(url.pathname);
      requests.push(file);
      if (corrupt) return new Response('tampered');
      return new Response(await readFile(join(output, file)));
    };
    const content = await openSplitContent('https://example.test/assets/' + release.manifest, release.sha256, fetchAsset);
    const assetName = path => hash(path) + '.bin';
    assert(!content.paths.includes(story));
    assert(!content.paths.includes('/music/sys99_ed.ogg'));
    assert.deepEqual(requests, [release.manifest]);

    for (const path of [font, catalog, shared]) assert.equal(Buffer.from(await content.read(path)).toString(), path);
    assert.deepEqual(requests, [release.manifest, ...[font, catalog, shared].map(assetName)]);
    const match = [reimu, marisa, stage, '/music/00a.ogg'];
    await Promise.all([...match, reimu].map(async path => {
      assert.equal(Buffer.from(await content.read(path)).toString(), path);
    }));
    assert.deepEqual(requests.slice(4), match.map(assetName));
    assert(!requests.includes(assetName(unused)));
    assert(!requests.includes(assetName('/music/00b.ogg')));
    await assert.rejects(content.read(story), /not declared/);
    corrupt = true;
    await assert.rejects(content.read(reimu), /Corrupt/);
    corrupt = false;
    assert.equal(Buffer.from(await content.read(reimu)).toString(), reimu);
  } finally {
    await rm(root, {recursive: true, force: true});
  }
});
