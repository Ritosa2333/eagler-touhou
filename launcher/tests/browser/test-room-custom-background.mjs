import assert from 'node:assert/strict';
import puppeteer from 'puppeteer-core';
const browser=await puppeteer.launch({executablePath:process.env.CHROME_PATH||'C:/Program Files/Google/Chrome/Application/chrome.exe',headless:true});
try {
 const page=await browser.newPage();
 await page.evaluateOnNewDocument(()=>{localStorage.setItem('eagler-touhou-first-use-notice-seen-v1','1');localStorage.setItem('eagler.background.settings',JSON.stringify({mode:'grid',shade:35,panel:60,card:70,navigation:88,category:88,fit:'contain'}));});
 await page.goto(new URL('lobby.html',process.argv[2]||'http://127.0.0.1:5173/'),{waitUntil:'networkidle2'});
 await page.waitForFunction(()=>document.documentElement.dataset.backgroundMode==='grid');
 await page.evaluate(()=>{const player=document.querySelector('#player');player.classList.add('room-workspace');player.insertAdjacentHTML('beforeend','<div class="room-workspace-sidebar" data-test-sidebar></div><div class="room-workspace-roster"><div class="mp-seat" data-test-seat data-card-state="ready" style="--seat-background:rgb(255, 0, 0)"></div></div>');});
 let result=await page.evaluate(()=>({background:getComputedStyle(document.querySelector('#player')).backgroundImage,sidebar:getComputedStyle(document.querySelector('[data-test-sidebar]')).backgroundColor,seat:getComputedStyle(document.querySelector('[data-test-seat]')).backgroundColor}));
 assert.match(result.background,/gradient/);assert.match(result.sidebar,/0\.6\)/);assert.equal(result.seat,'rgb(255, 0, 0)');
 await page.evaluate(()=>{document.documentElement.dataset.backgroundMode='seigaiha';});
 result=await page.evaluate(()=>({background:getComputedStyle(document.querySelector('#player')).backgroundImage}));
 assert.match(result.background,/radial-gradient/);
 await page.evaluate(()=>{const root=document.documentElement;root.dataset.backgroundMode='image';root.style.setProperty('--custom-background-image','url("/assets/favicon.png")');});
 result=await page.evaluate(()=>({background:getComputedStyle(document.querySelector('#player')).backgroundImage,size:getComputedStyle(document.querySelector('#player')).backgroundSize}));
 assert.match(result.background,/favicon\.png/);assert.match(result.size,/contain/);
 await page.evaluate(()=>document.documentElement.removeAttribute('data-custom-background'));
 assert.equal(await page.$eval('#player',e=>getComputedStyle(e).backgroundImage),'none');
 console.log('PASS: room background modes, opacity and default');
} finally {await browser.close();}
