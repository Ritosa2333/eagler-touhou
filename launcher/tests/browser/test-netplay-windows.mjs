// Production connection/calibration/decision windows in the permanent Player.
// Explicit network events exercise layout and controls; no native game is faked
// as transport acceptance. Room/native lifecycle has separate mounted tests.
import assert from 'node:assert/strict';
import {createServer} from 'node:http';
import {readFile, mkdir, writeFile} from 'node:fs/promises';
import {resolve, extname, sep} from 'node:path';
import {build} from 'esbuild';
import {loadPlaywright} from '../../../tools/browser/playwright.mjs';
import {authoredSourcesPlugin} from '../react-main/authored-sources.mjs';

const project = resolve(import.meta.dirname, '../..');
const out = resolve(project, '.cache/netplay-windows');
await mkdir(out, {recursive:true});
const bundle = await build({stdin:{resolveDir:project,loader:'tsx',contents:`
  import React,{useState,useSyncExternalStore} from 'react';
  import {createRoot} from 'react-dom/client';
  import {LocaleProvider,translate} from './app/i18n.tsx';
  import {PlayerSurface} from './app/components/player/PlayerSurface.tsx';
  import {NetplayConnectionWindow} from './app/components/NetplayConnectionWindow.tsx';
  import {NetplayCalibrationReport} from './app/components/NetplayCalibrationReport.tsx';
  import {FullscreenTransient} from './app/components/FullscreenTransient.tsx';
  import {ConfirmationDialog} from './app/components/ConfirmationDialog.tsx';
  import {createNetplayCalibration} from './app/models/netplay-calibration.ts';
  import {createDecisionStore} from './app/models/decisions.ts';
  import {createGameUpdatePrompt} from './app/models/game-update-prompt.ts';
  import {GameDataImportWindows} from './app/components/player/GameDataImportWindows.tsx';
  import {createGameDataImportModel} from './app/models/game-data-import.ts';
  import {StartupError} from './app/components/feedback/StartupError.tsx';
  import {createStartupErrorModel} from './app/models/startup-error.ts';
  import './app/components/room/room-workspace.css';
  const calibration=createNetplayCalibration({epoch:1,timers:window,userAgent:navigator.userAgent});
  const decisions=createDecisionStore(), context={product:'th06mp',gameVersion:'original',epoch:1};
  let restarts=0,returns=0,setPhase;
  const t=(k,p)=>translate('zh-CN',k,p);
  const gameData=createGameDataImportModel({translate:t,acquisition:{importPackage:async()=>{throw Error('No file import in layout test');}},
    getContext:()=>({product:'th06mp',roomCode:'1234',runtimeReady:false,runtimeEpoch:null,launched:false,playerOpen:false,replayViewer:false,importServer:false,fallback:{url:'https://downloads.invalid/package.zip',hint:'touhou'}}),
    ports:{pickFile:async()=>null,closePlayer:async()=>true,resetUnlaunchedRuntime(){},openPlayer(){},launchConfigured:async()=>{},feedback:{status(){},playerStatus(){},toast(){}},beginBlockingDownload:()=>({finish(){},cancel:async()=>{}})}});
  const startupError=createStartupErrorModel({translate:t,launched:()=>false,copyText:async()=>true,toast(){}});
  const updates=createGameUpdatePrompt({decisions,translate:(k,p)=>translate('zh-CN',k,p),current:()=>context,busy:()=>false,restart:async()=>{restarts++;},onError:e=>{throw e;}});
  const result={phase:'ready',automatic:true,adonisMode:2,inputDelay:1,fullDelay:3,predictionReserve:2,rttP95Us:100000,samples:120,lost:0,route:'rtc',calibration:{game:'th10mp',build:'layout-fixture',completedAt:100,localPlayer:0,players:[0,1].map(player=>({player,p95Us:100000,samples:120,lost:0,minUs:1000,maxUs:150000,meanUs:50000}))}};
  function App(){
    const[phase,set]=useState('connecting');setPhase=set;
    const request=useSyncExternalStore(decisions.subscribe,decisions.getSnapshot);
    const network={connection:{hidden:phase==='hidden'||phase==='ready',title:phase==='reconnecting'?'正在重新连接':'正在建立联机连接',summary:'等待其他玩家连接，连接完成后进入游戏。',reconnecting:phase==='reconnecting',peerRows:[{player:1,status:phase==='reconnecting'?'连接中断':'正在连接'}]},calibrationEligible:true,returnToRoom:true};
    return <LocaleProvider locale="zh-CN"><PlayerSurface open roomWorkspace={!new URLSearchParams(location.search).has('game')} editing={false} onElement={()=>{}}>
      <button id="fullscreen" style={{position:'fixed',left:12,bottom:12,zIndex:60}} onClick={()=>document.querySelector('#player').requestFullscreen()}>全屏</button>
      <NetplayConnectionWindow network={network} calibration={calibration} english={false} onReturn={()=>{returns++;set('hidden');calibration.dismiss();}}/>
      <NetplayCalibrationReport model={calibration} english={false} currentNetwork={()=>'RTC 38 ms'}/>
    </PlayerSurface><FullscreenTransient><GameDataImportWindows model={gameData} getRuntimeReady={()=>false}/><StartupError model={startupError}/><ConfirmationDialog open={!!request} {...request} onConfirm={()=>decisions.resolve('confirm')} onCancel={()=>decisions.resolve('cancel')}/></FullscreenTransient></LocaleProvider>;
  }
  window.fixture={phase:p=>{calibration.reset(1);setPhase(p);if(p==='measuring')calibration.record({phase:'measuring',probes:40,replies:38},1);if(p==='ready')calibration.record(result,1);},import:()=>gameData.beginManual({kind:'install-only',reason:'安装包缺少运行所需的共享字体，请更新游戏资源'}),error:()=>startupError.show(Error('资源准备失败')),update:()=>updates.ready(context),returns:()=>returns,restarts:()=>restarts};
  createRoot(document.getElementById('app')).render(<App/>);
`},bundle:true,write:false,outdir:out,format:'esm',platform:'browser',jsx:'automatic',plugins:[authoredSourcesPlugin(project)],define:{'process.env.NODE_ENV':'"production"'}});
const js=bundle.outputFiles.find(f=>f.path.endsWith('.js')).contents;
const css=bundle.outputFiles.find(f=>f.path.endsWith('.css')).contents;
const publicRoot=resolve(project,'public');
const server=createServer(async(req,res)=>{try{
  const path=new URL(req.url,'http://localhost').pathname;
  if(path==='/'){res.setHeader('content-type','text/html');res.end('<!doctype html><meta charset="utf-8"><meta name="viewport" content="width=device-width,initial-scale=1"><link rel="stylesheet" href="/styles.css"><link rel="stylesheet" href="/fixture.css"><div id="app"></div><script type="module" src="/fixture.js"></script>');return;}
  if(path==='/fixture.js'||path==='/fixture.css'){res.setHeader('content-type',path.endsWith('.js')?'text/javascript':'text/css');res.end(path.endsWith('.js')?js:css);return;}
  const file=resolve(publicRoot,'.'+path);if(!file.startsWith(publicRoot+sep))throw Error('asset path');
  res.setHeader('content-type',({'.css':'text/css','.svg':'image/svg+xml','.woff2':'font/woff2'})[extname(path)]||'application/octet-stream');res.end(await readFile(file));
}catch{res.writeHead(404).end();}});
await new Promise(r=>server.listen(0,'127.0.0.1',r));
const playwright=await loadPlaywright(), browser=await playwright.chromium.launch({headless:true});
const report=[];
async function visibleControl(page,selector){
  await page.locator(selector).scrollIntoViewIfNeeded();
  const box=await page.locator(selector).boundingBox();assert.ok(box,selector+' visible');
  const {width,height}=page.viewportSize();assert.ok(box.x>=0&&box.y>=0&&box.x+box.width<=width+1&&box.y+box.height<=height+1,selector+' within viewport '+JSON.stringify({viewport:page.viewportSize(),box}));
  assert.ok(await page.locator(selector).evaluate(node=>{const r=node.getBoundingClientRect();return node.contains(document.elementFromPoint(r.x+r.width/2,r.y+r.height/2));}),selector+' receives pointer');
  return box;
}
try{
  async function resourceWindows(page,fullscreen){
    await page.evaluate(()=>fixture.import());
    await page.locator('#gameDataImportWindow').waitFor({state:'visible'});
    await visibleControl(page,'#transferImport');await visibleControl(page,'#transferDownload');
    assert.equal(await page.locator('#gameDataImportWindow').evaluate(n=>!!n.closest('#player')),fullscreen);
    await page.locator('#transferDownload').click();
    await visibleControl(page,'#gameDataFallbackUrl');await visibleControl(page,'#gameDataLinkClose');
    await page.locator('#gameDataLinkClose').click();await page.locator('#gameDataImportClose').click();
    await page.locator('#gameDataImportWindow').waitFor({state:'hidden'});
    await page.evaluate(()=>fixture.error());await visibleControl(page,'#startupErrorCopy');await visibleControl(page,'#startupErrorClose');
    await page.locator('#startupErrorClose').click();
    assert.ok(await page.locator('#fullscreen').evaluate(n=>{const r=n.getBoundingClientRect();return n.contains(document.elementFromPoint(r.x+r.width/2,r.y+r.height/2));}),'empty transient host permits room interaction');
  }
  for(const viewport of [{width:1440,height:900},{width:390,height:844},{width:844,height:390}]){
    const page=await browser.newPage({viewport,reducedMotion:'reduce'}),errors=[];page.on('pageerror',e=>errors.push(e.message));
    await page.goto('http://127.0.0.1:'+server.address().port);await page.waitForFunction(()=>window.fixture);
    for(const phase of ['connecting','measuring','ready','reconnecting']){
      await page.evaluate(p=>fixture.phase(p),phase);await page.locator('#netplayConnectionWindow').waitFor({state:'visible'});
      const selector=phase==='ready'?'#netplayCalibrationDismiss':'#netplayConnectionReturn';
      await visibleControl(page,selector);
      if(viewport.width!==844)await page.screenshot({path:resolve(out,phase+'-'+viewport.width+'.png')});
      await page.locator(selector).click();
      await page.locator('#netplayConnectionWindow').waitFor({state:'hidden'});
    }
    assert.equal(await page.evaluate(()=>fixture.returns()),3);
    await resourceWindows(page,false);
    await page.locator('#fullscreen').click();await page.waitForFunction(()=>!!document.fullscreenElement);
    await resourceWindows(page,true);
    await page.evaluate(()=>fixture.phase('ready'));
    await visibleControl(page,'#netplayCalibrationDismiss');
    await page.locator('#netplayCalibrationDismiss').click();
    await page.locator('#netplayCalibrationReport').click();
    await visibleControl(page,'#netplayCalibrationCopy');
    assert.equal(await page.locator('#netplayCalibrationDialog').evaluate(n=>n.matches(':modal')),true);
    await page.keyboard.press('Escape');await page.locator('#netplayCalibrationDialog').waitFor({state:'hidden'});
    await page.evaluate(()=>fixture.phase('hidden'));await page.evaluate(()=>fixture.update());
    await page.locator('#decisionDialog').waitFor({state:'visible'});
    await visibleControl(page,'#decisionConfirm');await visibleControl(page,'#decisionCancel');
    assert.equal(await page.locator('#decisionDialog').evaluate(n=>!!n.closest('#player')),true,'decision follows fullscreen Player');
    if(viewport.width!==844)await page.screenshot({path:resolve(out,'update-fullscreen-'+viewport.width+'.png')});
    await page.locator('#decisionCancel').click();await page.locator('#decisionDialog').waitFor({state:'hidden'});
    assert.equal(await page.evaluate(()=>fixture.restarts()),0);
    assert.deepEqual(errors,[]);report.push({viewport,passed:true});await page.close();
  }
  await writeFile(resolve(out,'report.json'),JSON.stringify({passed:true,report},null,2));console.log(JSON.stringify({passed:true,report}));
}finally{await browser.close();server.closeAllConnections();await new Promise(r=>server.close(r));}
