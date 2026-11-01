import test from 'node:test';
import assert from 'node:assert/strict';
import {decodeMusicNotes} from '../web/music-notes.mjs';
test('PMD note display follows music time and measure boundaries',()=>{
 const header=new TextEncoder().encode(JSON.stringify({op:[0,2]})),bytes=new Uint8Array(4+header.length+28),view=new DataView(bytes.buffer);view.setUint32(0,header.length,true);bytes.set(header,4);const base=4+header.length;
 view.setUint32(base,10,true);view.setUint16(base+4,1,true);bytes.set([48,255,255,255,255,255,255,255],base+6);
 view.setUint32(base+14,250,true);view.setUint16(base+18,2,true);bytes.set([255,50,255,255,255,255,255,255],base+20);
 const notes=decodeMusicNotes(bytes);assert.equal(notes('OP.M2',0),null);assert.deepEqual(notes('OP.M2',.1),{measure:1,notes:[48,255,255,255,255,255,255,255]});assert.equal(notes('op',.3).measure,2);assert.equal(notes('op',.3).notes[1],50);assert.equal(notes('missing',1),null);assert.equal(notes('op',NaN),null);
 assert.throws(()=>decodeMusicNotes(bytes.subarray(0,bytes.length-1)),/range/);
});
