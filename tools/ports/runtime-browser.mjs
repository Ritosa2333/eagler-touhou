import assert from 'node:assert/strict';
import {createServer} from 'node:http';
import {createHash,randomUUID} from 'node:crypto';
import {readFile,writeFile,mkdir} from 'node:fs/promises';
import {resolve,extname} from 'node:path';
import {execFileSync} from 'node:child_process';
import {PRODUCT_GAMES} from '../../common/contracts/product-catalog.mjs';
import {gamepadProbeBundle,gamepadTargetFor,prepareVirtualGamepad,bindProtocolHostGamepad,probeNativeGamepad} from './gamepad-probe.mjs';
import {readVerifiedRuntimeFile,selectRuntimeTargets,verifyPrivateData,verifyRuntimePackage} from './acceptance-contracts.mjs';
import {sourceSnapshot} from '../testing/source-snapshot.mjs';
import {runWithReport} from '../testing/cli-report.mjs';
import {readRuntimeObservation,assertRuntimeObservation,runtimeScenarioResult} from './runtime-observations.mjs';
import {createNavigationAbortEvidence,shouldRecordHttpFailure} from './navigation-abort-evidence.mjs';
import {loadRuntimeReplayFixture} from './replay-fixtures.mjs';

const argument=name=>process.argv.find(value=>value.startsWith(`--${name}=`))?.slice(name.length+3);
const requestedOutput=argument('output');
if(!requestedOutput)throw Error('Required: --output=directory');
const result=await runWithReport({output:resolve(requestedOutput),schema:'eagler-touhou/runtime-browser/1'},async(report,save)=>{
if(!argument('manifest')||!argument('output'))throw Error('Required: --manifest=private-inputs.json --output=directory');
assert.equal(argument('variant'),'normal',
  'runtime-browser requires --variant=normal. Verify multiplayer through tools/ports/room-browser.mjs with real players and --route=rtc|relay');
const config=JSON.parse(await readFile(resolve(argument('manifest')),'utf8'));
const output=resolve(argument('output'));
const variant=argument('variant');
const gamepad=process.argv.includes('--gamepad'),gamepadBundle=gamepad?await gamepadProbeBundle():null;
const selection=selectRuntimeTargets(config,{game:argument('game'),variant});
const {targets}=selection;
await mkdir(output,{recursive:true});
const hash=bytes=>createHash('sha256').update(bytes).digest('hex');
const evidenceEvents=events=>events.map(event=>{
  if(!Array.isArray(event?.bytes))return event;
  const {bytes,...metadata}=event;
  return {...metadata,bytePayload:{bytes:bytes.length,sha256:hash(new Uint8Array(bytes))}};
});
const root=resolve(import.meta.dirname,'../..');
Object.assign(report,{commit:execFileSync('git',['rev-parse','HEAD'],{cwd:root,encoding:'utf8'}).trim(),
  sourceSnapshot:sourceSnapshot(root),
  selection:{games:selection.games,variant:selection.variant,expectedTargets:selection.expected},
  scope:'Desktop Chromium, hash-identified production Runtime package, real private game DATA, protocol host; no physical-device or multiplayer-session claim',targets:[]});
report.harnessSha256=hash(await readFile(new URL(import.meta.url)));
const observationReader=`export const readRuntimeObservation = (${readRuntimeObservation.toString()});\n`;
report.observationReaderSha256=hash(observationReader);
await writeFile(resolve(output,'observation-reader.mjs'),observationReader);
for(const name of ['runtime-observations','replay-fixtures']){
  const contents=await readFile(new URL(`./${name}.mjs`,import.meta.url));
  (report.helperSha256??={})[name]=hash(contents);
  await writeFile(resolve(output,`${name}.mjs`),contents);
}
if(gamepad){report.gamepadProbeSha256=hash(await readFile(new URL('./gamepad-probe.mjs',import.meta.url)));report.gamepadBundleSha256=hash(gamepadBundle);await writeFile(resolve(output,'gamepad-bundle.js'),gamepadBundle);await writeFile(resolve(output,'gamepad-probe.mjs'),await readFile(new URL('./gamepad-probe.mjs',import.meta.url)));}
await writeFile(resolve(output,'harness.mjs'),await readFile(new URL(import.meta.url)));
const routes=new Map();
const verifiedPackages=new Map();
const verifiedData=new Map();
for(const [name,file] of Object.entries(config.shared??{}))routes.set(`/shared/${name}`,resolve(file));
const server=createServer(async(request,response)=>{
  try{
    const url=new URL(request.url,'http://localhost');
    if(url.pathname==='/favicon.ico'){response.writeHead(204).end();return;}
    const selected=config.targets.find(target=>url.pathname===`/host/${target.id}`);
    if(selected){
      const epoch=Number(url.searchParams.get('epoch')??1);assert.ok(Number.isSafeInteger(epoch)&&epoch>0);
      const runtime=`/runtime/${selected.id}/${selected.entry}?hosted=1&managedData=1&gameGeneration=desktop-acceptance&runtimeEpoch=${epoch}`;
      response.setHeader('Content-Type','text/html');
      response.end(`<!doctype html><meta charset="utf-8"><style>html,body{margin:0;background:#222}iframe{width:960px;height:720px;border:0}</style><script>
      window.events=[];window.__eaglerPrepareManagedRuntimeDataV1=async()=>{const buffer=await fetch('/data/${selected.id}').then(r=>{if(!r.ok)throw Error('Missing private DATA');return r.arrayBuffer()});return{buffer,bytes:buffer.byteLength}};
      window.send=(command,fields={})=>document.querySelector('iframe').contentWindow.postMessage({protocol:'eagler-touhou/1',game:${JSON.stringify(selected.game)},epoch:${epoch},command,...fields},location.origin);
      addEventListener('message',event=>{if(event.origin===location.origin&&event.source===document.querySelector('iframe')?.contentWindow)events.push(event.data)});
      window.rpc=(command,fields={})=>new Promise((accept,reject)=>{const request=crypto.randomUUID(),timer=setTimeout(()=>{removeEventListener('message',receive);reject(Error('Timeout: '+command))},120000);function receive(event){if(event.origin!==location.origin||event.source!==document.querySelector('iframe').contentWindow||event.data?.request!==request)return;clearTimeout(timer);removeEventListener('message',receive);event.data.ok?accept(event.data):reject(Error(event.data.error||'Command rejected'))}addEventListener('message',receive);send(command,{...fields,request})});
      </script><iframe src="${runtime}"></iframe>`);return;
    }
    let file=routes.get(url.pathname),dataTarget;
    for(const target of config.targets){
      if(url.pathname===`/data/${target.id}`){file=target.data;dataTarget=target;}
      const prefix=`/runtime/${target.id}/`;
      if(url.pathname.startsWith(prefix)){
        const tail=decodeURIComponent(url.pathname.slice(prefix.length));
        const identity=verifiedPackages.get(target.id);
        if(!identity)throw Error(`${target.id}: Runtime package was not verified before use`);
        const bytes=await readVerifiedRuntimeFile(target,identity,tail);
        response.writeHead(200,{'Content-Type':({'.html':'text/html','.mjs':'text/javascript','.js':'text/javascript','.wasm':'application/wasm','.json':'application/json','.css':'text/css'})[extname(tail)]??'application/octet-stream','Content-Length':bytes.length});
        response.end(bytes);return;
      }
    }
    if(!file){response.writeHead(404).end('Unknown test input');return;}
    const bytes=await readFile(file);
    if(dataTarget){
      const expected=verifiedData.get(dataTarget.id);
      assert.ok(expected,`${dataTarget.id}: DATA was not verified before use`);
      assert.equal(bytes.length,expected.bytes,'DATA changed after verification');
      assert.equal(hash(bytes),expected.sha256,'DATA changed after verification');
    }
    response.writeHead(200,{'Content-Type':({'.html':'text/html','.mjs':'text/javascript','.js':'text/javascript','.wasm':'application/wasm','.json':'application/json','.css':'text/css'})[extname(file)]??'application/octet-stream','Content-Length':bytes.length});
    response.end(bytes);
  }catch(error){response.writeHead(404).end(error.message);}
});
await new Promise(done=>server.listen(0,'127.0.0.1',done));
let browser;
try{
  const {launchBrowser}=await import('../browser/launch.mjs');
  browser=await launchBrowser({args:['--enable-unsafe-swiftshader','--autoplay-policy=no-user-gesture-required']});
  report.browser=browser.version();
  for(const target of targets){
    const record={game:target.game,variant:target.variant,id:target.id,checks:[],errors:[],consoleErrors:[],httpErrors:[],expectedNavigationAborts:[],observations:{}};
    report.targets.push(record);
      const check=async(id,checks,operation,{complete=false,fatal=true,verified=()=>true}={})=>{try{const observation=await operation();record.checks.push({id,checks,complete,status:verified(observation)?'passed':'observed',observation});return observation;}catch(error){record.checks.push({id,checks,complete,status:'failed',error:String(error.stack||error)});if(fatal)throw error;}finally{await save();}};
    const context=await browser.newContext({viewport:{width:1100,height:820}});
    if(gamepad)await prepareVirtualGamepad(context,gamepadBundle,gamepadTargetFor(target.game,target.variant,0));
    const page=await context.newPage();
    const navigationEvidence=createNavigationAbortEvidence();
    page.on('pageerror',error=>record.errors.push(String(error)));
    page.on('console',message=>{if(message.type()==='error')record.consoleErrors.push(message.text());});
    page.on('request',request=>navigationEvidence.captureRequest(request));
    page.on('response',response=>{if(shouldRecordHttpFailure(response.status()))record.httpErrors.push({url:response.url(),status:response.status()});});
    page.on('requestfailed',request=>{
      const failure=navigationEvidence.requestFailure(request);
      const evidence={url:request.url(),failure:failure.errorText,isNavigationRequest:request.isNavigationRequest(),
        attribution:failure.attribution};
      if(failure.expectedNavigationAbort)record.expectedNavigationAborts.push(evidence);
      else record.httpErrors.push(evidence);
    });
    const navigate=async url=>{navigationEvidence.retirePage(page);return page.goto(url);};
    let frame;
    const observe=()=>frame.evaluate(readRuntimeObservation,{game:target.game,variant:target.variant});
    const inspect=async()=>({...await frame.evaluate(game=>{
      const runtime=window[`__${game}Runtime`],module=runtime?.Module??window.Module,core=runtime?.core??module;
      let state=null;
      try{state=runtime?.status?.()??(core?.[`_${game}_phase`]?{phase:core[`_${game}_phase`]()}:
        core?.th11_status?Array.from(new Int32Array(core.memory.buffer,core.th11_status(),12)):null);}catch(error){state={error:String(error)};}
      return{state,canvas:document.querySelector('canvas')?{width:document.querySelector('canvas').width,height:document.querySelector('canvas').height}:null,
        renderCallbacks:window.__eaglerDebugRenderCallbacks??null,filesystem:!!module?.FS,options:module?.eaglerOptions??null};
    },target.game),native:await observe()});
    const sendKey=async(code,key,keyCode,down)=>{
      const request='acceptance-keyboard-'+randomUUID();
      (record.inputRequests??=[]).push(request);
      await page.evaluate(fields=>send('keyboard',fields),{code,key,keyCode,location:0,inputSource:'physical',down,request});
    };
    const key=async(code,key,keyCode,hold=100)=>{
      await sendKey(code,key,keyCode,true);
      try{await page.waitForTimeout(hold);}
      finally{await sendKey(code,key,keyCode,false);}
      await page.waitForTimeout(180);
    };
    const gameplayReady=state=>state.capabilities.input&&state.capabilities.position&&!state.pause?.active&&
      (!state.nativeObservation||(state.localSeat===0&&Number.isFinite(state.frames.simulation)))&&
      (state.capabilities.phase?['stage','match'].includes(state.phase?.name):target.game==='th08'&&state.status?.supervisorActive===2&&state.status.active===1&&!state.status.loading);
    const waitObservation=async(label,predicate,timeoutMs=20000)=>{
      const started=Date.now();let state,samples=0;
      do{
        state=await observe();samples++;
        if(predicate(state)){
          (record.observations.preconditions??=[]).push({label,samples,durationMs:Date.now()-started,state});return state;
        }
        await page.waitForTimeout(75);
      }while(Date.now()-started<timeoutMs);
      throw Error(`${label}: native condition not reached: ${JSON.stringify(state)}`);
    };
    try{
      await check('runtime-input-identity',['BUILD-04'],async()=>{
        const packageIdentity=await verifyRuntimePackage(target,{repoRoot:root});verifiedPackages.set(target.id,packageIdentity);
        const dataIdentity=await verifyPrivateData(target.data);
        verifiedData.set(target.id,dataIdentity);
        if(target.dataSha256)assert.equal(dataIdentity.sha256,target.dataSha256,`${target.id}: private DATA hash mismatch`);
        record.packageIdentity=packageIdentity;record.dataIdentity=dataIdentity;
        return {packageIdentity,dataIdentity};
      });
      await check('runtime-ready',['RUNTIME-01'],async()=>{
        await navigate(`http://127.0.0.1:${server.address().port}/host/${target.id}?epoch=1`);
        await page.waitForFunction(()=>events.some(event=>event.event==='ready'),null,{timeout:120000});
        frame=page.frames().find(frame=>frame.url().includes(`/runtime/${target.id}/`));assert.ok(frame);
        const ready=await page.evaluate(()=>events.find(event=>event.event==='ready'));
        assert.equal(ready.protocol,'eagler-touhou/1');assert.equal(ready.game,target.game);assert.equal(ready.epoch,1);return ready;
      });
      await check('configure-launch-first-frame',['RUNTIME-02'],async()=>{
        const sharedResources=Object.keys(config.shared??{}).map(name=>({path:`/${name}`,url:`/shared/${name}`}));
        await page.evaluate(({sharedResources,options})=>rpc('configure',{language:'ja',music:'none',resources:[],runtimeResources:[],sharedResources,thpracCatalog:[],options:{alwaysHitbox:false,touchEnabled:false,thpracEnabled:false,limitPresentationTo60:false,...options}}),{sharedResources,options:target.options??{}});
        await page.evaluate(()=>rpc('launch'));
        await page.waitForFunction(()=>events.some(event=>event.event==='first-frame'),null,{timeout:120000});
        await page.waitForTimeout(2500);
        const snapshot=await inspect();assert.ok(snapshot.canvas?.width>0&&snapshot.canvas?.height>0);return snapshot;
      });
      record.observations.title=await inspect();
      await page.screenshot({path:resolve(output,target.id+'-title.png')});
      await check('reject-unknown-command',['RUNTIME-03'],async()=>{
        const error=await page.evaluate(async()=>{try{await rpc('acceptance-invalid-command');return null;}catch(error){return error.message;}});
        assert.ok(error);assert.ok(!error.startsWith('Timeout:'));return error;
      },{fatal:false});
      await check('save-list-and-flush',['SAVE-02'],async()=>{
        const result=await page.evaluate(()=>rpc('list'));
        assert.ok(Array.isArray(result.files));await page.evaluate(()=>rpc('sync'));
        const escaped=await page.evaluate(async()=>{try{await rpc('read',{path:'../outside-acceptance.dat'});return null;}catch(error){return error.message;}});
        assert.ok(escaped&&!escaped.startsWith('Timeout:'));return {files:result.files,directoryEscapeRejected:escaped};
      },{fatal:false});
      const beforeKeyboard=await observe();
      const keyboardScenario=async()=>{
        await frame.locator('canvas').first().focus();
        if(target.game==='th20'){
          await waitObservation('main menu accepts input',state=>state.status?.titleState===1&&state.status.titlePhase===2);
          const neutral=await observe();
          assert.equal(neutral.input?.value,0,'A neutral controller must not create native menu input');
          const cursor=neutral.status.titleCursor;
          await page.waitForTimeout(600);
          const stillNeutral=await observe();
          assert.equal(stillNeutral.input?.value,0,'A neutral controller must remain neutral across native samples');
          assert.equal(stillNeutral.status.titleCursor,cursor,'A neutral controller must not move the title cursor');
          record.observations.neutralController={before:neutral,after:stillNeutral};
          for(let i=0;i<10&&(await observe()).status.titleCursor!==0;i++)await key('ArrowUp','ArrowUp',38);
          assert.equal((await observe()).status.titleCursor,0,'Select the ordinary game before confirming');
          await key('KeyZ','z',90);
          await waitObservation('difficulty accepts input',state=>state.status?.titleState===5&&state.status.titlePhase===2);
          await key('KeyZ','z',90);
          await waitObservation('character accepts input',state=>state.status?.titleState===6&&state.status.titlePhase===2);
          await key('KeyZ','z',90);
          await waitObservation('default stone page accepts input',state=>state.status?.titleState===7&&state.status.titlePhase===2&&
            state.ui?.stone?.state===3&&state.ui.stone.category===0);
          await key('KeyZ','z',90);
          await waitObservation('loadout accepted the stone selection',state=>state.status?.titleState===7&&state.status.titlePhase===3);
        }else{
          for(let i=0;i<32;i++){
            if(gameplayReady(await observe()))break;
            await key('KeyZ','z',90);
          }
        }
        await key('ControlLeft','Control',17,350);
        const ready=await waitObservation('gameplay ready before keyboard',gameplayReady);
        const worldClock=ready.nativeObservation?'simulation':'enemy';
        assert.ok(Number.isFinite(ready.frames[worldClock]),'Gameplay requires an observable world clock');
        await waitObservation('world advances before keyboard',state=>gameplayReady(state)&&state.frames[worldClock]>ready.frames[worldClock]);
        const stage=await inspect();record.observations.afterKeyboard=stage;
        if(!['th06','th07'].includes(target.game))assert.ok(stage.native.capabilities.phase||stage.native.capabilities.input||stage.native.capabilities.position,
          'Hosted keyboard requires a read-only native effect observer');
        if(stage.native.capabilities.phase){
          assertRuntimeObservation(stage.native,'phase',{name:target.game==='th09'?'match':'stage'});
          assert.notDeepEqual(stage.native.phase,beforeKeyboard.phase,'Menu input must enter gameplay');
        }
        if(stage.native.capabilities.input||stage.native.capabilities.position){
          const before=await observe();
          await sendKey('ArrowRight','ArrowRight',39,true);
          try {
            await page.waitForTimeout(180);const held=await observe();
            if(before.capabilities.input){assertRuntimeObservation(held,'input-held');assert.notEqual(held.input.value,before.input.value,'Held direction must change native input');}
            if(before.capabilities.position)assertRuntimeObservation(held,'position-moved',{before:before.position,axis:'x',direction:'positive'});
            record.observations.directionHeld=held;
          } finally {await sendKey('ArrowRight','ArrowRight',39,false);}
          await page.waitForTimeout(150);const released=await observe();
          if(before.capabilities.input)assertRuntimeObservation(released,'input-released',{value:before.input.value});
          record.observations.directionReleased=released;
        }
        await page.screenshot({path:resolve(output,target.id+'-keyboard.png')});
        return stage;
      };
      await check('hosted-keyboard-native-effect',['INPUT-01'],keyboardScenario,{verified:stage=>
        stage.native.capabilities.input&&stage.native.capabilities.position&&(!stage.native.nativeObservation||stage.native.localSeat===0),fatal:false});
      if(gamepad)await check('native-gamepad-sampling',[],async()=>{
        if((await observe()).nativeObservation)await waitObservation('gameplay ready before gamepad',gameplayReady);
        await bindProtocolHostGamepad(page,target.game,1);
        const result=await probeNativeGamepad(page,frame,gamepadTargetFor(target.game,target.variant,0));
        record.checks.push({id:'native-gamepad-effect',checks:['INPUT-04','INPUT-06'],status:result.effectVerified?'passed':'observed',complete:false,
          coverage:result.coverage});
        return result;
      },{fatal:false});
      const lifecycle=async()=>{
        if((await observe()).nativeObservation)await waitObservation('gameplay ready before pause',gameplayReady);
        await key('Escape','Escape',27);await page.waitForTimeout(500);
        const paused=await inspect();await page.screenshot({path:resolve(output,target.id+'-paused.png')});
        if(paused.native.capabilities.pause){
          assertRuntimeObservation(paused.native,'pause-active');
          const keyName=paused.native.capabilities.restartFrame;
          const simulationKey=Number.isFinite(paused.native.frames.simulation)?'simulation':keyName;
          if(simulationKey){
            await page.waitForTimeout(350);assertRuntimeObservation(await observe(),'frame-frozen',{key:simulationKey,before:paused.native.frames[simulationKey]});
          }
        }
        await key('KeyR','r',82);
        if(target.game==='th20'){
          const confirmation=await waitObservation('retry confirmation accepts input',state=>state.ui?.pause?.retryConfirmInputReady);
          assertRuntimeObservation(confirmation,'pause-active');
          assertRuntimeObservation(confirmation,'frame-frozen',{key:'simulation',before:paused.native.frames.simulation});
          assert.equal(confirmation.ui.pause.cursor,1,'Retry confirmation initially selects no');
          await key('ArrowUp','ArrowUp',38);
          await waitObservation('retry confirmation selects yes',state=>state.ui?.pause?.retryConfirmInputReady&&state.ui.pause.cursor===0);
          await key('KeyZ','z',90);
          record.observations.retryConfirmation=confirmation;
        }
        const restartClock=paused.native.capabilities.restartFrame;
        const restartBoundary=paused.native.nativeObservation
          ?await waitObservation('gameplay rebuilt after restart',state=>gameplayReady(state)&&
            (!restartClock||state.frames[restartClock]<paused.native.frames[restartClock]))
          :null;
        if(!restartBoundary)await page.waitForTimeout(1000);
        const afterRestartKey=await inspect();await page.screenshot({path:resolve(output,target.id+'-after-restart-key.png')});
        record.observations.restart={paused,afterRestartKey,restartBoundary};
        const restarted=restartBoundary??afterRestartKey.native;
        if(paused.native.capabilities.pause)assertRuntimeObservation(restarted,'pause-inactive');
        if(paused.native.capabilities.restartFrame)assertRuntimeObservation(restarted,'restart-frame-reset',{before:paused.native.frames[paused.native.capabilities.restartFrame]});
        if(restarted.nativeObservation){
          const resumed=restarted;
          const frameName=Number.isFinite(resumed.frames.simulation)?'simulation':resumed.capabilities.restartFrame;
          assert.ok(frameName,'Restart requires a native gameplay clock');
          const advancing=await waitObservation('world advances after restart',state=>gameplayReady(state)&&state.frames[frameName]>resumed.frames[frameName]);
          record.observations.restart.resumed={before:resumed,after:advancing};
        }
        return record.observations.restart;
      };
      const lifecycleCapabilities=(await observe()).capabilities;
      if(lifecycleCapabilities.pause&&lifecycleCapabilities.restartFrame)
        await check('native-pause-and-restart',['RUNTIME-06'],lifecycle,{fatal:false});
      else if(lifecycleCapabilities.pause)
        await check('native-pause-state',[],lifecycle,{fatal:false,verified:()=>false});
      else record.observations.lifecycle={status:'observed',observation:await lifecycle(),reason:'No read-only native pause or restart observer'};
      await check('resize-and-continued-rendering',[],async()=>{
        const before=await observe(),healthBefore=await page.evaluate(()=>events.filter(event=>event.event==='frame-health').length);
        await page.setViewportSize({width:800,height:1000});
        await page.locator('iframe').evaluate(element=>{element.style.width='780px';element.style.height='900px';});
        await page.waitForTimeout(1800);await page.screenshot({path:resolve(output,target.id+'-portrait.png')});
        const resized=await inspect();
        if(before.capabilities.presentationFrame)assertRuntimeObservation(resized.native,'presentation-advanced',{after:before.render.presentationFrame});
        else {
          const health=await page.evaluate(()=>events.filter(event=>event.event==='frame-health'));
          assert.ok(health.length>healthBefore&&Number.isFinite(health.at(-1).fps)&&health.at(-1).fps>0,'Resize must be followed by a new native frame-health sample');
        }
        return resized;
      },{fatal:false});
      await check('reload-storage-and-epoch',['SAVE-03','SAVE-04','RUNTIME-01'],async()=>{
        await page.evaluate(()=>rpc('sync'));
        const listed=(await page.evaluate(()=>rpc('list'))).files;
        const storage=PRODUCT_GAMES[target.game].storage;
        const selected=listed.filter(file=>[storage.scoreFile,...(storage.configFiles??[])].includes(file.path));
        assert.ok(selected.length,'Runtime must persist at least a score or configuration file');
        const contents={};
        for(const file of selected){const value=await page.evaluate(path=>rpc('read',{path}),file.path);contents[file.path]=value.bytes;
          const directory=resolve(output,'user-files',target.id);await mkdir(directory,{recursive:true});
          await writeFile(resolve(directory,file.path),new Uint8Array(value.bytes));}
        record.priorSessionEvents=evidenceEvents(await page.evaluate(()=>window.events));
        await navigate(`http://127.0.0.1:${server.address().port}/host/${target.id}?epoch=2`);
        await page.waitForFunction(()=>events.some(event=>event.event==='ready'&&event.epoch===2),null,{timeout:120000});
        for(const [path,expected] of Object.entries(contents)){
          const actual=await page.evaluate(path=>rpc('read',{path}),path);assert.deepEqual(actual.bytes,expected,path);}
        const stale='old-epoch-request';
        await page.evaluate(request=>send('sync',{epoch:1,request}),stale);await page.waitForTimeout(350);
        assert.equal(await page.evaluate(request=>events.some(event=>event.request===request),stale),false);
        return{files:Object.fromEntries(Object.entries(contents).map(([name,bytes])=>[name,{bytes:bytes.length,sha256:hash(new Uint8Array(bytes))}])),oldEpochIgnored:true};
      },{fatal:false});
      await check('replay-file-roundtrip-persistence',['REPLAY-05','SAVE-02','SAVE-03','SAVE-04'],async()=>{
        const path=`replay/${PRODUCT_GAMES[target.game].replay.prefix}_udtest.rpy`;
        const fixture=await loadRuntimeReplayFixture(target),bytes=Array.from(fixture.contents);
        const rejected=[];
        for(const command of ['read','write','remove']){
          const error=await page.evaluate(async({command,bytes})=>{
            try{await rpc(command,{path:'../outside-acceptance.dat',bytes});return null;}
            catch(error){return error.message;}
          },{command,bytes});
          assert.ok(error&&!error.startsWith('Timeout:'),`${command} must reject paths outside the declared storage`);
          rejected.push({command,error});
        }
        await page.evaluate(({path,bytes})=>rpc('write',{path,bytes}),{path,bytes});
        assert.deepEqual((await page.evaluate(path=>rpc('read',{path}),path)).bytes,bytes);
        assert.ok((await page.evaluate(()=>rpc('list'))).files.some(file=>file.path===path));
        const reopen=async epoch=>{
          await page.evaluate(()=>rpc('sync'));
          record.priorSessionEvents=[...(record.priorSessionEvents??[]),...evidenceEvents(await page.evaluate(()=>window.events))];
          await navigate(`http://127.0.0.1:${server.address().port}/host/${target.id}?epoch=${epoch}`);
          await page.waitForFunction(epoch=>events.some(event=>event.event==='ready'&&event.epoch===epoch),epoch,{timeout:120000});
        };
        await reopen(3);
        assert.deepEqual((await page.evaluate(path=>rpc('read',{path}),path)).bytes,bytes);
        await page.evaluate(path=>rpc('remove',{path}),path);
        await reopen(4);
        assert.ok(!(await page.evaluate(()=>rpc('list'))).files.some(file=>file.path===path));
        assert.deepEqual((await loadRuntimeReplayFixture(target)).identity,fixture.identity,'Replay input changed during acceptance');
        return{path,bytes:bytes.length,sha256:hash(new Uint8Array(bytes)),restoredAcrossNewInstance:true,deletionPersisted:true,rejected,
          fixture:fixture.identity,scope:'An existing replay passes the real import validator and file operations through Runtime browser storage. Playback is a separate check.'};
      },{fatal:false});
      await check('runtime-errors',[],async()=>{
        const packageAfter=await verifyRuntimePackage(target,{repoRoot:root}),dataAfter=await verifyPrivateData(target.data);
        record.inputPostflight={packageIdentity:packageAfter,dataIdentity:dataAfter};
        assert.deepEqual(packageAfter,record.packageIdentity,'Runtime source, build plan, receipts or package changed during acceptance');
        assert.deepEqual(dataAfter,record.dataIdentity,'Private DATA changed during acceptance');
        assert.deepEqual(record.errors,[]);
        const events=[...(record.priorSessionEvents??[]),...await page.evaluate(()=>window.events)];
        record.rejectedInputRequests=events.filter(event=>record.inputRequests?.includes(event.request)&&event.ok===false);
        assert.deepEqual(record.rejectedInputRequests,[],'Runtime rejected a test keyboard event');
        assert.deepEqual(events.filter(event=>['error','abort'].includes(event.event)),[]);
        assert.deepEqual(record.httpErrors,[]);
        assert.deepEqual(record.consoleErrors.filter(message=>!message.startsWith('Failed to load resource:')),[]);
        return{expectedNavigationAborts:record.expectedNavigationAborts};
      },{fatal:false});
    }catch(error){record.failure=String(error.stack||error);await page.screenshot({path:resolve(output,target.id+'-failure.png')}).catch(()=>{});}
    finally{
      record.events=[...(record.priorSessionEvents??[]),...evidenceEvents(await page.evaluate(()=>window.events??[]).catch(()=>[]))];
      delete record.priorSessionEvents;
      Object.assign(record,runtimeScenarioResult(record));await context.close();await save();
      console.log(`${target.id}: ${record.status.toUpperCase()} ${record.failure?.split('\n')[0]??record.unverifiedChecks.join(', ')}`);
    }
  }
}finally{await browser?.close();await new Promise(done=>server.close(done));report.finishedAt=new Date().toISOString();
  report.status=report.targets.length===selection.expected.length&&report.targets.every(target=>target.passed)?'passed':report.targets.some(target=>target.status==='failed')?'failed':'incomplete';
  report.passed=report.status==='passed';await save();}


});
if(!result.passed){process.exitCode=1;if(result.failure)console.error(result.failure);}
