import assert from 'node:assert/strict';
import {writeFileSync,readFileSync} from 'node:fs';
import {launchBrowser} from '../th10_web/scripts/native/browser-launch.mjs';
const browser=await launchBrowser();
try {
 const page=await browser.newPage(),errors=[];page.on('pageerror',e=>errors.push(e.stack));page.on('console',m=>{if(m.type()==='error')console.log(m.text());});
 if(process.env.TEST_SHADER)await page.addInitScript(source=>{const old=WebGL2RenderingContext.prototype.shaderSource;WebGL2RenderingContext.prototype.shaderSource=function(shader,text){return old.call(this,shader,text.includes('uniform sampler2D sourceImage,weights;')?source:text);};},readFileSync('portable/sdl/ResampleShader.hpp','utf8').split('resampleFragment=R\"GLSL(')[1].split(')GLSL\";')[0]);
 await page.goto('http://127.0.0.1:8095/runtime/th08/th08.html?manual=1');
 await page.waitForFunction(()=>window.__th08Runtime,null,{timeout:120000});await page.locator('canvas').click();
 await page.evaluate(()=>__th08Runtime.launch());
 const result=await page.evaluate(()=>{const c=__th08Runtime.core;return {warmMask:c.sdl_warm_assets?.(),validation:c.sdl_capture_validation(),mismatches:c.sdl_capture_mismatches(),stats:Array.from(new Uint32Array(c.memory.buffer,c.sdl_stats(),17))};});
 writeFileSync('th08_web/artifacts/architecture-candidate/validation/capture-driver.json',JSON.stringify({result,errors},null,2));
 console.log(JSON.stringify({result,errors}));assert.deepEqual(errors,[]);assert.equal(result.validation,1,'GPU output must match the original CPU filter on this test device');
}finally{await browser.close();}
