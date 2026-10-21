import test from 'node:test';
import assert from 'node:assert/strict';
import {encodeLocalizationTable,encodeStringLocalizationTable} from '../../launcher/server/thcrap-compiler.mjs';
import {decodeLanguageTable,omitLanguageTableKeys} from './language-tables.mjs';

test('independent table oracle decodes the actual compiler strings and preserves per-key missing translations',()=>{
  const bytes=encodeStringLocalizationTable({'th07 Menu Start':'Start the game','th07 Menu Replay':'Watch a Replay'},{game:'th08'});
  const table=decodeLanguageTable(bytes);
  assert.equal(table.kind,'EST1');
  assert.equal(table.entries.find(entry=>entry.key==='th07 Menu Start').value,'Start the game');
  assert.equal(table.entries.find(entry=>entry.key==='th07 Menu Replay').hasTranslation,true);
  assert.ok(table.entries.some(entry=>!entry.hasTranslation));
  const partial=decodeLanguageTable(omitLanguageTableKeys(bytes,['th07 Menu Start']));
  assert.equal(partial.entries.some(entry=>entry.key==='th07 Menu Start'),false);
  assert.deepEqual(partial.entries.find(entry=>entry.key==='th07 Menu Replay').value,'Watch a Replay');
  assert.throws(()=>omitLanguageTableKeys(bytes,['invented']),/absent/);
});

test('numeric source tables retain their native key and line identity',()=>{
  const bytes=encodeLocalizationTable({th08_0:['Title','Comment'],th08_1:'Stage'},{game:'th08',table:'themes'});
  assert.deepEqual(decodeLanguageTable(bytes).entries.map(({key,line,value})=>({key,line,value})),[
    {key:0,line:0,value:'Title'},{key:0,line:1,value:'Comment'},{key:1,line:0,value:'Stage'},
  ]);
  assert.deepEqual(decodeLanguageTable(omitLanguageTableKeys(bytes,[0])).entries.map(entry=>entry.key),[1]);
});

test('the oracle rejects truncated, ambiguous and malformed records rather than inventing fallback text',()=>{
  const original=encodeLocalizationTable({th08_0:'Title'},{game:'th08',table:'themes'});
  for(let size=0;size<original.length;size++)assert.throws(()=>decodeLanguageTable(original.subarray(0,size)));
  assert.throws(()=>decodeLanguageTable(Buffer.concat([original,Buffer.from([0])])),/trailing/);
  const duplicate=Buffer.concat([original,original.subarray(8)]);duplicate.writeUInt32LE(2,4);
  assert.throws(()=>decodeLanguageTable(duplicate),/Duplicate/);
  const invalid=Buffer.from(original);invalid[16]=255;
  assert.throws(()=>decodeLanguageTable(invalid),/encoded data/);
});
