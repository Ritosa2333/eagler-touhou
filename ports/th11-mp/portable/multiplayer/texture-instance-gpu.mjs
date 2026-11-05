// Root runs this serially: two real O2 SDL/WebGL2 Renderers, exact readback equality.
// No retail data, game/network simulation, mobile device or FPS claim.
import {spawn} from 'node:child_process';
import {readFileSync,writeFileSync,mkdirSync,readdirSync,existsSync} from 'node:fs';
import {resolve,relative,extname,sep} from 'node:path';
import {createHash} from 'node:crypto';
import {createRequire} from 'node:module';
import {createServer} from 'node:http';

const root=resolve(import.meta.dirname,'../..');
const option=(name,fallback)=>{const i=process.argv.indexOf(name);if(i<0)return fallback;if(!process.argv[i+1]||process.argv[i+1].startsWith('--'))throw Error('Missing '+name);return process.argv[i+1];};
const sha=value=>createHash('sha256').update(value).digest('hex');
const pin='d32390730ae837fb1333bc1f1d03d746f4697936';
const knownPreimage={
  'portable/sdl/Renderer.cpp':'dabb57cc17c34c82bc860dadf0cf8217f590bafee41e0eb089d6f7751198a220',
  'portable/sdl/Shaders.hpp':'c2168b9412c46c6589f2c726d8ea7b9ea5def8c61c4eb8f8feff6b51cc11181e',
};
const out=resolve(option('--output',resolve(root,'artifacts/rollback-smoothness/texture-instance-gpu-'+new Date().toISOString().replaceAll(':','-').replaceAll('.','-'))));
if(existsSync(out))throw Error('Choose a fresh --output directory: '+out);
mkdirSync(out,{recursive:true});
const receipt={
  schema:'th11-texture-instance-gpu/1',startedAt:new Date().toISOString(),completedAt:null,passed:false,root,out,
  scope:{realRenderer:true,actualGpu:true,exactBgraReadback:true,optimization:'O2',phone:false,retailAssets:false,performance:false,fullWorld:false},
  baseline:{commit:pin,preimageCrlfSha256:knownPreimage,normalization:'Only CRLF/LF normalization for comparison to the recorded pre-edit Web Release source hashes; compile original git-show bytes.'},
  runtime:{node:process.execPath,version:process.version,platform:process.platform},
  sourceFiles:{},sourceDigest:null,sourceStable:false,buildStable:false,commands:[],builds:[],variants:[],comparisons:[],errors:[],cleanup:[],
  metadata:{words:24,originalStatistics:[0,11],checkpoints:'12..19: four (bufferReplacements,vertexUploadBytes) pairs; 20..23: layoutSetups at each checkpoint'},
};
const save=()=>writeFileSync(resolve(out,'receipt.json'),JSON.stringify(receipt,null,2)+'\n');
const requireOk=(condition,message)=>{if(!condition)throw Error(message);};
const observed=new Map(),generated=new Map();
function observe(path){const hash=sha(readFileSync(path));if(observed.has(path))requireOk(observed.get(path)===hash,'Source changed during observation: '+path);observed.set(path,hash);}
function stable(){for(const [path,hash] of observed)requireOk(sha(readFileSync(path))===hash,'Source changed: '+path);}
function builtStable(){for(const [path,hash] of generated)requireOk(sha(readFileSync(path))===hash,'Built artifact changed: '+path);}
function identity(path){const value=readFileSync(path);return {path,bytes:value.length,sha256:sha(value)};}
const walk=dir=>readdirSync(dir,{withFileTypes:true}).flatMap(entry=>entry.isDirectory()?walk(resolve(dir,entry.name)):[resolve(dir,entry.name)]);
async function command(label,program,args,env=process.env){
  const entry={label,program,args,cwd:root,startedAt:new Date().toISOString(),exitCode:null,signal:null,stdoutFile:label+'.stdout.log',stderrFile:label+'.stderr.log'};
  receipt.commands.push(entry);save();const stdout=[],stderr=[];let size=0;
  await new Promise(accept=>{
    let settled=false;
    const child=spawn(program,args,{cwd:root,env,windowsHide:true,stdio:['ignore','pipe','pipe']});
    const done=()=>{if(!settled){settled=true;clearTimeout(timer);accept();}};
    const timer=setTimeout(()=>{entry.error='command exceeded 240 seconds';child.kill();},240000);
    const data=(list,b)=>{size+=b.length;if(size>16*1024*1024){entry.error='bounded command output exceeded';child.kill();}else list.push(b);};
    child.stdout.on('data',b=>data(stdout,b));child.stderr.on('data',b=>data(stderr,b));
    child.once('error',error=>{entry.error=String(error);done();});
    child.once('close',(code,signal)=>{entry.exitCode=code;entry.signal=signal;done();});
  });
  const output=Buffer.concat(stdout),errors=Buffer.concat(stderr);
  writeFileSync(resolve(out,entry.stdoutFile),output);writeFileSync(resolve(out,entry.stderrFile),errors);
  Object.assign(entry,{completedAt:new Date().toISOString(),stdoutBytes:output.length,stderrBytes:errors.length,stdoutSha256:sha(output),stderrSha256:sha(errors)});save();
  requireOk(!entry.error&&entry.exitCode===0,label+' failed; inspect '+out);
  return output;
}
let browser=null,server=null;
save();
try{
  const sdl=resolve(root,'portable/sdl'),imgui=resolve(process.env.EAGLER_COMMON_ROOT??resolve(root,'../../common'),'third_party/imgui');
  const fixture=resolve(import.meta.dirname,'texture-instance-gpu.cpp');
  const sdk=resolve(option('--emsdk',process.env.EMSDK||resolve(root,'../../toolchains/emsdk')));
  const emcc=['upstream/emscripten/emcc.py','install/emscripten/emcc.py'].map(name=>resolve(sdk,name)).find(existsSync);
  requireOk(emcc,'Local Emscripten SDK is unavailable; no download is attempted');
  const python=option('--python',process.env.TH_PYTHON||'python');
  const env={...process.env,EMSDK:sdk,EM_CONFIG:process.env.EM_CONFIG||resolve(sdk,'.emscripten'),EMCC_CORES:'1'};
  const imguiSources=['imgui.cpp','imgui_draw.cpp','imgui_freetype.cpp','imgui_tables.cpp','imgui_widgets.cpp'].map(name=>resolve(imgui,name));
  for(const file of [fixture,import.meta.filename,resolve(sdl,'Renderer.cpp'),...walk(sdl).filter(p=>/\.(h|hpp|inc)$/.test(p)),...imguiSources,...walk(imgui).filter(p=>/\.(h|hpp|inc)$/.test(p))])observe(file);
  receipt.identity={
    head:(await command('game-head','git',['-C',root,'rev-parse','HEAD'])).toString().trim(),
    status:(await command('game-status','git',['-C',root,'status','--porcelain=v1','--untracked-files=normal'])).toString(),
  };
  receipt.toolchain={sdk,emcc:identity(emcc),config:identity(env.EM_CONFIG),python,
    version:(await command('compiler-version',python,[emcc,'--version'],env)).toString().trim()};
  if(existsSync(resolve(sdk,'touhou-sdk.json')))receipt.toolchain.metadata=identity(resolve(sdk,'touhou-sdk.json'));
  receipt.baseline.files={};
  mkdirSync(resolve(out,'reference'),{recursive:true});mkdirSync(resolve(out,'candidate'),{recursive:true});
  for(const [path,expected] of Object.entries(knownPreimage)){
    const bytes=await command('reference-'+path.split('/').pop().replaceAll('.','-'),'git',['-C',root,'show',pin+':'+path]);
    const normalized=Buffer.from(bytes.toString('utf8').replace(/\r?\n/g,'\r\n'));
    requireOk(sha(normalized)===expected,'Pinned reference preimage mismatch: '+path);
    const destination=resolve(out,'reference',path.split('/').pop());writeFileSync(destination,bytes);observe(destination);
    receipt.baseline.files[path]={...identity(destination),crlfSha256:sha(normalized)};
  }
  receipt.sourceFiles=Object.fromEntries([...observed].sort(([a],[b])=>a.localeCompare(b)).map(([p,h])=>[relative(root,p).replaceAll('\\','/'),h]));
  receipt.sourceDigest=sha(JSON.stringify(receipt.sourceFiles));save();
  const commonFlags=['-O2','-g0','-std=c++17','-ffp-contract=off','-fno-strict-aliasing','-fno-exceptions','-fno-rtti',
    '-DTH_NATIVE_PLATFORM=1','-DTH11_DEVELOPMENT_HARNESS=0','-DIMGUI_DISABLE_WIN32_FUNCTIONS','-I'+sdl,'-I'+imgui,'--use-port=sdl3','--use-port=sdl3_ttf'];
  const objects=resolve(out,'objects');mkdirSync(objects,{recursive:true});
  const shared=[];
  for(const file of [fixture,...imguiSources]){
    stable();const label=file.split(/[\\/]/).pop().replaceAll('.','-'),object=resolve(objects,label+'.o');
    console.log('COMPILE shared '+label);
    await command('shared-'+label,python,[emcc,...commonFlags,'-c',file,'-o',object],env);shared.push(object);
    generated.set(object,sha(readFileSync(object)));
  }
  const exports=['ti_initialize','ti_queue_mode','ti_raster_mode','ti_raster_stats','ti_case_count','ti_case_name','ti_run','ti_pixels','ti_bytes','ti_metadata','ti_error','ti_shutdown'].map(name=>'_'+name);
  for(const variant of ['reference','candidate']){
    stable();builtStable();
    const file=variant==='reference'?resolve(out,'reference/Renderer.cpp'):resolve(sdl,'Renderer.cpp');
    const object=resolve(objects,variant+'-renderer.o'),dependencies=resolve(out,variant,'renderer.d');
    console.log('COMPILE '+variant+' Renderer');
    await command(variant+'-compile',python,[emcc,...commonFlags,'-MMD','-MF',dependencies,'-c',file,'-o',object],env);
    const dep=readFileSync(dependencies,'utf8').replace(/\\\r?\n/g,' ').replaceAll('\\:',':').replaceAll('\\','/');
    const expectedShader=variant==='reference'?resolve(out,'reference/Shaders.hpp'):resolve(sdl,'Shaders.hpp');
    requireOk(dep.includes(expectedShader.replaceAll('\\','/')),'Compiler did not record the expected '+variant+' shader dependency');
    requireOk(dep.includes(resolve(sdl,'Renderer.hpp').replaceAll('\\','/')),'Both variants must use the current same Renderer.hpp');
    const loader=resolve(out,variant,'module.mjs');
    await command(variant+'-link',python,[emcc,...commonFlags,'--no-entry','-sDEFAULT_TO_CXX=1','-sMODULARIZE=1','-sEXPORT_ES6=1',
      '-sENVIRONMENT=web','-sALLOW_MEMORY_GROWTH=1','-sSTACK_SIZE=2097152','-sINITIAL_MEMORY=67108864','-sMAXIMUM_MEMORY=536870912',
      '-sFILESYSTEM=1','-sEXPORTED_RUNTIME_METHODS=HEAPU8','-sEXPORTED_FUNCTIONS='+exports.join(','),
      '-sINVOKE_RUN=0','-sEXIT_RUNTIME=0','-sMIN_WEBGL_VERSION=2','-sMAX_WEBGL_VERSION=2','-sGL_SUPPORT_AUTOMATIC_ENABLE_EXTENSIONS=0',
      ...shared,object,'-o',loader],env);
    for(const artifact of [object,loader,loader.replace(/\.mjs$/,'.wasm')])generated.set(artifact,sha(readFileSync(artifact)));
    receipt.builds.push({variant,flags:commonFlags,loader:identity(loader),wasm:identity(loader.replace(/\.mjs$/,'.wasm')),dependencies:identity(dependencies)});save();
  }
  stable();builtStable();
  const diagnostics=resolve(option('--playwright-root',process.env.TH11_PLAYWRIGHT_ROOT||resolve(root,'../../eagler-touhou/th11/tools/diagnostics')));
  const require=createRequire(resolve(diagnostics,'package.json'));
  const {chromium}=require('playwright');
  const chrome=option('--chrome',process.env.PLAYWRIGHT_CHROMIUM_EXECUTABLE||chromium.executablePath());
  requireOk(existsSync(chrome),'An installed desktop Chrome/Edge is required; no download is attempted');
  const temporary=resolve(out,'browser-temp');mkdirSync(temporary,{recursive:true});
  const browserEnv={...process.env,TEMP:temporary,TMP:temporary,TMPDIR:temporary};
  const args=['--disable-background-timer-throttling','--disable-backgrounding-occluded-windows','--disable-renderer-backgrounding','--enable-gpu'];
  if(process.platform==='win32')args.push('--use-gl=angle','--use-angle=d3d11');
  receipt.browser={executable:identity(chrome),playwright:require('playwright/package.json').version,diagnostics,args,headless:false};save();
  server=createServer((request,response)=>{
    try{
      const url=new URL(request.url,'http://127.0.0.1');
      if(url.pathname==='/'){
        const body=Buffer.from('<!doctype html><meta charset="utf-8"><link rel="icon" href="data:,"><title>TH11 Renderer GPU equality</title><canvas id="screen" width="640" height="480"></canvas>');
        response.writeHead(200,{'content-type':'text/html','content-length':body.length});response.end(body);return;
      }
      if(url.pathname==='/favicon.ico'){response.writeHead(204);response.end();return;}
      if(!/^\/(reference|candidate)\/module\.(mjs|wasm)$/.test(url.pathname)){response.writeHead(404);response.end();return;}
      const file=resolve(out,'.'+url.pathname);requireOk(file.startsWith(out+sep),'Unexpected HTTP path');
      const body=readFileSync(file);
      response.writeHead(200,{'content-type':extname(file)==='.wasm'?'application/wasm':'text/javascript','content-length':body.length,'cache-control':'no-store'});
      response.end(body);
    }catch(error){receipt.errors.push({kind:'http',error:String(error)});response.writeHead(500);response.end();}
  });
  server.keepAliveTimeout=65000;server.headersTimeout=70000;
  await new Promise(accept=>server.listen(0,'127.0.0.1',accept));
  const origin='http://127.0.0.1:'+server.address().port;
  browser=await chromium.launch({executablePath:chrome,headless:false,args,env:browserEnv});
  receipt.browser.version=browser.version();
  const expectedCandidate=[1,1,1,1,1,1,16,4,5,1,1,2,3,1,1,3,4,8,4,260,48,3,1025,2,3,4,2,2,4200,2,2,2];
  const expectedReference=[1,1,8,8,12,12,32,8,10,1023,1024,1025,20,8,8,4,6,8,7,260,48,5,1025,2,3,4,2,2,4200,2,2,2];
  const expectedRaster=[...expectedCandidate.slice(0,23),1,3,2,2,2,2,1,2,2];
  // New-VAO assertions need a fresh real Renderer, before any old fixture case.
  const caseOrder=[16,...Array.from({length:expectedCandidate.length},(_,i)=>i).filter(i=>i!==16)];receipt.caseOrder=caseOrder;
  for(const variant of ['reference','candidate','queued','raster']){
    stable();builtStable();
    if(variant==='queued'||variant==='raster')mkdirSync(resolve(out,variant),{recursive:true});
    const context=await browser.newContext({viewport:{width:640,height:480},serviceWorkers:'block'});
    const page=await context.newPage();page.setDefaultTimeout(60000);
    const errors=[],messages=[];
    page.on('pageerror',e=>errors.push({kind:'pageerror',error:String(e)}));
    page.on('console',message=>{if(message.type()==='error')errors.push({kind:'console-error',error:message.text()});else if(messages.length<100)messages.push({kind:message.type(),text:message.text()});});
    page.on('requestfailed',request=>errors.push({kind:'requestfailed',url:request.url(),error:request.failure()?.errorText}));
    const lane={variant,startedAt:new Date().toISOString(),passed:false,cases:[],errors,messages};receipt.variants.push(lane);save();
    try{
      await page.goto(origin+'/',{waitUntil:'load'});
      lane.gpu=await page.evaluate(async({modulePath,queued,raster})=>{
        const canvas=document.querySelector('canvas'),errors=[];
        canvas.addEventListener('webglcontextlost',event=>{errors.push('webglcontextlost');event.preventDefault();});
        globalThis.gpuFixtureErrors=errors;
        const {default:create}=await import(modulePath);
        const module=await create({canvas,print:text=>console.log(text),printErr:text=>console.error(text)});
        globalThis.gpuFixture=module;
        const string=ptr=>{let end=ptr;while(module.HEAPU8[end])++end;return new TextDecoder().decode(module.HEAPU8.subarray(ptr,end));};
        if(!module._ti_initialize())throw Error(string(module._ti_error()));
        module._ti_queue_mode(queued?1:0);
        module._ti_raster_mode(raster?1:0);
        const gl=canvas.getContext('webgl2');if(!gl)throw Error('Real WebGL2 context unavailable');
        const info=gl.getExtension('WEBGL_debug_renderer_info');
        const error=gl.getError();if(error!==gl.NO_ERROR)throw Error('Initialization WebGL error '+error);
        return {version:gl.getParameter(gl.VERSION),renderer:gl.getParameter(info?info.UNMASKED_RENDERER_WEBGL:gl.RENDERER),
          vendor:gl.getParameter(info?info.UNMASKED_VENDOR_WEBGL:gl.VENDOR),maxVertexAttribs:gl.getParameter(gl.MAX_VERTEX_ATTRIBS),
          clipControl:!!gl.getExtension('EXT_clip_control'),cases:module._ti_case_count()};
      },{modulePath:origin+'/'+(variant==='reference'?'reference':'candidate')+'/module.mjs',queued:variant==='queued'||variant==='raster',raster:variant==='raster'});
      requireOk(!/swiftshader|llvmpipe|software raster/i.test(lane.gpu.renderer),'Hardware GPU fixture required; renderer='+lane.gpu.renderer);
      requireOk(lane.gpu.cases===expectedCandidate.length,'Fixture case inventory mismatch');
      if(variant!=='reference')requireOk(JSON.stringify(lane.gpu)===JSON.stringify(receipt.variants[0].gpu),'Reference and candidate GPU configuration differs');
      for(const id of caseOrder){
        const sample=await page.evaluate(id=>{
          const m=globalThis.gpuFixture;
          const string=ptr=>{let end=ptr;while(m.HEAPU8[end])++end;return new TextDecoder().decode(m.HEAPU8.subarray(ptr,end));};
          if(!m._ti_run(id))throw Error(string(m._ti_error()));
          if(globalThis.gpuFixtureErrors.length)throw Error(globalThis.gpuFixtureErrors.join('; '));
          const bytes=m.HEAPU8.slice(m._ti_pixels(),m._ti_pixels()+m._ti_bytes());
          const parts=[];for(let i=0;i<bytes.length;i+=32768)parts.push(String.fromCharCode(...bytes.subarray(i,i+32768)));
          const words=Array.from(new Uint32Array(m.HEAPU8.buffer,m._ti_metadata(),24));
          const gl=document.querySelector('canvas').getContext('webgl2'),error=gl.getError();
          if(error!==gl.NO_ERROR)throw Error('Readback WebGL error '+error);
          return {id,name:string(m._ti_case_name(id)),metadata:words,rasterStats:Array.from(new Uint32Array(m.HEAPU8.buffer,m._ti_raster_stats(),9)),base64:btoa(parts.join(''))};
        },id);
        requireOk(errors.length===0,'Browser errors before '+variant+' case '+id);
        const pixels=Buffer.from(sample.base64,'base64'),file=resolve(out,variant,'case-'+String(id).padStart(2,'0')+'.bgra');
        writeFileSync(file,pixels);
        const item={id,name:sample.name,metadata:sample.metadata,rasterStats:sample.rasterStats,pixels:identity(file)};
        lane.cases.push(item);save();
        requireOk(sample.metadata[0]===id&&sample.metadata[9]===pixels.length&&pixels.length>=512*512*4,'Invalid GPU readback shape');
        requireOk(sample.metadata[10]>0,'All-clear image cannot satisfy GPU oracle');
        requireOk(sample.metadata[3]===(variant==='reference'?expectedReference:variant==='raster'?expectedRaster:expectedCandidate)[id],'Unexpected '+variant+' batch boundary count for '+sample.name+': '+sample.metadata[3]);
        if(id===15&&variant!=='queued'&&variant!=='raster'){
          const expected=variant==='reference'?[1,80,3,240,4,320]:[1,80,2,344,2,344];
          requireOk(JSON.stringify(sample.metadata.slice(12,18))===JSON.stringify(expected),
            'Singleton A / instanced B / singleton B must preserve exact upload boundaries: '+variant);
        }
        if(id===16&&variant!=='queued'&&variant!=='raster'){
          const expected=variant==='reference'?[2,160,3,240,4,324,6,484,1,1,2,2]:[2,344,2,344,3,428,5,772,1,2,3,3];
          requireOk(JSON.stringify(sample.metadata.slice(12,24))===JSON.stringify(expected),
            'Fresh VAO/reused quad/direct Screen invalidation upload and layout gates failed: '+variant);
        }
        if(variant!=='reference'){
          if(id>=17&&id<20&&variant==='candidate'){
            const expected=id===17?[2,164,3,248,5,412,6,496]:id===18?[2,344,3,608,4,872,4,872]:[0,0,0,0,2,160,2,160];
            requireOk(JSON.stringify(sample.metadata.slice(12,20))===JSON.stringify(expected),
              'Resident model geometry must survive dynamic Screen/instance uploads and replace changed quad bytes: '+sample.name);
          }
          if(variant==='queued'&&id>=20&&id<23){
            const uploads=id===20?1:id===21?3:2;
            requireOk(sample.metadata[5]===uploads,'Ordered upload run count failed for '+sample.name+': '+sample.metadata[5]);
          }
          const prior=receipt.variants[0].cases.find(item=>item.id===id),before=readFileSync(prior.pixels.path);
          requireOk(prior.id===id&&prior.name===sample.name&&prior.metadata[1]===sample.metadata[1]&&prior.metadata[2]===sample.metadata[2],'Reference/candidate draw workload differs');
          const equal=before.equals(pixels),comparison={id,variant,name:sample.name,equal,bytes:pixels.length,
            referenceBatches:prior.metadata[3],candidateBatches:sample.metadata[3],
            referenceVertexBytes:prior.metadata[4],candidateVertexBytes:sample.metadata[4],mismatchBytes:0,firstDifferences:[]};
          if(!equal){
            comparison.mismatchBytes=Math.abs(before.length-pixels.length);
            for(let at=0;at<Math.min(before.length,pixels.length);++at)if(before[at]!==pixels[at]){
              ++comparison.mismatchBytes;if(comparison.firstDifferences.length<16)comparison.firstDifferences.push({byte:at,plane:Math.floor(at/(512*512*4)),x:Math.floor(at/4)%512,y:Math.floor(at/(512*4))%512,channel:at%4,reference:before[at],candidate:pixels[at]});
            }
          }
          receipt.comparisons.push(comparison);save();
          requireOk(equal,'GPU pixels differ for '+sample.name+'; raw before/after retained');
        }
        console.log('GPU '+variant+' '+sample.name+' batches='+sample.metadata[3]+' bytes='+pixels.length);
      }
      await page.evaluate(()=>globalThis.gpuFixture._ti_shutdown());
      requireOk(errors.length===0,'Browser errors in '+variant);
      lane.passed=true;
    }finally{lane.completedAt=new Date().toISOString();await context.close();save();}
  }
  stable();receipt.sourceStable=true;builtStable();receipt.buildStable=true;
  receipt.passed=receipt.errors.length===0&&receipt.variants.length===4&&receipt.variants.every(x=>x.passed)&&receipt.comparisons.length===expectedCandidate.length*3&&receipt.comparisons.every(x=>x.equal);
  requireOk(receipt.passed,'Incomplete GPU comparison');
}catch(error){receipt.error=error?.stack||String(error);process.exitCode=1;console.error(String(error));}
finally{
  if(browser){try{await browser.close();receipt.cleanup.push({action:'close-owned-browser',ok:true});}catch(error){receipt.cleanup.push({action:'close-owned-browser',error:String(error)});receipt.passed=false;process.exitCode=1;}}
  if(server){server.closeAllConnections?.();await new Promise(accept=>server.close(accept));}
  receipt.completedAt=new Date().toISOString();save();console.log('Texture instance GPU receipt: '+resolve(out,'receipt.json'));
}
