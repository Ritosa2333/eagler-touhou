import {chooseSetting} from './launcher-ui-controls.mjs';
import assert from 'node:assert/strict';
import {readFile,writeFile} from 'node:fs/promises';
import {resolve} from 'node:path';
import {runWithReport} from '../testing/cli-report.mjs';
import {sourceSnapshot} from '../testing/source-snapshot.mjs';
import {assertCliArguments} from '../testing/cli-options.mjs';
import {fileIdentity,dependencySnapshot} from '../testing/dependency-snapshot.mjs';
import {selectRuntimeTargets,verifyRuntimePackage,verifyPrivateData} from './acceptance-contracts.mjs';
import {languageObservationRequest,readLanguageObservation} from '../../tests/adaptation/adapters/language.mjs';
import {selectLanguageInputs,inspectLanguageInputs,languageInputReceipt} from './language-inputs.mjs';
import {prepareLanguageFixture} from './language-fixture-site.mjs';
import {assessLanguageScene} from './language-contract.mjs';
import {explicitLanguageSelectionSteps,normalGameLanguagePreferenceKey} from './language-selection-contract.mjs';
import {installLanguageHostObserver} from './language-host-observer.mjs';
import {languageFailureResponse,classifyLanguageBrowserEvents,resolveLanguageRuntimeSource} from './language-network-contract.mjs';
import {resolveLanguagePackSource} from '../../launcher/src/launcher/language-catalog.mts';
import {availableFixturePort,startStaticServer,stopChild} from './launcher-fixture-preparation.mjs';
import {createOfflineStaticTlsProxy,captureOfflineSiteIdentity,watchOfflineSiteResponses,verifyOfflineSiteIdentity} from './offline-runtime-https.mjs';
import {PRODUCT_GAMES} from '../../common/contracts/product-catalog.mjs';
import {resolveAdaptationAdapter} from '../../tests/adaptation/adapters/registry.mjs';

const root=resolve(import.meta.dirname,'../..'),arg=name=>process.argv.find(value=>value.startsWith(`--${name}=`))?.slice(name.length+3);
const required=name=>{const value=arg(name);if(!value)throw Error(`Required: --${name}=VALUE`);return value;};
const output=resolve(required('output')),pause=ms=>new Promise(resolve=>setTimeout(resolve,ms));

const report=await runWithReport({output,schema:'eagler-touhou/language-browser/1'},async(report,save)=>{
  assertCliArguments(process.argv.slice(2),{values:['output','manifest','language-manifest','artwork-dir','game','variant']});
  const game=required('game'),variant=required('variant');assert.equal(variant,'normal','Select normal for the current Launcher language journey');
  const preferenceKey=normalGameLanguagePreferenceKey(game);
  const configPath=resolve(required('manifest')),config=JSON.parse(await readFile(configPath,'utf8'));
  const selected=selectRuntimeTargets(config,{game,variant});assert.equal(selected.targets.length,1,'Select exactly one language Runtime');
  const target=selected.targets[0],titleAdapter=resolveAdaptationAdapter(game,variant),
    request=languageObservationRequest({game,variant}),profile=request.contract;
  assert.ok(profile,`Register the ${game}/${variant} language observation and source samples`);
  const languageManifestPath=resolve(required('language-manifest'));
  const languageProduct=selectLanguageInputs(JSON.parse(await readFile(languageManifestPath,'utf8')),{game,variant});
  const inspected=await inspectLanguageInputs(languageProduct);
  const [identity,dataIdentity]=await Promise.all([verifyRuntimePackage(target,{repoRoot:root}),verifyPrivateData(target.data)]);
  if(target.dataSha256)assert.equal(dataIdentity.sha256,target.dataSha256,'Language Runtime DATA differs from its manifest');
  const shared=config.shared??{},sharedIdentity={};
  for(const [name,path] of Object.entries(shared))sharedIdentity[name]={path,...await fileIdentity(path)};
  report.sourceSnapshotStart=sourceSnapshot(root);
  report.dependencies=await dependencySnapshot(root,['tools/ports/language-browser.mjs']);
  report.inputs={manifest:{path:configPath,...await fileIdentity(configPath)},languageManifest:{path:languageManifestPath,...await fileIdentity(languageManifestPath)},
    languages:languageInputReceipt(inspected),data:dataIdentity,shared:sharedIdentity};
  report.scope={game,variant,statement:'Current packaged Launcher and Runtime; selected translations, cold offline Japanese, per-key partial translation and exact unavailable/corrupt archive controls; no visual approval'};
  const record={game,variant,id:target.id,status:'running',checks:[],scenes:[],errors:[]};report.targets.push(record);await save();
  const prepared=await prepareLanguageFixture({root,output,target,identity,dataIdentity,shared,artwork:resolve(required('artwork-dir')),inspected,profile});
  report.preparation={packaging:prepared.packaging,partialIdentity:prepared.partialIdentity};await save();
  let server,tls,browser,snapshot;const teardownErrors=[],responseOverrides=new Map();
  let japaneseBaseline;
  try{
    server=await startStaticServer(prepared.site,await availableFixturePort());
    tls=await createOfflineStaticTlsProxy({backendBase:server.baseUrl,responseOverrides});
    snapshot=await captureOfflineSiteIdentity(prepared.site);
    const deployment=JSON.parse(await readFile(resolve(prepared.site,'deployment.json'),'utf8'));
    const runtimeManifest=JSON.parse(await readFile(resolve(prepared.site,'runtime-manifest.json'),'utf8'));
    const runtimeGroup=runtimeManifest.groups.find(group=>group.root===`runtime/${game}/`);
    assert.ok(runtimeGroup?.current?.generation,'The packaged language site has no current normal Runtime');
    report.site={base:tls.base,identity:snapshot.identity,deployment,runtimeGroup};
    const {launchBrowser}=await import('../browser/launch.mjs');
    browser=await launchBrowser({args:['--enable-unsafe-swiftshader','--autoplay-policy=no-user-gesture-required',
      `--ignore-certificate-errors-spki-list=${tls.tlsIdentity.spkiSha256}`]});
    report.browser={engine:'Chromium',version:browser.version()};
    const scenes=[{id:'japanese-offline',language:'ja',offline:true},
      ...Object.keys(inspected.packs).map(language=>({id:`translated-${language}`,language})),
      {id:'partial-key',language:prepared.partialLanguage},
      ...['network-failure','corrupt-archive'].map(failure=>({id:failure,language:prepared.baseLanguage,failure}))];
    for(const scene of scenes){
      const observed={...scene,status:'running',browserEvents:[]};record.scenes.push(observed);await save();
      if(scene.failure){
        const entry=prepared.manifest.games[game].languages.find(entry=>entry.id===scene.language);
        const url=new URL(resolveLanguagePackSource(entry,tls.base).url),control=languageFailureResponse(
          await readFile(inspected.packs[scene.language].path),{kind:scene.failure});
        responseOverrides.set(url.pathname+url.search,control);
        const {body,...identity}=control;observed.control={url:url.href,...identity,published:entry.pack};
      }
      const context=await browser.newContext({viewport:{width:1280,height:900}});
      await context.addInitScript(installLanguageHostObserver,{game,contract:profile});
      let page,watcher;
      try{
        const settleBoot=async()=>{
          await page.waitForFunction(()=>window.__eaglerBoot?.done===true,null,{timeout:60000});
          const notice=page.locator('#firstUseNoticeDialog');
          if(await notice.count()&&await notice.evaluate(element=>element.open))await page.locator('#firstUseNoticeClose').click();
        };
        const open=async(phase)=>{
          page=await context.newPage();
          watcher=await watchOfflineSiteResponses(page,{base:tls.base,snapshot,appShell:deployment.appShell,output,
            label:`${scene.id}-${phase}`,controlledResponses:responseOverrides,resourceRouteDiscovery:true});
          (observed.responseIntegrity??=[]).push({phase,responses:watcher.responses,errors:watcher.errors});
          page.on('pageerror',error=>observed.browserEvents.push({phase,type:'pageerror',text:String(error)}));
          page.on('console',message=>{if(message.type()==='error')observed.browserEvents.push({phase,type:'console',text:message.text(),location:message.location()});});
          page.on('requestfailed',request=>observed.browserEvents.push({phase,type:'requestfailed',url:request.url(),error:request.failure()?.errorText}));
          page.on('response',response=>{if(response.status()>=400)observed.browserEvents.push({phase,type:'http',url:response.url(),status:response.status()});});
          await page.goto(new URL(`?game=${game}`,tls.base).href,{waitUntil:'load',timeout:60000});
          await settleBoot();
        };
        const launch=async(language)=>{
          if(await page.locator('#musicOption').getAttribute('hidden')===null){
            await chooseSetting(page,'musicSelect','none');observed.musicChoice={value:'none',source:'Launcher visible music menu'};
          }else observed.musicChoice={value:await page.locator('#musicSelect').inputValue(),source:'Launcher default for this published music profile'};
          const choices=await page.locator('#languageSelect option').evaluateAll(options=>options.map(option=>option.value));
          assert.ok(choices.includes(language),'Selected language is not present in the published Launcher selector');
          const current=await page.locator('#languageSelect').inputValue();
          const stored=await page.evaluate(key=>localStorage.getItem(key),preferenceKey);
          const languageSteps=explicitLanguageSelectionSteps({current,target:language,stored,options:choices});
          for(const value of languageSteps)await chooseSetting(page,'languageSelect',value);
          assert.equal(await page.locator('#languageSelect').inputValue(),language,'Launcher language selection did not settle');
          assert.equal(await page.evaluate(key=>localStorage.getItem(key),preferenceKey),language,
            'Launcher did not persist the selected language under its production preference key');
          await page.locator('#launch').click();
          const launchDeadline=Date.now()+180000;
          while(true){
            await page.waitForFunction(()=>window.__languageHostReceipt?.events.some(event=>event.event==='first-frame')||
              document.querySelector('#decisionDialog')?.open,null,{timeout:Math.max(1,launchDeadline-Date.now())});
            if(await page.evaluate(()=>window.__languageHostReceipt.events.some(event=>event.event==='first-frame')))break;
            await page.locator('#decisionConfirm').click();
            await page.waitForFunction(()=>!document.querySelector('#decisionDialog')?.open||
              window.__languageHostReceipt.events.some(event=>event.event==='first-frame'),null,
              {timeout:Math.max(1,launchDeadline-Date.now())});
          }
          const host=await page.evaluate(()=>window.__languageHostReceipt);
          assert.equal(host.enables.length,1,'Native observation must be enabled once at the real Runtime ready boundary');
          assert.equal(host.enables[0].appBefore,0,'Native language observation was enabled after native world construction');
          assert.deepEqual(host.errors,[],'Runtime reported a language startup error');
          const frame=page.frames().find(frame=>frame!==page.mainFrame()&&new URL(frame.url()).searchParams.has('runtimeEpoch'));
          assert.ok(frame,'The production Launcher did not create its Runtime frame');
          const url=new URL(frame.url()),epoch=Number(url.searchParams.get('runtimeEpoch'));
          assert.ok(Number.isSafeInteger(epoch)&&epoch>0,'Language Runtime has no valid epoch');
          assert.ok(url.pathname.startsWith(`/runtime/${game}/${runtimeGroup.current.generation}/`),'Launcher selected another Runtime generation');
          assert.equal(host.enables[0].epoch,epoch,'Language observer belongs to another Runtime epoch');
          assert.ok(host.events.filter(event=>['ready','first-frame'].includes(event.event)).every(event=>event.epoch===epoch),'Runtime ready and first-frame identities differ');
          assert.equal(host.configures.length,1,'Language resources must be configured once before launch');
          const configure=host.configures[0],first=host.events.find(event=>event.event==='first-frame');
          assert.equal(configure.epoch,epoch);assert.ok(configure.parentAt>=host.enables[0].at&&first.at>=configure.parentAt,'Language configure escaped its ready-to-first-frame boundary');
          if(language==='ja'||scene.failure)assert.equal(configure.pack,null,'Original-language launch unexpectedly received a translation pack');
          else{
            const entry=prepared.manifest.games[game].languages.find(entry=>entry.id===language);
            assert.equal(configure.pack?.language,language,'Launcher configured another language pack');
            assert.equal(configure.pack?.bytes,entry.pack.bytes,'Configured archive bytes differ from the published language');
            if(configure.pack.sha256)assert.equal(configure.pack.sha256,entry.pack.sha256,'Configured archive SHA differs from publication');
            const mounted=await frame.evaluate(async({mapping,files})=>{
              const fs=globalThis[mapping.global][mapping.module].FS,rows={};
              for(const path of files){const bytes=fs.readFile(path);
                const sha=Array.from(new Uint8Array(await crypto.subtle.digest('SHA-256',bytes)),value=>value.toString(16).padStart(2,'0')).join('');
                rows[path]={bytes:bytes.length,sha256:sha};}
              return rows;
            },{mapping:profile.runtime,files:Object.keys(prepared.mounts[language])});
            assert.deepEqual(mounted,prepared.mounts[language],'Runtime mounted language bytes differ from the independent archive');
            host.mounted=mounted;
          }
          return{frame,host,choices};
        };
        if(scene.offline){
          await open('warmup');
          const appShell=await page.evaluate(async()=>{
            const registration=await navigator.serviceWorker.ready;
            return{scope:registration.scope,worker:registration.active?.scriptURL??null,controlled:!!navigator.serviceWorker.controller};
          });
          if(!appShell.controlled){
            await page.evaluate(()=>document.fonts.ready);
            await watcher.flush();
            await page.reload({waitUntil:'load',timeout:60000});await settleBoot();
          }
          await page.waitForFunction(()=>!!navigator.serviceWorker.controller,null,{timeout:30000});
          const controlledShell=await page.evaluate(async()=>{
            const registration=await navigator.serviceWorker.ready;
            return{scope:registration.scope,worker:registration.active?.scriptURL??null,controlled:!!navigator.serviceWorker.controller};
          });
          assert.equal(controlledShell.controlled,true,'Warmup page is not controlled by the installed App Shell Service Worker');
          assert.ok(controlledShell.worker,'Warmup App Shell Service Worker has no active script');
          assert.equal(new URL(controlledShell.scope).origin,new URL(tls.base).origin,'Warmup Service Worker scope belongs to another origin');
          assert.equal(new URL(controlledShell.worker).origin,new URL(tls.base).origin,'Warmup Service Worker belongs to another origin');
          observed.warmupServiceWorker=controlledShell;observed.warmup=(await launch('ja')).host;
          await watcher.flush();await watcher.stop();watcher=null;await page.close();page=null;
          await context.setOffline(true);
        }
        await open(scene.offline?'offline':'online');
        if(scene.offline){
          await page.waitForFunction(()=>!!navigator.serviceWorker.controller,null,{timeout:30000});
          const worker=await page.evaluate(()=>navigator.serviceWorker.controller?.scriptURL??null);
          assert.ok(worker,'Offline Japanese launch did not use the installed App Shell Service Worker');
          assert.equal(worker,observed.warmupServiceWorker.worker,'Offline launch selected a different App Shell Service Worker');
          assert.equal(new URL(worker).origin,new URL(tls.base).origin,'Offline launch is controlled by another site worker');
        }
        const launched=await launch(scene.language);observed.host=launched.host;observed.languageChoices=launched.choices;
        observed.runtime={url:launched.frame.url(),online:await page.evaluate(()=>navigator.onLine)};
        if(scene.offline)assert.equal(observed.runtime.online,false,'Japanese baseline must execute after the browser is offline');
        const expected=scene.failure?{entries:[]}:prepared.expected[scene.language],deadline=Date.now()+30000;
        do{
          const native=await launched.frame.evaluate(readLanguageObservation,request);
          assert.equal(native.available,true,'Native language observations are unavailable');observed.trace=native.trace;
          observed.assessment=assessLanguageScene({trace:native.trace,profile,expected,baseline:japaneseBaseline,original:scene.language==='ja'});
          if(observed.assessment.status==='observed')break;
          await pause(100);
        }while(Date.now()<deadline);
        observed.preferenceKey=preferenceKey;
        observed.preference=await page.evaluate(key=>localStorage.getItem(key),preferenceKey);
        assert.equal(observed.preference,scene.language,'Language scene changed the durable preference');
        await page.screenshot({path:resolve(output,`${scene.id}.png`)});observed.screenshot=`${scene.id}.png`;
        await watcher.flush();
        const responses=observed.responseIntegrity.flatMap(phase=>phase.responses);
        const runtimeSource=resolveLanguageRuntimeSource({browserDiagnostics:profile.browserDiagnostics,runtimeGroup,
          productRuntimeAssets:PRODUCT_GAMES[game].runtimeAssets,base:tls.base});
        observed.eventClassification=classifyLanguageBrowserEvents(observed.browserEvents,{responses,runtimeSource,
          classifyDiagnostic:titleAdapter?.classifyLanguageDiagnostic,
          fontPaths:Object.keys(observed.host.mounted??{}).filter(path=>path.startsWith(`/thcrap/${game}/fonts/`))});
        assert.deepEqual(observed.eventClassification.blocking,[],'Language scene produced unexpected browser errors');
        if(scene.failure){
          observed.control.responses=responses.filter(response=>response.url===observed.control.url);
          assert.ok(observed.control.responses.length>0,'Cold language failure control was bypassed by a cache or another source');
          assert.ok(observed.control.responses.every(response=>response.contentIdentity==='passed'&&
            (response.actual.sha256===observed.control.sha256||scene.failure==='network-failure'&&
              response.semanticReceipt?.bodyConsumption==='not-read')),'The language failure did not receive its declared controlled response');
        }
        observed.status=observed.assessment.status;
        if(scene.language==='ja'&&observed.status==='observed')japaneseBaseline=observed.assessment.baseline;
      }catch(error){
        observed.status='failed';observed.error=String(error.stack||error);
        if(page&&!page.isClosed()){
          try{observed.failureState=await page.evaluate(()=>({host:window.__languageHostReceipt,
            language:document.querySelector('#languageSelect')?.value,music:document.querySelector('#musicSelect')?.value,
            dialogs:[...document.querySelectorAll('dialog[open]')].map(dialog=>({id:dialog.id,text:dialog.textContent.slice(0,4000)}))}));}
          catch(captureError){observed.failureStateError=String(captureError);}
          try{observed.screenshot=`${scene.id}-failure.png`;await page.screenshot({path:resolve(output,observed.screenshot)});}
          catch(captureError){observed.screenshotError=String(captureError);}
        }
      }
      finally{
        if(watcher)try{await watcher.stop();}catch(error){observed.status='failed';observed.errors??=[];observed.errors.push(String(error));}
        try{await context.close();}catch(error){teardownErrors.push(String(error));observed.status='failed';}
        responseOverrides.clear();
        await save();
      }
    }
    const resultFor=id=>record.scenes.find(scene=>scene.id===id),translated=record.scenes.filter(scene=>scene.id.startsWith('translated-'));
    const add=(id,scenes,reason)=>record.checks.push({id,status:scenes.every(scene=>scene?.status==='observed')?'observed':
      scenes.some(scene=>scene?.status==='failed')?'failed':'incomplete',complete:false,
      observation:{game,variant,adaptationIds:[id],observation:{sceneIds:scenes.map(scene=>scene?.id??'missing'),reason,
        results:scenes.map(scene=>({id:scene?.id,status:scene?.status,assessment:scene?.assessment,
          host:scene?.host,languageChoices:scene?.languageChoices,preference:scene?.preference,screenshot:scene?.screenshot,
          musicChoice:scene?.musicChoice,control:scene?.control,eventClassification:scene?.eventClassification}))}}});
    add('LANG-01',[resultFor('japanese-offline')],'Cold offline Japanese title text after an actual installed-package warmup');
    add('LANG-02',translated,'Published archive identities and actual Launcher language selection');
    add('LANG-03',translated,'Pack selected before native construction and naturally consumed by the title text renderer');
    add('LANG-04',[resultFor('network-failure'),resultFor('corrupt-archive')],
      'Fresh contexts reject an unavailable archive and a same-length corrupt archive, render independent Japanese text and retain the selected preference');
    add('LANG-05',record.scenes,'Actual observed title codepoints and selected glyph sources; visual review uses the scene screenshots');
    add('LANG-06',[resultFor('partial-key')],'Present translated key remains translated while the missing key matches the independent cold Japanese baseline');
    record.status=record.scenes.some(scene=>scene.status==='failed')?'failed':record.scenes.every(scene=>scene.status==='observed')?'passed':'incomplete';
    report.scenarios=[{id:'language-cold-launch-and-fallback',status:record.status,passed:record.status==='passed'}];
    report.status=record.status;report.passed=record.status==='passed';
  }finally{
    if(browser)try{await browser.close();}catch(error){teardownErrors.push(String(error));}
    if(tls)try{await tls.close();}catch(error){teardownErrors.push(String(error));}
    if(server)try{await stopChild(server.child);}catch(error){teardownErrors.push(String(error));}
    report.teardownErrors=teardownErrors;
    if(snapshot)report.sitePostflight=await verifyOfflineSiteIdentity(snapshot);
    report.inputPostflight=languageInputReceipt(await inspectLanguageInputs(languageProduct));
    assert.deepEqual(report.inputPostflight,report.inputs.languages,'Language inputs changed during the scene group');
    assert.deepEqual(await fileIdentity(configPath),{bytes:report.inputs.manifest.bytes,sha256:report.inputs.manifest.sha256},'Runtime input manifest changed');
    assert.deepEqual(await fileIdentity(languageManifestPath),{bytes:report.inputs.languageManifest.bytes,sha256:report.inputs.languageManifest.sha256},'Language input manifest changed');
    assert.deepEqual(await verifyPrivateData(target.data),dataIdentity,'Private DATA changed during language acceptance');
    for(const [name,input] of Object.entries(sharedIdentity))assert.deepEqual(await fileIdentity(input.path),
      {bytes:input.bytes,sha256:input.sha256},`Shared input changed during language acceptance: ${name}`);
    await verifyRuntimePackage(target,{repoRoot:root});
    report.sourceSnapshotEnd=sourceSnapshot(root);report.sourceStable=report.sourceSnapshotEnd.sha256===report.sourceSnapshotStart.sha256;
    assert.deepEqual(report.sourceSnapshotEnd,report.sourceSnapshotStart,'Source changed during language acceptance');
    assert.deepEqual(teardownErrors,[],'Language scene cleanup failed');
  }
});
if(report.status!=='passed')process.exitCode=1;
