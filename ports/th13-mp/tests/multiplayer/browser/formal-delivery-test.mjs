import assert from 'node:assert/strict';
import test from 'node:test';
import {assertBuildDelivery} from './formal-delivery.mjs';
import {readFile} from 'node:fs/promises';

const expectedFiles = [
  {name:'th13-sdl.mjs', bytes:123, sha256:'a'.repeat(64)},
  {name:'th13-sdl.wasm', bytes:456, sha256:'b'.repeat(64)},
];
const sample = () => ({expectedFiles, playerCount:3, delivery:
  [0,1,2].flatMap(seat => expectedFiles.map(file => ({seat,
    url:`http://localhost/runtime/${file.name}?identity=1`, bytes:file.bytes,
    sha256:file.sha256}))) });

test('all clients must receive the exact loader and WASM', () => {
  assert.equal(assertBuildDelivery(sample()).passed, true);
});
test('old build of the same length is rejected before start', () => {
  const input=sample(); input.delivery[3].sha256='c'.repeat(64);
  assert.throws(()=>assertBuildDelivery(input), /Seat 1: delivered th13-sdl.wasm hash/);
});
test('missing body, wrong length and a different later reload all fail', () => {
  const missing=sample(); missing.delivery.pop();
  assert.throws(()=>assertBuildDelivery(missing), /Seat 2: missing/);
  const length=sample(); length.delivery[0].bytes=122;
  assert.throws(()=>assertBuildDelivery(length), /Seat 0: delivered.*size/);
  const reload=sample(); reload.delivery.push({...reload.delivery[5],sha256:'d'.repeat(64)});
  assert.throws(()=>assertBuildDelivery(reload), /Seat 2: delivered.*hash/);
});
test('one client cannot stand in for another or supply only one executable', () => {
  const seats=sample(); seats.delivery[4].seat=0; seats.delivery[5].seat=0;
  assert.throws(()=>assertBuildDelivery(seats), /Seat 2: missing/);
  const files=sample(); files.expectedFiles=expectedFiles.slice(0,1);
  assert.throws(()=>assertBuildDelivery(files), /Expected runtime loader and WASM/);
});

test('rejects malformed response URLs, unexpected filenames and out-of-range seats', () => {
  for (const [patch, pattern] of [
    [{url:'relative/th13-sdl.wasm'}, /invalid URL/],
    [{url:'file:///tmp/th13-sdl.wasm'}, /invalid URL origin/],
    [{url:'http://user:pass@localhost/th13-sdl.wasm'}, /invalid URL origin/],
    [{url:'http://localhost/th13-sdl.wasm.backup'}, /unexpected executable filename/],
    [{url:'http://localhost/th13-sdl%2ewasm'}, /unexpected executable filename/],
    [{seat:-1}, /invalid seat/], [{seat:3}, /invalid seat/], [{seat:'1'}, /invalid seat/],
  ]) {
    const input=sample(); Object.assign(input.delivery[0],patch);
    assert.throws(()=>assertBuildDelivery(input),pattern);
  }
});
test('rejects null, malformed and extraneous responses instead of ignoring them', () => {
  for (const row of [null,{}, {seat:9,url:'http://localhost/unrelated'}]) {
    const input=sample(); input.delivery.push(row);
    assert.throws(()=>assertBuildDelivery(input));
  }
  const empty=sample();empty.delivery=[];
  assert.throws(()=>assertBuildDelivery(empty),/missing delivered/);
  assert.throws(()=>assertBuildDelivery({...sample(),delivery:null}),/Delivery response list/);
  assert.throws(()=>assertBuildDelivery({...sample(),expectedFiles:null}),/Expected executable list/);
});
test('matching reloads pass, mismatched reloads and invalid later seats fail', () => {
  const input=sample(); input.delivery.push({...input.delivery[0]});
  assert.equal(assertBuildDelivery(input).responses,7);
  input.delivery.at(-1).sha256='d'.repeat(64);
  assert.throws(()=>assertBuildDelivery(input),/Seat 0: delivered th13-sdl.mjs hash/);
  input.delivery.at(-1).seat=99;
  assert.throws(()=>assertBuildDelivery(input),/invalid seat/);
});

const root='/work/artifacts/mp-autonomous-formal-20261112';
const network13=root+'/lunatic-rtc-4x-network-13/formal-smoke-progress.json';
const normal14=root+'/lunatic-rtc-4x-14/formal-smoke-progress.json';

test('real network13 mismatched build is rejected and normal14 delivery matches', async () => {
  const bad=JSON.parse(await readFile(network13,'utf8'));
  const good=JSON.parse(await readFile(normal14,'utf8'));
  const expectedBad=bad.expectedFiles.find(file=>file.name==='th13-sdl.wasm');
  const deliveredBad=bad.delivery.filter(row=>new URL(row.url).pathname.endsWith('/th13-sdl.wasm'));
  assert.equal(expectedBad.sha256,'686ba546bfee3342f2c93a46fd402e4e342ce3a9707abd974d42ca7d788d6bff');
  assert.equal(deliveredBad.length,3);
  assert(deliveredBad.every(row=>row.sha256==='e04eb01fbce6833d78d63978a122d15cd64dd179198e1c256a9bcb473c63f1b0'));
  assert.throws(()=>assertBuildDelivery({expectedFiles:bad.expectedFiles,delivery:bad.delivery,playerCount:3}),/Seat 0: delivered th13-sdl.mjs hash/);
  // Independently expose the original WASM mismatch, without changing report files.
  const mjs=bad.delivery.find(row=>row.seat===0 && new URL(row.url).pathname.endsWith('/th13-sdl.mjs'));
  const expectedWithObservedLoader=bad.expectedFiles.map(file=>file.name==='th13-sdl.mjs'?{...file,bytes:mjs.bytes,sha256:mjs.sha256}:file);
  assert.throws(()=>assertBuildDelivery({expectedFiles:expectedWithObservedLoader,delivery:bad.delivery,playerCount:3}),/Seat 0: delivered th13-sdl.wasm size/);
  assert.equal(good.expectedFiles.find(file=>file.name==='th13-sdl.wasm').sha256,'50182ec36e5d85150514079542ebf0e5aaed9459c1d87738484260493c64e8e5');
  assert.equal(assertBuildDelivery({expectedFiles:good.expectedFiles,delivery:good.delivery,playerCount:3}).passed,true);
});

test('frame-bound deliveries must match the unique seat iframe, including all reloads', () => {
  const input=sample(); const frameGuids=['frame@one','frame@two','frame@three'];
  const documentEpochs=[1,2,0];
  input.delivery.forEach(row=>{row.frameGuid=frameGuids[row.seat];row.documentEpoch=documentEpochs[row.seat];});
  assert.match(assertBuildDelivery({...input,frameGuids,documentEpochs}).scope,/supplied frame\/document identities/);
  const child=structuredClone(input); child.delivery[0].frameGuid='frame@unrelated-child';
  assert.throws(()=>assertBuildDelivery({...child,frameGuids,documentEpochs}),/Seat 0: delivery frame identity/);
  const reload=structuredClone(input);reload.delivery.push({...reload.delivery[0],frameGuid:'frame@old'});
  assert.throws(()=>assertBuildDelivery({...reload,frameGuids,documentEpochs}),/Seat 0: delivery frame identity/);
  const absent=structuredClone(input);delete absent.delivery[0].frameGuid;
  assert.throws(()=>assertBuildDelivery({...absent,frameGuids,documentEpochs}),/Seat 0: delivery frame identity/);
  for (const invalid of [null,[],['frame@one'],['frame@one','frame@one','frame@three'],['frame@one','','frame@three']])
    assert.throws(()=>assertBuildDelivery({...input,frameGuids:invalid,documentEpochs}),/Delivery frame identit/);
});

test('legacy reports are explicitly hash-only and cannot satisfy frame-bound mode', async () => {
  const report=JSON.parse(await readFile(normal14,'utf8'));
  const input={expectedFiles:report.expectedFiles,delivery:report.delivery,playerCount:3};
  assert.match(assertBuildDelivery(input).scope,/frame identity not checked/);
  assert.throws(()=>assertBuildDelivery({...input,frameGuids:['frame@0','frame@1','frame@2'],documentEpochs:[0,0,0]}),/delivery frame identity/);
});


test('document epoch rejects older navigation in the same frame and requires paired identity', () => {
  const input=sample(), frameGuids=['f0','f1','f2'],documentEpochs=[3,1,7];
  input.delivery.forEach(row=>{row.frameGuid=frameGuids[row.seat];row.documentEpoch=documentEpochs[row.seat];});
  assert.equal(assertBuildDelivery({...input,frameGuids,documentEpochs}).passed,true);
  const stale=structuredClone(input); stale.delivery[0].documentEpoch=2;
  assert.throws(()=>assertBuildDelivery({...stale,frameGuids,documentEpochs}),/Seat 0: delivery document epoch/);
  const late=structuredClone(input);late.delivery.push({...late.delivery[0],documentEpoch:2});
  assert.throws(()=>assertBuildDelivery({...late,frameGuids,documentEpochs}),/Seat 0: delivery document epoch/);
  const absent=structuredClone(input);delete absent.delivery[1].documentEpoch;
  assert.throws(()=>assertBuildDelivery({...absent,frameGuids,documentEpochs}),/Seat 0: delivery document epoch/);
  assert.throws(()=>assertBuildDelivery({...input,frameGuids}),/must be paired/);
  assert.throws(()=>assertBuildDelivery({...input,documentEpochs}),/must be paired/);
  for(const invalid of [null,[],[1,2],[1,-1,2],[1,1.5,2],[1,Number.MAX_SAFE_INTEGER+1,2],[1,'2',3]])
    assert.throws(()=>assertBuildDelivery({...input,frameGuids,documentEpochs:invalid}),/Delivery document epoch/);
});
