import assert from 'node:assert/strict';
import {readFileSync,mkdirSync,writeFileSync} from 'node:fs';
import {resolve} from 'node:path';
import {productionBrowser} from './production-driver.mjs';
import {replayFixture} from './replay-fixture.mjs';
const args=Object.fromEntries(process.argv.slice(2).map(argument=>{const i=argument.indexOf('=');return [argument.slice(2,i),argument.slice(i+1)];}));
const output=resolve(args.output||'ports/thkkd/build/high-refresh-20261127/secondary');mkdirSync(output,{recursive:true});
const report={schema:'thkkd/secondary-presentation/1',checks:[]};
const driver=await productionBrowser({url:args.url||'http://127.0.0.1:19027/',profile:args.profile,report});const {page,browser}=driver;
try{
  const probeFiles=new Map(['presentation-probe.mjs','presentation-probe.wasm'].map(name=>[name,readFileSync(new URL('../build/web/'+name,import.meta.url))]));
  await page.setRequestInterception(true);page.on('request',request=>{const name=new URL(request.url()).pathname.match(/^\/diagnostics\/(presentation-probe\.(?:mjs|wasm))$/)?.[1];if(name)void request.respond({status:200,contentType:name.endsWith('.mjs')?'text/javascript':'application/wasm',body:probeFiles.get(name)});else void request.continue();});
  await page.setBypassServiceWorker(true);await page.setCacheEnabled(false);await driver.library();
  const frame=await driver.start({expectedBuildId:JSON.parse(readFileSync(new URL('../runtime/manifest.json',import.meta.url),'utf8')).buildId});
  report.rows=await frame.evaluate(async bytes=>{
    const original=__thkkdRuntime,assets=original.platform.assets;
    if(original.core._thkkd_test_word!==undefined)throw Error('测试写入接口进入正式构建');
    await original.command({command:'write',path:'replay/secondary-test.rpy',bytes});
    await original.command({command:'playback',action:'open',path:'replay/secondary-test.rpy'});await original.command({command:'playback',action:'pause'});
    const {default:createCore}=await import('/diagnostics/presentation-probe.mjs');
    const {Platform}=await import(new URL('./platform.mjs',location.href)),{Input}=await import(new URL('./input.mjs',location.href));
    const core=await createCore({print(){},printErr:message=>console.error(message)}),input=new Input({protocol:'eagler-touhou/1',game:'thkkd',epoch:1,origin:location.origin},()=>true);
    const platform=new Platform(document.querySelector('canvas'),input,()=>{});platform.assets=assets;platform.music=false;platform.silent=true;core.thkkdPlatform=platform;
    const requireSuccess=value=>{if(value<0)throw Error(core.UTF8ToString(core._thkkd_error()));};
    requireSuccess(core._thkkd_start());core.FS.writeFile('/savesthkkd/replay/probe.rpy',Uint8Array.from(bytes));core.HEAPU8.set(new TextEncoder().encode('replay/probe.rpy\0'),core._thkkd_replay_path());requireSuccess(core._thkkd_replay_open());
    const word=(address,value)=>requireSuccess(core._thkkd_test_word(address,value)),real=(address,value)=>requireSuccess(core._thkkd_test_real(address,value));
    const renderer=platform.renderer,rows=[];
    for(const entry of [0x612480,0x6128f0])for(const slot of [0,7,31]){
      const base=0xa8ce60+slot*192,prefix=String(500000+slot)+':';
      word(base,1);word(base+4,0);word(base+8,10);real(base+0x10,160);real(base+0x18,220);real(base+0x20,48);
      for(const offset of [0x30,0x38,0x40])real(base+offset,255);
      const draw=(tick,age,x)=>{word(0xb3e2e8,tick);word(base+8,age);real(base+0x10,x);requireSuccess(core._thkkd_test_draw(entry));renderer.present(.5);return renderer.commands.filter(command=>command.identity?.startsWith(prefix));};
      const first=draw(100,10,160);if(!first.length)throw Error('Native secondary drawing missing: '+entry.toString(16));
      const next=draw(101,11,168);const matched=next.filter(command=>renderer.previous.has(command.identity));if(matched.length!==next.length)throw Error('Secondary identity changed during continuous movement');
      const geometry=next.map(command=>({identity:command.identity,transform:command.transform}));
      const pixels=()=>{const gl=renderer.gl,data=new Uint8Array(640*480*4);gl.readPixels(0,0,640,480,gl.RGBA,gl.UNSIGNED_BYTE,data);return data;};
      renderer.present(0);const current=pixels();renderer.present(.5);const smooth=pixels();const visibleChange=current.some((value,index)=>value!==smooth[index]);
      if(!visibleChange)throw Error('Secondary extra frame did not move');const image=renderer.canvas.toDataURL();
      const newborn=draw(102,0,176);if(newborn.some(command=>renderer.previous.has(command.identity)))throw Error('Reused secondary inherited previous object');
      const moved=draw(103,1,350);if(moved.some(command=>{const offset=renderer.commands.indexOf(command)*6*renderer.vertexStride;return renderer.vertices[offset+10]!==0||renderer.vertices[offset+11]!==0;}))throw Error('Teleport extrapolated stale movement');
      word(base,0);word(0xb3e2e8,104);requireSuccess(core._thkkd_test_draw(entry));if(renderer.commands.some(command=>command.identity?.startsWith(prefix)))throw Error('Retired secondary still visible');
      rows.push({entry:'0x'+entry.toString(16),slot,commands:next.length,visibleChange,geometry,birthReset:true,teleportReset:true,retired:true,image});
    }
    const before=core.HEAPU8.slice();for(let i=0;i<100;i++)renderer.present(i%4/4);const after=core.HEAPU8.slice();if(before.some((value,index)=>value!==after[index]))throw Error('Extra presentation changed WASM memory');
    core._thkkd_stop();await platform.suspend();return rows;
  },[...replayFixture({count:1200,practice:true,bombFrame:-1})]);
  for(const row of report.rows){writeFileSync(resolve(output,`${row.entry}-${row.slot}.png`),Buffer.from(row.image.split(',')[1],'base64'));delete row.image;}
  assert.equal(report.rows.length,6);report.checks.push('native-secondary-both-drawing-layers-and-pool-boundaries','secondary-movement-produces-extra-frame','secondary-birth-reuse-teleport-and-retirement','diagnostic-write-exports-excluded-from-production','extra-presents-preserve-complete-probe-memory');
  await page.screenshot({path:resolve(output,'secondary.png')});report.result='passed';console.log(JSON.stringify(report.rows));
}catch(error){report.result='failed';report.error=String(error.stack||error);process.exitCode=1;console.error(error);await page.screenshot({path:resolve(output,'failure.png')});}
finally{await driver.collect();writeFileSync(resolve(output,'result.json'),JSON.stringify(report,null,2));await browser.close();}
