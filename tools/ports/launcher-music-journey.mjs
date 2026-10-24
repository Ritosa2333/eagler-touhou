import assert from 'node:assert/strict';
import {mkdir,writeFile} from 'node:fs/promises';
import {resolve} from 'node:path';
import {classifyLanguageBrowserEvents} from './language-network-contract.mjs';
import {prepareLauncherRuntimeFixture,verifyLauncherFixtureMusic} from './launcher-runtime-fixture.mjs';
import {chooseSetting} from './launcher-ui-controls.mjs';
import {availableFixturePort,startStaticServer,stopChild} from './launcher-fixture-preparation.mjs';
import {createOfflineStaticTlsProxy,captureOfflineSiteIdentity,watchOfflineSiteResponses,verifyOfflineSiteIdentity} from './offline-runtime-https.mjs';
import {installOfflinePackageStoreObserver} from './offline-package-observer.mjs';
import {PACKAGE_STORE_DB,PACKAGE_STORE_DB_VERSION,PACKAGE_INSTALLATIONS,PACKAGE_GENERATIONS,PACKAGE_OBJECTS} from '../../launcher/package/package-store.mjs';
import {verifyRuntimePackage,verifyPrivateData} from './acceptance-contracts.mjs';
import {verifyRoomResourcePostflight} from './room-resources.mjs';
import {dependencySnapshot} from '../testing/dependency-snapshot.mjs';
import {resolveAdaptationAdapter} from '../../tests/adaptation/adapters/registry.mjs';
import {summarizeRuntimePcmWindow,TH08_OGG_PCM_ROLE,TH08_MIDI_PCM_ROLE} from './music-browser-helpers.mjs';
import {correlateTh08MidiSends,th08MidiTitleOutcome,isVerifiedRuntimePcmWindow} from './music-browser-result.mjs';

export function matchesMusicLaunchConsent(mode,prompt){
  const normalize=value=>String(value??'').replace(/\s+/g,' ').trim();
  const messages={none:['将会在无音乐的情况下继续。 如果想听正常的音乐，请在音乐中选择 OGG。',
    'The game will continue without music. If you want normal music playback, choose OGG under Music.'],
    midi:['将会使用 MIDI 播放音乐。 如果想听正常的音乐，请在音乐中选择 OGG。',
      'The game will continue with MIDI music. If you want normal music playback, choose OGG under Music.']};
  return messages[mode]?.includes(normalize(prompt.message))===true&&['仍要启动','Start anyway'].includes(normalize(prompt.confirm));
}

/** Observe production messages and the actual vendor constructor; never creates a bridge or synth. */
export function installLauncherMusicObserver({game}){
  if(window!==window.top){
    addEventListener('message',event=>{
      const value=event.data;
      if(event.source!==parent||event.origin!==location.origin||value?.protocol!=='eagler-touhou/1'||value.game!==game||value.command!=='configure')return;
      const frame=parent.document.querySelector('#gameFrame');
      if(frame?.contentWindow!==window)return;
      parent.__launcherMusicReceipt?.configures.push({epoch:value.epoch,music:value.music,decodeMode:value.options?.oggDecodeMode,
        at:performance.timeOrigin+performance.now(),request:value.request});
    });return;
  }
  const receipt=window.__launcherMusicReceipt={events:[],configures:[],errors:[],synths:[],nativeEpochs:[],toasts:[],allSynthSends:[],transportStarted:false};
  window.__eaglerTinySynthSends=[];window.__eaglerMidiNativeEvents=[];
  const pending=new Set();
  window.__flushLauncherMusicFailures=async()=>{while(pending.size)await Promise.all([...pending]);};
  let constructor;
  Object.defineProperty(window,'WebAudioTinySynth',{configurable:true,get:()=>constructor,set:Real=>{
    constructor=new Proxy(Real,{construct(target,args){
      const synth=Reflect.construct(target,args,target),send=synth.send;
      window.__eaglerTinySynth=synth;
      synth.__eaglerMusicVoiceTap=window.__eaglerMusicPcmProbe?.attachTap?.(synth.out,'launcher-tinysynth-voice','tinysynth-voice-output')??null;
      receipt.synths.push({at:Date.now(),constructor:Real.name});
      synth.send=function(message,...rest){const bytes=Array.from(message??[]),result=send.call(this,message,...rest);
        receipt.allSynthSends.push(bytes);if(receipt.transportStarted)window.__eaglerTinySynthSends.push(bytes);return result;};
      return synth;
    }});
  }});
  const bound=new WeakSet();
  addEventListener('message',event=>{
    const iframe=document.querySelector('#gameFrame'),value=event.data;
    if(event.origin!==location.origin||event.source!==iframe?.contentWindow||value?.protocol!=='eagler-touhou/1'||value.game!==game)return;
    const epoch=Number(new URL(iframe.src,location.href).searchParams.get('runtimeEpoch'));
    if(!Number.isSafeInteger(epoch)||epoch<1||value.epoch!==epoch)return;
    receipt.events.push({...value,receivedAt:Date.now()});
    if(value.ok===false||['error','abort'].includes(value.event)){
      receipt.errors.push(value);const capture=Promise.resolve(window.__captureLauncherMusicFailure(value));pending.add(capture);
      void capture.then(()=>pending.delete(capture),error=>{receipt.errors.push({error:String(error)});pending.delete(capture);});
    }
    if(value.event==='ready'&&!bound.has(event.source.document)){
      bound.add(event.source.document);receipt.nativeEpochs.push(epoch);
      event.source.addEventListener('touhou-midi',midi=>{receipt.transportStarted=true;window.__eaglerMidiNativeEvents.push({bytes:Array.from(midi.detail?.bytes??[]),epoch,time:midi.detail?.time??0});},true);
    }
  });
  addEventListener('DOMContentLoaded',()=>{
    new MutationObserver(()=>{
      const node=document.querySelector('#toastText'),text=node?.textContent?.trim();
      if(text&&receipt.toasts.at(-1)?.text!==text)receipt.toasts.push({text,at:Date.now()});
    }).observe(document.documentElement,{subtree:true,childList:true,characterData:true,attributes:true});
  },{once:true});
}

/** Delete one verified optional object in this isolated context, retaining its installed declaration and reference. */
export async function deleteInstalledTitleOgg({game,input,dbName,objectsStore}){
  const observer=window.__offlinePackageStoreObserver;
  const before=await observer.readCurrentPackageGeneration(game),generation=before.generation;
  if(!generation)throw Error('No imported generation is available for the OGG fault');
  const component=generation.descriptor.components.ogg;
  const firstIds=Array.isArray(component?.files)?component.files.slice(0,2):[];
  const fileId=firstIds.find(id=>generation.descriptor.files[id]?.target===input.target);
  if(!fileId)throw Error('Registered title OGG must belong to the actual first two startup resources');
  const declaration=generation.descriptor.files[fileId],objectId=generation.files[fileId]?.objectId;
  if(!objectId||generation.descriptor.base.files.includes(fileId))throw Error('The selected OGG is not an installed optional object');
  const hash=async blob=>{const buffer=await blob.arrayBuffer();return{bytes:buffer.byteLength,sha256:Array.from(new Uint8Array(await crypto.subtle.digest('SHA-256',buffer)),n=>n.toString(16).padStart(2,'0')).join('')};};
  const original=await observer.readPackageObject(objectId);if(!original?.blob)throw Error('Fault object was absent before injection');
  const beforeIdentity=await hash(original.blob);
  if(beforeIdentity.bytes!==input.bytes||beforeIdentity.sha256!==input.sha256||declaration.bytes!==input.bytes||declaration.sha256!==input.sha256)throw Error('Fault object differs from the private OGG source');
  const dataId=generation.descriptor.runtimeRequirement.dataFile,dataObjectId=generation.files[dataId]?.objectId;
  if(dataObjectId===objectId)throw Error('Optional OGG cannot share the DATA object');
  const dataBefore=await observer.readPackageObject(dataObjectId);if(!dataBefore?.blob)throw Error('Imported DATA is unavailable');
  const dataIdentityBefore=await hash(dataBefore.blob);
  await new Promise((accept,reject)=>{
    const request=indexedDB.open(dbName);request.onupgradeneeded=()=>{request.transaction.abort();reject(Error('Fault cannot create Package Store'));};request.onerror=()=>reject(request.error);
    request.onsuccess=()=>{const db=request.result,transaction=db.transaction([objectsStore],'readwrite');transaction.objectStore(objectsStore).delete(objectId);
      transaction.oncomplete=()=>{db.close();accept();};transaction.onabort=transaction.onerror=()=>{db.close();reject(transaction.error??Error('Fault deletion failed'));};};
  });
  const afterObject=await observer.readPackageObject(objectId),after=await observer.readCurrentPackageGeneration(game);
  const dataAfter=await observer.readPackageObject(dataObjectId);if(!dataAfter?.blob)throw Error('Fault deleted DATA');
  return{at:Date.now(),fileId,target:input.target,objectId,firstIds,beforeIdentity,expectedIdentity:{bytes:input.bytes,sha256:input.sha256},
    afterObjectAbsent:afterObject===null,beforeGeneration:generation,afterDeletionGeneration:after.generation,
    dataIdentityBefore,dataIdentityAfter:await hash(dataAfter.blob)};
}

export function assessLauncherMusicFault(scene){
  const fault=scene?.fault,missing=[],failures=[];
  const same=(a,b)=>JSON.stringify(a)===JSON.stringify(b);
  if(!fault)return{status:'incomplete',passed:false,missing:['Verified isolated object deletion is absent'],failures:[]};
  else{
    if(fault.afterObjectAbsent!==true)failures.push('The actual OGG object was not deleted');
    if(!fault.objectId||!fault.fileId||!fault.firstIds?.includes(fault.fileId))failures.push('Fault does not identify a startup OGG object');
    if(!Number.isSafeInteger(fault.beforeIdentity?.bytes)||fault.beforeIdentity.bytes<=0||!/^[a-f0-9]{64}$/.test(fault.beforeIdentity?.sha256??'')||!same(fault.beforeIdentity,fault.expectedIdentity))failures.push('Deleted OGG bytes differ from the private source');
    const generation=fault.beforeGeneration,ref=generation?.files?.[fault.fileId],declaration=generation?.descriptor?.files?.[fault.fileId];
    if(typeof generation?.id!=='string'||ref?.objectId!==fault.objectId||declaration?.target!==fault.target||declaration?.sha256!==fault.beforeIdentity?.sha256||declaration?.bytes!==fault.beforeIdentity?.bytes)failures.push('Fault is not bound to the original installed OGG reference');
    if(!same(fault.beforeGeneration,fault.afterDeletionGeneration)||!same(fault.beforeGeneration,fault.afterLaunchGeneration))failures.push('Fault changed the installed generation, descriptor or references');
    if(!Number.isSafeInteger(fault.dataIdentityBefore?.bytes)||fault.dataIdentityBefore.bytes<=0||!/^[a-f0-9]{64}$/.test(fault.dataIdentityBefore?.sha256??''))failures.push('The imported DATA identity is unavailable');
    if(!same(fault.dataIdentityBefore,fault.dataIdentityAfter)||!same(fault.dataIdentityBefore,fault.dataIdentityAfterLaunch))failures.push('Fault changed the actual DATA bytes');
    const name=fault.target?.split('/').at(-1);
    const toast=scene.host?.toasts?.find(item=>item.at>=fault.at&&/OGG/i.test(item.text)&&/MIDI/i.test(item.text)&&name&&item.text.includes(name));
    if(!toast)failures.push('No actual resource-specific OGG to MIDI fallback toast was observed');
  }
  if(scene?.moduleMode!=='midi'||scene?.midiOutcome?.passed!==true)failures.push('The production fallback did not provide verified actual MIDI output');
  if(scene?.preference?.music!=='ogg-stream'||scene?.postLaunchPreference?.music!=='ogg-stream')failures.push('Launch-only fallback rewrote the stored OGG preference');
  return{status:failures.length?'failed':missing.length?'incomplete':'passed',passed:!failures.length&&!missing.length,missing,failures};
}

export function assessLauncherMusicScene(scene){
  const missing=[],failures=[],mode=scene.mode,epoch=scene.runtime?.epoch,fallback=scene.scenario==='missing-object-fallback';
  const check=(observed,valid,message)=>{if(!observed)missing.push(message);else if(!valid)failures.push(message);};
  const configured=scene.host?.configures?.filter(item=>item.epoch===epoch)??[];
  const first=scene.host?.events?.filter(item=>item.event==='first-frame'&&item.epoch===epoch)??[];
  check(Number.isSafeInteger(epoch)&&epoch>0,configured.length===1&&first.length===1,'One actual Runtime epoch must bind configure and first-frame');
  check(scene.host?.nativeEpochs?.length,scene.host?.nativeEpochs?.length===1&&scene.host.nativeEpochs[0]===epoch,'Native observer must bind the launched epoch');
  check(Number.isFinite(configured[0]?.at)&&Number.isFinite(first[0]?.receivedAt),first[0]?.receivedAt>=configured[0]?.at,'Configure must precede first-frame');
  const transport=fallback?'midi':mode.startsWith('ogg-')?'ogg':mode;
  check(configured.length,configured[0]?.music===transport||transport==='ogg'&&configured[0]?.music==='midi','Configure must select the requested music transport');
  check(typeof scene.moduleMode==='string',scene.moduleMode===transport,'Actual Runtime Module must select the requested music mode');
  check(scene.preference&&typeof scene.reloadedPreference==='string',scene.preference?.music===mode&&scene.preference.musicPreferenceExplicit===true&&scene.reloadedPreference===mode,'Music preference must survive a Launcher reload');
  if(mode==='none'||mode==='midi')check(scene.launchConsent,scene.launchConsent?.confirmed===true&&matchesMusicLaunchConsent(mode,scene.launchConsent),
    'The selected-mode production launch warning must be confirmed');
  if(scene.host?.errors?.length||scene.protocolErrors?.length||scene.browserErrors?.length||scene.responseErrors?.length)failures.push('Browser, protocol or response identity errors were recorded');
  const role=mode==='midi'||fallback?TH08_MIDI_PCM_ROLE:TH08_OGG_PCM_ROLE;
  const pcmReady=isVerifiedRuntimePcmWindow(scene.pcm,role,{minimumWindows:8});
  if(scene.pcm?.status==='complete'&&!pcmReady)failures.push('The reported PCM summary does not match complete source-bound running windows');
  if(mode.startsWith('ogg-')&&!fallback){
    check(configured.length,configured[0]?.decodeMode===(mode==='ogg-full'?'full':'stream'),'Configured OGG decode policy must match the menu');
    check(scene.audio?.decoder,scene.audio?.decoder?.decodeMode===(mode==='ogg-full'?1:0),'Native OGG decode policy must match the menu');
    check(typeof scene.sourceIdentityMatched==='boolean',scene.sourceIdentityMatched===true,'Native OGG bytes must match the registered source');
    check(pcmReady,scene.pcm?.outputObserved===true,'Eight running source-bound OGG PCM windows must contain output');
    check(scene.audio?.musicObservation,scene.audio?.musicObservation?.playing===true,'The identified native OGG source must be playing');
    if(scene.audio?.midiObservation?.active===true||scene.audio?.midiHost?.nativeEvents?.length)failures.push('OGG selection also emitted native MIDI');
  }else if(mode==='midi'||fallback){
    const host=scene.audio?.midiHost,correlation=correlateTh08MidiSends(host?.nativeEvents,host?.sentMessages);
    const output=th08MidiTitleOutcome({native:scene.audio?.midiObservation,events:correlation.events,sendCorrelation:correlation,
      pcm:scene.pcm,contextState:host?.contextState});
    check(scene.audio?.midiObservation&&host&&pcmReady,output.passed&&scene.midiOutcome?.passed===true,'Native MIDI, production synth sends and source-bound PCM must agree');
    if(scene.audio?.midiHost?.nativeEvents?.some(event=>event.epoch!==epoch))failures.push('MIDI observations crossed Runtime epochs');
  }else if(mode==='none'){
    check(scene.audio?.decoder&&scene.audio?.midiObservation,scene.audio?.decoder?.enabled===false&&scene.audio?.midiObservation?.active===false&&!scene.audio?.midiHost?.nativeEvents?.length,'Native BGM owners must be disabled');
    check(pcmReady,scene.pcm?.outputObserved===false&&scene.pcm.nonZeroSamples===0,'Eight running Runtime PCM windows must be quiet after settling');
  }else missing.push('Unsupported title-owned visible music mode');
  if(fallback){check(configured.length,configured[0]?.music==='midi'&&configured[0]?.decodeMode==='stream','Fallback must preserve the actual stream configure sentinel');const fault=assessLauncherMusicFault(scene);missing.push(...fault.missing);failures.push(...fault.failures);}
  return{status:failures.length?'failed':missing.length?'incomplete':'passed',passed:!failures.length&&!missing.length,missing,failures};
}

/** Real Launcher UI scenes. A supplied browser remains caller-owned; callback-created browsers close here. */
export async function runLauncherMusicJourney({root,output,target,identity,dataIdentity,shared,artwork,packageMap,musicInputs,
  browser,pcmProbeSource,readAudioState,launchBrowser}){
  await mkdir(output,{recursive:true});
  const report={schema:'eagler-touhou/launcher-music-journey/1',game:target.game,variant:target.variant,scenes:[],
    resourceFault:{status:'incomplete',passed:false,reason:'No controlled failure of a declared production OGG component has been selected'},cleanupErrors:[]};
  let prepared,server,tls,snapshot,ownedBrowser;
  const save=()=>writeFile(resolve(output,'launcher-music-journey.json'),JSON.stringify(report,null,2)+'\n');
  try{
    assert.equal(target.game,'th08','Register a title-owned native music assessment before selecting another game');
    assert.ok(musicInputs,'Complete, hash-bound OGG source inputs and an OGG-bearing ZIP are required');
    assert.ok(typeof pcmProbeSource==='string'&&typeof readAudioState==='function','Production-output observers are required');
    report.dependencies=await dependencySnapshot(root,['tools/ports/launcher-music-journey.mjs']);
    report.injectedObservers={pcmProbeSource,readAudioState:readAudioState.toString(),hostObserver:installLauncherMusicObserver.toString()};
    prepared=await prepareLauncherRuntimeFixture({root,output,target,identity,dataIdentity,shared,artwork,packageMap,musicInputs});
    report.preparation={site:prepared.site,packaging:prepared.packaging,resource:prepared.resource,music:prepared.music};
    server=await startStaticServer(prepared.site,await availableFixturePort());
    tls=await createOfflineStaticTlsProxy({backendBase:server.baseUrl});snapshot=await captureOfflineSiteIdentity(prepared.site);
    report.site={base:tls.base,tlsIdentity:tls.tlsIdentity,identity:snapshot.identity};
    // A browser created earlier cannot trust a certificate whose SPKI is generated here.
    if(launchBrowser){ownedBrowser=await launchBrowser({tlsIdentity:tls.tlsIdentity});browser=ownedBrowser;}
    assert.ok(browser,'Supply launchBrowser({tlsIdentity}) or an already certificate-configured browser');
    const runtime=resolveAdaptationAdapter(target.game,'normal').runtime;
    for(const scenario of ['none','midi','ogg-stream','ogg-full','missing-object-fallback']){
      const fallback=scenario==='missing-object-fallback',mode=fallback?'ogg-stream':scenario,effectiveMidi=mode==='midi'||fallback;
      const scene={scenario,mode,status:'running',browserErrors:[],protocolErrors:[]};report.scenes.push(scene);await save();
      let context,watcher,page;
      try{
        context=await browser.newContext({viewport:{width:1280,height:900}});
        await context.addInitScript({content:'window.__eaglerMusicPcmProbeTop=true;\n'+pcmProbeSource});
        await context.addInitScript(installLauncherMusicObserver,{game:target.game});
        await context.addInitScript(installOfflinePackageStoreObserver,{name:PACKAGE_STORE_DB,version:PACKAGE_STORE_DB_VERSION,
          installations:PACKAGE_INSTALLATIONS,generations:PACKAGE_GENERATIONS,objects:PACKAGE_OBJECTS});
        page=await context.newPage();
        await page.exposeBinding('__captureLauncherMusicFailure',(source,value)=>{
          if(source.page!==page||source.frame!==page.mainFrame())scene.protocolErrors.push({error:'Protocol mirror came from another page/frame'});
          else scene.protocolErrors.push(value);
        });
        page.on('pageerror',error=>scene.browserErrors.push({type:'pageerror',error:String(error)}));
        page.on('console',message=>{if(message.type()==='error')scene.browserErrors.push({type:'console',text:message.text(),location:message.location()});});
        page.on('requestfailed',request=>scene.browserErrors.push({type:'requestfailed',url:request.url(),error:request.failure()?.errorText}));
        page.on('response',response=>{if(response.status()>=400)scene.browserErrors.push({type:'http',url:response.url(),status:response.status()});});
        watcher=await watchOfflineSiteResponses(page,{base:tls.base,snapshot,appShell:prepared.deployment.appShell,output,label:'music-ui-'+scenario,resourceRouteDiscovery:true});
        const boot=async()=>{
          await page.waitForFunction(()=>window.__eaglerBoot?.done===true,null,{timeout:60000});
          if(await page.locator('#firstUseNoticeDialog').evaluate(node=>node.open))await page.locator('#firstUseNoticeClose').click();
        };
        await page.goto(new URL('?game='+target.game,tls.base).href);await boot();
        await page.locator('#gamePackageImport').click();await page.locator('#gameDataImportInput').setInputFiles(resolve(packageMap[target.game]));
        await page.waitForFunction(()=>{const node=document.querySelector('#gameDataImportWindow');return node&&node.hidden&&node.getAttribute('aria-busy')!=='true';},null,{timeout:180000});
        scene.installation=await page.evaluate(async game=>{
          const observer=window.__offlinePackageStoreObserver,{generation}=await observer.readCurrentPackageGeneration(game);
          if(!generation)throw Error('Launcher UI import did not install a package');
          const fileId=generation.descriptor.runtimeRequirement.dataFile,ref=generation.files[fileId];
          const object=await observer.readPackageObject(ref.objectId),bytes=await object.blob.arrayBuffer();
          const sha256=Array.from(new Uint8Array(await crypto.subtle.digest('SHA-256',bytes)),n=>n.toString(16).padStart(2,'0')).join('');
          return{generationId:generation.id,revision:generation.descriptor.revision,bytes:bytes.byteLength,sha256};
        },target.game);
        assert.equal(scene.installation.sha256,dataIdentity.sha256);assert.equal(scene.installation.bytes,dataIdentity.bytes);
        scene.choices=await page.locator('#musicSelect option').evaluateAll(nodes=>nodes.map(node=>({value:node.value,disabled:node.disabled})));
        assert.ok(scene.choices.some(choice=>choice.value===mode&&!choice.disabled),'Selected music mode is unavailable in the production menu');
        const alternative=mode==='none'?'midi':'none';await chooseSetting(page,'musicSelect',alternative);await chooseSetting(page,'musicSelect',mode);
        const key='eagler-touhou-game-options-v1-'+target.game;
        scene.preference=await page.evaluate(key=>JSON.parse(localStorage.getItem(key)),key);
        await page.reload();await boot();scene.reloadedPreference=await page.locator('#musicSelect').inputValue();
        if(fallback){const file=musicInputs.files.find(item=>item.name==='th08_01.ogg');assert.ok(file,'The private title OGG identity is absent');
          scene.fault=await page.evaluate(deleteInstalledTitleOgg,{game:target.game,input:{target:musicInputs.mount+'/'+file.name,bytes:file.bytes,sha256:file.sha256},dbName:PACKAGE_STORE_DB,objectsStore:PACKAGE_OBJECTS});}
        await page.locator('#launch').click();
        if(mode==='none'||mode==='midi'){
          await page.waitForFunction(()=>document.querySelector('#decisionDialog')?.open===true||window.__launcherMusicReceipt.errors.length>0,null,{timeout:30000});
          scene.launchConsent=await page.evaluate(()=>({open:document.querySelector('#decisionDialog')?.open===true,
            message:document.querySelector('#decisionMessage')?.textContent,confirm:document.querySelector('#decisionConfirm')?.textContent,confirmed:false}));
          assert.ok(scene.launchConsent.open&&matchesMusicLaunchConsent(mode,scene.launchConsent),'Unexpected Launcher music decision');
          await page.locator('#decisionConfirm').click();
          await page.waitForFunction(()=>document.querySelector('#decisionDialog')?.open!==true,null,{timeout:15000});scene.launchConsent.confirmed=true;
        }
        await page.waitForFunction(()=>window.__launcherMusicReceipt.events.some(event=>event.event==='first-frame')||
          window.__launcherMusicReceipt.errors.length>0||document.querySelector('#decisionDialog')?.open===true,null,{timeout:180000});
        const launchBoundary=await page.evaluate(()=>({errors:window.__launcherMusicReceipt.errors,
          decision:document.querySelector('#decisionDialog')?.open===true,message:document.querySelector('#decisionMessage')?.textContent}));
        assert.equal(launchBoundary.errors.length,0,'Runtime failed before first-frame: '+JSON.stringify(launchBoundary.errors));
        assert.equal(launchBoundary.decision,false,'Unexpected Launcher decision before first-frame: '+launchBoundary.message);
        const handle=await page.locator('#gameFrame').elementHandle(),frame=await handle.contentFrame();assert.ok(frame);
        const href=frame.url(),url=new URL(href),epoch=Number(url.searchParams.get('runtimeEpoch'));
        assert.ok(url.pathname.startsWith(`/runtime/${target.game}/${prepared.runtimeGroup.current.generation}/`),'Launcher selected another Runtime generation');
        scene.runtime={href,epoch};
        await page.waitForTimeout(1800);
        const windows=[];
        for(let sample=0;sample<60;sample++){
          scene.audio=await frame.evaluate(readAudioState,{game:target.game,includeRuntimePcm:true,includeMidiHost:true});
          const source=effectiveMidi?scene.audio.midiHost:scene.audio.runtimePcm;
          const targetPcm=effectiveMidi?source?.pcmTarget:source?.target;
          if(targetPcm?.available)windows.push({target:targetPcm,captures:source.captures});
          scene.pcm=summarizeRuntimePcmWindow(windows,{role:effectiveMidi?TH08_MIDI_PCM_ROLE:TH08_OGG_PCM_ROLE,expectedTarget:windows[0]?.target,minimumWindows:8});
          if(mode==='none'&&scene.pcm.status==='complete')break;
          if(effectiveMidi){
            const host=scene.audio.midiHost,correlation=correlateTh08MidiSends(host.nativeEvents,host.sentMessages);
            scene.midiOutcome=th08MidiTitleOutcome({native:scene.audio.midiObservation,events:correlation.events,sendCorrelation:correlation,pcm:scene.pcm,contextState:host.contextState});
            if(scene.midiOutcome.passed)break;
          }else if(scene.pcm.status==='complete'&&scene.pcm.outputObserved)break;
          await page.waitForTimeout(100);
        }
        scene.moduleMode=await frame.evaluate(mapping=>globalThis[mapping.global][mapping.module].touhouMusicMode,runtime);
        if(mode.startsWith('ogg-')&&!fallback){
          const name=scene.audio.musicObservation?.name?.replace(/\.wav$/i,'.ogg'),file=musicInputs.files.find(file=>file.name===name);
          assert.ok(file,'Native OGG track is not registered in the private input');
          scene.sourceIdentity=await frame.evaluate(async({runtime,path})=>{
            const bytes=globalThis[runtime.global][runtime.module].FS.readFile(path);
            return{bytes:bytes.length,sha256:Array.from(new Uint8Array(await crypto.subtle.digest('SHA-256',bytes)),n=>n.toString(16).padStart(2,'0')).join('')};
          },{runtime,path:musicInputs.mount+'/'+file.name});
          scene.sourceIdentityMatched=scene.sourceIdentity.bytes===file.bytes&&scene.sourceIdentity.sha256===file.sha256;
        }
        if(fallback){scene.postLaunchPreference=await page.evaluate(key=>JSON.parse(localStorage.getItem(key)),key);
          Object.assign(scene.fault,await page.evaluate(async game=>{const observer=window.__offlinePackageStoreObserver,{generation}=await observer.readCurrentPackageGeneration(game);
            const fileId=generation.descriptor.runtimeRequirement.dataFile,object=await observer.readPackageObject(generation.files[fileId].objectId),bytes=await object.blob.arrayBuffer();
            return{afterLaunchGeneration:generation,dataIdentityAfterLaunch:{bytes:bytes.byteLength,sha256:Array.from(new Uint8Array(await crypto.subtle.digest('SHA-256',bytes)),n=>n.toString(16).padStart(2,'0')).join('')}};},target.game));}
        scene.host=await page.evaluate(()=>window.__launcherMusicReceipt);
        await page.screenshot({path:resolve(output,'launcher-'+scenario+'.png')});
      }catch(error){scene.error=String(error.stack??error);scene.observedFailure=error.code==='ERR_ASSERTION';}
      finally{
        if(watcher){try{await watcher.flush();}catch(error){scene.browserErrors.push({type:'response-audit',error:String(error)});}scene.responseErrors=[...watcher.errors];await watcher.stop().catch(error=>scene.browserErrors.push({type:'watcher-stop',error:String(error)}));}
        if(page){try{await page.evaluate(async()=>{await window.__flushLauncherMusicFailures();});scene.host=await page.evaluate(()=>window.__launcherMusicReceipt);}catch(error){scene.browserErrors.push({type:'protocol-tail',error:String(error)});}}
        scene.rawBrowserErrors=[...scene.browserErrors];
        const classified=classifyLanguageBrowserEvents(scene.browserErrors,{responses:watcher?.responses??[]});scene.browserErrors=classified.blocking;scene.expectedBrowserEvents=classified.expected;
        if(context)await context.close().catch(error=>scene.browserErrors.push({type:'context-close',error:String(error)}));
        Object.assign(scene,assessLauncherMusicScene(scene));if(scene.error){if(scene.observedFailure)scene.status='failed';else if(scene.status!=='failed')scene.status='incomplete';scene.passed=false;scene.missing.push(scene.error);}await save();
      }
    }
    report.packagePostflight=await verifyRuntimePackage(target,{repoRoot:root});assert.deepEqual(report.packagePostflight,identity);
    assert.deepEqual(await verifyPrivateData(target.data),dataIdentity);
    report.archivePostflight=await verifyRoomResourcePostflight({target,prepared:prepared.resource});
    const {inspectOfflineUpdateDescriptor}=await import('./offline-runtime-update.mjs');
    const archive=await inspectOfflineUpdateDescriptor(packageMap[target.game]);
    await verifyLauncherFixtureMusic({game:target.game,input:musicInputs,descriptor:archive.descriptor});
    await verifyOfflineSiteIdentity(snapshot);
    assert.deepEqual(await dependencySnapshot(root,['tools/ports/launcher-music-journey.mjs']),report.dependencies);
    const faultScene=report.scenes.find(scene=>scene.scenario==='missing-object-fallback');
    const fault=assessLauncherMusicFault(faultScene);report.resourceFault={...fault,passed:fault.passed&&faultScene?.passed===true,scene:'missing-object-fallback',externalInputsStable:true};
    if(!report.resourceFault.passed&&report.resourceFault.status==='passed')report.resourceFault.status=faultScene?.status??'incomplete';
    report.status=report.scenes.some(scene=>scene.status==='failed')?'failed':report.scenes.every(scene=>scene.passed)?'observed':'incomplete';
  }catch(error){report.status='incomplete';report.error=String(error.stack??error);}
  finally{
    if(ownedBrowser)await ownedBrowser.close().catch(error=>report.cleanupErrors.push(String(error)));
    if(tls)await tls.close().catch(error=>report.cleanupErrors.push(String(error)));
    if(server)await stopChild(server.child).catch(error=>report.cleanupErrors.push(String(error)));
    if(report.cleanupErrors.length&&report.status!=='failed')report.status='incomplete';await save();
  }
  return report;
}
