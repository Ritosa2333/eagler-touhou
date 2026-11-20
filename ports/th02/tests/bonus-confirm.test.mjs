import assert from 'node:assert/strict';
import test from 'node:test';
import {freshBonusConfirm,installBonusConfirm} from '../web/netplay/bonus-confirm.js';

test('TH02 bonus accepts a fresh shot or confirm from any seat, irrespective of other held keys',()=>{
  assert.equal(freshBonusConfirm([32,32,32],[32,32,32]),false);
  assert.equal(freshBonusConfirm([32,256,32],[32,32,32]),true);
  assert.equal(freshBonusConfirm([32,0,32],[32,0,0]),true);
  assert.equal(freshBonusConfirm([32,128,16],[32,0,0]),false);
});

test('TH02 installs the regular and Extra waits while preserving DOS relocations',()=>{
  const calls=[0x1400a,0x14189,0x17d99,0x19426,0x1ac2f];
  const patch={headerSize:0x1a00,codeSegment:0xb16};
  const exe=new Uint8Array(0x20000),view=new DataView(exe.buffer),table=0x3e;
  const relocation=(i,at)=>{view.setUint16(table+i*4,at%16,true);view.setUint16(table+i*4+2,at>>4,true);};
  view.setUint16(6,calls.length,true);view.setUint16(24,table,true);
  calls.forEach((at,i)=>{relocation(i,at+3);exe.set([0x9a,0x6c,0,0x45,0x0f],patch.headerSize+at);});
  const original=exe.slice();installBonusConfirm(exe,patch);
  assert.equal(view.getUint16(6,true),calls.length+1);
  assert.deepEqual(exe.slice(table,table+calls.length*4),original.slice(table,table+calls.length*4));
  const targets=calls.map(at=>view.getUint16(patch.headerSize+at+1,true));
  assert.equal(targets[0],targets[1]);
  assert(targets.slice(2).every(offset=>offset===targets[2]&&offset!==targets[0]));
  const substitute=patch.headerSize+patch.codeSegment*16;
  assert.equal(exe[substitute+targets[2]],0xcb);
  const p=table+calls.length*4,added=view.getUint16(p,true)+16*view.getUint16(p+2,true);
  assert.equal(view.getUint16(patch.headerSize+added-2,true),0x1c8);
  assert.equal(view.getUint16(patch.headerSize+added,true),0xf45);
});
