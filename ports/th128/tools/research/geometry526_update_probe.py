import argparse
import hashlib
import json
from pathlib import Path
import struct

from bullet_probe import NativeBullet, ROOT, EXE_SHA256, CONTEXT, datum
from enemy_probe import emulate as direct_emulate
from freeze_probe import STACK_BASE, STACK_SIZE, STACK_POINTER, RETURN_TRAP, address
from geometry526_probe import OBJECT, VM_TABLE, VMS, VERTICES, POSITIONS, STRIPS

D3DX_SHA256 = 'f8b9cfab7fffbc8f98e41aa439d72921dc180634a1febca2a9d41a0df35d3472'
POSITION = 0x3e0000


def emulate(chunks, entry, data=(), registers=None, arguments=b'', stops=(), budget=4096,
            stack=None, bindings=None):
    import capstone
    from capstone.x86_const import X86_OP_IMM, X86_OP_MEM
    import unicorn
    from unicorn import x86_const

    if not 1 <= budget <= 400000 or len(chunks) > 64 or len(data) > 256 or len(arguments) > 1024:
        raise ValueError('geometry526 bounded emulator input budget')
    decoder = capstone.Cs(capstone.CS_ARCH_X86, capstone.CS_MODE_32)
    decoder.detail = True
    instructions, spans, pages = {}, [], {}

    def map_pages(start, size, role):
        if not 0 <= start < start + size <= 0x100000000:
            raise ValueError('invalid explicit memory range')
        for page in range(start & ~4095, (start + size + 4095) & ~4095, 4096):
            if page in pages and pages[page] != role:
                raise ValueError('code/data/stack page overlap')
            pages[page] = role

    for start, payload in chunks:
        if not isinstance(payload, bytes) or not 0 < len(payload) <= 65536:
            raise ValueError('invalid code chunk')
        if any(start < end and begin < start + len(payload) for begin, end in spans):
            raise ValueError('overlapping code chunks')
        decoded = list(decoder.disasm(payload, start))
        if sum(item.size for item in decoded) != len(payload):
            raise ValueError('undecodable explicit code')
        instructions.update((item.address, item) for item in decoded)
        spans.append((start, start + len(payload)))
        map_pages(start, len(payload), 'code')
    if entry not in instructions or len({item['name'] for item in data}) != len(data):
        raise ValueError('entry outside original code or duplicate data names')
    for index, region in enumerate(data):
        start, payload = region['address'], region['bytes']
        if not isinstance(payload, bytes) or not 0 < len(payload) <= 65536:
            raise ValueError('invalid explicit data')
        if any(start < other['address'] + len(other['bytes']) and other['address'] < start + len(payload)
               for other in data[:index]):
            raise ValueError('overlapping data spans')
        map_pages(start, len(payload), 'data')
    for location, (slot, target) in (bindings or {}).items():
        instruction = instructions.get(location)
        if instruction is None or target not in instructions:
            raise ValueError('import target must be an explicit original instruction')
        if not (instruction.group(capstone.CS_GRP_CALL) or instruction.mnemonic == 'jmp'):
            raise ValueError('import binding requires a call/jump')
        operand = instruction.operands[0]
        if (operand.type != X86_OP_MEM or operand.size != 4 or operand.mem.base
                or operand.mem.index or operand.mem.segment or operand.mem.disp != slot):
            raise ValueError('import must read its exact absolute32-bit slot')
        source = next((region for region in data if region['address'] <= slot
                       and slot + 4 <= region['address'] + len(region['bytes'])), None)
        if (source is None or source['writable']
                or struct.unpack_from('<I', source['bytes'], slot - source['address'])[0] != target):
            raise ValueError('import must resolve through a declared matching readonly slot')
    map_pages(STACK_BASE, STACK_SIZE, 'stack')
    map_pages(RETURN_TRAP, 1, 'trap')
    machine = unicorn.Uc(unicorn.UC_ARCH_X86, unicorn.UC_MODE_32)
    for page in pages:
        machine.mem_map(page, 4096)
    for start, payload in chunks:
        machine.mem_write(start, payload)
    for region in data:
        machine.mem_write(region['address'], region['bytes'])
    machine.mem_write(STACK_POINTER, struct.pack('<I', RETURN_TRAP) + arguments)
    for offset, payload in (stack or {}).items():
        if not isinstance(payload, bytes) or not STACK_BASE <= STACK_POINTER + offset <= STACK_BASE + STACK_SIZE - len(payload):
            raise ValueError('stack initializer exceeds declared bounded stack')
        machine.mem_write(STACK_POINTER + offset, payload)
    for page, role in pages.items():
        machine.mem_protect(page, 4096, unicorn.UC_PROT_READ |
                            (unicorn.UC_PROT_EXEC if role in ('code', 'trap') else unicorn.UC_PROT_WRITE))
    names = ('eax', 'ebx', 'ecx', 'edx', 'esi', 'edi', 'ebp', 'esp', 'eip', 'eflags')
    identifiers = {name: getattr(x86_const, 'UC_X86_REG_' + name.upper()) for name in names}
    initial = dict.fromkeys(names[:7], 0)
    if any(name not in initial for name in (registers or {})):
        raise ValueError('unsupported input register')
    initial.update(registers or {})
    initial.update(esp=STACK_POINTER, eflags=2)
    for name, value in initial.items():
        machine.reg_write(identifiers[name], value & 0xffffffff)
    machine.reg_write(x86_const.UC_X86_REG_FPCW, 0x037f)
    machine.reg_write(x86_const.UC_X86_REG_MXCSR, 0x1f80)
    stop, trace, imported = {}, [], []

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
        if instruction.group(capstone.CS_GRP_CALL) or instruction.mnemonic == 'jmp':
            operand = instruction.operands[0]
            if operand.type == X86_OP_IMM:
                if operand.imm not in instructions and operand.imm not in stops:
                    finish('unsupported_transfer', location, target=address(operand.imm))
                    return
            else:
                binding = (bindings or {}).get(location)
                if binding is None:
                    finish('unsupported_transfer', location, target=instruction.op_str)
                    return
                slot, target = binding
                if struct.unpack('<I', bytes(machine.mem_read(slot, 4)))[0] != target:
                    finish('import_changed', location)
                    return
                imported.append({'instruction': address(location), 'slot': address(slot), 'target': address(target)})
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
        machine.emu_start(entry, RETURN_TRAP, count=budget, timeout=5000000)
    except unicorn.UcError as error:
        if not stop:
            stop.update(reason='fault', address=address(machine.reg_read(identifiers['eip'])), detail=str(error))
    final = {name: machine.reg_read(identifier) for name, identifier in identifiers.items()}
    if not stop:
        stop.update(reason='returned' if final['eip'] == RETURN_TRAP else 'budget_or_timeout_exhausted',
                    address=address(final['eip']))
    return {'entry': address(entry), 'inputRegisters': initial, 'arguments': arguments.hex(),
            'stackInitialization': {str(offset): payload.hex() for offset, payload in (stack or {}).items()},
            'instructionBudget': budget, 'instructionsExecuted': len(trace), 'instructionTrace': trace,
            'importsExecuted': imported, 'stop': stop, 'stackDelta': final['esp'] - STACK_POINTER,
            'registers': final, 'memory': {region['name']: {
                'address': address(region['address']), 'before': region['bytes'].hex(),
                'after': bytes(machine.mem_read(region['address'], len(region['bytes']))).hex(),
                'writable': region['writable']} for region in data}}


class NativeNormalize:
    def __init__(self, path):
        import pefile

        self.path = Path(path).resolve()
        self.binary = self.path.read_bytes()
        if hashlib.sha256(self.binary).hexdigest() != D3DX_SHA256:
            raise ValueError('D3DX addresses require pinned read-only32-bit d3dx9_42.dll')
        self.image = pefile.PE(data=self.binary)
        if self.image.FILE_HEADER.Machine != 0x14c or self.image.OPTIONAL_HEADER.ImageBase != 0x10000000:
            raise ValueError('unexpected D3DX PE architecture/base')
        exports = [symbol for symbol in self.image.DIRECTORY_ENTRY_EXPORT.symbols if symbol.name == b'D3DXVec3Normalize']
        if len(exports) != 1 or exports[0].forwarder:
            raise ValueError('D3DXNormalize export is absent or forwarded')
        self.export = 0x10000000 + exports[0].address
        self.code, self.constants = {}, {}
        thunk = self.chunk(self.export, self.export + 6)
        if thunk[1][:2] != b'\xff\x25':
            raise ValueError('unexpected original normalize export dispatch')
        self.slot = struct.unpack_from('<I', thunk[1], 2)[0]
        self.setter = direct_emulate([self.chunk(0x101a392f, 0x101a3b2c)], 0x101a392f,
            [datum('dispatch', 0x101b1068, self.read(0x101b1190, 0x118))],
            arguments=struct.pack('<I', 0x101b1068), budget=512)
        if self.setter['stop']['reason'] != 'returned':
            raise ValueError(self.setter['stop'])
        self.dispatch = bytes.fromhex(self.setter['memory']['dispatch']['after'])
        self.target = struct.unpack_from('<I', self.dispatch, self.slot - 0x101b1068)[0]
        if self.target != 0x101a4013:
            raise ValueError('actual SSE2 setter did not resolve expected normalize target')
        self.chunks = [thunk, self.chunk(self.target, 0x101a40a7)]
        self.data = [datum('normalizeDispatch', self.slot, struct.pack('<I', self.target), writable=False),
                     self.constant(0x10018050, 0x24)]

    def read(self, start, size):
        result = bytes(self.image.get_data(start - 0x10000000, size))
        if len(result) != size:
            raise ValueError('truncated original D3DX slice')
        return result

    def chunk(self, start, end):
        payload = self.read(start, end - start)
        self.code[f'{start:#x}:{end:#x}'] = {'start': address(start), 'endExclusive': address(end),
            'bytes': payload.hex(), 'sha256': hashlib.sha256(payload).hexdigest()}
        return start, payload

    def constant(self, start, size):
        payload = self.read(start, size)
        self.constants[address(start)] = {'bytes': payload.hex(), 'sha256': hashlib.sha256(payload).hexdigest()}
        return datum(address(start), start, payload, writable=False)

    def evidence(self):
        return {'dll': {'sha256': D3DX_SHA256, 'sha256After': hashlib.sha256(self.path.read_bytes()).hexdigest()},
            'export': address(self.export), 'dispatchSlot': address(self.slot), 'resolvedTarget': address(self.target),
            'setter': self.setter, 'code': self.code, 'constants': self.constants, 'windowsDllLoaded': False,
            'apiReturnsAssigned': False, 'cpuMode': 'declared SSE2 dispatch from executed original setter; not a loader/CPU-detection trace'}


def probe_normalize(path):
    native = NativeNormalize(path)
    try:
        cases = []
        for vector in ((0, 0, 0), (3, 4, 0), (-5, 12, 0), (1, 2, 3), (1e-8, 0, 0), (0, -7, 0)):
            result = emulate(native.chunks, native.export,
                [datum('vector', POSITION, struct.pack('<3f', *vector), writable=False),
                 datum('output', POSITION + 16, bytes([0xa5]) * 12), *native.data],
                arguments=struct.pack('<2I', POSITION + 16, POSITION),
                bindings={native.export: (native.slot, native.target)}, budget=128)
            if result['stop']['reason'] != 'returned':
                raise ValueError(result['stop'])
            result.update(input=list(vector), outputAfter=result['memory']['output']['after'])
            cases.append(result)
        return {**native.evidence(), 'cases': cases}
    finally:
        native.image.close()


def probe(executable, dll):
    native, normalize = NativeBullet(executable), NativeNormalize(dll)
    try:
        chunks = [native.chunk(0x416d89, 0x41722d), native.chunk(0x412550, 0x41267f),
                  native.chunk(0x412700, 0x412783), native.chunk(0x46a700, 0x46a790),
                  native.chunk(0x490190, 0x490222), native.chunk(0x491b78, 0x491c2a), *normalize.chunks]
        cases = []
        inputs = ((17, 17, 0, 100, 0, 16, 80, 0x00800080, 0, 0, 1),
                  (17, 17, 0, 100, 0, 80, 80, 0x00800080, 0.5, -0.25, 1),
                  (3, 3, -190, 0, 0, 40, 10, 0xff123456, 2, -2, 0.5),
                  (4, 4, 190, 445, 0, 120, 120, 0xffffffff, -3, 3, 1))
        for columns, rows, x, y, z, scale, radius, color, angle_x, angle_y, delta in inputs:
            context = bytearray(0x4000)
            struct.pack_into('<I2fI2f', context, 0x3f80, OBJECT, radius, scale, color, angle_x, angle_y)
            header = struct.pack('<6I', columns, rows, 0, VM_TABLE, VERTICES, POSITIONS)
            pointers, vms = bytearray((columns - 1) * 4), bytearray((columns - 1) * 0x4e8)
            for index in range(columns - 1):
                struct.pack_into('<I', pointers, index * 4, VMS + index * 0x4e8)
                struct.pack_into('<I', vms, index * 0x4e8 + 0x4ac, STRIPS + index * rows * 56)
            data = [datum('context', CONTEXT, bytes(context)), datum('object', OBJECT, header, writable=False),
                    datum('vmTable', VM_TABLE, bytes(pointers), writable=False), datum('vms', VMS, bytes(vms), writable=False),
                    datum('vertices', VERTICES, bytes([0xa5]) * (columns * rows * 28)),
                    datum('positions', POSITIONS, bytes([0xa5]) * (columns * rows * 12)),
                    datum('strips', STRIPS, bytes([0xa5]) * ((columns - 1) * rows * 56)),
                    datum('position', POSITION, struct.pack('<3f', x, y, z), writable=False),
                    datum('delta', 0x4b6f58, struct.pack('<f', delta), writable=False),
                    datum('iatNormalize', 0x49a2a0, struct.pack('<I', normalize.export), writable=False),
                    datum('crtSseEnabled', 0x4dbc20, struct.pack('<I', 0), writable=False),
                    datum('crtExceptions', 0x4dbc10, struct.pack('<I', 0), writable=False),
                    native.constant(0x4a6500, 0xb00), *normalize.data]
            result = emulate(chunks, 0x416d89, data, registers={'ebx': CONTEXT},
                stack={0x1c: struct.pack('<I', POSITION)}, stops=(0x41722d,), budget=400000,
                bindings={0x417023: (0x49a2a0, normalize.export), normalize.export: (normalize.slot, normalize.target)})
            if result['stop'] != {'reason': 'boundary', 'address': '0x0041722d'}:
                raise ValueError(result['stop'])
            after = bytes.fromhex(result['memory']['context']['after'])
            copied = direct_emulate([chunks[2]], 0x412700,
                [datum(region['name'], region['address'], bytes.fromhex(result['memory'][region['name']]['after']),
                       writable=region['writable']) for region in data[:7]],
                registers={'ebx': OBJECT}, budget=40000)
            if copied['stop']['reason'] != 'returned':
                raise ValueError(copied['stop'])
            result.update(input={'columns': columns, 'rows': rows, 'position': [x, y, z], 'scale': scale,
                'radius': radius, 'color': color, 'angles': [angle_x, angle_y], 'delta': delta},
                after={'scale': struct.unpack_from('<f', after, 0x3f88)[0],
                       'angles': list(struct.unpack_from('<2f', after, 0x3f90))},
                verticesAfter=result['memory']['vertices']['after'], positionsAfter=result['memory']['positions']['after'],
                stripsAfter=copied['memory']['strips']['after'], copy=copied)
            cases.append(result)
        return {'schema': 'th128-original-geometry526-update/1',
            'executable': {'sha256': EXE_SHA256, 'sha256After': hashlib.sha256(native.executable.read_bytes()).hexdigest()},
            'normalize': normalize.evidence(), 'updateCases': cases, 'code': native.code, 'constants': native.constants,
            'allocationExecuted': False, 'apiReturnsAssigned': False, 'windowsGameProcessLaunched': False,
            'nativeCpuTrace': False, 'generatorSha256': hashlib.sha256(Path(__file__).read_bytes()).hexdigest(),
            'scope': 'complete original416d89..41722d geometry update prefix and real412700 callback over explicit preallocated buffers; genuine installed32-bit D3DX SSE2 setter/helper bytes execute through exact declared readonly import slots; declared legacy CRT x87 path; not OS loader/allocation/full enemy trace'}
    finally:
        native.image.close()
        normalize.image.close()


if __name__ == '__main__':
    parser = argparse.ArgumentParser()
    parser.add_argument('--exe', required=True)
    parser.add_argument('--d3dx', required=True)
    arguments = parser.parse_args()
    report = probe(arguments.exe, arguments.d3dx)
    (ROOT / 'local/research/geometry526-update-fixtures.json').write_text(
        json.dumps(report, indent=2) + '\n', encoding='utf-8', newline='\n')
    print('four complete original526 update prefixes and strip callbacks; genuine D3DX SSE2 bytes, no assigned API returns')
