// Native Chromium geometry, not a synthetic DOM or an Android WebView claim.
// Pass an explicit running Launcher origin with TH06MP available.
import assert from 'node:assert/strict';
import {mkdir,writeFile} from 'node:fs/promises';
import {resolve} from 'node:path';
import puppeteer from 'puppeteer-core';
if(!process.argv[2])throw new Error('usage: node tests/browser/test-options-viewport.mjs URL');
const base=new URL(process.argv[2]);
if(!['http:','https:'].includes(base.protocol)||base.username||base.password)throw new Error('Invalid Launcher origin');
const evidence=resolve('.cache/options-viewport-evidence');await mkdir(evidence,{recursive:true});
const browser=await puppeteer.launch({executablePath:process.env.CHROME_PATH||'C:/Program Files/Google/Chrome/Application/chrome.exe',headless:true});
const results=[];
try {
 for(const [width,height] of [[412,780],[780,412],[360,640],[1280,900]]) {
  const page=await browser.newPage();await page.setViewport({width,height,isMobile:width<1000,hasTouch:width<1000,deviceScaleFactor:1});
  const url=new URL('lobby.html',base);url.searchParams.set('game','th06mp');
  await page.goto(url.href,{waitUntil:'networkidle2'});
  await page.waitForFunction(()=>!document.documentElement.hasAttribute('data-lobby-boot'));
  await page.evaluate(()=>{
   const rect=node=>{if(!node)return null;const r=node.getBoundingClientRect();return {x:r.x,y:r.y,w:r.width,h:r.height};};
   window.__optionsGeometry=[];window.__beforeOptions={library:rect(document.querySelector('.lobby-library')),scroll:[scrollX,scrollY],top:visualViewport.offsetTop,left:visualViewport.offsetLeft};
   const end=performance.now()+1000;
   const sample=()=>{
    const panel=document.querySelector('.tools'),actions=document.getElementById('mpReplayViewer')?.closest('.launch-actions');
    if(document.getElementById('lobbyOptionsDialog')?.open&&panel&&actions){const p=rect(panel),a=rect(actions),scale=new DOMMatrixReadOnly(getComputedStyle(panel).transform).d||1;window.__optionsGeometry.push({top:visualViewport.offsetTop,left:visualViewport.offsetLeft,scroll:[scrollX,scrollY],library:rect(document.querySelector('.lobby-library')),panelHeight:panel.offsetHeight,actionsY:(a.y-p.y)/scale});}
    if(performance.now()<end)requestAnimationFrame(sample);
   };sample();
  });
  const point=await page.evaluate(()=>{const r=document.querySelector('.game[data-product="th06mp"]').getBoundingClientRect();return {x:r.left+r.width/2,y:r.top+r.height/2};});
  // ElementHandle.tap would first scrollIntoView and contaminate this check.
  if(width<1000)await page.touchscreen.tap(point.x,point.y);else await page.mouse.click(point.x,point.y);
  await page.waitForFunction(()=>document.getElementById('lobbyOptionsDialog')?.open);
  await new Promise(resolve=>setTimeout(resolve,1100));
  const trace=await page.evaluate(()=>({before:window.__beforeOptions,frames:window.__optionsGeometry}));
  assert.ok(trace.frames.length>5,'Entrance must have observable native frames');
  for(const frame of trace.frames){assert.ok(Math.abs(frame.top-trace.before.top)<.5,'Opening must not pan the visual viewport vertically');assert.ok(Math.abs(frame.left-trace.before.left)<.5,'Opening must not pan the visual viewport horizontally');assert.deepEqual(frame.scroll,trace.before.scroll);for(const key of ['x','y','w','h'])assert.ok(Math.abs(frame.library[key]-trace.before.library[key])<.5,'Background library geometry changed');}
  const positions=trace.frames.map(frame=>frame.actionsY),heights=trace.frames.map(frame=>frame.panelHeight);
  assert.ok(Math.max(...positions)-Math.min(...positions)<.5,'Controls must stay fixed inside the moving panel');
  assert.ok(Math.max(...heights)-Math.min(...heights)<.5,'Entrance must not resize the panel');
  await page.screenshot({path:resolve(evidence,`options-${width}x${height}.png`)});
  await page.click('#libraryBack');await new Promise(resolve=>setTimeout(resolve,35));
  const backdrop=await page.$eval('#libraryBackdrop',node=>{const style=getComputedStyle(node);return {opacity:style.opacity,visibility:style.visibility,pointerEvents:style.pointerEvents};});
  assert.ok(Number(backdrop.opacity)>0 && Number(backdrop.opacity)<1,'Closing shade must fade smoothly');
  assert.equal(backdrop.pointerEvents,'none');
  await new Promise(resolve=>setTimeout(resolve,220));
  assert.equal(await page.$eval('#libraryBackdrop',node=>getComputedStyle(node).opacity),'0','Shade must finish with the panel');
  results.push({width,height,frames:trace.frames.length,viewportPan:0,backgroundMovement:0,controlsMovement:Math.max(...positions)-Math.min(...positions)});
  await page.close();
 }
 await writeFile(resolve(evidence,'report.json'),JSON.stringify({passed:true,results},null,2)+'\n');
 console.log(JSON.stringify({passed:true,results}));
}finally{await browser.close();}
