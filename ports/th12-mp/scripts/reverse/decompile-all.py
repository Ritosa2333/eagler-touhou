"""Explicit whole-image static decompilation of the user's pinned TH12 binary.

Outputs stay in ignored local/reverse. The EXE is never modified or executed.
Generated pseudocode is research, never a browser runtime or replay golden.
Every inferred function is accounted for; errors and timeouts are not success.
"""
import argparse, collections, hashlib, importlib.util, json, logging, os
import pathlib, pickle, subprocess, sys, threading, time, traceback

TITLE = pathlib.Path(__file__).resolve().parents[2]
ROOT = TITLE / 'local/reverse/th12-1.00b/full-reviewed'
EXE = TITLE.parent / 'th12/th12.exe'
EXPECTED = '99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417'
sys.path.insert(0, str(TITLE.parent / 'tools/reverse-python'))

# Exact native entries established by prior instruction/execution research.
# The list is a CFG seed, not a claim that automatic function boundaries match.
PRIORITY = [
    0x422e80,0x4270b0,0x425c20,0x4273f0,0x449f50,0x449f80,0x449fa0,
    0x44a2d0,0x44a4f0,0x44a5d0,0x44a860,0x44a870,0x44a880,0x44a890,
    0x44a8a0,0x44ac00,0x44aca0,0x44af00,0x4271c0,0x4214b0,0x421a60,
    0x412990,0x413230,0x413190,0x413840,0x4154d0,0x468d90,0x467170,
    0x4359a0,0x435ae0,0x436100,0x4364f0,0x436ba0,0x437660,0x437810,
    0x437980,0x437a80,0x437d00,0x4381e0,0x438370,0x4385b0,0x4390f0,
    0x439630,0x4399d0,0x439a40,0x439b10,0x439ed0,0x406a80,0x406b20,
    0x406bb0,0x406bf0,0x406ce0,0x406de0,0x407010,0x4072d0,0x4073b0,
    0x407580,0x407780,0x407950,0x4079d0,0x407a30,0x408120,0x4082e0,
    0x407ea0,0x407b80,0x408030,0x4089c0,0x408b90,0x408c10,0x408c30,
    0x408cb0,0x408f00,0x46a5f0,0x46a970,0x46a250,0x46a300,0x469750,
    0x469d40,0x409350,0x4099e0,0x40a020,0x40aa60,0x40b5e0,0x40b670,
    0x40b6f0,0x40bef0,0x40c000,0x40c0b0,0x40c8b0,0x40caa0,0x40cd00,
    0x427ec0,0x427fd0,0x428270,0x428450,0x4287c0,0x428b00,0x429150,
    0x429400,0x429520,0x429610,0x429bc0,0x42a1f0,0x42a800,0x42ab80,
    0x42ada0,0x42b1d0,0x42b750,0x42bd90,0x42c1a0,0x42c260,0x42c3f0,
    0x42c770,0x42cca0,0x42d030,0x42d6e0,0x42d800,0x42d890,0x42d9c0,
    0x42df00,0x42e3e0,0x42e770,0x42e820,0x42e880,0x402520,0x454d10,
    0x454df0,0x454ee0,0x4551b0,0x4553f0,0x455580,0x455630,0x454b80,
    0x460fe0,0x4610d0,0x460c30,0x460c40,0x461250,0x4612d0,0x461350,
    0x4613d0,0x4615a0,0x461720,0x461840,0x461920,0x461970,0x462060,
    0x45fe60,0x460040,0x4606b0,0x460760,0x460800,0x44e660,0x402970,0x402a40,0x402f40,
    0x403ec0,0x403020,0x4049a0,0x404620,0x403d10,0x4046c0,0x41f960,
    0x41fcc0,0x420e40,0x41e810,0x41f030,0x453d90,0x453e20,0x430150,
    0x401090,0x41d250,0x42e9b0,0x42f4b0,0x42feb0,0x43bc10,0x43c350,
    0x43cf90,0x43d33d,0x43f3d0,0x43f760,0x446560,0x4466a0,0x449360,
    0x4537b0,0x454960,0x4643d0,0x464440,0x421690,0x41d560,0x449f20,
    0x44a120,0x44a230,0x44a290,0x44a2b0,0x414af0,
]

def atomic(path, value):
    tmp = path.with_suffix(path.suffix + '.tmp')
    tmp.write_text(json.dumps(value, indent=2) + '\n', encoding='utf-8')
    os.replace(tmp, path)

def identity():
    import importlib.metadata
    digest = hashlib.sha256(EXE.read_bytes()).hexdigest()
    if digest != EXPECTED:
        raise RuntimeError('Pinned original identity mismatch: ' + digest)
    return {'exeSha256':digest,'exeBytes':EXE.stat().st_size,'decompiler':'angr',
            'version':importlib.metadata.version('angr'),'architecture':'x86 PE32',
            'originalSourceRecovered':False,'wholeGameOracle':False,
            'functionBoundaries':'automatically inferred, not all reviewed'}

def initialize(meta):
    import angr
    cache = ROOT / 'analysis.pickle'
    if cache.exists() and (ROOT/'functions.json').exists():
        old = json.loads((ROOT/'functions.json').read_text())
        if old['identity'] == meta and old.get('prioritySeeds') == sorted(set(PRIORITY)):
            return old
        raise RuntimeError('Stale full-analysis cache; preserve it before starting a new analysis')
    start = time.monotonic()
    project = angr.Project(str(EXE), auto_load_libs=False)
    cfg = project.analyses.CFGFast(normalize=True,data_references=True,
                                  cross_references=True,function_starts=sorted(set(PRIORITY)))
    sections = [{'name':s.name,'start':s.vaddr,'end':s.vaddr+s.memsize,
                 'executable':bool(s.is_executable)} for s in project.loader.main_object.sections]
    rows=[];covered=[]
    for address,function in sorted(project.kb.functions.items()):
        if not project.loader.main_object.contains_addr(address):continue
        blocks=[]
        for block in sorted(function.blocks,key=lambda b:b.addr):
            blocks.append([block.addr,block.size]);covered.append((block.addr,block.addr+block.size))
        rows.append({'address':address,'name':function.name,'size':function.size,'blocks':blocks,
                     'prioritySeed':address in PRIORITY,'isSimProcedure':function.is_simprocedure,
                     'callees':sorted(project.kb.functions.callgraph.successors(address)),
                     'callers':sorted(project.kb.functions.callgraph.predecessors(address))})
    with cache.open('wb') as stream:pickle.dump((meta,project,cfg),stream,pickle.HIGHEST_PROTOCOL)
    result={'identity':meta,'indexSeconds':time.monotonic()-start,'prioritySeeds':sorted(set(PRIORITY)),
            'sections':sections,'functions':rows,'unresolvedPrioritySeeds':[
                hex(a) for a in PRIORITY if not any(r['address']==a for r in rows)]}
    atomic(ROOT/'functions.json',result)
    atomic(ROOT/'binary-sections.json',{'identity':meta,'entryPoint':project.entry,
        'sections':sections,'imports':sorted(project.loader.main_object.imports),
        'prioritySeeds':sorted(set(PRIORITY))})
    print(json.dumps({'phase':'indexed','functions':len(rows),'seconds':round(result['indexSeconds'],1),
                      'unresolvedPrioritySeeds':result['unresolvedPrioritySeeds']}),flush=True)
    return result

def read_record(address):
    p=ROOT/'records'/f'{address:08x}.json'
    try:return json.loads(p.read_text())
    except (FileNotFoundError,json.JSONDecodeError):return None

def worker(meta, shard, timeout):
    import angr
    # A fresh Project loads Win32 definitions, but unpickling it in a worker
    # does not. FS:[30h] rewriting otherwise fails with KeyError('ntdll.dll').
    from angr.procedures.definitions import load_win32api_definitions
    load_win32api_definitions()
    with (ROOT/'analysis.pickle').open('rb') as stream:cached,project,cfg=pickle.load(stream)
    if cached!=meta:raise RuntimeError('Worker cache identity mismatch')
    tasks=json.loads(pathlib.Path(shard).read_text())
    for address in tasks:
        old=read_record(address)
        if old and old.get('status') not in ('running','interrupted'):continue
        function=project.kb.functions.get(address)
        started=time.monotonic();record={'identity':meta,'address':hex(address),
            'status':'running','semanticVerification':False,'originalSourceRecovered':False}
        if function is None:
            record.update(status='missing-function');atomic(ROOT/'records'/f'{address:08x}.json',record);continue
        record.update(name=function.name,blocks=len(function.block_addrs_set),size=function.size,
                      callees=[hex(x) for x in sorted(project.kb.functions.callgraph.successors(address))],
                      callers=[hex(x) for x in sorted(project.kb.functions.callgraph.predecessors(address))])
        destination=ROOT/'records'/f'{address:08x}.json';atomic(destination,record)
        def expire():
            record.update(status='timeout',seconds=time.monotonic()-started,limitSeconds=timeout)
            atomic(destination,record);os._exit(124)
        timer=threading.Timer(timeout,expire);timer.daemon=True;timer.start()
        try:
            assembly=[]
            for block in sorted(function.blocks,key=lambda b:b.addr):
                assembly.append(f'\n// block {block.addr:08x}')
                for instruction in block.capstone.insns:
                    assembly.append(f'{instruction.address:08x}  {instruction.mnemonic:10} {instruction.op_str}')
            asm='\n'.join(assembly)+'\n';(ROOT/'assembly'/f'{address:08x}.asm').write_bytes(asm.encode('utf-8'))
            result=project.analyses.Decompiler(function,cfg=cfg.model)
            code=result.codegen.text if result.codegen else None
            record['status']='generated' if code else 'no-codegen'
            record['decompilerErrors']=[str(error) for error in result.errors]
            if code:
                text='// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR\n'
                text+=f'// TH12 1.00b {address:#x}; EXE {meta["exeSha256"]}; angr {meta["version"]}\n'
                text+='// Inferred types/register conventions/x87 expressions require assembly review.\n\n'+code
                (ROOT/'pseudocode'/f'{address:08x}.c').write_bytes(text.encode('utf-8'))
                record.update(pseudocodeSha256=hashlib.sha256(text.encode()).hexdigest(),
                              pseudocodeBytes=len(text.encode()))
            record['assemblySha256']=hashlib.sha256(asm.encode()).hexdigest()
        except Exception as error:
            record.update(status='failed',error=type(error).__name__+': '+str(error),traceback=traceback.format_exc())
        finally:timer.cancel()
        record['seconds']=time.monotonic()-started;atomic(destination,record)
    print(json.dumps({'shard':pathlib.Path(shard).name,'status':'finished'}),flush=True)

def progress(meta, rows, started):
    counts=collections.Counter();records=[]
    for row in rows:
        record=read_record(row['address'])
        if record is None:counts['pending']+=1;continue
        counts[record['status']]+=1;records.append({k:record[k] for k in (
            'address','name','status','seconds','error','pseudocodeSha256','assemblySha256') if k in record})
    terminal=len(rows)-counts['pending']-counts['running']-counts['interrupted']
    result={'schema':'th12-whole-image-static-decompilation/1','identity':meta,
        'candidateFunctions':len(rows),'terminalFunctions':terminal,'counts':dict(counts),
        'elapsedSeconds':time.monotonic()-started,'allCandidatesAttempted':terminal==len(rows),
        'allCandidatesGenerated':counts['generated']==len(rows),
        'allFunctionBoundariesReviewed':False,'wholeGameOracle':False,'semanticVerificationComplete':False,
        'warning':'Generation success does not prove correct types, calling conventions, function boundaries or behavior.',
        'functions':records}
    atomic(ROOT/'decompilation-index.json',result)
    return {k:result[k] for k in ('candidateFunctions','terminalFunctions','counts','allCandidatesAttempted','elapsedSeconds')}

def run(meta, jobs, timeout):
    inventory=initialize(meta);rows=inventory['functions'];started=time.monotonic()
    priority={a:i for i,a in enumerate(PRIORITY)}
    addresses=sorted((r['address'] for r in rows),key=lambda a:(a not in priority,priority.get(a,a)))
    tasks=[];children={};restarts=collections.Counter()
    for i in range(jobs):
        path=ROOT/f'jobs-{i}.json';atomic(path,addresses[i::jobs]);tasks.append(path)
    def spawn(i):
        log=(ROOT/f'worker-{i}.log').open('a',encoding='utf-8')
        process=subprocess.Popen([sys.executable,str(pathlib.Path(__file__).resolve()),
            '--worker',str(tasks[i]),'--timeout',str(timeout)],stdout=log,stderr=log,cwd=TITLE)
        log.close();children[i]=process
    for i in range(jobs):spawn(i)
    last=0
    try:
        while children:
            for i,process in list(children.items()):
                status=process.poll()
                if status is None:continue
                pending=[a for a in addresses[i::jobs] if not read_record(a) or read_record(a).get('status') in ('running','interrupted')]
                if pending and restarts[i]<len(addresses):
                    # A worker failure cannot silently leave an entry pending.
                    for a in pending:
                        record=read_record(a)
                        if record and record.get('status')=='running':
                            record.update(status='failed-worker',workerExitCode=status);atomic(ROOT/'records'/f'{a:08x}.json',record)
                    restarts[i]+=1;spawn(i)
                else:children.pop(i)
            if time.monotonic()-last>=15:
                print(json.dumps(progress(meta,rows,started)),flush=True);last=time.monotonic()
            time.sleep(1)
    except KeyboardInterrupt:
        for process in children.values():process.terminate()
        for row in rows:
            record=read_record(row['address'])
            if record and record.get('status')=='running':record['status']='interrupted';atomic(ROOT/'records'/f'{row["address"]:08x}.json',record)
        raise
    finally:
        print(json.dumps(progress(meta,rows,started)),flush=True)

if __name__=='__main__':
    p=argparse.ArgumentParser();p.add_argument('--run',action='store_true');p.add_argument('--worker')
    p.add_argument('--jobs',type=int,default=3);p.add_argument('--timeout',type=int,default=90)
    args=p.parse_args()
    if not args.run and not args.worker:p.error('Use --run explicitly; this tool is not called by builds/tests')
    if not 1<=args.jobs<=8 or not 5<=args.timeout<=1800:p.error('Invalid worker count or function timeout')
    ROOT.mkdir(parents=True,exist_ok=True)
    for name in ('records','assembly','pseudocode'):(ROOT/name).mkdir(exist_ok=True)
    logging.basicConfig(level=logging.ERROR);meta=identity()
    if args.worker:worker(meta,args.worker,args.timeout)
    else:run(meta,args.jobs,args.timeout)
