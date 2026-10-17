import {parseMakeDependencies} from './make-dependencies.mjs';
import {spawnSync} from 'node:child_process';
import {existsSync,mkdirSync,readFileSync,writeFileSync,openSync,closeSync,statSync} from 'node:fs';
import {resolve,relative,dirname,isAbsolute,sep} from 'node:path';
import {fileURLToPath} from 'node:url';
import {createHash} from 'node:crypto';
import {WASI} from 'node:wasi';

// Usage: WASI_SDK_PATH=... node portable/check-anm-overflow-rollback.mjs
// Optional: --out artifacts/.../fresh-directory ; TH10_TEST_UBSAN=1.
// Resource-free WASI units: construct the actual production AnimationEngine
// and call AnmManager create/create_at through the actual overflow allocator.
// No game build, browser, test-site deployment, asset reads, or GPU draws.
// The counterfactual copies the CURRENT complete AnimationEngine.cpp bytes,
// replacing only the seven-line MP overflow body with its old allocation.
// It is not a pinned historical AnimationEngine.cpp or a function excerpt.
// Only RollbackState::Touch is supplied by the explicit test delegate; the
// production RollbackState dispatcher/World/Application gates are not linked.

const NEW_ALLOCATION=[
    '    auto* vm=static_cast<AnmVm*>(rollback_animation_overflow.allocate(sizeof(AnmVm)));',
    '    if(!vm)return nullptr; // Preserve the existing exhausted-pool result.',
    '    // Overflow slots are sparse-captured just like callback slots. Preserve',
    '    // their complete preimage before the original zero/initialize sequence.',
    '    preserve_animation_slot(*vm);',
    '    std::memset(vm,0,sizeof(AnmVm));',
    '    return vm;'
].join('\r\n');
const OLD_ALLOCATION='    return static_cast<AnmVm*>(rollback_animation_overflow.allocate(sizeof(AnmVm),true));';
const ALLOCATION_PREFIX='AnmVm* AnimationEngine::allocate_animation(){\r\n#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY\r\n';
const ALLOCATION_SUFFIX='\r\n#else\r\n    return static_cast<AnmVm*>(std::malloc(sizeof(AnmVm)));\r\n#endif\r\n}';

const driver=fileURLToPath(import.meta.url);
const root=resolve(dirname(driver),'..');
const commonRoot=resolve(process.env.EAGLER_COMMON_ROOT||resolve(root,'third_party/eagler-common'));
const sdk=process.env.WASI_SDK_PATH;
if(!sdk)throw Error('Set WASI_SDK_PATH to an installed WASI SDK directory.');
const compiler=resolve(sdk,'bin',process.platform==='win32'?'clang++.exe':'clang++');
if(!existsSync(compiler))throw Error('WASI compiler not found: '+compiler);
let requestedOut=null;
for(let i=2;i<process.argv.length;++i){
    if(process.argv[i]==='--out'&&!requestedOut&&process.argv[i+1])requestedOut=process.argv[++i];
    else throw Error('Usage: node portable/check-anm-overflow-rollback.mjs [--out artifacts/.../fresh-directory]');
}
const sanitized=process.env.TH10_TEST_UBSAN==='1';
const suffix=new Date().toISOString().replace(/[:.]/g,'-')+(sanitized?'-ubsan':'');
const out=resolve(root,requestedOut||'artifacts/anm-overflow-rollback/run-'+suffix);
const outRelative=relative(resolve(root,'artifacts'),out);
if(!outRelative||outRelative==='..'||outRelative.startsWith('..'+sep)||isAbsolute(outRelative))
    throw Error('Output must be a new directory inside this checkout artifacts directory.');
if(existsSync(out))throw Error('Refusing to overwrite an existing receipt directory: '+out);
mkdirSync(out,{recursive:true});

const receiptPath=resolve(out,'receipt.json');
const hash=bytes=>createHash('sha256').update(bytes).digest('hex');
const rel=path=>relative(root,path).replaceAll('\\','/');
const platform=resolve(root,'th10_web/cpp/platform');
const engineSource=resolve(platform,'AnimationEngine.cpp');
const legacyDirectory=resolve(out,'counterfactual/th10_web/cpp/platform');
const legacySource=resolve(legacyDirectory,'AnimationEngine.cpp');
const testSource=resolve(root,'tests/anm-overflow-rollback-test.cpp');
const receipt={
    schema:1,suite:'anm-overflow-rollback',passed:false,status:'RUNNING',startedAt:new Date().toISOString(),
    runtime:'wasm32-wasip1',node:process.version,sanitized,
    scope:'Full real AnimationEngine construction/destruction and actual AnmManager create/create_at, overflow allocation, preservation and release. Test-only RollbackState::Touch delegates to actual pool capture and common journal; independent same-owner full-byte snapshots.',
    productionExecution:['AnimationEngine::AnimationEngine','AnimationEngine::~AnimationEngine',
        'AnimationEngine::allocate_animation','AnimationEngine::preserve_animation_slot',
        'AnimationEngine::release_memory','AnmManager::allocate','AnmManager::create',
        'AnmManager::create_at','AnmManager::insert','AnmManager::remove',
        'AnmVm::clear','AnmVm::initialize','AnmFile::prepare_script','AnmFile::initialize_script',
        'RollbackPoolCapture::Capture','RollbackPoolCapture::TouchSlot',
        'Netplay::RollbackJournal::Touch','Netplay::RollbackJournal::EndFrame','Netplay::RollbackJournal::UndoTo'],
    notCovered:['Production RollbackState::Touch dispatch and BeginFrame/EndFrame with a real World',
        'Application::multiplayer_finalize_frame and MarkSimulated',
        'GPU rendering, actual RTC, gameplay performance, or the earlier 700 ms stalls',
        'Vendored SoftFloat sanitizer coverage'],
    hostBoundary:'Real FileSystem/AnimationResources/GraphicsDevice objects; FileHost calls abort. GraphicsHost permits only the real constructor/destructor model-buffer, shader, transform and viewport operations. No GPU implementation is linked.',
    touchBoundary:'A legally constructed real RollbackState object identifies the active test fixture; its test-only Touch definition delegates to actual RollbackPoolCapture/Journal. No allocator, AnmManager method, or AnimationEngine method is replaced.',
    failureContract:'Preservation failure is sticky in the common journal. The already allocated pointer remains non-null and payload initialization can continue; common EndFrame must reject. This unit does not execute the Application submission gate.',
    negativeControl:'Separate WASM with counterfactual old-overflow-body fixture: current complete raw AnimationEngine.cpp copied with only the seven-line MP allocation body replaced by its former one-line allocate-with-clear. Same test oracle and shared production objects; the legacy test macro changes the report label only. No historical whole-file pin is claimed.',
    sanitizer:{enabled:sanitized,softfloatIncluded:false,scope:sanitized?'C++ tests and linked production C++ only; vendored SoftFloat remains O2 without instrumentation':'none'},
    commands:[],results:{},errors:[]
};
function saveReceipt(){writeFileSync(receiptPath,JSON.stringify(receipt,null,2)+'\n');}
function requireCondition(condition,message){if(!condition)throw Error(message);}
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
function replaceUniqueBytes(source,find,replacement,label){
    const first=source.indexOf(find);
    requireCondition(first>=0&&source.indexOf(find,first+find.length)===-1,
        'Current source does not contain exactly one frozen allocation body: '+label);
    return Buffer.concat([source.subarray(0,first),replacement,source.subarray(first+find.length)]);
}
const explicitSources=[
    driver,resolve(root,'portable/make-dependencies.mjs'),testSource,
    engineSource,resolve(platform,'AnimationEngine.hpp'),
    resolve(platform,'Callbacks.hpp'),resolve(platform,'Callbacks.cpp'),legacySource
];
function dependencyPaths(files){
    const result=new Set(explicitSources);
    for(const file of files)for(const token of parseMakeDependencies(readFileSync(file,'utf8'))){
        const path=resolve(root,token),name=relative(root,path);
        const commonName=relative(commonRoot,path);
        const inCommon=commonName!=='..'&&!commonName.startsWith('..'+sep)&&!isAbsolute(commonName);
        if((name==='..'||name.startsWith('..'+sep)||isAbsolute(name))&&!inCommon)
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
    '--target=wasm32-wasip1','-O2','-std=c++17','-Wall','-Wextra','-Werror','-Wno-unused-function',
    '-DTH_NATIVE_PLATFORM=1','-DTH_ENABLE_MULTIPLAYER_GAMEPLAY=1',
    '-fno-exceptions','-fno-rtti','-ffp-contract=off','-fno-strict-aliasing',
    '-I',resolve(commonRoot,'include'),'-I',platform
];
if(sanitized)baseFlags.push('-fsanitize=undefined','-fsanitize-trap=undefined');
const softfloatFlags=['--target=wasm32-wasip1','-O2','-x','c','-std=c11','-DSOFTFLOAT_FAST_INT64','-DINLINE_LEVEL=5'];
const linkFlags=['--target=wasm32-wasip1','-Wl,-z,stack-size=1048576'];
// Real constructors retain their virtual methods and registered callbacks.
// Keep those production translation units, rather than extracting allocator
// functions or dropping unresolved symbols from an incomplete engine fixture.
const gameNames=[
    'ResourceArchive','ArchiveLifecycle','ResourceCodec','ResourceFiles',
    'AnmResources','AnmTexture','TexturePlatform','AnmFile','AnmScript',
    'AnmVm','AnmInterpreter','AnmVariables','AnmRenderer','Arithmetic',
    'GameMath','Timer','Interpolation','Rng','AnmManager','AnmSystems',
    'AnmFrame','AnmLayers','AnmProjection','AnmDistortion','AnmRegistry',
    'AnmMesh','Camera','GraphicsMath','UpdateChain'
];
const platformNames=['AnimationResources','TransitionResources','Textures','FileSystem','Callbacks','Graphics'];
const sharedSources=[
    ...gameNames.map(name=>'th10_web/cpp/game/'+name+'.cpp'),
    ...platformNames.map(name=>'th10_web/cpp/platform/'+name+'.cpp'),
    resolve(commonRoot,'src/netplay/RollbackJournal.cpp').replaceAll('\\','/'),
    'tests/live-motion-disabled-stub.cpp'
];
const units=[
    // This pre-existing TU has an unused read_le template. Keep -Werror and
    // all unit warnings; suppress only that existing TU's template warning.
    ...sharedSources.map((source,index)=>({name:'shared-'+index+'-'+source.split('/').at(-1).replace('.cpp',''),source:resolve(root,source),
        flags:source==='th10_web/cpp/platform/AnimationResources.cpp'?[...baseFlags,'-Wno-unused-template']:baseFlags})),
    {name:'shared-softfloat',source:resolve(root,'th10_web/cpp/rebuild/third_party/softfloat.c'),flags:softfloatFlags},
    {name:'candidate-test',source:testSource,flags:baseFlags},
    {name:'candidate-engine',source:engineSource,flags:baseFlags},
    {name:'legacy-test',source:testSource,flags:[...baseFlags,'-DTH10_ANM_OVERFLOW_ROLLBACK_LEGACY=1']},
    {name:'legacy-engine',source:legacySource,flags:baseFlags}
].map(unit=>({...unit,object:resolve(out,unit.name+'.o'),
    discovery:resolve(out,unit.name+'.before.d'),depfile:resolve(out,unit.name+'.d')}));
receipt.compileFlags=baseFlags;receipt.softfloatFlags=softfloatFlags;receipt.linkFlags=linkFlags;
receipt.translationUnits=units.map(({name,source,flags})=>({name,source:rel(source),flags}));

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
    const length=statSync(stdoutPath).size;
    requireCondition(length>0&&length<=65536,'Unit stdout is absent or exceeds its bounded summary: '+name);
    const lines=readFileSync(stdoutPath,'utf8').trim().split(/\r?\n/).filter(Boolean);
    let summary=null;
    if(lines.length===1){try{summary=JSON.parse(lines[0]);}catch{}}
    const result={wasm:rel(wasm),wasmSha256:hash(wasmBytes),exitCode,trap,
        stdout:rel(stdoutPath),stderr:rel(stderrPath),summary};
    receipt.results[name]=result;saveReceipt();
    requireCondition(!trap&&summary&&summary.complete===true&&summary.schema===1&&
        summary.suite==='anm-overflow-rollback','Unit did not complete the oracle: '+name);
    return result;
}
function validateCases(result,legacyMode){
    const s=result.summary;
    const pairs=['cold-inactive-create','poisoned-inactive-create-at','active-release-reuse',
        'inactive-create-release-reuse','history-ring-replay','byte-budget-failure','block-budget-failure'];
    const expected=pairs.flatMap(name=>[name+':false',name+':true']);
    expected.push('outside-capture:false','overflow-exhausted-pool-existing-result:false');
    requireCondition(Array.isArray(s.cases)&&s.cases.length===16,'Missing unit cases');
    requireCondition(s.cases.every(item=>item&&typeof item.name==='string'&&
        typeof item.packed==='boolean'&&Number.isSafeInteger(item.failures)&&item.failures>=0),
        'Malformed per-case summary');
    const actual=s.cases.map(item=>item.name+':'+item.packed).sort();
    requireCondition(JSON.stringify(actual)===JSON.stringify(expected.sort()),'Unexpected unit coverage');
    requireCondition(s.legacy===legacyMode&&s.productionEngineLinked===true&&s.productionManagerLinked===true&&
        s.rollbackStateTouchDelegate===true&&s.productionRollbackStateLinked===false&&
        s.applicationMarkSimulatedCovered===false,'Unit scope tag mismatch');
    for(const field of ['checks','failures','payloadBytes','storageStride','fullPoolComparisons',
        'fullPoolMismatches','fullManagerComparisons','fullManagerMismatches','preclearObservations',
        'preclearMisses','budgetCases','budgetRejections','budgetUnexpectedSuccesses','budgetPayloadChanged',
        'positiveControlFailures','engineConstructors','engineDestructors','scriptWriteChecks','assetIoCalls','gpuDrawCalls'])
        requireCondition(Number.isSafeInteger(s[field])&&s[field]>=0,'Invalid summary count: '+field);
    requireCondition(s.capacity===8192&&s.nativeCapacity===4096&&
        s.storageStride>=s.payloadBytes&&s.payloadBytes>0,'Unexpected ANM pool layout');
    requireCondition(s.checks>0&&s.fullPoolComparisons===20&&s.fullManagerComparisons===20&&
        s.fullManagerMismatches===0&&s.preclearObservations===6&&
        s.budgetCases===4&&s.budgetPayloadChanged===4,'Insufficient full-byte/budget coverage');
    requireCondition(s.engineConstructors===16&&s.engineDestructors===16&&s.scriptWriteChecks===32&&
        s.assetIoCalls===0&&s.gpuDrawCalls===0,'Real engine lifecycle or resource-free boundary failed');
    requireCondition(s.positiveControlFailures===0,'A positive control failed');
    requireCondition(s.cases.reduce((sum,item)=>sum+item.failures,0)===s.failures,
        'Per-case failures do not account for the complete oracle');
    if(!legacyMode){
        requireCondition(result.exitCode===0&&s.passed===true&&s.failures===0&&
            s.fullPoolMismatches===0&&s.preclearMisses===0&&s.budgetRejections===4&&
            s.budgetUnexpectedSuccesses===0&&s.cases.every(item=>item.failures===0),
            'Candidate ANM overflow rollback oracle failed');
    }else{
        requireCondition(result.exitCode===1&&s.passed===false&&s.failures>0&&
            s.fullPoolMismatches>=6&&s.preclearMisses===6&&s.budgetRejections===0&&
            s.budgetUnexpectedSuccesses===4,'Old-overflow-body fixture did not reproduce the missing barrier');
        for(const item of s.cases){
            const positive=['active-release-reuse','outside-capture','overflow-exhausted-pool-existing-result'].includes(item.name);
            requireCondition(positive?item.failures===0:item.failures>0,
                'Legacy failure occurred outside its expected oracle boundary: '+item.name);
        }
    }
}

try{
    receipt.compiler=checkedCommand('compiler-version',['--version']).trim();
    const candidateBytes=readFileSync(engineSource);
    const currentBody=Buffer.from(ALLOCATION_PREFIX+NEW_ALLOCATION+ALLOCATION_SUFFIX);
    const oldBody=Buffer.from(ALLOCATION_PREFIX+OLD_ALLOCATION+ALLOCATION_SUFFIX);
    const counterfactual=replaceUniqueBytes(candidateBytes,currentBody,oldBody,'current MP function');
    const roundtrip=replaceUniqueBytes(counterfactual,oldBody,currentBody,'counterfactual MP function');
    requireCondition(roundtrip.equals(candidateBytes),'Counterfactual inverse did not preserve every original source byte');
    mkdirSync(legacyDirectory,{recursive:true});
    writeFileSync(legacySource,counterfactual);
    requireCondition(readFileSync(legacySource).equals(counterfactual),'Counterfactual source write changed bytes');
    receipt.fixture={
        kind:'counterfactual-old-overflow-body',historicalWholeFilePin:false,
        current:{path:rel(engineSource),bytes:candidateBytes.length,sha256:hash(candidateBytes)},
        counterfactual:{path:rel(legacySource),bytes:counterfactual.length,sha256:hash(counterfactual)},
        changedRegion:'Only the seven-line MP allocate_animation body; native function context and all remaining raw source bytes are identical.',
        originalLineEndingsPreserved:true,inverseByteIdentity:true
    };
    saveReceipt();
    for(const unit of units)
        checkedCommand('discover-'+unit.name,[...unit.flags,'-MM','-MF',unit.discovery,'-MT','source',unit.source]);
    const before=inventory(dependencyPaths(units.map(unit=>unit.discovery)));
    requireCondition(before[rel(engineSource)]===hash(candidateBytes)&&
        before[rel(legacySource)]===hash(counterfactual),'Engine bytes changed after counterfactual creation');
    receipt.sourcesBefore=before;saveReceipt();
    for(const unit of units)
        checkedCommand('compile-'+unit.name,[...unit.flags,'-MMD','-MF',unit.depfile,'-MT','source',
            '-c',unit.source,'-o',unit.object]);
    sameInventory(before,inventory(dependencyPaths(units.map(unit=>unit.depfile))),'during compilation');
    const shared=units.filter(unit=>unit.name.startsWith('shared-')).map(unit=>unit.object);
    const object=name=>units.find(unit=>unit.name===name).object;
    const candidateWasm=resolve(out,'candidate.wasm'),legacyWasm=resolve(out,'legacy.wasm');
    checkedCommand('link-candidate',[...linkFlags,...shared,object('candidate-test'),object('candidate-engine'),'-o',candidateWasm]);
    checkedCommand('link-legacy',[...linkFlags,...shared,object('legacy-test'),object('legacy-engine'),'-o',legacyWasm]);
    const candidate=await executeUnit('candidate',candidateWasm);
    const legacyResult=await executeUnit('legacy',legacyWasm);
    validateCases(candidate,false);validateCases(legacyResult,true);
    const sources=inventory(dependencyPaths(units.map(unit=>unit.depfile)));
    sameInventory(before,sources,'before verification completed');
    receipt.sources=sources;receipt.sourceSha256=hash(JSON.stringify(sources));
    receipt.sharedObjects=shared.map(path=>({path:rel(path),sha256:hash(readFileSync(path))}));
    receipt.dependencyScope='Clang -MM/-MMD non-system closure plus runner, Windows-safe Make parser, production integration headers/sources and counterfactual source; SDK system headers/libraries excluded.';
    receipt.sourceStability='Same raw-byte inventory before compilation, after compilation and after execution; no line-ending normalization.';
    receipt.status='PASS';receipt.passed=true;
}catch(error){
    receipt.status='FAIL';receipt.errors.push(String(error));process.exitCode=1;
}finally{
    receipt.finishedAt=new Date().toISOString();saveReceipt();
}
console.log('ANM overflow rollback '+receipt.status+': '+receiptPath);
