import test from 'node:test';
import assert from 'node:assert/strict';
import {readFile} from 'node:fs/promises';
import {createPracticeEngine} from '../../ports/th02/web/practice-engine.js';
import {PRACTICE_DEFAULTS,PRACTICE_VERSION,normalizePracticeSetup} from '../../ports/th02/web/practice-config.js';

test('TH02 allows at most five spare lives and bombs',()=>{
  assert.equal(normalizePracticeSetup({lives:5,bombs:5}).lives,5);
  assert.throws(()=>normalizePracticeSetup({lives:6}),/超出范围/);
  assert.throws(()=>normalizePracticeSetup({bombs:6}),/超出范围/);
});

test('TH02 counts a final death after native deathbomb and respawn handling',async()=>{
  const manifest=JSON.parse(await readFile(new URL('../../ports/th02/web/practice-assets.json',import.meta.url)));
  const patch=manifest.languages.jp.patch,base=0x10000,cs=0x3000,ds=0x5000,d=base+ds*16,cb=base+cs*16,resident=base+0x90000;
  const h=new Uint8Array(0x120000),v=new DataView(h.buffer);
  h.set(new TextEncoder().encode(patch.signature),cb+patch.entries.mailbox);
  h.set(new TextEncoder().encode('MIKOConfig\0'),resident);
  v.setUint16(d+0x27fe,0x9000,true);v.setUint16(d+0x27fc,0,true);
  const engine=createPracticeEngine({mod:{HEAPU8:h},patch,config:{version:PRACTICE_VERSION,mode:'target',setup:{stage:0,entry:'stage',lives:2,bombs:3},options:PRACTICE_DEFAULTS},clock:{setSpeed(){}}});
  const signal=port=>engine.signal(port,ds<<16|cs),write=(offset,value)=>h[d+offset]=value;
  v.setUint32(d+0xb28,100000,true);v.setUint32(d+0x3e3c,999,true);v.setUint16(d+0x3e40,10,true);v.setUint16(d+0xb46,3,true);
  signal(0x7fd0);signal(0x7fd1);assert.equal(engine.status.lives,2);assert.equal(engine.status.bombs,3);
  assert.equal(v.getUint32(d+0xb28,true),0);assert.equal(v.getUint32(d+0x3e3c,true),0);assert.equal(v.getUint16(d+0x3e40,true),0);assert.equal(v.getUint16(d+0xb46,true),0);
  write(0x2b94,1);signal(0x7fd4);assert.equal(engine.status.outcome,null,'contact is not game over');
  write(0x2b94,0);write(0xaa7,1);write(0x2b99,10);signal(0x7fd4);write(0xaa7,0);write(0x2b99,0);signal(0x7fd4);
  assert.equal(engine.status.outcome,null,'cancelled death and spare lives do not end practice');
  write(0xb2c,0);write(0xaa7,1);write(0x2b99,10);signal(0x7fd4);assert.equal(engine.status.outcome,null);
  write(0x2b99,0);signal(0x7fd4);assert.equal(engine.status.outcome,'failed','last life ends after native death processing');
});
