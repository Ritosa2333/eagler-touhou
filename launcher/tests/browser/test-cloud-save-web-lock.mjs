/** Two real Chromium pages share the product lock; closing a live page releases it. */
import assert from 'node:assert/strict';
import {createHash} from 'node:crypto';
import {createServer} from 'node:http';
import {fileURLToPath} from 'node:url';
import {build} from 'esbuild';
import puppeteer from 'puppeteer-core';
import {findChromiumExecutable} from '../../lib/chromium-executable.mjs';
import {PRODUCT_GAMES} from '../../../common/contracts/product-catalog.mjs';
import {authoredSourcesPlugin} from '../react-main/authored-sources.mjs';

const project=fileURLToPath(new URL('../..',import.meta.url));
const scoreName=PRODUCT_GAMES.th06.storage.scoreFile;
const allowed=[scoreName,...PRODUCT_GAMES.th06.storage.configFiles,...(PRODUCT_GAMES.th06.storage.hintFiles??[]),'score06.dat','spell_capture.dat'];
const bytes=[4,5,6],sha256=value=>createHash('sha256').update(Buffer.from(value)).digest('hex');
const built=await build({absWorkingDir:project,stdin:{resolveDir:project,loader:'ts',contents:`
  import {createCloudSaveSync} from './app/services/cloud-save-sync.ts';
  window.installCloudSaveSync = initial => {
    window.mutableSaveBytes = initial;
    window.saveReadCount = 0;
    window.cloudSaveSync = createCloudSaveSync({storage:window.localStorage,
      read:async()=>{window.saveReadCount++;return window.mutableSaveBytes.length?[new File([new Uint8Array(window.mutableSaveBytes)],${JSON.stringify(scoreName)},{type:'application/octet-stream'})]:[];},
      isRunning:()=>false,decide:async()=> 'local',restore:async()=>{},notify:key=>(window.cloudSaveNotices??=[]).push(key)});
  };
`},bundle:true,platform:'browser',format:'iife',write:false,logLevel:'silent',define:{'import.meta.env':JSON.stringify({VITE_ACCOUNTS_ENABLED:'true'})},plugins:[authoredSourcesPlugin(project)]});
const script=built.outputFiles[0].text;
let revision=0,pending=[],saves=[],uploadCount=0,sessionCount=0;
const product={id:'th06',game:'th06',multiplayer:false,files:allowed,requiredFiles:[scoreName],revision:0};
const statistics={game:'th06',parserVersion:1,format:'test',complete:false,playtimeSeconds:null,applicationTimeSeconds:null,playCount:null,clearCount:null,oneCreditClearCount:null,loadouts:[],progress:[],warnings:[]};
function listing(){return {products:[{...product,revision}],saves,pending,limits:{fileBytes:4*1024*1024,totalBytes:8*1024*1024,files:6}};}
const server=createServer((request,response)=>{
  const url=new URL(request.url,'http://127.0.0.1');
  if(request.method==='OPTIONS'){response.writeHead(204,{'Access-Control-Allow-Origin':'*','Access-Control-Allow-Methods':'GET,POST,OPTIONS','Access-Control-Allow-Headers':'content-type,origin'});response.end();return;}
  if(url.pathname==='/'){response.writeHead(200,{'Content-Type':'text/html','Cache-Control':'no-store'});response.end('<!doctype html><script src="/cloud.js"></script>');return;}
  if(url.pathname==='/cloud.js'){response.writeHead(200,{'Content-Type':'text/javascript','Cache-Control':'no-store'});response.end(script);return;}
  if(url.pathname==='/api/ready'){response.writeHead(200,{'Content-Type':'application/json','Cache-Control':'no-store'});response.end(JSON.stringify({ready:true}));return;}
  if(url.pathname==='/api/auth/get-session'){response.writeHead(200,{'Content-Type':'application/json','Cache-Control':'no-store'});response.end(JSON.stringify({user:{username:'alice',name:'Alice'}}));return;}
  if(url.pathname==='/api/v1/me/cloud-saves'&&request.method==='GET'){response.writeHead(200,{'Content-Type':'application/json','Cache-Control':'no-store'});response.end(JSON.stringify(listing()));return;}
  const route=url.pathname;
  if(route==='/api/v1/me/cloud-saves/th06/sessions'&&request.method==='POST'){
    const chunks=[];request.on('data',chunk=>chunks.push(chunk));request.on('end',()=>{const body=JSON.parse(Buffer.concat(chunks).toString());sessionCount++;pending=[{product:'th06',deviceId:body.deviceId,runId:body.runId,baseRevision:body.baseRevision,startedAt:new Date().toISOString()}];response.writeHead(200,{'Content-Type':'application/json'});response.end(JSON.stringify({started:true}));});return;
  }
  if(route==='/api/v1/me/cloud-saves/th06/sessions/complete'&&request.method==='POST'){
    const chunks=[];request.on('data',chunk=>chunks.push(chunk));request.on('end',()=>{pending=[];response.writeHead(200,{'Content-Type':'application/json'});response.end(JSON.stringify({completed:true}));});return;
  }
  if(route==='/api/v1/me/cloud-saves/th06'&&request.method==='POST'){
    request.resume();request.on('end',()=>{uploadCount++;revision++;product.revision=revision;
      saves=[{product:'th06',revision,uploadedAt:new Date().toISOString(),files:[{name:scoreName,bytes:bytes.length,sha256:sha256(bytes)}],statistics}];pending=[];
      response.writeHead(200,{'Content-Type':'application/json'});response.end(JSON.stringify(saves[0]));});return;
  }
  response.writeHead(404,{'Content-Type':'application/json'});response.end('{}');
});
let browser;
try{
  await new Promise(resolve=>server.listen(0,'127.0.0.1',resolve));
  const origin=`http://127.0.0.1:${server.address().port}`;
  browser=await puppeteer.launch({headless:true,executablePath:await findChromiumExecutable(),args:['--no-sandbox']});
  const active=await browser.newPage(),follower=await browser.newPage(),errors=[];
  active.on('pageerror',error=>errors.push(error.message));follower.on('pageerror',error=>errors.push(error.message));
  await Promise.all([active.goto(origin),follower.goto(origin)]);
  await Promise.all([active.evaluate(initial=>window.installCloudSaveSync(initial),[]),follower.evaluate(initial=>window.installCloudSaveSync(initial),bytes)]);
  assert.equal(await active.evaluate(()=>navigator.locks?.request instanceof Function),true,'Chromium exposes Web Locks on this same-origin fixture');
  assert.equal(await active.evaluate(()=>window.cloudSaveSync.beforeLaunch('th06')),true,'the first page owns the game lock and registers its run');
  const readsBefore=await follower.evaluate(()=>window.saveReadCount);
  assert.equal(await follower.evaluate(()=>window.cloudSaveSync.beforeLaunch('th06')),false,'the second page is held at the launch gate');
  await follower.evaluate(()=>window.cloudSaveSync.flush());
  assert.equal(await follower.evaluate(()=>window.saveReadCount),readsBefore,'a follower never reads the active game’s save files');
  assert.equal(sessionCount,1);assert.equal(uploadCount,0);
  // This closes the actual document. It does not call sync.dispose(), finished(), or release a test-owned lock.
  await active.close();
  await follower.evaluate(values=>{window.mutableSaveBytes=values;},bytes);
  for(let attempt=0;attempt<30&&uploadCount===0;attempt++){
    await follower.evaluate(()=>window.cloudSaveSync.flush());
    if(uploadCount===0)await new Promise(resolve=>setTimeout(resolve,100));
  }
  assert.equal(uploadCount,1,'the remaining page immediately recovers and uploads after Chromium closes the lock holder');
  assert.equal(pending.length,0);assert.deepEqual(errors,[]);
  await follower.close();
}finally{
  await browser?.close();
  await new Promise(resolve=>server.close(resolve));
}
