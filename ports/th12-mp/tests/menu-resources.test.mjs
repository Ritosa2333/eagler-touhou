import test from 'node:test';
import assert from 'node:assert/strict';
import fs from 'node:fs';
import {MENU_SCRIPTS,selectEnding,decodeEndingMessage,decodeEndingText,musicComment,validateMenuResources} from '../src/menu-resources.mjs';
import {AnimationSampler} from '../src/animation.mjs';
import {parseReplay} from '../src/replay.mjs';

const retail=new URL('../local/retail/',import.meta.url),prepared=new URL('../site/assets/menu-resources.json',import.meta.url);
const available=fs.existsSync(prepared)&&fs.existsSync(new URL('staff.msg',retail));
const resource=available?JSON.parse(fs.readFileSync(prepared,'utf8')):null;

test('ending choice uses character, equipment and continue state as native410d76',()=>{
  for(let character=0;character<3;character++)for(let shot=0;shot<2;shot++){
    assert.equal(selectEnding(character,shot,true),character*2+shot);
    assert.equal(selectEnding(character,shot,false),character*2+shot+6);
  }
  assert.throws(()=>selectEnding(-1,0));assert.throws(()=>selectEnding(1,2));
});

test('message decoder handles bounds and encrypted terminators',()=>{
  let key=0x77,step=7;const bytes=Uint8Array.from([...Buffer.from('ENDING'),0],b=>{const value=b^key;key=(key+step)&255;step=(step+16)&255;return value;});
  assert.equal(decodeEndingText(bytes),'ENDING');
  assert.throws(()=>decodeEndingText(bytes.subarray(0,1)),/结束标记/);
  const header=new Uint8Array(16);new DataView(header.buffer).setUint32(0,1,true);new DataView(header.buffer).setUint32(4,12,true);
  assert.equal(decodeEndingMessage(header).at(-1).op,0);
  header[15]=8;assert.throws(()=>decodeEndingMessage(header),/越界/);
});

test('local ending and credit metadata exactly retain decoded native MSG commands',{skip:!available},()=>{
  assert.equal(validateMenuResources(resource),resource);
  for(const item of [...resource.endings,resource.credits]){
    const actual=decodeEndingMessage(fs.readFileSync(new URL(item.file,retail)));
    assert.deepEqual(actual,item.commands,item.file);
  }
  assert.equal(resource.endings.filter(e=>e.commands.some(c=>c.op===12)).length,6);
  assert.deepEqual(resource.credits.commands.filter(c=>c.op>=15&&c.op<=17).map(c=>[c.difficulty,c.script]),[[1,41],[2,42],[3,43]]);
});

test('all ending and credit script references resolve to original exported banks',{skip:!available},()=>{
  for(const sequence of [...resource.endings,resource.credits]){
    const banks=new Map();
    for(const command of sequence.commands){
      if(command.op===7)banks.set(command.bankSlot,command.file.replace(/\.anm$/,''));
      if(command.kind==='animation')assert.ok(resource.atlases[banks.get(command.bankSlot)]?.scripts[command.script],`${sequence.file}: ${command.script}`);
    }
  }
  const sampler=new AnimationSampler(resource.atlases);
  const credit=sampler.sampleAll('staff',0,300);
  assert.ok(credit.some(p=>p.sprite===0));assert.ok(credit.some(p=>p.sprite===1));
  assert.equal(sampler.diagnostics().unimplemented.length,0);
});

test('Music Room records preserve all 17 original names and eight comment lines',{skip:!available},()=>{
  assert.equal(resource.musicComments.length,17);
  for(const item of resource.musicComments){assert.equal(item.lines.length,8);assert.ok(item.lines[0].startsWith('♪'));assert.equal(musicComment(resource,item.file),item);}
  assert.equal(musicComment(resource,0).id,'th12_01');assert.equal(musicComment(resource,'th12_17.ogg').index,12);
});

test('four exported demo replays match originals and checked header metadata',{skip:!available},()=>{
  for(const demo of resource.demos){
    const bytes=fs.readFileSync(new URL(demo.file,retail));
    assert.deepEqual(bytes,fs.readFileSync(new URL('../site/assets/'+demo.file,import.meta.url)));
    const r=parseReplay(bytes);assert.equal(r.character,demo.character);assert.equal(r.shot,demo.shot);assert.equal(r.difficulty,demo.difficulty);assert.equal(r.stages[0].number,demo.stage);assert.equal(r.stages[0].frames,demo.frames);
  }
  assert.deepEqual(resource.demos.map(d=>d.stage),[4,3,1,1]);
  assert.equal(MENU_SCRIPTS.title.entries.length,8);
  assert.equal(resource.manual.available,false);
});
