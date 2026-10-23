import test from 'node:test';
import assert from 'node:assert/strict';
import {languageFailureResponse,classifyLanguageBrowserEvents,resolveLanguageRuntimeSource} from './language-network-contract.mjs';
import {resolveLanguagePackSource} from '../../launcher/src/launcher/language-catalog.mts';
import {classifyTh08LanguageDiagnostic} from '../../tests/adaptation/adapters/titles/th08-language.mjs';

test('language negatives alter the real archive and preserve the complete corrupt byte count',()=>{
  const original=Buffer.alloc(64,12),corrupt=languageFailureResponse(original,{kind:'corrupt-archive'});
  assert.equal(corrupt.bytes,original.length);assert.equal(corrupt.status,200);
  assert.equal(corrupt.body.filter((value,index)=>value!==original[index]).length,1);
  assert.equal(original[32],12);
  assert.equal(languageFailureResponse(original,{kind:'network-failure'}).status,404);
  assert.throws(()=>languageFailureResponse(original,{kind:'any-error'}),/Unknown/);
});

test('only verified exact 404 response events are classified as intended controls',()=>{
  const url='https://127.0.0.1:123/games/new/language/en.zip';
  const response={url,status:404,contentIdentity:'passed',semanticReceipt:{kind:'controlled-resource-response'}};
  const http={type:'http',url,status:404},console404={type:'console',location:{url},
    text:'Failed to load resource: the server responded with a status of 404 (Not Found)'};
  assert.equal(classifyLanguageBrowserEvents([http,console404],{responses:[response]}).blocking.length,0);
  for(const events of [[{...http,url:url+'-other'}],[{...console404,text:'Unrelated native error'}],[{type:'pageerror',url}],
    [{...http,status:500}],[{type:'requestfailed',url,error:'net::ERR_FAILED'}]])
    assert.equal(classifyLanguageBrowserEvents(events,{responses:[response]}).blocking.length,1);
  assert.equal(classifyLanguageBrowserEvents([http],{responses:[{...response,contentIdentity:'failed'}]}).blocking.length,1);
  assert.equal(classifyLanguageBrowserEvents([http]).blocking.length,1);
  const unread={...response,semanticReceipt:{kind:'controlled-resource-response',bodyConsumption:'not-read'}};
  assert.equal(classifyLanguageBrowserEvents([{type:'requestfailed',url,error:'net::ERR_ABORTED'}],{responses:[unread]}).blocking.length,0);
  assert.equal(classifyLanguageBrowserEvents([{type:'requestfailed',url,error:'net::ERR_ABORTED'}],{responses:[response]}).blocking.length,1);
  assert.equal(classifyLanguageBrowserEvents([{...console404,text:'Failed to load resource: the server responded with a status of 404 ()'}],{responses:[response]}).blocking.length,0);
});

test('only the successful source-identified TH08 localization status is non-blocking',()=>{
  const runtimeSource='https://127.0.0.1:1234/runtime/th08/0123456789abcdef/th08-sdl.mjs';
  const text='th08 thcrap localization active: font=unifont-15.1.05-subset.otf (Unifont) utf8=ok spells=222 stages=0 themes=21 musiccmt=140 spellcomments=657 ascii=42 strings=154';
  const event={type:'console',text,location:{url:runtimeSource}};
  assert.deepEqual(classifyTh08LanguageDiagnostic({text}),{classification:'localization-activation',disposition:'expected',
    diagnostic:{font:'unifont-15.1.05-subset.otf',fontName:'Unifont',utf8:'ok',
      counts:{spells:222,stages:0,themes:21,musicComments:140,spellComments:657,ascii:42,strings:154}}});
  const accepted=classifyLanguageBrowserEvents([event],{runtimeSource,classifyDiagnostic:classifyTh08LanguageDiagnostic});
  assert.equal(accepted.expected[0].titleDiagnostic.classification,'localization-activation');assert.deepEqual(accepted.blocking,[]);
  for(const negative of [
    {...event,location:{url:runtimeSource+'?other'}},
    {...event,text:text.replace('utf8=ok','utf8=FAILED')},
    {...event,text:text+' unexpected'},
    {...event,text:'Unrelated Runtime failure'},
  ])assert.equal(classifyLanguageBrowserEvents([negative],{runtimeSource,classifyDiagnostic:classifyTh08LanguageDiagnostic}).blocking.length,1);
  assert.equal(classifyLanguageBrowserEvents([event],{runtimeSource}).blocking.length,1,'A known-looking message without a host classifier remains blocking');
});

test('successful pack glyph fallback requires the current Runtime and a verified mounted font',()=>{
  const runtimeSource='https://127.0.0.1:1234/runtime/th08/0123456789abcdef/th08-sdl.mjs';
  const font='/thcrap/th08/fonts/unifont-15.1.05-subset.otf';
  const event={type:'console',text:`th08 thcrap font fallback: U+6B63 from pack font ${font}`,location:{url:runtimeSource}};
  const options={runtimeSource,fontPaths:[font],classifyDiagnostic:classifyTh08LanguageDiagnostic};
  assert.equal(classifyLanguageBrowserEvents([event],options).blocking.length,0);
  for(const negative of [
    {...event,location:{url:runtimeSource+'?other'}},
    {...event,text:event.text.replace('U+6B63','U+D800')},
    {...event,text:event.text.replace('.otf','.otf missing')},
    {...event,text:event.text.replace('from pack font','missing glyph from pack font')},
    {...event,text:event.text.replace('unifont-15.1.05-subset.otf','unknown.otf')},
  ])assert.equal(classifyLanguageBrowserEvents([negative],options).blocking.length,1);
  assert.equal(classifyLanguageBrowserEvents([event],{runtimeSource}).blocking.length,1);
});

test('a next-title diagnostic uses its arbitrary declared asset and host classifier only for exact source',()=>{
  const generation='a'.repeat(64),asset='scripts/runtime-entry.any-name.js';
  const runtimeGroup={root:'runtime/next/',current:{generation,files:[{path:asset}]}};
  const browserDiagnostics={runtimeAsset:asset,source:'ports/next/runtime.cpp | diagnostic producer'};
  const runtimeSource=resolveLanguageRuntimeSource({browserDiagnostics,runtimeGroup,productRuntimeAssets:[asset],base:'https://launcher.invalid/'});
  assert.equal(runtimeSource,`https://launcher.invalid/runtime/next/${generation}/${asset}`);
  assert.equal(resolveLanguageRuntimeSource({browserDiagnostics,runtimeGroup,productRuntimeAssets:['another.js'],base:'https://launcher.invalid/'}),null,
    'The declaration must also appear in the selected product asset inventory');
  assert.equal(resolveLanguageRuntimeSource({browserDiagnostics,runtimeGroup:{...runtimeGroup,current:{...runtimeGroup.current,files:[]}},
    productRuntimeAssets:[asset],base:'https://launcher.invalid/'}),null,'The selected generation must contain the declared asset');
  assert.equal(resolveLanguageRuntimeSource({runtimeGroup,productRuntimeAssets:[asset],base:'https://launcher.invalid/'}),null,
    'A missing descriptor disables diagnostic exemptions without throwing');

  const calls=[],classifyDiagnostic=({text})=>{calls.push(text);return text==='next runtime active'
    ?{classification:'next-localization-ready',disposition:'expected'}
    :text==='next runtime failed'?{classification:'next-localization-ready',disposition:'blocking'}:null;};
  const event={type:'console',text:'next runtime active',location:{url:runtimeSource}};
  assert.equal(classifyLanguageBrowserEvents([event],{runtimeSource,classifyDiagnostic}).expected.length,1);
  assert.equal(calls.length,1);
  for(const negative of [
    {...event,location:{url:`${runtimeSource}?other`}},
    {...event,text:'unrecognized runtime log'},
    {...event,text:'next runtime failed'},
  ])assert.equal(classifyLanguageBrowserEvents([negative],{runtimeSource,classifyDiagnostic}).blocking.length,1);
  const throws=()=>{throw new Error('parser failed');};
  assert.equal(classifyLanguageBrowserEvents([event],{runtimeSource,classifyDiagnostic:throws}).blocking.length,1,
    'Title classifier exceptions cannot make browser events non-blocking');
});

test('failure control addresses the production versioned archive URL',()=>{
  const sha256='a'.repeat(64),entry={id:'lang_en',title:'English',pack:{url:'games/th08/language/lang_en.zip',bytes:100,sha256}};
  const source=resolveLanguagePackSource(entry,'https://127.0.0.1:1234/');
  assert.equal(new URL(source.url).searchParams.get('v'),sha256);
  assert.equal(new URL(source.url).pathname,'/games/th08/language/lang_en.zip');
});
