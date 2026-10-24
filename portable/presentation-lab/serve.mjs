import {readFileSync,writeFileSync,existsSync,mkdirSync,copyFileSync,readdirSync,statSync} from 'node:fs';
import {resolve,dirname,relative} from 'node:path';
import {execFileSync} from 'node:child_process';
import config from './lab.config.mjs';
import {createPresentationLabServer,sha256} from '../../../../common/testkit/presentation-lab/server-core.mjs';

const lab=import.meta.dirname,root=resolve(lab,'../..'),workspace=resolve(root,'..');
const common=resolve(root,'../../common/testkit/presentation-lab');
for(const name of ['controller-core.mjs','contracts.mjs','analyzer.mjs','report-core.mjs','server-core.mjs','workbench.html','workbench.mjs','workbench.css'])
  if(!existsSync(resolve(common,name)))throw Error('Initialize the pinned eagler-common submodule: missing '+name);

const build=JSON.parse(readFileSync(resolve(config.buildRoot,'build.json'),'utf8'));
if(build.game!=='th11'||build.profile!=='presentation-lab'||build.diagnostic!==true)throw Error('Build TH11 with --presentation-lab first');
const exportsSet=new Set((build.exports||[]).map(entry=>entry.name));
for(const name of config.requiredExports)if(!exportsSet.has(name))throw Error('Diagnostic build is missing '+name);

// Assemble an isolated runtime directory: packaged shell + lab WASM.
const runtime=config.runtimeRoot,shell=resolve(root,'build-eagler');
mkdirSync(runtime,{recursive:true});
if(!existsSync(resolve(runtime,'manifest.json'))){
  const source=existsSync(resolve(shell,'shell.mjs'))?shell:resolve(root,'web/sdl-runtime');
  for(const name of ['shell.mjs','keyboard.mjs','directory-keyboard.mjs','eagler-host.mjs','managed.css','startup-branding.mjs','motion-replay.mjs','manifest.json'])
    if(existsSync(resolve(source,name)))copyFileSync(resolve(source,name),resolve(runtime,name));
  const html=existsSync(resolve(shell,'th11.html'))
    ?readFileSync(resolve(shell,'th11.html'),'utf8')
    :readFileSync(resolve(source,'managed.html'),'utf8').replace('<head>','<head><meta name="eagler-data-provider" content="retail-memory">');
  writeFileSync(resolve(runtime,'th11.html'),html);
}
// The Lab does not need the original startup signature animation, and its
// isolated runtime can fail that decorative draw before the game is ready.
// Keep installStartupBranding (loading resources) but skip the animation.
{
  const shellPath=resolve(runtime,'shell.mjs');
  if(existsSync(shellPath)){
    const shell=readFileSync(shellPath,'utf8');
    const patched=shell.replace('await finishStartupAnimation(startupImageAt,frames=>core.th11_draw_loading(frames));','');
    if(patched!==shell)writeFileSync(shellPath,patched);
  }
}
copyFileSync(resolve(config.buildRoot,'th11-sdl.mjs'),resolve(runtime,'th11-sdl.mjs'));
copyFileSync(resolve(config.buildRoot,'th11-sdl.wasm'),resolve(runtime,'th11-sdl.wasm'));
mkdirSync(resolve(runtime,'fonts'),{recursive:true});
const fontSource=resolve(root,'web/assets/sdl-native/fonts/cp932.bin');
if(existsSync(fontSource))copyFileSync(fontSource,resolve(runtime,'fonts/cp932.bin'));
const resources={schema:'eagler-sdl-resources/1',game:'th11',resources:[{path:'/fonts/cp932.bin',url:'./fonts/cp932.bin',bytes:existsSync(fontSource)?statSync(fontSource).size:131072}]};
writeFileSync(resolve(runtime,'resources.json'),JSON.stringify(resources,null,2)+'\n');
if(sha256(readFileSync(resolve(runtime,'th11-sdl.wasm')))!==build.sha256)throw Error('Lab WASM is stale');

const files=new Map([
  ['/','workbench.html'],['/index.html','workbench.html'],
  ['/common/workbench.html','workbench.html'],['/common/workbench.mjs','workbench.mjs'],['/common/workbench.css','workbench.css'],
].map(([url,name])=>[url,resolve(common,name)]));
for(const name of ['controller-core.mjs','contracts.mjs','analyzer.mjs','report-core.mjs'])files.set('/third_party/eagler-common/testkit/presentation-lab/'+name,resolve(common,name));
for(const name of ['browser-config.mjs','controller.mjs','adapter.mjs','owners.mjs','native-abi.mjs','flicker.mjs'])files.set(name==='browser-config.mjs'?'/lab-config.mjs':'/'+name,resolve(lab,name));
for(const name of readdirSync(runtime)){
  const path=resolve(runtime,name);if(statSync(path).isFile())files.set('/runtime/'+name,path);
}
for(const name of readdirSync(resolve(runtime,'fonts')))files.set('/runtime/fonts/'+name,resolve(runtime,'fonts',name));

const font=process.env.TH11_LAB_FONT||'C:/Windows/Fonts/msgothic.ttc';
if(!existsSync(font))throw Error('Set TH11_LAB_FONT to a local MS Gothic font');
files.set('/msgothic.ttc',font);
const data=process.env.TH11_LAB_DATA||'';
if(existsSync(data))files.set('/input/th11.dat',data);

const identity={schema:'presentation-lab/build/1',game:'th11',commit:execFileSync('git',['rev-parse','HEAD'],{cwd:root,encoding:'utf8'}).trim(),wasm:build.sha256,loader:build.loaderSha256,
  commonCommit:execFileSync('git',['rev-parse','HEAD'],{cwd:resolve(root,'../../common'),encoding:'utf8'}).trim(),instrumented:true,dataAvailable:files.has('/input/th11.dat'),
  evidence:'diagnostic runtime, not production deployment',sampling:'authoritative Draw endpoints + frozen alpha sweep'};
const port=Number(process.env.PORT||8136);
const {server,start}=createPresentationLabServer({port,files,identity:()=>identity,incident:{directory:resolve(config.artifactRoot,'incidents'),validate(report){
  if(report?.schema!=='presentation-lab/report/1'||!Number.isInteger(report.tick)||!Array.isArray(report.objects)||report.objects.length>8192)throw Error('Invalid incident report');
  return String(report.tick);
}}});
server.on('listening',()=>console.log(JSON.stringify({url:`http://127.0.0.1:${port}/`,wasm:identity.wasm,data:identity.dataAvailable,scope:'loopback reads / bounded same-origin incident write'})));
start();
