import assert from 'node:assert/strict';
import {createHash} from 'node:crypto';
import {cp, mkdir, mkdtemp, readFile, rm, writeFile} from 'node:fs/promises';
import {tmpdir} from 'node:os';
import {join} from 'node:path';
import {pathToFileURL} from 'node:url';
import {gzipSync, gunzipSync} from 'node:zlib';
import {test} from 'node:test';
import {exportPC98Disks} from '../../tools/build/pc98-disks.mjs';
import {publishPC98Blocks} from '../lib/pc98-static-publication.mjs';
import {pc98RequiredBlockFiles} from '../package/pc98-block-selection.mjs';
import {spawnSync} from 'node:child_process';

test('selected published blocks restore a disk through the managed fetch layer', async () => {
  const root = await mkdtemp(join(tmpdir(), 'pc98-runtime-')), urls = [];
  try {
    const paths = ['disks/3-cn.hdi.gz', 'disks/3-jp.hdi.gz', 'common.bin'];
    const payload = paths.map(path => path.endsWith('.gz') ? gzipSync(Buffer.from(path)) : Buffer.from(path));
    let offset = 0;
    const entries = paths.map((path, i) => {const bytes = payload[i], entry = {path, offset, bytes: bytes.length,
      sha256: createHash('sha256').update(bytes).digest('hex')}; offset += bytes.length; return entry;});
    const index = Buffer.from(JSON.stringify({schema:'eagler-touhou/pc98-content/1',game:'th03',files:entries}));
    const header = Buffer.alloc(12); header.write('EPC98PK1'); header.writeUInt32LE(index.length, 8);
    const data = join(root, 'source.data'); await writeFile(data, Buffer.concat([header,index,...payload]));
    await exportPC98Disks({game:'th03',output:root,entries,payload});
    const files = {}, components = {}, staging = join(root, 'site');
    const descriptorFile = async (source,target) => {const bytes = await readFile(join(staging,source));
      return {source,target,bytes:bytes.length,revision:'1',sha256:createHash('sha256').update(bytes).digest('hex')};};
    await publishPC98Blocks({game:'th03',source:root,data,staging,descriptorFile,packageFiles:files,components});
    files.data = {source:'games/th03/th03.data',target:'/th03.data',revision:'1'};
    const descriptor = {schema:'eagler-touhou/package/1',game:'th03',revision:'1',files,components,
      runtimeRequirement:{protocol:'eagler-touhou/1',target:'th03',dataFile:'data'},base:{files:['data']}};
    await cp(new URL('../../ports/th02/web/managed-data.mjs',import.meta.url),join(root,'managed-data.mjs'));
    await cp(new URL('../../ports/th02/web/preload-cache.js',import.meta.url),join(root,'preload-cache.js'));
    await mkdir(join(root,'netplay'));
    await cp(new URL('../../ports/th02/web/netplay/sha256.js',import.meta.url),join(root,'netplay/sha256.js'));
    await cp(new URL('../lib/pc98-disk-blocks.mjs',import.meta.url),join(root,'pc98-disk-blocks.mjs'));
    const {ManagedContent} = await import(pathToFileURL(join(root,'managed-data.mjs')).href);
    const content = await ManagedContent.open(await readFile(join(staging,'games/th03/th03.data')),'th03');
    for (const id of pc98RequiredBlockFiles(descriptor,{game:'th03',language:'ja',multiplayer:false})) {
      const file = files[id], blob = new Blob([await readFile(join(staging,file.source))]);
      const url = URL.createObjectURL(blob); urls.push(url);
      await content.addResources([{path:file.target,url,bytes:file.bytes,sha256:file.sha256}]);
    }
    const environment = {location:{href:'https://local/game/'},fetch:globalThis.fetch};
    content.install(environment,'https://local/game/');
    const response = await environment.fetch('https://local/game/disks/3-jp.hdi.gz');
    assert.equal(response.status,200);
    assert.deepEqual(gunzipSync(Buffer.from(await response.arrayBuffer())),Buffer.from(paths[1]));
    await assert.rejects(environment.fetch('https://local/game/disks/3-cn.hdi.gz'),/not installed/);
    await writeFile(join(staging,'th03.package.json'),JSON.stringify(descriptor));
    const zip=join(root,'offline.zip'),pack=spawnSync(process.execPath,[new URL('../scripts/package-offline-game.mjs',import.meta.url).pathname.replace(/^\/([A-Za-z]:)/,'$1'),staging,'th03',zip],{encoding:'utf8'});
    assert.equal(pack.status,0,pack.stderr);
    const {parsePackageZip}=await import('../package/package-zip.mjs');
    const imported=await parsePackageZip(new Blob([await readFile(zip)]));
    assert.equal(imported.files.size,Object.keys(descriptor.files).length);
  } finally { for (const url of urls) URL.revokeObjectURL(url); await rm(root,{recursive:true,force:true}); }
});
