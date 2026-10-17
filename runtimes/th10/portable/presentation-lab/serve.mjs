import {readFileSync,existsSync} from 'node:fs';
import {resolve} from 'node:path';
import {execFileSync} from 'node:child_process';
import config from './lab.config.mjs';
import {createPresentationLabServer,sha256,verifyRuntimeInventory} from '../../third_party/eagler-common/testkit/presentation-lab/server-core.mjs';

const lab=import.meta.dirname,root=resolve(lab,'../..'),workspace=resolve(root,'..');
const common=resolve(root,'third_party/eagler-common/testkit/presentation-lab');
const build=JSON.parse(readFileSync(resolve(config.buildRoot,'build.json'),'utf8'));
if(build.game!=='th10'||build.profile!=='presentation-lab'||build.diagnostic!==true)throw Error('Build TH10 with --presentation-lab first');
const exports=new Set((build.exports||[]).map(entry=>entry.name));
for(const name of config.requiredExports)if(!exports.has(name))throw Error('Diagnostic build is missing '+name);
if(sha256(readFileSync(resolve(config.runtimeRoot,config.wasm)))!==build.sha256)throw Error('Packaged lab WASM is stale');
const inventory=JSON.parse(readFileSync(resolve(config.runtimeRoot,'runtime-files.json'),'utf8')).files;
verifyRuntimeInventory(config.runtimeRoot,inventory);
for(const [name,digest] of Object.entries(build.sourceFiles))if(sha256(readFileSync(resolve(root,name)))!==digest)throw Error('Rebuild modified instrumented source: '+name);

const files=new Map([
  ['/','workbench.html'],['/index.html','workbench.html'],['/common/workbench.html','workbench.html'],
  ['/common/workbench.mjs','workbench.mjs'],['/common/workbench.css','workbench.css'],
].map(([url,name])=>[url,resolve(common,name)]));
for(const name of ['controller-core.mjs','contracts.mjs','analyzer.mjs','report-core.mjs'])files.set('/third_party/eagler-common/testkit/presentation-lab/'+name,resolve(common,name));
for(const name of ['browser-config.mjs','controller.mjs','adapter.mjs','owners.mjs','native-abi.mjs'])files.set(name==='browser-config.mjs'?'/lab-config.mjs':'/'+name,resolve(lab,name));
for(const name of Object.keys(inventory))files.set('/runtime/'+name,resolve(config.runtimeRoot,name));
const font=process.env.TH10_LAB_FONT||resolve(workspace,'th06-eagler/assets/msgothic.ttc');
if(!existsSync(font))throw Error('Set TH10_LAB_FONT to a local MS Gothic font');
files.set('/msgothic.ttc',font);
const data=process.env.TH10_LAB_DATA||resolve(config.artifactRoot,'input/th10.dat');
if(existsSync(data))files.set('/input/th10.dat',data);

const identity={schema:'presentation-lab/build/1',game:'th10',commit:execFileSync('git',['rev-parse','HEAD'],{cwd:root,encoding:'utf8'}).trim(),wasm:build.sha256,loader:build.loaderSha256,
  commonCommit:execFileSync('git',['rev-parse','HEAD'],{cwd:resolve(root,'third_party/eagler-common'),encoding:'utf8'}).trim(),instrumented:true,dataAvailable:files.has('/input/th10.dat'),
  evidence:'diagnostic runtime, not production deployment',sampling:'original Draw endpoints + frozen alpha sweep'};
const port=Number(process.env.PORT||8134);
const {server,start}=createPresentationLabServer({port,files,identity:()=>identity,incident:{directory:resolve(config.artifactRoot,'incidents'),validate(report){if(report?.schema!=='presentation-lab/report/1'||!Number.isInteger(report.tick)||!Array.isArray(report.objects)||report.objects.length>8192)throw Error('Invalid incident report');return String(report.tick);}}});
server.on('listening',()=>console.log(JSON.stringify({url:`http://127.0.0.1:${port}/`,wasm:identity.wasm,data:identity.dataAvailable,scope:'loopback reads / bounded same-origin incident write'})));
start();
