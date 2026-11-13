// The first Sig/ASCII/text batch uses the actual independent CPU parser.
import {readFile,writeFile} from 'node:fs/promises';
import {Worker} from 'node:worker_threads';
import {resolve} from 'node:path';
import {pathToFileURL,fileURLToPath} from 'node:url';
import {createHash} from 'node:crypto';
import assert from 'node:assert/strict';
import {Th14WebGpuRuntime} from '../runtimes/th12/portable/sdl/Th14WebGpuRuntime.mjs';
const args=process.argv.slice(2);assert.equal(args.length,4);assert.equal(args[0],'--build');assert.equal(args[2],'--fixture');
const build=resolve(args[1]),home=resolve(args[3]),sha=b=>createHash('sha256').update(b).digest('hex');
const buildRaw=await readFile(resolve(build,'build.json')),fixtureRaw=await readFile(resolve(home,'build.json')),fixture=JSON.parse(fixtureRaw);
assert.equal(fixture.browserBuildSha256,sha(buildRaw));
for(const [name,key]of [['resources.mjs','loaderSha256'],['resources.wasm','wasmSha256']])assert.equal(sha(await readFile(resolve(home,name))),fixture[key]);
const {default:createModule}=await import(pathToFileURL(resolve(home,'resources.mjs'))),module=await createModule();
const adapter=resolve(home,'node-message-adapter.mjs');await writeFile(adapter,`import {parentPort} from 'node:worker_threads';
globalThis.self={postMessage:(m,t)=>parentPort.postMessage(m,t),addEventListener:(_,accept)=>parentPort.on('message',data=>accept({data}))};
await import(${JSON.stringify(pathToFileURL(resolve(build,'Th14ArchiveDecodeWorker.mjs')).href)});`,{flag:'wx'});
let invoke=()=>{},worker,pointer=0,report,copies=0;const corruptControls=[];
const runtime=new Th14WebGpuRuntime({ccall:()=>invoke()},{createArchiveWorker:()=>worker=new Worker(pathToFileURL(adapter),{type:'module'})});module.th14WebGpuRuntime=runtime;
const copy=runtime.archiveAnmCopy.bind(runtime);runtime.archiveAnmCopy=(name,bytes)=>{const result=copy(name,bytes);if(result===1)++copies;return result;};
const ordinaryCopy=runtime.archiveAnmCopy,names=['sig.anm','ascii.anm','text.anm'];
const deadline=setTimeout(()=>void worker?.terminate(),60000);
async function valid(){
    invoke=()=>{assert.equal(runtime.archiveStatus(),1);assert.equal(module._fixture_signature(),1);assert.equal(module._fixture_load(),1,module.UTF8ToString(module._fixture_error()));
        assert.equal(module._fixture_compare(),1);const before=copies;assert.equal(module._fixture_load(),1);assert.equal(copies,before,'Duplicate bootstrap must not read again');};
    await runtime.scheduleArchive(1,0,names).delivered;
}
try{
    const bytes=new Uint8Array(await readFile('H:/Games/th/14/th14.dat'));assert.equal(sha(bytes),'57d84a5cbe82460931cca26f090a0fde7aef180889ce5bfe8315851fc97b0233');
    pointer=module._malloc(bytes.length);module.HEAPU8.set(bytes,pointer);assert.equal(module._fixture_open(pointer,bytes.length),1);assert.equal(module._fixture_empty(),1,'Caller must leave initial resources unparsed');
    runtime.openArchive(bytes);assert.equal(await runtime.archiveOpening,227);await valid();assert.equal(copies,3);
    assert.equal(module._fixture_reset(),1);assert.equal(module._fixture_empty(),1);const before=copies;
    invoke=()=>{assert.equal(runtime.archiveStatus(),0);assert.equal(module._fixture_signature(),0);assert.equal(module._fixture_load(),0);assert.equal(module._fixture_empty(),1);};
    await runtime.scheduleArchive(1,0,['__missing__']).delivered;assert.equal(copies,before);
    for(const corruptName of names){
        assert.equal(module._fixture_reset(),1);assert.equal(module._fixture_empty(),1);
        invoke=()=>{
            runtime.archiveAnmCopy=(name,bytes)=>{const result=ordinaryCopy(name,bytes);if(result===1&&name===corruptName)bytes[bytes.length-1]^=1;return result;};
            try{if(corruptName==='sig.anm')assert.equal(module._fixture_signature(),0);else{assert.equal(module._fixture_signature(),1);assert.equal(module._fixture_load(),0);}}
            finally{runtime.archiveAnmCopy=ordinaryCopy;}
        };await runtime.scheduleArchive(1,0,names).delivered;corruptControls.push({name:corruptName,rejectedWithoutFallback:true});
    }
    assert.equal(module._fixture_reset(),1);await valid();assert.equal(copies,12);assert.equal(runtime.failure,null);
    report={passed:true,names,completeValidBatches:2,parsedCopies:12,corruptControls,callerDidNotParse:true,allAnmFieldsAndBytesEqual:true,actualProductionBootstrapOwner:true,actualIndependentCpuWorker:true,failedBatchCannotFallback:true,duplicateBootstrapDoesNotRead:true,subsequentValidBootstrapPassed:true,noGraphicsOrAudioDevices:true,actualApplicationControllerTest:false,browserBuildSha256:sha(buildRaw),fixtureBuildSha256:sha(fixtureRaw),checkerSha256:sha(await readFile(fileURLToPath(import.meta.url))),completeGame:false,nativeEquivalent:false};
}catch(error){report={passed:false,error:String(error.stack??error),copies,corruptControls,completeGame:false,nativeEquivalent:false};process.exitCode=1;}
finally{clearTimeout(deadline);runtime.archive?.close();if(pointer)module._free(pointer);}
await writeFile(resolve(home,'check.json'),JSON.stringify(report,null,2)+'\n');console.log(JSON.stringify(report));
