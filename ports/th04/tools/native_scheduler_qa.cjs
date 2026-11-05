// Route an isolated source build through the real game/room entry points.
const fs=require('node:fs'),path=require('node:path');
const root=path.resolve(__dirname,'..'),dir=path.resolve(root,process.env.NATIVE_RUNTIME||'build/native-scheduler/runtime');
async function install(page){
 if(process.env.NATIVE_SCHEDULER!=='1')return;
 const tracked=process.env.NATIVE_TRACKED==='1';
 if(process.env.NATIVE_FRAME==='1')await page.route('**/frame-presenter.js',r=>r.fulfill({contentType:'text/javascript',body:fs.readFileSync(path.join(dir,'frame-presenter-native.js'))}));
 for(const name of ['dosbox-rollback.mjs',...(tracked?['dosbox-rollback-tracked.mjs']:[])])await page.route('**/'+name,r=>r.fulfill({contentType:'text/javascript',body:fs.readFileSync(path.join(dir,name))}));
 for(const name of ['wdosbox-x-rollback.wasm',...(tracked?['wdosbox-x-rollback-tracked.wasm']:[])])await page.route('**/'+name,r=>r.fulfill({contentType:'application/wasm',body:fs.readFileSync(path.join(dir,name))}));
 if(tracked)await page.route('**/snapshot-diff-tracked.wasm',r=>r.fulfill({contentType:'application/wasm',body:fs.readFileSync(path.join(dir,'snapshot-diff-tracked.wasm'))}));
 await page.route('**/dosbox-adapter.js',async r=>{
  const response=await r.fetch();let source=await response.text();
  source=source.replace('nativeScheduling:true,nativeCpu:true,nativeFramePresentation:false,','');
  if(!tracked)source=source.replace('const tracked=net&&','const tracked=false&&net&&');
  source=source.replace('deterministic:net,','deterministic:net,nativeScheduling:'+(process.env.NATIVE_BATCH==='0'?'false':'true')+',');
  source=source.replace('deterministic:net,','deterministic:net,nativeCpu:'+(process.env.NATIVE_CPU==='0'?'false':'true')+',');
  source=source.replace('deterministic:net,','deterministic:net,nativeFramePresentation:'+(process.env.NATIVE_FRAME==='1'&&process.env.NATIVE_FRAME_START!=='0'?'true':'false')+',');
  await r.fulfill({response,body:source});
 });
}
module.exports={install};
