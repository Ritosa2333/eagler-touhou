// Build the production MP objects first: node portable/build.mjs --multiplayer
// Relinks those exact objects with an audio-only fixture, without a game loop.
import assert from 'node:assert/strict';
import {readFileSync,writeFileSync,mkdirSync} from 'node:fs';
import {resolve,relative} from 'node:path';
import {execFileSync} from 'node:child_process';
import {createRequire} from 'node:module';
import {createServer} from 'node:http';
import {responseFileContents} from '../response-file.mjs';
const root=resolve(import.meta.dirname,'../..'),out=resolve(root,'artifacts/audio-output-fixture');mkdirSync(out,{recursive:true});
const sdk=process.env.EMSDK||resolve(root,'../../toolchains/emsdk');
const plan=JSON.parse(execFileSync(process.execPath,[resolve(root,'portable/build.mjs'),'--multiplayer','--print-plan'],{encoding:'utf8',env:{...process.env,EMSDK:sdk}}));
const python=process.env.TH_PYTHON||'python',emcc=resolve(sdk,'upstream/emscripten/emcc.py');
const run=(args,name)=>{const path=resolve(out,name+'.rsp.utf-8');writeFileSync(path,responseFileContents(args));execFileSync(python,[emcc,'@'+path],{cwd:root,env:{...process.env,EMSDK:sdk,EM_CONFIG:resolve(sdk,'.emscripten')},stdio:'inherit',windowsHide:true});};
const fixture=resolve(out,'fixture.o');run([...plan.flags,'-c',resolve(import.meta.dirname,'AudioOutputFixture.cpp'),'-o',fixture],'compile');
const objects=plan.sources.map(source=>resolve(root,'th11_web/artifacts/multiplayer/objects',relative(root,resolve(root,source)).replaceAll('\\','_').replaceAll('/','_').replaceAll(':','_').replaceAll('.','_')+'.o'));
run([...plan.flags,...plan.linkFlags,'--no-entry','-sDEFAULT_TO_CXX=1','-sMODULARIZE=1','-sEXPORT_ES6=1','-sENVIRONMENT=web,worker','-sALLOW_MEMORY_GROWTH=1','-sSTACK_SIZE=1048576','-sINITIAL_MEMORY=134217728','-sMAXIMUM_MEMORY=1073741824','-sFILESYSTEM=1','-lidbfs.js','-sEXPORTED_RUNTIME_METHODS=FS,HEAPU8','-sINVOKE_RUN=0','-sEXIT_RUNTIME=0','-sMIN_WEBGL_VERSION=2','-sMAX_WEBGL_VERSION=2','-sEXPORTED_FUNCTIONS=_malloc,_free',...objects,fixture,'-o',resolve(out,'audio.mjs')],'link');
const archive=process.env.TH11_TEST_ARCHIVE;if(!archive)throw Error('Set TH11_TEST_ARCHIVE to the private TH11 archive.');
const launcher=process.env.EAGLER_LAUNCHER_ROOT||resolve(root,'../../eagler-touhou');
const {chromium}=createRequire(resolve(launcher,'package.json'))(process.env.EAGLER_PLAYWRIGHT_MODULE||'playwright');
const server=createServer((req,res)=>{
 const path=new URL(req.url,'http://localhost').pathname;
 if(path==='/'){res.setHeader('Content-Type','text/html');res.end('<canvas id="canvas"></canvas>');return;}
 if(!['/audio.mjs','/audio.wasm','/th11.dat'].includes(path)){res.writeHead(404).end();return;}
 res.setHeader('Content-Type',path.endsWith('.wasm')?'application/wasm':path.endsWith('.mjs')?'text/javascript':'application/octet-stream');
 res.end(readFileSync(path==='/th11.dat'?archive:resolve(out,path.slice(1))));
});
await new Promise(r=>server.listen(0,'127.0.0.1',r));let browser;
try{
 browser=await chromium.launch({headless:true,executablePath:process.env.EAGLER_TEST_CHROMIUM||undefined,args:['--autoplay-policy=no-user-gesture-required']});
 const page=await browser.newPage(),errors=[];page.on('pageerror',e=>errors.push(String(e)));
 await page.goto('http://127.0.0.1:'+server.address().port);
 const result=await page.evaluate(async()=>{
  let raw;const {default:create}=await import('/audio.mjs');
  const c=await create({canvas:document.querySelector('canvas'),instantiateWasm(imports,done){WebAssembly.instantiateStreaming(fetch('/audio.wasm'),imports).then(({instance,module})=>{raw=instance.exports;done(instance,module);});return {};}});
  c.FS.writeFile('/th11.dat',new Uint8Array(await fetch('/th11.dat').then(r=>r.arrayBuffer())));
  if(!raw.audio_fixture_initialize())throw Error('Audio fixture initialization failed');
  const pool=c.eaglerSfxOutput,command=raw.audio_fixture_command;
  command(0,1);const voice=[...pool.voices.values()].find(v=>v.playing);if(!voice?.node)throw Error('Actual sound_play did not start native SFX');
  const voices=pool.voices.size;command(1,1);const stopped=!voice.playing&&!voice.node;
  command(0,1);const silent=!voice.playing&&!voice.node;
  command(1,0);const noStalePlayback=!voice.playing&&!voice.node;
  command(0,1);const restored=!!voice.node;
  command(2,1);const cursor=pool.cursor(voice);await new Promise(r=>setTimeout(r,80));const frozen=cursor===pool.cursor(voice)&&!voice.node;
  command(2,0);const resumed=!!voice.node;command(3,0);const released=pool.voices.size===0;
  return {voices,stopped,silent,noStalePlayback,restored,frozen,resumed,released};
 });
 assert.deepEqual(errors,[]);assert.equal(result.voices,56);for(const [name,value] of Object.entries(result))if(name!=='voices')assert.equal(value,true,name);
 writeFileSync(resolve(out,'result.json'),JSON.stringify(result,null,2));console.log(JSON.stringify(result));
}finally{if(browser)await browser.close();await new Promise(r=>server.close(r));}
