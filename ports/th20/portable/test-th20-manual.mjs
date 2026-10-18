import {createServer} from 'node:http';
import {createReadStream,statSync} from 'node:fs';
import {resolve,extname,sep} from 'node:path';

const root=resolve(import.meta.dirname,'../build-eagler');
const data=resolve(import.meta.dirname,'../build-eagler-smoke/game-data/th20.dat');
const font='C:/Windows/Fonts/msgothic.ttc';
const types={'.html':'text/html','.mjs':'text/javascript','.js':'text/javascript','.json':'application/json','.wasm':'application/wasm','.dat':'application/octet-stream'};
const server=createServer((request,response)=>{
  const pathname=new URL(request.url,'http://localhost').pathname;
  const file=pathname==='/game-data/th20.dat'?data:pathname==='/msgothic.ttc'?font:resolve(root,'.'+pathname);
  if(file!==data&&file!==font&&!file.startsWith(root+sep)){response.writeHead(403).end();return;}
  try{const stat=statSync(file);if(!stat.isFile())throw Error('not a file');response.writeHead(200,{'Content-Type':types[extname(file)]||'application/octet-stream','Content-Length':stat.size,'Cache-Control':'no-store'});createReadStream(file).pipe(response);}catch{response.writeHead(404).end();}
}).listen(8100,'127.0.0.1',()=>console.log('th20 perf probe http://127.0.0.1:8098'));

import puppeteer from '../../eagler-touhou/node_modules/puppeteer-core/lib/puppeteer/puppeteer-core.js';

const browser=await puppeteer.launch({executablePath:'C:/Users/w3051/AppData/Local/ms-playwright/chromium-1243/chrome-win64/chrome.exe',headless:true,args:['--use-gl=angle','--use-angle=swiftshader','--enable-webgl','--no-sandbox']});
let failed=false;try{
 const page=await browser.newPage();await page.setViewport({width:1280,height:960,deviceScaleFactor:1});
 page.on('pageerror',error=>{failed=true;console.error('PAGEERROR',error.stack);});
 page.on('console',message=>{if(message.type()==='error')console.error('CONSOLE',message.text());});
 await page.evaluateOnNewDocument(()=>{window.__eaglerPrepareManagedRuntimeDataV1=async()=>({buffer:await fetch('/game-data/th20.dat').then(r=>r.arrayBuffer())});});
 await page.goto('http://127.0.0.1:8100/th20.html?managedData=1&runtimeEpoch=1&gameGeneration=local',{waitUntil:'domcontentloaded',timeout:30000});
 await page.waitForFunction(()=>window.__th20Runtime?.core||document.querySelector('#error')?.textContent,{timeout:120000});
 const error=await page.$eval('#error',element=>element.textContent);if(error)throw Error(error);
 await page.evaluate(async()=>{await window.__th20Runtime.command({command:'configure',music:'none',sharedResources:[{path:'/msgothic.ttc',url:'./msgothic.ttc'}]});window.__th20Runtime.launch();});
 await page.waitForFunction(()=>window.__th20Runtime.status()[0]===4,{timeout:120000});
 await new Promise(r=>setTimeout(r,5000));
 const title=await page.evaluate(()=>{const c=window.__th20Runtime.core;return {state:window.__th20Runtime.status(),memoryMiB:c.memory.buffer.byteLength/1048576,stats:Array.from(new Uint32Array(c.memory.buffer,c.sdl_stats(),17))};});
 console.log('TITLE',JSON.stringify(title));
 const press=async code=>{await page.evaluate(code=>window.postMessage({protocol:'eagler-touhou/1',game:'th20',epoch:1,command:'keyboard',code,down:true},location.origin),code);await new Promise(r=>setTimeout(r,130));await page.evaluate(code=>window.postMessage({protocol:'eagler-touhou/1',game:'th20',epoch:1,command:'keyboard',code,down:false},location.origin),code);await new Promise(r=>setTimeout(r,650));};
   const snapshot=()=>page.evaluate(()=>{const c=window.__th20Runtime.core;return Array.from(new Int32Array(c.memory.buffer,c.manual_probe(),5));});
 const waitHelp=(state,sub)=>page.waitForFunction((state,sub)=>{const c=window.__th20Runtime.core;const s=new Int32Array(c.memory.buffer,c.manual_probe(),5);return s[0]===state&&s[1]===sub&&s[2]>=20;},{timeout:30000},state,sub);
 await page.waitForFunction(()=>window.__th20Runtime.status()[3]===1&&window.__th20Runtime.status()[4]===2,{timeout:30000});await new Promise(r=>setTimeout(r,1000));
 for(let cycle=0;cycle<3;cycle++){
  if(cycle===0){await press('ArrowUp');await press('ArrowUp');}await press('KeyZ');await waitHelp(1,1);
  await press('KeyZ');await waitHelp(1,4);
  for(let i=0;i<9;i++){await waitHelp(1,4);console.log('PAGE',cycle,i,await snapshot());if(i<8){await press('ArrowDown');await page.waitForFunction(i=>{const c=window.__th20Runtime.core;const s=new Int32Array(c.memory.buffer,c.manual_probe(),5);return s[3]===i+1&&s[1]===4&&s[2]>=20;},{timeout:30000},i);}}
  await press('Escape');await waitHelp(1,1);await press('Escape');await page.waitForFunction(()=>window.__th20Runtime.status()[3]===1&&window.__th20Runtime.status()[4]===2,{timeout:30000});console.log('EXIT',cycle,await snapshot());
 }
 if(failed)throw Error('Manual raised a browser error');console.log('PASS 27 pages, 3 menu exits');
}finally{await browser.close();server.close();}

