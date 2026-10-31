"""Explicit, read-only static decompilation of the user's TH12 1.00b binary.

Never invoked by the browser, build, ordinary tests, or golden acceptance.
Generated C is an analysis artifact, not recovered original source or runtime.
"""
import argparse, hashlib, importlib.metadata, json, logging, pathlib, pickle, subprocess, sys, time

TITLE = pathlib.Path(__file__).resolve().parents[2]
WORKSPACE = TITLE.parent
sys.path.insert(0, str(WORKSPACE / 'tools/reverse-python'))
import angr

EXPECTED = '99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417'
EXE = WORKSPACE / 'th12/th12.exe'
OUT = TITLE / 'local/reverse/th12-1.00b'
CACHE = OUT / 'analysis.pickle'
TARGETS = {
    0x4643d0: 'rng16', 0x464440: 'rng32',
    0x422e80: 'ufo_inventory_add', 0x4270b0: 'ufo_token_collect',
    0x425c20: 'item_manager_tick', 0x4273f0: 'item_spawn',
    0x449f50: 'ufo_manager_reset', 0x449f80: 'ufo_manager_construct',
    0x449fa0: 'ufo_manager_register', 0x44a2d0: 'ufo_manager_tick',
    0x44a4f0: 'ufo_manager_draw', 0x44a5d0: 'ufo_manager_draw_aux',
    0x44a860: 'ufo_tick_callback', 0x44a870: 'ufo_draw_callback',
    0x44a880: 'ufo_draw_aux_callback', 0x44a890: 'ufo_death_callback',
    0x44a8a0: 'ufo_spawn', 0x44ac00: 'ufo_target_angle',
    0x44aca0: 'ufo_fill_reward', 0x44af00: 'ufo_finish_reward',
    0x4271c0: 'ufo_item_absorb_gate', 0x4214b0: 'ufo_inventory_hud_update',
}

def dump(name, data):
    (OUT / name).write_text(json.dumps(data, indent=2) + '\n', encoding='utf-8')

def identity():
    actual = hashlib.sha256(EXE.read_bytes()).hexdigest()
    if actual != EXPECTED:
        raise RuntimeError('TH12 identity differs: ' + actual)
    return {'exeSha256': actual, 'exeBytes': EXE.stat().st_size,
            'decompiler': 'angr', 'version': importlib.metadata.version('angr'),
            'architecture': 'x86 32-bit PE', 'autoLoadLibraries': False,
            'scope': 'static analysis; does not execute or modify the game',
            'originalSourceRecovered': False, 'wholeGameOracle': False}

def index(meta, scope):
    started = time.monotonic()
    project = angr.Project(str(EXE), auto_load_libs=False)
    options = {'normalize': True, 'data_references': True,
               'cross_references': True, 'function_starts': list(TARGETS)}
    if scope == 'ufo':
        options.update(regions=[(0x4214b0,0x421b60),(0x422e80,0x422f20),
                                (0x425850,0x427b00),(0x449f50,0x44b600),
                                (0x4643d0,0x4644c0)], force_smart_scan=False)
    cfg = project.analyses.CFGFast(**options)
    rows = []
    for address, function in sorted(project.kb.functions.items()):
        if not project.loader.main_object.contains_addr(address):
            continue
        rows.append({'address': hex(address), 'name': function.name,
            'blocks': len(function.block_addrs_set), 'size': function.size,
            'targetLabel': TARGETS.get(address), 'isSimProcedure': function.is_simprocedure,
            'callees': [hex(a) for a in sorted(project.kb.functions.callgraph.successors(address))],
            'callers': [hex(a) for a in sorted(project.kb.functions.callgraph.predecessors(address))]})
    with CACHE.open('wb') as stream:
        pickle.dump((meta, project, cfg), stream, protocol=pickle.HIGHEST_PROTOCOL)
    dump('functions-ufo.json' if scope == 'ufo' else 'functions.json', {'identity': meta, 'analysisSeconds': time.monotonic()-started,
                          'boundaryStatus': 'automatically inferred; requires review', 'functions': rows})
    print(json.dumps({'indexedFunctions': len(rows), 'seconds': round(time.monotonic()-started, 2)}), flush=True)

def decompile(meta, address, annotate=False):
    # Only load this pipeline's local cache; pickle is not an exchange format.
    with CACHE.open('rb') as stream:
        cached_meta, project, cfg = pickle.load(stream)
    if cached_meta['exeSha256'] != meta['exeSha256'] or cached_meta['version'] != meta['version']:
        raise RuntimeError('Stale analysis cache')
    function = project.kb.functions.get(address)
    if function is None:
        raise RuntimeError('No function at requested entry ' + hex(address))
    name = TARGETS.get(address, function.name)
    base = f'{address:08x}_{name}'
    hints = []
    if annotate:
        if address != 0x422e80:
            raise RuntimeError('No reviewed annotation for this function')
        from angr.calling_conventions import SimCCUsercall, SimRegArg
        from angr.sim_type import SimStruct, SimTypeFunction, SimTypeInt, SimTypePointer
        from angr.knowledge_plugins.functions import PrototypeSource
        inventory = SimStruct({name: SimTypeInt() for name in
            ('firstColor','secondColor','thirdColor','count','displayState','gateTimer','combination')},
            name='UfoInventoryResearch')
        function.calling_convention = SimCCUsercall(project.arch,
            [SimRegArg('esi',4),SimRegArg('edi',4)],SimRegArg('eax',4))
        function.prototype = SimTypeFunction([SimTypePointer(inventory),SimTypeInt()],
            SimTypeInt(),arg_names=['inventory','color']).with_arch(project.arch)
        function.prototype_source = PrototypeSource.USER
        hints = ['ESI=inventory pointer, EDI=color, EAX=return; validated by native 422e80 probe',
                 'Seven 32-bit fields named from assembly offsets and probe; research names, not original symbols']
        base += '.annotated'
    disassembly = []
    for block in sorted(function.blocks, key=lambda b: b.addr):
        disassembly.append(f'\n// block {block.addr:08x}')
        for instruction in block.capstone.insns:
            disassembly.append(f'{instruction.address:08x}  {instruction.mnemonic:10} {instruction.op_str}')
    (OUT / 'assembly' / (base + '.asm')).write_text('\n'.join(disassembly)+'\n', encoding='utf-8')
    started = time.monotonic()
    result = project.analyses.Decompiler(function, cfg=cfg.model)
    code = result.codegen.text if result.codegen else None
    if annotate and (not code or 'struct UfoInventoryResearch *' not in code or 'int color)' not in code):
        raise RuntimeError('Decompiler did not preserve the reviewed entry annotation')
    record = {'identity': meta, 'address': hex(address), 'label': name,
        'status': 'generated' if code else 'no-codegen', 'seconds': time.monotonic()-started,
        'blocks': len(function.block_addrs_set), 'size': function.size,
        'warning': 'automatic types, calling conventions and names may be incorrect; check assembly and native sampling',
        'manualAnnotations': hints,
        'manualAnnotationsApplied': bool(annotate),
        'callees': [hex(a) for a in sorted(project.kb.functions.callgraph.successors(address))],
        'callers': [hex(a) for a in sorted(project.kb.functions.callgraph.predecessors(address))]}
    if code:
        text = '// AUTOMATIC DECOMPILER OUTPUT; NOT ORIGINAL SOURCE OR VERIFIED RUNTIME\n'
        text += f'// TH12 1.00b entry {address:#x}; angr {meta["version"]}; EXE SHA-256 {meta["exeSha256"]}\n'
        text += '// Register-based arguments and inferred types require manual validation.\n\n' + code
        (OUT / 'pseudocode' / (base + '.c')).write_bytes(text.encode('utf-8'))
        record['pseudocodeSha256'] = hashlib.sha256(text.encode()).hexdigest()
    dump(base+'.json', record)
    print(json.dumps({'address': hex(address), 'status': record['status'], 'seconds': round(record['seconds'], 2)}), flush=True)

def batch(meta, scope):
    records = []
    for address, name in TARGETS.items():
        base = f'{address:08x}_{name}'
        started = time.monotonic()
        command = [sys.executable, str(pathlib.Path(__file__).resolve()),
                   '--scope', scope, '--function', hex(address)]
        try:
            process = subprocess.run(command, capture_output=True, text=True,
                                     encoding='utf-8', errors='replace', timeout=90)
            (OUT / (base+'.log')).write_text(process.stdout+process.stderr, encoding='utf-8')
            status = 'generated' if process.returncode == 0 and (OUT/'pseudocode'/(base+'.c')).exists() else 'failed'
            record = {'address': hex(address), 'label': name, 'status': status,
                      'exitCode': process.returncode, 'seconds': time.monotonic()-started}
        except subprocess.TimeoutExpired:
            record = {'address': hex(address), 'label': name, 'status': 'timeout',
                      'seconds': time.monotonic()-started}
        records.append(record)
        dump('decompilation-index.json', {'identity': meta, 'functions': records,
            'warning': 'generation success is not semantic verification'})
        print(json.dumps(record), flush=True)

if __name__ == '__main__':
    parser = argparse.ArgumentParser()
    parser.add_argument('--index', action='store_true')
    parser.add_argument('--function', type=lambda text: int(text, 0))
    parser.add_argument('--scope', choices=['all','ufo'], default='all')
    parser.add_argument('--batch', action='store_true')
    parser.add_argument('--annotate', action='store_true', help='Apply only explicitly reviewed register/type annotations')
    arguments = parser.parse_args()
    if arguments.scope == 'ufo':
        CACHE = OUT / 'analysis-ufo.pickle'
    if not arguments.index and arguments.function is None and not arguments.batch:
        parser.error('Select --index, --function ADDRESS or --batch explicitly')
    OUT.mkdir(parents=True, exist_ok=True)
    for name in ('assembly', 'pseudocode'):
        (OUT/name).mkdir(exist_ok=True)
    logging.basicConfig(level=logging.ERROR)
    meta = identity()
    meta['indexScope'] = arguments.scope
    if arguments.index:
        dump('identity.json', meta)
        index(meta, arguments.scope)
    if arguments.function is not None:
        decompile(meta, arguments.function, arguments.annotate)
    if arguments.batch:
        batch(meta, arguments.scope)
