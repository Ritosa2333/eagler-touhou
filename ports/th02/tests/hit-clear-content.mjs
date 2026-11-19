import assert from 'node:assert/strict';
import {readFile} from 'node:fs/promises';
import {gunzipSync} from 'node:zlib';
import {createHash} from 'node:crypto';
import {ManagedContent} from '../web/managed-data.mjs';
import {Fat12} from '../build-eagler/solo-fat12.js';

const patch=JSON.parse(await readFile(new URL('../build-eagler/local-patch.json',import.meta.url)));
assert.equal(patch.hitClear?.version,1);
assert.equal(patch.hitClear.testInvincibility,true);
assert.equal(patch.nativeBossHud.implementation,'guest-ems');
assert.deepEqual(patch.hitClear.pools,{bullets:150,lasers:12});
const content=await ManagedContent.open(await readFile(new URL('../content/th02.data',import.meta.url)));
const base=patch.headerSize+patch.codeSegment*16;
for(const language of ['jp','cn']){
  const spec=patch.disks[language];
  const response=await content.fetch(spec.file);assert.equal(response.status,200);
  const disk=gunzipSync(Buffer.from(await response.arrayBuffer()));
  assert.equal(createHash('sha256').update(disk).digest('hex'),spec.sha256);
  const fat=new Fat12(new Uint8Array(disk)),exe=fat.read(fat.find('HUMAROKU/MAIN.EXE'));
  assert.equal(createHash('sha256').update(exe).digest('hex'),spec.mainSha256);
  for(const edit of patch.hitClear.edits[language])
    assert.equal(Buffer.from(exe.subarray(base+edit.codeOffset,base+edit.codeOffset+edit.replacement.length/2)).toString('hex'),edit.replacement);
  const bank=JSON.parse(await readFile(new URL('../build-eagler/netplay/bullet-bank-patch.json',import.meta.url)));
  assert.equal(bank.sourceMainSha256[language],spec.mainSha256);
  assert(bank.bytes<=bank.limit-bank.target);
}
console.log('PASS managed TH02 JP/CN content contains native hit clear, matching private collision patches and EMS HUD');
