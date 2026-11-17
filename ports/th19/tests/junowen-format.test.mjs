import assert from 'node:assert/strict';
import {decodeJunowen,encodeJunowen,webKeysToNative,nativeKeysToWeb,nativeRandomStep} from '../tools/replay/junowen-format.mjs';
const h={randSeed1:0x12345678,randSeed2:0x87654321,difficulty:3,matchup:0,common:0x90,p1Settings:12,p2Settings:12,p1Character:18,p1Card:3,p2Character:14,p2Card:7};
for(let matchup=0;matchup<4;matchup++){
 const data={header:{...h,matchup},inputs:matchup===1?[[1],[256],[65535]]:[[1,2],[256,8],[65535,0]]};
 const b=encodeJunowen(data);assert.deepEqual(decodeJunowen(b),data);
 for(let n=0;n<17;n++)assert.throws(()=>decodeJunowen(b.subarray(0,n)));
 assert.throws(()=>decodeJunowen(b.subarray(0,b.length-1)));
}
// Independent writer-layout fixture: seed1=1, seed2=2, Normal, two humans.
const fixture=new Uint8Array(17+600*4);fixture.set([1,0,0,0,2,0,0,0,1,0,0,12,12,0,0,1,0]);
assert.deepEqual(encodeJunowen(decodeJunowen(fixture)),fixture);
for(let bits=0;bits<256;bits++)for(const pause of [0,1024])assert.equal(nativeKeysToWeb(webKeysToNative(bits|pause)),bits|pause);
assert.throws(()=>webKeysToNative(256));assert.throws(()=>nativeKeysToWeb(1024));
let state=1;for(const value of [48271,182605794,1291394886,1914720637,2078669041]){const r=nativeRandomStep(state);assert.equal(r.state,value);assert.equal(r.shadow,value);state=r.state;}
console.log('PASS: 17-byte header, 4 matchup layouts, exact fixture roundtrip, 512 input mappings, native RNG reference sequence');
