// Native Tab/Enter/Space against the production React dialog, without a game.
import assert from 'node:assert/strict';
import {createServer} from 'node:http';
import {execFileSync} from 'node:child_process';
import {resolve} from 'node:path';
import {build} from 'esbuild';
import puppeteer from 'puppeteer-core';
import {authoredSourcesPlugin} from '../react-main/authored-sources.mjs';
const project=resolve(import.meta.dirname,'../..');
const bundle=await build({stdin:{resolveDir:project,loader:'tsx',contents:`
 import React from 'react';import {createRoot} from 'react-dom/client';
 import {ConfirmationDialog} from './app/components/ConfirmationDialog.tsx';
 const root=createRoot(document.getElementById('root'));let sequence=0;
 window.results=[];const settle=choice=>{window.results.push(choice);document.body.dataset.decisions=String(window.results.length);};
 window.show=options=>root.render(<ConfirmationDialog key={++sequence} open
  title="Decision" message="Choose an action" confirmText="Confirm" cancelText="Cancel" {...options}
  onConfirm={()=>settle('confirm')} onCancel={()=>settle('cancel')} onSecondary={()=>settle('secondary')}/>);
 `},bundle:true,write:false,format:'esm',platform:'browser',jsx:'automatic',plugins:[authoredSourcesPlugin(project)]});
const server=createServer((req,res)=>{res.setHeader('Content-Type',req.url==='/api.mjs'?'text/javascript':'text/html');res.end(req.url==='/api.mjs'?bundle.outputFiles[0].contents:'<!doctype html><div id="root"></div><script type="module" src="/api.mjs"></script>');});
await new Promise(resolve=>server.listen(0,'127.0.0.1',resolve));
let browser;
try {
 const executablePath=process.env.CHROME_PATH||execFileSync(process.env.PYTHON||'python',['-c','from playwright.sync_api import sync_playwright\nwith sync_playwright() as p: print(p.chromium.executable_path)'],{encoding:'utf8'}).trim();
 browser=await puppeteer.launch({executablePath,headless:true});const page=await browser.newPage(),errors=[];
 page.on('pageerror',error=>errors.push(error.message));
 await page.emulateMediaFeatures([{name:'prefers-reduced-motion',value:'reduce'}]);
 await page.goto(`http://127.0.0.1:${server.address().port}`);
 await page.waitForFunction(()=>document.querySelector('#root'));
 const cases=[
  [{},1,'Enter','confirm'],[{},1,'Space','confirm'],[{},0,'Enter','cancel'],
  [{secondaryText:'Other'},1,'Enter','secondary'],[{secondaryText:'Other'},2,'Enter','confirm'],
  [{secondaryText:'Other',confirmOnEnter:true},1,'Enter','secondary'],
  [{hideCancel:true},0,'Enter','confirm'],
 ];
 for(const [options,tabs,key,expected] of cases) {
  const before=await page.evaluate(()=>window.results.length);
  await page.evaluate(options=>window.show(options),options);await page.waitForSelector('dialog[open]');
  for(let n=0;n<tabs;n++)await page.keyboard.press('Tab');
  await page.keyboard.press(key);await page.waitForSelector('dialog:not([open])');
  await page.waitForFunction(count=>Number(document.body.dataset.decisions)>count,{},before);
  assert.equal(await page.evaluate(()=>results.at(-1)),expected,JSON.stringify({options,tabs,key}));
 }
 for(const confirmOnEnter of [false,true]) {
  await page.evaluate(confirmOnEnter=>window.show({confirmOnEnter}),confirmOnEnter);await page.waitForSelector('dialog[open]');
  const before=await page.evaluate(()=>{const dialog=document.querySelector('dialog');dialog.tabIndex=-1;dialog.focus();return results.length;});
  await page.keyboard.press('Enter');
  if(confirmOnEnter){await page.waitForSelector('dialog:not([open])');await page.waitForFunction(count=>Number(document.body.dataset.decisions)>count,{},before);assert.equal(await page.evaluate(()=>results.at(-1)),'confirm');}
  else {assert.equal(await page.evaluate(()=>results.length),before);await page.keyboard.press('Escape');await page.waitForSelector('dialog:not([open])');await page.waitForFunction(count=>Number(document.body.dataset.decisions)>count,{},before);}
 }
 assert.deepEqual(errors,[]);console.log(JSON.stringify({confirmationKeyboard:'PASS',nativeCases:9,browser:await browser.version()}));
} finally {await browser?.close();await new Promise(resolve=>server.close(resolve));}
