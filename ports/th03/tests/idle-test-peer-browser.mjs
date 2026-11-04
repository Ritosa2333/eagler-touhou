import assert from 'node:assert/strict';
import {createRequire} from 'node:module';
import {findChromiumExecutable} from '../../../launcher/lib/chromium-executable.mjs';
const require=createRequire(new URL('../../../launcher/package.json',import.meta.url));
const browser=await require('puppeteer-core').launch({executablePath:await findChromiumExecutable(),headless:true,args:['--enable-unsafe-swiftshader','--autoplay-policy=no-user-gesture-required','--disable-background-timer-throttling','--disable-renderer-backgrounding']});
const page=await browser.newPage();
try{
 await page.goto('http://127.0.0.1:18310/lobby.html?test=th03');
 await page.waitForFunction(()=>!document.documentElement.hasAttribute('data-lobby-boot'),{timeout:45000});
 await page.evaluate(()=>document.querySelector('#firstUseNoticeDialog')?.close());
 await page.waitForFunction(()=>document.querySelector('#createButton')?.disabled===false);await page.click('#createButton');
 await page.waitForSelector('#roomDialog[open]');await page.select('#gameSelect','th03mp');await page.click('#submitRoom');
 await page.waitForSelector('#mpRoomCode');console.log('room',await page.$eval('#mpRoomCode',e=>e.textContent));
 await page.waitForFunction(()=>document.querySelector('#mpReady')?.disabled===false,{timeout:120000});await page.click('#mpReady');
 await page.waitForFunction(()=>document.querySelector('#mpStartGame')?.disabled===false,{timeout:120000});
 assert.equal(await page.$eval('#mpStartGame',e=>e.disabled),false);console.log('PASS idle peer joined and ready');
 await page.click('#mpStartGame');
 await page.waitForFunction(()=>[...document.querySelectorAll('iframe')].some(e=>e.src.includes('runtimeEpoch=')),{timeout:30000});
 const frame=page.frames().find(f=>/th03.html.*runtimeEpoch=/.test(f.url()));
 await frame.waitForFunction(()=>window.th03SyncState?.frame>180,{timeout:120000});console.log('PASS game synchronized with idle peer');
}finally{await browser.close();}
