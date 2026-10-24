import assert from 'node:assert/strict';
import {mkdir,writeFile} from 'node:fs/promises';
import {resolve} from 'node:path';
import {isDeepStrictEqual} from 'node:util';
import {prepareLauncherRuntimeFixture} from './launcher-runtime-fixture.mjs';
import {chooseSetting,openAdvancedSettings} from './launcher-ui-controls.mjs';
import {matchesMusicLaunchConsent} from './launcher-music-journey.mjs';
import {availableFixturePort,startStaticServer,stopChild} from './launcher-fixture-preparation.mjs';
import {createOfflineStaticTlsProxy,captureOfflineSiteIdentity,watchOfflineSiteResponses,verifyOfflineSiteIdentity} from './offline-runtime-https.mjs';
import {installOfflinePackageStoreObserver} from './offline-package-observer.mjs';
import {PACKAGE_STORE_DB,PACKAGE_STORE_DB_VERSION,PACKAGE_INSTALLATIONS,PACKAGE_GENERATIONS,PACKAGE_OBJECTS} from '../../launcher/package/package-store.mjs';
import {verifyRuntimePackage,verifyPrivateData,readVerifiedRuntimeFile} from './acceptance-contracts.mjs';
import {verifyRoomResourcePostflight} from './room-resources.mjs';
import {fileIdentity,dependencySnapshot} from '../testing/dependency-snapshot.mjs';
import {resolveAdaptationAdapter} from '../../tests/adaptation/adapters/registry.mjs';
import {resolveTitleFlow} from '../../tests/adaptation/adapters/title-flow.mjs';
import {readPracticeMenuRaw,readPracticeSnapshotRaw,practiceSnapshotRequest,practiceGameplayReady} from '../../tests/adaptation/adapters/practice.mjs';
import {readRuntimeObservation,runtimeObservationRequest} from './runtime-observations.mjs';
import {classifyLanguageBrowserEvents} from './language-network-contract.mjs';

/** Passive observation of the production configure and trusted Runtime replies. */
export function installLauncherPracticeObserver({game}){
  if(window!==window.top){
    window.__practiceBrowserEvents=[];
    addEventListener('eagler-thprac-menu',event=>window.__practiceBrowserEvents.push({open:event.detail?.open,at:performance.now()}));
    addEventListener('message',event=>{
      const value=event.data,iframe=parent.document.querySelector('#gameFrame');
      if(event.source!==parent||event.origin!==location.origin||iframe?.contentWindow!==window||value?.protocol!=='eagler-touhou/1'||value.game!==game||value.command!=='configure')return;
      parent.__launcherPracticeReceipt?.configures.push({epoch:value.epoch,language:value.language,runtimePackPresent:value.runtimePack!=null,music:value.music,options:value.options,
        at:performance.timeOrigin+performance.now()});
    });return;
  }
  const receipt=window.__launcherPracticeReceipt={events:[],configures:[],errors:[]},pending=new Set();
  window.__flushLauncherPracticeFailures=async()=>{while(pending.size)await Promise.all([...pending]);};
  addEventListener('message',event=>{
    const iframe=document.querySelector('#gameFrame'),value=event.data;
    if(event.source!==iframe?.contentWindow||event.origin!==location.origin||value?.protocol!=='eagler-touhou/1'||value.game!==game)return;
    const epoch=Number(new URL(iframe.src,location.href).searchParams.get('runtimeEpoch'));
    if(!Number.isSafeInteger(epoch)||epoch<1||value.epoch!==epoch)return;
    receipt.events.push({...value,at:performance.timeOrigin+performance.now()});
    if(value.ok===false||['error','abort'].includes(value.event)){
      receipt.errors.push(value);const capture=Promise.resolve(window.__captureLauncherPracticeFailure(value));pending.add(capture);
      void capture.then(()=>pending.delete(capture),error=>{receipt.errors.push({error:String(error)});pending.delete(capture);});
    }
  });
}

/** Recompute from the published request, actual UI, native owner and protocol evidence. */
export function assessLauncherPracticeScene(scene={}){
  const missing=[],failures=[];
  const require=(present,condition,message)=>{if(!present)missing.push(message);else if(!condition)failures.push(message);};
  const host=scene.hostEnabled,enabled=scene.enabled,epoch=scene.runtime?.epoch;
  require(typeof host==='boolean'&&typeof enabled==='boolean',host||!enabled,'Explicit legal Host and user settings');
  require(typeof scene.packageFeature==='boolean',scene.packageFeature===true,'Practice-enabled Runtime package');
  require(typeof scene.publishedFeature==='boolean',scene.publishedFeature===host,'Published Host capability matches the requested setting');
  require(scene.ui&&typeof scene.ui.advancedOpen==='boolean'&&typeof scene.ui.readyControlVisible==='boolean',
    scene.ui?.advancedOpen===true&&scene.ui?.readyControlVisible===true,'Advanced settings body is open and actionable');
  require(scene.ui&&typeof scene.ui.visible==='boolean',scene.ui?.visible===host,'Actual practice control visibility follows the Host');
  if(host){
    require(scene.ui?.checked!==undefined,scene.ui?.checked===enabled&&scene.ui?.disabled===false,'Visible practice control follows the user choice');
    require(typeof scene.reloadedChecked==='boolean',scene.reloadedChecked===enabled,'Practice setting survives Launcher reload');
    require(typeof scene.preference?.options?.thpracEnabled==='boolean',scene.preference.options.thpracEnabled===enabled,'Production preference records the visible choice');
  }
  require(scene.installation?.sha256&&scene.expectedData?.sha256,scene.installation?.sha256===scene.expectedData?.sha256&&scene.installation?.bytes===scene.expectedData?.bytes,'Imported DATA matches the selected private input');
  const configures=scene.protocol?.configures?.filter(item=>item.epoch===epoch)??[],first=scene.protocol?.events?.filter(item=>item.epoch===epoch&&item.event==='first-frame')??[];
  require(Number.isSafeInteger(epoch)&&epoch>0,configures.length===1&&first.length===1,'One configure and first-frame bind to the actual Runtime epoch');
  let runtimeUrl;try{runtimeUrl=new URL(scene.runtime?.url);}catch{}
  require(runtimeUrl&&typeof scene.runtime?.generation==='string',runtimeUrl?.pathname.startsWith(`/runtime/${scene.game}/${scene.runtime?.generation}/`)&&Number(runtimeUrl?.searchParams.get('runtimeEpoch'))===epoch,'Actual frame URL selects the verified Runtime generation and epoch');
  if(configures.length===1){
    const configure=configures[0],options=configure.options;
    require(options&&typeof options==='object',host?options?.thpracEnabled===enabled&&options?.thpracLocale==='ja-JP':!Object.hasOwn(options??{},'thpracEnabled')&&!Object.hasOwn(options??{},'thpracLocale'),'Production configure respects the Host and Japanese language');
    require(Number.isFinite(configure.at)&&Number.isFinite(first[0]?.at),configure.at<=first[0]?.at,'Configure precedes the actual first-frame');
    require(typeof scene.selectedLanguage==='string'&&typeof configure.runtimePackPresent==='boolean',scene.selectedLanguage==='ja'&&configure.runtimePackPresent===false&&[undefined,'ja'].includes(configure.language),'The actual Japanese selection uses the built-in source without a translation pack');
    require(typeof configure.music==='string',configure.music===scene.musicMode,'The actual music choice agrees with its launch decision');
  }
  require(scene.native?.before&&scene.native?.opened&&scene.native?.closed,['before','opened','closed'].every(name=>scene.native?.[name]?.available===true)&&scene.native?.before.open===false&&scene.native?.opened.open===enabled&&scene.native?.closed.open===false,'Native Practice menu follows actual Backspace input');
  let nativeOwner;
  try{const adapter=resolveAdaptationAdapter(scene.game,'normal')?.practice;if(scene.nativeOwnerRaw?.available&&adapter)nativeOwner=adapter.snapshot.decode(scene.nativeOwnerRaw);}catch(error){failures.push(String(error.message));}
  require(nativeOwner,typeof nativeOwner?.state.enabled==='boolean'&&nativeOwner.state.enabled===enabled,'Native Practice owner consumes the selected enable state');
  const worldFrame=snapshot=>snapshot?.frames?.simulation??snapshot?.frames?.enemy;
  require(scene.gameplay?.before&&scene.gameplay?.after,practiceGameplayReady(scene.gameplay?.before)&&practiceGameplayReady(scene.gameplay?.after)&&Number.isSafeInteger(worldFrame(scene.gameplay?.before))&&worldFrame(scene.gameplay?.after)>worldFrame(scene.gameplay?.before),'Real ordinary gameplay is ready and its world clock advances before the menu actions');
  const nativeOptions=scene.native?.opened?.options;
  require(nativeOptions,host?nativeOptions?.thpracEnabled===enabled&&nativeOptions?.thpracLocale==='ja-JP':nativeOptions?.thpracEnabled===undefined&&nativeOptions?.thpracLocale===undefined,'Runtime options agree with the actual native owner');
  if(enabled){
    const opened=scene.native?.opened?.events??[],closed=scene.native?.closed?.events??[];
    require(opened.length&&closed.length,opened.some(event=>event.open===true)&&closed.slice(opened.length).some(event=>event.open===false),'Native open and close events belong to the observed actions');
  }
  require(scene.launchConsent&&typeof scene.musicMode==='string',['none','midi'].includes(scene.musicMode)&&scene.launchConsent?.confirmed===true&&matchesMusicLaunchConsent(scene.musicMode,scene.launchConsent),'The real selected music-mode launch decision is handled');
  require(Array.isArray(scene.browserErrors)&&Array.isArray(scene.protocol?.errors)&&Array.isArray(scene.protocolErrors)&&Array.isArray(scene.responseErrors),
    scene.browserErrors?.length===0&&scene.protocol?.errors?.length===0&&scene.protocolErrors?.length===0&&scene.responseErrors?.length===0,'Browser, protocol and served-byte audits are clean');
  return{status:failures.length?'failed':missing.length?'incomplete':'observed',complete:false,passed:!failures.length&&!missing.length,missing,failures};
}

export function assessLauncherPracticeJourney(report={}){
  const scenes=Array.isArray(report.scenes)?report.scenes:[],missing=[],failures=[];
  const expected=[[true,true],[true,false],[false,false]];
  for(const [host,enabled] of expected)if(scenes.filter(scene=>scene.hostEnabled===host&&scene.enabled===enabled).length!==1)
    missing.push(`Exactly one Host=${host}, enabled=${enabled} scene`);
  if(scenes.length!==3)missing.push('Exactly three actual settings scenes');
  const sceneResults=scenes.map(scene=>({hostEnabled:scene.hostEnabled,enabled:scene.enabled,...assessLauncherPracticeScene(scene)}));
  if(scenes.some(scene=>scene.game!==report.game))failures.push('Scene belongs to another game');
  if(sceneResults.some(scene=>scene.status==='failed'))failures.push('A real settings scene failed');
  if(sceneResults.some(scene=>!scene.passed))missing.push('Every settings scene must have complete actual observations');
  if(!report.identity?.runtime||!report.identity?.data||!report.postflight?.runtime||!report.postflight?.data)missing.push('Runtime and private DATA preflight/postflight identities');
  else if(!isDeepStrictEqual(report.identity,report.postflight))failures.push('Runtime or private DATA changed');
  if(!Array.isArray(report.cleanupErrors))missing.push('Final cleanup audit');
  else if(report.cleanupErrors.length)failures.push('Cleanup failed');
  if(report.error)failures.push(report.error);
  return{status:failures.length?'failed':missing.length?'incomplete':'observed',passed:!failures.length&&!missing.length,complete:false,sceneResults,missing,failures};
}

/** Three real UI settings on formally packaged import-mode Hosts, using the shared title flow. */
export async function runLauncherPracticeJourney({root,output,target,identity,dataIdentity,shared,artwork,packageMap,launchBrowser}){
  await mkdir(output,{recursive:true});
  const report={schema:'eagler-touhou/launcher-practice-journey/1',game:target.game,variant:target.variant,identity:{runtime:identity,data:dataIdentity},scenes:[],cleanupErrors:[]};
  const save=()=>writeFile(resolve(output,'launcher-practice-journey.json'),JSON.stringify(report,null,2)+'\n');
  const title=resolveAdaptationAdapter(target.game,target.variant),flow=resolveTitleFlow(target),practice=title?.practice;
  try{
    assert.ok(practice?.snapshot,'Register a native practice snapshot before selecting the Launcher journey');
    report.dependencies=await dependencySnapshot(root,['tools/ports/launcher-practice-journey.mjs']);
    const manifest=JSON.parse(await readVerifiedRuntimeFile(target,identity,'manifest.json'));
    for(const hostEnabled of [true,false]){
      let server,tls,browser,snapshot,prepared;
      const group=resolve(output,hostEnabled?'host-enabled':'host-disabled');await mkdir(group,{recursive:true});
      try{
        prepared=await prepareLauncherRuntimeFixture({root,output:group,target,identity,dataIdentity,shared,artwork,packageMap,practiceFeatures:{[target.game]:hostEnabled}});
        server=await startStaticServer(prepared.site,await availableFixturePort());tls=await createOfflineStaticTlsProxy({backendBase:server.baseUrl});
        snapshot=await captureOfflineSiteIdentity(prepared.site);browser=await launchBrowser({tlsIdentity:tls.tlsIdentity});
        report.hosts??=[];report.hosts.push({hostEnabled,features:prepared.features,sourceProvenance:prepared.sourceProvenance,site:prepared.site,identity:snapshot.identity});
        for(const enabled of hostEnabled?[true,false]:[false]){
          const scene={game:target.game,hostEnabled,enabled,status:'running',packageFeature:manifest.features?.thprac,publishedFeature:prepared.siteManifest.games[target.game].features.thprac,
            expectedData:dataIdentity,browserErrors:[],protocolErrors:[],responseErrors:[]};report.scenes.push(scene);await save();
          let context,page,watcher;
          try{
            context=await browser.newContext({viewport:{width:1280,height:900}});
            await context.addInitScript(installLauncherPracticeObserver,{game:target.game});
            await context.addInitScript(installOfflinePackageStoreObserver,{name:PACKAGE_STORE_DB,version:PACKAGE_STORE_DB_VERSION,installations:PACKAGE_INSTALLATIONS,generations:PACKAGE_GENERATIONS,objects:PACKAGE_OBJECTS});
            page=await context.newPage();
            await page.exposeBinding('__captureLauncherPracticeFailure',(source,value)=>{
              if(source.page!==page||source.frame!==page.mainFrame())scene.protocolErrors.push({error:'Protocol mirror came from another page/frame'});
              else scene.protocolErrors.push(value);
            });
            page.on('pageerror',error=>scene.browserErrors.push({type:'pageerror',error:String(error)}));
            page.on('console',message=>{if(message.type()==='error')scene.browserErrors.push({type:'console',text:message.text(),location:message.location()});});
            page.on('requestfailed',request=>scene.browserErrors.push({type:'requestfailed',url:request.url(),error:request.failure()?.errorText}));
            page.on('response',response=>{if(response.status()>=400)scene.browserErrors.push({type:'http',url:response.url(),status:response.status()});});
            watcher=await watchOfflineSiteResponses(page,{base:tls.base,snapshot,appShell:prepared.deployment.appShell,output:group,label:'practice-ui-'+enabled,resourceRouteDiscovery:true});
            const boot=async()=>{await page.waitForFunction(()=>window.__eaglerBoot?.done===true,null,{timeout:60000});if(await page.locator('#firstUseNoticeDialog').evaluate(node=>node.open))await page.locator('#firstUseNoticeClose').click();};
            await page.goto(new URL('?game='+target.game,tls.base).href);await boot();
            await page.locator('#gamePackageImport').click();await page.locator('#gameDataImportInput').setInputFiles(resolve(packageMap[target.game]));
            await page.waitForFunction(()=>{const node=document.querySelector('#gameDataImportWindow');return node&&node.hidden&&node.getAttribute('aria-busy')!=='true';},null,{timeout:180000});
            scene.installation=await page.evaluate(async game=>{const observer=window.__offlinePackageStoreObserver,{generation}=await observer.readCurrentPackageGeneration(game);if(!generation)throw Error('No UI-imported package');
              const object=await observer.readPackageObject(generation.files[generation.descriptor.runtimeRequirement.dataFile].objectId),bytes=await object.blob.arrayBuffer();
              return{generationId:generation.id,revision:generation.descriptor.revision,bytes:bytes.byteLength,sha256:Array.from(new Uint8Array(await crypto.subtle.digest('SHA-256',bytes)),n=>n.toString(16).padStart(2,'0')).join('')};},target.game);
            const advancedReady=await openAdvancedSettings(page),option=page.locator('#thpracOption');
            await option.waitFor({state:hostEnabled?'visible':'hidden'});
            const toggle=page.locator('#thpracToggle');scene.ui={...advancedReady,visible:await option.isVisible()};
            if(hostEnabled){
              const checked=()=>toggle.getAttribute('aria-checked');
              if((await checked()==='true')===enabled)await toggle.click();
              await toggle.click();scene.ui.checked=await checked()==='true';scene.ui.disabled=await toggle.isDisabled();
              scene.preference=await page.evaluate(key=>JSON.parse(localStorage.getItem(key)),'eagler-touhou-game-options-v1-'+target.game);
            }
            if(await page.locator('#musicOption').isVisible())await chooseSetting(page,'musicSelect','none');
            scene.musicMode=await page.locator('#musicSelect').inputValue();assert.ok(['none','midi'].includes(scene.musicMode),'Practice fixture needs an actual built-in music selection');
            await page.reload();await boot();
            scene.selectedLanguage=await page.locator('#languageSelect').inputValue();
            if(hostEnabled)scene.reloadedChecked=await page.locator('#thpracToggle').getAttribute('aria-checked')==='true';
            await page.locator('#launch').click();
            await page.waitForFunction(()=>document.querySelector('#decisionDialog')?.open===true||window.__launcherPracticeReceipt.errors.length>0,null,{timeout:30000});
            scene.launchConsent=await page.evaluate(()=>({open:document.querySelector('#decisionDialog')?.open===true,message:document.querySelector('#decisionMessage')?.textContent,confirm:document.querySelector('#decisionConfirm')?.textContent,confirmed:false}));
            assert.ok(scene.launchConsent.open&&matchesMusicLaunchConsent(scene.musicMode,scene.launchConsent),'Unexpected launch decision');
            await page.locator('#decisionConfirm').click();scene.launchConsent.confirmed=true;
            await page.waitForFunction(()=>window.__launcherPracticeReceipt.events.some(event=>event.event==='first-frame')||window.__launcherPracticeReceipt.errors.length>0,null,{timeout:180000});
            const frame=await(await page.locator('#gameFrame').elementHandle()).contentFrame();assert.ok(frame);
            const url=new URL(frame.url()),epoch=Number(url.searchParams.get('runtimeEpoch'));scene.runtime={url:url.href,epoch,generation:prepared.runtimeGroup.current.generation};
            assert.ok(url.pathname.startsWith(`/runtime/${target.game}/${prepared.runtimeGroup.current.generation}/`),'Launcher selected another Runtime generation');
            await frame.locator('canvas').focus();
            const observe=()=>frame.evaluate(readRuntimeObservation,runtimeObservationRequest(target));
            const waitUntil=async(label,predicate,timeoutMs=20000)=>{const started=Date.now();let state;do{state=await observe();if(predicate(state))return state;await page.waitForTimeout(75);}while(Date.now()-started<timeoutMs);throw Error(label+': '+JSON.stringify(state));};
            const press=async key=>{await page.keyboard.press(key,{delay:90});await page.waitForTimeout(140);};
            await flow.enterGameplay({observe,waitUntil,press,lane:'practice',epoch});
            const before=await waitUntil('ordinary gameplay before Practice menu',practiceGameplayReady);
            const beforeFrame=before.frames.simulation??before.frames.enemy;
            const after=await waitUntil('ordinary world clock advances before Practice menu',state=>practiceGameplayReady(state)&&(state.frames.simulation??state.frames.enemy)>beforeFrame);
            scene.gameplay={before,after};
            scene.nativeOwnerRaw=await frame.evaluate(readPracticeSnapshotRaw,practiceSnapshotRequest({runtime:title.runtime,contract:practice}));
            scene.native={before:await frame.evaluate(readPracticeMenuRaw,{runtime:title.runtime})};
            await press('Backspace');scene.native.opened=await frame.evaluate(readPracticeMenuRaw,{runtime:title.runtime});
            await press('Backspace');scene.native.closed=await frame.evaluate(readPracticeMenuRaw,{runtime:title.runtime});
            await page.screenshot({path:resolve(group,'practice-'+enabled+'.png')});
          }catch(error){scene.error=String(error.stack??error);scene.observedFailure=error.code==='ERR_ASSERTION';}
          finally{
            if(watcher){try{await watcher.flush();}catch(error){scene.browserErrors.push({type:'response-audit',error:String(error)});}scene.responseErrors=[...watcher.errors];await watcher.stop().catch(error=>scene.browserErrors.push({type:'watcher-stop',error:String(error)}));}
            if(page&&!page.isClosed())try{await page.evaluate(()=>window.__flushLauncherPracticeFailures());scene.protocol=await page.evaluate(()=>window.__launcherPracticeReceipt);}catch(error){scene.browserErrors.push({type:'protocol-tail',error:String(error)});}
            const classified=classifyLanguageBrowserEvents(scene.browserErrors,{responses:watcher?.responses??[]});scene.rawBrowserErrors=scene.browserErrors;scene.browserErrors=classified.blocking;scene.expectedBrowserEvents=classified.expected;
            if(context)await context.close().catch(error=>scene.browserErrors.push({type:'context-close',error:String(error)}));
            Object.assign(scene,assessLauncherPracticeScene(scene));if(scene.error){scene.passed=false;if(scene.observedFailure)scene.status='failed';else if(scene.status!=='failed')scene.status='incomplete';scene.missing.push(scene.error);}await save();
          }
        }
        await verifyOfflineSiteIdentity(snapshot);
        report.archivePostflight=await verifyRoomResourcePostflight({target,prepared:prepared.resource});
        for(const expected of Object.values(prepared.sharedIdentity)){const{path,...identity}=expected;assert.deepEqual(await fileIdentity(path),identity);}
      }finally{
        if(browser)await browser.close().catch(error=>report.cleanupErrors.push(String(error)));
        if(tls)await tls.close().catch(error=>report.cleanupErrors.push(String(error)));
        if(server)await stopChild(server.child).catch(error=>report.cleanupErrors.push(String(error)));
      }
    }
    report.postflight={runtime:await verifyRuntimePackage(target,{repoRoot:root}),data:await verifyPrivateData(target.data)};
    assert.deepEqual(report.postflight,report.identity);
    assert.deepEqual(await dependencySnapshot(root,['tools/ports/launcher-practice-journey.mjs']),report.dependencies);
    Object.assign(report,assessLauncherPracticeJourney(report));
  }catch(error){report.status=report.scenes.some(scene=>scene.status==='failed')||error.code==='ERR_ASSERTION'?'failed':'incomplete';report.error=String(error.stack??error);}
  finally{if(report.cleanupErrors.length&&report.status!=='failed')report.status='incomplete';report.finishedAt=new Date().toISOString();await save();}
  return report;
}
