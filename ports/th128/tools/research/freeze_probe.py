import argparse
import hashlib
import importlib.metadata
import json
import math
import struct
import sys
from pathlib import Path


ROOT = Path(__file__).resolve().parents[2]
EXE_SHA256 = 'ba515750ece845b56640fe440592fc39ebb7bdd1ed87c541b67a19b148e767d4'
POWER = 0x4B4D6C
PERFECT = 0x4B4D70
AREA = 0x4B4D60
MANAGER_POINTER = 0x4B8938
MANAGER = 0x300000
PLAYER = 0x380000
BUSY = 0x390000
POSITION = 0x3A0000
STACK_BASE = 0x100000
STACK_SIZE = 0x10000
STACK_POINTER = 0x108000
RETURN_TRAP = 0x200000


def address(value):
    return f'0x{value:08x}'


def datum(name, location, value, kind='i', writable=True):
    payload = value if isinstance(value, bytes) else struct.pack('<' + kind, value)
    return {'name': name, 'address': location, 'bytes': payload, 'writable': writable}


def report_destination(path):
    destination = Path(path).resolve()
    allowed = (ROOT / 'local' / 'research').resolve()
    if (allowed != ROOT.resolve() / 'local' / 'research'
            or destination.parent != allowed or destination.suffix != '.json'
            or not destination.name.startswith('freeze-')):
        raise ValueError('report must be local/research/freeze-*.json inside this deliverable')
    if destination.drive.lower() == 'e:':
        raise ValueError('original E: files are read-only')
    return destination


def emulate(chunks, entry, data=(), registers=None, arguments=b'', stops=(), budget=1024, stack=None,cpu_state=None):
    import capstone
    import unicorn
    from capstone.x86_const import X86_OP_IMM
    from unicorn import x86_const

    if not 1 <= budget <= 200000 or len(arguments) > 1024:
        raise ValueError('invalid bounded probe budget or argument length')
    if cpu_state is not None and (type(cpu_state) is not dict or set(cpu_state)!={'mxcsr','fpcw'}
                                 or cpu_state['mxcsr']!=0x1f80 or cpu_state['fpcw']!=0x37f):
        raise ValueError('only explicit masked round-to-nearest SSE/x87 environment is supported')
    decoder = capstone.Cs(capstone.CS_ARCH_X86, capstone.CS_MODE_32)
    decoder.detail = True
    instructions = {}
    spans = []
    for start, code in chunks:
        if not code or len(code) > 65536:
            raise ValueError('invalid code chunk')
        if any(start < end and begin < start + len(code) for begin, end in spans):
            raise ValueError('overlapping code chunks')
        spans.append((start, start + len(code)))
        for instruction in decoder.disasm(code, start):
            instructions[instruction.address] = instruction
    if entry not in instructions:
        raise ValueError('entry is not an explicit instruction boundary')
    names = [region['name'] for region in data]
    if len(set(names)) != len(names):
        raise ValueError('data names must be unique')
    for index, region in enumerate(data):
        start, size = region['address'], len(region['bytes'])
        if not 0 < size <= 65536:
            raise ValueError('invalid explicit data span')
        for previous in data[:index]:
            if start < previous['address'] + len(previous['bytes']) and previous['address'] < start + size:
                raise ValueError('overlapping data spans')
    emulator = unicorn.Uc(unicorn.UC_ARCH_X86, unicorn.UC_MODE_32)
    pages = {}

    def add_pages(start, size, category):
        for page in range(start & ~4095, (start + size + 4095) & ~4095, 4096):
            if page in pages and pages[page] != category:
                raise ValueError('code/data/stack page overlap')
            pages[page] = category

    for start, code in chunks:
        add_pages(start, len(code), 'code')
    for region in data:
        add_pages(region['address'], len(region['bytes']), 'data')
    add_pages(STACK_BASE, STACK_SIZE, 'stack')
    add_pages(RETURN_TRAP, 1, 'trap')
    for page in pages:
        emulator.mem_map(page, 4096, unicorn.UC_PROT_ALL)
    for start, code in chunks:
        emulator.mem_write(start, code)
    for region in data:
        emulator.mem_write(region['address'], region['bytes'])
    emulator.mem_write(STACK_POINTER, struct.pack('<I', RETURN_TRAP) + arguments)
    for offset, payload in (stack or {}).items():
        location = STACK_POINTER + offset
        if not STACK_BASE <= location <= STACK_BASE + STACK_SIZE - len(payload):
            raise ValueError('stack initialization outside bounded stack')
        emulator.mem_write(location, payload)
    for page, category in pages.items():
        protection = unicorn.UC_PROT_READ | (unicorn.UC_PROT_EXEC if category in ('code', 'trap')
                                           else unicorn.UC_PROT_WRITE)
        emulator.mem_protect(page, 4096, protection)
    register_ids = {name: getattr(x86_const, 'UC_X86_REG_' + name.upper())
                    for name in ('eax', 'ebx', 'ecx', 'edx', 'esi', 'edi', 'ebp', 'esp', 'eip', 'eflags')}
    initial = dict.fromkeys(('eax', 'ebx', 'ecx', 'edx', 'esi', 'edi', 'ebp'), 0)
    if any(name not in initial for name in (registers or {})):
        raise ValueError('unsupported input register')
    initial.update(registers or {})
    initial.update(esp=STACK_POINTER, eflags=2)
    for name, value in initial.items():
        emulator.reg_write(register_ids[name], value & 0xFFFFFFFF)
    emulator.reg_write(x86_const.UC_X86_REG_FPCW, 0x037F)
    if cpu_state is not None:
        emulator.reg_write(x86_const.UC_X86_REG_MXCSR,cpu_state['mxcsr'])
        emulator.reg_write(x86_const.UC_X86_REG_FPCW,cpu_state['fpcw'])
    stop = {}
    trace = []

    def finish(reason, location, **details):
        if not stop:
            stop.update(reason=reason, address=address(location), **details)
        emulator.emu_stop()

    def code_hook(machine, location, size, context):
        if location in stops:
            finish('boundary', location)
            return
        instruction = instructions.get(location)
        if instruction is None or instruction.size != size:
            finish('outside_code', location)
            return
        if instruction.group(capstone.CS_GRP_CALL):
            target = instruction.operands[0]
            if target.type != X86_OP_IMM or target.imm not in instructions:
                finish('unsupported_call', location,
                       target=address(target.imm) if target.type == X86_OP_IMM else instruction.op_str)
                return
        trace.append(address(location))

    def memory_hook(machine, access, location, size, value, context):
        if STACK_BASE <= location and location + size <= STACK_BASE + STACK_SIZE:
            return
        writing = access == unicorn.UC_MEM_WRITE
        for region in data:
            if region['address'] <= location and location + size <= region['address'] + len(region['bytes']):
                if writing and not region['writable']:
                    finish('readonly_data', machine.reg_read(register_ids['eip']), memory=address(location))
                return
        if not writing and any(start <= location and location + size <= end for start, end in spans):
            return
        finish('outside_data', machine.reg_read(register_ids['eip']), memory=address(location), size=size)

    emulator.hook_add(unicorn.UC_HOOK_CODE, code_hook)
    emulator.hook_add(unicorn.UC_HOOK_MEM_READ | unicorn.UC_HOOK_MEM_WRITE, memory_hook)
    try:
        emulator.emu_start(entry, RETURN_TRAP, count=budget)
    except unicorn.UcError as error:
        if not stop:
            stop.update(reason='fault', address=address(emulator.reg_read(register_ids['eip'])), detail=str(error))
    result_registers = {name: emulator.reg_read(identifier) for name, identifier in register_ids.items()}
    if not stop:
        stop.update(reason='returned' if result_registers['eip'] == RETURN_TRAP else 'budget_exhausted',
                    address=address(result_registers['eip']))
    memory = {}
    for region in data:
        before = region['bytes']
        after = bytes(emulator.mem_read(region['address'], len(before)))
        record = {'address': address(region['address']), 'bytes': len(before),
                  'writable': region['writable'], 'before': before.hex(), 'after': after.hex()}
        if len(before) == 4:
            for label, payload in (('before', before), ('after', after)):
                record[label + 'I32'] = struct.unpack('<i', payload)[0]
                floating = struct.unpack('<f', payload)[0]
                if math.isfinite(floating):
                    record[label + 'F32'] = floating
        memory[region['name']] = record
    final_stack = result_registers['esp']
    stack_head = (bytes(emulator.mem_read(final_stack, 64)).hex()
                  if STACK_BASE <= final_stack <= STACK_BASE + STACK_SIZE - 64 else None)
    return {'entry': address(entry), 'inputRegisters': initial,
            **({'inputCpuState':dict(cpu_state)} if cpu_state is not None else {}),
            'arguments': arguments.hex(), 'stackInitialization': {str(key): value.hex() for key, value in (stack or {}).items()},
            'stopBoundaries': [address(value) for value in stops], 'instructionBudget': budget,
            'instructionsExecuted': len(trace), 'instructionTrace': trace, 'stop': stop,
            'registers': result_registers, 'stackDelta': final_stack - STACK_POINTER,
            'stackHead': stack_head, 'memory': memory,
            'mappedPages': {address(page): kind for page, kind in sorted(pages.items())}}


def probe(executable):
    binary = executable.read_bytes()
    if hashlib.sha256(binary).hexdigest() != EXE_SHA256:
        raise ValueError('EXE SHA-256 does not match the pinned Japanese executable')
    return recover(executable, binary)


CODE = {
    'power_get': (0x410C40, 0x410C46, 'leaf; global i32 -> EAX; ret'),
    'power_add': (0x41F410, 0x41F470, 'entry; stack i32 delta; ret 4 on returning paths'),
    'perfect_add': (0x427300, 0x427396, 'entry; stack i32 delta; ret 4 on returning paths'),
    'motivation_add': (0x427260, 0x4272F8, 'entry; stack i32 delta; ret 4 on returning paths'),
    'area_add': (0x410CB0, 0x410CC6, 'leaf; stack float32 delta; ret 4'),
    'held_rate': (0x43F35D, 0x43F39F, 'slice; ESI=player, EDI=45; selects power-add argument'),
    'baseline_replenish': (0x43F462, 0x43F46B, 'slice; push 5 and call the original power adder'),
    'release_gate': (0x43F40C, 0x43F442, 'slice; EAX=timer, EBX=0, ECX=busy object, EDI=player'),
    'release_to_activation': (0x43F40C, 0x43F44E, 'slice; release gate, real timer setter, real activation entry'),
    'timer_set': (0x403280, 0x4032BD, 'entry; EAX=timer object, stack i32 value; ret 4'),
    'activation': (0x4101D0, 0x4102E1, 'entry; EBX=pointer to 3 float32 coordinates; ret'),
    'original_memset': (0x47BB30, 0x47BBAA, 'original cdecl memset; stack destination, byte, size; ret'),
    'activation_arguments': (0x4102A0, 0x4102CE, 'slice; EDI=position; stops before 0x410590'),
    'activation_zero': (0x4102D2, 0x4102DC, 'slice; single power-reset store; boundary stop'),
    'duration_gate': (0x4103E2, 0x410409, 'slice; EDI=slot; manager saved power and slot timer'),
    'replenish_300': (0x417404, 0x41745D, 'slice; +300, upper clamp, crossing callouts'),
    'replenish_100': (0x43E0A7, 0x43E0FE, 'slice; +100, upper clamp, crossing callouts'),
    'impact_replenish': (0x43FCB8, 0x43FD24, 'slice; +100 if manager flag zero, otherwise +10'),
    'area_percentage': (0x40F5D3, 0x40F5FF, 'slice; EBX=manager, stack+0x24=i32 bit count'),
    'area_perfect_gain': (0x40F7EA, 0x40F85B, 'slice; EBX=manager; stops before perfect-add call'),
    'original_ftol': (0x48FAC0, 0x48FB6B, 'original conversion helper; x87 ST0 -> EAX; fallback also sets EDX; ret'),
}

STATIC = {
    'ice_resource_constructor': (0x40E820, 0x40E8C0),
    'button_and_timer_dispatch': (0x43F2E3, 0x43F335),
    'held_timer_context': (0x43F335, 0x43F35D),
    'release_to_activation_call': (0x43F40C, 0x43F48B),
    'perfect_use_gate_and_cost': (0x43B7A8, 0x43B7F3),
    'perfect_death_state_gate': (0x43B8C1, 0x43B925),
    'power_initialization': (0x425EF6, 0x425F0C),
    'miss_recovery': (0x43CDD0, 0x43CE20),
    'ice_mask_popcount': (0x410960, 0x410A5C),
    'area_percentage_context': (0x40F5B4, 0x40F606),
    'impact_gate_context': (0x43FCA8, 0x43FCC4),
    'power_percent_display': (0x43F569, 0x43F592),
}


def recover(executable, binary):
    import capstone
    import pefile

    image = pefile.PE(data=binary)
    if image.FILE_HEADER.Machine != 0x14C or image.OPTIONAL_HEADER.ImageBase != 0x400000:
        raise ValueError('expected pinned x86 PE at 0x400000')
    decoder = capstone.Cs(capstone.CS_ARCH_X86, capstone.CS_MODE_32)
    chunks = {}
    catalog = {}

    def original(start, end):
        payload = image.get_data(start - 0x400000, end - start)
        if len(payload) != end - start:
            raise ValueError('original byte range truncated')
        return payload

    for name, (start, end, contract) in CODE.items():
        payload = original(start, end)
        chunks[name] = (start, payload)
        catalog[name] = {'start': address(start), 'endExclusive': address(end),
                         'fileOffset': image.get_offset_from_rva(start - 0x400000),
                         'size': len(payload), 'sha256': hashlib.sha256(payload).hexdigest(),
                         'bytes': payload.hex(), 'contract': contract,
                         'disassembly': [{'address': address(instruction.address), 'bytes': instruction.bytes.hex(),
                                          'mnemonic': instruction.mnemonic, 'operands': instruction.op_str}
                                         for instruction in decoder.disasm(payload, start)]}
    contexts = {}
    for name, (start, end) in STATIC.items():
        payload = original(start, end)
        contexts[name] = {'start': address(start), 'endExclusive': address(end),
                          'sha256': hashlib.sha256(payload).hexdigest(), 'bytes': payload.hex(),
                          'executedByThisProbe': False,
                          'disassembly': [{'address': address(instruction.address), 'bytes': instruction.bytes.hex(),
                                           'mnemonic': instruction.mnemonic, 'operands': instruction.op_str}
                                          for instruction in decoder.disasm(payload, start)]}
    records = []

    def run(name, code_names, data=(), registers=None, arguments=b'', stops=(), stack=None):
        result = emulate([chunks[key] for key in code_names], chunks[code_names[0]][0],
                         data=data, registers=registers, arguments=arguments, stops=stops, stack=stack)
        if result['stop']['reason'] not in ('returned', 'boundary', 'unsupported_call'):
            raise RuntimeError(f'{name}: unexpected sandbox stop: {result["stop"]}')
        records.append({'id': name, 'code': code_names, 'result': result})

    def manager_fields(saved=None):
        result = [datum('managerPointer', MANAGER_POINTER, MANAGER)]
        if saved is not None:
            result.append(datum('savedPower', MANAGER + 0x6B200, saved))
        return result

    def constant(location, size):
        return datum('constant-' + address(location), location, original(location, location + size), writable=False)

    for power in (-1, 0, 2999, 3000, 10000):
        run(f'power-get-{power}', ['power_get'], [datum('power', POWER, power)])
    for power in (0, 1, 2999, 3000, 9999, 10000, 12000):
        for delta in (-10000, -1, 0, 1, 5, 50, 10000):
            run(f'power-add-{power}-{delta}', ['power_add'], [datum('power', POWER, power)],
                arguments=struct.pack('<i', delta))
    for perfect in (0, 9999, 10000, 19999, 20000, 29999, 30000):
        for delta in (-10000, -1, 0, 1, 10000, 40000):
            run(f'perfect-add-{perfect}-{delta}', ['perfect_add'], [datum('perfect', PERFECT, perfect)],
                arguments=struct.pack('<i', delta))
    for motivation, delta, latch in ((0, -1, 0), (0, 10000, 1), (100000, 1, 0),
                                     (20000, -10000, 0), (9999, 1, 0), (99999, 2, 0)):
        run(f'motivation-{motivation}-{delta}-{latch}', ['motivation_add'],
            [datum('motivation', 0x4B4D64, motivation), datum('negativeLatch', 0x4B4D68, latch)],
            arguments=struct.pack('<i', delta))
    for area_value, delta in ((0.0, 1.0), (100.0, 0.1), (125.0, -200.0), (3000.0, 100.0)):
        run(f'area-add-{area_value}-{delta}', ['area_add'], [datum('area', AREA, area_value, 'f')],
            arguments=struct.pack('<f', delta))
    for timer in (0, 19, 20, 44, 45, 46, 80, 81, 110, 111, 140, 141, 1000):
        run(f'held-{timer}', ['held_rate'], [datum('holdTimer', PLAYER + 0x43914, timer)],
            registers={'esi': PLAYER, 'edi': 45}, stops=[0x43F39F])
    for power in (0, 2999, 10000):
        run(f'baseline-{power}', ['baseline_replenish', 'power_add'],
            [datum('power', POWER, power)], stops=[0x43F469])
    for timer, power, busy_value in ((0, 10000, 0), (44, 3000, 0), (45, 2999, 0),
                                    (45, 3000, 0), (45, 10000, 1), (100, 10000, 0)):
        run(f'release-{timer}-{power}-{busy_value}', ['release_gate'],
            [datum('power', POWER, power), datum('busy', BUSY + 0x3C, busy_value)],
            registers={'eax': timer, 'ebx': 0, 'ecx': BUSY, 'edi': PLAYER}, stops=[0x43F442, 0x43F462])
    for power in (0, 2999, 3000, 10000):
        for first, second in ((0, 0), (1, 0), (1, 1), (2, 0)):
            slots = bytearray(0xA8)
            struct.pack_into('<i', slots, 0, first)
            struct.pack_into('<i', slots, 0x54, second)
            run(f'activation-{power}-{first}-{second}', ['activation', 'original_memset'],
                manager_fields(-123) + [datum('power', POWER, power), datum('slots', MANAGER + 0x10, bytes(slots)),
                                        datum('anmHandle', MANAGER + 0x65DDC, 0x3B0000),
                                        datum('position', POSITION, struct.pack('<3f', 12.5, 200.0, 0.0))],
                registers={'ebx': POSITION})
        run(f'activation-arguments-{power}', ['activation_arguments'],
            manager_fields(power) + [constant(0x4A6B18, 4)], registers={'edi': MANAGER + 0x14})
        run(f'activation-zero-{power}', ['activation_zero'], [datum('power', POWER, power)], stops=[0x4102DC])
    for timer, power, first, second in ((45, 3000, 0, 0), (45, 3000, 1, 1), (44, 3000, 0, 0), (45, 2999, 0, 0)):
        slots = bytearray(0xA8)
        struct.pack_into('<i', slots, 0, first)
        struct.pack_into('<i', slots, 0x54, second)
        timer_state = struct.pack('<iifII', timer - 1, timer, float(timer), 0x4B6F58, 1)
        run(f'release-call-{timer}-{power}-{first}-{second}',
            ['release_to_activation', 'timer_set', 'activation', 'original_memset'],
            manager_fields(-123) + [datum('power', POWER, power), datum('busy', BUSY + 0x3C, 0),
                                    datum('slots', MANAGER + 0x10, bytes(slots)),
                                    datum('anmHandle', MANAGER + 0x65DDC, 0x3B0000),
                                    datum('position', PLAYER + 0xECC, struct.pack('<3f', 12.5, 200.0, 0.0)),
                                    datum('timerState', PLAYER + 0x43910, timer_state)],
            registers={'eax': timer, 'ebx': 0, 'ecx': BUSY, 'edi': PLAYER}, stops=[0x43F44E, 0x43F462])
    for power, timer in ((3000, 11), (3000, 12), (10000, 99), (10000, 100),
                         (3039, 12), (3040, 12), (2999, 12), (2000, 0), (0, 0), (9999, 99), (9999, 100)):
        run(f'duration-{power}-{timer}', ['duration_gate'],
            manager_fields(power) + [datum('slotTimer', MANAGER + 0x50, timer)],
            registers={'edi': MANAGER + 0x10}, stops=[0x410409, 0x410454])
    for code_name in ('replenish_300', 'replenish_100'):
        for power in (0, 2900, 2999, 3000, 9900, 9999, 10000):
            run(f'{code_name}-{power}', [code_name], [datum('power', POWER, power)], stops=[CODE[code_name][1]])
    for active in (0, 1, -1):
        for power in (0, 2999, 9999, 10000):
            run(f'impact-{power}-{active}', ['impact_replenish'],
                manager_fields() + [datum('power', POWER, power), datum('active', MANAGER + 0x6B218, active)],
                stops=[0x43FD24])
    for count in (0, 1, 17203, 51610, 86016, 172031, 172032):
        run(f'area-{count}', ['area_percentage'],
            [datum('area', AREA, 125.0, 'f'), datum('lastArea', MANAGER + 0x6B214, -5.0, 'f'),
             constant(0x4A6B80, 8), constant(0x4A6590, 8)], registers={'ebx': MANAGER},
            stops=[0x40F5FF], stack={0x24: struct.pack('<i', count)})
    area_constants = [constant(0x4A6A60, 4)] + [constant(location, 8) for location in
                                               (0x4A6988, 0x4A6990, 0x4A6738, 0x4A6B78)]
    for feature in (0, 1):
        for area_value in (0, 0.01, 1, 9.999999046325684, 10, 10.000000953674316,
                           20, 29.999998092651367, 30, 30.000001907348633, 31, 100):
            run(f'area-gain-{area_value}-cpu{feature}', ['area_perfect_gain', 'original_ftol'],
                [datum('effectManager', 0x4D2E50, 0), datum('savedPower', MANAGER + 0x6B200, 10000),
                 datum('lastArea', MANAGER + 0x6B214, area_value, 'f'), datum('cpuFeature', 0x4DCD68, feature)]
                + area_constants, registers={'ebx': MANAGER}, stops=[0x40F861])
    run('area-gain-bypass-10001', ['area_perfect_gain', 'original_ftol'],
        [datum('effectManager', 0x4D2E50, 0), datum('savedPower', MANAGER + 0x6B200, 10001)],
        registers={'ebx': MANAGER}, stops=[0x40F861])
    image.close()
    after = executable.read_bytes()
    if after != binary:
        raise RuntimeError('original EXE changed during read-only probe')
    callouts = {}
    for record in records:
        stop = record['result']['stop']
        if stop['reason'] == 'unsupported_call':
            key = stop['target']
            callouts.setdefault(key, {'instructions': set(), 'caseCount': 0})
            callouts[key]['instructions'].add(stop['address'])
            callouts[key]['caseCount'] += 1
    references = []
    reference_paths = [ROOT.parents[1] / 'work' / 'upstream' / 'thprac_th128.cpp',
                       ROOT / 'docs' / 'native-static-index.json', ROOT / 'docs' / 'native-circle-fixtures.json']
    catalog_path = ROOT / 'local' / 'catalog.json'
    if catalog_path.is_file():
        assets = json.loads(catalog_path.read_text(encoding='utf-8'))
        manual_parent = (ROOT / 'local' / assets['variants']['zh']['manifest']).parent / 'raw'
        reference_paths.extend(manual_parent / f'help_{page:02}.png' for page in range(2, 8))
    for path in reference_paths:
        if path.is_file():
            payload = path.read_bytes()
            references.append({'path': str(path.resolve()), 'bytes': len(payload),
                               'sha256': hashlib.sha256(payload).hexdigest(), 'executed': False})
    return {
        'schema': 'th128-original-byte-freeze-fixtures/1',
        'executable': {'path': str(executable.resolve()), 'bytes': len(binary), 'sha256': EXE_SHA256,
                       'sha256After': hashlib.sha256(after).hexdigest(), 'unchanged': True},
        'method': 'Only allowlisted original x86 chunks in Unicorn; synthetic explicit state; stop before unprovided calls.',
        'windowsGameProcessLaunched': False, 'windowsApisAvailable': False,
        'nativeCpuTrace': False, 'sourceDecompilation': False,
        'probeScriptSha256': hashlib.sha256(Path(__file__).read_bytes()).hexdigest(),
        'python': sys.version,
        'tools': {name: importlib.metadata.version(name) for name in ('pefile', 'capstone', 'unicorn')},
        'x87ControlWord': '0x037f', 'stack': {'base': address(STACK_BASE), 'size': STACK_SIZE,
                                           'entryEsp': address(STACK_POINTER), 'returnTrap': address(RETURN_TRAP)},
        'statePolicy': 'General registers zero except listed inputs; stack zero except return trap, arguments, explicit offsets. No other data exists.',
        'fieldMeanings': {
            address(POWER): {'type': 'int32', 'label': 'ice power', 'units': '100 raw units per displayed percent',
                             'confidence': 'manual/THPrac label, original display division and additive/clamp instructions'},
            address(PERFECT): {'type': 'int32', 'label': 'Perfect Freeze charge', 'units': '10000 per stock, maximum 30000',
                               'confidence': 'original additive/clamp function and activation call-site cost'},
            address(AREA): {'type': 'float32', 'label': 'cumulative frozen area', 'units': 'percentage points; not integer hundredths',
                            'confidence': 'original fld/fadd/fstp plus percentage update; THPrac/manual label'},
            '0x004b4d64': {'type': 'int32', 'label': 'motivation', 'units': '100 raw units per displayed percent; cap 100000',
                           'confidence': 'manual/THPrac label, original additive/clamp function; full life/death lifecycle unverified'},
            'manager+0x6b200': {'type': 'int32', 'label': 'saved activation power', 'confidence': 'observed original store/read'},
            'manager+0x6b214': {'type': 'float32', 'label': 'last mask area percentage', 'confidence': 'original percentage and gain slices'},
            'manager+0x6b218': {'type': 'int32', 'label': 'freeze-active candidate', 'confidence': 'static activation write and charge-dispatch checks; lifecycle unverified'},
        },
        'code': catalog, 'staticContexts': contexts, 'referenceSources': references,
        'thpracCommit': '33c78d989cca8891af07b513b468d88323c4fc1e',
        'unsupportedCallouts': {target: {**value, 'instructions': sorted(value['instructions'])}
                                for target, value in sorted(callouts.items())},
        'caseCount': len(records), 'cases': records,
        'limitations': [
            'Instruction emulation with synthetic states, not a Windows game run or native-CPU trace.',
            'Boundary stops and unsupported-call stops verify only the executed prefix, not the rest of a function.',
            'Caller update frequency, FPU state, reentrancy, graphics, sound, input timing and object lifecycle remain unverified.',
            'No OS API stubs, allocation simulation, imported API execution or replacement game model is supplied.',
            'Propagation geometry, bullet/laser eligibility and full cancellation/state transitions are not recovered here.',
            'Original bytes and local manual hashes are private derived analysis, not redistributable game source.',
        ],
    }


def main(argv=None):
    parser = argparse.ArgumentParser(description='Bounded read-only TH128 freeze-rule instruction emulation; never launches the game.')
    parser.add_argument('--exe', type=Path, required=True)
    parser.add_argument('--libs', type=Path)
    parser.add_argument('--report', type=Path, default=ROOT / 'local' / 'research' / 'freeze-fixtures.json')
    arguments = parser.parse_args(argv)
    try:
        destination = report_destination(arguments.report)
        if arguments.libs:
            sys.path.insert(0, str(arguments.libs.resolve()))
        result = probe(arguments.exe)
        destination.parent.mkdir(parents=True, exist_ok=True)
        destination.write_text(json.dumps(result, indent=2, ensure_ascii=False, allow_nan=False) + '\n', encoding='utf-8')
        print(f'{result["caseCount"]} bounded original-byte cases; original EXE unchanged; no game process or APIs.')
        print(f'Report: {destination}')
    except (OSError, ValueError, RuntimeError, ImportError) as error:
        print(f'Freeze probe failed: {error}', file=sys.stderr)
        return 1
    return 0


if __name__ == '__main__':
    raise SystemExit(main())
