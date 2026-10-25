import test from 'node:test';
import assert from 'node:assert/strict';
import {mkdtempSync,writeFileSync,rmSync} from 'node:fs';
import {tmpdir} from 'node:os';
import {join} from 'node:path';
import {createHash} from 'node:crypto';
import {createStaticThcrapPack} from '../../../launcher/server/thcrap-static-pack.mjs';
import {encodeStringLocalizationTable} from '../../../launcher/server/thcrap-compiler.mjs';
import {decodeLanguageArchive} from '../../../tools/ports/language-inputs.mjs';
import {omitLanguageTableKeys} from '../../../tools/ports/language-tables.mjs';
import {languageFailureResponse} from '../../../tools/ports/language-network-contract.mjs';
import {normalGameLanguagePreferenceKey} from '../../../tools/ports/language-selection-contract.mjs';
import {resolveAdaptationAdapter} from './registry.mjs';
import {assertLanguageEvidence} from './language-evidence.mjs';
import {assessDriverReport} from '../results.mjs';

const product={game:'th08',variant:'normal'},profile=resolveAdaptationAdapter('th08','normal').language;
const hash=bytes=>createHash('sha256').update(bytes).digest('hex');
const hex=text=>Buffer.from(text).toString('hex');
function fixture(t){
  const directory=mkdtempSync(join(tmpdir(),'eagler-language-proof-'));
  t.after(()=>rmSync(directory,{recursive:true,force:true}));
  const save=(name,bytes)=>{
    const path=join(directory,name);writeFileSync(path,bytes);return{path,bytes:bytes.length,sha256:hash(bytes)};
  };
  const values=Object.fromEntries(profile.samples.map((sample,index)=>[sample.key,index?'Replay':'Start']));
  const original=Object.fromEntries(profile.samples.map((sample,index)=>[sample.key,index?'Original Replay':'Original Start']));
  const table=encodeStringLocalizationTable(values,{game:'th08'});
  const pack=(language,bytes)=>createStaticThcrapPack({pack:{game:'th08',language,assets:[]},
    resources:[{targetPath:profile.table,bytes}]});
  const full=pack('lang_en',table),partial=pack('lang_acceptance-partial',omitLanguageTableKeys(table,[profile.missingKey]));
  const fullIdentity=save('lang_en.zip',full.archive),partialIdentity={...save('partial.zip',partial.archive),language:'lang_acceptance-partial'};
  const fullMembers=decodeLanguageArchive(full.archive,{game:'th08',language:'lang_en'}).members;
  const partialMembers=decodeLanguageArchive(partial.archive,{game:'th08',language:partialIdentity.language}).members;
  const trace=(text,translated)=>({schema:'eagler-touhou/language-trace/1',enabled:true,eventCapacity:256,
    uniqueEvents:10,totalObservations:10,droppedEvents:0,events:[...profile.samples.flatMap(sample=>{
      const value=text[sample.key];
      return[{kind:'lookup',operation:'string-by-id',phase:'runtime',scene:1,flags:translated(sample.key)?1:0,
        textHex:hex(sample.key),valueHex:hex(value),first:1,last:1},
      ...[...new Set([...value])].map(character=>({kind:'glyph',phase:'runtime',scene:1,a:16,b:400,
        c:character.codePointAt(0),d:character.codePointAt(0),flags:33,source:'ascii-atlas',first:2,last:2})),
      {kind:'raster',phase:'runtime',scene:1,a:16,b:400,encoding:'utf8',textHex:hex(value),first:3,last:3}];
    }),{kind:'present',phase:'runtime',scene:1,first:4,last:4,count:1,a:1,b:1}]});
  const scene=(id,language,identity,mounted,text,translated)=>({id,language,status:'observed',
    preferenceKey:normalGameLanguagePreferenceKey('th08'),preference:language,
    languageChoices:['ja','lang_en',partialIdentity.language],browserEvents:[],responseIntegrity:[{phase:'online',responses:[],errors:[]}],
    runtime:{url:'https://127.0.0.1/runtime/th08/'+ 'a'.repeat(64)+'/th08.html?runtimeEpoch=1',online:id!=='japanese-offline'},
    host:{enables:[{epoch:1,appBefore:0,at:1}],configures:[{epoch:1,parentAt:2,pack:identity?{language,bytes:identity.bytes,sha256:identity.sha256}:null}],
      events:[{event:'ready',epoch:1,at:1},{event:'first-frame',epoch:1,at:3}],errors:[],...(mounted?{mounted}:{})},
    trace:trace(text,translated),assessment:{status:'observed',baseline:{wrong:'ignored'}}});
  const scenes=[scene('japanese-offline','ja',null,null,original,()=>false),
    scene('translated-lang_en','lang_en',fullIdentity,fullMembers,values,()=>true),
    scene('partial-key',partialIdentity.language,partialIdentity,partialMembers,{...values,[profile.missingKey]:original[profile.missingKey]},key=>key!==profile.missingKey)];
  for(const id of ['network-failure','corrupt-archive']){
    const value=scene(id,'lang_en',null,null,original,()=>false),control=languageFailureResponse(full.archive,{kind:id});
    value.control={...control,body:undefined,url:'https://127.0.0.1/games/th08/language/lang_en.zip',
      published:{bytes:fullIdentity.bytes,sha256:fullIdentity.sha256}};
    const responses=[{url:value.control.url,status:control.status,contentIdentity:'passed',fromServiceWorker:false,
      actual:{bytes:control.bytes,sha256:control.sha256}}];
    value.responseIntegrity=[{phase:'online',responses,errors:[]}];value.control.responses=responses;scenes.push(value);
  }
  const ids=['LANG-02','LANG-03','LANG-04','LANG-06'];
  const checks=ids.map(id=>({id,status:'observed',complete:true,evidenceRefs:['/targets/0/languageEvidence'],
    observation:{...product,adaptationIds:[id],observation:{sceneIds:[]}}}));
  const target={...product,status:'passed',scenes,checks,
    languageEvidence:{...product,adaptationIds:ids,observation:{scenes}}};
  const catalog=save('catalog.json',Buffer.from(JSON.stringify({schema:'eagler-touhou/thcrap-static-catalog/1',game:'th08',languages:[full.catalog]})));
  const driver={schema:'eagler-touhou/language-browser/1',runId:'synthetic-language-proof',startedAt:'2026-10-24T00:00:00.000Z',
    finishedAt:'2026-10-24T00:00:01.000Z',status:'passed',passed:true,targets:[target],
    inputs:{languages:{...product,catalog,packs:{lang_en:{...fullIdentity,members:fullMembers}}}},
    preparation:{partialInput:partialIdentity},site:{base:'https://127.0.0.1',runtimeGroup:{root:'runtime/th08/',current:{generation:'a'.repeat(64),files:[{path:profile.browserDiagnostics.runtimeAsset}]}}}};
  return{driver,target,checks,fullIdentity,partialIdentity,entry:id=>checks.find(check=>check.id===id)};
}

test('language completeness reopens real producer archives and recomputes native traces and fallback controls',t=>{
  const f=fixture(t);
  for(const entry of f.checks)assert.doesNotThrow(()=>assertLanguageEvidence(f.driver,entry,product));
  const result=assessDriverReport({...product,group:'language',checksMapped:f.checks.map(row=>row.id),scenarioIds:[]},
    {status:'passed',code:0},f.driver);
  assert.ok(result.checks.every(row=>row.complete),JSON.stringify(result.issues));
});

const corruptions={
  'cached translation on failed download':['LANG-04',f=>{f.target.scenes.find(s=>s.id==='network-failure').host.configures[0].pack={language:'lang_en'};}],
  'lost retry preference':['LANG-04',f=>{f.target.scenes.find(s=>s.id==='corrupt-archive').preference='ja';}],
  'unobserved fault request':['LANG-04',f=>{const s=f.target.scenes.find(s=>s.id==='network-failure');s.responseIntegrity=[];s.control.responses=[];}],
  'forged corrupt-byte success':['LANG-04',f=>{f.target.scenes.find(s=>s.id==='corrupt-archive').responseIntegrity[0].responses[0].actual.sha256=f.fullIdentity.sha256;}],
  'fault served by cache':['LANG-04',f=>{f.target.scenes.find(s=>s.id==='network-failure').responseIntegrity[0].responses[0].fromServiceWorker=true;}],
  'unrelated request failure':['LANG-04',f=>{f.target.scenes.find(s=>s.id==='network-failure').browserEvents.push({type:'requestfailed',url:'https://127.0.0.1/unrelated',error:'net::ERR_ABORTED'});}],
  'changed native missing-key text':['LANG-06',f=>{f.target.scenes.find(s=>s.id==='partial-key').trace.events.find(e=>e.kind==='lookup'&&e.textHex===hex(profile.missingKey)).valueHex=hex('Wrong');}],
  'present translation dropped with missing key':['LANG-06',f=>{f.target.scenes.find(s=>s.id==='partial-key').trace.events.find(e=>e.kind==='lookup'&&e.textHex!==hex(profile.missingKey)).flags=0;}],
  'missing actual raster':['LANG-06',f=>{const s=f.target.scenes.find(s=>s.id==='partial-key');s.trace.events=s.trace.events.filter(e=>e.kind!=='raster');}],
  'missing successful present':['LANG-03',f=>{const s=f.target.scenes.find(s=>s.id==='translated-lang_en');s.trace.events=s.trace.events.filter(e=>e.kind!=='present');}],
  'late configure':['LANG-03',f=>{f.target.scenes.find(s=>s.id==='translated-lang_en').host.configures[0].parentAt=4;}],
  'wrong mounted bytes':['LANG-02',f=>{f.target.scenes.find(s=>s.id==='translated-lang_en').host.mounted[profile.table].sha256='0'.repeat(64);}],
  'missing selector language':['LANG-02',f=>{f.target.scenes.find(s=>s.id==='translated-lang_en').languageChoices=['ja'];}],
  'changed archive after capture':['LANG-02',f=>{writeFileSync(f.fullIdentity.path,Buffer.from('changed'));}],
};
for(const [name,[id,mutate]] of Object.entries(corruptions))test(`language evidence rejects ${name}`,t=>{
  const f=fixture(t);mutate(f);assert.throws(()=>assertLanguageEvidence(f.driver,f.entry(id),product));
});
