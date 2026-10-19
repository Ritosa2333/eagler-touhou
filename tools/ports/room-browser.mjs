import assert from 'node:assert/strict';
import {mkdir,readFile,writeFile,rename} from 'node:fs/promises';
import {resolve} from 'node:path';
import {createHash} from 'node:crypto';
import {execFileSync} from 'node:child_process';
import {launchBrowser} from '../browser/launch.mjs';
import {PRODUCT_GAMES} from '../../common/contracts/product-catalog.mjs';
import {gamepadProbeBundle,prepareVirtualGamepad,probeNativeGamepad} from './gamepad-probe.mjs';

const option=(name,fallback)=>process.argv.find(value=>value.startsWith(`--${name}=`))?.slice(name.length+3)??fallback;
const base=option('url'),out=option('output'),route=option('route','rtc'),players=Number(option('players','2'));
const disconnect=process.argv.includes('--disconnect');
const gamepad=process.argv.includes('--gamepad'),gamepadBundle=gamepad?await gamepadProbeBundle():null;
assert.ok(base&&out,'Required: --url=local-launcher --output=private-directory');
assert.ok(['rtc','relay'].includes(route));assert.ok([2,3].includes(players));
const games=option('game',Object.keys(PRODUCT_GAMES).filter(game=>PRODUCT_GAMES[game].multiplayer).join(',')).split(',');
const directory=resolve(out);await mkdir(directory,{recursive:true});
const packages=option('packages')?JSON.parse(await readFile(resolve(option('packages')),'utf8')):{};
const report={schema:'eagler-touhou/room-browser/1',startedAt:new Date().toISOString(),url:base,route,players,
  scope:'Real Launcher room controls, actual production Runtime and private installed DATA; desktop browser session acceptance',targets:[]};
report.disconnectRequested=disconnect;
report.commit=execFileSync('git',['rev-parse','HEAD'],{encoding:'utf8',windowsHide:true}).trim();
report.harnessSha256=createHash('sha256').update(await readFile(new URL(import.meta.url))).digest('hex');
if(gamepad){report.gamepadProbeSha256=createHash('sha256').update(await readFile(new URL('./gamepad-probe.mjs',import.meta.url))).digest('hex');report.gamepadBundleSha256=createHash('sha256').update(gamepadBundle).digest('hex');await writeFile(resolve(directory,'gamepad-bundle.js'),gamepadBundle);await writeFile(resolve(directory,'gamepad-probe.mjs'),await readFile(new URL('./gamepad-probe.mjs',import.meta.url)));}
await writeFile(resolve(directory,'harness.mjs'),await readFile(new URL(import.meta.url)));
const save=async()=>{
  const pending=resolve(directory,'report.pending.json');
  await writeFile(pending,JSON.stringify(report,null,2)+'\n');
  await rename(pending,resolve(directory,'report.json'));
};
async function dismiss(page){
  await page.waitForFunction(()=>localStorage.getItem('eagler-touhou-first-use-notice-seen-v1')==='1'||document.querySelector('#firstUseNoticeDialog')?.open===true,null,{timeout:30000});
  if(await page.locator('#firstUseNoticeDialog[open]').count()){
    await page.locator('#firstUseNoticeClose').click();await page.waitForFunction(()=>!document.querySelector('#firstUseNoticeDialog')?.open);
  }
}
async function roomSnapshot(page){return page.evaluate(()=>({body:document.body.innerText,
  ready:document.querySelector('#mpReady')?.getAttribute('aria-pressed'),
  startDisabled:document.querySelector('#mpStartGame')?.disabled,
  sessions:Object.fromEntries(Object.entries(sessionStorage).filter(([key])=>key.includes('room'))),events:window.acceptanceEvents??[]}));}
async function nativeSnapshot(page){return page.evaluate(()=>{
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
    events:window.acceptanceEvents??[]};
});}
try{
  for(const game of games){
    assert.ok(PRODUCT_GAMES[game]?.multiplayer,game);
    if(!PRODUCT_GAMES[game].multiplayer.playerCounts.includes(players))continue;
    const browser=await launchBrowser({args:['--enable-unsafe-swiftshader','--autoplay-policy=no-user-gesture-required',
      '--disable-background-timer-throttling','--disable-renderer-backgrounding']});report.browser=browser.version();
    const contexts=[],pages=[],record={game,players,route,steps:[],errors:[],consoleErrors:[],httpErrors:[]};report.targets.push(record);
    const step=async(name,operation)=>{record.stage=name;record.steps.push({name,at:new Date().toISOString()});await save();return operation();};
    try{
      for(let seat=0;seat<players;seat++){
        const context=await browser.newContext({viewport:{width:1280,height:900}});contexts.push(context);
        if(gamepad)await prepareVirtualGamepad(context,gamepadBundle);
        if(route==='relay')await context.addInitScript(()=>Object.defineProperty(window,'RTCPeerConnection',{value:undefined,configurable:true}));
        await context.addInitScript(()=>{window.acceptanceEvents=[];addEventListener('message',event=>{
          if(event.source===document.querySelector('#gameFrame')?.contentWindow&&event.data?.protocol==='eagler-touhou/1')acceptanceEvents.push(event.data);
        });});
        const page=await context.newPage();pages.push(page);
        page.on('pageerror',error=>record.errors.push({seat,error:String(error)}));
        page.on('console',message=>{if(message.type()==='error')record.consoleErrors.push({seat,stage:record.stage,text:message.text()});});
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
          await page.locator('#submitRoom').click();await page.locator('#mpRoomCode').waitFor({state:'visible'});
          const me=page.locator(`[data-mp-seat-me="${seat}"]`);
          await page.waitForFunction(seat=>document.querySelector(`[data-mp-seat-me="${seat}"]`)?.hidden===false||
            document.querySelector(`[data-mp-seat-drop="${seat}"] button`)?.disabled===false,seat);
          if(!(await me.evaluate(element=>element.hidden===false)))await page.locator(`[data-mp-seat-drop="${seat}"] button`).click();
          await page.waitForFunction(seat=>document.querySelector(`[data-mp-seat-me="${seat}"]`)?.hidden===false,seat);
          record.room=(await page.locator('#mpRoomCode').innerText()).trim();
          if(await page.locator('#mpMusicSelect').count())await page.locator('#mpMusicSelect').selectOption('none',{force:true});
          if(packages[game]){
            await page.locator('#mpSettingsRoomDrawerToggle').click();
            await page.locator('#mpGamePackageImport').click();
            await page.locator('#gameDataImportWindow').waitFor({state:'visible'});
            const chooser=page.waitForEvent('filechooser');await page.locator('#transferImport').click();
            await(await chooser).setFiles(packages[game]);
            await page.locator('#gameDataImportWindow').waitFor({state:'hidden',timeout:120000});
            await page.locator('#mpGamePackageImport').focus();await page.keyboard.press('Escape');
            await page.locator('#mpSettingsRoomDrawer').waitFor({state:'hidden'});
            await page.waitForFunction(()=>document.querySelector('#mpReady')?.disabled===false||
              document.querySelector('#mpRoomResourceRetry')?.hidden===false,null,{timeout:120000});
            if(await page.locator('#mpRoomResourceRetry').isVisible()){
              record.steps.push({name:`P${seat+1}-resume-after-import`,at:new Date().toISOString()});
              await page.locator('#mpRoomResourceRetry').click();
            }
          }
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
        while(true){record.native=await Promise.all(pages.map(nativeSnapshot));
          if(record.native.every(state=>state?.active&&state.frame>=180))break;
          if(record.native.some(state=>state?.failed||state?.error))throw Error('Native session failed: '+JSON.stringify(record.native));
          if(Date.now()>deadline)throw Error('Native session did not reach frame 180: '+JSON.stringify(record.native));
          await pages[0].waitForTimeout(200);
        }
        for(let seat=0;seat<players;seat++){
          const state=record.native[seat];assert.equal(state.options.player,seat);assert.equal(state.options.players,players);assert.equal(state.options.mode,'lan');assert.equal(state.route,route);
          if(game==='th09')assert.equal(state.state?.session?.[0],1,'TH09 must enter an actual versus match');
          await pages[seat].screenshot({path:resolve(directory,`${game}-${players}p-${route}-p${seat+1}.png`)});
          await pages[seat].frameLocator('#gameFrame').locator('canvas').first().screenshot({path:resolve(directory,`${game}-${players}p-${route}-p${seat+1}-canvas.png`)});
        }
      });
      if(gamepad)await step('native-gamepad-sampling',async()=>{
        record.gamepad=[];
        for(const page of pages){const frame=await(await page.locator('#gameFrame').elementHandle()).contentFrame();
          assert.ok(frame,'Actual Runtime frame required');
          try{record.gamepad.push({passed:true,...await probeNativeGamepad(page,frame)});}catch(error){record.gamepad.push({passed:false,error:String(error.stack||error)});}}
      });
      if(disconnect)await step('peer-departure-and-freeze',async()=>{
        record.disconnect={before:await Promise.all(pages.map(nativeSnapshot)),requestedAt:new Date().toISOString()};
        await contexts.pop().close();pages.pop();
        const deadline=Date.now()+(route==='relay'?35000:20000);
        while(true){
          const current=await nativeSnapshot(pages[0]);
          const playerOpen=await pages[0].locator('#player').evaluate(element=>element.classList.contains('open'));
          const notified=await pages[0].locator('#netplayConnectionReturn').isVisible();
          if(current?.failed||current?.error||!playerOpen||notified){record.disconnect.after={native:current,playerOpen,notified};break;}
          if(Date.now()>deadline)throw Error('Departed peer did not reach the remaining client');
          await pages[0].waitForTimeout(100);
        }
        await pages[0].waitForTimeout(800);
        record.disconnect.settled=await nativeSnapshot(pages[0]);
        if(record.disconnect.after.playerOpen&&record.disconnect.after.native?.frame>=0)
          assert.equal(record.disconnect.settled?.frame,record.disconnect.after.native.frame,'Native world must stop advancing after disconnect');
        if(await pages[0].locator('#netplayConnectionReturn').isVisible()){
          record.disconnect.notification=await pages[0].locator('#netplayConnectionWindow').innerText();
          await pages[0].locator('#netplayConnectionReturn').click();
          await pages[0].waitForFunction(()=>!document.querySelector('#player')?.classList.contains('open'),null,{timeout:30000});
          assert.equal((await pages[0].locator('#mpRoomCode').innerText()).trim(),record.room,'Disconnect return keeps the same room');
          record.disconnect.returnedToRoom=true;
        }
      });
      assert.deepEqual(record.errors,[]);
      const nonCardFailures=record.httpErrors.filter(failure=>!/\/assets\/th\d+-card\.(?:webp|png|jpe?g)$/.test(new URL(failure.url).pathname));
      assert.deepEqual(nonCardFailures,[]);
      if(gamepad){record.stage='native-gamepad-sampling';assert.ok(record.gamepad.every(result=>result.passed),JSON.stringify(record.gamepad.filter(result=>!result.passed)));}
      record.runtimePassed=true;
      record.stage='site-assets';assert.deepEqual(record.httpErrors,[]);record.passed=true;
    }catch(error){record.passed=false;record.error=String(error.stack||error);record.failure=await Promise.all(pages.map(page=>roomSnapshot(page).catch(error=>({error:String(error)}))));
      for(let seat=0;seat<pages.length;seat++)await pages[seat].screenshot({path:resolve(directory,`${game}-failure-p${seat+1}.png`)}).catch(()=>{});
    }finally{try{for(const context of contexts)await context.close();}finally{await browser.close();}await save();console.log(`${game}: ${record.passed?'PASS':record.runtimePassed?'NATIVE PASS / SITE ASSETS FAIL':'FAIL'} at ${record.stage} ${record.error?.split('\n')[0]??''}`);}
  }
}finally{report.finishedAt=new Date().toISOString();await save();}
if(report.targets.some(target=>!target.passed))process.exitCode=1;
