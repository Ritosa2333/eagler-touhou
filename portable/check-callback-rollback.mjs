import {parseMakeDependencies} from './make-dependencies.mjs';
import {spawnSync} from 'node:child_process';
import {existsSync,mkdirSync,readFileSync,writeFileSync,openSync,closeSync} from 'node:fs';
import {resolve,relative,dirname,isAbsolute,sep} from 'node:path';
import {fileURLToPath} from 'node:url';
import {createHash} from 'node:crypto';
import {WASI} from 'node:wasi';

// Usage: WASI_SDK_PATH=... node portable/check-callback-rollback.mjs
// Optional: --out artifacts/.../fresh-directory ; TH10_TEST_UBSAN=1.
// This builds resource-free WASI units only. It never builds or loads a game,
// edits production sources, starts a browser, or changes the frozen r10e WASM.
// These two literal fixtures preserve the exact CRLF bytes read before the
// allocation barrier was added (1875-byte header and 1437-byte implementation).
const LEGACY_HEADER = "#pragma once\r\n#include \"../game/UpdateChain.hpp\"\r\n#include <map>\r\n#include <array>\r\n#include \"../game/CallbackNames.hpp\"\r\n#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY\r\n#include \"../multiplayer/RollbackPool.hpp\"\r\n#endif\r\nnamespace th10::browser {\r\nstruct Callbacks;\r\nstruct CallbackReceiver {\r\n    void* callback_context=this;\r\n#ifndef TH_NATIVE_PLATFORM\r\n    virtual bool invoke(CallbackToken token,void* owner,i32& result)=0;\r\n#endif\r\n    virtual void bind_callbacks(Callbacks&){}\r\n};\r\nstruct Callbacks final:UpdateChainEnvironment {\r\n    CallbackReceiver& receiver;u8 depth=0;u32 calls=0;\r\n#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY\r\n    multiplayer::RollbackPool<sizeof(UpdateChainEntry),4096> rollback_entries{};\r\n#endif\r\n    explicit Callbacks(CallbackReceiver& receiver):receiver(receiver){lock_depth=&depth;}\r\n    using NativeFunction=NativeCallback::Function;\r\n    using Binding=NativeCallback;\r\n #ifdef TH_NATIVE_PLATFORM\r\n    std::array<Binding,callback_id::Count> native{};\r\n    void bind(CallbackToken token,void* context,NativeFunction function,i32 parameter=0){if(token==0||token>=native.size())__builtin_trap();native[token]={context,function,parameter};}\r\n    void unbind(void* context){for(auto& binding:native)if(binding.context==context)binding={};}\r\n #else\r\n    std::map<CallbackToken,Binding> native;\r\n    void bind(CallbackToken token,void* context,NativeFunction function,i32 parameter=0){native[token]={context,function,parameter};}\r\n    void unbind(void* context){for(auto it=native.begin();it!=native.end();)if(it->second.context==context)it=native.erase(it);else ++it;}\r\n #endif\r\n    void enter_lock() override {}\r\n    void leave_lock() override {}\r\n    i32 invoke(CallbackToken,void*) override;\r\n    NativeCallback resolve(CallbackToken) override;\r\n    UpdateChainEntry* allocate_entry() override;\r\n    void release_entry(UpdateChainEntry*) override;\r\n};\r\n}\r\n";
const LEGACY_SOURCE = "#include \"Callbacks.hpp\"\r\n#include <cstdlib>\r\n#include <cstdio>\r\nnamespace th10::browser {\r\ni32 Callbacks::invoke(CallbackToken token,void* owner){++calls;\r\n#ifdef TH_NATIVE_PLATFORM\r\n    if(token<native.size()&&native[token].function)return native[token](owner);\r\n    std::fprintf(stderr,\"Unbound named game callback %u\\n\",token);std::abort();\r\n#else\r\n    const auto found=native.find(token);if(found!=native.end())return found->second(owner);\r\n    i32 result=0;if(!receiver.invoke(token,owner,result))__builtin_trap();return result;\r\n#endif\r\n}\r\nNativeCallback Callbacks::resolve(CallbackToken token){\r\n#ifdef TH_NATIVE_PLATFORM\r\n    if(token<native.size()&&native[token].function)return native[token];\r\n    std::fprintf(stderr,\"Unbound named update job %u\\n\",token);std::abort();\r\n#else\r\n    const auto found=native.find(token);if(found!=native.end())return found->second;\r\n    return UpdateChainEnvironment::resolve(token);\r\n#endif\r\n}\r\nUpdateChainEntry* Callbacks::allocate_entry(){\r\n#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY\r\n    return static_cast<UpdateChainEntry*>(rollback_entries.allocate(sizeof(UpdateChainEntry),true));\r\n#else\r\n    return static_cast<UpdateChainEntry*>(std::calloc(1,sizeof(UpdateChainEntry)));\r\n#endif\r\n}\r\nvoid Callbacks::release_entry(UpdateChainEntry* entry){\r\n#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY\r\n    if(entry&&!rollback_entries.release(entry))__builtin_trap();\r\n#else\r\n    std::free(entry);\r\n#endif\r\n}\r\n}\r\n";

const driver=fileURLToPath(import.meta.url);
const root=resolve(dirname(driver),'..');
const sdk=process.env.WASI_SDK_PATH;
if(!sdk)throw Error('Set WASI_SDK_PATH to an installed WASI SDK directory.');
const compiler=resolve(sdk,'bin',process.platform==='win32'?'clang++.exe':'clang++');
if(!existsSync(compiler))throw Error('WASI compiler not found: '+compiler);
let requestedOut=null;
for(let i=2;i<process.argv.length;++i){
    if(process.argv[i]==='--out'&&!requestedOut&&process.argv[i+1])requestedOut=process.argv[++i];
    else throw Error('Usage: node portable/check-callback-rollback.mjs [--out artifacts/.../fresh-directory]');
}
const sanitized=process.env.TH10_TEST_UBSAN==='1';
const suffix=new Date().toISOString().replace(/[:.]/g,'-')+(sanitized?'-ubsan':'');
const out=resolve(root,requestedOut||'artifacts/callback-rollback/run-'+suffix);
const outRelative=relative(resolve(root,'artifacts'),out);
if(!outRelative||outRelative==='..'||outRelative.startsWith('..'+sep)||isAbsolute(outRelative))
    throw Error('Output must be a new directory inside this checkout artifacts directory.');
if(existsSync(out))throw Error('Refusing to overwrite an existing receipt directory: '+out);
mkdirSync(out,{recursive:true});

const receiptPath=resolve(out,'receipt.json');
const hash=bytes=>createHash('sha256').update(bytes).digest('hex');
const normalized=text=>text.replace(/\r\n/g,'\n');
const rel=path=>relative(root,path).replaceAll('\\','/');
const platform=resolve(root,'th10_web/cpp/platform');
const legacy=resolve(out,'legacy/th10_web/cpp/platform');
mkdirSync(legacy,{recursive:true});
const oldHeader=resolve(legacy,'Callbacks.hpp'),oldSource=resolve(legacy,'Callbacks.cpp');
writeFileSync(oldHeader,LEGACY_HEADER);writeFileSync(oldSource,LEGACY_SOURCE);
if(Buffer.byteLength(LEGACY_HEADER)!==1875||Buffer.byteLength(LEGACY_SOURCE)!==1437)
    throw Error('Pinned pre-change callback fixture byte lengths changed.');

const receipt={
    schema:1,suite:'callback-rollback',passed:false,status:'RUNNING',startedAt:new Date().toISOString(),
    runtime:'wasm32-wasip1',node:process.version,sanitized,
    scope:'Production Callbacks and UpdateChain with the actual pool capture and common journal; independent same-owner full-byte snapshots.',
    productionExecution:['Callbacks::allocate_entry','Callbacks::release_entry','Callbacks::resolve',
        'UpdateChain::add','UpdateChain::allocate','UpdateChain::insert','UpdateChain::run',
        'UpdateChain::remove_locked','RollbackPoolCapture::Capture','RollbackPoolCapture::TouchSlot',
        'Netplay::RollbackJournal::Touch','Netplay::RollbackJournal::EndFrame','Netplay::RollbackJournal::UndoTo'],
    notCovered:['AnimationEngine runtime construction and forwarding execution','RollbackState::BeginFrame/EndFrame with a real World',
        'Application::multiplayer_finalize_frame and MarkSimulated','rendering, actual RTC, gameplay performance, or the earlier 700 ms stalls'],
    failureContract:'The MP preserve hook latches journal failure. The already allocated pointer remains non-null and payload initialization can continue; EndFrame must reject. This unit does not claim execution of the Application submission gate.',
    negativeControl:'Separate WASM linked with the exact pinned pre-change Callbacks.hpp/.cpp; same test oracle, unchanged shared UpdateChain/Journal objects. Only the legacy test override specifier/report label differs.',
    fixtures:{
        header:{path:rel(oldHeader),bytes:Buffer.byteLength(LEGACY_HEADER),sha256:hash(Buffer.from(LEGACY_HEADER))},
        source:{path:rel(oldSource),bytes:Buffer.byteLength(LEGACY_SOURCE),sha256:hash(Buffer.from(LEGACY_SOURCE))}
    },
    commands:[],results:{},errors:[]
};
function saveReceipt(){writeFileSync(receiptPath,JSON.stringify(receipt,null,2)+'\n');}
saveReceipt();

function checkedCommand(label,args){
    const started=Date.now();
    const result=spawnSync(compiler,args,{
        cwd:root,shell:false,windowsHide:true,encoding:'utf8',maxBuffer:8*1024*1024
    });
    const safe=label.replace(/[^a-zA-Z0-9_.-]/g,'_');
    const stdout=resolve(out,safe+'.stdout.log'),stderr=resolve(out,safe+'.stderr.log');
    writeFileSync(stdout,result.stdout||'');writeFileSync(stderr,result.stderr||'');
    receipt.commands.push({label,executable:compiler,args,status:result.status,signal:result.signal,
        milliseconds:Date.now()-started,stdout:rel(stdout),stderr:rel(stderr),
        error:result.error?String(result.error):null});
    saveReceipt();
    if(result.error||result.status!==0)
        throw Error(label+' failed; see '+rel(stderr)+(result.error?': '+String(result.error):''));
    return result.stdout||'';
}
function requireCondition(condition,message){if(!condition)throw Error(message);}
function replaceOnce(source,find,replacement,label){
    requireCondition(source.includes(find)&&source.indexOf(find)===source.lastIndexOf(find),
        'Current production source no longer matches the isolated barrier change: '+label);
    return source.replace(find,replacement);
}
const explicitSources=[
    driver,resolve(root,'portable/make-dependencies.mjs'),
    resolve(platform,'Callbacks.hpp'),resolve(platform,'Callbacks.cpp'),
    resolve(platform,'AnimationEngine.hpp'),resolve(platform,'AnimationEngine.cpp'),
    oldHeader,oldSource
];
function dependencyPaths(files){
    const result=new Set(explicitSources);
    for(const file of files)for(const token of parseMakeDependencies(readFileSync(file,'utf8'))){
        const path=resolve(root,token),name=relative(root,path);
        if(name==='..'||name.startsWith('..'+sep)||isAbsolute(name))
            throw Error('Project dependency outside checkout: '+path);
        result.add(path);
    }
    return [...result].sort();
}
function inventory(paths){
    return Object.fromEntries(paths.map(path=>[rel(path),hash(readFileSync(path))]));
}
function sameInventory(first,second,stage){
    requireCondition(JSON.stringify(first)===JSON.stringify(second),'Source closure or bytes changed '+stage);
}
const baseFlags=[
    '--target=wasm32-wasip1','-O2','-std=c++17','-Wall','-Wextra','-Werror',
    '-DTH_NATIVE_PLATFORM=1','-DTH_ENABLE_MULTIPLAYER_GAMEPLAY=1',
    '-fno-exceptions','-fno-rtti','-ffp-contract=off','-fno-strict-aliasing',
    '-I',resolve(root,'third_party/eagler-common/include')
];
if(sanitized)baseFlags.push('-fsanitize=undefined','-fsanitize-trap=undefined');
const linkFlags=['--target=wasm32-wasip1','-Wl,-z,stack-size=1048576'];
const testSource=resolve(root,'tests/callback-rollback-test.cpp');
const units=[
    {name:'shared-update-chain',source:resolve(root,'th10_web/cpp/game/UpdateChain.cpp'),flags:baseFlags},
    {name:'shared-journal',source:resolve(root,'third_party/eagler-common/src/netplay/RollbackJournal.cpp'),flags:baseFlags},
    {name:'candidate-test',source:testSource,flags:[...baseFlags,'-I',platform]},
    {name:'candidate-callbacks',source:resolve(platform,'Callbacks.cpp'),flags:[...baseFlags,'-I',platform]},
    {name:'legacy-test',source:testSource,flags:[...baseFlags,'-DTH10_CALLBACK_ROLLBACK_LEGACY=1','-I',legacy,'-I',platform]},
    {name:'legacy-callbacks',source:oldSource,flags:[...baseFlags,'-DTH10_CALLBACK_ROLLBACK_LEGACY=1','-I',legacy,'-I',platform]}
].map(unit=>({...unit,object:resolve(out,unit.name+'.o'),
    discovery:resolve(out,unit.name+'.before.d'),depfile:resolve(out,unit.name+'.d')}));
receipt.compileFlags=baseFlags;receipt.linkFlags=linkFlags;

async function executeUnit(name,wasm){
    const stdoutPath=resolve(out,name+'.jsonl'),stderrPath=resolve(out,name+'.stderr.log');
    const stdout=openSync(stdoutPath,'w'),stderr=openSync(stderrPath,'w');
    let exitCode=null,trap=null;
    const wasmBytes=readFileSync(wasm);
    try{
        const wasi=new WASI({version:'preview1',args:[name],env:{},returnOnExit:true,stdout,stderr});
        const {instance}=await WebAssembly.instantiate(wasmBytes,{wasi_snapshot_preview1:wasi.wasiImport});
        exitCode=wasi.start(instance);
    }catch(error){trap=String(error);}
    finally{closeSync(stdout);closeSync(stderr);}
    const data=readFileSync(stdoutPath);
    requireCondition(data.length<=65536,'Unit stdout exceeded its bounded summary: '+name);
    const lines=data.toString('utf8').trim().split(/\r?\n/).filter(Boolean);
    let summary=null;
    if(lines.length===1){try{summary=JSON.parse(lines[0]);}catch{}}
    const result={wasm:rel(wasm),wasmSha256:hash(wasmBytes),exitCode,trap,
        stdout:rel(stdoutPath),stderr:rel(stderrPath),summary};
    receipt.results[name]=result;saveReceipt();
    requireCondition(!trap&&summary&&summary.complete===true&&summary.schema===1&&
        summary.suite==='callback-rollback','Unit did not complete the oracle: '+name);
    return result;
}
function validateCases(result,legacyMode){
    const s=result.summary;
    const pairs=['cold-inactive','poisoned-inactive','active-release-reuse',
        'inactive-create-release-reuse','history-ring-replay','byte-budget-failure','block-budget-failure'];
    const expected=pairs.flatMap(name=>[name+':false',name+':true']);
    expected.push('outside-capture:false','exhausted-pool-existing-result:false');
    requireCondition(Array.isArray(s.cases)&&s.cases.length===16,'Missing unit cases');
    const actual=s.cases.map(item=>item.name+':'+item.packed).sort();
    requireCondition(JSON.stringify(actual)===JSON.stringify(expected.sort()),'Unexpected unit coverage');
    requireCondition(s.legacy===legacyMode&&s.applicationLinked===false&&
        s.applicationMarkSimulatedCovered===false,'Unit scope tag mismatch');
    requireCondition(s.capacity===4096&&s.storageStride>=s.payloadBytes&&s.payloadBytes>0,'Unexpected callback layout');
    requireCondition(s.checks>0&&s.fullPoolComparisons>=24&&s.preclearObservations===6&&
        s.budgetCases===4&&s.budgetPayloadChanged===4,'Insufficient full-byte/budget coverage');
    requireCondition(s.positiveControlFailures===0,'A positive control failed');
    if(!legacyMode){
        requireCondition(result.exitCode===0&&s.passed===true&&s.failures===0&&
            s.fullPoolMismatches===0&&s.preclearMisses===0&&s.budgetRejections===4&&
            s.budgetUnexpectedSuccesses===0&&s.cases.every(item=>item.failures===0),
            'Candidate callback rollback oracle failed');
    }else{
        requireCondition(result.exitCode===1&&s.passed===false&&s.failures>0&&
            s.fullPoolMismatches>=6&&s.preclearMisses===6&&s.budgetRejections===0&&
            s.budgetUnexpectedSuccesses===4,'Pinned old source did not reproduce the missing barrier');
        for(const item of s.cases){
            const positive=['active-release-reuse','outside-capture','exhausted-pool-existing-result'].includes(item.name);
            requireCondition(positive?item.failures===0:item.failures>0,
                'Legacy failure occurred outside its expected oracle boundary: '+item.name);
        }
    }
}

try{
    receipt.compiler=checkedCommand('compiler-version',['--version']).trim();
    // Require that this negative control isolates exactly the allocation
    // barrier against its real pre-change files; normalize line endings only
    // for this textual diff, while all inventories/pins retain raw byte hashes.
    const hookDeclaration = "#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY\n    // MP allocation barrier, before any new callback payload is cleared.\n    // The owner latches journal failure; this is not a nullable allocation.\n    virtual void preserve_callback_entry(UpdateChainEntry*){}\n#endif\n";
    const allocationReplacement = "    auto* entry=static_cast<UpdateChainEntry*>(rollback_entries.allocate(sizeof(UpdateChainEntry)));\n    if(!entry)return nullptr; // Preserve the existing exhausted-pool result.\n    // Metadata was captured at BeginFrame. An initially dormant slot still\n    // needs its complete preimage before calloc semantics clear the payload.\n    // A journal failure is sticky and rejects EndFrame; as with ANM first\n    // writes, allocation itself remains non-null and may finish initialization.\n    receiver.preserve_callback_entry(entry);\n    std::memset(entry,0,sizeof(UpdateChainEntry));\n    return entry;";
    const header=normalized(readFileSync(resolve(platform,'Callbacks.hpp'),'utf8'));
    const source=normalized(readFileSync(resolve(platform,'Callbacks.cpp'),'utf8'));
    requireCondition(replaceOnce(header,hookDeclaration,'','header hook')===normalized(LEGACY_HEADER),
        'Current Callbacks.hpp has changes beyond the pinned allocation barrier');
    const restoredInclude=replaceOnce(source,'#include <cstring>\n','','cstring include');
    const restored=replaceOnce(restoredInclude,allocationReplacement,
        '    return static_cast<UpdateChainEntry*>(rollback_entries.allocate(sizeof(UpdateChainEntry),true));',
        'allocator barrier');
    requireCondition(restored===normalized(LEGACY_SOURCE),
        'Current Callbacks.cpp has changes beyond the pinned allocation barrier');
    receipt.sourceDiffScope='Only MP receiver hook, explicit cstring include and allocate-before-preserve-before-original-clear; exact pre-change text after reversing those edits (line endings normalized solely for this comparison).';
    for(const unit of units)
        checkedCommand('discover-'+unit.name,[...unit.flags,'-MM','-MF',unit.discovery,'-MT','source',unit.source]);
    const before=inventory(dependencyPaths(units.map(unit=>unit.discovery)));
    receipt.sourcesBefore=before;saveReceipt();
    for(const unit of units)
        checkedCommand('compile-'+unit.name,[...unit.flags,'-MMD','-MF',unit.depfile,'-MT','source',
            '-c',unit.source,'-o',unit.object]);
    sameInventory(before,inventory(dependencyPaths(units.map(unit=>unit.depfile))),'during compilation');
    const shared=units.slice(0,2).map(unit=>unit.object);
    const candidateWasm=resolve(out,'candidate.wasm'),legacyWasm=resolve(out,'legacy.wasm');
    checkedCommand('link-candidate',[...linkFlags,...shared,units[2].object,units[3].object,'-o',candidateWasm]);
    checkedCommand('link-legacy',[...linkFlags,...shared,units[4].object,units[5].object,'-o',legacyWasm]);
    const candidate=await executeUnit('candidate',candidateWasm);
    const legacyResult=await executeUnit('legacy',legacyWasm);
    validateCases(candidate,false);validateCases(legacyResult,true);
    const sources=inventory(dependencyPaths(units.map(unit=>unit.depfile)));
    sameInventory(before,sources,'before verification completed');
    receipt.sources=sources;receipt.sourceSha256=hash(JSON.stringify(sources));
    receipt.dependencyScope='Clang -MM/-MMD non-system closure + runner/Windows-safe Make parser + four production integration files and exact legacy pins; SDK system headers/libraries excluded.';
    receipt.sourceStability='Same raw-byte inventory before compilation, after compilation and after execution.';
    receipt.status='PASS';receipt.passed=true;
}catch(error){
    receipt.status='FAIL';receipt.errors.push(String(error));process.exitCode=1;
}finally{
    receipt.finishedAt=new Date().toISOString();saveReceipt();
}
console.log('Callback rollback '+receipt.status+': '+receiptPath);
