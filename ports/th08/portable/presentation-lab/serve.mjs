import {readFileSync,existsSync} from 'node:fs';
import {resolve} from 'node:path';
import {execFileSync} from 'node:child_process';
import {dependencySnapshot} from '../../../../tools/testing/dependency-snapshot.mjs';
import {createPresentationLabServer,sha256 as sha,verifyRuntimeInventory} from '../../../../common/testkit/presentation-lab/server-core.mjs';
import {TH08_PRESENTATION_LAB_EXPORTS} from './native-abi.mjs';
const lab=import.meta.dirname,root=resolve(lab,'../..');
const common=resolve(root,'../../common/testkit/presentation-lab');
const commonEntries=['controller-core.mjs','contracts.mjs','report-core.mjs','server-core.mjs','analyzer.mjs','review-record.mjs','replay-pair.mjs'];
const commonPrefix='common/testkit/presentation-lab/';
const commonModules=(await dependencySnapshot(resolve(root,'../..'),commonEntries.map(name=>commonPrefix+name)))
  .filter(file=>file.path.startsWith(commonPrefix)).map(file=>file.path.slice(commonPrefix.length));
for(const name of commonModules)if(!existsSync(resolve(common,name)))throw Error('Shared common testkit is missing '+name);
const snapshot=process.env.TH08_LAB_SNAPSHOT||'';
if(snapshot&&!/^[a-z0-9-]{1,40}$/.test(snapshot))throw Error('Invalid snapshot');
const runtime=snapshot?resolve(root,'artifacts/presentation-lab/builds',snapshot):resolve(root,'artifacts/presentation-lab/runtime');
const input=process.env.TH08_LAB_INPUT_ROOT?resolve(process.env.TH08_LAB_INPUT_ROOT):resolve(root,'artifacts/presentation-lab/input');
const build=JSON.parse(readFileSync(snapshot?resolve(runtime,'build.json'):resolve(root,'web/artifacts/presentation-lab/build.json'),'utf8'));
const names=new Set(build.exports.map(x=>x.name));
if(!build.diagnostic||build.profile!=='presentation-lab')throw Error('Refusing to serve a non-diagnostic build');
for(const name of TH08_PRESENTATION_LAB_EXPORTS)if(!names.has(name))throw Error('Build with --th08 --presentation-lab first: missing '+name);
if(sha(readFileSync(resolve(runtime,'th08-sdl.wasm')))!==build.sha256)throw Error('Packaged lab WASM is stale');
const inventory=JSON.parse(readFileSync(resolve(runtime,'runtime-files.json'),'utf8')).files;
verifyRuntimeInventory(runtime,inventory);
if(!snapshot)for(const [name,digest] of Object.entries(build.sourceFiles))if(sha(readFileSync(resolve(root,name)))!==digest)throw Error('Rebuild modified instrumented source: '+name);
function readProductionBuildIdentity(){
 const path=resolve(root,'web/artifacts/sdl3/build.json');if(!existsSync(path))return {available:false,reason:'normal SDL build descriptor is absent'};
 const bytes=readFileSync(path),normal=JSON.parse(bytes.toString('utf8'));
 const sources=Object.fromEntries(Object.entries(normal.sourceFiles||{}).sort(([a],[b])=>a.localeCompare(b)));
 let sourceMatchesCurrent=!!Object.keys(sources).length;
 for(const [name,digest] of Object.entries(sources)){
  try{if(sha(readFileSync(resolve(root,name)))!==digest)sourceMatchesCurrent=false;}catch{sourceMatchesCurrent=false;}
 }
 return {available:true,schema:'th08/build-comparison/1',profile:normal.profile,diagnostic:normal.diagnostic===true,
  manifestSha256:sha(bytes),wasmSha256:normal.sha256||null,loaderSha256:normal.loaderSha256||null,
  sourceDigest:sha(JSON.stringify(sources)),sourceCount:Object.keys(sources).length,sourceMatchesCurrent,
  toolchain:normal.toolchain||null};
}
const productionBuild=readProductionBuildIdentity();
const files=new Map([['/','index.html'],['/index.html','index.html'],['/app.mjs','app.mjs'],['/controller.mjs','controller.mjs'],['/adapter.mjs','adapter.mjs'],['/native-abi.mjs','native-abi.mjs'],['/analyzer.mjs','analyzer.mjs'],['/owners.mjs','owners.mjs'],['/timing.mjs','timing.mjs'],['/style.css','style.css'],['/fullscreen.css','fullscreen.css']].map(([url,file])=>[url,resolve(lab,file)]));
for(const name of commonModules)files.set('/common/testkit/presentation-lab/'+name,resolve(common,name));
for(const file of Object.keys(inventory))files.set('/runtime/'+file,resolve(runtime,file));
const font=process.env.TH08_LAB_FONT||process.env.EAGLER_DEVELOPMENT_VANILLA_FONT;
if(!font||!existsSync(font))throw Error('Set TH08_LAB_FONT to your local MS Gothic font');
files.set('/msgothic.ttc',font);
const data=process.env.TH08_LAB_DATA||resolve(input,'th08.dat');
if(existsSync(data))files.set('/input/th08.dat',data);
for(let i=0;i<4;i++)if(existsSync(resolve(input,`demo${i}.rpy`)))files.set(`/input/demo${i}.rpy`,resolve(input,`demo${i}.rpy`));
const campaignManifest=resolve(input,'campaign/manifest.json');
if(existsSync(campaignManifest)){
 files.set('/campaign/manifest.json',campaignManifest);
 const corpus=JSON.parse(readFileSync(campaignManifest,'utf8'));
 for(const fixture of corpus.fixtures||[]){
  if(!/^[a-z0-9-]{1,40}$/.test(fixture.id))throw Error('Invalid campaign fixture ID');
  const path=resolve(input,'campaign',fixture.id+'.rpy');
  if(sha(readFileSync(path))!==fixture.sha256)throw Error('Replay fixture identity mismatch');
  files.set('/campaign/'+fixture.id+'.rpy',path);
 }
}
const identity={schema:'th08/presentation-lab-build/1',commit:execFileSync('git',['rev-parse','HEAD'],{cwd:root,encoding:'utf8'}).trim(),wasm:build.sha256,loader:build.loaderSha256,
  commonCommit:execFileSync('git',['rev-parse','HEAD'],{cwd:resolve(root,'../../common'),encoding:'utf8'}).trim(),
  sourceDigest:sha(JSON.stringify(build.sourceFiles)),snapshot:snapshot||null,instrumented:true,dataAvailable:files.has('/input/th08.dat'),fixtureManifest:existsSync(resolve(input,'manifest.json'))?JSON.parse(readFileSync(resolve(input,'manifest.json'),'utf8')):{},
  productionBuild,evidence:'diagnostic runtime, not production deployment',sampling:'authoritative Draw endpoints + frozen alpha sweep'};
function currentIdentity(){
 const local=['index.html','style.css','fullscreen.css','app.mjs','controller.mjs','adapter.mjs','native-abi.mjs','analyzer.mjs','owners.mjs','timing.mjs'];
 const toolSources=Object.fromEntries(local.map(name=>[name,sha(readFileSync(resolve(lab,name)))]));
for(const name of commonModules)toolSources['eagler-common/'+name]=sha(readFileSync(resolve(common,name)));
 return {...identity,toolSources,toolDigest:sha(JSON.stringify(toolSources))};
}
const port=Number(process.env.PORT||8132);
const {server,start}=createPresentationLabServer({port,files,identity:currentIdentity,incident:{directory:resolve(root,'artifacts/presentation-lab/incidents'),validate(report){if(report?.schema!=='th08/presentation-audit/1'||report?.timing?.schema!=='th08/presentation-timing-ring/1'||!Array.isArray(report.timing.rows)||report.timing.rows.length>512||!Number.isInteger(report.tick))throw Error('Invalid incident report');return String(report.tick);}}});
server.on('listening',()=>console.log(JSON.stringify({url:`http://127.0.0.1:${port}/`,wasm:identity.wasm,data:identity.dataAvailable,scope:'loopback reads / bounded same-origin incident write'})));start();
