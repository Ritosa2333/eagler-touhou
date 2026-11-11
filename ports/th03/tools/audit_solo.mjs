import assert from 'node:assert/strict';
import {readFile} from 'node:fs/promises';
import {gunzipSync} from 'node:zlib';
import {Fat12,sha256} from '../web/disk.js';
import {createSoloAssist} from '../web/solo-assist.js';
import {prepareSoloDisk} from '../web/solo-save.js';
import {decodeScoreSection} from '../web/scores.js';

const root=new URL('../web/',import.meta.url);
globalThis.fetch=async path=>{const bytes=await readFile(new URL(path,root));return new Response(bytes);};
const helper=await createSoloAssist(()=>null);
const inventory=JSON.parse(await readFile(new URL('../reports/asset-inventory.json',import.meta.url)));
for(const lang of ['jp','cn']){
  const original=new Uint8Array(gunzipSync(await readFile(new URL(`disks/3-${lang}.hdi.gz`,root))));
  const originalFat=new Fat12(original),patched=await helper.install(original.slice()),fat=new Fat12(patched);
  const main=originalFat.find('YUMEZIKU/MAIN.EXE');
  const allowed=new Uint8Array(original.length);
  allowed.fill(1,fat.fat,fat.fat+fat.fatCount*fat.fatSize);
  allowed.fill(1,main,main+32);
  for(const cluster of fat.chain(fat.view.getUint16(main+26,true)))allowed.fill(1,fat.offset(cluster),fat.offset(cluster)+fat.clusterSize);
  for(let at=0;at<original.length;at++)if(original[at]!==patched[at])assert(allowed[at],`Unexpected disk edit at ${at}`);
  for(const {path} of inventory[lang].filter(({path})=>/^[\x00-\x7f]+$/.test(path)))if(path!=='YUMEZIKU/MAIN.EXE')assert.deepEqual(fat.read(fat.find(path)),originalFat.read(originalFat.find(path)),path);
  const restored=new Fat12(helper.exportDisk(patched));
  for(const {path} of inventory[lang].filter(({path})=>/^[\x00-\x7f]+$/.test(path)))assert.deepEqual(restored.read(restored.find(path)),originalFat.read(originalFat.find(path)),path);
  assert.equal(await sha256(restored.read(main)),helper.bridge.meta.originalSha256);
  assert.notEqual(await sha256(fat.read(main)),helper.bridge.meta.originalSha256,'Export must not modify the running disk');
  await helper.install(restored.data.slice());
  console.log(`PASS ${lang}: only MAIN/FAT allocation changed; all boot/config/score/music files preserved; export restores original files and reimports`);
  const upgraded=prepareSoloDisk(original.slice()),saveFat=new Fat12(upgraded);
  const score=originalFat.find('YUMEZIKU/YUME.NEM'),cfg=originalFat.find('YUMEZIKU/YUME.CFG');
  const configOffset=originalFat.offset(originalFat.view.getUint16(cfg+26,true));
  assert.equal(saveFat.read(cfg)[2],3,'Solo defaults to Lunatic');
  const originalScore=originalFat.read(score),unlockedScore=saveFat.read(score);
  for(let rank=0;rank<4;rank++){
    const before=decodeScoreSection(originalScore.subarray(rank*206,(rank+1)*206));
    const after=decodeScoreSection(unlockedScore.subarray(rank*206,(rank+1)*206));
    assert.equal(after[82],99,'Every difficulty is fully unlocked');
    after[0]=before[0];after[1]=before[1];after[82]=before[82];
    assert.deepEqual(after,before,'Names, scores, characters, stages and keys are preserved');
  }
  const normalized=upgraded.slice();normalized[configOffset+2]=original[configOffset+2];
  originalFat.chain(originalFat.view.getUint16(score+26,true)).forEach((cluster,i)=>{
    const offset=originalFat.offset(cluster),count=Math.min(originalFat.clusterSize,originalScore.length-i*originalFat.clusterSize);
    if(count>0)normalized.set(original.subarray(offset,offset+count),offset);
  });
  assert.deepEqual(normalized,original,'Only difficulty and existing score bytes change');
  assert.deepEqual(prepareSoloDisk(upgraded.slice()),upgraded,'Saved/imported disk upgrade is idempotent');
  const running=await helper.install(upgraded.slice()),exported=helper.exportDisk(running);
  const exportFat=new Fat12(exported);
  assert.deepEqual(exportFat.read(score),unlockedScore,'Export retains full unlock');
  assert.equal(exportFat.read(cfg)[2],3);
  assert.equal(await sha256(exportFat.read(main)),helper.bridge.meta.originalSha256);
  console.log(`PASS ${lang}: solo originals/saves/imports default Lunatic, all four ranks unlocked, exact leaderboard/disk preservation, compatible export`);
}
