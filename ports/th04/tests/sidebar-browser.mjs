import assert from 'node:assert/strict';
import {createServer} from 'node:http';
import {readFile} from 'node:fs/promises';
import {createRequire} from 'node:module';
import {findChromiumExecutable} from '../../../launcher/lib/chromium-executable.mjs';
const require=createRequire(new URL('../../../launcher/package.json',import.meta.url)),puppeteer=require('puppeteer-core');
const source=await readFile(new URL('../web/frame-presenter.js',import.meta.url));
const server=createServer((req,res)=>{if(req.url==='/frame-presenter.js'){res.setHeader('content-type','text/javascript');res.end(source);}else {res.setHeader('content-type','text/html');res.end('<canvas width="640" height="400"></canvas>');}});
await new Promise(r=>server.listen(0,'127.0.0.1',r));let browser;
try{
 browser=await puppeteer.launch({executablePath:await findChromiumExecutable(),headless:true,args:['--enable-unsafe-swiftshader']});
 const page=await browser.newPage();await page.goto(`http://127.0.0.1:${server.address().port}/`);
 const pixels=await page.evaluate(async()=>{
  const {installFramePresenter}=await import('/frame-presenter.js'),canvas=document.querySelector('canvas');
  const gl=canvas.getContext('webgl',{preserveDrawingBuffer:true}),heap=new Uint8Array(640*400*3+12000),palette=640*400*3+11000;
  const mod={gl,HEAPU8:heap,sidebarPaletteAddress:palette},presenter=installFramePresenter(mod,{deferred:true});
  const sample=(x,y)=>{const p=new Uint8Array(4);gl.readPixels(x,y,1,1,gl.RGBA,gl.UNSIGNED_BYTE,p);return [...p];};
  const cases=[];
  for(const [tone,rgb] of [[100,[255,138,32]],[50,[117,69,16]],[150,[255,207,154]]]){
   new DataView(heap.buffer).setInt16(palette-0x26de,tone,true);heap.set([240,128,32],palette);
   for(let i=0;i<640*400;i++)heap.set(rgb,i*3);
   for(let y=0;y<400;y++)for(let x=500;x<510;x++)heap.set([20,240,250],(y*640+x)*3);
   mod.updateTexture(0,640,400);new DataView(heap.buffer).setInt16(palette-0x26de,0,true);presenter.flush();
   cases.push({background:sample(470,200),foreground:sample(505,200),field:sample(200,200)});
  }
  return cases;
 });
 for(const [i,row] of pixels.entries()){
  assert.deepEqual(row.background,[0,0,0,255],`tone ${i} sidebar background`);
  assert.deepEqual(row.foreground,[20,240,250,255],`tone ${i} HUD foreground`);
  assert.notDeepEqual(row.field,[0,0,0,255],`tone ${i} playfield`);
 }
 console.log('TH04 WebGL sidebar background, foreground and playfield passed at three tones');
}finally{await browser?.close();server.close();}


