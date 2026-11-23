import assert from 'node:assert/strict';
import {createHash} from 'node:crypto';
import {readFile} from 'node:fs/promises';
import {test} from 'node:test';
import {makeCoopRules,coopWire,coopFingerprint,checkCoopAgreement} from '../runtime/coop-rules.mjs';
import {createConfirmedHistory,historyRules,readMultiplayerReplay} from '../runtime/confirmed-history.mjs';
const remaster={randomCirnoPalette:true,cirnoPaletteSet:'remastered-6'};
const base={schema:'th128-coop/1',mode:'same-stage-coop',matchId:'cirno-palettes',epoch:1,route:'A',players:3,difficulty:1,seed:7854,
  inputDelay:0,rollbackWindow:2,challengeMode:false,coreSHA256:'c'.repeat(64),resourceSHA256:'d'.repeat(64)};

async function verifyMatch(players){
  const w={...base,players},before=makeCoopRules(w),off=makeCoopRules({...w,randomCirnoPalette:false});
  assert.deepEqual(coopWire(off),coopWire(before),'the OFF setting does not rewrite old replay rule bytes');
  assert.equal(await coopFingerprint(off),await coopFingerprint(before));
  const on=makeCoopRules({...w,...remaster}),id=await coopFingerprint(on);
  assert.equal(coopWire(on).randomCirnoPalette,true);assert.equal(coopWire(on).cirnoPaletteSet,'remastered-6');
  assert.notEqual(id,await coopFingerprint(off));
  const oldPaletteTape=makeCoopRules({...w,randomCirnoPalette:true});
  const oldDigest=await coopFingerprint(oldPaletteTape);
  assert.notEqual(oldDigest,id,'a previous four-palette match cannot use this generation by accident');
  await assert.rejects(checkCoopAgreement(on,coopWire(oldPaletteTape),oldDigest),/differ/);
  await assert.rejects(checkCoopAgreement(off,coopWire(on),id),/differ/);
  const tracks=await createConfirmedHistory({rules:on,allowedMask:763});
  tracks.append({frame:0,inputs:Array.from({length:players},(_,player)=>({player,held:0,pressed:0,released:0,predicted:false}))});tracks.seal('left');
  const decoded=await readMultiplayerReplay(tracks.replayBytes(),w);
  const {chooseCirnoPalettes}=await import('../runtime/coop-cirno-palettes.mjs');
  assert.deepEqual(chooseCirnoPalettes(historyRules(decoded.header.rules)),chooseCirnoPalettes(on),'replay does not resample');
  assert.deepEqual(chooseCirnoPalettes(off),Array(players).fill(0));
  assert.throws(()=>chooseCirnoPalettes(oldPaletteTape),/旧版染色/,'do not silently change an old colored replay');
  const colors=Array.from({length:players-1},()=>new Set());
  for(let seed=0;seed<256;seed++){
    const rules=makeCoopRules({...w,seed,...remaster}),skins=chooseCirnoPalettes(rules);
    assert.equal(skins[0],0);assert.equal(skins.length,players);assert.equal(new Set(skins).size,players);
    assert.ok(skins.slice(1).every(id=>Number.isInteger(id)&&id>=1&&id<=6));
    skins.slice(1).forEach((id,i)=>colors[i].add(id));
    assert.deepEqual(chooseCirnoPalettes({...rules,stageId:5}),skins,'second or third stage does not resample');
  }
  for(const seen of colors)assert.equal(seen.size,6);
  const nextRuns=new Set(Array.from({length:64},(_,i)=>{
    const next=makeCoopRules({...w,matchId:`th128mp-7402-${i+1}`,...remaster});
    return chooseCirnoPalettes(next)[1];
  }));
  assert.equal(nextRuns.size,6,'new runs in the same room must not be permanently stuck on one color');
}
test('2P/3P palette options are part of the agreed tape and all six remastered styles can be picked',async()=>{
  await verifyMatch(2);await verifyMatch(3);
  assert.throws(()=>makeCoopRules({...base,randomCirnoPalette:1}),/palette/i);
  assert.throws(()=>makeCoopRules({...base,...remaster,cirnoPaletteSet:'unknown'}),/palette/i);
  assert.throws(()=>makeCoopRules({...base,...remaster,randomCirnoPalette:false}),/palette/i);
});

test('only guest body atlas records change, including a half-opaque ghost',async()=>{
  const {applyCirnoPalettes,cirnoPalettes}=await import('../runtime/coop-cirno-palettes.mjs');
  const body=seat=>({bank:7,entry:0,id:4000000+seat*10,drawPass:'player',sprite:18,color:seat===2?0x80ffffff:0xffffffff,x:320,y:416});
  const p2=body(1),p3=body(2),other=[body(0),{...p2,id:p2.id+1}, {...p3,id:p3.id+1},
    {...p2,sprite:24},{...p2,id:5000010,drawPass:'playerShotGroup'},{...p2,bank:11,id:12000010}];
  const records=Object.freeze([...other,p2,p3]);
  const output=applyCirnoPalettes(records,[0,5,6]);assert.deepEqual(output.slice(0,other.length),other);
  other.forEach((r,i)=>assert.strictEqual(output[i],r));assert.deepEqual(output.slice(-2),[{...p2,bank:cirnoPalettes[4].bank},{...p3,bank:cirnoPalettes[5].bank}]);
  assert.strictEqual(applyCirnoPalettes(records,[0,0,0]),records);assert.equal(p2.bank,7);
});

test('the six remastered patches change only body pixels and leave the locally imported base intact',async()=>{
  const {cirnoPalettes,applyCirnoPalettePixels}=await import('../runtime/coop-cirno-palettes.mjs');
  assert.equal(cirnoPalettes.length,6);
  const paints=[];
  for(const [i,asset] of cirnoPalettes.entries()){
    const bytes=await readFile(new URL(asset.patchUrl));assert.equal(bytes.length,asset.bytes);
    assert.equal(createHash('sha256').update(bytes).digest('hex'),asset.sha256);assert.equal(asset.id,i+1);
    const pixels=new Uint8ClampedArray(256*256*4).fill(254),body=pixels.subarray(0,256*144*4).slice();
    applyCirnoPalettePixels(pixels,bytes);
    assert.notDeepEqual(pixels.subarray(0,body.length),body);
    assert.ok(pixels.subarray(body.length).every(n=>n===254));
    paints.push(createHash('sha256').update(pixels).digest('hex'));
    assert.throws(()=>applyCirnoPalettePixels(pixels,bytes.subarray(0,-1)),/Cirno/);
    const outOfRange=Buffer.from(bytes);outOfRange.writeUInt16LE(65535,4);
    assert.throws(()=>applyCirnoPalettePixels(pixels,outOfRange),/Cirno/);
  }
  assert.equal(new Set(paints).size,6);
});

test('successful storage no longer calls an on-screen status message, and storage errors still have feedback',async()=>{
  const entry=await readFile(new URL('../site/managed-entry.mjs',import.meta.url),'utf8');
  assert.doesNotMatch(entry,/录像已保存到/);assert.match(entry,/录像保存失败/);assert.match(entry,/shelf\.save\(file\)/);
});
