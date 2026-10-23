import test from 'node:test';
import assert from 'node:assert/strict';
import {explicitLanguageSelectionSteps,normalGameLanguagePreferenceKey} from './language-selection-contract.mjs';
import {resolvePreferredGameLanguage} from '../../launcher/src/launcher/language-catalog.mts';

const choices=['ja','lang_en','lang_zh-hans'];

test('a translated UI-locale default is explicitly selected before claiming durable preference',()=>{
  const current=resolvePreferredGameLanguage(choices.map(id=>({id})),null,'zh-CN');
  assert.equal(current,'lang_zh-hans');
  assert.deepEqual(explicitLanguageSelectionSteps({current,target:current,stored:null,options:choices}),['ja','lang_zh-hans']);
});

test('a stored choice already selected needs no toggle, while another selection writes the requested choice',()=>{
  assert.deepEqual(explicitLanguageSelectionSteps({current:'lang_zh-hans',target:'lang_zh-hans',stored:'lang_zh-hans',options:choices}),[]);
  assert.deepEqual(explicitLanguageSelectionSteps({current:'lang_zh-hans',target:'lang_en',stored:null,options:choices}),['lang_en']);
  assert.equal(normalGameLanguagePreferenceKey('th08'),'eagler-touhou-language-v1-th08');
});

test('a default with no other published option cannot be mistaken for a persisted selection',()=>{
  assert.throws(()=>explicitLanguageSelectionSteps({current:'lang_zh-hans',target:'lang_zh-hans',stored:null,
    options:['lang_zh-hans']}),/no alternate Launcher choice/);
  assert.throws(()=>explicitLanguageSelectionSteps({current:'ja',target:'lang_zh-hans',stored:null,options:['ja']}),/not available/);
});
