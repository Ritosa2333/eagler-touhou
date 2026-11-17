// Execute tiny instruction budgets from one captured real-mode native machine.
const {chromium}=require('C:/Users/14915/.cache/codex-runtimes/codex-primary-runtime/dependencies/node/node_modules/playwright');
const fs=require('node:fs'),path=require('node:path'),assert=require('node:assert/strict');
process.env.NATIVE_SCHEDULER='1';process.env.NATIVE_CPU='0';
(async()=>{
 const browser=await chromium.launch({executablePath:'C:/Program Files (x86)/Microsoft/Edge/Application/msedge.exe',headless:true});
 try{
 const page=await browser.newPage(),errors=[];page.on('pageerror',e=>errors.push(e.stack));await require('./native_scheduler_qa.cjs').install(page);
 await page.route('**/netplay/runtime.js',async route=>{const response=await route.fetch();await route.fulfill({response,body:await response.text()+'\nglobalThis.cpuQA=()=>emulator;'});});
 await page.goto((process.env.BASE||'http://127.0.0.1:19870')+'/netplay/runtime.html');await page.waitForFunction(()=>window.th04Sync);
 await page.evaluate(async()=>{th04Sync.configure(0,0,2);await th04Sync.load({language:'cn',players:2,p1:0,p2:2,difficulty:3,lives:6,bombs:2});th04Sync.start();for(let i=0;i<180;i++){th04Sync.step([0,0],{frame:i});th04Sync.confirm(i+1);}});
 const cases=await page.evaluate(async()=>{
  const m=cpuQA().module,a=m.nativeExports(),control=a.thCpuState(0),guest=a.thCoreState(0),reg=a.thCoreState(2),seg=a.thCpuState(7),view=()=>new DataView(m.HEAPU8.buffer);
  const saved=m.HEAPU8.slice(),globals=Object.entries(a).filter(([k,v])=>k.startsWith('__rollback_g')&&v instanceof WebAssembly.Global).map(([k,v])=>[k,v.value]);
  const reset=()=>{m.HEAPU8.set(saved);for(const[k,v]of globals)a[k].value=v;};
  const hash=async bytes=>Array.from(new Uint8Array(await crypto.subtle.digest('SHA-256',bytes))).map(v=>v.toString(16).padStart(2,'0')).join('');
  const bytes=(p,n)=>Array.from(m.HEAPU8.subarray(p,p+n));
  const programs=[
   ['arithmetic',[0xb8,1,0,0xbb,3,0,0x01,0xd8,0x48,0x75,0xfb]],
   ['memory',[0xbe,0,0x20,0xb8,4,0,0x89,0x04,0x03,0x04,0x83,0xc6,2,0xeb,0xf7]],
   ['memory-page-cross',[0xbe,0xff,0x0f,0xb8,0x34,0x12,0x89,0x04,0x8b,0x1c,0xeb,0xf4]],
   ['memory32-page-cross',[0xbe,0xfd,0x0f,0x66,0xb8,0x12,0x34,0x56,0x78,0x66,0x89,0x04,0x66,0x8b,0x1c,0xeb,0xef]],
   ['stack-call',[0x50,0x5b,0xe8,2,0,0xeb,0xf9,0x43,0xc3]],
   ['stack-wrap',[0xbc,1,0,0xb8,0x34,0x12,0x50,0x5b,0x66,0x50,0x66,0x59,0xeb,0xf2]],
   ['rep-ram',[0xbe,0,0x20,0xbf,0,0x30,0xb9,32,0,0xf3,0xa5,0xeb,0xf3]],
   ['rep-vram',[0xb8,0,0xa8,0x8e,0xc0,0xbf,0,0,0xb9,8,0,0xf3,0xab,0xeb,0xf9]],
   ['operand32',[0x66,0xb8,1,0,0,0,0x66,0x40,0xeb,0xfc]],
   ['address32-fallback',[0x67,0x8b,0x03,0xeb,0xfb]],
   ['two-byte-fallback',[0x0f,0xaf,0xc3,0xeb,0xfb]],
   ['io-fallback',[0xe4,0x60,0x40,0xeb,0xfb]],
   ['interrupt-shadow',[0xfa,0xfb,0x90,0xfa,0xeb,0xfa]],
   ['segment-shadow',[0x8e,0xd0,0x90,0xeb,0xfb]],
   ['flags',[0x9c,0x9d,0x9e,0x9f,0xeb,0xfa]]
  ],results=[];
  for(const[name,program]of programs)for(const budget of [0,1,2,3,7,16,31,100]){
   const setup=enabled=>{
    reset();const v=view();m.HEAPU32[control>>>2]=enabled;
    for(let i=0;i<8;i++)v.setUint32(reg+i*4,0,true);
    v.setUint32(reg+0,0x2000,true);v.setUint32(reg+12,0x2000,true);v.setUint32(reg+16,0x6000,true);
    v.setUint32(reg+32,0x8000,true);v.setUint32(reg+36,0x202,true);
    for(let i=0;i<6;i++){const value=i===1?0x1000:0x2000;v.setUint32(seg+i*4,value,true);v.setUint32(seg+32+i*4,value*16,true);}
    m.HEAPU8.set(program,guest+0x18000);m.HEAPU8.fill(0,guest+0x22000,guest+0x22100);
   };
   const state=async ret=>({ret,guest:await hash(m.HEAPU8.subarray(guest,guest+a.thCoreState(1))),regs:bytes(reg,a.thCoreState(3)),cpu:bytes(a.thCoreState(4),a.thCoreState(5)),lazy:bytes(a.thCpuState(1),a.thCpuState(2)),core:bytes(a.thCpuState(3),a.thCpuState(4)),segments:bytes(seg,a.thCpuState(8)),cycles:bytes(a.thCpuState(5),4),instructions:bytes(a.thCpuState(6),4),prefix:bytes(a.thCpuState(9),1),offset:bytes(a.thCpuState(10),2)});
   setup(0);const expected=await state(a.thCpuRun(budget));setup(1);const actual=await state(a.thCpuRun(budget));
   const equal=JSON.stringify(actual)===JSON.stringify(expected),fast=m.HEAPU32[(control>>>2)+1]-new DataView(saved.buffer).getUint32(control+4,true);
   results.push({name,budget,equal,fast,...(!equal?{expected,actual}:{})});if(!equal){reset();return results;}
  }
  reset();return results;
 });
 fs.writeFileSync(path.resolve(__dirname,'..',process.env.OUTPUT||'reports/native-cpu-budgets-20261015.json'),JSON.stringify({cases,errors},null,2)+'\n');
 assert.deepEqual(errors,[]);for(const c of cases)assert(c.equal,c.name+' budget '+c.budget);assert.equal(cases.length,120);assert(cases.reduce((n,c)=>n+c.fast,0)>100,'sync lane must actually execute');console.log('PASS native CPU instruction budgets:',cases.length);
 await page.evaluate(()=>th04Sync.stop());await page.close();
 }finally{await browser.close();}
})().catch(e=>{console.error(e);process.exitCode=1;});
