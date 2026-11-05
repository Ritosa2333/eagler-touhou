import assert from 'node:assert/strict';
import {createServer} from 'node:http';
import {createHash} from 'node:crypto';
import {readFile,writeFile,mkdir,stat} from 'node:fs/promises';
import {resolve,dirname,extname,relative} from 'node:path';
import {execFileSync} from 'node:child_process';
import {assertCliArguments} from '../testing/cli-options.mjs';
import {runWithReport} from '../testing/cli-report.mjs';
import {runtimeTestTargets,selectRuntimeInputs,resultCoverage,runtimeTargetKey} from './runtime-targets.mjs';
import {PRODUCT_GAMES} from '../../common/contracts/product-catalog.mjs';
import {gamepadProbeBundle,prepareVirtualGamepad,bindProtocolHostGamepad,probeNativeGamepad} from './gamepad-probe.mjs';

const argument=name=>process.argv.find(value=>value.startsWith(`--${name}=`))?.slice(name.length+3);
if(!argument('output'))throw Error('Required: --manifest=private-inputs.json --output=directory');
const output=resolve(argument('output'));
const result=await runWithReport({output,schema:'eagler-touhou/runtime-browser/1'},async(report,save)=>{
assertCliArguments(process.argv.slice(2),{values:['manifest','output','game','variant'],flags:['gamepad']});
assert.ok(argument('manifest'),'Required: --manifest=private-inputs.json');
const config=JSON.parse(await readFile(resolve(argument('manifest')),'utf8'));
const variant=argument('variant')??'normal';
const gamepad=process.argv.includes('--gamepad'),gamepadBundle=gamepad?await gamepadProbeBundle():null;
const required=await runtimeTestTargets({game:argument('game'),variant});
report.requiredTargets=required.map(runtimeTargetKey);
const targets=selectRuntimeInputs(config,required);
const {launchBrowser}=await import('../browser/launch.mjs');
await mkdir(output,{recursive:true});
const hash=bytes=>createHash('sha256').update(bytes).digest('hex');
const root=resolve(import.meta.dirname,'../..');
Object.assign(report,{commit:execFileSync('git',['rev-parse','HEAD'],{cwd:root,encoding:'utf8'}).trim(),
  scope:'Desktop Chromium, production Runtime, real private game DATA, protocol host; no physical-device or multiplayer-session claim',targets:[]});
report.harnessSha256=hash(await readFile(new URL(import.meta.url)));
if(gamepad){report.gamepadProbeSha256=hash(await readFile(new URL('./gamepad-probe.mjs',import.meta.url)));report.gamepadBundleSha256=hash(gamepadBundle);await writeFile(resolve(output,'gamepad-bundle.js'),gamepadBundle);await writeFile(resolve(output,'gamepad-probe.mjs'),await readFile(new URL('./gamepad-probe.mjs',import.meta.url)));}
await writeFile(resolve(output,'harness.mjs'),await readFile(new URL(import.meta.url)));
const patch=execFileSync('git',['diff','--binary','HEAD'],{cwd:root,maxBuffer:16*1024*1024});
report.trackedPatchSha256=hash(patch);await writeFile(resolve(output,'source.patch'),patch);
const routes=new Map();
for(const [name,file] of Object.entries(config.shared??{}))routes.set(`/shared/${name}`,resolve(file));
const server=createServer(async(request,response)=>{
  try{
    const url=new URL(request.url,'http://localhost');
    if(url.pathname==='/favicon.ico'){response.writeHead(204).end();return;}
    const selected=config.targets.find(target=>url.pathname===`/host/${target.id}`);
    if(selected){
      const epoch=Number(url.searchParams.get('epoch')??1);assert.ok(Number.isSafeInteger(epoch)&&epoch>0);
      const runtime=`/runtime/${selected.id}/${selected.entry}?hosted=1&managedData=1&gameGeneration=desktop-acceptance&runtimeEpoch=${epoch}`;
      response.setHeader('Content-Type','text/html');
      response.end(`<!doctype html><meta charset="utf-8"><style>html,body{margin:0;background:#222}iframe{width:960px;height:720px;border:0}</style><script>
      window.events=[];window.__eaglerPrepareManagedRuntimeDataV1=async()=>{const buffer=await fetch('/data/${selected.id}').then(r=>{if(!r.ok)throw Error('Missing private DATA');return r.arrayBuffer()});return{buffer,bytes:buffer.byteLength}};
      window.send=(command,fields={})=>document.querySelector('iframe').contentWindow.postMessage({protocol:'eagler-touhou/1',game:${JSON.stringify(selected.game)},epoch:${epoch},command,...fields},location.origin);
      addEventListener('message',event=>{if(event.origin===location.origin&&event.source===document.querySelector('iframe')?.contentWindow)events.push(event.data)});
      window.rpc=(command,fields={})=>new Promise((accept,reject)=>{const request=crypto.randomUUID(),timer=setTimeout(()=>{removeEventListener('message',receive);reject(Error('Timeout: '+command))},120000);function receive(event){if(event.origin!==location.origin||event.source!==document.querySelector('iframe').contentWindow||event.data?.request!==request)return;clearTimeout(timer);removeEventListener('message',receive);event.data.ok?accept(event.data):reject(Error(event.data.error||'Command rejected'))}addEventListener('message',receive);send(command,{...fields,request})});
      </script><iframe src="${runtime}"></iframe>`);return;
    }
    let file=routes.get(url.pathname);
    for(const target of config.targets){
      if(url.pathname===`/data/${target.id}`)file=target.data;
      const prefix=`/runtime/${target.id}/`;
      if(url.pathname.startsWith(prefix)){
        const tail=decodeURIComponent(url.pathname.slice(prefix.length));
        const resolved=resolve(target.runtime,tail);
        if(relative(resolve(target.runtime),resolved).startsWith('..'))throw Error('Invalid path');
        file=resolved;
      }
    }
    if(!file){response.writeHead(404).end('Unknown test input');return;}
    const bytes=await readFile(file);
    response.writeHead(200,{'Content-Type':({'.html':'text/html','.mjs':'text/javascript','.js':'text/javascript','.wasm':'application/wasm','.json':'application/json','.css':'text/css'})[extname(file)]??'application/octet-stream','Content-Length':bytes.length});
    response.end(bytes);
  }catch(error){response.writeHead(404).end(error.message);}
});
await new Promise(done=>server.listen(0,'127.0.0.1',done));
let browser;
try{
  browser=await launchBrowser({args:['--enable-unsafe-swiftshader','--autoplay-policy=no-user-gesture-required']});
  report.browser=browser.version();
  for(const target of targets){
    const record={game:target.game,variant:target.variant,id:target.id,checks:[],errors:[],consoleErrors:[],httpErrors:[],observations:{}};
    report.targets.push(record);
    const check=async(id,checks,operation,{complete=false,fatal=true}={})=>{try{const observation=await operation();record.checks.push({id,checks,complete,status:'passed',observation});return observation;}catch(error){record.checks.push({id,checks,complete,status:'failed',error:String(error.stack||error)});if(fatal)throw error;}finally{await save();}};
    const context=await browser.newContext({viewport:{width:1100,height:820}});
    if(gamepad)await prepareVirtualGamepad(context,gamepadBundle);
    const page=await context.newPage();
    page.on('pageerror',error=>record.errors.push(String(error)));
    page.on('console',message=>{if(message.type()==='error')record.consoleErrors.push(message.text());});
    page.on('response',response=>{if(response.status()>=400&&!response.url().endsWith('/favicon.ico'))record.httpErrors.push({url:response.url(),status:response.status()});});
    let frame;
    const inspect=()=>frame.evaluate(game=>{
      const runtime=window[`__${game}Runtime`],module=runtime?.Module??window.Module,core=runtime?.core??module;
      let state=null;
      try{state=runtime?.status?.()??(core?.[`_${game}_phase`]?{phase:core[`_${game}_phase`]()}:
        core?.th11_status?Array.from(new Int32Array(core.memory.buffer,core.th11_status(),12)):null);}catch(error){state={error:String(error)};}
      return{state,canvas:document.querySelector('canvas')?{width:document.querySelector('canvas').width,height:document.querySelector('canvas').height}:null,
        renderCallbacks:window.__eaglerDebugRenderCallbacks??null,filesystem:!!module?.FS,options:module?.eaglerOptions??null};
    },target.game);
    const key=async(code,key,keyCode,hold=100)=>{
      const fields={code,key,keyCode,location:0,inputSource:'physical'};
      await page.evaluate(fields=>send('keyboard',{...fields,down:true}),fields);await page.waitForTimeout(hold);
      await page.evaluate(fields=>send('keyboard',{...fields,down:false}),fields);await page.waitForTimeout(180);
    };
    try{
      await check('runtime-input-identity',['BUILD-04'],async()=>{
        assert.ok((await stat(target.data)).size>0,'Private DATA must exist');
        const inventoryFile=resolve(target.runtime,'runtime-files.json');
        let inventory;
        try{inventory=JSON.parse(await readFile(inventoryFile,'utf8'));}catch(error){if(error.code!=='ENOENT')throw error;}
        const inputs=inventory?.files??Object.fromEntries([target.entry,...(target.files??[])].map(name=>[name,null]));
        const identities={};
        for(const [name,expected] of Object.entries(inputs)){const bytes=await readFile(resolve(target.runtime,name));identities[name]={bytes:bytes.length,sha256:hash(bytes)};if(expected){assert.equal(bytes.length,expected.bytes,name);assert.equal(hash(bytes),expected.sha256,name);}}
        return identities;
      });
      await check('runtime-ready',['RUNTIME-01'],async()=>{
        await page.goto(`http://127.0.0.1:${server.address().port}/host/${target.id}`);
        await page.waitForFunction(()=>events.some(event=>event.event==='ready'),null,{timeout:120000});
        frame=page.frames().find(frame=>frame.url().includes(`/runtime/${target.id}/`));assert.ok(frame);
        const ready=await page.evaluate(()=>events.find(event=>event.event==='ready'));
        assert.equal(ready.protocol,'eagler-touhou/1');assert.equal(ready.game,target.game);assert.equal(ready.epoch,1);return ready;
      });
      if(target.variant==='multiplayer'&&!target.options?.netplayMode)throw Error('Multiplayer requires a real room scenario and explicit Runtime options');
      await check('configure-launch-first-frame',['RUNTIME-02'],async()=>{
        const sharedResources=Object.keys(config.shared??{}).map(name=>({path:`/${name}`,url:`/shared/${name}`}));
        await page.evaluate(({sharedResources,options})=>rpc('configure',{language:'ja',music:'none',resources:[],runtimeResources:[],sharedResources,thpracCatalog:[],options:{alwaysHitbox:false,touchEnabled:false,thpracEnabled:false,limitPresentationTo60:false,...options}}),{sharedResources,options:target.options??{}});
        await page.evaluate(()=>rpc('launch'));
        await page.waitForFunction(()=>events.some(event=>event.event==='first-frame'),null,{timeout:120000});
        await page.waitForTimeout(2500);
        const snapshot=await inspect();assert.ok(snapshot.canvas?.width>0&&snapshot.canvas?.height>0);return snapshot;
      },{complete:true});
      record.observations.title=await inspect();
      await page.screenshot({path:resolve(output,target.id+'-title.png')});
      await check('reject-unknown-command',['RUNTIME-03'],async()=>{
        const error=await page.evaluate(async()=>{try{await rpc('acceptance-invalid-command');return null;}catch(error){return error.message;}});
        assert.ok(error);assert.ok(!error.startsWith('Timeout:'));return error;
      });
      await check('save-list-and-flush',['SAVE-02'],async()=>{
        const result=await page.evaluate(()=>rpc('list'));
        assert.ok(Array.isArray(result.files));await page.evaluate(()=>rpc('sync'));
        const escaped=await page.evaluate(async()=>{try{await rpc('read',{path:'../outside-acceptance.dat'});return null;}catch(error){return error.message;}});
        assert.ok(escaped&&!escaped.startsWith('Timeout:'));return {files:result.files,directoryEscapeRejected:escaped};
      });
      await check('physical-keyboard-path',['INPUT-01'],async()=>{
        await frame.locator('canvas').first().focus();
        for(let i=0;i<(target.game==='th20'?28:12);i++)await key('KeyZ','z',90);
        await key('ControlLeft','Control',17,1600);
        await page.waitForTimeout(1600);
        const stage=await inspect();record.observations.afterKeyboard=stage;
        await page.screenshot({path:resolve(output,target.id+'-keyboard.png')});
        return stage;
      });
      if(gamepad)await check('native-gamepad-sampling',['INPUT-04','INPUT-06'],async()=>{
        await bindProtocolHostGamepad(page,target.game,1);
        return probeNativeGamepad(page,frame);
      },{fatal:false});
      await check('pause-and-restart',['RUNTIME-06'],async()=>{
        await key('Escape','Escape',27);await page.waitForTimeout(500);
        const paused=await inspect();await page.screenshot({path:resolve(output,target.id+'-paused.png')});
        await key('KeyR','r',82);await page.waitForTimeout(1000);
        const afterRestartKey=await inspect();await page.screenshot({path:resolve(output,target.id+'-after-restart-key.png')});
        record.observations.restart={paused,afterRestartKey};
        if(target.game==='th09'){
          assert.notEqual(afterRestartKey.state?.session?.[1],paused.state?.session?.[1],
            'TH09 remains at the paused native frame after the required R restart');
          assert.ok(afterRestartKey.state?.session?.[1]<paused.state?.session?.[1],
            'TH09 restart must reset the native run frame');
        }
        if(target.game==='th20'){
          await key('ArrowUp','ArrowUp',38);await key('KeyZ','z',90);await page.waitForTimeout(1200);
          record.observations.restart.afterConfirmation=await inspect();
          await page.screenshot({path:resolve(output,target.id+'-after-restart-confirmation.png')});
        }
        return record.observations.restart;
      },{fatal:false});
      await check('resize-and-continued-rendering',[],async()=>{
        await page.setViewportSize({width:800,height:1000});
        await page.locator('iframe').evaluate(element=>{element.style.width='780px';element.style.height='900px';});
        await page.waitForTimeout(1800);await page.screenshot({path:resolve(output,target.id+'-portrait.png')});
        return inspect();
      });
      await check('runtime-errors',[],async()=>{
        assert.deepEqual(record.errors,[]);
        const fatal=await page.evaluate(()=>events.filter(event=>['error','abort'].includes(event.event)));
        assert.deepEqual(fatal,[]);assert.deepEqual(record.httpErrors,[]);assert.deepEqual(record.consoleErrors,[]);return true;
      });
      await check('reload-storage-and-epoch',['SAVE-03','SAVE-04','RUNTIME-01'],async()=>{
        await page.evaluate(()=>rpc('sync'));
        const listed=(await page.evaluate(()=>rpc('list'))).files;
        const storage=PRODUCT_GAMES[target.game].storage;
        const selected=listed.filter(file=>[storage.scoreFile,...(storage.configFiles??[])].includes(file.path));
        assert.ok(selected.length,'Runtime must persist at least a score or configuration file');
        const contents={};
        for(const file of selected){const value=await page.evaluate(path=>rpc('read',{path}),file.path);contents[file.path]=value.bytes;
          const directory=resolve(output,'user-files',target.id);await mkdir(directory,{recursive:true});
          await writeFile(resolve(directory,file.path),new Uint8Array(value.bytes));}
        record.priorSessionEvents=await page.evaluate(()=>window.events);
        await page.goto(`http://127.0.0.1:${server.address().port}/host/${target.id}?epoch=2`);
        await page.waitForFunction(()=>events.some(event=>event.event==='ready'&&event.epoch===2),null,{timeout:120000});
        for(const [path,expected] of Object.entries(contents)){
          const actual=await page.evaluate(path=>rpc('read',{path}),path);assert.deepEqual(actual.bytes,expected,path);}
        const stale='old-epoch-request';
        await page.evaluate(request=>send('sync',{epoch:1,request}),stale);await page.waitForTimeout(350);
        assert.equal(await page.evaluate(request=>events.some(event=>event.request===request),stale),false);
        return{files:Object.fromEntries(Object.entries(contents).map(([name,bytes])=>[name,{bytes:bytes.length,sha256:hash(new Uint8Array(bytes))}])),oldEpochIgnored:true};
      });
    }catch(error){record.failure=String(error.stack||error);await page.screenshot({path:resolve(output,target.id+'-failure.png')}).catch(()=>{});}
    finally{
      record.events=[...(record.priorSessionEvents??[]),...await page.evaluate(()=>window.events??[]).catch(()=>[])];
      await context.close();
      record.passed=!record.failure&&!record.errors.length&&!record.consoleErrors.length&&!record.httpErrors.length&&!record.events.some(event=>['error','abort'].includes(event.event))&&!record.checks.some(check=>check.status==='failed');await save();
      console.log(`${target.id}: ${record.passed?'PASS':'FAIL'} ${record.failure?.split('\n')[0]??''}`);
    }
  }
}finally{try{await browser?.close();}finally{await new Promise(done=>server.close(done));}}
report.coverage=resultCoverage(targets.map(target=>target.id),report.targets);
report.passed=report.coverage.complete;
});
if(!result.passed){console.error(result.failure??'Runtime cases failed');process.exitCode=1;}
