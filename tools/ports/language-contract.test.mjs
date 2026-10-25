import test from 'node:test';
import assert from 'node:assert/strict';
import {assessLanguageScene,assessMessageTextRasters,planLanguageScenes} from './language-contract.mjs';
const hex=text=>Buffer.from(text).toString('hex');
const profile={samples:[{key:'start',operation:'string-by-id'},{key:'replay',operation:'string-by-id'}]};
const trace=(values=['Start','Replay'])=>({schema:'eagler-touhou/language-trace/1',enabled:true,eventCapacity:256,
  uniqueEvents:5,totalObservations:5,droppedEvents:0,stringTruncated:0,events:[
    ...values.flatMap((text,index)=>[{kind:'lookup',operation:'string-by-id',phase:'runtime',scene:1,flags:1,textHex:hex(profile.samples[index].key),valueHex:hex(text),first:index+1,last:index+1},
      ...[...new Set([...text])].map(character=>({kind:'glyph',phase:'runtime',scene:1,a:16,b:400,c:character.codePointAt(0),d:character.codePointAt(0),flags:33,source:'pack',first:3,last:3})),
      {kind:'raster',phase:'runtime',scene:1,a:16,b:400,encoding:'utf8',textHex:hex(text),first:4,last:4}]),
    {kind:'present',phase:'runtime',scene:1,first:5,last:5,count:1,a:1,b:1},
  ]});
const expected={entries:[{key:'start',value:'Start',hasTranslation:true},{key:'replay',value:'Replay',hasTranslation:true}]};

test('selected language checks keep their independent baseline and omit unrelated cold launches',()=>{
  const plan=checks=>planLanguageScenes({checks,languages:['en','zh']});
  assert.deepEqual(plan(['LANG-01']).scenes.map(s=>s.id),['japanese-offline']);assert.equal(plan(['LANG-01']).consumers,true);
  assert.deepEqual(plan(['LANG-04']).scenes.map(s=>s.id),['japanese-offline','network-failure','corrupt-archive']);assert.equal(plan(['LANG-04']).consumers,false);
  assert.deepEqual(plan(['LANG-02','LANG-06']).scenes.map(s=>s.id),['japanese-offline','translated-en','translated-zh','partial-key']);
  assert.equal(plan(['LANG-05']).consumers,true);assert.equal(plan().scenes.length,6);
  assert.throws(()=>plan(['LANG-99']));assert.throws(()=>plan([]));
});

function messageTrace(){return{schema:'eagler-touhou/language-trace/1',enabled:true,droppedEvents:0,events:[
  {kind:'glyph',phase:'runtime',scene:2,a:15,b:400,c:0x3042,d:0x3042,flags:33,source:'msgothic',first:3,last:3},
  {kind:'raster',phase:'runtime',scene:2,a:15,b:400,encoding:'cp932',textHex:'82a0',first:4,last:4},
  {kind:'present',phase:'runtime',scene:2,count:1,a:1,b:1,first:5,last:5},
]};}
const messageExpected={entry:0,rows:[{offset:12,time:0,opcode:16,textHex:'82a0'}]};
test('loaded MSG text proves original glyphs and present without inventing a localization lookup',()=>{
  const value=assessMessageTextRasters({trace:messageTrace(),expected:messageExpected,scene:2,after:1});assert.equal(value.status,'observed');assert.equal(value.rows[0].text,'あ');
});
for(const [name,mutate]of Object.entries({
  'missing real raster':t=>t.events=t.events.filter(e=>e.kind!=='raster'),
  'missing glyph':t=>t.events=t.events.filter(e=>e.kind!=='glyph'),
  'missing present':t=>t.events=t.events.filter(e=>e.kind!=='present'),
  'wrong loaded text':t=>t.events[1].textHex='82a2',
  'wrong scene':t=>t.events[1].scene=3,
  'preparation glyph':t=>t.events[0].phase='prepare',
  'prior-only present':t=>{t.events[2].first=1;t.events[2].last=2;},
}))test(`original MSG proof rejects ${name}`,()=>{const trace=messageTrace();mutate(trace);assert.equal(assessMessageTextRasters({trace,expected:messageExpected,scene:2,after:1}).status,'incomplete');});
test('original MSG proof rejects stale windows, overflow and substituted glyphs',()=>{
  assert.equal(assessMessageTextRasters({trace:messageTrace(),expected:messageExpected,scene:2,after:6}).status,'incomplete');
  const overflow=messageTrace();overflow.droppedEvents=1;assert.throws(()=>assessMessageTextRasters({trace:overflow,expected:messageExpected,scene:2}));
  const notdef=messageTrace();Object.assign(notdef.events[0],{flags:2,d:-1});assert.throws(()=>assessMessageTextRasters({trace:notdef,expected:messageExpected,scene:2}));
});

test('translated and missing-key values use independent pack and cold-baseline evidence',()=>{
  assert.equal(assessLanguageScene({trace:trace(),profile,expected}).status,'observed');
  const partial={entries:[expected.entries[0]]},baseline={replay:hex('Original replay')};
  const actual=trace(['Start','Original replay']);actual.events.find(event=>event.kind==='lookup'&&event.textHex===hex('replay')).flags=0;
  const result=assessLanguageScene({trace:actual,profile,expected:partial,baseline});
  assert.deepEqual(result.rows.map(row=>row.expectedSource),['selected-pack','japanese-original']);
  assert.throws(()=>assessLanguageScene({trace:trace(),profile,expected:partial,baseline}),/differs/);
});

test('a key query, warm-up glyph or lookup record cannot substitute for actual raster and successful present',()=>{
  for(const kind of ['raster','present']){
    const value=trace();value.events=value.events.filter(event=>event.kind!==kind);
    assert.equal(assessLanguageScene({trace:value,profile,expected}).status,'incomplete');
  }
  const prepare=trace();prepare.events.forEach(event=>event.phase='prepare');
  assert.equal(assessLanguageScene({trace:prepare,profile,expected}).status,'incomplete');
  const overflow=trace();overflow.droppedEvents=1;
  assert.equal(assessLanguageScene({trace:overflow,profile,expected}).status,'incomplete');
  const unrelated=trace();unrelated.stringTruncated=1;
  assert.equal(assessLanguageScene({trace:unrelated,profile,expected}).status,'observed');
  const truncated=trace();truncated.stringTruncated=1;truncated.events[0].valueTruncated=true;
  assert.equal(assessLanguageScene({trace:truncated,profile,expected}).status,'incomplete');
  const changed=trace();changed.events[0].valueHex='00';
  assert.throws(()=>assessLanguageScene({trace:changed,profile,expected}),/differs/);
  const boundary=trace();
  const boundaryRaster=boundary.events.find(event=>event.kind==='raster');
  const boundaryLookup=boundary.events.find(event=>event.kind==='lookup');
  const boundaryPresent=boundary.events.find(event=>event.kind==='present');
  boundary.events.filter(event=>event.kind!=='present').forEach(event=>{event.scene=0;});boundaryPresent.scene=1;
  assert.equal(boundaryPresent.first,boundaryRaster.last+1,'Fixture models the production update-to-present Scene transition');
  assert.equal(assessLanguageScene({trace:boundary,profile,expected}).status,'observed');
  const unrelatedScene=trace();
  unrelatedScene.events.filter(event=>event.kind!=='present').forEach(event=>{event.scene=0;});
  unrelatedScene.events.find(event=>event.kind==='present').scene=2;
  Object.assign(unrelatedScene.events.find(event=>event.kind==='present'),{first:6,last:6});
  assert.equal(assessLanguageScene({trace:unrelatedScene,profile,expected}).status,'incomplete');
  const prior=trace();Object.assign(prior.events.find(event=>event.kind==='present'),{first:1,last:3});
  assert.equal(assessLanguageScene({trace:prior,profile,expected}).status,'incomplete');
  for(const fields of [{count:0},{a:0},{b:0}]){
    const failedPresent=trace();Object.assign(failedPresent.events.find(event=>event.kind==='present'),fields);
    assert.equal(assessLanguageScene({trace:failedPresent,profile,expected}).status,'incomplete');
  }
  const notdef=trace();Object.assign(notdef.events.find(event=>event.kind==='glyph'),{flags:2,d:-1,source:'msgothic-notdef'});
  assert.throws(()=>assessLanguageScene({trace:notdef,profile,expected}),/Missing or substituted glyph/);
});
