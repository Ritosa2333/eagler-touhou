import assert from 'node:assert/strict';
import {readFileSync,writeFileSync} from 'node:fs';
import {createHash} from 'node:crypto';
import {launchBrowser} from '../th10_web/scripts/native/browser-launch.mjs';
const game=process.argv.includes('--th08')?'th08':'th10',base='http://127.0.0.1:'+(game==='th08'?8095:8093),out=game+'_web/artifacts/architecture-candidate/validation',browser=await launchBrowser(),runs=[];
try{for(const baseline of [true,false]){
 const context=await browser.newContext({viewport:{width:800,height:600}}),page=await context.newPage(),errors=[];page.on('pageerror',e=>errors.push(e.stack));await page.addInitScript(()=>{Date.now=()=>100000;});
 if(baseline)for(const ext of ['wasm','mjs'])await page.route('**/'+game+'-sdl.'+ext,r=>r.fulfill({contentType:ext==='wasm'?'application/wasm':'text/javascript',body:readFileSync(game+'_web/artifacts/sdl-release/site/runtime/'+game+'/'+game+'-sdl.'+ext)}));
 await page.goto(base+'/runtime/'+game+'/'+game+'.html?manual=1');await page.waitForFunction(game=>window['__'+game+'Runtime'],game,{timeout:120000});
 await page.evaluate(async game=>{
  const r=window['__'+game+'Runtime'],c=r.core;let app;if(game==='th08'){await r.launch();app=r.app;}else app=c.sdl_game_open(0,Date.now()&65535);
  if(!app)throw Error('Application failed');c.sdl_touch_options(0,0,1);
  const text=game==='th08'?c.allocate(80):c.graphics_allocate(80),active=new Set();
  const tick=(wanted=[])=>{const keys=new Set(wanted);for(const key of new Set([...active,...keys]))if(active.has(key)!==keys.has(key)){new Uint8Array(c.memory.buffer,text,80).set(new TextEncoder().encode(key+'\0'));c.sdl_key(text,keys.has(key));}active.clear();for(const key of keys)active.add(key);const result=c.sdl_loop_tick(app,1/60,17);if(result)throw Error('Tick failed '+result);};
  const wait=n=>{for(let i=0;i<n;i++)tick();},press=key=>{for(let i=0;i<3;i++)tick([key]);wait(42);};
  const menu=()=>{if(game==='th08')return {scene:c.status(app,0),screen:c.status(app,8),cursor:c.status(app,9)};const p=c.application_title(app),v=new DataView(c.memory.buffer);return {scene:0,screen:v.getInt32(p+28,true),cursor:v.getInt32(p+36,true)};};
  const stats=()=>Array.from(new Uint32Array(c.memory.buffer,game==='th08'?c.preload_stats?.(app):c.application_preload_stats?.(app),game==='th08'?2:3));
  window.probe={r,c,app,tick,wait,press,menu,stats};wait(game==='th08'?100:330);if(game==='th10')press('KeyZ');
 },game);
 const pictures=[],timings=[],counts=[];
 for(let cycle=0;cycle<3;cycle++){
  const result=await page.evaluate(game=>{const p=probe,target=game==='th08'?6:5;for(let n=0;n<12&&p.menu().cursor!==target;n++)p.press('ArrowDown');if(p.menu().cursor!==target)throw Error('Music menu cursor');const uploaded=()=>new Uint32Array(p.c.memory.buffer,p.c.sdl_stats(),6)[2],before=uploaded();const begin=performance.now();p.press('KeyZ');p.wait(60);return {ms:performance.now()-begin,menu:p.menu(),stats:p.stats(),uploadBytes:uploaded()-before,residentHits:game==='th08'?p.c.resident_hits?.(p.app):p.c.application_resident_hits?.(p.app)};},game);
  assert.equal(game==='th08'?result.menu.scene:result.menu.screen,game==='th08'?8:14);timings.push(result);counts.push(result.stats);
  const png=await page.locator('canvas').screenshot();pictures.push(createHash('sha256').update(png).digest('hex'));writeFileSync(out+'/preload-'+(baseline?'baseline':'native')+'-'+cycle+'.png',png);
  await page.evaluate(()=>{probe.press('Escape');probe.wait(100);});
 }
 assert.equal(errors.length,0);runs.push({baseline,pictures,timings,counts,errors});await context.close();
}
 const spriteColorsFixed=process.argv.includes('--sprite-colors-fixed');
 if(spriteColorsFixed){
  assert.equal(game,'th08');
  const visual=JSON.parse(readFileSync('th08_web/artifacts/stage-visual/verified-transition.json'));
  assert.equal(visual.passed,true);assert.equal(visual.wasm,JSON.parse(readFileSync(out+'/../site/manifest.json')).execution.sha256);
  // 3.2's sprite submission ignored vertex alpha. Fixing the original
  // FlushVertexBuffer state also restores dimming of unselected music rows.
  // Keep the old screenshots for review, but do not require the old bug.
  assert.equal(new Set(runs[1].pictures).size,1,'Corrected menu pixels must be stable across all three cache/reload cycles');
 }else assert.deepEqual(runs[1].pictures,runs[0].pictures,'Menu pixels must match the previously released renderer');
 assert(runs[1].counts[0][0]>0);
 assert(runs[1].timings[0].residentHits>0,'Preloaded static textures must be reused');
 if(game==='th08'){
  const previous=runs[0].timings[2],current=runs[1].timings[2];
  if(previous.residentHits>0)assert(current.uploadBytes<=previous.uploadBytes,'Music Room must not regress the released resident texture cache');
  else assert(current.uploadBytes<previous.uploadBytes,'Music Room must upload fewer texture bytes than the old template-only cache');
 }
 if(game==='th08'){assert(runs[1].counts[0][0]>=4,'Transition animations remain prepared alongside gameplay assets');assert(runs[1].counts[2][1]>runs[1].counts[0][1],'Returning to the menu must reuse the prepared template');}
 else {assert(runs[1].counts[0][2]>=2,'Both title ANMs and any shared gameplay resources come from preload');assert.deepEqual(runs[1].counts[2],runs[1].counts[0],'TH10 keeps title animations alive throughout Music Room');}
 const manifest=JSON.parse(readFileSync(out+'/../site/manifest.json'));const result={passed:true,game,wasm:manifest.execution.sha256,checks:['three Music Room enter/return cycles',spriteColorsFixed?'restored original sprite color/alpha; exact pixels across cache/reload cycles (previous 3.2 screenshots retained for review)':'exact baseline canvas screenshots','prepared animation cache used with original scene lifetime','no browser errors'],runs};writeFileSync(out+'/transition-preload.json',JSON.stringify(result,null,2));console.log(JSON.stringify(result));
}catch(error){writeFileSync(out+'/transition-preload-failure.json',JSON.stringify({error:error.stack,runs},null,2));throw error;}finally{await browser.close();}
