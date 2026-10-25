import assert from 'node:assert/strict';
import {readFileSync} from 'node:fs';
import {createHash} from 'node:crypto';
import {PRODUCT_GAMES} from '../../../common/contracts/product-catalog.mjs';
import {resolveAdaptationAdapter} from './registry.mjs';
import {decodeLanguageArchive} from '../../../tools/ports/language-inputs.mjs';
import {decodeLanguageTable} from '../../../tools/ports/language-tables.mjs';
import {assessLanguageScene} from '../../../tools/ports/language-contract.mjs';
import {languageFailureResponse,classifyLanguageBrowserEvents,resolveLanguageRuntimeSource} from '../../../tools/ports/language-network-contract.mjs';
import {normalGameLanguagePreferenceKey} from '../../../tools/ports/language-selection-contract.mjs';
import {assertLanguageConsumerEvidence} from './language-consumer-evidence.mjs';

const accepted=new Set(['LANG-01','LANG-02','LANG-03','LANG-04','LANG-05','LANG-06']);
const hash=bytes=>createHash('sha256').update(bytes).digest('hex');
function readBoundInput(receipt,max=64*1024*1024){
  assert.ok(receipt&&Number.isSafeInteger(receipt.bytes)&&receipt.bytes>0&&receipt.bytes<=max,'Missing bounded language input');
  const bytes=readFileSync(receipt.path);
  assert.equal(bytes.length,receipt.bytes,'Language input size changed');
  assert.equal(hash(bytes),receipt.sha256,'Language input bytes changed');return bytes;
}

function assertHost(scene,{product,profile,pack,original=false}){
  assert.equal(scene.status,'observed','Required language scene did not complete');
  assert.equal(scene.preferenceKey,normalGameLanguagePreferenceKey(product.game),'Wrong durable preference key');
  assert.equal(scene.preference,scene.language,'Language preference did not survive the scene');
  const host=scene.host,url=new URL(scene.runtime.url),epoch=Number(url.searchParams.get('runtimeEpoch'));
  assert.ok(Number.isSafeInteger(epoch)&&epoch>0,'Language Runtime epoch is missing');
  assert.equal(host.enables?.length,1);assert.equal(host.configures?.length,1);assert.deepEqual(host.errors,[]);
  assert.equal(host.enables[0].epoch,epoch);assert.equal(host.enables[0].appBefore,0,'Language observer was enabled after game construction');
  const configure=host.configures[0],first=host.events.find(event=>event.event==='first-frame');
  assert.ok(first&&host.events.some(event=>event.event==='ready'),'Language scene has no ready and presented frame');
  assert.equal(configure.epoch,epoch);assert.equal(first.epoch,epoch);
  assert.ok(configure.parentAt>=host.enables[0].at&&first.at>=configure.parentAt,'Language configure missed the first-frame boundary');
  if(original){assert.equal(configure.pack,null,'Fallback retained a translation pack');}
  else{
    assert.equal(configure.pack?.language,scene.language);
    assert.equal(configure.pack?.sha256,pack.identity.sha256,'Configured archive identity differs');
    assert.equal(configure.pack?.bytes,pack.identity.bytes);
    assert.deepEqual(host.mounted,pack.members,'Actual mounted language bytes differ from the independent archive');
    assert.ok(pack.files.some(file=>file.path===profile.table),'Declared language table is missing');
  }
}

/** Raw language traces and independently decoded archives establish fallback rules. */
export function assertLanguageEvidence(driver,entry,product,{readInput=readBoundInput}={}){
  assert.equal(driver.schema,'eagler-touhou/language-browser/1');
  const targets=driver.targets?.filter(target=>target.game===product.game&&target.variant===product.variant)??[];
  assert.equal(targets.length,1,'Language evidence must select one matching product');
  const target=targets[0];assert.ok(target.checks.includes(entry)&&target.checks.filter(row=>row.id===entry.id).length===1,'Language claim is not unique');
  const adapter=resolveAdaptationAdapter(product.game,product.variant),profile=adapter?.language;
  assert.ok(profile,'Register title language observations');
  assert.deepEqual(target.languageEvidence?.observation?.scenes,target.scenes,'Raw language scenes differ from the proof-bound observation');
  const scene=id=>{
    const matches=target.scenes.filter(value=>value.id===id);assert.equal(matches.length,1,`Missing or duplicate language scene ${id}`);return matches[0];
  };
  const baselineScene=scene('japanese-offline');
  assertHost(baselineScene,{product,profile,original:true});
  assert.equal(baselineScene.runtime.online,false,'Japanese baseline must be a cold offline launch');
  const baseline=assessLanguageScene({trace:baselineScene.trace,profile,original:true});
  assert.equal(baseline.status,'observed','Independent Japanese raster baseline is incomplete');
  const inputs=driver.inputs.languages;
  assert.equal(inputs.game,product.game);assert.equal(inputs.variant,product.variant);
  const catalog=JSON.parse(readInput(inputs.catalog).toString('utf8'));
  const declarations=catalog.schema==='eagler-touhou/site-manifest/1'?catalog.games[product.game].languages:catalog.languages;
  if(catalog.schema!=='eagler-touhou/site-manifest/1')assert.equal(catalog.game,product.game);
  const packs={};
  for(const [language,receipt] of Object.entries(inputs.packs)){
    const bytes=readInput(receipt),declaration=declarations.find(value=>value.id===language)?.pack;
    assert.ok(declaration,'Selected language archive is not published');
    assert.equal(declaration.sha256,hash(bytes));assert.equal(declaration.bytes,bytes.length);
    const decoded=decodeLanguageArchive(bytes,{game:product.game,language});
    assert.deepEqual(decoded.members,receipt.members,'Archive members differ from the recorded input identity');
    packs[language]={...decoded,bytes,identity:{sha256:hash(bytes),bytes:bytes.length}};
  }
  const expected=pack=>{
    const table=pack.files.find(file=>file.path===profile.table);assert.ok(table,'Language sample table is absent');return decodeLanguageTable(table.bytes);
  };
  const reassess=(value,pack,original=false)=>{
    assertHost(value,{product,profile,pack,original});
    const result=assessLanguageScene({trace:value.trace,profile,expected:original?{entries:[]}:expected(pack),baseline:baseline.baseline});
    assert.equal(result.status,'observed','Language raw text/glyph/presentation proof is incomplete');
    const runtimeSource=resolveLanguageRuntimeSource({browserDiagnostics:adapter.language.browserDiagnostics,
      runtimeGroup:driver.site.runtimeGroup,productRuntimeAssets:PRODUCT_GAMES[product.game].runtimeAssets,base:driver.site.base});
    assert.ok(Array.isArray(value.responseIntegrity)&&value.responseIntegrity.length,'Language scene has no network capture');
    for(const group of value.responseIntegrity)assert.deepEqual(group.errors,[],'Language response capture failed');
    const responses=value.responseIntegrity.flatMap(group=>group.responses);
    const classified=classifyLanguageBrowserEvents(value.browserEvents,{responses,runtimeSource,
      classifyDiagnostic:adapter.classifyLanguageDiagnostic,
      fontPaths:Object.keys(value.host.mounted??{}).filter(path=>path.startsWith(`/thcrap/${product.game}/fonts/`))});
    assert.deepEqual(classified.blocking,[],'Unexpected language browser errors');
    return result;
  };
  if(entry.id==='LANG-01'){
    assert.equal(entry.complete,true);assertLanguageConsumerEvidence({driver,scene:baselineScene,adapter,expected:{entries:[]},original:true});
  }else if(entry.id==='LANG-05'){
    assert.equal(entry.complete,false);assert.equal(entry.machineComplete,true);
    assertLanguageConsumerEvidence({driver,scene:baselineScene,adapter,expected:{entries:[]},original:true});
    for(const [language,pack]of Object.entries(packs)){
      const value=scene(`translated-${language}`);reassess(value,pack);assertLanguageConsumerEvidence({driver,scene:value,adapter,expected:expected(pack),original:false});
    }
  }else if(entry.id==='LANG-02'||entry.id==='LANG-03'){
    const published=declarations.filter(value=>value.pack).map(value=>value.id).sort();
    assert.deepEqual(Object.keys(packs).sort(),published,'A published language was omitted from the acceptance input');
    for(const [language,pack] of Object.entries(packs)){
      const value=scene(`translated-${language}`);reassess(value,pack);
      const choices=[...value.languageChoices].sort();
      assert.deepEqual(choices,['ja',...published,driver.preparation.partialInput.language].sort(),'Published selector language matrix differs');
    }
  }else if(entry.id==='LANG-04'){
    for(const id of ['network-failure','corrupt-archive']){
      const value=scene(id),pack=packs[value.language];assert.ok(pack,'Failure scene selected another archive');
      reassess(value,pack,true);
      const control=value.control,expectedControl=languageFailureResponse(pack.bytes,{kind:id});
      for(const key of ['kind','purpose','status','bytes','sha256'])assert.equal(control[key],expectedControl[key],`Fault ${key} differs from controlled source bytes`);
      assert.equal(control.published.sha256,pack.identity.sha256);assert.equal(control.published.bytes,pack.identity.bytes);
      assert.deepEqual(control.responses,value.responseIntegrity.flatMap(group=>group.responses).filter(row=>row.url===control.url),'Controlled responses are detached from the browser network log');
      assert.ok(control.responses.length,'Failure injection was bypassed by cache');
      for(const response of control.responses){
        assert.equal(response.status,expectedControl.status);assert.equal(response.contentIdentity,'passed');
        assert.equal(response.fromServiceWorker,false,'Failure request was fulfilled by a previous cache');
        if(id==='network-failure'&&response.semanticReceipt?.bodyConsumption==='not-read'){
          assert.equal(response.semanticReceipt.purpose,expectedControl.purpose);assert.equal(response.transport?.event,'loadingFailed');
        }else{assert.equal(response.actual.bytes,expectedControl.bytes);assert.equal(response.actual.sha256,expectedControl.sha256);}
      }
    }
  }else if(entry.id==='LANG-06'){
    const receipt=driver.preparation.partialInput,bytes=readInput(receipt);
    const partial={...decodeLanguageArchive(bytes,{game:product.game,language:receipt.language}),identity:{bytes:bytes.length,sha256:hash(bytes)}};
    const table=expected(partial),kept=profile.samples.filter(sample=>sample.key!==profile.missingKey);
    assert.ok(kept.length>0&&kept.every(sample=>table.entries.some(entry=>entry.key===sample.key&&entry.hasTranslation)),
      'Partial control removed the present translated samples');
    assert.ok(!table.entries.some(entry=>entry.key===profile.missingKey&&entry.hasTranslation),'Partial control still translates the missing key');
    reassess(scene('partial-key'),partial);
  }
}

export function assessRuntimeLanguageEvidence({driver,entry,product}={}){
  if(!accepted.has(entry?.id)||driver?.schema!=='eagler-touhou/language-browser/1')return{available:false,valid:false,kind:null};
  try{assertLanguageEvidence(driver,entry,product);return{available:true,valid:true,kind:'native-language-rules',...(entry.id==='LANG-05'?{humanRequired:true}:{})};}
  catch(error){return{available:true,valid:false,kind:'native-language-rules',reason:error.message};}
}
