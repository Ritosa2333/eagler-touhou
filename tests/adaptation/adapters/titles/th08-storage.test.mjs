import test from 'node:test';
import assert from 'node:assert/strict';
import {inspectTh08Configuration,inspectTh08ScorePayload} from './th08-storage.mjs';

function config(){const bytes=Buffer.alloc(60);bytes.writeUInt32LE(0x80001,20);bytes[31]=2;bytes[33]=1;bytes[39]=100;bytes[40]=80;return bytes;}
function score(){
  const bytes=Buffer.alloc(28+12+0x228+28);bytes.writeUInt16LE(1,4);bytes.writeUInt32LE(28,8);
  bytes.writeUInt32LE(bytes.length,16);bytes.writeUInt32LE(bytes.length-28,20);
  const chapter=(at,type,length,version)=>{bytes.write(type,at);bytes.writeUInt16LE(length,at+4);bytes.writeUInt16LE(length,at+6);bytes[at+8]=version;};
  chapter(28,'TH8K',12,1);chapter(40,'PLST',0x228,2);const version=40+0x228;
  chapter(version,'VRSM',28,1);bytes.write('0100d',version+12);bytes.writeUInt32LE(840704,version+20);bytes.writeUInt32LE(2724749753,version+24);
  bytes.writeUInt32LE(2,40+44+68);bytes.writeUInt32LE(2,40+44+68+4);
  return bytes;
}

test('configuration reader rejects values that production would replace with defaults',()=>{
  assert.equal(inspectTh08Configuration(config()).values.soundVolume,80);
  for(const [offset,value] of [[20,0],[28,7],[29,4],[30,2],[31,3],[32,2],[33,6],[34,2],[35,3],[36,3],[37,2],[38,2]]){
    const bytes=config();bytes[offset]=value;assert.throws(()=>inspectTh08Configuration(bytes));
  }
  const legacy=config();legacy[39]=255;assert.equal(inspectTh08Configuration(legacy).values.musicVolume,-1);
});

test('score reader validates consumed chapter versions and executable compatibility',()=>{
  const valid=score(),parsed=inspectTh08ScorePayload(valid);
  assert.equal(parsed.values.statistics[1].total,2);assert.equal(parsed.values.statistics[1].characters[0],2);
  assert.equal(parsed.values.program.version,'0100d');
  for(const mutate of [b=>b[36]=2,b=>b[48]=1,b=>b.writeUInt16LE(11,44),b=>b.writeUInt32LE(0,b.length-4),b=>b.writeUInt32LE(0,20)]){
    const bytes=score();mutate(bytes);assert.throws(()=>inspectTh08ScorePayload(bytes));
  }
  const clock=score();clock.writeUInt32LE(999,40+12);
  assert.equal(inspectTh08ScorePayload(clock).semanticSha256,parsed.semanticSha256);
  const progress=score();progress.writeUInt32LE(3,40+44+68);
  assert.notEqual(inspectTh08ScorePayload(progress).semanticSha256,parsed.semanticSha256);
});

test('score statistics follow the last complete matching chapter consumed by the game',()=>{
  const original=score(),last=Buffer.from(original.subarray(40,40+0x228));
  last.writeUInt32LE(3,44+68);
  const short=Buffer.alloc(12);short.write('PLST');short.writeUInt16LE(12,4);short[8]=2;
  const bytes=Buffer.concat([original,last,short]);
  bytes.writeUInt32LE(bytes.length,16);bytes.writeUInt32LE(bytes.length-28,20);
  assert.equal(inspectTh08ScorePayload(bytes).values.statistics[1].total,3);
});

export {config as configurationFixture,score as decodedScoreFixture};
