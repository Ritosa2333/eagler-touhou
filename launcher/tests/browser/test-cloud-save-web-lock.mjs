/** Two real Chromium tabs transfer the product lock after the first commits local data. */
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
  import {accountSession} from './app/services/account-session.ts';
  window.installCloudSaveSync = async initial => {
    window.mutableSaveBytes = initial;
    window.saveReadCount = 0;
    window.cloudSaveSync = createCloudSaveSync({storage:window.localStorage,
      read:async()=>{window.saveReadCount++;return window.mutableSaveBytes.length?[new File([new Uint8Array(window.mutableSaveBytes)],${JSON.stringify(scoreName)},{type:'application/octet-stream'})]:[];},
      isRunning:()=>false,decide:async()=> 'local',restore:async()=>{},notify:key=>(window.cloudSaveNotices??=[]).push(key),
      decideStart:async kind=>{(window.cloudSaveDecisions??=[]).push(kind);return false;},
      closeForHandoff:async()=>{
        await new Promise(resolve=>setTimeout(resolve,120));
        localStorage.setItem('fixture-committed-save',JSON.stringify(window.mutableSaveBytes));
        window.cloudSaveSync.finished(true);
        window.handoffClosed=true;
        return true;
      },
      retirePrepared:async()=>{
        window.mutableSaveBytes=JSON.parse(localStorage.getItem('fixture-committed-save'));
        window.handoffReloaded=true;
        return true;
      }});
    await accountSession.whenReady();
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
  const beforeStart=page=>page.evaluate(()=>window.cloudSaveSync.beforeStart('th06',new AbortController().signal));
  assert.equal(await beforeStart(active),'continue');
  await active.evaluate(()=>window.cloudSaveSync.started('th06'));
  await active.waitForFunction(()=>localStorage.getItem('eagler-cloud-sync-v1:save:th06')?.includes('"runId":"'));
  await active.evaluate(values=>{window.mutableSaveBytes=values;},bytes);

  assert.equal(await beforeStart(follower),'reload','the follower starts after the previous tab saves and retires');
  assert.equal(await active.evaluate(()=>window.handoffClosed),true);
  assert.equal(await follower.evaluate(()=>window.handoffReloaded),true);
  assert.deepEqual(await follower.evaluate(()=>window.mutableSaveBytes),bytes);
  assert.deepEqual(await follower.evaluate(()=>window.cloudSaveDecisions??=[]),[]);
  assert.deepEqual(await active.evaluate(()=>window.cloudSaveNotices??=[]),[]);
  assert.equal(uploadCount,0,'handoff does not flush cloud progress while starting the next game');
  assert.equal(sessionCount,1,'both tabs use the same device and local journal');

  await follower.evaluate(()=>window.cloudSaveSync.started('th06'));
  await follower.evaluate(()=>window.cloudSaveSync.finished());
  for(let attempt=0;attempt<30&&uploadCount===0;attempt++){
    await follower.evaluate(()=>window.cloudSaveSync.flush());
    if(uploadCount===0)await new Promise(resolve=>setTimeout(resolve,100));
  }
  assert.equal(uploadCount,1,'the last tab commits to the cloud on exit');
  assert.equal(pending.length,0);
  assert.deepEqual(errors,[]);
  await active.close();
  await follower.close();
}finally{
  await browser?.close();
  await new Promise(resolve=>server.close(resolve));
}
