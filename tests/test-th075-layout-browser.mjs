import assert from 'node:assert/strict';
import {resolve} from 'node:path';import {mkdir,writeFile} from 'node:fs/promises';
import puppeteer from 'puppeteer-core';import {findChromiumExecutable} from '../lib/chromium-executable.mjs';
import {touchLayoutControlNames,touchLayoutControlMeta,touchLayoutStorageKey,touchLayoutVersion} from '../src/launcher/touch-layout-model.mts';
const base=process.env.EAGLER_TH075_TEST_URL||'http://127.0.0.1:18175/',out=resolve('artifacts/th075-layout');await mkdir(out,{recursive:true});
const shared=JSON.stringify({version:touchLayoutVersion,profiles:{landscape:{controls:Object.fromEntries(touchLayoutControlNames.map(name=>[name,{x:.4,y:.4,scale:1,priority:touchLayoutControlMeta[name].priority}])),viewport:{x:0}},portrait:null}});
const b=await puppeteer.launch({executablePath:await findChromiumExecutable(),headless:true,args:['--no-sandbox']});
try{const context=await b.createBrowserContext(),p=await context.newPage();await p.setViewport({width:1000,height:650,isMobile:true,hasTouch:true});
 await p.evaluateOnNewDocument((key,value)=>localStorage.setItem(key,value),touchLayoutStorageKey,shared);
 async function open(){await p.goto(base+'?test=th075-layout');await p.waitForFunction(()=>window.__eaglerBoot?.done); await p.waitForFunction(()=>document.querySelector('#firstUseNoticeDialog')?.open,{timeout:2500}).catch(()=>{}); await p.evaluate(()=>document.querySelector('#firstUseNoticeCloseHint')?.click());await p.$eval('.game[data-game=th075]',e=>{e.click();e.click();});await p.waitForSelector('#touchLayoutEdit');await p.$eval('#touchLayoutEdit',e=>e.click());await p.waitForFunction(()=>document.querySelector('#player')?.classList.contains('touch-layout-custom')&&document.querySelector('#touchLayoutEditor')?.hidden===false);}
 const rect=()=>p.$eval('#touchJoystick',e=>{const r=e.getBoundingClientRect();return{x:r.x,y:r.y,w:r.width,h:r.height};});
 await open();assert.ok((await p.$eval('.touch-workbench-note',e=>e.textContent)).includes('独立保存'));const before=await rect();assert(before.x<50,'TH075 must begin with its own left-side joystick rather than the shared layout');
 const widths=await p.$$eval('#touchFire,#touchFocus,#touchBomb,#touchFunction',nodes=>nodes.map(n=>Math.round(n.getBoundingClientRect().width)));assert.deepEqual(widths,[60,60,60,60]);
 await p.mouse.move(before.x+before.w/2,before.y+before.h/2);await p.mouse.down();await p.mouse.move(before.x+before.w/2+180,before.y+before.h/2-65,{steps:10});await p.mouse.up();
 await p.screenshot({path:resolve(out,'drag.png')}); const after=await rect();assert(Math.abs(after.x-before.x-180)<3,JSON.stringify({before,after}));assert(Math.abs(after.y-before.y+65)<3,JSON.stringify({before,after}));
 await p.$eval('#touchLayoutSave',e=>e.click());await p.waitForFunction(()=>localStorage.getItem('eagler-touhou-touch-layout-v1-th075'));
 assert.equal(await p.evaluate(k=>localStorage.getItem(k),touchLayoutStorageKey),shared,'Shared shooter layout must remain byte-for-byte unchanged');
 await p.screenshot({path:resolve(out,'saved.png')});await open();const restored=await rect();assert(Math.abs(restored.x-after.x)<3&&Math.abs(restored.y-after.y)<3,JSON.stringify({after,restored}));
 const report={url:base,independentLayout:true,sharedLayoutPreserved:true,horizontalDelta:after.x-before.x,verticalDelta:after.y-before.y,reloadPreserved:true,defaultButtonWidths:widths};await writeFile(resolve(out,'report.json'),JSON.stringify(report,null,2)+'\n');console.log(JSON.stringify(report));await context.close();
}finally{await b.close();}
