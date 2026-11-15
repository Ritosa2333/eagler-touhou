import argparse
import hashlib
import json
import math
import struct
import sys
from pathlib import Path


ROOT = Path(__file__).resolve().parents[2]
sys.path.insert(0, str(Path(__file__).resolve().parent))

STACK_BASE, STACK_POINTER, STACK_SIZE, RETURN_TRAP = 0x100000, 0x108000, 65536, 0x200000


def address(value):
    return f'0x{value:08x}'


def datum(name, location, value, kind='i', writable=True):
    return {'name': name, 'address': location, 'bytes': value if isinstance(value, bytes)
            else struct.pack('<' + kind, value), 'writable': writable}


EXE_SHA256 = 'ba515750ece845b56640fe440592fc39ebb7bdd1ed87c541b67a19b148e767d4'
ECL_SHA256 = '181aa57ef1581ef19340b283550dd140982ea0b1c12290a5f8957eec2a3f7b35'
CONTEXT, TASK, STATE, COMMAND = 0x300000, 0x340000, 0x350000, 0x360000
EMITTER_OFFSET, EMITTER_SIZE = 0x4a0, 0x380
HANDLER_STOP = 0x41c52c
OPCODES = (435, 500, 501, 502, 504, 507, 509, 521, 522)


def report_destination(path):
    destination = Path(path).resolve()
    allowed = ROOT / 'local' / 'research'
    if (allowed.resolve() != allowed or destination.parent != allowed
            or destination.suffix != '.json' or not destination.name.startswith('bullet-')
            or destination.drive.lower() == 'e:'):
        raise ValueError('report must be deliverable local/research/bullet-*.json')
    return destination


def emulate(chunks, entry, data=(), *, registers=None, arguments=b'', stops=(), budget=2048, stack=None):
    chunks, data, stops = tuple(chunks), tuple(data), tuple(stops)
    if (type(budget) is not int or not 1 <= budget <= 20000 or len(chunks) > 32
            or sum(len(code) for _, code in chunks) > 65536 or len(data) > 64
            or sum(len(region['bytes']) for region in data) > 131072 or len(stops) > 16):
        raise ValueError('bullet probe code/data/state/instruction budget exceeded')
    import capstone
    import unicorn
    from capstone.x86_const import X86_OP_IMM
    from unicorn import x86_const

    decoder = capstone.Cs(capstone.CS_ARCH_X86, capstone.CS_MODE_32)
    decoder.detail = True
    instructions, spans, pages = {}, [], {}

    def add_pages(start, size, role):
        if type(start) is not int or not 0 <= start < 2**32 or not 0 < size <= 65536 or start + size > 2**32:
            raise ValueError('invalid explicit memory span')
        for page in range(start & ~4095, (start + size + 4095) & ~4095, 4096):
            roles = pages.setdefault(page, set())
            roles.add(role)
            if len(roles) > 1 and roles != {'code', 'readonly-data'}:
                raise ValueError('overlapping code/writable-data/stack pages')

    for start, code in chunks:
        if not isinstance(code, bytes) or not code or len(code) > 65536:
            raise ValueError('invalid code chunk')
        if any(start < end and begin < start + len(code) for begin, end in spans):
            raise ValueError('overlapping code chunks')
        decoded = list(decoder.disasm(code, start))
        if sum(item.size for item in decoded) != len(code):
            raise ValueError('undecodable original code chunk')
        instructions.update((item.address, item) for item in decoded)
        spans.append((start, start + len(code)))
        add_pages(start, len(code), 'code')
    if entry not in instructions or len({region['name'] for region in data}) != len(data):
        raise ValueError('entry is not code or duplicate state region names')
    for index, region in enumerate(data):
        start, payload = region['address'], region['bytes']
        if not isinstance(payload, bytes):
            raise ValueError('state payload must be immutable bytes')
        if any(start < other['address'] + len(other['bytes']) and other['address'] < start + len(payload)
               for other in data[:index]) or any(start < end and begin < start + len(payload) for begin, end in spans):
            raise ValueError('overlapping code/data spans')
        add_pages(start, len(payload), 'data' if region['writable'] else 'readonly-data')
    if not isinstance(arguments, bytes) or len(arguments) > 1024:
        raise ValueError('argument initialization budget exceeded')
    add_pages(STACK_BASE, STACK_SIZE, 'stack')
    add_pages(RETURN_TRAP, 1, 'trap')
    machine = unicorn.Uc(unicorn.UC_ARCH_X86, unicorn.UC_MODE_32)
    for page in pages:
        machine.mem_map(page, 4096)
    for start, code in chunks:
        machine.mem_write(start, code)
    for region in data:
        machine.mem_write(region['address'], region['bytes'])
    machine.mem_write(STACK_POINTER, struct.pack('<I', RETURN_TRAP) + arguments)
    for offset, payload in (stack or {}).items():
        if (type(offset) is not int or not isinstance(payload, bytes)
                or not STACK_BASE <= STACK_POINTER + offset <= STACK_BASE + STACK_SIZE - len(payload)):
            raise ValueError('stack initialization outside declared span')
        machine.mem_write(STACK_POINTER + offset, payload)
    for page, roles in pages.items():
        machine.mem_protect(page, 4096, unicorn.UC_PROT_READ |
                            (unicorn.UC_PROT_EXEC if 'code' in roles or 'trap' in roles else 0) |
                            (unicorn.UC_PROT_WRITE if roles & {'data', 'stack'} else 0))
    identifiers = {name: getattr(x86_const, 'UC_X86_REG_' + name.upper())
                   for name in ('eax', 'ebx', 'ecx', 'edx', 'esi', 'edi', 'ebp', 'esp', 'eip')}
    initial = dict.fromkeys(('eax', 'ebx', 'ecx', 'edx', 'esi', 'edi', 'ebp'), 0)
    if any(name not in initial for name in (registers or {})):
        raise ValueError('unsupported input register')
    initial.update(registers or {})
    initial['esp'] = STACK_POINTER
    for name, value in initial.items():
        machine.reg_write(identifiers[name], value & 0xffffffff)
    machine.reg_write(x86_const.UC_X86_REG_EFLAGS, 2)
    machine.reg_write(x86_const.UC_X86_REG_FPCW, 0x037f)
    trace, stop = [], {}

    def finish(reason, location, **details):
        if not stop:
            stop.update(reason=reason, address=address(location), **details)
        machine.emu_stop()

    def code_hook(emulator, location, size, user):
        if location in stops:
            finish('boundary', location)
            return
        instruction = instructions.get(location)
        if instruction is None or instruction.size != size:
            finish('outside_code', location)
            return
        if instruction.group(capstone.CS_GRP_CALL):
            operand = instruction.operands[0]
            if operand.type != X86_OP_IMM or operand.imm not in instructions:
                finish('unsupported_call', location, target=address(operand.imm)
                       if operand.type == X86_OP_IMM else instruction.op_str)
                return
        if instruction.group(capstone.CS_GRP_INT) or instruction.mnemonic in ('syscall', 'sysenter', 'hlt', 'in', 'out'):
            finish('unsupported_host_instruction', location)
            return
        trace.append(address(location))

    def memory_hook(emulator, access, location, size, value, user):
        if STACK_BASE <= location and location + size <= STACK_BASE + STACK_SIZE:
            return
        writing = access == unicorn.UC_MEM_WRITE
        for region in data:
            if region['address'] <= location and location + size <= region['address'] + len(region['bytes']):
                if writing and not region['writable']:
                    finish('readonly_data', machine.reg_read(identifiers['eip']), memory=address(location))
                return
        if not writing and any(begin <= location and location + size <= end for begin, end in spans):
            return
        finish('outside_data', machine.reg_read(identifiers['eip']), memory=address(location), size=size)

    machine.hook_add(unicorn.UC_HOOK_CODE, code_hook)
    machine.hook_add(unicorn.UC_HOOK_MEM_READ | unicorn.UC_HOOK_MEM_WRITE, memory_hook)
    try:
        machine.emu_start(entry, RETURN_TRAP, count=budget, timeout=500000)
    except unicorn.UcError as error:
        if not stop:
            stop.update(reason='fault', address=address(machine.reg_read(identifiers['eip'])), detail=str(error))
    result_registers = {name: machine.reg_read(identifier) for name, identifier in identifiers.items()}
    if not stop:
        stop.update(reason='returned' if result_registers['eip'] == RETURN_TRAP else 'budget_exhausted',
                    address=address(result_registers['eip']))
    memory = {region['name']: {'address': address(region['address']), 'writable': region['writable'],
                               'before': region['bytes'].hex(),
                               'after': bytes(machine.mem_read(region['address'], len(region['bytes']))).hex()}
              for region in data}
    final_stack = result_registers['esp']
    return {'entry': address(entry), 'inputRegisters': initial, 'arguments': arguments.hex(),
            'instructionBudget': budget, 'instructionsExecuted': len(trace), 'instructionTrace': trace,
            'stop': stop, 'stopBoundaries': [address(value) for value in stops], 'registers': result_registers,
            'memory': memory, 'stackDelta': final_stack - STACK_POINTER,
            'stackHead': bytes(machine.mem_read(final_stack, 64)).hex()
            if STACK_BASE <= final_stack <= STACK_BASE + STACK_SIZE - 64 else None,
            'mappedPages': {address(page): '+'.join(sorted(roles)) for page, roles in sorted(pages.items())}}


def load_first_emitter():
    sys.path.insert(0, str(ROOT / 'tools'))
    from th128_assets.formats import parse_ecl

    local = ROOT / 'local'
    catalog = json.loads((local / 'catalog.json').read_text(encoding='utf-8'))
    manifest = (local / catalog['variants']['ja']['manifest']).resolve()
    source = (manifest.parent / 'raw' / 'st_a1_1.ecl').resolve()
    if not source.is_relative_to(local.resolve()) or source.stat().st_size != 34116:
        raise ValueError('unexpected actual Japanese ECL resource')
    raw = source.read_bytes()
    if hashlib.sha256(raw).hexdigest() != ECL_SHA256:
        raise ValueError('first-emitter evidence requires pinned st_a1_1.ecl')
    program = parse_ecl(raw, max_commands=4096)
    commands = next(sub['commands'] for sub in program['subroutines'] if sub['name'] == 'GirlA1b')
    selected = [item for item in commands if 484 <= item['offset'] <= 816]
    return {'path': str(source), 'sha256': ECL_SHA256, 'subroutine': 'GirlA1b',
            'initialLocals': {'4': 1, '8': 0}, 'commands': selected}


def emitter_fields(context, slot=0):
    start = EMITTER_OFFSET + EMITTER_SIZE * slot
    block = context[start:start + EMITTER_SIZE]

    def read(kind, offset):
        return struct.unpack_from('<' + kind, block, offset)[0]

    fields = {name: read(kind, offset) for name, kind, offset in
              [('type', 'i', 0), ('color', 'i', 4), ('angle', 'f', 0x14),
               ('spacing', 'f', 0x18), ('speed', 'f', 0x1c), ('endSpeed', 'f', 0x20),
               ('count', 'h', 0x364), ('layers', 'h', 0x366), ('aimMode', 'H', 0x368),
               ('flags', 'I', 0x36c), ('shotSound', 'i', 0x370),
               ('transformSound', 'i', 0x374), ('extensionIndex', 'i', 0x378)]}
    fields['position'] = list(struct.unpack_from('<3f', block, 8))
    fields['extension0'] = {name: read(kind, offset) for name, kind, offset in
                            [('parameterR', 'f', 0x28), ('parameterS', 'f', 0x2c), ('parameterA', 'i', 0x38),
                             ('parameterB', 'i', 0x3c), ('type', 'i', 0x48), ('control', 'i', 0x4c)]}
    return fields


class NativeBullet:
    def __init__(self, executable):
        import pefile

        self.executable = Path(executable).resolve()
        if not 0 < self.executable.stat().st_size <= 1048576:
            raise ValueError('executable input size budget exceeded')
        self.binary = self.executable.read_bytes()
        if hashlib.sha256(self.binary).hexdigest() != EXE_SHA256:
            raise ValueError('addresses require the pinned Japanese TH128 executable')
        self.image = pefile.PE(data=self.binary)
        if self.image.OPTIONAL_HEADER.ImageBase != 0x400000:
            raise ValueError('unexpected PE image base')
        self.code = {}
        self.constants = {}
        indices = self.read(0x41c7c0, 360)
        targets = self.read(0x41c554, 620)
        self.handlers = {opcode: struct.unpack_from('<I', targets, indices[opcode - 256] * 4)[0]
                         for opcode in OPCODES}
        all_targets = sorted(set(struct.unpack_from('<I', targets, offset)[0]
                                 for offset in range(0, len(targets), 4)))
        self.ends = {opcode: min(value for value in all_targets if value > start)
                     for opcode, start in self.handlers.items()}

    def read(self, start, size):
        if type(size) is not int or not 0 < size <= 65536:
            raise ValueError('native slice size budget')
        result = bytes(self.image.get_data(start - 0x400000, size))
        if len(result) != size:
            raise ValueError('truncated original slice')
        return result

    def chunk(self, start, end):
        code = self.read(start, end - start)
        self.code[f'{start:#x}:{end:#x}'] = {'start': address(start), 'endExclusive': address(end),
                                          'sha256': hashlib.sha256(code).hexdigest(), 'bytes': code.hex()}
        return start, code

    def constant(self, start, size):
        payload = self.read(start, size)
        self.constants[address(start)] = {'bytes': payload.hex(), 'sha256': hashlib.sha256(payload).hexdigest()}
        return datum(address(start), start, payload, writable=False)

    def handler(self, opcode, payload, mask=0, difficulty=1, *, context=None, state=None, slot=0):
        if opcode not in OPCODES or len(payload) > 64 or not 0 <= slot < 16:
            raise ValueError('unsupported bounded handler fixture')
        original_context = bytearray(context or bytes(0x4000))
        struct.pack_into('<I', original_context, 0x3f7c, TASK)
        original_state = bytearray(state or bytes(0x1020))
        struct.pack_into('<I', original_state, 4, COMMAND)
        command = struct.pack('<iHHHBBI', 0, opcode, 16 + len(payload), mask, 255, len(payload) // 4, 0) + payload
        chunks = [self.chunk(self.handlers[opcode], self.ends[opcode]),
                  self.chunk(0x41e0c0, 0x41e0d0), self.chunk(0x46fed0, 0x46ff66),
                  self.chunk(0x41e110, 0x41e11e), self.chunk(0x46ff70, 0x470026),
                  self.chunk(HANDLER_STOP, HANDLER_STOP + 2)]
        data = [datum('context', CONTEXT, bytes(original_context)),
                datum('task', TASK, struct.pack('<2I', 0, STATE), writable=False),
                datum('state', STATE, bytes(original_state)), datum('command', COMMAND, command, writable=False),
                datum('difficulty', 0x4b4d0c, difficulty, writable=False)]
        if opcode == 435:
            chunks.extend([self.chunk(0x41e0d0, 0x41e10d), self.chunk(0x41c132, 0x41c140)])
            data.append(self.constant(0x41c928, 20))
        if opcode == 500:
            chunks.append(self.chunk(0x47bb30, 0x47bbaa))
            data.extend([self.constant(0x4a682c, 4), datum('memsetCpuFlag', 0x4dcd68, 0, writable=False)])
        if opcode == 501:
            data.extend([self.constant(0x4a6b00, 8), datum('managerPointer', 0x4b8930, 0x380000, kind='I', writable=False),
                         datum('manager', 0x380000, bytes(0x64))])
        result = emulate(chunks, self.handlers[opcode], data, registers={'ebx': CONTEXT},
                         stops=[HANDLER_STOP], budget=2048)
        final_context = bytes.fromhex(result['memory']['context']['after'])
        final_state = bytes.fromhex(result['memory']['state']['after'])
        result['opcode'], result['difficulty'], result['parameterMask'] = opcode, difficulty, mask
        result['emitter'] = emitter_fields(final_context, slot)
        result['locals'] = {str(offset): struct.unpack_from('<i', final_state, 8 + offset)[0] for offset in (4, 8)}
        start = EMITTER_OFFSET + EMITTER_SIZE * slot
        result['emitterBytes'] = final_context[start:start + EMITTER_SIZE].hex()
        first, last = EMITTER_OFFSET, EMITTER_OFFSET + 16 * EMITTER_SIZE
        result['adjacentEmittersUnchanged'] = (final_context[first:start] == original_context[first:start]
                                             and final_context[start + EMITTER_SIZE:last]
                                             == original_context[start + EMITTER_SIZE:last])
        result['stateBeforeSha256'] = hashlib.sha256(original_context + original_state).hexdigest()
        result['memory'] = {name: record for name, record in result['memory'].items()
                            if name not in ('context', 'state')}
        return result, final_context, final_state

    def geometry_prefix(self, block, bullet_index=0, layer_index=0, aim_angle=1.5707963705062866):
        if len(block) != EMITTER_SIZE or not math.isfinite(aim_angle):
            raise ValueError('invalid emitter geometry fixture')
        manager = bytearray(0x64)
        struct.pack_into('<I', manager, 0x10, 0x390000)
        chunks = [self.chunk(0x4091f0, 0x40971d), self.chunk(0x46a660, 0x46a6f9),
                  self.chunk(0x40d110, 0x40d130)]
        data = [datum('emitter', 0x370000, block, writable=False),
                datum('manager', 0x380000, bytes(manager)), datum('bullet', 0x390000, bytes(0x11b8)),
                self.constant(0x409a38, 36), self.constant(0x4a6578, 72),
                self.constant(0x4a68e8, 8), self.constant(0x4a6aa0, 4)]
        result = emulate(chunks, 0x4091f0, data, arguments=struct.pack('<4If', 0x380000, 0x370000,
                         bullet_index, layer_index, aim_angle), stops=[0x409717], budget=2048)
        result['completedEmission'] = False
        result['entryKind'] = 'actual-call-target-prefix'
        result['input'] = {'emitter': emitter_fields(bytes(EMITTER_OFFSET) + block),
                           'bulletIndex': bullet_index, 'layerIndex': layer_index, 'aimAngle': aim_angle}
        bullet = bytes.fromhex(result['memory']['bullet']['after'])
        stack = bytes.fromhex(result['stackHead']) if result['stackHead'] else b''
        result['geometry'], result['geometryBits'] = {}, {}
        if result['stop']['reason'] == 'boundary':
            relative = result['registers']['ebp'] - 4 - result['registers']['esp']
            values = {'rawAngle': stack[relative:relative + 4], 'angle': bullet[0x9f4:0x9f8],
                      'speed': bullet[0x9f0:0x9f4], 'velocityX': bullet[0x9e4:0x9e8],
                      'velocityY': bullet[0x9e8:0x9ec]}
            result['geometry'] = {name: struct.unpack('<f', payload)[0] for name, payload in values.items()}
            result['geometryBits'] = {name: struct.unpack('<I', payload)[0] for name, payload in values.items()}
        result['memory'] = {name: record for name, record in result['memory'].items()
                            if name not in ('bullet', 'emitter', 'manager')}
        return result


def integers(*values):
    return struct.pack('<' + 'i' * len(values), *values)


def probe(executable):
    native = NativeBullet(executable)
    resource = load_first_emitter()
    cases = []

    def record(identifier, opcode, payload, **options):
        result, context, state = native.handler(opcode, payload, **options)
        result['id'] = identifier
        cases.append(result)
        return result, context, state

    for slot in (0, 1, 15):
        context = bytearray(0x4000)
        context[EMITTER_OFFSET:0x3ca0] = b'\xa5' * (16 * EMITTER_SIZE)
        record(f'reset-{slot}', 500, integers(slot), context=context, slot=slot)
    for mode in (-1, 65536, 65537):
        record(f'mode-{mode}', 507, integers(0, mode))
    record('extension-distinct-parameters', 509, integers(0, 0, 42, 37, -123456, 987654)
           + struct.pack('<2f', .625, -8.25))
    for difficulty in (-1, 0, 1, 2, 3, 4, 5):
        state = bytearray(0x1020)
        struct.pack_into('<i', state, 16, 91)
        record(f'choose-{difficulty}', 435, integers(8, 3, 3, 5, 7), mask=1, difficulty=difficulty, state=state)
        result, context, state = native.handler(521, integers(0) + struct.pack('<8f', *[value + .5 for value in range(1, 9)]),
                                               difficulty=difficulty)
        record(f'ranked-{difficulty}', 522, integers(0, 1, 32767, 65535, 32768, 4, 5, 65536, -1),
               difficulty=difficulty, context=context, state=state)
        cases[-1]['speedStep'] = result
    for difficulty in range(5):
        context, state, steps = None, bytearray(0x1020), []
        struct.pack_into('<i', state, 12, 1)
        for command in resource['commands']:
            if command['opcode'] not in OPCODES or command['opcode'] == 501:
                continue
            result, context, state = native.handler(command['opcode'], bytes.fromhex(command['raw']),
                                                   command['mask'], difficulty, context=context, state=state)
            steps.append(result)
        result = dict(steps[-1])
        result.update(id=f'first-{difficulty}', steps=steps)
        cases.append(result)
        record(f'fire-prefix-{difficulty}', 501, integers(0), difficulty=difficulty, context=context, state=state)
        sample = native.geometry_prefix(context[EMITTER_OFFSET:EMITTER_OFFSET + EMITTER_SIZE])
        sample['id'] = f'geometry-first-{difficulty}'
        cases.append(sample)
    for label, count in [('odd', 3), ('even', 4)]:
        block = bytearray(EMITTER_SIZE)
        struct.pack_into('<2f', block, 0x14, 0, .25)
        struct.pack_into('<2f', block, 0x1c, 4, 1)
        struct.pack_into('<3h', block, 0x364, count, 3, 0)
        for index in range(count):
            sample = native.geometry_prefix(bytes(block), index, 1, 1.0)
            sample['id'] = f'geometry-{label}-{index}'
            cases.append(sample)
    for mode, angle, count, layers, bullet_index, layer_index in [
            (0, 3.2, 3, 1, 2, 0), (0, -3.2, 3, 1, 1, 0),
            (1, .5, 1, 3, 0, 0), (1, -.5, 1, 3, 0, 2),
            (0, 0, 32767, 1, 32766, 0), (0, 0, 1, 32767, 0, 32766)]:
        block = bytearray(EMITTER_SIZE)
        struct.pack_into('<4f', block, 0x14, angle, .03926991, 2, .5)
        struct.pack_into('<3h', block, 0x364, count, layers, mode)
        sample = native.geometry_prefix(bytes(block), bullet_index, layer_index, .75)
        sample['id'] = f'geometry-vector-{len(cases)}'
        cases.append(sample)
    selector = native.read(0x418030, 0x3d)
    wrapper = native.read(0x417fe0, 11)
    mapping = []
    for opcode in OPCODES:
        index_address = 0x41c7c0 + opcode - 256
        index = native.read(index_address, 1)[0]
        target_address = 0x41c554 + index * 4
        mapping.append({'opcode': opcode, 'indexAddress': address(index_address), 'index': index,
                        'targetAddress': address(target_address), 'targetBytes': native.read(target_address, 4).hex()})
    after = hashlib.sha256(native.executable.read_bytes()).hexdigest()
    report = {'schema': 'th128-original-byte-bullet-fixtures/1',
              'executable': {'path': str(native.executable), 'sha256': EXE_SHA256, 'sha256After': after,
                             'unchanged': after == EXE_SHA256},
              'nativeCpuTrace': False, 'windowsGameProcessLaunched': False,
              'dispatch': {str(opcode): {'handler': address(native.handlers[opcode]), 'endExclusive': address(native.ends[opcode])}
                           for opcode in OPCODES}, 'resource': resource, 'code': native.code,
              'staticDispatchEvidence': {'selectorStart': '0x00418030', 'selectorBytes': selector.hex(),
                                         'selectorSha256': hashlib.sha256(selector).hexdigest(),
                                         'wrapperStart': '0x00417fe0', 'wrapperBytes': wrapper.hex(),
                                         'wrapperSha256': hashlib.sha256(wrapper).hexdigest(), 'mapping': mapping},
              'constants': native.constants, 'cases': cases, 'caseCount': len(cases),
              'limits': {'codeBytes': 65536, 'codeChunks': 32, 'dataBytes': 131072, 'regions': 64,
                         'instructionsPerCase': 20000, 'stackBytes': 65536},
              'limitations': ['Original-byte emulation, not native Windows CPU execution.',
                              'Handler cases stop before the shared dispatcher epilogue.',
                              'Opcode 501 stops at the unsupported original emission call; no return stub.',
                              'Geometry cases call the actual 0x4091f0 target but stop at 0x409717 before animation/ETEX.',
                              'Aim angle is an explicit fixture input, not a completed native aim resolver.',
                              'ETEX storage is proven, not its later update behavior. Opcode 509 remains unsupported at runtime.']}
    native.image.close()
    return report


def main(argv=None):
    parser = argparse.ArgumentParser(description='Bounded TH128 original-byte first-emitter evidence; no game process.')
    parser.add_argument('--exe', type=Path, required=True)
    parser.add_argument('--output', type=Path, default=ROOT / 'local/research/bullet-fixtures.json')
    args = parser.parse_args(argv)
    destination = report_destination(args.output)
    report = probe(args.exe)
    sys.path.insert(0, str(ROOT / 'tools'))
    from th128_assets.pipeline import atomic_json
    atomic_json(destination, report)
    print(json.dumps({'report': str(destination), 'cases': report['caseCount'],
                      'unchanged': report['executable']['unchanged']}))


if __name__ == '__main__':
    main()
