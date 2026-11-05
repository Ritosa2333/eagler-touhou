// Compare the complete native heap/globals after the actual CPU decoder,
// including budgets that stop at every phase of the optimized triplet.
const {chromium}=require('C:/Users/14915/.cache/codex-runtimes/codex-primary-runtime/dependencies/node/node_modules/playwright');
const fs=require('node:fs'),path=require('node:path'),assert=require('node:assert/strict'),root=path.resolve(__dirname,'..');
(async()=>{const browser=await chromium.launch({executablePath:'C:/Program Files (x86)/Microsoft/Edge/Application/msedge.exe',headless:true}),reports=[];
try{for(const language of ['cn','jp'])for(const candidate of [false,true]){
 const page=await browser.newPage(),errors=[];page.on('pageerror',e=>errors.push(e.message));
 if(candidate)await page.route('**/wdosbox-x-rollback.wasm',r=>r.fulfill({contentType:'application/wasm',body:fs.readFileSync(path.join(root,'build/core-idle-compatible.wasm'))}));
 await page.route('**/netplay/runtime.js',async route=>{const response=await route.fetch();await route.fulfill({response,body:await response.text()+'\nglobalThis.idleQA=()=>emulator.module;'});});
 await page.goto((process.env.BASE||'http://127.0.0.1:19870')+'/netplay/runtime.html');await page.waitForFunction(()=>window.th04Sync);
 await page.evaluate(async language=>{th04Sync.configure(0,0,2);await th04Sync.load({language,rollback:false,players:2,p1:0,p2:2,p3:1,difficulty:3,lives:6,bombs:2});th04Sync.start();for(let i=0;i<60;i++){th04Sync.step([0,0],{frame:i});th04Sync.confirm(i+1);}},language);
 const cases=await page.evaluate(async()=>{
  const m=idleQA(),exports=m.nativeExports(),table=Object.values(exports).find(x=>x instanceof WebAssembly.Table);if(!table)throw Error('CPU table missing');
  const decode=table.get(2301),initial=m.HEAPU8.slice(),globals=Object.entries(exports).filter(([n])=>n.startsWith('__rollback_g')).map(([name,g])=>[name,g.value]);
  const result=[],hash=async()=>Array.from(new Uint8Array(await crypto.subtle.digest('SHA-256',m.HEAPU8))).map(x=>x.toString(16).padStart(2,'0')).join('');
  for(const phase of [0,3,7])for(const budget of [0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,29,30,31,32,59,60,61,300,301,302,29999,30000,30001]){
   m.HEAPU8.set(initial);for(const [name,value]of globals)exports[name].value=value;
   const v=new DataView(m.HEAPU8.buffer),ds=v.getUint32(28264412,true),readPtr=linear=>v.getUint32(6765572+(linear>>>12)*4,true)+linear;
   const counter=readPtr(ds+0x2ab2),factor=readPtr(ds+0x5390),code=readPtr(113269);
   if(v.getUint32(code,true)!==0x3b2ab2a1||v.getUint32(code+4,true)!==0x72539006)throw Error('Wrong loop fixture');
   v.setUint16(counter,0,true);v.setUint16(factor,1,true);v.setUint16(28264160,0,true);
   v.setUint32(28264192,113269-v.getUint32(28264404,true)+phase,true);v.setInt32(28264484,budget,true);
   const ret=decode();result.push({phase,budget,ret,heap:await hash(),globals:globals.map(([name])=>[name,exports[name].value])});
  }
  m.HEAPU8.set(initial);for(const [name,value]of globals)exports[name].value=value;return result;
 });
 assert.deepEqual(errors,[]);reports.push({language,candidate,cases});console.log('CPU parity cases',language,candidate,cases.length);
 if(candidate)assert.deepEqual(cases,reports.at(-2).cases,'Every instruction budget must match the baseline complete heap and globals');
 await page.close();
}
fs.writeFileSync(path.join(root,'reports/weak-core-idle-instruction-parity-20261014.json'),JSON.stringify(reports,null,2));console.log('PASS complete instruction-budget parity');
}finally{await browser.close();}})().catch(e=>{console.error(e);process.exitCode=1});
