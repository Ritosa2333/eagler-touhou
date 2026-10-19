import assert from 'node:assert/strict';
import {mkdir,readFile,writeFile} from 'node:fs/promises';
import {resolve} from 'node:path';
import {createHash} from 'node:crypto';
import {execFileSync} from 'node:child_process';
import {PRODUCT_GAMES} from '../../common/contracts/product-catalog.mjs';
import {gamepadProbeBundle,gamepadProbeFailureEvidence,gamepadTargetFor,prepareVirtualGamepad,probeNativeGamepad} from './gamepad-probe.mjs';
import {selectRoomTargets,selectRuntimeTargets,verifyPublishedRuntime,verifyRuntimePackage} from './acceptance-contracts.mjs';
import {inspectRoomResources,inspectRoomSiteAssets,recordRoomSiteAssetFailures,verifyRoomResourcePostflight} from './room-resources.mjs';
import {classifyRoomConsoleErrors,roomProtocolErrorEntries,stampRoomProtocolEvent} from './room-errors.mjs';
import {compareTh08FrozenWorld,compareTh08LiveWorld,th08DisconnectedWaitState} from './th08-world-progress.mjs';
import {readRuntimeObservation} from './runtime-observations.mjs';
import {sourceSnapshot} from '../testing/source-snapshot.mjs';
import {runWithReport} from '../testing/cli-report.mjs';

const option=name=>process.argv.find(value=>value.startsWith(`--${name}=`))?.slice(name.length+3);
const requestedOutput=option('output');
if(!requestedOutput)throw Error('Required: --output=directory');
const result=await runWithReport({output:resolve(requestedOutput),schema:'eagler-touhou/room-browser/1'},async(report,save)=>{
const base=option('url'),out=option('output'),route=option('route'),playersText=option('players'),manifestPath=option('manifest');
const disconnect=process.argv.includes('--disconnect');
const gamepad=process.argv.includes('--gamepad'),gamepadBundle=gamepad?await gamepadProbeBundle():null;
assert.ok(base&&manifestPath&&out,'Required: --url=local-launcher --manifest=private-inputs.json --game=thXX[,thYY] --players=2|3 --route=rtc|relay --output=private-directory');
assert.ok(playersText&&/^[23]$/.test(playersText),'--players must be explicitly set to 2 or 3');
const players=Number(playersText),selection=selectRoomTargets({game:option('game'),players,route}),games=selection.games;
const directory=resolve(out);await mkdir(directory,{recursive:true});
const config=JSON.parse(await readFile(resolve(manifestPath),'utf8'));
const runtimeSelection=selectRuntimeTargets(config,{game:games.join(','),variant:'multiplayer'});
const runtimeTargets=new Map(runtimeSelection.targets.map(target=>[target.game,target]));
const root=resolve(import.meta.dirname,'../..');
const sourcePackages=new Map(),sourcePackageErrors=new Map();
for(const game of games){
  const target=runtimeTargets.get(game);
  try{sourcePackages.set(game,await verifyRuntimePackage(target,{repoRoot:root}));}
  catch(error){sourcePackageErrors.set(game,String(error.stack||error));}
}
let packages={},packageMapError=null;
if(option('packages'))try{packages=JSON.parse(await readFile(resolve(option('packages')),'utf8'));}
catch(error){packages=null;packageMapError=`Cannot read --packages=${option('packages')}: ${error.message}`;}
Object.assign(report,{url:base,route,players,
  selection:{games,players,route,expectedTargets:selection.expected},
  scope:'Real Launcher room controls, hash-identified current multiplayer Runtime and current private DATA; desktop browser session acceptance',targets:[]});
report.disconnectRequested=disconnect;
report.commit=execFileSync('git',['rev-parse','HEAD'],{encoding:'utf8',windowsHide:true}).trim();
report.sourceSnapshot=sourceSnapshot(root);
report.sourcePackages=Object.fromEntries(sourcePackages);
report.sourcePackageErrors=Object.fromEntries(sourcePackageErrors);
report.harnessSha256=createHash('sha256').update(await readFile(new URL(import.meta.url))).digest('hex');
report.helperSources={};
for(const name of ['room-errors.mjs','runtime-observations.mjs','native-observation-contract.mjs','th08-world-progress.mjs']){
  const bytes=await readFile(new URL(name,import.meta.url));
  report.helperSources[name]=createHash('sha256').update(bytes).digest('hex');
  await writeFile(resolve(directory,name),bytes);
}
const observationReader=readRuntimeObservation.toString();
report.observationReaderSha256=createHash('sha256').update(observationReader).digest('hex');
await writeFile(resolve(directory,'observation-reader.mjs'),observationReader);
if(gamepad){report.gamepadProbeSha256=createHash('sha256').update(await readFile(new URL('./gamepad-probe.mjs',import.meta.url))).digest('hex');report.gamepadBundleSha256=createHash('sha256').update(gamepadBundle).digest('hex');await writeFile(resolve(directory,'gamepad-bundle.js'),gamepadBundle);await writeFile(resolve(directory,'gamepad-probe.mjs'),await readFile(new URL('./gamepad-probe.mjs',import.meta.url)));}
await writeFile(resolve(directory,'harness.mjs'),await readFile(new URL(import.meta.url)));
report.resourcePreparation=await inspectRoomResources({baseUrl:base,targets:[...runtimeTargets.values()],packageMap:packages,packageMapError});
report.siteAssets=await inspectRoomSiteAssets({baseUrl:base});
report.siteAssetsPassed=report.siteAssets.passed;
await save();
async function dismiss(page){
  await page.waitForFunction(()=>localStorage.getItem('eagler-touhou-first-use-notice-seen-v1')==='1'||document.querySelector('#firstUseNoticeDialog')?.open===true,null,{timeout:30000});
  if(await page.locator('#firstUseNoticeDialog[open]').count()){
    await page.locator('#firstUseNoticeClose').click();await page.waitForFunction(()=>!document.querySelector('#firstUseNoticeDialog')?.open);
  }
}
async function roomSnapshot(page){const snapshot=await page.evaluate(()=>({body:document.body.innerText,
  ready:document.querySelector('#mpReady')?.getAttribute('aria-pressed'),
  startDisabled:document.querySelector('#mpStartGame')?.disabled,
  sessions:Object.fromEntries(Object.entries(sessionStorage).filter(([key])=>key.includes('room'))),events:window.acceptanceEvents??[]}));
  snapshot.resources=await roomResourceSnapshot(page);return snapshot;}
async function roomEntryDiagnostic(page){return page.evaluate(()=>{
  const text=selector=>document.querySelector(selector)?.textContent?.trim()??'';
  const visible=element=>!!element&&!element.hidden&&element.getClientRects().length>0;
  const code=document.querySelector('#roomCodeInput'),field=document.querySelector('#codeField');
  const error=document.querySelector('#formError'),submit=document.querySelector('#submitRoom');
  const warning=document.querySelector('#connectionWarning'),membership=document.querySelector('#membership');
  const notice=document.querySelector('#notice');
  return{url:location.href,roomDialogOpen:document.querySelector('#roomDialog')?.open??false,
    product:document.querySelector('#gameSelect')?.value??null,mode:document.querySelector('#createFields')?.hidden===false?'create':'join',
    code:{visible:visible(field),value:code?.value??null,valid:code?.validity?.valid??null,
      validity:code?.validity?{valueMissing:code.validity.valueMissing,patternMismatch:code.validity.patternMismatch,
        tooShort:code.validity.tooShort,tooLong:code.validity.tooLong,customError:code.validity.customError}:null,
      validationMessage:code?.validationMessage??''},
    formError:{visible:visible(error),text:text('#formError')},
    submit:{visible:visible(submit),disabled:submit?.disabled??null,text:text('#submitRoom')},
    connection:{browserOnline:navigator.onLine,warningVisible:visible(warning),warningTitle:text('#connectionWarningTitle'),
      warningHint:text('#connectionWarningHint'),membershipVisible:visible(membership),membershipTitle:text('#membershipTitle'),
      noticeVisible:visible(notice),notice:text('#notice')},
    sessions:Object.fromEntries(Object.entries(sessionStorage).filter(([key])=>key.toLowerCase().includes('room'))),
    roomSessionKeys:Object.keys(sessionStorage).filter(key=>key.toLowerCase().includes('room'))};
});}
async function waitForRoomEntry(page,seat){
  let outcome;
  try{
    const handle=await page.waitForFunction(()=>{
      const room=document.querySelector('#mpRoomCode');
      if(room&&!room.hidden&&room.getClientRects().length)return{kind:'success'};
      const dialog=document.querySelector('#roomDialog');
      if(!dialog?.open)return false;
      const error=document.querySelector('#formError'),code=document.querySelector('#roomCodeInput');
      const field=document.querySelector('#codeField');
      const visible=element=>!!element&&!element.hidden&&element.getClientRects().length>0;
      if(visible(error)&&error.textContent.trim())return{kind:'failure',reason:'form-error'};
      if(visible(field)&&code&&!code.validity.valid)return{kind:'failure',reason:'invalid-room-code'};
      return false;
    },null,{timeout:30000});
    outcome=await handle.jsonValue();
  }catch(error){
    const diagnostic=await roomEntryDiagnostic(page).catch(snapshotError=>({snapshotError:String(snapshotError.message??snapshotError)}));
    const failure=new Error(`P${seat+1} room entry did not reach the room route or an explicit form failure within 30000ms: ${JSON.stringify(diagnostic)}; ${error.message}`);
    failure.roomEntryDiagnostic={outcome:'timeout',...diagnostic};
    throw failure;
  }
  const diagnostic=await roomEntryDiagnostic(page);
  if(outcome.kind==='failure'){
    const failure=new Error(`P${seat+1} room form reported ${outcome.reason}: ${JSON.stringify(diagnostic)}`);
    failure.roomEntryDiagnostic={outcome:'failure',reason:outcome.reason,...diagnostic};
    throw failure;
  }
  return{outcome:'success',...diagnostic};
}
async function roomResourceSnapshot(page){return page.evaluate(()=>{
  const progress=document.querySelector('#mpRoomResourceProgress');
  const retry=document.querySelector('#mpRoomResourceRetry');
  return{status:progress?.dataset.status??null,label:document.querySelector('#mpRoomResourceLabel')?.textContent?.trim()??'',
    detail:progress?.title??'',retryVisible:retry?.hidden===false,readyDisabled:document.querySelector('#mpReady')?.disabled??true};
});}
async function waitForRoomResources(page,record,seat){
  const terminal=()=>page.waitForFunction(()=>['ready','failed','cancelled'].includes(
    document.querySelector('#mpRoomResourceProgress')?.dataset.status??''),null,{timeout:120000});
  try{await terminal();}catch(error){throw Error(`P${seat+1} resource preparation did not report ready/failed/cancelled: ${JSON.stringify(await roomResourceSnapshot(page))}; ${error.message}`);}
  let snapshot=await roomResourceSnapshot(page);
  if(snapshot.status!=='ready'&&snapshot.retryVisible){
    record.steps.push({name:`P${seat+1}-resource-retry`,at:new Date().toISOString(),previous:snapshot});
    await save();
    await page.locator('#mpRoomResourceRetry').click();
    try{await terminal();}catch(error){throw Error(`P${seat+1} resource retry did not report ready/failed/cancelled: ${JSON.stringify(await roomResourceSnapshot(page))}; ${error.message}`);}
    snapshot=await roomResourceSnapshot(page);
  }
  assert.equal(snapshot.status,'ready',`P${seat+1} room resources are ${snapshot.status}: ${JSON.stringify(snapshot)}`);
  assert.equal(snapshot.readyDisabled,false,`P${seat+1} resources report ready but the native Ready control is disabled: ${JSON.stringify(snapshot)}`);
  return snapshot;
}
async function nativeSnapshot(page,game,seat){
  const handle=await page.locator('#gameFrame').elementHandle();
  const frame=handle?await handle.contentFrame():null;
  const [snapshot,observation]=await Promise.all([page.evaluate(()=>{
  const r=document.querySelector('#gameFrame')?.contentWindow;
  if(!r)return null;
  const runtime=Object.keys(r).filter(key=>/^__th\d+Runtime$/.test(key)).map(key=>r[key])[0];
  const module=runtime?.Module??r.Module,core=runtime?.core??r.core??module;
  let native=null;
  for(const name of ['th11_mp_status','_th15_mp_status'])if(typeof core?.[name]==='function'){
    const memory=core.memory?.buffer??module?.HEAPU8?.buffer;
    if(memory)native=Array.from(new Uint32Array(memory,core[name](),24));
  }
  const options=module?.eaglerOptions??core?.eaglerOptions??{};
  return{active:r.__eaglerNetplayLanActive??(native?!!native[1]:false),
    frame:r.__eaglerNetplayLanFrame??native?.[3]??-1,confirmed:r.__eaglerNetplayLanConfirmed??native?.[4]??-1,
    route:r.__eaglerPeerTransport?.route??r.__eaglerNetplayTransport??'',
    failed:r.__eaglerNetplayFailed??false,error:r.__eaglerNetplayError??'',native,
    transportState:{disconnected:r.__eaglerPeerTransport?.disconnected===true,recovering:r.__eaglerPeerTransport?.isRecovering?.()===true},
    state:runtime?.status?.()??null,gameplayStage:r.__eaglerNetplayGameplayStage??null,
    options:{player:options.netplayPlayer,players:options.netplayPlayerCount,mode:options.netplayMode,prediction:options.netplayPredictionLimit},
    runtimeUrl:r.location?.href??'',events:window.acceptanceEvents??[]};
  }),frame?frame.evaluate(readRuntimeObservation,{game,variant:'multiplayer',localSeat:seat}):null]);
  return snapshot?{...snapshot,observation,observationTarget:{game,variant:'multiplayer',localSeat:seat}}:null;
}
async function gamepadFailureSnapshot(page,game,seat){
  const [room,native,iframe]=await Promise.all([
    roomSnapshot(page).catch(error=>({error:String(error.message??error)})),
    nativeSnapshot(page,game,seat).catch(error=>({error:String(error.message??error)})),
    page.evaluate(()=>{const frame=document.querySelector('#gameFrame');let url=null,documentState=null;
      try{url=frame?.contentWindow?.location?.href??null;documentState=frame?.contentDocument?.readyState??null;}catch{}
      return{exists:!!frame,connected:frame?.isConnected??false,src:frame?.getAttribute('src')??null,url,documentState};
    }).catch(error=>({error:String(error.message??error)}))
  ]);
  const events=room?.events??[];
  const {events:ignoredEvents,...roomState}=room??{};
  return{seat,room:roomState,native,iframe,runtimeExitEvents:events.filter(event=>event?.event==='exit'),
    recentProtocolEvents:events.slice(-12)};
}
async function protocolEventSnapshot(page){
  return page.evaluate(()=>({events:window.acceptanceEvents??[]}));
}
function requireSimulationFrame(snapshot,game,seat,label){
  const observation=snapshot?.observation;
  assert.ok(observation,`${game} P${seat+1}: native Runtime observation is unavailable at ${label}`);
  assert.equal(observation.game,game,`${game} P${seat+1}: wrong native observation game at ${label}`);
  assert.equal(observation.variant,'multiplayer',`${game} P${seat+1}: wrong native observation variant at ${label}`);
  assert.equal(observation.localSeat,seat,`${game} P${seat+1}: observer belongs to local seat ${observation.localSeat}, expected ${seat} at ${label}`);
  const frame=observation.frames?.simulation;
  assert.ok(Number.isSafeInteger(frame)&&frame>=0,
    `${game} P${seat+1}: native simulation frame is unavailable at ${label}; frames=${JSON.stringify(observation.frames)}`);
  return frame;
}
function assertRoomErrors(record,entries){
  const classification=classifyRoomConsoleErrors({entries,disconnect:record.disconnect});
  record.errorClassification=classification;
  assert.deepEqual(classification.unexpected,[],`Unexpected browser or Runtime errors: ${JSON.stringify(classification.unexpected)}`);
  assert.deepEqual(record.errors,[],'Browser page errors');
}
const records=new Map(games.map(game=>{
  const resource=report.resourcePreparation.games.find(item=>item.game===game);
  const record={game,players,route,resourcePreparation:resource??{status:'failed',error:{code:'resource-result-missing',message:`${game}: resource preflight returned no result`}},
    steps:[],errors:[],consoleErrors:[],httpErrors:[]};report.targets.push(record);return[game,record];
}));
for(const game of games){
  const record=records.get(game);
  if(record.resourcePreparation.status!=='ready'){
    record.passed=false;record.stage='resource-preflight';record.error=record.resourcePreparation.error?.message??`${game}: resource preflight failed`;
    continue;
  }
  if(sourcePackageErrors.has(game)){
    record.passed=false;record.stage='runtime-package';record.error=sourcePackageErrors.get(game);
    continue;
  }
  try{record.runtimeIdentity=await verifyPublishedRuntime(base,game,'multiplayer',{expectedPackageSha256:sourcePackages.get(game).packageSha256});}
  catch(error){record.passed=false;record.stage='runtime-identity';record.error=String(error.stack||error);}
}
await save();
try{
  for(const game of games){
    const record=records.get(game);if(!record.runtimeIdentity||record.resourcePreparation.status!=='ready')continue;
    let browser;const contexts=[],pages=[],pendingConsoleSamples=[];
    const step=async(name,operation)=>{record.stage=name;record.steps.push({name,at:new Date().toISOString()});await save();return operation();};
    const awaitPendingConsoleSamples=async()=>{
      while(pendingConsoleSamples.length)await Promise.all(pendingConsoleSamples.splice(0));
    };
    try{
      const {launchBrowser}=await import('../browser/launch.mjs');
      browser=await launchBrowser({args:['--enable-unsafe-swiftshader','--autoplay-policy=no-user-gesture-required',
        '--disable-background-timer-throttling','--disable-renderer-backgrounding']});report.browser=browser.version();
      for(let seat=0;seat<players;seat++){
        const context=await browser.newContext({viewport:{width:1280,height:900}});contexts.push(context);
        if(gamepad)await prepareVirtualGamepad(context,gamepadBundle,gamepadTargetFor(game,'multiplayer',seat));
        if(route==='relay')await context.addInitScript(()=>Object.defineProperty(window,'RTCPeerConnection',{value:undefined,configurable:true}));
        await context.addInitScript({content:`{
          const stampRoomProtocolEvent=(${stampRoomProtocolEvent.toString()});
          window.acceptanceEvents=[];
          addEventListener('message',event=>{
            if(event.source!==document.querySelector('#gameFrame')?.contentWindow||event.data?.protocol!=='eagler-touhou/1')return;
            const runtime=event.source;
            const nativeAtReceipt={failed:runtime.__eaglerNetplayFailed===true,
              error:String(runtime.__eaglerNetplayError??''),
              transportState:{disconnected:runtime.__eaglerPeerTransport?.disconnected===true}};
            window.acceptanceEvents.push(stampRoomProtocolEvent(event.data,Date.now(),nativeAtReceipt));
          });
        }`});
        const page=await context.newPage();pages.push(page);
        page.on('pageerror',error=>record.errors.push({seat,error:String(error)}));
        page.on('console',message=>{if(message.type()==='error'){
          const entry={seat,stage:record.stage,checkerMs:Date.now(),kind:'console-error',text:message.text(),native:null};
          record.consoleErrors.push(entry);
          pendingConsoleSamples.push(nativeSnapshot(page,game,seat).then(native=>{entry.native=native;})
            .catch(error=>{entry.observationError=String(error.message??error);}));
        }});
        page.on('response',response=>{if(response.status()>=400&&!response.url().endsWith('/favicon.ico'))record.httpErrors.push({seat,url:response.url(),status:response.status()});});
        await step(`P${seat+1}-directory`,async()=>{
          await page.goto(base);await page.waitForFunction(()=>window.__eaglerBoot?.done===true);await dismiss(page);
          await page.locator(`[data-product="${game}mp"]`).click();await page.locator('#createButton').waitFor({state:'visible'});
        });
        await step(`P${seat+1}-room`,async()=>{
          await page.locator(seat===0?'#createButton':'#codeButton').click();
          await page.locator('#gameSelect').selectOption(game+'mp');
          if(seat===0){
            if(await page.locator('#capacitySelect').isEnabled())await page.locator('#capacitySelect').selectOption(String(players));
            else assert.equal(await page.locator('#capacitySelect').inputValue(),String(players));
            if(!PRODUCT_GAMES[game].multiplayer.inGameSelection)await page.locator('#difficultySelect').selectOption('1');
            await page.locator('#visibilitySelect').selectOption('private');
          }
          else await page.locator('#roomCodeInput').fill(record.room);
          record.roomEntryDiagnostics??=[];
          await page.locator('#submitRoom').click();
          try{record.roomEntryDiagnostics[seat]=await waitForRoomEntry(page,seat);}
          catch(error){record.roomEntryDiagnostics[seat]=error.roomEntryDiagnostic??
            await roomEntryDiagnostic(page).catch(snapshotError=>({snapshotError:String(snapshotError.message??snapshotError)}));throw error;}
          const me=page.locator(`[data-mp-seat-me="${seat}"]`);
          await page.waitForFunction(seat=>document.querySelector(`[data-mp-seat-me="${seat}"]`)?.hidden===false||
            document.querySelector(`[data-mp-seat-drop="${seat}"] button`)?.disabled===false,seat);
          if(!(await me.evaluate(element=>element.hidden===false)))await page.locator(`[data-mp-seat-drop="${seat}"] button`).click();
          await page.waitForFunction(seat=>document.querySelector(`[data-mp-seat-me="${seat}"]`)?.hidden===false,seat);
          record.room=(await page.locator('#mpRoomCode').innerText()).trim();
          if(await page.locator('#mpMusicSelect').count())await page.locator('#mpMusicSelect').selectOption('none',{force:true});
          if(record.resourcePreparation.source==='offline-package'){
            await page.locator('#mpSettingsRoomDrawerToggle').click();
            await page.locator('#mpGamePackageImport').click();
            await page.locator('#gameDataImportWindow').waitFor({state:'visible'});
            const chooser=page.waitForEvent('filechooser');await page.locator('#transferImport').click();
            await(await chooser).setFiles(packages[game]);
            await page.locator('#gameDataImportWindow').waitFor({state:'hidden',timeout:120000});
            await page.locator('#mpGamePackageImport').focus();await page.keyboard.press('Escape');
            await page.locator('#mpSettingsRoomDrawer').waitFor({state:'hidden'});
          }
          record.seatResources??=[];
          record.seatResources[seat]=await waitForRoomResources(page,record,seat);
        });
      }
      for(let seat=0;seat<players;seat++)await step(`P${seat+1}-ready`,async()=>{
        await pages[seat].locator('#mpReady').click();
        await pages[seat].waitForFunction(()=>document.querySelector('#mpReady')?.getAttribute('aria-pressed')==='true',null,{timeout:120000});
      });
      record.ready=await Promise.all(pages.map(roomSnapshot));
      await step('synchronized-start',async()=>{
        await pages[0].waitForFunction(()=>!document.querySelector('#mpStartGame')?.disabled,null,{timeout:30000});
        await pages[0].locator('#mpStartGame').click();
        for(const page of pages){
          await page.waitForFunction(()=>document.querySelector('#decisionDialog')?.open||
            window.acceptanceEvents?.some(event=>event.event==='first-frame'),null,{timeout:120000});
          if(await page.locator('#decisionDialog[open]').count()){
            const message=await page.locator('#decisionDialog').innerText();
            assert.match(message,/无音乐|without (?:background )?music|no (?:background )?music/i,'Unexpected launch confirmation');
            (record.launchConfirmations??=[]).push(message);
            await page.locator('#decisionConfirm').click();
          }
        }
        for(const page of pages)await page.waitForFunction(()=>window.acceptanceEvents?.some(event=>event.event==='first-frame'),null,{timeout:120000});
      });
      if(PRODUCT_GAMES[game].multiplayer.inGameSelection)await step('native-versus-selection',async()=>{
        for(let round=0;round<12;round++)for(const page of pages){
          await page.bringToFront();await page.frameLocator('#gameFrame').locator('canvas').first().focus();
          await page.keyboard.press('z',{delay:100});await page.waitForTimeout(220);
        }
        for(const page of pages)await page.keyboard.down('Control');
        await pages[0].waitForTimeout(1600);for(const page of pages)await page.keyboard.up('Control');
      });
      await step('actual-native-frame-180',async()=>{
        const deadline=Date.now()+120000;
        while(true){record.native=await Promise.all(pages.map((page,seat)=>nativeSnapshot(page,game,seat)));
          if(record.native.every(state=>state?.active&&state.frame>=180))break;
          if(record.native.some(state=>state?.failed||state?.error))throw Error('Native session failed: '+JSON.stringify(record.native));
          if(Date.now()>deadline)throw Error('Native session did not reach frame 180: '+JSON.stringify(record.native));
          await pages[0].waitForTimeout(200);
        }
        for(let seat=0;seat<players;seat++){
          const state=record.native[seat];assert.equal(new URL(state.runtimeUrl).pathname,new URL(record.runtimeIdentity.entryUrl).pathname,'Room launched a Runtime other than the hash-identified current package');
          assert.equal(state.options.player,seat);assert.equal(state.options.players,players);assert.equal(state.options.mode,'lan');assert.equal(state.route,route);
          if(game==='th09')assert.equal(state.state?.session?.[0],1,'TH09 must enter an actual versus match');
        }
      });
      if(gamepad)await step('native-gamepad-sampling',async()=>{
        record.gamepad=[];let firstFailure=null;
        for(let seat=0;seat<pages.length;seat++){const page=pages[seat];
          const target=gamepadTargetFor(game,'multiplayer',seat);
          try{const element=await page.locator('#gameFrame').elementHandle();
            const frame=element?await element.contentFrame():null;assert.ok(frame,'Actual Runtime frame required');
            const runtime=await nativeSnapshot(page,game,seat),frameUrl=frame.url();
            if(frameUrl==='about:blank'||runtime?.active!==true){
              const exits=(runtime?.events??[]).filter(event=>event?.event==='exit');
              throw Error(`P${seat+1} Runtime is no longer active before native gamepad sampling; iframe=${frameUrl}; runtimeUrl=${runtime?.runtimeUrl??null}; native=${JSON.stringify(runtime?.observation??null)}; exitEvents=${JSON.stringify(exits)}`);
            }
            const result=await probeNativeGamepad(page,frame,target);assert.equal(result.effectVerified,true,'MP native gamepad effect was not verified');
            record.gamepad.push({passed:true,...result});}
          catch(error){firstFailure??=error;const evidence=gamepadProbeFailureEvidence(error,target);
            evidence.failureSnapshot=await gamepadFailureSnapshot(page,game,seat);
            record.gamepad.push(evidence);}}
        const failures=record.gamepad.filter(result=>!result.passed);
        if(failures.length){
          record.gamepadFailure={firstFailedSeat:failures[0].target?.localSeat??null,failures};
          if(firstFailure&&typeof firstFailure==='object')firstFailure.roomGamepadFailure=record.gamepadFailure;
          throw firstFailure??new Error(`Native gamepad failed for ${failures.length} room seat(s)`);
        }
      });
      await step('capture-gameplay',async()=>{
        record.captureNative=await Promise.all(pages.map((page,seat)=>nativeSnapshot(page,game,seat)));
        for(let seat=0;seat<pages.length;seat++){
          await pages[seat].screenshot({path:resolve(directory,`${game}-${players}p-${route}-p${seat+1}.png`)});
          await pages[seat].frameLocator('#gameFrame').locator('canvas').first().screenshot({path:resolve(directory,`${game}-${players}p-${route}-p${seat+1}-canvas.png`)});
        }
      });
      await awaitPendingConsoleSamples();
      record.protocolErrors=roomProtocolErrorEntries(await Promise.all(pages.map(protocolEventSnapshot)),record.stage);
      assertRoomErrors(record,[...record.consoleErrors,...record.protocolErrors]);
      if(disconnect)await step('peer-departure-and-freeze',async()=>{
        const before=await Promise.all(pages.map((page,seat)=>nativeSnapshot(page,game,seat)));
        record.disconnect={before,closedSeats:[]};
        if(game==='th08'){
          await Promise.all(pages.map(page=>page.waitForTimeout(500)));
          record.disconnect.preDisconnectProgress=await Promise.all(pages.map((page,seat)=>nativeSnapshot(page,game,seat)));
          record.disconnect.liveWorldCheck=record.disconnect.preDisconnectProgress.map((sample,seat)=>
            compareTh08LiveWorld(before[seat],sample,{seat}));
          const liveWorldErrors=record.disconnect.liveWorldCheck.flatMap((check,seat)=>
            check.errors.map(error=>`P${seat+1}: ${error}`));
          assert.deepEqual(liveWorldErrors,[],`TH08 must show same-stage active gameplay with enemy, control, and network frames advancing before peer departure`);
        }
        const requestedCheckerMs=Date.now();
        record.disconnect.requestedAt=new Date().toISOString();record.disconnect.requestedCheckerMs=requestedCheckerMs;
        record.disconnect.closedSeats.push({seat:players-1,checkerMs:requestedCheckerMs,reason:'peer-close'});
        await contexts.pop().close();pages.pop();
        const deadline=Date.now()+(route==='relay'?35000:20000);
        while(true){
          const seats=await Promise.all(pages.map(async(page,seat)=>{
            const native=await nativeSnapshot(page,game,seat);
            const worldWait=game==='th08'?th08DisconnectedWaitState(native,{seat}):null;
            return{seat,native,worldWait,
            playerOpen:await page.locator('#player').evaluate(element=>element.classList.contains('open')),
            notificationVisible:await page.locator('#netplayConnectionWindow').isVisible(),
            notification:await page.locator('#netplayConnectionWindow').innerText().catch(()=>''),
            notified:await page.locator('#netplayConnectionReturn').isVisible()};
          }));
          const observationErrors=seats.flatMap(state=>(state.worldWait?.errors??[]).map(error=>`P${state.seat+1}: ${error}`));
          assert.deepEqual(observationErrors,[],`TH08 disconnect observer must remain available for every surviving seat`);
          record.disconnect.waitingSample=seats.map(state=>({seat:state.seat,native:state.native,worldWait:state.worldWait}));
          if(seats.every(state=>state.playerOpen&&state.notificationVisible&&state.notified&&state.notification.trim().length>0&&
              (game!=='th08'||state.worldWait.ready))){
            record.disconnect.afterSeats=seats;record.disconnect.after=seats[0];break;
          }
          if(Date.now()>deadline)throw Error('Departed peer did not notify every surviving client, expose its return button, and freeze the required native world state: '+JSON.stringify(seats));
          await Promise.all(pages.map(page=>page.waitForTimeout(100)));
        }
        for(const state of record.disconnect.afterSeats){
          assert.equal(state.playerOpen,true,`P${state.seat+1} player must remain open until its disconnect return is clicked`);
          assert.equal(state.notificationVisible,true,`P${state.seat+1} disconnect notification must be visible`);
          assert.ok(state.notified&&state.notification.trim(),`P${state.seat+1} disconnect notification must include its return control`);
        }
        const simulationFrameErrors=[];
        const readSimulationFrame=(snapshot,seat,label)=>{
          try{return requireSimulationFrame(snapshot,game,seat,label);}
          catch(error){simulationFrameErrors.push(error.message);return null;}
        };
        const notificationFrames=new Map(game==='th08'?[]:record.disconnect.afterSeats.map(state=>[
          state.seat,readSimulationFrame(state.native,state.seat,'disconnect notification')]));
        await Promise.all(pages.map(page=>page.waitForTimeout(800)));
        record.disconnect.settledSeats=await Promise.all(pages.map(async(page,seat)=>
          nativeSnapshot(page,game,seat).catch(error=>({observationError:String(error.message??error)}))));
        await Promise.all(pages.map(page=>page.waitForTimeout(250)));
        record.disconnect.stableSeats=await Promise.all(pages.map(async(page,seat)=>
          nativeSnapshot(page,game,seat).catch(error=>({observationError:String(error.message??error)}))));
        record.disconnect.settled=record.disconnect.settledSeats[0];
        record.disconnect.frameEvidence=record.disconnect.afterSeats.map(state=>({seat:state.seat,
          before:{simulation:before[state.seat]?.observation?.frames?.simulation??null,
            multiplayer:before[state.seat]?.observation?.frames?.multiplayer??null,network:before[state.seat]?.frame??null,
            worldProgress:before[state.seat]?.observation?.worldProgress??null},
          atNotification:{simulation:state.native?.observation?.frames?.simulation??null,
            multiplayer:state.native?.observation?.frames?.multiplayer??null,network:state.native?.frame??null,
            worldProgress:state.native?.observation?.worldProgress??null},
          settled:{simulation:record.disconnect.settledSeats[state.seat]?.observation?.frames?.simulation??null,
            multiplayer:record.disconnect.settledSeats[state.seat]?.observation?.frames?.multiplayer??null,
            network:record.disconnect.settledSeats[state.seat]?.frame??null,
            worldProgress:record.disconnect.settledSeats[state.seat]?.observation?.worldProgress??null},
          stable:{simulation:record.disconnect.stableSeats[state.seat]?.observation?.frames?.simulation??null,
            multiplayer:record.disconnect.stableSeats[state.seat]?.observation?.frames?.multiplayer??null,
            network:record.disconnect.stableSeats[state.seat]?.frame??null,
            worldProgress:record.disconnect.stableSeats[state.seat]?.observation?.worldProgress??null}}));
        if(game==='th08'){
          record.disconnect.worldFreezeCheck=record.disconnect.afterSeats.map(state=>compareTh08FrozenWorld(
            state.native,record.disconnect.settledSeats[state.seat],record.disconnect.stableSeats[state.seat],{seat:state.seat}));
          for(let index=0;index<record.disconnect.worldFreezeCheck.length;index++)
            simulationFrameErrors.push(...record.disconnect.worldFreezeCheck[index].errors.map(error=>
              `P${record.disconnect.afterSeats[index].seat+1}: ${error}`));
        }else{
          for(const state of record.disconnect.afterSeats){
            const atNotification=notificationFrames.get(state.seat);
            const settled=readSimulationFrame(record.disconnect.settledSeats[state.seat],state.seat,'settled sample');
            const stable=readSimulationFrame(record.disconnect.stableSeats[state.seat],state.seat,'stability sample');
            if(atNotification!==null&&settled!==null&&stable!==null){
              if(settled!==atNotification)simulationFrameErrors.push(`P${state.seat+1} native simulation frame advanced after disconnect notification (${atNotification} -> ${settled})`);
              if(stable!==settled)simulationFrameErrors.push(`P${state.seat+1} native simulation frame did not remain stable after disconnect (${settled} -> ${stable})`);
            }
          }
        }
        record.protocolErrors=roomProtocolErrorEntries(await Promise.all(pages.map(protocolEventSnapshot)),record.stage);
        await awaitPendingConsoleSamples();
        record.disconnect.returns=[];
        for(let seat=0;seat<pages.length;seat++){
          const page=pages[seat];
          await page.waitForFunction(()=>{
            const player=document.querySelector('#player');
            const notice=document.querySelector('#netplayConnectionWindow');
            const button=document.querySelector('#netplayConnectionReturn');
            return player?.classList.contains('open')&&notice&&!notice.hidden&&notice.innerText.trim().length>0&&
              !!(button?.getClientRects().length&&getComputedStyle(button).visibility!=='hidden');
          },null,{timeout:30000});
          const notification=await page.locator('#netplayConnectionWindow').innerText();
          assert.ok(notification.trim(),`P${seat+1} must see the disconnect notification before returning`);
          assert.equal(await page.locator('#netplayConnectionReturn').isVisible(),true,
            `P${seat+1} must have a visible return-to-room control`);
          const requestedCheckerMs=Date.now();
          const closeRecord={seat,checkerMs:null,requestedCheckerMs,reason:'return-to-room'};
          record.disconnect.closedSeats.push(closeRecord);
          await page.evaluate(()=>{
            const button=document.querySelector('#netplayConnectionReturn');
            if(!button)throw Error('Disconnect return button is unavailable for click timestamp capture');
            window.__acceptanceRoomReturnClick=null;
            button.addEventListener('click',()=>{window.__acceptanceRoomReturnClick={checkerMs:Date.now()};},
              {capture:true,once:true});
          });
          await page.locator('#netplayConnectionReturn').click();
          const clickCheckerMs=await page.evaluate(()=>window.__acceptanceRoomReturnClick?.checkerMs??null);
          assert.ok(Number.isFinite(clickCheckerMs)&&clickCheckerMs>=requestedCheckerMs,
            `P${seat+1} return button click event must have a page-clock timestamp`);
          closeRecord.checkerMs=clickCheckerMs;
          await page.waitForFunction(()=>!document.querySelector('#player')?.classList.contains('open'),null,{timeout:30000});
          assert.equal((await page.locator('#mpRoomCode').innerText()).trim(),record.room,`P${seat+1} disconnect return keeps the same room`);
          record.disconnect.returns.push({seat,notification,returnControlClicked:true,returnedToRoom:true});
          await awaitPendingConsoleSamples();
          record.protocolErrors=roomProtocolErrorEntries(await Promise.all(pages.map(protocolEventSnapshot)),record.stage);
        }
        record.disconnect.returnedToRoom=record.disconnect.returns.every(state=>state.returnedToRoom);
        record.disconnect.frameCheck={passed:simulationFrameErrors.length===0,errors:simulationFrameErrors};
        assert.deepEqual(simulationFrameErrors,[],game==='th08'
          ?`TH08 enemy, control, network, and local-player state must remain frozen after every surviving client receives the disconnect notice`
          :`Native simulation frames must be observable and remain frozen for every surviving client`);
      });
      await awaitPendingConsoleSamples();
      if(disconnect)record.protocolErrors=roomProtocolErrorEntries(await Promise.all(pages.map(protocolEventSnapshot)),record.stage);
      assertRoomErrors(record,[...record.consoleErrors,...record.protocolErrors]);
      assert.deepEqual(record.errors,[]);
      const nonSiteAssetFailures=record.httpErrors.filter(failure=>{
        const pathname=new URL(failure.url).pathname;
        return pathname!=='/favicon.ico'&&!pathname.startsWith('/assets/');
      });
      assert.deepEqual(nonSiteAssetFailures,[]);
      if(gamepad){record.stage='native-gamepad-sampling';assert.ok(record.gamepad.every(result=>result.passed),JSON.stringify(record.gamepad.filter(result=>!result.passed)));}
      record.runtimePassed=true;
      record.passed=true;
    }catch(error){record.passed=false;record.error=String(error.stack||error);record.failure=await Promise.all(pages.map(page=>roomSnapshot(page).catch(error=>({error:String(error)}))));
      for(let seat=0;seat<pages.length;seat++)await pages[seat].screenshot({path:resolve(directory,`${game}-failure-p${seat+1}.png`)}).catch(()=>{});
    }finally{recordRoomSiteAssetFailures(report.siteAssets,record.httpErrors);report.siteAssetsPassed=report.siteAssets.passed;
      try{for(const context of contexts)await context.close();}finally{if(browser)await browser.close();}
      await save();console.log(`${game}: ${record.passed?'PASS':'FAIL'} at ${record.stage} ${record.error?.split('\n')[0]??''}`);}
  }
}finally{
  for(const game of games){
    const record=records.get(game),prepared=record.resourcePreparation;
    if(prepared.status!=='ready'){
      record.resourcePostflight={status:'not-run',reason:'resource-preflight-failed'};
      continue;
    }
    try{record.resourcePostflight=await verifyRoomResourcePostflight({target:runtimeTargets.get(game),prepared});}
    catch(error){
      record.resourcePostflight={status:'failed',error:{code:error?.code||'resource-postflight-failed',message:String(error?.message||error)}};
      record.passed=false;record.postflightError=record.resourcePostflight.error.message;
      if(!record.error)record.error=record.postflightError;
    }
    try{
      const source=await verifyRuntimePackage(runtimeTargets.get(game),{repoRoot:root});
      assert.deepEqual(source,sourcePackages.get(game),'Runtime source, build plan, receipts or package changed during room acceptance');
      const published=await verifyPublishedRuntime(base,game,'multiplayer',{expectedPackageSha256:source.packageSha256});
      for(const key of ['source','game','variant','entryUrl','packageSha256','generation','inventorySha256','generationDescriptorSha256'])
        assert.equal(published[key],record.runtimeIdentity?.[key],`Launcher Runtime ${key} changed during room acceptance`);
      record.runtimePostflight={status:'passed',source,published};
    }catch(error){record.runtimePostflight={status:'failed',error:String(error.stack||error)};record.passed=false;}
    await save();
  }
  report.resourcePostflightPassed=report.targets.length===selection.expected.length&&report.targets.every(target=>
    target.resourcePreparation.status==='ready'&&target.resourcePostflight?.status==='passed');
  report.finishedAt=new Date().toISOString();await save();
}
report.runtimePassed=report.targets.length===selection.expected.length&&report.targets.every(target=>target.runtimePassed===true);
report.runtimePostflightPassed=report.targets.length===selection.expected.length&&report.targets.every(target=>target.runtimePostflight?.status==='passed');
report.passed=report.runtimePassed&&report.runtimePostflightPassed&&report.siteAssetsPassed&&report.resourcePostflightPassed;
await save();
if(!report.passed){
  process.exitCode=1;
  if(!report.siteAssetsPassed)console.error(`site-assets: FAIL (${report.siteAssets.assets.filter(asset=>asset.status==='failed').length} preflight failures, ${report.siteAssets.observedFailures.length} observed HTTP failures)`);
  if(!report.resourcePostflightPassed)for(const record of report.targets)if(record.resourcePostflight?.status==='failed')
    console.error(`${record.game}: resource-postflight FAIL ${record.resourcePostflight.error.message}`);
}

});
if(!result.passed){process.exitCode=1;if(result.failure)console.error(result.failure);}
