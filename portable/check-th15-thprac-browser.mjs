// Private-resource actual Runtime regression; not a native/device parity claim.
import assert from 'node:assert/strict';
import {createServer} from 'node:http';
import {readFile,readdir,writeFile} from 'node:fs/promises';
import {resolve} from 'node:path';
import {inflateSync} from 'node:zlib';
import {launchBrowser} from '../th10_web/scripts/native/browser-launch.mjs';
const root=resolve(import.meta.dirname,'../th15_web');
const build=resolve(root,'artifacts/thprac-purple-browser-check'),assets=resolve(root,'artifacts/thprac-private-assets');
const fonts=(await readdir(resolve(assets,'fonts'))).filter(n=>n.endsWith('.bin'));
// Inspect the presented canvas, not the pre-ImGui game render target.
function greenPixels(png){
 let width=0,height=0,channels=0;const data=[];
 for(let at=8;at<png.length;){const length=png.readUInt32BE(at),type=png.toString('ascii',at+4,at+8),body=png.subarray(at+8,at+8+length);at+=length+12;
  if(type==='IHDR'){width=body.readUInt32BE(0);height=body.readUInt32BE(4);assert.equal(body[8],8);channels=body[9]===6?4:body[9]===2?3:0;assert.ok(channels);assert.equal(body[12],0);}
  if(type==='IDAT')data.push(body);
 }
 const raw=inflateSync(Buffer.concat(data)),stride=width*channels;let previous=Buffer.alloc(stride),count=0;
 const paeth=(a,b,c)=>{const p=a+b-c,pa=Math.abs(p-a),pb=Math.abs(p-b),pc=Math.abs(p-c);return pa<=pb&&pa<=pc?a:pb<=pc?b:c;};
 for(let y=0;y<height;y++){const filter=raw[y*(stride+1)],row=Buffer.alloc(stride);assert.ok(filter<=4);
  for(let x=0;x<stride;x++){const a=x>=channels?row[x-channels]:0,b=previous[x],c=x>=channels?previous[x-channels]:0;
   row[x]=(raw[y*(stride+1)+1+x]+(filter===1?a:filter===2?b:filter===3?Math.floor((a+b)/2):filter===4?paeth(a,b,c):0))&255;
  }
  for(let x=0;x<stride;x+=channels)if(row[x]<50&&row[x+1]>220&&row[x+2]<100)count++;
  previous=row;
 }
 return count;
}
const files=new Map([['/module.mjs',resolve(build,'th15-application.mjs')],['/th15-application.wasm',resolve(build,'th15-application.wasm')],['/th15.dat',resolve(assets,'th15.dat')],['/unifont.otf',resolve(assets,'unifont.otf')],...fonts.map(n=>['/fonts/'+n,resolve(assets,'fonts',n)])]);
const html='<!doctype html><canvas id="canvas" width="640" height="480"></canvas><script type="module">import create from "/module.mjs";window.fixture=await create({canvas:document.querySelector("canvas"),eaglerOptions:{thpracEnabled:true,thpracLocale:"en"}});</script>';
const server=createServer(async(req,res)=>{try{const p=new URL(req.url,'http://localhost').pathname;res.setHeader('Cross-Origin-Opener-Policy','same-origin');res.setHeader('Cross-Origin-Embedder-Policy','require-corp');if(p==='/'){res.setHeader('Content-Type','text/html');res.end(html);return;}if(!files.has(p)){res.writeHead(404).end();return;}res.setHeader('Content-Type',p.endsWith('.mjs')?'text/javascript':p.endsWith('.wasm')?'application/wasm':'application/octet-stream');res.end(await readFile(files.get(p)));}catch(e){res.writeHead(500).end(String(e));}});
await new Promise(ok=>server.listen(0,'127.0.0.1',ok));let browser;const errors=[],observations=[];
try{
 browser=await launchBrowser();const page=await browser.newPage({viewport:{width:900,height:650}});page.on('pageerror',e=>errors.push(String(e)));
 await page.goto('http://127.0.0.1:'+server.address().port+'/');await page.waitForFunction(()=>window.fixture,{timeout:90000});
 await page.evaluate(async fonts=>{const c=fixture;c.FS.mkdir('/fonts');c.FS.mkdir('/save');for(const p of ['/th15.dat','/unifont.otf',...fonts.map(n=>'/fonts/'+n)]){const r=await fetch(p);if(!r.ok)throw Error('Missing '+p);c.FS.writeFile(p,new Uint8Array(await r.arrayBuffer()));}
 window.decode=p=>new TextDecoder().decode(c.HEAPU8.subarray(p,c.HEAPU8.indexOf(0,p)));window.state=()=>Array.from(c.HEAP32.subarray(c._application_state()/4,c._application_state()/4+16));window.prac=()=>Array.from(c.HEAP32.subarray(c._application_practice_state()/4,c._application_practice_state()/4+12));
 c._application_test_music_enabled(0);if(!c._application_initialize(0)||!c._application_practice_initialize())throw Error(decode(c._application_error()));
 window.step=(held=0,pressed=0)=>{if(!c._application_step(held,pressed,0,60,0))throw Error(decode(c._application_error())+' state='+state());c._application_practice_frame();return state();};window.frames=n=>{for(let i=0;i<n;i++)step();};
 window.key=(vk,bits=0)=>{c._application_keyboard_key(vk,1);step(bits,bits);c._application_keyboard_key(vk,0);step();};
 window.waitMenu=(screen,limit=350)=>{for(let i=0;i<limit;i++){const s=step();if(s[0]===0&&s[8]===screen&&s[9]===2)return s;}throw Error('Menu timeout '+screen+' '+state());};window.choose=(cursor,screen)=>{c._application_title_select(cursor);key(90,1);return waitMenu(screen);};waitMenu(1);
 },fonts);
 observations.push(await page.evaluate(()=>{key(123);const opened=prac();key(123);const closed=prac();if(!opened[4]||closed[4])throw Error('F12 lifecycle');return {kind:'F12',opened,closed};}));
 if(process.argv.includes('--tools-only')){
 await page.evaluate(()=>{window.tap=(x,y)=>{fixture._application_practice_mouse(0,x,y);step();fixture._application_practice_mouse(1,x,y);step();fixture._application_practice_mouse(2,x,y);step();};key(123);tap(80,119);});
 await page.locator('canvas').screenshot({path:resolve(build,'advanced-reaction-start.png')});
 await page.evaluate(()=>key(90));
 for(let trial=0;trial<5;trial++){
  let green=0;
  for(let poll=0;poll<100;poll++){
   await page.evaluate(()=>step());green=greenPixels(await page.locator('canvas').screenshot());
   if(green>10000)break;
   await page.waitForTimeout(50);
  }
  if(green<=10000)await page.locator('canvas').screenshot({path:resolve(build,'advanced-reaction-missing-signal.png')});
  assert.ok(green>10000,'Reaction signal did not turn green, trial '+trial+' pixels '+green);
  await page.evaluate(()=>{key(37);frames(2);});
  await page.locator('canvas').screenshot({path:resolve(build,'advanced-reaction-result-'+trial+'.png')});
  if(trial<4)await page.evaluate(()=>key(90));
 }
 observations.push({kind:'Source reaction keydown test',completedTrials:5,signal:'real rendered green rectangle; real raw arrow key'});
 await page.evaluate(()=>{key(90);tap(516,148);key(90);});
 for(let trial=0;trial<5;trial++){
  await page.evaluate(()=>{fixture._application_keyboard_key(37,1);step();});let green=0;
  for(let poll=0;poll<100;poll++){
   await page.evaluate(()=>step());green=greenPixels(await page.locator('canvas').screenshot());if(green>10000)break;await page.waitForTimeout(50);
  }
  assert.ok(green>10000,'Reaction keyup signal missing, trial '+trial);
  await page.evaluate(()=>{fixture._application_keyboard_key(37,0);step();frames(2);});
  await page.locator('canvas').screenshot({path:resolve(build,'advanced-reaction-keyup-'+trial+'.png')});
  if(trial<4)await page.evaluate(()=>key(90));
 }
 observations.push({kind:'Source reaction keyup test',completedTrials:5,signal:'real rendered green rectangle; held/released raw arrow key'});
 await page.evaluate(()=>{tap(80,119);key(123);});
 }
 if(process.argv.includes('--tools-only')){assert.deepEqual(errors,[]);console.log(JSON.stringify({passed:true,observations}));await page.evaluate(()=>fixture._application_close());process.exitCode=0;}else{
 await page.evaluate(()=>{choose(2,6);choose(1,7);choose(0,9);frames(20);if(!prac()[1])throw Error('Real Practice overlay missing '+prac());});
 await page.locator('canvas').screenshot({path:resolve(build,'practice-before-cancel.png')});
 observations.push(await page.evaluate(()=>{key(27,256);frames(10);if(prac()[1]||prac()[7])throw Error('Cancel stale owner '+prac());waitMenu(7);return {kind:'Practice cancel',state:state(),practice:prac()};}));
 await page.evaluate(()=>{choose(0,9);frames(20);window.tap=(x,y)=>{fixture._application_practice_mouse(0,x,y);step();fixture._application_practice_mouse(1,x,y);step();fixture._application_practice_mouse(2,x,y);step();};tap(525,172);frames(2);});
 await page.locator('canvas').screenshot({path:resolve(build,'practice-warp-popup.png')});
 await page.evaluate(()=>{tap(300,262);frames(2);});
 await page.locator('canvas').screenshot({path:resolve(build,'practice-selected-warp.png')});
 await page.evaluate(()=>{if(prac()[9]!==4)throw Error('Pointer Boss section selection failed '+prac());key(90,1);for(let i=0;i<240&&state()[0]!==1;i++)step();if(state()[0]!==1||!prac()[7])throw Error('Practice not active '+state()+' '+prac());fixture._application_test_protection(900);frames(40);});
 await page.locator('canvas').screenshot({path:resolve(build,'practice-live.png')});
 observations.push(await page.evaluate(()=>{if(!fixture._application_practice_save())throw Error('Production PRAC save failed '+decode(fixture._application_error()));const files=fixture.FS.readdir('/save/replay').filter(n=>n.endsWith('.rpy'));if(files.length!==1)throw Error('Expected one replay '+files);return {kind:'Unassisted production PRAC save',files};}));
 observations.push(await page.evaluate(()=>{key(9);if(!prac()[5])throw Error('Tab tracker missing');key(8);key(112);if(!prac()[11])throw Error('Assisted owner missing');key(123);const p=prac();if(!p[4]||!fixture._application_practice_pointer(100,100))throw Error('F12 pointer carrier');return {kind:'Live Tab/Backspace/F1/F12',state:state(),practice:p};}));
 await page.evaluate(()=>{if(fixture._application_practice_save())throw Error('Assisted save accepted');key(8);tap(100,73);frames(2);tap(411,97);const p=fixture._application_practice_options()/4;if(!fixture.HEAP32[p+2])throw Error('F12 disable Z checkbox did not mutate input owner');tap(411,97);});
 await page.locator('canvas').screenshot({path:resolve(build,'advanced-gameplay.png')});
 await page.evaluate(()=>{tap(100,70);tap(100,95);frames(2);});
 await page.locator('canvas').screenshot({path:resolve(build,'advanced-secret.png')});
 observations.push(await page.evaluate(()=>{tap(45,122);const options=()=>Array.from(fixture.HEAP32.subarray(fixture._application_practice_options()/4,fixture._application_practice_options()/4+13));if(!options()[12])throw Error('Flip checkbox owner');const before=new Float32Array(fixture.HEAPU8.buffer)[fixture._application_test_lifecycle()/4+1];for(let i=0;i<3;i++)step(16,i===0?16:0);const after=new Float32Array(fixture.HEAPU8.buffer)[fixture._application_test_lifecycle()/4+1];if(!(after>before))throw Error('Native flipped movement sign '+before+' '+after);tap(45,358);if(options()[12])throw Error('Mirrored pointer cannot restore flip');tap(18,146);frames(3);tap(320,146);frames(2);tap(18,146);return {kind:'Secret flip native movement and mirrored pointer',before,after,restored:!options()[12]};}));
 await page.evaluate(()=>key(123));
 await page.locator('canvas').screenshot({path:resolve(build,'practice-tracker.png')});
 observations.push(await page.evaluate(()=>{key(27,256);frames(30);key(81,0x10000);waitMenu(1);choose(3,12);fixture._application_title_select(0);key(90,1);frames(18);if(state()[9]!==4)throw Error("Replay stage chooser missing "+state());key(90,1);for(let i=0;i<240&&state()[0]!==1;i++)step();const p=prac();if(state()[0]!==1||!p[8]||p[10]!==4)throw Error('Native replay selection failed '+state()+' '+p);frames(20);return {kind:'Native saved PRAC playback',state:state(),practice:prac()};}));
 await page.evaluate(()=>{key(27,256);frames(30);key(81,0x10000);waitMenu(12);key(88,2);waitMenu(1);choose(2,6);choose(1,7);choose(0,9);frames(20);tap(480,148);tap(300,217);tap(480,200);frames(2);});
 await page.locator('canvas').screenshot({path:resolve(build,'practice-ab-popup.png')});
 observations.push(await page.evaluate(()=>{tap(300,368);if(prac()[9]!==74)throw Error('AB section unavailable '+prac());key(90,1);for(let i=0;i<240&&state()[0]!==1;i++)step();if(prac()[10]!==74||state()[2]!==3)throw Error('AB native entry failed '+state()+' '+prac());fixture._application_test_protection(900);frames(150);return {kind:'Purple AB authored ECL actual execution',state:state(),practice:prac()};}));
 await page.locator('canvas').screenshot({path:resolve(build,'practice-ab-live.png')});
 observations.push(await page.evaluate(()=>{frames(7200);return {kind:'Purple AB extended simulation',state:state(),practice:prac()};}));
 await page.locator('canvas').screenshot({path:resolve(build,'practice-ab-extended.png')});
 await page.evaluate(()=>{key(8);key(112);key(8);window.ab=()=>Array.from(fixture.HEAP32.subarray(fixture._application_practice_ab_state()/4,fixture._application_practice_ab_state()/4+3));});
 // The authored test has several phases; one death is not the result state.
 // Run bounded chunks so a real missing instruction is still a hard failure.
 let abResult;
 for(let chunk=0;chunk<20;chunk++){
  abResult=await page.evaluate(()=>{for(let i=0;i<1500&&ab()[1]<180;i++)step();return {kind:'Purple AB natural hit/result reveal',state:state(),ab:ab()};});
  console.log(JSON.stringify({chunk,...abResult}));
  if(abResult.ab[1]>=180)break;
 }
 assert.ok(abResult.ab[1]>=180,'AB result did not finish score animation '+JSON.stringify(abResult));observations.push(abResult);
 await page.locator('canvas').screenshot({path:resolve(build,'practice-ab-result.png')});
 assert.deepEqual(errors,[]);await page.evaluate(()=>fixture._application_close());
 await writeFile(resolve(build,'practice-browser.json'),JSON.stringify({passed:true,observations,scope:'Actual original DAT/font Runtime, native Practice entry/cancel/reentry/start; F12, Tab, assist state and pointer capture. Music disabled; no audio/native/device equivalence.'},null,2));console.log(JSON.stringify({passed:true,observations}));
 }
}finally{await browser?.close();await new Promise(ok=>server.close(ok));}
