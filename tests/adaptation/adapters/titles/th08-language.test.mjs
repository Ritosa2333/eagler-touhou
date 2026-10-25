import test from 'node:test';
import assert from 'node:assert/strict';
import {parseTh08DialogueText,TH08_LANGUAGE_CONSUMERS} from './th08-language.mjs';
import {validateObservationFlow,matchesObservationFlowEnd} from '../flow-steps.mjs';

function message(){
  // Two independently specified retail entries: SpeakerText and Text(prefix=4), each followed by Wait.
  const first=Buffer.from([0,0,16,3,0xf5,0xd7,0x77,0,0,4,4,60,0,0,0]),second=Buffer.from([0,0,3,7,0,0,0,0,0xf5,0xd5,0x77,0,0,4,4,60,0,0,0]);
  const header=Buffer.alloc(12);header.writeUInt32LE(2);header.writeUInt32LE(12,4);header.writeUInt32LE(12+first.length,8);return Buffer.concat([header,first,second]);
}
test('independent MSG parser preserves the selected entry, XOR bytes and instruction prefix',()=>{
  const bytes=message();assert.equal(parseTh08DialogueText(bytes,0).rows[0].textHex,'82a0');assert.equal(parseTh08DialogueText(bytes,1).rows[0].textHex,'82a2');
  assert.equal(parseTh08DialogueText(bytes,1).rows[0].offset,27);
});
for(const [name,mutate,entry]of [
  ['bad table',b=>b.writeUInt32LE(4097),0],['header offset',b=>b.writeUInt32LE(4,4),0],['cross-entry instruction',b=>b[15]=255,0],
  ['unterminated text',b=>b[18]=1,0],['wrong entry',()=>{},2],
])test(`independent MSG parser rejects ${name}`,()=>{const bytes=message();mutate(bytes);assert.throws(()=>parseTh08DialogueText(bytes,entry));});
test('statistics route is finite and its independent original label is human-readable Japanese',()=>{
  for(const key of ['enter','exit'])assert.equal(validateObservationFlow(TH08_LANGUAGE_CONSUMERS.consumers.statistics[key]).status,'passed');
  const label=TH08_LANGUAGE_CONSUMERS.consumers.statistics.samples[0];assert.match(new TextDecoder('shift_jis',{fatal:true}).decode(Buffer.from(label.originalHex,'hex')),/^プレイ回数/);
  const observed={scene:5,resultActive:true,resultState:20};
  assert.equal(matchesObservationFlowEnd(TH08_LANGUAGE_CONSUMERS.consumers.statistics.enter,observed),true);
  assert.equal(matchesObservationFlowEnd(TH08_LANGUAGE_CONSUMERS.consumers.statistics.enter,{...observed,scene:6}),false);
});
