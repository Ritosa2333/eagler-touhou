import {chromium} from 'file:///D:/workspace/东方测试.zip/TouhouDev/th10_web/node_modules/playwright/index.mjs';
import {readFileSync} from 'node:fs';
import assert from 'node:assert/strict';
const browser=await chromium.launch({executablePath:'C:/Program Files (x86)/Microsoft/Edge/Application/msedge.exe',headless:true});
try{
 for(const game of ['th08','th10']){
  const context=await browser.newContext({viewport:{width:800,height:600},hasTouch:true,isMobile:true});
  const page=await context.newPage();
  await page.setContent('<meta name="viewport" content="width=device-width,initial-scale=1"><style>html,body{margin:0;width:100%;height:100%}canvas{position:absolute;left:80px;top:60px;width:640px;height:480px;touch-action:none}</style><canvas></canvas>');
  const source=readFileSync('D:/workspace/eagler/worktrees/'+game+'-eagler-portable/'+game+'_web/sdl-runtime/eagler-host.mjs','utf8');
  await page.evaluate(async source=>{
   const {bindOutsideTouches}=await import(URL.createObjectURL(new Blob([source],{type:'text/javascript'})));
   window.calls=[];window.native=[];
   const canvas=document.querySelector('canvas');
   for(const type of ['pointerdown','pointermove','pointerup'])canvas.addEventListener(type,e=>native.push(e.type));
   const cancel=bindOutsideTouches(document,canvas,()=>({sdl_touch:(...a)=>calls.push(a),sdl_touch_cancel:()=>calls.push(['cancel'])}),()=>true);
   window.addEventListener('blur',cancel);
  },source);
  const cdp=await context.newCDPSession(page);
  const touch=async(type,x,y)=>{await cdp.send('Input.dispatchTouchEvent',{type,touchPoints:type==='touchEnd'?[]:[{x,y,id:1}]});};
  await touch('touchStart',40,300);await touch('touchMove',160,300);await touch('touchEnd');
  const result=await page.evaluate(()=>({calls,native}));
  assert.deepEqual(result.calls.map(c=>c[0]),[0,1,2]);assert.equal(result.native.length,0);
  assert.equal(result.calls[0][2],-.0625);assert.equal(result.calls[1][2],.125);
  await page.evaluate(()=>{calls.length=0;native.length=0;});
  await touch('touchStart',200,300);await touch('touchEnd');
  assert.equal(await page.evaluate(()=>calls.length),0);
  assert.deepEqual(await page.evaluate(()=>native),['pointerdown','pointerup']);
  await touch('touchStart',40,300);await page.evaluate(()=>window.dispatchEvent(new Event('blur')));
  await touch('touchMove',45,310);await touch('touchEnd');
  assert.deepEqual(await page.evaluate(()=>calls.map(c=>c[0])),[0,'cancel']);
  console.log(game+': browser letterbox drag, native ownership, blur PASS');
  await context.close();
 }
}finally{await browser.close();}
