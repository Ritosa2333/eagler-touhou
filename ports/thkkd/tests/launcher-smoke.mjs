import puppeteer from '../../../launcher/node_modules/puppeteer-core/lib/puppeteer/puppeteer-core.js';
import {mkdirSync,writeFileSync} from 'node:fs';
import {fileURLToPath} from 'node:url';
const output=fileURLToPath(new URL('../build/launcher-smoke/',import.meta.url));mkdirSync(output,{recursive:true});
const browser=await puppeteer.launch({executablePath:'C:/Program Files (x86)/Microsoft/Edge/Application/msedge.exe',headless:true,args:['--autoplay-policy=no-user-gesture-required'],defaultViewport:{width:1280,height:800}});
try{
 const page=await browser.newPage(),errors=[];page.on('pageerror',e=>errors.push(e.message));page.on('console',m=>{if(m.type()==='error')errors.push(m.text());});page.on('requestfailed',r=>errors.push(r.url()+': '+r.failure()?.errorText));
 await page.goto('http://127.0.0.1:8130/',{waitUntil:'networkidle0',timeout:60000});
 await page.screenshot({path:output+'launcher.png'});
 const result=await page.evaluate(()=>({text:document.body.innerText,buttons:[...document.querySelectorAll('button')].map(b=>({text:b.innerText,label:b.getAttribute('aria-label')}))}));
 writeFileSync(output+'inspect.json',JSON.stringify({errors,...result},null,2));console.log(JSON.stringify({errors,text:result.text.slice(0,4200),buttons:result.buttons}));
 await page.evaluate(()=>[...document.querySelectorAll('button')].find(b=>b.textContent.trim()==='Got it')?.click());
 await page.waitForFunction(()=>!document.querySelector('#firstUseNoticeDialog')?.open,{timeout:10000});
 const links=await page.evaluate(()=>[...document.querySelectorAll('a')].filter(a=>a.href.includes('thkkd')||a.textContent.trim()==='Play').map(a=>({text:a.textContent,href:a.getAttribute('href')})));console.log(JSON.stringify({links}));
 const cards=await page.$$('a[data-book-product="thkkd"]');console.log(JSON.stringify({cards:await Promise.all(cards.map(c=>c.evaluate(e=>{const r=e.getBoundingClientRect();return {rect:r.toJSON(),hit:document.elementFromPoint(r.x+r.width/2,r.y+r.height/2)?.outerHTML.slice(0,800),onClick:String(e[Object.keys(e).find(k=>k.startsWith('__reactProps$'))]?.onClick),inert:!!e.closest('[inert]')};})))}));
 for(const card of cards){if(await card.evaluate(e=>!!e[Object.keys(e).find(k=>k.startsWith('__reactProps$'))]?.onClick&&!e.closest('[inert]'))){await card.click();break;}}await new Promise(r=>setTimeout(r,500));
 if(!new URL(page.url()).searchParams.has('game'))await page.evaluate(()=>{const e=document.querySelector('a[data-book-product="thkkd"]');e[Object.keys(e).find(k=>k.startsWith('__reactProps$'))]?.onClick?.({ctrlKey:false,metaKey:false,shiftKey:false,altKey:false,preventDefault(){}});});await new Promise(r=>setTimeout(r,500));
 console.log(JSON.stringify({session:await page.evaluate(()=>{for(const element of document.querySelectorAll('*')){let fiber=element[Object.keys(element).find(k=>k.startsWith('__reactFiber$'))];while(fiber){const session=fiber.memoizedProps?.session;if(session?.metadata){const state=session.metadata.getSnapshot();return {phase:state.phase,products:state.products,games:Object.keys(state.siteManifest?.games||{}),errors:state.metadataErrors.map(e=>String(e)),available:session.productAvailable('thkkd')};}fiber=fiber.return;}}return null;})}));
 console.log(JSON.stringify({controls:await page.evaluate(()=>({href:location.href,play:[...document.querySelectorAll('*')].filter(b=>b.childElementCount===0&&b.textContent.trim()==='Play').map(b=>b.parentElement.outerHTML)}))}));
 await page.evaluate(()=>[...document.querySelectorAll('button')].find(b=>b.textContent.trim()==='Start game')?.click());
 await new Promise(r=>setTimeout(r,12000));
 await page.screenshot({path:output+'launch.png'});
 const state=await page.evaluate(()=>({text:document.body.innerText.slice(-5000),iframes:[...document.querySelectorAll('iframe')].map(f=>({src:f.src,error:f.contentDocument?.querySelector('#error')?.textContent,ready:!!f.contentWindow?.__thkkdRuntime,first:f.contentWindow?.__thkkdRuntime?.status().first}))}));
 console.log(JSON.stringify({errors,state}));writeFileSync(output+'launch-result.json',JSON.stringify({errors,state},null,2));
}finally{await browser.close();}
