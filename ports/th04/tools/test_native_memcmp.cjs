const {chromium}=require('C:/Users/14915/.cache/codex-runtimes/codex-primary-runtime/dependencies/node/node_modules/playwright');
const fs=require('node:fs'),path=require('node:path');
process.env.NATIVE_SCHEDULER='1';
(async()=>{const browser=await chromium.launch({executablePath:'C:/Program Files (x86)/Microsoft/Edge/Application/msedge.exe',headless:true});try{
 const page=await browser.newPage();await require('./native_scheduler_qa.cjs').install(page);
 await page.route('**/netplay/runtime.js',async r=>{const response=await r.fetch();await r.fulfill({response,body:await response.text()+'\nglobalThis.memcmpQA=()=>emulator.module;'});});
 await page.goto((process.env.BASE||'http://127.0.0.1:9868')+'/netplay/runtime.html');
 await page.waitForFunction(()=>!!window.th04Sync);
 await page.evaluate(async()=>{th04Sync.configure(0,0,2);await th04Sync.load({players:2,rollback:false,language:'cn',p1:0,p2:2,difficulty:1,lives:6,bombs:2});});
 const result=await page.evaluate(()=>{
  const m=memcmpQA(),a=m.nativeExports(),h=m.HEAPU8,base=a.thCoreState(0)+0x90000,saved=h.slice(base,base+1024);let cases=0;
  const check=(x,y,n)=>{const actual=a.thFastMemcmp(x,y,n),expected=a.thReferenceMemcmp(x,y,n);if(actual!==expected)throw Error(JSON.stringify({x,y,n,actual,expected}));cases++;};
  try{for(let off=0;off<16;off++)for(let n=0;n<=257;n++){
   const x=base+off,y=base+512+(15-off);for(let i=0;i<n;i++)h[x+i]=h[y+i]=(i*73+off*17)&255;
   check(x,y,n);check(y,x,n);
   for(const at of new Set([0,1,7,8,15,16,n-1,n>>1]))if(at>=0&&at<n){const v=h[y+at];h[y+at]^=0xff;check(x,y,n);check(y,x,n);h[y+at]=v;}
  }
  // End-of-memory tests detect speculative/unguarded block overreads.
  for(let n=0;n<=257;n++){check(h.length-n,base,n);check(base,h.length-n,n);check(h.length-n,h.length-n,n);}
  return {cases,exactReturnValues:true,unaligned:true,memoryEnd:true};
  }finally{h.set(saved,base);}
 });
 fs.writeFileSync(path.resolve(__dirname,'../reports/tablet-memcmp-parity-20261015.json'),JSON.stringify(result,null,2));console.log('PASS',result);await page.close();
 }finally{await browser.close();}})().catch(e=>{console.error(e);process.exitCode=1;});
