// Production room/player layout, native Chromium fullscreen and a retained iframe.
import assert from 'node:assert/strict';
import {createServer} from 'node:http';
import {readFile, mkdir} from 'node:fs/promises';
import {resolve, extname, sep} from 'node:path';
import {fileURLToPath} from 'node:url';
import {build} from 'esbuild';
import puppeteer from 'puppeteer-core';
import {findChromiumExecutable} from '../../lib/chromium-executable.mjs';
import {authoredSourcesPlugin} from '../../lib/authored-source-resolution.mjs';

const project = fileURLToPath(new URL('../../', import.meta.url));
const out = resolve(project, '.cache/room-fullscreen-viewport');
await mkdir(out, {recursive: true});
const entry = `
import React,{useState,useLayoutEffect} from 'react';
import{createRoot}from'react-dom/client';
import{RoomWorkspace}from'./app/components/room/RoomWorkspace.tsx';
import{PlayerSurface}from'./app/components/player/PlayerSurface.tsx';
import{LocaleProvider}from'./app/i18n.tsx';
import './app/ui/foundation.css';
import './app/ui/system.css';
const noop=()=>{},subscribe=()=>noop;
const params=new URLSearchParams(location.search);
const state={product:params.get('game')||'th05mp',connected:true,localClientId:'alice',seat:0,ready:true,
 timingChoice:'auto',rollbackEnabled:true,preferredLoadout:0,canStart:false,preparation:{status:'ready',percent:100},
 room:{code:'1925',synced:true,playerCount:3,difficulty:3,phase:params.has('playing')?'running':'lobby',inputDelay:0,startSerial:0,
 seats:['alice','bob','carol'].map((clientId,index)=>({clientId,name:'P'+(index+1),loadout:index,ready:true,controlMode:'normal',resource:{status:'ready',percent:100}})),spectators:[]}};
const service=new Proxy({subscribe,getSnapshot:()=>state,timingRecommendation:()=>({inputDelay:0})},{get:(o,k)=>o[k]??noop});
const network={subscribe,getSnapshot:()=>0,metric:()=>({state:'connected',rtt:38,jitter:2}),loss:()=>({state:'idle'}),lossReady:()=>false,testLoss:noop,capabilities:()=>({supported:true,rtcAvailable:true,turnConfigured:true}),retry:noop};
function App(){
 const[host,setHost]=useState(null),[fullscreen,setFullscreen]=useState(false);
 useLayoutEffect(()=>{const changed=()=>setFullscreen(!!document.fullscreenElement);document.addEventListener('fullscreenchange',changed);return()=>document.removeEventListener('fullscreenchange',changed);},[]);
 return <LocaleProvider locale="zh-CN">
  <PlayerSurface open roomWorkspace={!fullscreen} editing={false} onElement={setHost}/>
  {host&&<RoomWorkspace visible={!fullscreen} service={service} network={network} context={{launched:params.has('playing'),previewReady:true,th09NetworkOverlayOpen:false,launchStage:null,touchEnabled:false,touchMovementMode:'joystick'}} assetUrl={p=>'/'+p} workspaceHost={host} workspaceStage={host.querySelector('.player-stage')} onLeave={noop} onCopyRoomCode={noop} onGuide={noop} onFullscreen={()=>host.requestFullscreen()} onOpenSettings={noop} settingsOpen={false} panel={null} onOpenPanel={noop} onClosePanel={noop}/>}
 </LocaleProvider>;
}
createRoot(document.getElementById('root')).render(<App/>);
`;
const result = await build({absWorkingDir: project, stdin: {contents: entry, loader: 'tsx', resolveDir: project}, bundle: true, format: 'esm', jsx: 'automatic', write: false, outdir: out, external: ['/assets/*','assets/*'], loader: {'.webp': 'dataurl'}, plugins: [authoredSourcesPlugin(project)]});
const js = result.outputFiles.find(file => file.path.endsWith('.js')).text;
const css = result.outputFiles.find(file => file.path.endsWith('.css')).text;
const publicRoot = resolve(project, 'public');
const server = createServer(async (req, res) => {
  try {
    const path = new URL(req.url, 'http://localhost').pathname;
    if (path === '/') {res.setHeader('content-type', 'text/html; charset=utf-8'); res.end('<!doctype html><meta charset="utf-8"><meta name="viewport" content="width=device-width,initial-scale=1"><style>@layer properties,foundation,theme,base,components,utilities;</style><link rel="stylesheet" href="/fixture.css"><div id="root"></div><script type="module" src="/fixture.js"></script>'); return;}
    if (path === '/fixture.js' || path === '/fixture.css') {res.setHeader('content-type', path.endsWith('js') ? 'text/javascript' : 'text/css'); res.end(path.endsWith('js') ? js : css); return;}
    const file = resolve(publicRoot, '.' + path);
    if (!file.startsWith(publicRoot + sep)) throw Error('asset path');
    res.setHeader('content-type', ({'.css': 'text/css', '.svg': 'image/svg+xml', '.woff2': 'font/woff2'})[extname(path)] || 'application/octet-stream');
    res.end(await readFile(file));
  } catch {res.writeHead(404).end();}
});
await new Promise(done => server.listen(0, '127.0.0.1', done));
let browser;
try {
  browser = await puppeteer.launch({headless: true, executablePath: await findChromiumExecutable()});
  const page = await browser.newPage(), errors = [];
  page.on('pageerror', error => {errors.push(error.message); console.error(error.message);});
  await page.emulateMediaFeatures([{name: 'prefers-reduced-motion', value: 'reduce'}]);
  const origin = `http://127.0.0.1:${server.address().port}`;
  const visible = () => page.waitForFunction(() => {
    const viewport=document.querySelector('#gameViewport'),frame=document.querySelector('#gameFrame');
    return viewport?.getBoundingClientRect().height>100&&frame?.getBoundingClientRect().height>100;
  });
  for (const game of ['th05mp','th06mp']) for (const playing of [false,true]) for (const [width,height] of [[1440,900],[2468,1391],[390,844],[844,390]]) {
    await page.setViewport({width,height});
    await page.goto(`${origin}/?game=${game}${playing?'&playing=1':''}`);
    await page.waitForSelector('.room-workspace-fullscreen');
    await visible();
    await page.evaluate(()=>{
      window.retainedFrame=document.querySelector('#gameFrame');
      window.retainedDocument=window.retainedFrame.contentDocument;
      window.retainedDocument.body.innerHTML='<canvas width="640" height="480" style="width:100%;height:100%"></canvas>';
      const canvas=window.retainedDocument.querySelector('canvas');canvas.getContext('2d').fillRect(0,0,640,480);window.retainedCanvas=canvas;
    });
    for(let cycle=0;cycle<3;cycle++) {
      await page.click('.room-workspace-fullscreen');
      await page.waitForFunction(()=>!!document.fullscreenElement&&!document.querySelector('#player').classList.contains('room-workspace'));
      await page.evaluate(()=>document.exitFullscreen());
      await page.waitForSelector('#player.room-workspace');
      await visible();
      assert(await page.evaluate(()=>document.querySelector('#gameFrame')===window.retainedFrame&&window.retainedFrame.contentDocument===window.retainedDocument&&window.retainedDocument.querySelector('canvas')===window.retainedCanvas),'fullscreen exit retains the iframe, document and canvas');
    }
    // A temporarily boxless stage must not poison the image-height cap.
    await page.waitForFunction(()=>parseFloat(document.querySelector('#gameViewport').style.getPropertyValue('--workspace-game-height'))>100);
    const before=await page.$eval('#gameViewport',node=>node.style.getPropertyValue('--workspace-game-height'));
    await page.evaluate(async()=>{
      document.querySelector('.player-stage').style.setProperty('display','contents','important');
      document.dispatchEvent(new Event('webkitfullscreenchange'));
      await new Promise(done=>requestAnimationFrame(()=>requestAnimationFrame(done)));
    });
    assert.equal(await page.$eval('#gameViewport',node=>node.style.getPropertyValue('--workspace-game-height')),before,'transient zero width preserves the last usable image height');
    await page.evaluate(()=>{document.querySelector('.player-stage').style.removeProperty('display');document.dispatchEvent(new Event('webkitfullscreenchange'));});
    await visible();
    await page.setViewport({width:height,height:width});
    await visible();
    await page.screenshot({path:resolve(out,`${game}-${playing?'playing':'preview'}-${width}.png`)});
  }
  assert.deepEqual(errors, []);
  console.log('PASS room preview/gameplay fullscreen exits, transient zero width, rotation and retained iframe/canvas (48 fullscreen cycles)');
} finally {await browser?.close(); server.close();}
