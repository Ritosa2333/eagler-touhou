import argparse
import hashlib
import json
import math
import struct
import sys
from dataclasses import dataclass
from pathlib import Path


ROOT = Path(__file__).resolve().parents[2]
EXE_SHA256 = 'ba515750ece845b56640fe440592fc39ebb7bdd1ed87c541b67a19b148e767d4'
PLAYER = 0x300000
MANAGER = 0x380000
SHOT = 0x390000
STACK = 0x100000
SP = 0x108000
RETURN = 0x200000
NATIVE_SPEEDS = (576, 256, 407, 181)
SHOT_SHA256 = 'aed90a45b565510e97cc7d9ab4846d8c93914e57faafe9339e9ab03d47b1511b'


class ProbeError(ValueError):
    pass


@dataclass(frozen=True)
class Region:
    name: str
    address: int
    data: bytes
    writable: bool = True


def report_destination(path):
    destination = Path(path).resolve()
    parent = ROOT / 'local/research'
    if (parent.resolve() != parent or destination.parent != parent
            or destination.suffix != '.json' or not destination.name.startswith('player-')
            or destination.drive.lower() == 'e:'):
        raise ValueError('report must be deliverable local/research/player-*.json')
    return destination


def emulate(chunks, entry, data=(), *, registers=None, stops=(), budget=2048, stack=None):
    import capstone
    import unicorn
    from capstone.x86_const import X86_OP_IMM
    from unicorn import x86_const

    if type(budget) is not int or not 1 <= budget <= 200000:
        raise ValueError('invalid instruction budget')
    chunks, data, stops = tuple(chunks), tuple(data), tuple(stops)
    if not 1 <= len(chunks) <= 64 or len(data) > 256 or sum(len(code) for start, code in chunks) > 1048576:
        raise ValueError('code/data region count or code byte budget exceeded')
    instructions, spans, pages = {}, [], {}

    def pages_for(start, size, category):
        if type(start) is not int or not 0 <= start < 2**32 or not 0 < size <= 1048576 or start + size > 2**32:
            raise ValueError('invalid explicit address span')
        for page in range(start & ~4095, (start + size + 4095) & ~4095, 4096):
            if page in pages and pages[page] != category:
                raise ValueError('overlapping code/data/stack pages')
            pages[page] = category

    decoder = capstone.Cs(capstone.CS_ARCH_X86, capstone.CS_MODE_32)
    decoder.detail = True
    for start, code in chunks:
        if not isinstance(code, bytes) or not 0 < len(code) <= 65536:
            raise ValueError('invalid code chunk')
        if any(start < end and begin < start + len(code) for begin, end in spans):
            raise ValueError('overlapping code chunks')
        spans.append((start, start + len(code)))
        pages_for(start, len(code), 'code')
        decoded = list(decoder.disasm(code, start))
        if sum(item.size for item in decoded) != len(code):
            raise ValueError('code chunk contains undecodable bytes')
        instructions.update((item.address, item) for item in decoded)
    if entry not in instructions:
        raise ValueError('entry is not an instruction boundary')
    if len({region.name for region in data}) != len(data) or sum(len(region.data) for region in data) > 2097152:
        raise ValueError('duplicate data name or data budget exceeded')
    for index, region in enumerate(data):
        if not isinstance(region.data, bytes):
            raise ValueError('data region must be immutable bytes')
        if any(region.address < other.address + len(other.data)
               and other.address < region.address + len(region.data) for other in data[:index]):
            raise ValueError('overlapping data regions')
        pages_for(region.address, len(region.data), 'data')
    pages_for(STACK, 65536, 'stack')
    pages_for(RETURN, 1, 'trap')
    if len(pages) > 2048:
        raise ValueError('mapped page budget exceeded')
    machine = unicorn.Uc(unicorn.UC_ARCH_X86, unicorn.UC_MODE_32)
    for page in pages:
        machine.mem_map(page, 4096)
    for start, code in chunks:
        machine.mem_write(start, code)
    for region in data:
        machine.mem_write(region.address, region.data)
    machine.mem_write(SP, struct.pack('<I', RETURN))
    for offset, payload in (stack or {}).items():
        if type(offset) is not int or not isinstance(payload, bytes) or not 0 <= offset <= 1024 - len(payload):
            raise ValueError('invalid explicit stack initialization')
        machine.mem_write(SP + offset, payload)
    for page, category in pages.items():
        machine.mem_protect(page, 4096, unicorn.UC_PROT_READ |
                            (unicorn.UC_PROT_EXEC if category in ('code', 'trap') else unicorn.UC_PROT_WRITE))
    register_ids = {name: getattr(x86_const, 'UC_X86_REG_' + name.upper())
                    for name in ('eax', 'ebx', 'ecx', 'edx', 'esi', 'edi', 'ebp', 'esp', 'eip')}
    initial = dict.fromkeys(('eax', 'ebx', 'ecx', 'edx', 'esi', 'edi', 'ebp'), 0)
    if any(name not in initial for name in (registers or {})):
        raise ValueError('unsupported input register')
    initial.update(registers or {})
    initial['esp'] = SP
    for name, value in initial.items():
        machine.reg_write(register_ids[name], value & 0xffffffff)
    machine.reg_write(x86_const.UC_X86_REG_EFLAGS, 2)
    machine.reg_write(x86_const.UC_X86_REG_FPCW, 0x037f)
    trace, failure, stopped = [], [], []

    def fail(message):
        failure.append(message)
        machine.emu_stop()

    def code_hook(emulator, location, size, context):
        if location in stops:
            stopped.append(location)
            machine.emu_stop()
            return
        instruction = instructions.get(location)
        if instruction is None or instruction.size != size:
            fail(f'outside declared code at {location:#x}')
            return
        if instruction.group(capstone.CS_GRP_CALL):
            target = instruction.operands[0]
            if target.type != X86_OP_IMM or target.imm not in instructions:
                fail(f'unsupported native call at {location:#x}: {instruction.op_str}')
                return
        if instruction.group(capstone.CS_GRP_INT) or instruction.mnemonic in ('syscall', 'sysenter', 'hlt', 'in', 'out'):
            fail(f'unsupported host instruction at {location:#x}')
            return
        trace.append(location)

    def memory_hook(emulator, access, location, size, value, context):
        if STACK <= location and location + size <= STACK + 65536:
            return
        writing = access == unicorn.UC_MEM_WRITE
        for region in data:
            if region.address <= location and location + size <= region.address + len(region.data):
                if writing and not region.writable:
                    fail(f'read-only memory write at {location:#x}')
                return
        if not writing and any(begin <= location and location + size <= end for begin, end in spans):
            return
        fail(f'outside declared memory at {location:#x}, size {size}')

    machine.hook_add(unicorn.UC_HOOK_CODE, code_hook)
    machine.hook_add(unicorn.UC_HOOK_MEM_READ | unicorn.UC_HOOK_MEM_WRITE, memory_hook)
    try:
        machine.emu_start(entry, RETURN, count=budget)
    except unicorn.UcError as error:
        if not failure:
            failure.append(f'native memory/instruction fault: {error}')
    if failure:
        raise ProbeError(failure[0])
    location = machine.reg_read(register_ids['eip'])
    if not stopped and location != RETURN:
        raise ProbeError(f'instruction budget exhausted at {location:#x}')
    if not stopped and (not trace or not instructions[trace[-1]].group(capstone.CS_GRP_RET)):
        raise ProbeError('return trap reached without a native return instruction')
    return {'stop': location, 'trace': trace,
            'registers': {name: machine.reg_read(identifier) for name, identifier in register_ids.items()},
            'memory': {region.name: bytes(machine.mem_read(region.address, len(region.data))) for region in data}}


def i32(value):
    return (value + 2**31) % 2**32 - 2**31


def checked_i32(value):
    if type(value) is not int or not -2**31 <= value < 2**31:
        raise ValueError('expected signed int32')
    return value


def keys(value):
    if type(value) is not int or not 0 <= value <= 65535:
        raise ValueError('expected replay uint16 keys')
    return value


def field(name, offset, value, kind='i', writable=True):
    return Region(name, PLAYER + offset, struct.pack('<' + kind, value), writable)


def load_actual_shot():
    local = (ROOT / 'local').resolve()
    catalog = json.loads((local / 'catalog.json').read_text(encoding='utf-8'))
    manifest = (local / catalog['variants']['ja']['manifest']).resolve()
    if not manifest.is_relative_to(local):
        raise ValueError('Japanese manifest escaped the local catalog')
    source = (manifest.parent / 'raw/pl00a.sht').resolve()
    if not source.is_relative_to(local) or source.stat().st_size != 6736:
        raise ValueError('unexpected actual Japanese shot resource')
    raw = source.read_bytes()
    if hashlib.sha256(raw).hexdigest() != SHOT_SHA256:
        raise ValueError('shot layout evidence requires pinned actual pl00a.sht')
    return source, raw


def decoded_shot(raw):
    tools = str(ROOT / 'tools')
    if tools not in sys.path:
        sys.path.insert(0, tools)
    from th128_assets.shot import decode_shot

    return decode_shot(raw)


class NativePlayer:
    def __init__(self, executable):
        import pefile

        self.executable = Path(executable).resolve()
        if not 0 < self.executable.stat().st_size <= 16 * 1024 * 1024:
            raise ValueError('original executable exceeds size budget')
        with self.executable.open('rb') as source:
            binary = source.read(16 * 1024 * 1024 + 1)
        if hashlib.sha256(binary).hexdigest() != EXE_SHA256:
            raise ValueError('player addresses require pinned Japanese EXE SHA-256')
        self.image = pefile.PE(data=binary)
        if self.image.OPTIONAL_HEADER.ImageBase != 0x400000 or self.image.FILE_HEADER.Machine != 0x14c:
            raise ValueError('unexpected original image architecture')
        self.code_evidence = {}

    def chunk(self, start, end):
        if not 0x401000 <= start < end <= 0x499c15 or end - start > 65536:
            raise ValueError('slice outside original text')
        code = bytes(self.image.get_data(start - 0x400000, end - start))
        if len(code) != end - start:
            raise ValueError('truncated original code slice')
        self.code_evidence[f'{start:#x}:{end:#x}'] = {'start': start, 'endExclusive': end,
                                                    'sha256': hashlib.sha256(code).hexdigest(), 'bytes': code.hex()}
        return start, code

    def constant(self, name, start, size):
        rva = start - 0x400000
        section = self.image.get_section_by_rva(rva)
        if section is None or not 0 < size <= 65536 or rva + size > section.VirtualAddress + section.Misc_VirtualSize:
            raise ValueError('constant outside an explicit PE section')
        offset = rva - section.VirtualAddress
        physical_size = min(size, max(0, section.SizeOfRawData - offset))
        physical = bytes(self.image.get_data(rva, physical_size)) if physical_size else b''
        if len(physical) != physical_size:
            raise ValueError('truncated physical PE constant')
        return Region(name, start, physical + bytes(size - physical_size), False)

    def ftol(self):
        return self.chunk(0x48fac0, 0x48fb6b)

    def direction_focus(self, holding, *, manager_present=True, manager_active=True, counter_f98=4, hold_count=0):
        data = [Region('holding', 0x4db210, struct.pack('<I', keys(holding)), False),
                Region('manager_pointer', 0x4b8948, struct.pack('<I', MANAGER if manager_present else 0), False),
                Region('manager_active', MANAGER + 0xc0, struct.pack('<I', bool(manager_active)), False),
                field('counter', 0xf98, checked_i32(counter_f98), writable=False),
                field('hold_count', 0x43914, checked_i32(hold_count), writable=False),
                field('direction', 0xf6c, -123), field('focused', 0x43a88, -123),
                field('cooldown', 0x438d4, -123)]
        result = emulate([self.chunk(0x43af80, 0x43b093)], 0x43af80, data,
                         registers={'esi': PLAYER}, stops=[0x43b093])
        memory = result['memory']
        return {'direction': struct.unpack('<i', memory['direction'])[0],
                'focused': bool(struct.unpack('<i', memory['focused'])[0]),
                'cooldown_438d4': struct.unpack('<i', memory['cooldown'])[0]}

    def speeds(self, speed_bytes):
        if not isinstance(speed_bytes, bytes) or len(speed_bytes) != 16:
            raise ValueError('expected four original float32 speed fields')
        if any(not math.isfinite(value) or not 0 <= value * 128 < 2**31 for value in struct.unpack('<4f', speed_bytes)):
            raise ValueError('unsupported speed conversion input')
        data = [Region('speeds', SHOT + 0x10, speed_bytes, False),
                self.constant('raw_scale', 0x4a65f8, 8),
                Region('sse2_ftol', 0x4dcd68, struct.pack('<I', 1), False),
                Region('raw_speeds', PLAYER + 0xee0, bytes(16))]
        result = emulate([self.chunk(0x43a669, 0x43a6af), self.ftol()], 0x43a669, data,
                         registers={'esi': PLAYER, 'edi': SHOT}, stops=[0x43a6af])
        return list(struct.unpack('<4i', result['memory']['raw_speeds']))

    def velocity(self, direction, focused):
        if type(direction) is not int or not 0 <= direction <= 8 or type(focused) is not bool:
            raise ValueError('unsupported movement direction/focus')
        row = bytes(self.image.get_data(0xa3850 + direction * 8, 8))
        horizontal, vertical = struct.unpack('<2i', row)
        data = [field('direction', 0xf6c, direction, writable=False),
                Region('speeds', PLAYER + 0xee0, struct.pack('<4i', *NATIVE_SPEEDS), False),
                field('movement_scale', 0x87b0, 1.0, 'f', False),
                Region('sse2_ftol', 0x4dcd68, struct.pack('<I', 1), False)]
        entry = 0x43b0eb if focused else 0x43b155
        result = emulate([self.chunk(0x43b0eb, 0x43b1b7), self.ftol()], entry, data,
                         registers={'esi': PLAYER, 'edi': horizontal, 'ebx': vertical}, stops=[0x43b1b7])
        return [i32(result['registers']['edi']), i32(result['registers']['eax'])]

    def position(self, horizontal, vertical, delta_x, delta_y):
        values = [checked_i32(value) for value in (horizontal, vertical, delta_x, delta_y)]
        data = [Region('player_low', PLAYER + 0xecc, bytes(0xb4)),
                Region('time_scale', 0x4b6f58, struct.pack('<f', 1.0), False),
                Region('animation_manager', 0x4d2e50, bytes(4), False),
                self.constant('pixel_scale', 0x4a65f0, 8),
                Region('sse2_ftol', 0x4dcd68, struct.pack('<I', 1), False)]
        initial = bytearray(data[0].data)
        struct.pack_into('<2i', initial, 0xc, *values[:2])
        struct.pack_into('<i', initial, 0xa0, 1)
        data[0] = Region('player_low', PLAYER + 0xecc, bytes(initial))
        result = emulate([self.chunk(0x43b230, 0x43b33f), self.ftol()], 0x43b230, data,
                         registers={'esi': PLAYER, 'edi': delta_x}, stops=[0x43b33f],
                         stack={0xc: struct.pack('<i', delta_x), 0x18: struct.pack('<i', delta_y)})
        memory = result['memory']['player_low']
        return {'raw': list(struct.unpack_from('<2i', memory, 0xc)),
                'pixels': list(struct.unpack_from('<2f', memory))}

    def shot_start(self, holding, pressed, *, current=-1):
        data = [Region('holding', 0x4db210, struct.pack('<I', keys(holding)), False),
                Region('pressed', 0x4db21c, struct.pack('<I', keys(pressed)), False),
                field('current', 0x43900, checked_i32(current), writable=False)]
        result = emulate([self.chunk(0x43f1c9, 0x43f243)], 0x43f1c9, data,
                         registers={'edi': PLAYER}, stops=[0x43f1f6, 0x43f241, 0x43f2e1])
        return {0x43f1f6: 'start', 0x43f241: 'active', 0x43f2e1: 'inactive'}[result['stop']]

    def shot_cycle(self, holding, pressed, current):
        if type(current) is not int or not 0 <= current <= 14:
            raise ValueError('supported active shot cycle counts are 0..14')
        timer = struct.pack('<iifII', current - 1, current, float(current), 0x4b6f58, 1)
        data = [Region('holding', 0x4db210, struct.pack('<I', keys(holding)), False),
                Region('pressed', 0x4db21c, struct.pack('<I', keys(pressed)), False),
                Region('timer', PLAYER + 0x438fc, timer),
                Region('time_scale', 0x4b6f58, struct.pack('<f', 1.0), False),
                self.constant('near_one_low', 0x4a6698, 8),
                self.constant('near_one_high', 0x4a6694, 4),
                self.constant('one', 0x4a6588, 8),
                self.constant('minus_one', 0x4a6688, 4),
                Region('sse2_ftol', 0x4dcd68, struct.pack('<I', 1), False)]
        result = emulate([self.chunk(0x43f264, 0x43f2e3), self.chunk(0x403280, 0x4032bd),
                          self.chunk(0x402ac0, 0x402b18), self.ftol()], 0x43f264, data,
                         registers={'edi': PLAYER, 'esi': PLAYER + 0x438fc}, stops=[0x43f2e3])
        previous, final, accumulator, scale, flags = struct.unpack('<iifII', result['memory']['timer'])
        return {'counts': [previous, final], 'accumulator': accumulator, 'flags': flags}

    def record_due(self, period, phase, count):
        if (type(period) is not int or not 1 <= period <= 127 or type(phase) is not int
                or not -128 <= phase <= 127):
            raise ValueError('unsupported signed-byte shot period/phase')
        data = [Region('record', SHOT, struct.pack('<bb', period, phase), False)]
        result = emulate([self.chunk(0x43f180, 0x43f19e)], 0x43f180, data,
                         registers={'esi': SHOT, 'eax': period, 'ebx': checked_i32(count)},
                         stops=[0x43f190, 0x43f19e])
        return result['stop'] == 0x43f190

    def relocate_shot(self, raw):
        if not isinstance(raw, bytes) or hashlib.sha256(raw).hexdigest() != SHOT_SHA256:
            raise ValueError('native relocation is restricted to the pinned actual shot file')
        decoded = decoded_shot(raw)
        addresses = (0x4b1cd8, 0x4b1a14, 0x4d2e2c, 0x4b1cf8)
        tables = [self.constant(f'callback_table_{index}', address, 4)
                  for index, address in enumerate(addresses)]
        callbacks = [struct.unpack('<I', table.data)[0] for table in tables]
        data = [Region('shot', SHOT, raw), field('shot_pointer', 0xf7c, SHOT, 'I', False), *tables]
        result = emulate([self.chunk(0x43c298, 0x43c322)], 0x43c298, data,
                         registers={'eax': SHOT, 'esi': PLAYER}, budget=20000)
        expected = bytearray(raw)
        pointers = []
        for index, group in enumerate(decoded['groups']):
            pointer = SHOT + group['offset']
            pointers.append(pointer)
            struct.pack_into('<I', expected, 0x268 + index * 8, pointer)
            for record in group['records']:
                if record['callbackIds'] != [0, 0, 0, 0]:
                    raise ValueError('unimplemented native callback-table indices')
                struct.pack_into('<4I', expected, record['offset'] + 0x24, *callbacks)
        if result['registers']['eax'] != 0 or result['memory']['shot'] != bytes(expected):
            raise ProbeError('original shot loader changed unexpected bytes')
        return {'groupCount': decoded['groupCount'], 'recordCount': decoded['recordCount'],
                'groupPointers': pointers, 'callbackTableFirstPointers': callbacks,
                'callbackTableState': 'PE initial bytes/declared zero-fill, not a live runtime snapshot',
                'unknownBytesUnchanged': True, 'instructionsExecuted': len(result['trace']),
                'relocatedSha256': hashlib.sha256(result['memory']['shot']).hexdigest(),
                'originalBytesPreservedByReader': decoded['raw'] == raw}


def build_report(executable):
    import capstone
    import importlib.metadata

    native = NativePlayer(executable)
    source, raw = load_actual_shot()
    speeds = native.speeds(raw[0x10:0x20])
    if tuple(speeds) != NATIVE_SPEEDS:
        raise ProbeError('actual shot speed fields differ from the supported C++ constants')
    relocation = native.relocate_shot(raw)
    cases = {name: [] for name in ('directionFocus', 'velocity', 'position', 'shotStart', 'shotCycle', 'recordSchedule')}

    def focus_case(holding, present=True, active=True, counter=4, held=0):
        cases['directionFocus'].append({
            'holding': holding,
            'context': {'managerPresent': present, 'managerActive': active,
                        'counterF98': counter, 'holdCount43914': held},
            'result': native.direction_focus(holding, manager_present=present,
                                             manager_active=active, counter_f98=counter, hold_count=held)})

    for nibble in range(16):
        for extra in (0, 1, 8, 512, 513, 0xfe0f):
            focus_case((nibble << 4) | extra)
    for counter in (-1, 3, 4, 5):
        for present in (False, True):
            for active in (False, True):
                for held in (11, 12):
                    for holding in (0, 8):
                        focus_case(holding, present, active, counter, held)
    for direction in range(9):
        for focused in (False, True):
            cases['velocity'].append({'direction': direction, 'focused': focused,
                                      'result': native.velocity(direction, focused)})
    for start in ((0, 51200), (-23552, 4096), (23552, 55296), (-23553, 4095),
                  (23553, 55297), (-2**31, 2**31 - 1), (2**31 - 1, -2**31)):
        for delta in ((0, 0), (576, 0), (-576, 0), (0, 576), (0, -576), (407, -407), (1, 1), (-1, -1)):
            cases['position'].append({'start': list(start), 'delta': list(delta),
                                      'result': native.position(*start, *delta)})
    for holding in (0, 1, 2, 8, 512, 513, 520, 65535):
        for pressed in (0, 1, 512, 513):
            for current in (-2, -1, 0, 14):
                cases['shotStart'].append({'holding': holding, 'pressed': pressed, 'current': current,
                                          'result': native.shot_start(holding, pressed, current=current)})
    for current in range(15):
        for holding in (0, 1, 512, 513):
            for pressed in (0, 1):
                cases['shotCycle'].append({'holding': holding, 'pressed': pressed, 'current': current,
                                          'result': native.shot_cycle(holding, pressed, current)})
    for period in (1, 3, 6, 15, 127):
        for phase in (-1, 0, 1):
            for count in (-2**31, -4, -1, 0, 1, 2, 3, 5, 6, 14, 15, 16, 2**31 - 1):
                cases['recordSchedule'].append({'period': period, 'phase': phase, 'count': count,
                                               'result': native.record_due(period, phase, count)})
    decoder = capstone.Cs(capstone.CS_ARCH_X86, capstone.CS_MODE_32)
    code = list(native.code_evidence.values())
    for chunk in code:
        chunk['disassembly'] = [f'{item.address:08x} {item.mnemonic} {item.op_str}'.rstrip()
                                for item in decoder.disasm(bytes.fromhex(chunk['bytes']), chunk['start'])]
    decoded = decoded_shot(raw)
    return {'schema': 'th128-player-native-fixtures/1', 'exeSha256': EXE_SHA256,
            'shotSha256': SHOT_SHA256, 'shotResource': str(source.relative_to(ROOT)),
            'emulator': importlib.metadata.version('unicorn'), 'windowsGameProcessLaunched': False,
            'unimplementedCallouts': 'rejected, never substituted', 'instructionBudgetPerSlice': 2048,
            'loaderInstructionBudget': 20000, 'timeScale': 1.0, 'movementScale': 1.0,
            'ftolSse2Flag': 1, 'nativeRawSpeeds': speeds, 'nativeCode': code,
            'nativeDirectionTable': list(struct.iter_unpack('<ii', bytes(native.image.get_data(0xa3850, 72)))),
            'shotLayout': {'headerBytes': 0x268, 'groupTableStride': 8, 'recordStride': 52,
                           'groupRecordCounts': [len(group['records']) for group in decoded['groups']],
                           'relocation': relocation},
            'cases': cases, 'caseCount': sum(len(group) for group in cases.values()),
            'unknown': ['Whole-frame scheduling, animation calls, and non-unit time/movement scales.',
                        'Freeze/bomb activation, collision, damage application, and rendering.',
                        'Shot group selection and callback behavior; retained IDs are not executed.',
                        'Raw gate field meanings beyond the observed comparisons.']}


def main(argv=None):
    parser = argparse.ArgumentParser(description='Bounded original-byte TH128 player slices; never launches the game.')
    parser.add_argument('--exe', required=True, type=Path)
    parser.add_argument('--report', type=Path, default=ROOT / 'local/research/player-native-fixtures.json')
    arguments = parser.parse_args(argv)
    try:
        destination = report_destination(arguments.report)
        report = build_report(arguments.exe)
        destination.write_text(json.dumps(report, ensure_ascii=False, indent=2, allow_nan=False) + '\n', encoding='utf-8')
        print(f'{report["caseCount"]} original-byte player cases; report: {destination}')
    except (ValueError, OSError, ImportError, KeyError) as error:
        print(f'player probe failed: {error}', file=sys.stderr)
        return 1
    return 0


if __name__ == '__main__':
    raise SystemExit(main())
