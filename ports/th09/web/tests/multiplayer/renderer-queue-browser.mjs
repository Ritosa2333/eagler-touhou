// Compare synthetic queue/resource cases against a frozen authored renderer.
import assert from 'node:assert/strict';
import {spawnSync,execFileSync} from 'node:child_process';
import {mkdirSync,readFileSync,writeFileSync,existsSync} from 'node:fs';
import {createHash} from 'node:crypto';
import {createServer} from 'node:http';
import {resolve} from 'node:path';
import {fileURLToPath} from 'node:url';
import {launchBrowser} from '../../../../../tools/browser/launch.mjs';
const workspace=fileURLToPath(new URL('../../../',import.meta.url)),root=resolve(workspace,'web');
const label=process.env.RUN_LABEL||'renderer-queue',revision=process.env.REFERENCE_REVISION||'6ce1c46';assert.match(revision,/^[0-9a-f]{7,40}$/);
const out=resolve(root,'artifacts/multiplayer-tests',label),reference=resolve(out,'reference');mkdirSync(reference,{recursive:true});
const sdk=process.env.TH09_EMSDK;assert.ok(sdk,'TH09_EMSDK is required');
const emcc=['install','upstream'].map(p=>resolve(sdk,p,'emscripten/emcc.py')).find(existsSync);assert.ok(emcc);
for(const name of ['Renderer.cpp','Renderer.hpp','GraphicsState.hpp','Shaders.hpp','ResampleShader.hpp'])writeFileSync(resolve(reference,name),execFileSync('git',['show',revision+':portable/sdl/'+name],{cwd:workspace}));
const sha=bytes=>createHash('sha256').update(bytes).digest('hex'),identity={revision,source:sha(readFileSync(fileURLToPath(new URL('./renderer-queue.cpp',import.meta.url))))};
const dirs=[reference,resolve(workspace,'portable/sdl')],outputs=[];
for(let side=0;side<2;++side){
 const dir=resolve(out,String(side));mkdirSync(dir,{recursive:true});outputs.push(dir);
 const args=[emcc,'-O2','-g0','-std=c++17','-ffp-contract=off','-fno-exceptions','-fno-rtti','-DTH_NATIVE_PLATFORM=1','-I'+dirs[side],resolve(root,'tests/multiplayer/renderer-queue.cpp'),resolve(dirs[side],'Renderer.cpp'),'--use-port=sdl3','--no-entry','-sDEFAULT_TO_CXX=1','-sMODULARIZE=1','-sEXPORT_ES6=1','-sENVIRONMENT=web','-sALLOW_MEMORY_GROWTH=1','-sEXPORTED_RUNTIME_METHODS=HEAPU8,HEAPU32','-sINVOKE_RUN=0','-sMIN_WEBGL_VERSION=2','-sMAX_WEBGL_VERSION=2','-sGL_SUPPORT_AUTOMATIC_ENABLE_EXTENSIONS=0','-o',resolve(dir,'renderer.mjs')];
 const run=spawnSync('python',args,{cwd:workspace,env:{...process.env,EM_CONFIG:resolve(sdk,'.emscripten'),EMSDK:sdk},encoding:'utf8',windowsHide:true});assert.equal(run.status,0,run.stdout+run.stderr);console.log('Compiled renderer '+side);
}
identity.wasm=outputs.map(d=>sha(readFileSync(resolve(d,'renderer.wasm'))));
const routes=new Map();for(let side=0;side<2;++side)for(const name of ['renderer.mjs','renderer.wasm'])routes.set('/'+side+'/'+name,resolve(outputs[side],name));
const server=createServer((req,res)=>{const p=new URL(req.url,'http://localhost').pathname;if(routes.has(p)){res.setHeader('Content-Type',p.endsWith('.wasm')?'application/wasm':'text/javascript');res.end(readFileSync(routes.get(p)));return;}const side=p==='/1'?1:0;res.setHeader('Content-Type','text/html');res.end('<canvas id="canvas" width="640" height="480"></canvas><script type="module">import create from "/'+side+'/renderer.mjs";window.core=await create({canvas:document.querySelector("canvas")});window.ready=!!core._queue_test_open();</script>');});
await new Promise(r=>server.listen(0,'127.0.0.1',r));const url='http://127.0.0.1:'+server.address().port;
const browser=await launchBrowser({args:['--enable-gpu','--use-gl=angle','--use-angle=d3d11']}),pages=[],errors=[],checks=[];
try{
 for(let side=0;side<2;++side){const page=await browser.newPage();pages.push(page);page.on('pageerror',e=>errors.push(e.stack));await page.goto(url+'/'+side);await page.waitForFunction(()=>window.ready,{timeout:30000});}
 for(let mode=0;mode<5;++mode){const pair=[];for(const page of pages)pair.push(await page.evaluate(mode=>{if(!core._queue_test_case(mode))throw Error('GL error in case '+mode);const at=core._sdl_stats()/4;return {image:document.querySelector('canvas').toDataURL(),stats:Array.from(core.HEAPU32.subarray(at,at+17))};},mode));
  for(let side=0;side<2;++side)writeFileSync(resolve(out,`case-${mode}-${side}.png`),Buffer.from(pair[side].image.split(',')[1],'base64'));
  checks.push(pair.map(x=>({image:sha(x.image),stats:x.stats})));assert.equal(pair[0].image,pair[1].image,'Exact framebuffer case '+mode);
 }
 assert.equal(errors.length,0,errors.join('\n'));const report={passed:true,identity,browser:browser.version(),scope:'Synthetic authored-renderer comparison: mixed strides/VAOs, normal and instanced offsets, 1024 instances, 1024 commands, both arena capacities, oversized owner-buffer reuse, resource writes/preparation/release, immediate indexed/line-strip draws, batched independent lines, branch-dependent uniforms, resample texture-unit ownership, readback and discard; not performance acceptance',checks};writeFileSync(resolve(out,'report.json'),JSON.stringify(report,null,2));console.log(JSON.stringify(report));
}catch(error){writeFileSync(resolve(out,'failure.json'),JSON.stringify({error:error.stack,identity,checks,errors},null,2));throw error;}
finally{await browser.close();await new Promise(r=>server.close(r));}
