// Actual browser ICE/DataChannels and the personal panel | two local peers.
import assert from 'node:assert/strict';
import {createServer} from 'node:http';
import {mkdir, readFile, writeFile} from 'node:fs/promises';
import {resolve} from 'node:path';
import {fileURLToPath} from 'node:url';
import {build} from 'esbuild';
import puppeteer from 'puppeteer-core';
import {findChromiumExecutable} from '../../lib/chromium-executable.mjs';
const project = fileURLToPath(new URL('../../', import.meta.url));
const out = resolve(project, '.cache/room-packet-loss-browser'); await mkdir(out, {recursive:true});
await build({absWorkingDir:project, stdin:{loader:'tsx', resolveDir:project, contents:`
import React from 'react';
import {createRoot} from 'react-dom/client';
import {RoomPersonal} from './app/components/room/RoomPersonal';
import {createRoomNetwork} from './src/launcher/room-network.mts';
import './app/components/room/room-workspace.css';
const side=Number(new URLSearchParams(location.search).get('side')||0), ids=['peer-a','peer-b'];
const seats=ids.map((clientId,index)=>({clientId,name:index?'对方':'自己',loadout:0}));
const state={product:'th09mp',room:{code:'test',synced:true,phase:'lobby',playerCount:2,seats},seat:side,preferredLoadout:0,displayName:seats[side].name,connected:true,preferTurn:false};
const root=createRoot(document.getElementById('root'));
window.suspended=false;
window.network=createRoomNetwork({send:message=>{parent.postMessage({kind:'signal',side,message},location.origin);return true;},changed:()=>render()});
const service={setDisplayName:x=>x,standUp(){},checkGame(){},setPreferTurn(value){state.preferTurn=value;render();}};
function render(){root.render(<RoomPersonal state={state} service={service} network={window.network} assetUrl={x=>x} th09NetworkOverlayOpen={false} hidden={false}/>);}
window.signal=async message=>{await window.network.receive({...message,from:ids[1-side]});};
window.start=async()=>{await window.network.receive({type:'room-probe-config',iceServers:[]});window.network.update({localId:ids[side],peers:ids,active:true});render();};
window.stop=()=>{window.network.suspend();state.room.phase='running';render();};
window.audit=()=>window.network.loss(ids[1-side],'direct');
render();
`},bundle:true,format:'esm',platform:'browser',jsx:'automatic',outfile:resolve(out,'peer.mjs'),logLevel:'silent'});
const peerHtml=`<!doctype html><meta charset="utf-8"><meta name="viewport" content="width=device-width,initial-scale=1"><link rel="stylesheet" href="/peer.css"><style>
:root{--line:#45453c;--paper:#eae8d9;--ink:#191a17;--muted:#a5a59a}body{margin:0;padding:22px;background:var(--ink);color:var(--paper);font:14px system-ui}button,input{font:inherit;background:transparent;color:inherit;border:1px solid var(--line);border-radius:9px;padding:9px}input{max-width:150px}.mp-name-editor{display:flex;align-items:center;justify-content:space-between;gap:12px;margin-bottom:16px}.mp-local-player{margin:16px 0}.mp-game-check-button{margin-top:12px}button:disabled{opacity:.45}[hidden]{display:none!important}
</style><div id="mpRoomView" class="room-workspace-view"><div id="root"></div></div><script type="module" src="/peer.mjs"></script>`;
const html=`<!doctype html><meta charset="utf-8"><style>body{margin:0;background:#111}iframe{width:430px;height:560px;border:0}</style><iframe id="a" src="/peer?side=0"></iframe><iframe id="b" src="/peer?side=1"></iframe><script>
addEventListener('message',e=>{if(e.origin!==location.origin||e.data?.kind!=='signal')return;document.getElementById(e.data.side?'a':'b').contentWindow.signal(e.data.message);});
</script>`;
const server=createServer(async(req,res)=>{
  try {const url=new URL(req.url,'http://test'); if(url.pathname==='/'){res.setHeader('Content-Type','text/html; charset=utf-8');res.end(html);}
    else if(url.pathname==='/peer'){res.setHeader('Content-Type','text/html; charset=utf-8');res.end(peerHtml);}
    else if(['/peer.mjs','/peer.css'].includes(url.pathname)){res.setHeader('Content-Type',url.pathname.endsWith('.css')?'text/css':'text/javascript');res.end(await readFile(resolve(out,url.pathname.slice(1))));}
    else{res.statusCode=404;res.end();}
  }catch{res.statusCode=500;res.end();}
});
await new Promise(r=>server.listen(0,'127.0.0.1',r));
let browser;
try{
  browser=await puppeteer.launch({headless:true,executablePath:await findChromiumExecutable(),args:['--disable-features=WebRtcHideLocalIpsWithMdns']});
  const page=await browser.newPage(), errors=[];page.on('pageerror',e=>errors.push(e.message));await page.setViewport({width:900,height:620});
  await page.goto('http://127.0.0.1:'+server.address().port);
  const frames=()=>page.frames().filter(f=>f.url().includes('/peer?'));
  for(const frame of frames())await frame.waitForFunction(()=>!!window.network);
  await Promise.all(frames().map(f=>f.evaluate(()=>start())));
  await frames()[0].waitForFunction(()=>network.lossReady('peer-b','direct'),{timeout:15000});
  assert.deepEqual(await Promise.all(frames().map(f=>f.evaluate(()=>audit().state))),['idle','idle']);
  await frames()[0].click('#mpPreferTurn');
  assert.equal(await frames()[0].$eval('#mpPreferTurn',e=>e.getAttribute('aria-checked')), 'true');
  assert.equal(await frames()[1].$eval('#mpPreferTurn',e=>e.getAttribute('aria-checked')), 'false');
  await frames()[0].click('#mpPacketLossTest');
  await Promise.all(frames().map(f=>f.waitForFunction(()=>audit().state==='complete',{timeout:15000})));
  const results=await Promise.all(frames().map(f=>f.evaluate(()=>audit())));
  for(const r of results){assert.equal(r.outgoing.sent,300);assert.equal(r.incoming.sent,300);assert.equal(r.outgoing.percent,0);assert.equal(r.incoming.percent,0);}
  assert.deepEqual(results[0].outgoing,results[1].incoming);
  assert.match(await frames()[0].$eval('[data-loss-lane="direct"]',e=>e.textContent),/0\.0%.*0\.0%/);
  assert.equal(await frames()[0].$eval('[data-loss-lane="turn"]',e=>e.textContent),'TURN连接未就绪');
  await page.screenshot({path:resolve(out,'desktop.png')});
  await page.setViewport({width:390,height:650});
  await frames()[0].evaluate(()=>{document.body.style.padding='16px';});
  await page.evaluate(()=>{document.querySelector('#a').style.width='390px';document.querySelector('#b').style.display='none';});
  assert.equal(await frames()[0].evaluate(()=>document.documentElement.scrollWidth<=390),true);
  await page.screenshot({path:resolve(out,'mobile.png')});
  await frames()[0].click('#mpPacketLossTest');
  await frames()[0].evaluate(()=>stop());
  await frames()[1].waitForFunction(()=>audit().state==='interrupted',{timeout:5000});
  assert.equal(await frames()[0].$('#mpPacketLossTest'),null,'test button disappears when the room leaves the lobby');
  assert.deepEqual(errors,[]);
  await writeFile(resolve(out,'report.json'),JSON.stringify({passed:true,browser:await browser.version(),scope:'Two local ICE peers, actual unreliable DataChannels and RoomPersonal. TURN is unconfigured in this fixture.',results,errors},null,2)+'\n');
  console.log(JSON.stringify({passed:true,report:resolve(out,'report.json')}));
}finally{await browser?.close();await new Promise(r=>server.close(r));}
