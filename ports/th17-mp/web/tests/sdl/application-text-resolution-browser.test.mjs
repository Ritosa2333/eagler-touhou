import test from 'node:test';
import assert from 'node:assert/strict';
import {readFile,mkdir} from 'node:fs/promises';
import {resolve} from 'node:path';
import {launchBrowser} from '../../../../../tools/browser/launch.mjs';
import {applicationServer,applicationPage} from './application-fixture.mjs';
import {root,sha,report} from '../cpp/helpers.mjs';

test('Small text uses the original resolution-specific bitmap across title and gameplay screens',{timeout:360000},async()=>{
 const wasmSha256=sha(await readFile(resolve(root,'artifacts/sdl-application/th17-application.wasm'))),server=await applicationServer(),observations=[];
 await mkdir(resolve(root,'artifacts/browser'),{recursive:true});
 try{
  for(const scale of [1,2]){
   let browser;const errors=[];
   try{
    browser=await launchBrowser();const page=await applicationPage(browser,server,scale);page.on('pageerror',e=>errors.push(String(e)));
    const atlas=scale===2?'ascii_1280.anm':'ascii.anm',bytes=await readFile(resolve(root,'reference/assets',atlas)),texture=bytes.readUInt32LE(28),size=bytes.readUInt32LE(texture+12),pixels=bytes.subarray(texture+16,texture+16+size),dimension=256*scale;
    assert.equal(bytes.toString('ascii',texture,texture+4),'THTX');assert.equal(pixels.length,dimension*dimension*2);
    const image=await page.evaluate(async()=>{const c=fixture,p=c._application_test_ascii_image();if(!p)throw Error('ASCII image missing');const metadata=Array.from(c.HEAPU32.subarray(p>>>2,(p>>>2)+8)),address=c._application_test_ascii_pixels();if(!address)throw Error('ASCII pixels missing');const digest=await crypto.subtle.digest('SHA-256',c.HEAPU8.slice(address,address+metadata[6]));return{metadata,sha256:Array.from(new Uint8Array(digest),b=>b.toString(16).padStart(2,'0')).join('')};});
    assert.deepEqual(image.metadata,[256,256,dimension,dimension,dimension*2,5,size,293],'Logical bank stays in logical pixels while the actual GPU image follows the selected resolution');
    assert.equal(image.sha256,sha(pixels),'Actual texture bytes must equal the native resolution-specific bitmap');
    const sprites=await page.evaluate(()=>{const c=fixture,p=c._application_test_ascii_sprites(),d=new DataView(c.HEAPU8.buffer);return Array.from({length:293},(_,i)=>{const at=p+i*40;return{source:d.getUint32(at,true),texture:d.getUint32(at+4,true),rect:Array.from({length:4},(_,j)=>d.getFloat32(at+8+j*4,true)),uv:Array.from({length:4},(_,j)=>d.getFloat32(at+24+j*4,true))};});});
    let base=0,bank=0,index=0;do{const width=bytes.readUInt16LE(base+10),height=bytes.readUInt16LE(base+12),factor=bank===0?scale:1;for(let i=0;i<bytes.readUInt16LE(base+4);i++){const at=base+bytes.readUInt32LE(base+64+i*4),rect=Array.from({length:4},(_,j)=>bytes.readFloatLE(at+4+j*4));assert.deepEqual(sprites[index],{source:bytes.readUInt32LE(at),texture:bank,rect:rect.map(v=>v/factor),uv:[rect[0]/width,rect[1]/height,(rect[0]+rect[2])/width,(rect[1]+rect[3])/height]},'Native rectangle and UV '+index);index++;}const stride=bytes.readUInt32LE(base+36);if(!stride)break;base+=stride;bank++;}while(base<bytes.length);assert.equal(index,293);
    assert.deepEqual(await page.locator('canvas').evaluate(c=>[c.width,c.height]),[640*scale,480*scale]);
    const screens=[];
    const capture=async name=>{const stats=await page.evaluate(()=>{const c=fixture,p=c._application_graphics_stats()>>>2;return{state:state(),graphics:Array.from(c.HEAPU32.subarray(p,p+17))};});assert.equal(stats.graphics[7],0,name+' graphics errors');const path=resolve(root,'artifacts/browser/text-'+scale+'x-'+name+'.png');await page.locator('canvas').screenshot({path});screens.push({name,...stats,screenshot:path,screenshotSha256:sha(await readFile(path))});};
    await page.evaluate(()=>{choose(4,12);settle(30);});await capture('replay');
    if(scale===2){
     await page.evaluate(()=>{step(0x102);ready(1);settle(30);fixture._application_unlock();choose(2,5);choose(0,6);choose(0,7);choose(1,8);settle(30);});await capture('practice');
     await page.evaluate(()=>{for(const screen of [7,6,5,1]){step(0x102);ready(screen);settle(15);}choose(5,10);choose(0,11);settle(30);});await capture('ranking');
     await page.evaluate(()=>{step(0x102);ready(10);choose(1,11);settle(30);});await capture('spell-data');
     await page.evaluate(()=>{step(0x102);ready(10);step(0x102);ready(1);settle(30);choose(0,5);choose(0,6);choose(0,7);fixture._application_title_select(0);step(1);waitFor(()=>state()[0]===1);settle(120);});await capture('game-hud');
     await page.evaluate(()=>{step(0x100);waitFor(()=>pause()[1]===6);settle(20);});await capture('pause');
     assert.deepEqual(await page.evaluate(()=>{const c=fixture,p=c._application_test_ascii_image()>>>2;return Array.from(c.HEAPU32.subarray(p,p+8));}),image.metadata,'Shared high-resolution text texture survives title-to-game and pause transitions');
    }
    assert.deepEqual(errors,[]);observations.push({scale,atlas,...image,screens});
   }finally{await browser?.close();}
  }
  report('sdl-application-text-resolution',{passed:true,wasmSha256,observations,scope:'Actual SDL3/WebGL2 application at 640x480 and 1280x960. Every uploaded ASCII atlas byte and all 293 sprite rectangles/UVs match the corresponding original resource, normalized to logical pixels without changing scripts. Screenshots cover Replay at both resolutions and Practice, ranking, spell statistics, gameplay HUD and pause at 1280x960. This is a font-source and rendering-path check, not exhaustive native screenshot parity or physical phone performance.'});
 }finally{await server.close();}
});
