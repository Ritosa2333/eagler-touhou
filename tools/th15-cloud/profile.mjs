// Diagnostic sampling only. No timing from this file can satisfy a performance gate.
import assert from 'node:assert/strict';
import {readFileSync,writeFileSync,readdirSync,mkdirSync,rmSync,existsSync} from 'node:fs';
import {resolve,basename} from 'node:path';
import {createHash} from 'node:crypto';
import os from 'node:os';
import {runCloudCommand} from './cloud-command-runner.mjs';
const root=resolve(import.meta.dirname,'../..'),port=resolve(root,'ports/th15-mp');
const out=resolve(root,'artifacts/th15-cloud-diagnostic');mkdirSync(out,{recursive:true});
const buildOut=resolve(port,'web/artifacts/sdl-multiplayer');
const digest=p=>createHash('sha256').update(readFileSync(p)).digest('hex');
const report={schema:'th15-cloud-diagnostic/1',head:process.env.GITHUB_SHA??null,startedAt:new Date().toISOString(),commands:[],status:'running',performanceEligible:false,sixtyHzAccepted:false,
 host:{platform:process.platform,cpu:os.cpus()[0]?.model,logicalCpus:os.cpus().length,node:process.version}};
const save=()=>writeFileSync(resolve(out,'summary.json'),JSON.stringify(report,null,2)+'\n');
function run(name,args,{required=true,timeout=300000}={}){
 console.log('BEGIN '+name);const command=runCloudCommand(process.execPath,args,{cwd:root,log:resolve(out,name+'.log'),timeout});
 report.commands.push({name,...command});save();console.log('END '+name+' exit='+command.exitCode+' cleanup='+command.cleanup.verified);
 if(!command.cleanup.verified)throw Error('Owned process group cleanup not established: '+name);
 if(required&&(command.error||command.exitCode!==0))throw Error('Command failed: '+name);return command;
}
try{
 const builder=resolve(port,'web/scripts/build-sdl-application.mjs'),source=readFileSync(builder,'utf8');
 const anchor="await run([...common,...(!release&&process.argv.includes('--profile')?['--profiling-funcs']:[]),";
 assert.equal(source.split(anchor).length,2,'Exact release link anchor required');
 report.before={wasm:digest(resolve(buildOut,'th15.wasm')),loader:digest(resolve(buildOut,'th15.mjs')),builder:digest(builder)};
 const temporary=resolve(port,'web/scripts/.cloud-symbol-map-'+process.pid+'.mjs');
 const modified=source.replace(anchor,anchor+"'--emit-symbol-map',");
 report.symbolBuildScriptSha256=createHash('sha256').update(modified).digest('hex');
 writeFileSync(temporary,modified,{flag:'wx'});
 try{run('symbol-relink',[temporary,'--release','--multiplayer']);}finally{rmSync(temporary,{force:true});}
 report.after={wasm:digest(resolve(buildOut,'th15.wasm')),loader:digest(resolve(buildOut,'th15.mjs')),builder:digest(builder)};
 assert.deepEqual(report.after,report.before,'Symbol-map relink changed the frozen runtime or source');
 report.binaryUnchanged=true;
 const maps=readdirSync(buildOut).filter(p=>p.endsWith('.symbols'));assert.equal(maps.length,1,'Unique linker symbol map required');
 const map=resolve(buildOut,maps[0]);report.symbols={name:basename(map),sha256:digest(map)};
 writeFileSync(resolve(out,'th15.symbols'),readFileSync(map));
 writeFileSync(resolve(out,'build.json'),readFileSync(resolve(buildOut,'build.json')));save();
 for(const name of ['TH15_TEST_ARCHIVE','TH15_TEST_FONT','TH15_TEST_UNICODE_FONT','EAGLER_FONT_ROOT','EAGLER_CHROME_PATH'])assert.ok(process.env[name],name+' missing');
 run('package',[resolve(port,'portable/package-eagler.mjs'),'--multiplayer']);
 const destination=resolve(out,'cpu4-2p-mode0');
 const command=run('profile',[resolve(port,'portable/multiplayer/performance-browser.mjs'),'--players=2','--mode=0','--duration=15','--cpu-throttle=4','--renderer=swiftshader','--diagnostic=all','--runtime='+resolve(port,'build-eagler-multiplayer'),'--output='+destination],{required:false,timeout:300000});
 const final=resolve(destination,'receipt.final.json');
 report.receipt=existsSync(final)?{path:final,sha256:digest(final)}:null;
 const result=existsSync(final)?JSON.parse(readFileSync(final,'utf8')):null;
 report.status=command.exitCode===0?'diagnostic-completed':'diagnostic-failed';
 report.observationPassed=result?.observationPassed??false;report.saveValidationPassed=result?.saveValidationPassed??false;
 report.profileCount=result?.diagnostics?.profiles?.filter(p=>p.stopped&&p.file&&!p.error).length??0;
 if(command.exitCode!==0)process.exitCode=1;
}catch(error){report.status='failed';report.error=String(error);process.exitCode=1;}
finally{report.completedAt=new Date().toISOString();save();console.log(JSON.stringify(report));}
