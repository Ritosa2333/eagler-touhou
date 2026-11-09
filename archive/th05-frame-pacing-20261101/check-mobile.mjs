import assert from 'node:assert/strict';
import {readFile,writeFile} from 'node:fs/promises';
import {resolve,extname} from 'node:path';
import {launchBrowser} from '../../tools/browser/launch.mjs';
const ops=import.meta.dirname,staged=process.env.TH05_PUBLIC!=='1',origin='https://test.touhou.vip',browser=await launchBrowser();
const musicRoom=process.env.TH05_MUSIC_TEST==='1';
const profile=process.env.TH05_PHONE??'android',ios=profile==='ios',label=(staged?'preflight':'public')+'-'+profile+(musicRoom?'-music-room':'');
const page=await browser.newPage({viewport:{width:844,height:390},hasTouch:true,isMobile:true,deviceScaleFactor:1,userAgent:ios?'Mozilla/5.0 (iPhone; CPU iPhone OS 18_0 like Mac OS X) AppleWebKit/605.1.15 Version/18.0 Mobile/15E148 Safari/604.1':'Mozilla/5.0 (Linux; Android 14; Pixel 8) AppleWebKit/537.36 Chrome/130.0.0.0 Mobile Safari/537.36',serviceWorkers:'block'}),errors=[];
page.setDefaultTimeout(30000);
page.on('pageerror',error=>errors.push(error.message));
const mime={'.html':'text/html','.mjs':'text/javascript','.js':'text/javascript','.json':'application/json','.wasm':'application/wasm','.css':'text/css','.ogg':'audio/ogg','.webp':'image/webp','.svg':'image/svg+xml','.png':'image/png','.woff2':'font/woff2'};
let frame;
try{
 await page.addInitScript(()=>{localStorage.setItem('eagler-touhou-first-use-notice-seen-v1','1');window.__th05Events=[];addEventListener('message',event=>{if(event.data?.game==='th05')window.__th05Events.push(event.data);});});
 if(staged)await page.route(origin+'/**',async route=>{const path=decodeURIComponent(new URL(route.request().url()).pathname);try{const bytes=await readFile(resolve(ops,'payload-v3/site-files',path==='/'?'index.html':path.slice(1)));await route.fulfill({status:200,body:bytes,contentType:mime[extname(path)]??(path==='/'?'text/html':'application/octet-stream')});}catch(error){if(error.code!=='ENOENT'&&error.code!=='EISDIR')throw error;await route.continue();}});
 await page.goto(origin+'/',{waitUntil:'domcontentloaded'});const card=page.locator('[data-shelf=singleplayer] a[data-game=th05]');await card.click();await page.waitForTimeout(400);if(!await page.locator('#advancedOptions > summary').isVisible())await card.click();
 const toggle=async(id,value)=>{const node=page.locator('#'+id);if((await node.getAttribute('aria-checked')==='true')!==value)await node.click();};
 await toggle('touchToggle',true);await page.locator('#touchLayoutEdit').click();await toggle('thpracTouchControlsToggle',true);await page.locator('#touchLayoutSave').click();await page.locator('#touchLayoutExit').click();await page.locator('#languageSelect').evaluate(node=>{node.value='lang_zh-hans';node.dispatchEvent(new Event('change',{bubbles:true}));});
 if(!await page.locator('#advancedOptions').evaluate(node=>node.open))await page.locator('#advancedOptions > summary').click();
 await page.locator('#musicOption .mizuki-select-trigger').click();await page.locator('[role=option][data-value='+(musicRoom?'ogg-full':'none')+']:visible').click();await toggle('thpracToggle',true);
 await page.locator('#launch').click();if(!musicRoom)await page.getByRole('button',{name:'仍要启动',exact:true}).click();await page.waitForFunction(()=>document.querySelector('#gameFrame')?.contentWindow?.Module?._th05_ticks()>10,null,{timeout:90000});
 frame=page.frames().find(frame=>frame.url().includes('/runtime/th05/'));assert.ok(frame);if(await page.locator('#touchHelpClose').isVisible())await page.locator('#touchHelpClose').tap();
 assert.equal(await page.locator('#touchDirectSurface').isVisible(),true,'native touch surface');
 const advance=async count=>frame.evaluate(count=>{for(let n=0;n<count;n++)Module._th05_step();},count);
 const pointer=async(type,id,x,y)=>page.locator('#touchDirectSurface').evaluate((surface,{type,id,x,y,ios})=>{
  const r=document.querySelector('#gameFrame').getBoundingClientRect(),scale=Math.min(r.width/640,r.height/480),clientX=r.left+(r.width-640*scale)/2+x*scale,clientY=r.top+(r.height-480*scale)/2+y*scale;
  if(ios){const contact=new Touch({identifier:id,target:surface,clientX,clientY});surface.dispatchEvent(new TouchEvent({down:'touchstart',move:'touchmove',up:'touchend'}[type],{bubbles:true,cancelable:true,touches:type==='up'?[]:[contact],changedTouches:[contact]}));}
  else surface.dispatchEvent(new PointerEvent({down:'pointerdown',move:'pointermove',up:'pointerup'}[type],{pointerId:id,pointerType:'touch',clientX,clientY,bubbles:true,cancelable:true,buttons:type==='up'?0:1}));
 },{type,id,x,y,ios});
 const tap=async(x,y)=>{await pointer('down',17,x,y);await advance(3);await pointer('up',17,x,y);await page.waitForTimeout(90);await advance(3);};
 console.log('launched',profile);await advance(450);assert.equal(await frame.evaluate(()=>Module._th05_scene()),2);
 if(musicRoom){
  const press=async key=>frame.evaluate(key=>{Module._th05_key(key,1);Module._th05_step();Module._th05_step();Module._th05_key(key,0);Module._th05_step();},key);
  for(let n=0;n<3;n++)await press(38);await press(90);await advance(90);assert.equal(await frame.evaluate(()=>Module._th05_scene()),8,'Music Room');
  const collections=[];
  for(const [game,track,path] of [[4,'G_OP.M2','room-g_op.ogg'],[3,'Y_OP.M2','room-y_op.ogg'],[2,'H_OP.M2','room-h_op.ogg'],[1,'R_00.M2','room-r_00.ogg']]){
   await press(37);await frame.waitForFunction(({track,path})=>{const health=Module.eaglerAudio.health();return health.track===track&&health.path==='/music/'+path&&health.playing;},{track,path},{timeout:180000});
   const health=await frame.evaluate(()=>Module.eaglerAudio.health());collections.push({game,...health});assert.equal(health.sfx.backend,'uth05win-original-se');
  }
  await frame.waitForFunction(()=>Module.FS.readdir('/music').filter(name=>name.startsWith('room-')&&name.endsWith('.ogg')).length===84,null,{timeout:180000});
  const decode=await frame.evaluate(async()=>{const names=Module.FS.readdir('/music').filter(name=>name.startsWith('room-')&&name.endsWith('.ogg'));const context=new AudioContext();let minPeak=1;for(const name of names){const bytes=Module.FS.readFile('/music/'+name),audio=await context.decodeAudioData(bytes.buffer.slice(bytes.byteOffset,bytes.byteOffset+bytes.byteLength));let peak=0;for(const channel of [0,1])if(channel<audio.numberOfChannels){const pcm=audio.getChannelData(channel);for(let at=0;at<pcm.length;at+=113)peak=Math.max(peak,Math.abs(pcm[at]));}if(peak<.001)throw Error('Silent Music Room track: '+name);minPeak=Math.min(minPeak,peak);}await context.close();return {tracks:names.length,minPeak};});
  assert.equal(decode.tracks,84);await page.screenshot({path:resolve(ops,label+'.png')});const result={passed:true,staged,profile,collections,decode,errors};assert.deepEqual(errors,[]);await writeFile(resolve(ops,label+'.json'),JSON.stringify(result,null,2)+'\n');console.log(JSON.stringify(result));await browser.close();process.exit(0);
 }

 await tap(350,300);await advance(90);assert.equal(await frame.evaluate(()=>Module._th05_scene()),3,'touch confirms title');
 await pointer('down',41,320,240);await pointer('down',42,350,240);await pointer('up',41,320,240);await pointer('up',42,350,240);await page.waitForTimeout(400);await advance(10);assert.equal(await frame.evaluate(()=>Module._th05_scene()),2,'cold two-finger back');
 const press=async key=>frame.evaluate(key=>{Module._th05_key(key,1);Module._th05_step();Module._th05_step();Module._th05_key(key,0);Module._th05_step();},key);
 await advance(90);for(let n=0;n<3;n++)await press(38);await press(90);await advance(90);assert.equal(await frame.evaluate(()=>Module._th05_scene()),8);
 const start=await frame.evaluate(()=>({time:performance.now(),ticks:Module._th05_ticks()}));await page.waitForTimeout(8000);
 const perf=await frame.evaluate(()=>({time:performance.now(),ticks:Module._th05_ticks(),fps:Module._th05_display_fps(),draws:Module._th05_graphics_stat(0),font:Module.eaglerFontStats}));perf.tickRate=(perf.ticks-start.ticks)*1000/(perf.time-start.time);assert.ok(perf.tickRate>35,JSON.stringify(perf));assert.ok(perf.draws<100);assert.equal(perf.font.simplifiedGlyphs,1829);await page.screenshot({path:resolve(ops,label+'-music-room.png')});
 await press(27);await advance(80);for(let n=0;n<3;n++)await press(40);
 await tap(350,300);await advance(90);assert.equal(await frame.evaluate(()=>Module._th05_scene()),3);
 await tap(350,300);await advance(140);assert.equal(await frame.evaluate(()=>Module._th05_scene()),4,'touch confirms character');
 await advance(100);const before=await frame.evaluate(()=>({x:Module._th05_player_x(),y:Module._th05_player_y()}));
 await pointer('down',29,220,300);await pointer('move',29,260,280);await page.waitForTimeout(90);await advance(3);const after=await frame.evaluate(()=>({x:Module._th05_player_x(),y:Module._th05_player_y()}));await pointer('up',29,260,280);
 assert.ok(after.x>before.x+20,JSON.stringify({before,after}));assert.ok(after.y<before.y-10,JSON.stringify({before,after}));
 await page.locator('#touchThpracBackspace').tap();await advance(4);assert.equal((await frame.evaluate(()=>Module._th05_mobile_state()))&3,3,'launcher opens native mobile panels');
 await tap(45,20);assert.ok((await frame.evaluate(()=>Module._th05_mobile_state()))&4,'native cheat panel unfolds');
 const tools=[];
 for(let index=0;index<8;index++){const before=await frame.evaluate(index=>Module._th05_practice_tool(index),index);await tap(35,59+index*32);const after=await frame.evaluate(index=>Module._th05_practice_tool(index),index);tools.push({index,before,after});assert.notEqual(after,before,'native cheat '+index);}
 await page.screenshot({path:resolve(ops,label+'-cheats.png')});
 await tap(45,20);await tap(190,20);assert.ok((await frame.evaluate(()=>Module._th05_mobile_state()))&8,'native advanced panel unfolds');await page.screenshot({path:resolve(ops,label+'-advanced.png')});
 await tap(50,20);await page.locator('#touchThpracBackspace').tap();await advance(4);assert.equal((await frame.evaluate(()=>Module._th05_mobile_state()))&2,0,'launcher hides native panels');
 const runtimeErrors=await page.evaluate(()=>window.__th05Events.filter(e=>e.event==='error'));assert.deepEqual(errors,[]);assert.deepEqual(runtimeErrors,[]);
 const audio=await frame.evaluate(()=>Module.eaglerAudio.health());assert.equal(audio.sfx.backend,'uth05win-original-se');const result={passed:true,staged,profile,perf,audio,before,after,tools,runtimeErrors,errors};await writeFile(resolve(ops,label+'.json'),JSON.stringify(result,null,2)+'\n');console.log(JSON.stringify(result));
}catch(error){await page.screenshot({path:resolve(ops,label+'-failure.png')});console.error((await page.locator('body').innerText()).slice(-2000));throw error;}finally{await browser.close();}
