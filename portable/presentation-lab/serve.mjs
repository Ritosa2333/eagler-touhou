import {readFileSync,existsSync} from 'node:fs';
import {resolve} from 'node:path';
import {execFileSync} from 'node:child_process';
import {createPresentationLabServer,sha256 as sha,verifyRuntimeInventory} from '../../third_party/eagler-common/testkit/presentation-lab/server-core.mjs';
const lab=import.meta.dirname,root=resolve(lab,'../..'),workspace=resolve(root,'..');
const common=resolve(root,'third_party/eagler-common/testkit/presentation-lab');
const commonModules=['controller-core.mjs','contracts.mjs','report-core.mjs','server-core.mjs'];
for(const name of commonModules)if(!existsSync(resolve(common,name)))throw Error('Initialize the pinned eagler-common submodule: missing '+name);
const snapshot=process.env.TH08_LAB_SNAPSHOT||'';
if(snapshot&&!/^[a-z0-9-]{1,40}$/.test(snapshot))throw Error('Invalid snapshot');
const runtime=snapshot?resolve(root,'artifacts/presentation-lab/builds',snapshot):resolve(root,'build-eagler'),input=resolve(root,'artifacts/presentation-lab/input');
const build=JSON.parse(readFileSync(snapshot?resolve(runtime,'build.json'):resolve(root,'th08_web/artifacts/sdl3/build.json'),'utf8'));
const names=new Set(build.exports.map(x=>x.name));
for(const name of ['audit_draw','audit_reference','audit_state','audit_fault','audit_timing_records','audit_timing_capacity','audit_timing_count','audit_timing_next','audit_timing_stride'])if(!names.has(name))throw Error('Build with --th08 --presentation-lab first: missing '+name);
if(sha(readFileSync(resolve(runtime,'th08-sdl.wasm')))!==build.sha256)throw Error('Packaged lab WASM is stale');
const inventory=JSON.parse(readFileSync(resolve(runtime,'runtime-files.json'),'utf8')).files;
verifyRuntimeInventory(runtime,inventory);
if(!snapshot)for(const [name,digest] of Object.entries(build.sourceFiles))if(sha(readFileSync(resolve(root,name)))!==digest)throw Error('Rebuild modified instrumented source: '+name);
const files=new Map([['/','index.html'],['/index.html','index.html'],['/app.mjs','app.mjs'],['/controller.mjs','controller.mjs'],['/adapter.mjs','adapter.mjs'],['/analyzer.mjs','analyzer.mjs'],['/owners.mjs','owners.mjs'],['/timing.mjs','timing.mjs'],['/style.css','style.css'],['/fullscreen.css','fullscreen.css']].map(([url,file])=>[url,resolve(lab,file)]));
for(const name of commonModules)files.set('/third_party/eagler-common/testkit/presentation-lab/'+name,resolve(common,name));
for(const file of Object.keys(inventory))files.set('/runtime/'+file,resolve(runtime,file));
const font=process.env.TH08_LAB_FONT||resolve(workspace,'th06-eagler/assets/msgothic.ttc');
if(!existsSync(font))throw Error('Set TH08_LAB_FONT to your local MS Gothic font');
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
  sourceDigest:sha(JSON.stringify(build.sourceFiles)),snapshot:snapshot||null,instrumented:true,dataAvailable:files.has('/input/th08.dat'),fixtureManifest:existsSync(resolve(input,'manifest.json'))?JSON.parse(readFileSync(resolve(input,'manifest.json'),'utf8')):{},
  evidence:'diagnostic runtime, not production deployment',sampling:'authoritative Draw endpoints + frozen alpha sweep'};
function currentIdentity(){
 const local=['index.html','style.css','fullscreen.css','app.mjs','controller.mjs','adapter.mjs','analyzer.mjs','owners.mjs','timing.mjs'];
 const toolSources=Object.fromEntries(local.map(name=>[name,sha(readFileSync(resolve(lab,name)))]));
for(const name of commonModules)toolSources['eagler-common/'+name]=sha(readFileSync(resolve(common,name)));
 return {...identity,toolSources,toolDigest:sha(JSON.stringify(toolSources))};
}
const port=Number(process.env.PORT||8132);
const {server,start}=createPresentationLabServer({port,files,identity:currentIdentity,incident:{directory:resolve(root,'artifacts/presentation-lab/incidents'),validate(report){if(report?.schema!=='th08/presentation-audit/1'||report?.timing?.schema!=='th08/presentation-timing-ring/1'||!Array.isArray(report.timing.rows)||report.timing.rows.length>512||!Number.isInteger(report.tick))throw Error('Invalid incident report');return String(report.tick);}}});
server.on('listening',()=>console.log(JSON.stringify({url:`http://127.0.0.1:${port}/`,wasm:identity.wasm,data:identity.dataAvailable,scope:'loopback reads / bounded same-origin incident write'})));start();
