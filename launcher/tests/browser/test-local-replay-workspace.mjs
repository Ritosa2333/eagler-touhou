// Real React workspace + shared model + TH11 WASM. The fixture replaces the
// outer site/acquisition shell only; metadata, stages and playback are native.
import assert from 'node:assert/strict';
import {createServer} from 'node:http';
import {readFileSync,readdirSync,existsSync,mkdirSync,writeFileSync} from 'node:fs';
import {resolve,extname} from 'node:path';
import {fileURLToPath,pathToFileURL} from 'node:url';
import {createHash} from 'node:crypto';
import {execFileSync} from 'node:child_process';
import {build} from 'esbuild';
// Use the same installed Playwright driver as the native Python checks.
const driver=process.env.EAGLER_PLAYWRIGHT_MODULE||execFileSync('python',['-c',"import pathlib,playwright; print(pathlib.Path(playwright.__file__).parent/'driver/package/index.mjs')"],{encoding:'utf8',windowsHide:true}).trim();
const {chromium}=await import(pathToFileURL(driver));
import {authoredSourcesPlugin} from '../react-main/authored-sources.mjs';
const project=resolve(fileURLToPath(new URL('../../',import.meta.url))),workspace=resolve(project,'../..');
const nativeRoot=resolve(workspace,'th11-eagler'),out=resolve(project,'artifacts/replay-workspace/ui');mkdirSync(out,{recursive:true});
const nativeBuild=resolve(nativeRoot,'web/artifacts/sdl3'),fonts=resolve(nativeRoot,'build-eagler/fonts');
const dataDirectory=readdirSync(nativeRoot).find(name=>existsSync(resolve(nativeRoot,name,'th11.dat')));
assert.ok(dataDirectory,'TH11 game data is required');
const fixtures=readdirSync(resolve(nativeRoot,'web/reference/assets')).filter(name=>/^demo.*\.rpy$/.test(name)).sort().map((name,i)=>({path:`replay/th11_ud${String(i).padStart(4,'0')}.rpy`,file:resolve(nativeRoot,'web/reference/assets',name)}));
const resources=new Map([['/native.mjs',resolve(nativeBuild,'th11-sdl.mjs')],['/native.wasm',resolve(nativeBuild,'th11-sdl.wasm')],['/game.dat',resolve(nativeRoot,dataDirectory,'th11.dat')],
 ...readdirSync(fonts).map(name=>['/fonts/'+name,resolve(fonts,name)]),...fixtures.map(f=>['/'+f.path,f.file]),
 ...['local-replay.mjs','local-replay-native.mjs'].map(name=>['/'+name,resolve(workspace,'eagler-common/browser',name)])]);
const entry=`
import React,{useEffect,useSyncExternalStore} from 'react';import{createRoot}from'react-dom/client';
import{createMemoryRouter,RouterProvider}from'react-router';
import{SurfaceNavigationProvider,useSurfaceNavigation}from'./app/navigation/surface-navigation';
import{PlayerSurface}from'./app/components/player/PlayerSurface';import{ReplayWorkspace}from'./app/components/player/ReplayWorkspace';
import{createReplayModel}from'./app/models/replays';
const rows=await Promise.all(${JSON.stringify(fixtures.map(f=>f.path))}.map(async path=>({path,bytes:new Uint8Array(await(await fetch('/'+path)).arrayBuffer()),size:0})));
let frame,serial=0,readyResolve;const ready=new Promise(r=>readyResolve=r),pending=new Map(),listeners=new Set();
let snapshot={locale:'zh-CN',playerOpen:true,replayViewer:true,runtime:{runtimeInfo:{}}};window.commands=[];window.errors=[];
function publish(patch){snapshot={...snapshot,...patch};for(const fn of listeners)fn();}
const send=(command,payload={})=>new Promise((ok,no)=>{const request=String(++serial);pending.set(request,{ok,no});window.commands.push({command,...payload});frame.contentWindow.postMessage({protocol:'eagler-touhou/1',game:'th11',epoch:1,command,request,...payload},location.origin);});
addEventListener('message',event=>{if(event.source!==frame?.contentWindow||event.origin!==location.origin)return;const m=event.data;if(m.event==='ready'){readyResolve();return;}if(m.event==='runtime-info'){window.playback=m.playback;publish({runtime:{runtimeInfo:{playback:m.playback}}});return;}if(m.event==='error'){window.errors.push(m.error);return;}if(m.request){const p=pending.get(m.request);if(p){pending.delete(m.request);m.ok?p.ok(m):p.no(Error(m.error));}}});
const runtime={getSnapshot:()=>({game:'th11',epoch:1,launched:true,ready:true,runtimeVariant:'normal'}),send,withFileSession:async(_game,fn)=>fn({epoch:1,sync:async()=>{},send})};
const model=createReplayModel({runtime,storage:{read:async(_product,selection={})=>rows.filter(row=>!selection.path||selection.path===row.path).map(row=>({...row,size:row.bytes.length}))},
 prepareViewer:()=>ready,prepareFiles:()=>ready,releasePrepared:async()=>{},afterPaint:()=>new Promise(requestAnimationFrame),afterClose(){},
 inspect:async(_product,path)=>(await send('playback',{action:'inspect',path})).description,
 watch:async(_product,path,stage)=>{await send('playback',{action:'open',path,stage});},translate:key=>key,prompt:()=>null,confirm:async()=>false,toast:message=>window.errors.push(message),download(){}});
const session={baseUrl:location.origin+'/',subscribe:fn=>{listeners.add(fn);return()=>listeners.delete(fn);},getSnapshot:()=>snapshot,getReplays:()=>model,getRuntime:()=>runtime,
 nativeTouch:{isFullscreen:()=>false},reportError:error=>window.errors.push(String(error)),requestRuntimeClose:async()=>{publish({playerOpen:false});return true;},togglePlayerFullscreen:async()=>{},chooseReplayFile:async()=>{},importFile:async()=>{}};
function Body(){const navigation=useSurfaceNavigation(),app=useSyncExternalStore(session.subscribe,session.getSnapshot);
 return <><button id="openReplay" style={{position:'fixed',top:0,left:0,zIndex:99}} onClick={event=>{event.currentTarget.hidden=true;navigation.openInfoDialog('replayDialog',{replace:true});void model.open('th11');}}>打开录像</button><PlayerSurface open={app.playerOpen} editing={false} replayWorkspace onElement={()=>{}} onFrame={element=>{if(element&&frame!==element){frame=element;element.src='/engine.html';}}}><ReplayWorkspace session={session}/></PlayerSurface></>;}
function Fixture(){return <SurfaceNavigationProvider dirty={false} isEditing={false} onDiscard={()=>{}} playerOpen><Body/></SurfaceNavigationProvider>;}
const router=createMemoryRouter([{path:'*',element:<Fixture/>}],{initialEntries:['/?game=th11']});createRoot(document.getElementById('root')).render(<RouterProvider router={router}/>);
`;
await build({absWorkingDir:project,stdin:{contents:entry,resolveDir:project,loader:'tsx'},outfile:resolve(out,'ui.mjs'),bundle:true,format:'esm',platform:'browser',jsx:'automatic',external:['/assets/*'],plugins:[authoredSourcesPlugin(project)],logLevel:'silent'});
const engine=`<!doctype html><style>body{margin:0;background:#000}canvas{width:100vw;height:100vh;object-fit:contain}</style><canvas id="canvas" width="640" height="480"></canvas><script type="module">
import createModule from '/native.mjs';import{createNativeLocalReplay}from'/local-replay-native.mjs';let core;
const emit=(event,fields={})=>parent.postMessage({protocol:'eagler-touhou/1',game:'th11',epoch:1,event,...fields},location.origin);
const module=await createModule({canvas:document.querySelector('canvas'),noInitialRun:true,resetBrowserKeyboard(){},locateFile:p=>p.endsWith('.wasm')?'/native.wasm':p,instantiateWasm(imports,ready){return WebAssembly.instantiateStreaming(fetch('/native.wasm'),imports).then(({instance,module})=>{core=instance.exports;ready(instance,module);return core;});}});
module.FS.mkdirTree('/fonts');module.FS.mkdirTree('/save/replay');for(const url of ${JSON.stringify(['/game.dat',...readdirSync(fonts).map(name=>'/fonts/'+name),...fixtures.map(f=>'/'+f.path)])}){const target=url==='/game.dat'?'/th11.dat':url.startsWith('/replay/')?'/save'+url:url;module.FS.writeFile(target,new Uint8Array(await(await fetch(url)).arrayBuffer()));}
core.th11_music_enabled(0);if(!core.th11_initialize())throw Error('Native initialize rejected');
const player=createNativeLocalReplay({core:()=>core,module:()=>module,physical:path=>'/save/'+path,stopLoop:()=>core.th11_loop_stop(),onState:playback=>emit('runtime-info',{playback}),onError:error=>emit('error',{error:String(error)})});
addEventListener('message',async event=>{if(event.source!==parent||event.origin!==location.origin)return;const m=event.data;if(m.command!=='playback')return;try{const result=await player.command(m);parent.postMessage({...m,ok:true,...result},location.origin);}catch(error){parent.postMessage({...m,ok:false,error:String(error)},location.origin);}});emit('ready');
</script>`;
const html='<!doctype html><meta charset="utf-8"><link rel="stylesheet" href="/public/styles.css"><link rel="stylesheet" href="/ui.css"><style>body{margin:0;--paper:#e8e4db;--muted:#a5a197;--line:#ffffff20;--ui-font:Arial,sans-serif}#player{position:fixed;inset:0}.replay-file-browser{min-width:0}</style><div id="root"></div><script type="module" src="/ui.mjs"></script>';
const server=createServer((request,response)=>{try{const path=new URL(request.url,'http://localhost').pathname;let content;
 if(path==='/')content=Buffer.from(html);else if(path==='/engine.html')content=Buffer.from(engine);else if(path==='/ui.mjs'||path==='/ui.css')content=readFileSync(resolve(out,path.slice(1)));
 else if(resources.has(path))content=readFileSync(resources.get(path));else if(/^\/(?:public\/|assets\/)[\w./-]+$/.test(path)&&!path.includes('..'))content=readFileSync(resolve(project,'public',path.replace(/^\/(?:public\/)?/,'')));
 else{response.writeHead(404);response.end();return;}
 const type=path==='/'||path.endsWith('.html')?'text/html':path.endsWith('.mjs')?'text/javascript':path.endsWith('.css')?'text/css':path.endsWith('.wasm')?'application/wasm':extname(path)==='.svg'?'image/svg+xml':'application/octet-stream';response.writeHead(200,{'Content-Type':type,'Content-Length':content.length});response.end(content);
 }catch{response.writeHead(404);response.end();}});
await new Promise(ok=>server.listen(0,'127.0.0.1',ok));
const report={passed:false,wasm:createHash('sha256').update(readFileSync(resources.get('/native.wasm'))).digest('hex')};let browser,page;const pageErrors=[];
try{browser=await chromium.launch({headless:true,args:['--enable-unsafe-swiftshader','--autoplay-policy=no-user-gesture-required']});page=await browser.newPage({viewport:{width:1320,height:900}});const errors=pageErrors;page.on('pageerror',e=>errors.push(e.message));
 await page.goto('http://127.0.0.1:'+server.address().port);await page.locator('#openReplay').click();await page.locator('[data-replay-watch]').first().waitFor();const frame=await page.locator('#gameFrame').elementHandle();
 assert.equal(await page.evaluate(()=>commands.some(m=>m.action==='open')),false);await page.locator('[data-replay-watch]').first().click();await page.locator('[data-replay-stage]').first().waitFor({timeout:120000});
 assert.equal(await page.evaluate(()=>commands.some(m=>m.action==='open')),false,'file selection expands original metadata without playback');assert.match(await page.locator('.replay-stage-list').innerText(),/Last Score/);
 assert.doesNotMatch(await page.locator('.replay-stage-list').innerText(),/\b(?:NAME|DATE|PLAYER|RANK|SCORE|SLOW|MODE)\b/);
 await page.locator('[data-replay-stage]').first().click();await page.waitForFunction(()=>window.playback?.frame>=120||window.errors.length);assert.deepEqual(await page.evaluate(()=>window.errors),[]);
 await page.getByRole('button',{name:'暂停',exact:true}).click();await page.waitForFunction(()=>window.playback?.paused===true);const paused=await page.evaluate(()=>playback.frame);await page.waitForTimeout(250);assert.equal(await page.evaluate(()=>playback.frame),paused);assert.equal(await page.evaluate(element=>element===document.querySelector('#gameFrame'),frame),true);
 const checkColumns=async()=>{const columns=await page.locator('.replay-file-summary').first().locator('span').evaluateAll(elements=>elements.map(element=>{const r=element.getBoundingClientRect();return {x:r.x,y:r.y,right:r.right,bottom:r.bottom,visible:element.clientWidth>=element.scrollWidth};}));assert.equal(columns.length,4);assert.ok(columns.every(c=>c.visible),'native summary values fit their columns');assert.ok(columns[0].right<columns[1].x&&columns[2].right<columns[3].x,'summary columns have a visible gap');assert.ok(columns[0].bottom<=columns[2].y,'summary rows do not collide');};
 await checkColumns();await page.screenshot({path:resolve(out,'desktop.png')});await page.setViewportSize({width:390,height:844});await checkColumns();await page.screenshot({path:resolve(out,'mobile.png'),fullPage:true});await page.locator('.replay-stage-list').scrollIntoViewIfNeeded();await page.screenshot({path:resolve(out,'mobile-details.png')});
 assert.equal(await page.evaluate(()=>document.documentElement.scrollWidth<=innerWidth),true,'mobile metadata must not overflow horizontally');assert.deepEqual(errors,[]);report.passed=true;report.metadata=await page.locator('.replay-stage-list').innerText();report.playback=await page.evaluate(()=>playback);
}catch(error){report.failure=String(error);report.errors=pageErrors;if(page){report.dom=await page.locator('body').innerText();await page.screenshot({path:resolve(out,'failure.png')});}throw error;}finally{writeFileSync(resolve(out,'report.json'),JSON.stringify(report,null,2));await browser?.close();await new Promise(ok=>server.close(ok));}
console.log('Replay React/native workspace: PASS');
