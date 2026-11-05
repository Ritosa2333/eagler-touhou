const {chromium}=require('C:/Users/14915/.cache/codex-runtimes/codex-primary-runtime/dependencies/node/node_modules/playwright');
const fs=require('node:fs'),path=require('node:path'),assert=require('node:assert/strict');
process.env.NATIVE_SCHEDULER='1';process.env.NATIVE_FRAME='0';
(async()=>{const browser=await chromium.launch({executablePath:'C:/Program Files (x86)/Microsoft/Edge/Application/msedge.exe',headless:true}),reports=[];try{
 for(const [language,players]of [['cn',2],['jp',3]]){
  const page=await browser.newPage(),errors=[];page.on('pageerror',e=>errors.push(e.message));await require('./native_scheduler_qa.cjs').install(page);
  await page.route('**/netplay/runtime.js',async r=>{const response=await r.fetch();await r.fulfill({response,body:await response.text()+'\nglobalThis.planarQA=()=>emulator;'});});
  await page.goto((process.env.BASE||'http://127.0.0.1:9868')+'/netplay/runtime.html');
  await page.waitForFunction(()=>!!window.th04Sync);
  await page.evaluate(async config=>{th04Sync.configure(0,0,config.players);await th04Sync.load({...config,rollback:true,p1:0,p2:2,p3:1,difficulty:1,lives:6,bombs:2});th04Sync.start();globalThis.planarFrame=0;for(let i=0;i<180;i++){th04Sync.step(Array(config.players).fill(32),{frame:planarFrame++});th04Sync.confirm(planarFrame);}}, {language,players});
  const checks=[];
  for(let window=0;window<8;window++)checks.push(await page.evaluate(async({players,window})=>{
   const r=th04Sync,m=planarQA().module,a=m.nativeExports(),control=a.thPlanarState(),start=planarFrame;
   const set=x=>{m.thDirtyMark?.(control,4);m.HEAPU32[control>>>2]=x;};
   const hash=async v=>Array.from(new Uint8Array(await crypto.subtle.digest('SHA-256',v))).join(',');
   const state=async()=>{const f=m.framePresenter.capture(),gl=m.gl,gpu=new Uint8Array(f.frameWidth*f.frameHeight*4);gl.readPixels(0,0,f.frameWidth,f.frameHeight,gl.RGBA,gl.UNSIGNED_BYTE,gpu);return {
    rgb:await hash(m.HEAPU8.subarray(f.frame,f.frame+f.frameWidth*f.frameHeight*3)),gpu:await hash(gpu),
    guest:await hash(m.HEAPU8.subarray(a.thCoreState(0),a.thCoreState(0)+a.thCoreState(1))),game:r.snapshot(),check:r.checksum(),now:m.virtualNow,
    audio:await Promise.all([...planarQA().outputs].map(async([frame,blocks])=>[frame,await Promise.all(blocks.map(b=>hash(new Uint8Array(b.buffer,b.byteOffset,b.byteLength))))]))};};
   const input=i=>Array.from({length:players},(_,p)=>32|((i+p+window)%4<2?8:4));
   set(0);r.capture(start);const expected=[];for(let i=0;i<12;i++){r.step(input(i),{frame:start+i});r.present();expected.push(await state());}
   r.restore(start);set(1);r.capture(start);for(let i=0;i<12;i++){r.step(input(i),{frame:start+i});r.present();const actual=await state();if(JSON.stringify(actual)!==JSON.stringify(expected[i]))return {equal:false,frame:start+i,expected:expected[i],actual};}
   r.confirm(start+12);planarFrame+=12;return {equal:true,start,frames:12};
  },{players,window}));
  reports.push({language,players,checks,errors});fs.writeFileSync(path.resolve(__dirname,'../reports/tablet-planar-parity-20261015.json'),JSON.stringify(reports,null,2));
  assert.deepEqual(errors,[]);assert(checks.every(c=>c.equal),'PC98 planar pixels/state differ');console.log('PASS PC98 RGB/GPU/game/audio',language,players);await page.evaluate(()=>th04Sync.stop());await page.close();
 }
 }finally{await browser.close();}})().catch(e=>{console.error(e);process.exitCode=1;});
