import assert from 'node:assert/strict';
import {createServer} from 'node:http';
import {readFile} from 'node:fs/promises';
import {resolve} from 'node:path';
import {fileURLToPath} from 'node:url';
import {build} from 'esbuild';
import puppeteer from 'puppeteer-core';
import {findChromiumExecutable} from '../../lib/chromium-executable.mjs';
import {authoredSourcesPlugin} from '../../lib/authored-source-resolution.mjs';

const project=fileURLToPath(new URL('../../',import.meta.url));
const result=await build({absWorkingDir:project,stdin:{loader:'tsx',resolveDir:project,contents:`
import React from 'react';
import {createRoot} from 'react-dom/client';
import {MultiplayerQuickChat} from './app/components/player/MultiplayerQuickChat';
import {LocaleProvider} from './app/i18n';
import {createMultiplayerQuickChatModel} from './src/launcher/multiplayer-quick-chat-model.mts';
const chat=globalThis.chat=createMultiplayerQuickChatModel({voice:()=>{},send:()=>true});
const id={room:'th11mp-4023',serial:1};
chat.update({...id,visible:true,connected:true,localSeat:0,seats:[{clientId:'a',name:'P1'},{clientId:'b',name:'P2'}]});
chat.receive({type:'chat-status',registered:true,authorId:'account-a',bannedUntil:0});
chat.receive({type:'free-chat',...id,messageId:8,seat:1,clientId:'b',authorId:'account-b',name:'P2',text:'A message over the controls'});
globalThis.bombs=0;
createRoot(document.getElementById('root')).render(<LocaleProvider locale="en">
  <section id="player" className="player open touch-enabled touch-layout-custom">
    <iframe id="gameFrame" title="Game" />
    <div id="touchDirectSurface" className="touch-direct-surface" />
    <div id="touchHud" className="touch-hud"><button id="touchBomb" className="touch-bomb" data-touch-layout-control="bomb"
      style={{'--touch-layout-x':'140px','--touch-layout-y':'263px','--touch-layout-scale':1,zIndex:33}}
      onPointerDown={()=>globalThis.bombs++}>B</button></div>
    <MultiplayerQuickChat model={chat} iconUrl="" decisions={{askConfirmation:async()=>false}} />
  </section>
</LocaleProvider>);
`},bundle:true,format:'esm',jsx:'automatic',write:false,outdir:resolve(project,'.cache/chat-touch'),plugins:[authoredSourcesPlugin(project)]});
const css=await readFile(resolve(project,'public/styles.css'));
const server=createServer((req,res)=>{
  const file={ '/chat.js':'.js','/chat.css':'.css' }[req.url];
  if (file) {res.setHeader('content-type',file==='.js'?'text/javascript':'text/css');res.end(result.outputFiles.find(f=>f.path.endsWith(file)).contents);}
  else if(req.url==='/styles.css') {res.setHeader('content-type','text/css');res.end(css);}
  else if(req.url==='/') {res.setHeader('content-type','text/html');res.end('<!doctype html><meta name="viewport" content="width=device-width,initial-scale=1"><link rel="stylesheet" href="/styles.css"><link rel="stylesheet" href="/chat.css"><div id="root"></div><script type="module" src="/chat.js"></script>');}
  else res.writeHead(404).end();
});
await new Promise(done=>server.listen(0,'127.0.0.1',done));
let browser;
try{
  browser=await puppeteer.launch({headless:true,executablePath:await findChromiumExecutable()});
  const page=await browser.newPage(),errors=[];
  page.on('pageerror',error=>errors.push(error.message));
  await page.setViewport({width:800,height:360,isMobile:true,hasTouch:true,deviceScaleFactor:2});
  await page.goto('http://127.0.0.1:'+server.address().port);
  await page.waitForSelector('.mp-quick-chat-prompt');
  await page.tap('.mp-quick-chat-prompt');
  await page.waitForSelector('.mp-quick-chat-picker:not([hidden])',{visible:true});
  const inspect=()=>page.evaluate(()=>{
    const rect=sel=>document.querySelector(sel).getBoundingClientRect();
    const b=rect('#touchBomb'),p=rect('.mp-quick-chat-picker'),l=rect('.mp-quick-chat-log');
    const x=b.x+b.width/2,y=b.y+b.height/2;
    return {x,y,overlap:x>=p.left&&x<=p.right&&y>=p.top&&y<=p.bottom,hit:document.elementFromPoint(x,y)?.id,separate:l.bottom<=p.top};
  });
  const opened=await inspect();
  assert.equal(opened.overlap,true,'fixture reproduces the panel covering B');
  assert.equal(opened.hit,'touchBomb','B is the actual touch target under the open panel');
  assert.equal(opened.separate,true,'the log and picker do not overlap');
  await page.touchscreen.tap(opened.x,opened.y);
  assert.equal(await page.evaluate(()=>globalThis.bombs),1);
  const inputPoint=await page.$eval('.mp-quick-chat-picker input',node=>{
    const bounds=node.getBoundingClientRect(),y=bounds.y+bounds.height/2;
    for(let x=bounds.x+5;x<bounds.right-5;x+=5)
      if(document.elementFromPoint(x,y)===node)return {x,y};
    return null;
  });
  assert.ok(inputPoint,'chat input remains available beside the gameplay control');
  await page.touchscreen.tap(inputPoint.x,inputPoint.y);
  assert.equal(await page.$eval('.mp-quick-chat-picker input',node=>document.activeElement===node),true);
  await page.keyboard.type('unsent');
  assert.equal(await page.evaluate(()=>chat.getSnapshot().draft),'unsent');
  await page.tap('.mp-quick-chat-close');
  assert.equal(await page.$eval('.mp-quick-chat-picker',node=>node.hidden),true);
  assert.equal(await page.evaluate(()=>document.activeElement.id),'gameFrame');
  assert.equal(await page.$('.mp-quick-chat-log button'),null,'closed log has no clickable rows');
  assert.equal((await inspect()).hit,'touchBomb','B stays available after closing free chat');
  await page.touchscreen.tap(opened.x,opened.y);
  assert.equal(await page.evaluate(()=>globalThis.bombs),2);
  await page.evaluate(() => {
    for (let i=9;i<=13;i++) chat.receive({type:'free-chat',room:'th11mp-4023',serial:1,messageId:i,
      seat:1,clientId:'b',authorId:'account-b',name:'P2',text:'Message '+i});
  });
  const messages=()=>page.$$eval('.mp-quick-chat-log p:not([hidden])',rows=>rows.map(row=>row.textContent));
  const latest=['P2(P2): Message 11','P2(P2): Message 12','P2(P2): Message 13'];
  await page.waitForFunction(()=>document.querySelectorAll('.mp-quick-chat-log p:not([hidden])').length===3);
  assert.deepEqual(await messages(),latest);
  await page.evaluate(()=>chat.openComposer());
  await page.waitForSelector('.mp-quick-chat-history button');
  assert.deepEqual(await messages(),latest);
  await page.$eval('.mp-quick-chat-history button:first-child',button=>button.click());
  await page.waitForFunction(()=>document.querySelector('.mp-quick-chat-log p:not([hidden])').textContent.includes('A message'));
  assert.deepEqual(await messages(),['P2(P2): A message over the controls','P2(P2): Message 9','P2(P2): Message 10']);
  await page.$eval('.mp-quick-chat-history button:last-child',button=>button.click());
  await page.waitForFunction(()=>document.querySelector('.mp-quick-chat-log p:not([hidden])').textContent.endsWith('11'));
  assert.deepEqual(await messages(),latest);
  await page.evaluate(() => {
    document.querySelector('#player').classList.remove('touch-layout-custom');
    document.querySelector('#touchBomb').removeAttribute('style');
    chat.dismiss();
  });
  await page.setViewport({width:390,height:844,isMobile:true,hasTouch:true,deviceScaleFactor:2});
  const defaultBomb=await page.evaluate(()=>{
    const b=document.querySelector('#touchBomb').getBoundingClientRect();
    const x=b.left+b.width/2,y=b.top+b.height/2;
    const log=document.querySelector('.mp-quick-chat-log').getBoundingClientRect();
    const prompt=document.querySelector('.mp-quick-chat-prompt').getBoundingClientRect();
    const overlaps=box=>b.left<box.right&&b.right>box.left&&b.top<box.bottom&&b.bottom>box.top;
    return {x,y,overlap:overlaps(log)||overlaps(prompt),hit:document.elementFromPoint(x,y)?.id};
  });
  assert.equal(defaultBomb.overlap,true,'portrait layout exercises the default controls overlapping chat');
  assert.equal(defaultBomb.hit,'touchBomb');
  await page.touchscreen.tap(defaultBomb.x,defaultBomb.y);
  assert.equal(await page.evaluate(()=>globalThis.bombs),3);
  await page.tap('.mp-quick-chat-prompt');
  await page.waitForSelector('.mp-quick-chat-picker:not([hidden])',{visible:true});
  assert.deepEqual(await messages(),latest);
  assert.deepEqual(errors,[]);
  console.log('PASS: mobile chat and B touch hit testing');
}finally{await browser?.close();await new Promise(done=>server.close(done));}
