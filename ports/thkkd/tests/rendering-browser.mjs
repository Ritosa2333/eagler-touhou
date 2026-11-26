import assert from 'node:assert/strict';
import {readFileSync,writeFileSync,mkdirSync} from 'node:fs';
import {resolve} from 'node:path';
import {fileURLToPath} from 'node:url';
import {productionBrowser,nativeState,wait} from './production-driver.mjs';

const args=Object.fromEntries(process.argv.slice(2).map(value=>{const at=value.indexOf('=');return [value.slice(2,at),value.slice(at+1)];}));
const output=resolve(args.output||fileURLToPath(new URL('../build/rendering-browser',import.meta.url)));
mkdirSync(output,{recursive:true});
const fixtures=JSON.parse(readFileSync(new URL('./fixtures/rendering.json',import.meta.url)));
const report={schema:'thkkd/rendering-browser/1',checks:[],display:[]};
const driver=await productionBrowser({url:args.url||'http://127.0.0.1:8174/',report,headed:args.headed==='1',nativeViewport:args['native-window']==='1',launchArgs:args['window-position']?[`--window-position=${args['window-position']}`,'--window-size=1200,900']:[]}),{page,browser}=driver;
try{
  await driver.library();const frame=await driver.start();
  report.hardware=await frame.evaluate(()=>{const gl=__thkkdRuntime.platform.renderer.gl,e=gl.getExtension('WEBGL_debug_renderer_info');return {userAgent:navigator.userAgent,dpr:devicePixelRatio,screen:{left:screen.availLeft,top:screen.availTop,width:screen.width,height:screen.height},renderer:e?gl.getParameter(e.UNMASKED_RENDERER_WEBGL):gl.getParameter(gl.RENDERER)};});
  if(args['native-window']==='1'){
    assert.ok(await page.evaluate(()=>!!document.fullscreenElement),'启动器自动进入实际浏览器全屏');
    report.nativeFullscreen=await frame.evaluate(()=>{const c=document.querySelector('canvas'),r=c.getBoundingClientRect();return {dpr:devicePixelRatio,screen:{left:screen.availLeft,top:screen.availTop,width:screen.width,height:screen.height},canvas:{width:c.width,height:c.height},physical:{width:r.width*devicePixelRatio,height:r.height*devicePixelRatio}};});
    assert.deepEqual(report.nativeFullscreen.canvas,{width:640,height:480});if(args['window-position']?.startsWith('-'))assert.ok(report.nativeFullscreen.screen.left<0,'在副屏验证实际浏览器全屏');
    report.checks.push('headed-secondary-monitor-native-DPI-fullscreen');
  }
  report.alpha=await frame.evaluate(fixtures=>{
    const p=__thkkdRuntime.platform,gl=p.renderer.gl,old=gl.getParameter(gl.FRAMEBUFFER_BINDING),oldTexture=gl.getParameter(gl.TEXTURE_BINDING_2D),fb=gl.createFramebuffer(),rows=[];
    try{gl.bindFramebuffer(gl.FRAMEBUFFER,fb);for(const fixture of fixtures){
      const graph=p.asset(fixture.source).graph;gl.bindTexture(gl.TEXTURE_2D,graph.texture);
      gl.framebufferTexture2D(gl.FRAMEBUFFER,gl.COLOR_ATTACHMENT0,gl.TEXTURE_2D,graph.texture,0);
      const pixels=fixture.pixels.map(({x,y,rgba})=>{const bytes=new Uint8Array(4);gl.readPixels(x,y,1,1,gl.RGBA,gl.UNSIGNED_BYTE,bytes);return {x,y,expected:rgba,actual:[...bytes]};});
      rows.push({source:fixture.source,pixels,min:gl.getTexParameter(gl.TEXTURE_2D,gl.TEXTURE_MIN_FILTER),mag:gl.getTexParameter(gl.TEXTURE_2D,gl.TEXTURE_MAG_FILTER)});
    }}finally{gl.bindFramebuffer(gl.FRAMEBUFFER,old);gl.bindTexture(gl.TEXTURE_2D,oldTexture);gl.deleteFramebuffer(fb);}return rows;
  },fixtures.straightAlphaPixels);
  for(const row of report.alpha){assert.equal(row.min,9729);assert.equal(row.mag,9729);for(const pixel of row.pixels)assert.deepEqual(pixel.actual,pixel.expected,`${row.source} (${pixel.x},${pixel.y}) 原始 RGBA`);}
  report.checks.push('actual-PNG-GPU-straight-alpha-and-bilinear-filter');
  report.font=await frame.evaluate(()=>{const p=__thkkdRuntime.platform,style=[...p.fonts.values()].find(f=>f.size===26);return {style,menuWidth:p.textRenderer.width('Game Start',style,26),glyphs:p.textRenderer.width('東方海恵堂 EX Ω',style,26)};});
  assert.equal(report.font.style.weight,300);assert.equal(report.font.style.edge,1);assert.ok(report.font.menuWidth>130&&report.font.glyphs>100);
  report.checks.push('native-menu-font-weight-outline-and-Unicode-layout');
  for(const viewport of [{width:1280,height:900,deviceScaleFactor:1},{width:1280,height:900,deviceScaleFactor:1.25},{width:1365,height:768,deviceScaleFactor:1.5},{width:1920,height:1080,deviceScaleFactor:2},{width:800,height:600,deviceScaleFactor:1},{width:390,height:844,deviceScaleFactor:3},{width:844,height:390,deviceScaleFactor:3}]){
    await page.setViewport(viewport);await wait(150);
    const sample=await frame.evaluate(()=>{const c=document.querySelector('canvas'),r=c.getBoundingClientRect(),dpr=devicePixelRatio;return {dpr,view:{width:innerWidth,height:innerHeight},buffer:{width:c.width,height:c.height},rect:{left:r.left,top:r.top,width:r.width,height:r.height},physical:{left:r.left*dpr,top:r.top*dpr,width:r.width*dpr,height:r.height*dpr}};});
    report.display.push(sample);assert.deepEqual(sample.buffer,{width:640,height:480});assert.ok(Math.abs(sample.physical.width/sample.physical.height-4/3)<.0002);
    for(const value of Object.values(sample.physical))assert.ok(Math.abs(value-Math.round(value))<.06,`画布边界对齐设备像素：${value}`);
    assert.ok(sample.rect.width<=sample.view.width+.1&&sample.rect.height<=sample.view.height+.1);
  }
  report.checks.push('seven-viewport-DPR-portrait-landscape-fits');
  await page.setViewport({width:1280,height:900,deviceScaleFactor:1.25});
  report.automaticFullscreen=await page.evaluate(()=>!!document.fullscreenElement);
  if(report.automaticFullscreen){await page.click('#fullscreenToggle');await page.waitForFunction(()=>!document.fullscreenElement);}
  await page.click('#fullscreenToggle');await page.waitForFunction(()=>!!document.fullscreenElement);
  report.fullscreen=await frame.evaluate(()=>{const c=document.querySelector('canvas'),r=c.getBoundingClientRect();return {buffer:{width:c.width,height:c.height},rect:{width:r.width,height:r.height},dpr:devicePixelRatio};});
  assert.deepEqual(report.fullscreen.buffer,{width:640,height:480});assert.ok(Math.abs(report.fullscreen.rect.width/report.fullscreen.rect.height-4/3)<.0002);
  await page.evaluate(()=>document.exitFullscreen());report.checks.push('launcher-fullscreen-with-native-buffer-and-aspect');
  report.readOnly=await frame.evaluate(()=>{const r=__thkkdRuntime,w=a=>r.core._thkkd_word(a),state=()=>[w(0xa74c08),w(0xb3e2e8),w(0xb43110),r.platform.presented,...r.platform.sounds.values()].map(v=>typeof v==='object'?v.playSerial:v),before=state();for(let i=0;i<120;i++)r.platform.renderer.present((i%10)/10);return {before,after:state()};});
  assert.deepEqual(report.readOnly.before,report.readOnly.after);report.checks.push('120-extra-presentations-do-not-step-or-play-audio');
  await page.screenshot({path:resolve(output,'scaled-title.png')});
  if(args.offline==='1'){await page.setOfflineMode(true);await driver.library();const offline=await driver.start();report.offline=await offline.evaluate(()=>({fontRenderer:!!__thkkdRuntime.platform.textRenderer,serviceWorker:!!navigator.serviceWorker.controller,first:__thkkdRuntime.status().first}));assert.deepEqual(report.offline,{fontRenderer:true,serviceWorker:true,first:true});report.checks.push('updated-renderer-and-fonts-load-offline');}
  assert.equal(report.errors.length,0);report.result='passed';console.log(JSON.stringify({result:report.result,checks:report.checks,menuWidth:report.font.menuWidth,displays:report.display.length}));
}catch(error){report.result='failed';report.error=error.stack;process.exitCode=1;console.error(error);await page.screenshot({path:resolve(output,'failure.png')}).catch(()=>{});}
finally{await driver.collect();writeFileSync(resolve(output,'result.json'),JSON.stringify(report,null,2));await browser.close();}
