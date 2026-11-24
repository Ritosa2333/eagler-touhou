import argparse
import hashlib
import importlib.metadata
import json
import math
import struct
import sys
from pathlib import Path

from freeze_probe import STACK_BASE, STACK_SIZE, STACK_POINTER, RETURN_TRAP, address, datum


ROOT = Path(__file__).resolve().parents[2]
EXE_SHA256 = 'ba515750ece845b56640fe440592fc39ebb7bdd1ed87c541b67a19b148e767d4'
STATE = 0x300000
RUNNER = 0x310000
CONTEXT = 0x320000
INSTRUCTION = 0x330000
ANIMATION_MANAGER = 0x350000
ANIMATION_NODE = 0x360000
ANIMATION = 0x370000
ENEMY_MANAGER = 0x380000
ANIMATION_RESOURCE = 0x390000
OUTPUT = 0x3A0000
EXIT = 0x41C52C
EXPECTED_HANDLERS = {
    256: 0x41807D, 257: 0x418366, 258: 0x4188F9, 260: 0x418720,
    262: 0x418DA3, 304: 0x419467, 305: 0x41954A,
    400: 0x41A31F, 401: 0x41A345, 402: 0x41A36B, 403: 0x41A3A4,
}
CODE = {
    'spawn': (0x41807D, 0x4188F9),
    'bank': (0x4188F9, 0x41890D),
    'animation_slot': (0x418DA3, 0x418E63),
    'polar': (0x419467, 0x41976D),
    'bounds_flags': (0x41A31F, 0x41A3E2),
    'integer_argument': (0x41E0C0, 0x41E0D0),
    'float_argument': (0x41E110, 0x41E11E),
    'masked_integer_argument': (0x41E190, 0x41E1C1),
    'masked_float_argument': (0x41E1D0, 0x41E209),
    'integer_value': (0x46FED0, 0x46FF66),
    'float_value': (0x46FF70, 0x470026),
    'normalize': (0x46A700, 0x46A78E),
    'set_angle': (0x41F670, 0x41F692),
    'animation_lookup': (0x467420, 0x467479),
    'animation_show_hide': (0x467870, 0x4678F0),
    'animation_retire': (0x4679B0, 0x4679F6),
    'linear_evaluator': (0x41EB00, 0x41ED5F),
    'zero_first_pair': (0x41ED90, 0x41EDA3),
    'zero_second_pair': (0x41EDB0, 0x41EDC3),
    'reset_pair_timer': (0x41EDE0, 0x41EE14),
    'timer_step': (0x402AC0, 0x402B18),
    'original_memset': (0x47BB30, 0x47BBAA),
    'polar_velocity': (0x46AE00, 0x46AE37),
    'position_integrator': (0x46AE80, 0x46AF07),
    'position_combine': (0x416030, 0x41615C),
    'simple_motion_caller': (0x4162D3, 0x4163F5),
    'simple_motion_boundary': (0x4163F5, 0x4163F9),
    'original_fsincos': (0x46C1F0, 0x46C210),
    'original_floor_sse2': (0x48FDF0, 0x48FF11),
    'handler_exit_boundary': (EXIT, EXIT + 2),
}


def report_destination(path):
    destination = Path(path).resolve()
    allowed = (ROOT / 'local' / 'research').resolve()
    if (allowed != ROOT.resolve() / 'local' / 'research' or destination.parent != allowed
            or destination.suffix != '.json' or not destination.name.startswith('enemy-')
            or destination.drive.lower() == 'e:'):
        raise ValueError('report must be local/research/enemy-*.json inside this deliverable')
    return destination


def snapshot(location, before, after, writable=True):
    result = {'address': address(location), 'bytes': len(before), 'writable': writable,
              'before': before.hex(), 'after': after.hex()}
    if len(before) == 4:
        for label, payload in (('before', before), ('after', after)):
            result[label + 'I32'] = struct.unpack('<i', payload)[0]
            result[label + 'U32'] = struct.unpack('<I', payload)[0]
            floating = struct.unpack('<f', payload)[0]
            if math.isfinite(floating):
                result[label + 'F32'] = floating
    return result


def emulate(chunks, entry, data=(), registers=None, arguments=b'', stops=(), budget=4096,
            stack=None, stack_capture=None):
    import capstone
    import unicorn
    from capstone.x86_const import X86_OP_IMM
    from unicorn import x86_const

    if not 1 <= budget <= 200000 or len(arguments) > 1024 or len(chunks) > 64 or len(data) > 256:
        raise ValueError('invalid bounded probe inputs')
    decoder = capstone.Cs(capstone.CS_ARCH_X86, capstone.CS_MODE_32)
    decoder.detail = True
    instructions = {}
    spans = []
    for start, code in chunks:
        if not code or len(code) > 65536 or any(start < end and begin < start + len(code) for begin, end in spans):
            raise ValueError('invalid or overlapping code chunks')
        spans.append((start, start + len(code)))
        for instruction in decoder.disasm(code, start):
            instructions[instruction.address] = instruction
    if entry not in instructions:
        raise ValueError('entry is not an explicit instruction boundary')
    if len({region['name'] for region in data}) != len(data):
        raise ValueError('data names must be unique')
    for index, region in enumerate(data):
        start, size = region['address'], len(region['bytes'])
        if not 0 < size <= 65536:
            raise ValueError('invalid explicit data span')
        if any(start < previous['address'] + len(previous['bytes']) and previous['address'] < start + size
               for previous in data[:index]):
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
    captures = {}
    for name, (offset, size) in (stack_capture or {}).items():
        location = STACK_POINTER + offset
        if not 1 <= size <= 1024 or not STACK_BASE <= location <= STACK_BASE + STACK_SIZE - size:
            raise ValueError('invalid stack capture')
        captures[name] = (location, bytes(emulator.mem_read(location, size)))
    for page, category in pages.items():
        protection = unicorn.UC_PROT_READ | (unicorn.UC_PROT_EXEC if category in ('code', 'trap') else unicorn.UC_PROT_WRITE)
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
    emulator.reg_write(x86_const.UC_X86_REG_MXCSR, 0x1F80)
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
    final = {name: emulator.reg_read(identifier) for name, identifier in register_ids.items()}
    if not stop:
        stop.update(reason='returned' if final['eip'] == RETURN_TRAP else 'budget_exhausted', address=address(final['eip']))
    return {'entry': address(entry), 'inputRegisters': initial, 'arguments': arguments.hex(),
            'stackInitialization': {str(key): value.hex() for key, value in (stack or {}).items()},
            'instructionBudget': budget, 'stopBoundaries': [address(value) for value in stops],
            'instructionsExecuted': len(trace), 'instructionTrace': trace, 'stop': stop,
            'registers': final, 'stackDelta': final['esp'] - STACK_POINTER,
            'stackHead': bytes(emulator.mem_read(final['esp'], 64)).hex()
            if STACK_BASE <= final['esp'] <= STACK_BASE + STACK_SIZE - 64 else None,
            'memory': {region['name']: snapshot(region['address'], region['bytes'],
                       bytes(emulator.mem_read(region['address'], len(region['bytes']))), region['writable']) for region in data},
            'stackSnapshots': {name: snapshot(location, before, bytes(emulator.mem_read(location, len(before))))
                               for name, (location, before) in captures.items()},
            'mappedPages': {address(page): kind for page, kind in sorted(pages.items())}}


def probe(executable):
    binary = executable.read_bytes()
    if hashlib.sha256(binary).hexdigest() != EXE_SHA256:
        raise ValueError('EXE SHA-256 does not match the pinned Japanese executable')
    return recover(binary)


def recover(binary):
    import capstone
    import pefile

    image = pefile.PE(data=binary)
    if image.FILE_HEADER.Machine != 0x14C or image.OPTIONAL_HEADER.ImageBase != 0x400000:
        raise ValueError('pinned executable must be x86 at image base 0x400000')

    def original(location, size):
        payload = image.get_data(location - 0x400000, size)
        if len(payload) != size:
            raise ValueError('original byte range is not present in the PE file')
        return payload

    dispatch = {}
    for opcode, expected in EXPECTED_HANDLERS.items():
        index = original(0x41C7C0 + opcode - 256, 1)[0]
        handler = struct.unpack('<I', original(0x41C554 + index * 4, 4))[0]
        if handler != expected:
            raise ValueError(f'opcode {opcode} dispatch does not match the independently recovered address')
        dispatch[str(opcode)] = {'index': index, 'indexAddress': address(0x41C7C0 + opcode - 256),
                                 'tableAddress': address(0x41C554 + index * 4), 'handler': address(handler)}
    chunks = [(start, original(start, end - start)) for start, end in CODE.values()]
    decoder = capstone.Cs(capstone.CS_ARCH_X86, capstone.CS_MODE_32)
    catalog = {name: {'start': address(start), 'endExclusive': address(end),
                     'sha256': hashlib.sha256(original(start, end - start)).hexdigest(),
                     'instructions': [{'address': address(instruction.address), 'mnemonic': instruction.mnemonic,
                                       'operands': instruction.op_str}
                                      for instruction in decoder.disasm(original(start, end - start), start)]}
               for name, (start, end) in CODE.items()}
    static_contexts = {}
    for name, start, end in [('dispatcher', 0x418030, 0x41806D),
                              ('animation_postbirth', 0x418DF5, 0x418E63),
                              ('spawn_constructor_fields', 0x415021, 0x41507E),
                              ('linear_angle_store_caller', 0x4161F6, 0x416234),
                              ('enemy_manager_loop', 0x4158F0, 0x415954),
                              ('enemy_update_guard', 0x416160, 0x4161F6),
                              ('enemy_update_to_ecl', 0x4163F5, 0x416500),
                              ('runner_core_call', 0x46FE20, 0x46FE57),
                              ('core_host_call', 0x46FBD4, 0x46FBE5),
                              ('host_this_adjuster', 0x417FE0, 0x417FEB),
                              ('constructor_initial_update', 0x41517A, 0x415185)]:
        payload = original(start, end - start)
        static_contexts[name] = {'start': address(start), 'endExclusive': address(end),
                                 'sha256': hashlib.sha256(payload).hexdigest(), 'executed': False,
                                 'instructions': [{'address': address(item.address), 'mnemonic': item.mnemonic,
                                                   'operands': item.op_str} for item in decoder.disasm(payload, start)]}
    direct_calls = []
    for location in (0x415180, 0x415927, 0x416200, 0x416273, 0x416330, 0x4163A5, 0x4163B6,
                     0x4163EB, 0x4163F0, 0x4164E1, 0x46FE52):
        payload = original(location, 5)
        if payload[0] != 0xE8:
            raise ValueError('expected direct-call opcode in the static local-order evidence')
        target = location + 5 + struct.unpack_from('<i', payload, 1)[0]
        direct_calls.append({'call': address(location), 'target': address(target)})
    local_order = {'executed': False, 'fullFrameScheduleProven': False,
                   'meaning': 'Static per-enemy path order: polar interpolation, simple XY integration/combine, then ECL on paths reaching 0x4164e1. Not a dynamic manager/birth-frame trace.',
                   'directCalls': direct_calls, 'coreIndirectHostCall': '0x0046fbde',
                   'hostVtableFirstSlot': '0x004a2704',
                   'hostVtableTarget': address(struct.unpack('<I', original(0x4A2704, 4))[0]),
                   'stateThisAdjustment': '0x11c8', 'hostDispatcherFunction': '0x00417ff0',
                   'guards': ['manager skips state update for Enemy flag 0x04000000',
                              '0x416183 tests state flag 0x00080000; set branches to return path 0x41734d',
                              'manager clears Enemy flag 0x00080000 after a zero update result',
                              'ANM lookup, bounds/removal paths and callbacks surround the ECL call',
                              'constructor also calls state update at 0x415180 before returning']}
    constants = [datum('constant-' + address(location), location, original(location, size), writable=False)
                 for location, size in ((0x4A6578, 8), (0x4A6580, 8), (0x4A6588, 8),
                                        (0x4A65B8, 8), (0x4A6688, 4), (0x4A6694, 4),
                                        (0x4A6698, 8), (0x4A6710, 8), (0x4A6C40, 8), (0x4A6590, 8),
                                        (0x4A6CA0, 16), (0x4A6CB0, 16), (0x4A6CC0, 16),
                                        (0x4A6CD0, 16), (0x4A6CE0, 16))]
    sys.path.insert(0, str(ROOT / 'tools'))
    from th128_assets.formats import parse_ecl

    catalog_path = ROOT / 'local' / 'catalog.json'
    assets = json.loads(catalog_path.read_text(encoding='utf-8'))
    generation = (ROOT / 'local' / assets['variants']['ja']['manifest']).resolve().parent
    if not generation.is_relative_to((ROOT / 'local').resolve()):
        raise ValueError('asset manifest must remain inside local prepared assets')
    script_path = generation / 'raw' / 'st_a1_1.ecl'
    script_bytes = script_path.read_bytes()
    script = parse_ecl(script_bytes)
    subroutines = {subroutine['name']: subroutine for subroutine in script['subroutines']}

    def actual(subroutine, opcode):
        owner = subroutines[subroutine]
        command = next(command for command in owner['commands'] if command['opcode'] == opcode)
        start = owner['offset'] + 16 + command['offset']
        return script_bytes[start:start + 16 + len(bytes.fromhex(command['raw']))]

    def instruction(opcode, payload, count):
        return struct.pack('<iHHHBBI', 0, opcode, 16 + len(payload), 0, 255, count, 0) + payload

    def base(instruction_bytes):
        return [*constants, datum('stateRunner', STATE + 0x3F7C, RUNNER, 'I', False),
                datum('runnerContext', RUNNER + 4, CONTEXT, 'I', False),
                datum('contextInstruction', CONTEXT + 4, INSTRUCTION, 'I', False),
                datum('instruction', INSTRUCTION, instruction_bytes, writable=False)]

    def state_word(name, offset, value, kind='I'):
        return datum(name, STATE + offset, value, kind)

    records = []

    def run(identifier, opcode, instruction_bytes, state, inputs, operation, stack_capture=None):
        result = emulate(chunks, EXPECTED_HANDLERS[opcode], base(instruction_bytes) + state,
                         registers={'eax': opcode, 'ebx': STATE, 'edx': RUNNER, 'edi': INSTRUCTION},
                         stops=(EXIT,), stack={0x18: struct.pack('<I', INSTRUCTION)},
                         stack_capture=stack_capture)
        records.append({'id': identifier, 'opcode': opcode, 'operation': operation, 'input': inputs, **result})
        return records[-1]

    for opcode, flags, mask, live in [(402, 0, 32, True), (402, 0, 64, False), (402, 64, 2, False),
                                      (402, 32, 0, True), (402, 0x80000000, 2, False),
                                      (402, 0, 0xFFFFFFFF, False), (403, 0x62, 32, True),
                                      (403, 0x62, 64, True), (403, 0, 0, True), (403, 0xFFFFFFFF, 0xFFFFFFFF, False)]:
        identifier = 'set-root-32-visible-animation' if (opcode, flags, mask, live) == (402, 0, 32, True) else f'flags-{opcode}-{flags:x}-{mask:x}'
        state = [state_word('flags', 0x3EE4, flags), datum('animationManagerPointer', 0x4D2E50, ANIMATION_MANAGER, 'I', False)]
        for slot in range(16):
            state.append(state_word(f'animationSlot{slot}', 0xE0 + slot * 4, 0x1234 if live and slot == 0 else 0))
        if live:
            state.extend([datum('animationListHead', ANIMATION_MANAGER + 0x8B9704, ANIMATION_NODE, 'I', False),
                          datum('animationListNode', ANIMATION_NODE, struct.pack('<II', ANIMATION, 0), writable=False),
                          datum('animationId', ANIMATION, 0x1234, 'I', False),
                          datum('animationChildPolicy', ANIMATION + 0x18, 1, 'I', False),
                          datum('animationFlags', ANIMATION + 0x4B0, 2 if opcode == 402 else 0, 'I')])
        run(identifier, opcode, actual('main', 402) if identifier.startswith('set-root') else instruction(opcode, struct.pack('<I', mask), 1),
            state, {'flags': flags, 'mask': mask}, 'flags')

    for opcode, width, height in [(400, 24.0, 24.0), (401, 16.0, 16.0), (400, -1.0, 0.0), (401, 0.125, 99.5)]:
        offset = 0xD0 if opcode == 400 else 0xD8
        run(f'bounds-{opcode}-{width}-{height}', opcode, instruction(opcode, struct.pack('<ff', width, height), 2),
            [state_word('width', offset, 17.0, 'f'), state_word('height', offset + 4, 19.0, 'f')],
            {'width': width, 'height': height}, 'bounds')

    for bank in (0, 1, -1, 7):
        run(f'animation-bank-{bank}', 258, instruction(258, struct.pack('<i', bank), 1),
            [state_word('bank', 0x220, 17, 'i')], {'bank': bank}, 'bank')
    for slot, selected_script, bank, layer, flags in [(0, 0, 1, 0, 0), (1, 45, 1, -2, 32), (15, 9, 0, 3, 0)]:
        result = run(f'animation-request-{slot}-{selected_script}', 262,
                     instruction(262, struct.pack('<ii', slot, selected_script), 2),
                     [state_word('bank', 0x220, bank, 'i'), state_word('layerOffset', 0x238, layer, 'i'),
                      state_word('slotHandle', 0xE0 + slot * 4, 0),
                      datum('animationManagerPointer', 0x4D2E50, ANIMATION_MANAGER, 'I', False),
                      datum('enemyManagerPointer', 0x4B8948, ENEMY_MANAGER, 'I', False),
                      datum('bankResource', ENEMY_MANAGER + 0x80 + bank * 4, ANIMATION_RESOURCE, 'I', False)],
                     {'slot': slot, 'script': selected_script, 'bank': bank, 'layerOffset': layer, 'flags': flags},
                     'animation-request')
        result['animationArguments'] = {'resourcePointer': address(result['registers']['ecx']),
                                        'layer': result['registers']['eax'],
                                        'script': struct.unpack_from('<i', bytes.fromhex(result['stackHead']), 4)[0]}

    polar_vectors = [(1.75, 3.0, 0.0, 2.0, 0), (1.75, 3.0, -999999.0, -999999.0, 0),
                     (1.75, 3.0, -1000000.0, -2.0, 0), (1.75, 3.0, 0.0, 3.0, 0x100000),
                     (1.75, 3.0, 3.1415927410125732, 1.0, 0),
                     (1.75, 3.0, -3.1415927410125732, 1.0, 0),
                     (1.75, 3.0, 7.0, 0.25, 0), (1.75, 3.0, -7.0, 0.25, 0x100000),
                     (1.75, 3.0, 200.0, 4.0, 0), (1.75, 3.0, -200.0, 4.0, 0)]
    for index, (old_angle, old_speed, angle, speed, flags) in enumerate(polar_vectors):
        run(f'polar-{index}', 304, instruction(304, struct.pack('<ff', angle, speed), 2),
            [state_word('flags', 0x3EE4, flags), state_word('angle', 0x84, old_angle, 'f'),
             state_word('speed', 0x80, old_speed, 'f'), state_word('polarModeFlags', 0x98, 0xFFFFFFFF)],
            {'oldAngle': old_angle, 'oldSpeed': old_speed, 'angle': angle, 'speed': speed, 'flags': flags,
             'modeFlags': 0xFFFFFFFF}, 'polar')
    run('polar-first-stack-argument', 304, actual('GirlA1b', 304),
        [state_word('flags', 0x3EE4, 0), state_word('angle', 0x84, 0.0, 'f'),
         state_word('speed', 0x80, 0.0, 'f'), state_word('polarModeFlags', 0x98, 0xFFFFFFFF),
         datum('contextStackType', CONTEXT + 8, 0x66, 'I', False),
         datum('contextStackValue', CONTEXT + 12, 1.75, 'f', False),
         datum('contextStackOffset', CONTEXT + 0x1008, 8, 'I')],
        {'oldAngle': 0.0, 'oldSpeed': 0.0, 'angle': 1.75, 'speed': 3.0, 'flags': 0,
         'modeFlags': 0xFFFFFFFF, 'expressionValueSource': 'synthetic typed float expression result; not a guessed RNG return'}, 'polar')

    interpolation_names = [(0, 'interpolationStartAngle', 'f'), (4, 'interpolationStartSpeed', 'f'),
                           (8, 'interpolationTargetAngle', 'f'), (12, 'interpolationTargetSpeed', 'f'),
                           (16, 'firstZeroX', 'f'), (20, 'firstZeroY', 'f'), (24, 'secondZeroX', 'f'), (28, 'secondZeroY', 'f'),
                           (32, 'timerPrevious', 'i'), (36, 'timerCurrent', 'i'), (40, 'timerFloat', 'f'),
                           (44, 'timerScalePointer', 'I'), (48, 'timerFlags', 'I'),
                           (52, 'interpolationDuration', 'i'), (56, 'interpolationMode', 'i')]

    def interpolation_state(start_angle=1.75, start_speed=3.0, target_angle=0.3926991, target_speed=1.0,
                            duration=100, elapsed=0):
        values = [start_angle, start_speed, target_angle, target_speed, 0.0, 0.0, 0.0, 0.0,
                  elapsed - 1, elapsed, float(elapsed), 0x4B6F58, 1, duration, 0]
        return [state_word(name, 0x338 + offset, value, kind)
                for (offset, name, kind), value in zip(interpolation_names, values)]

    interpolation_vectors = [(100, 0, 0.3926991, 1.0, 0, 1.75), (100, 0, -999999.0, -999999.0, 0, 1.75),
                             (100, 0, -3.0, 1.0, 0, 3.0), (100, 0, 3.0, 1.0, 0, -3.0),
                             (100, 0, 0.0, 1.0, 0x100000, 1.75),
                             (0, 99, 0.0, 1.0, 0, 1.75), (-1, 99, 0.0, 1.0, 0, 1.75)]
    for index, (duration, mode, angle, speed, flags, old_angle) in enumerate(interpolation_vectors):
        run('polar-interpolate-first' if index == 0 else f'polar-interpolate-{index}', 305,
            actual('GirlA1b', 305) if index == 0 else instruction(305, struct.pack('<iiff', duration, mode, angle, speed), 4),
            [state_word('flags', 0x3EE4, flags), state_word('angle', 0x84, old_angle, 'f'),
             state_word('speed', 0x80, 3.0, 'f'), state_word('polarModeFlags', 0x98, 0xFFFFFFFF),
             *interpolation_state(duration=19), datum('zeroPair', 0x4D2E60, struct.pack('<ff', 0, 0), writable=False)],
            {'oldAngle': old_angle, 'oldSpeed': 3.0, 'duration': duration, 'mode': mode,
             'angle': angle, 'speed': speed, 'flags': flags, 'modeFlags': 0xFFFFFFFF}, 'polar-interpolate')

    for elapsed in (0, 1, 49, 98, 99):
        state = [*constants, *interpolation_state(elapsed=elapsed),
                 datum('timeScale', 0x4B6F58, 1.0, 'f', False),
                 datum('outputAngle', OUTPUT, 0.0, 'f'), datum('outputSpeed', OUTPUT + 4, 0.0, 'f')]
        result = emulate(chunks, 0x41EB00, state, registers={'edi': STATE + 0x338, 'ebx': OUTPUT})
        records.append({'id': f'linear-step-{elapsed + 1}-of-100', 'operation': 'linear-step',
                        'input': {'duration': 100, 'elapsed': elapsed, 'startAngle': 1.75, 'startSpeed': 3.0,
                                  'targetAngle': 0.3926991, 'targetSpeed': 1.0}, **result})

    first_spawn = actual('MainSub00', 257)
    inherited = list(range(0x11223000, 0x1122300C))
    for opcode, source, identifier in [(256, first_spawn, 'spawn-256-relative'),
                                        (257, first_spawn, 'actual-main-spawn-257'),
                                        (260, first_spawn, 'spawn-260-relative'),
                                        (260, actual('MainSub00', 260), 'actual-main-spawn-260')]:
        encoded = bytearray(source)
        struct.pack_into('<H', encoded, 4, opcode)
        numeric_offset = 20 + struct.unpack_from('<I', encoded, 16)[0]
        position_x, position_y, life, score, drop = struct.unpack_from('<ffiii', encoded, numeric_offset)
        result = run(identifier, opcode, bytes(encoded),
                     [state_word('flags', 0x3EE4, 0), state_word('parentX', 0x34, 100.0, 'f'),
                      state_word('parentY', 0x38, 200.0, 'f'),
                      state_word('inheritedWords', 0x23C, struct.pack('<12I', *inherited)),
                      datum('runnerContextTag', RUNNER + 0x11C0, 0xAABBCCDD, 'I', False)],
                     {'parentX': 100.0, 'parentY': 200.0, 'flags': 0, 'x': position_x, 'y': position_y,
                      'life': life, 'score': score, 'drop': drop, 'inheritedWords': inherited, 'contextTag': 0xAABBCCDD},
                     'spawn', stack_capture={'spawnData': (0x3F4, 0x54)})
        values = struct.unpack('<fffiiiII12II', bytes.fromhex(result['stackSnapshots']['spawnData']['after']))
        result['spawnArguments'] = {'name': bytes(encoded[20:]).split(b'\0', 1)[0].decode('ascii'),
                                    'position': list(values[:3]), 'score': values[3], 'drop': values[4],
                                    'life': values[5], 'mode': values[6], 'reserved1c': values[7],
                                    'inheritedWords': list(values[8:20]), 'contextTag': values[20]}

    motion_environment = [*constants, datum('timeScale', 0x4B6F58, 1.0, 'f', False),
                          datum('crtSse2Enabled', 0x4DBC20, 1, 'I', False)]

    def motion_component(prefix, offset, position, angle=0.0, speed=0.0):
        fields = [('X', 0, position[0], 'f'), ('Y', 4, position[1], 'f'), ('Z', 8, position[2], 'f'),
                  ('VelocityX', 12, 0.0, 'f'), ('VelocityY', 16, 0.0, 'f'), ('VelocityZ', 20, 0.0, 'f'),
                  ('Speed', 24, speed, 'f'), ('Angle', 28, angle, 'f'), ('ModeFlags', 48, 0, 'I')]
        return [state_word(prefix + name, offset + displacement, value, kind)
                for name, displacement, value, kind in fields]

    result = emulate(chunks, 0x46AE80,
                     [*motion_environment, *motion_component('position', 0x68, (-0.001, 0.001, 0))],
                     registers={'eax': STATE + 0x68})
    records.append({'id': 'cartesian-negative-floor', 'operation': 'cartesian',
                    'input': {'position': [-0.001, 0.001, 0], 'angle': 0.0, 'speed': 0.0}, **result})

    motion_vectors = [(angle, speed, (0.0, 0.0, 0.0), 0.0, 0.0)
                      for angle, speed in ((0.0, 3.0), (1.75, 3.0), (0.3926991, 1.0),
                                           (1.5707963705062866, 3.0), (3.1415927410125732, 3.0),
                                           (-3.1415927410125732, 3.0), (-1.75, 3.0),
                                           (0.7853981852531433, 3.0), (0.0, -2.0),
                                           (0.0, 0.0), (1.75, 0.0001), (2.8, 1.25))]
    motion_vectors.extend([(1.75, 3.0, (0.125, -0.125, 0), -1.0, 1.5),
                           (0.3926991, 1.0, (-0.001, 0.001, 0), 0.0, 0.0)])
    for index, (angle, speed, relative, relative_angle, relative_speed) in enumerate(motion_vectors):
        absolute = (-128.0, -32.0, 0.0)
        combined = (absolute[0] + relative[0], absolute[1] + relative[1], 0.0)
        state = [*motion_environment, *motion_component('absolute', 0x68, absolute, angle, speed),
                 *motion_component('relative', 0x9C, relative, relative_angle, relative_speed),
                 *motion_component('combined', 0x34, combined), state_word('flags', 0x3EE4, 0),
                 state_word('absolutePositionInterpolationDuration', 0x2E4, 0),
                 state_word('relativePositionInterpolationDuration', 0x330, 0)]
        result = emulate(chunks, 0x4162D3, state, registers={'esi': STATE},
                         stack={0x20: struct.pack('<I', STATE)}, stops=(0x4163F5,))
        records.append({'id': f'motion-frame-{index}', 'operation': 'motion-frame',
                        'input': {'absolute': list(absolute), 'relative': list(relative), 'combined': list(combined),
                                  'angle': angle, 'speed': speed, 'relativeAngle': relative_angle,
                                  'relativeSpeed': relative_speed, 'flags': 0}, **result})

    return {'schema': 'th128-enemy-probe/1', 'exeSha256': EXE_SHA256,
            'proof': 'bounded original x86 instruction emulation; not a Windows game run or native CPU proof',
            'versions': {name: importlib.metadata.version(name) for name in ('pefile', 'capstone', 'unicorn')},
            'dispatchEntry': '0x00418030', 'dispatchIndexTable': '0x0041c7c0', 'dispatchHandlerTable': '0x0041c554',
            'dispatchOpcodeBase': 256, 'dispatchMaximumIndex': 359, 'dispatch': dispatch,
            'stateBase': 'Enemy + 0x11c8; handler EBX is the embedded data base, not the full Enemy object',
            'script': {'path': str(script_path.relative_to(ROOT)), 'sha256': hashlib.sha256(script_bytes).hexdigest(),
                       'actualSpawnOwner': 'MainSub00', 'actualSpawnReach': 'main calls MainSub00 after ins_83(100)'},
            'statePolicy': 'Only explicit data spans and a bounded stack exist; zero register/stack initialization is synthetic. No OS/API/allocation stubs or fabricated callout returns.',
            'fpuControlWord': '0x037f', 'mxcsr': '0x1f80',
            'motionQuantizationFactor': struct.unpack('<d', original(0x4A6590, 8))[0],
            'code': catalog, 'staticContexts': static_contexts, 'localCallOrder': local_order,
            'caseCount': len(records), 'cases': records,
            'limitations': [
                'Handler exit is an explicit boundary, not proof of the surrounding dispatcher or world scheduler.',
                'Animation 262 stops before original birth helper 0x41f2a0; creation, script execution, metrics and post-birth bookkeeping are not emulated.',
                'Spawn 256/257/260 stops before original allocating constructor 0x414ff0; only prepared arguments are proved.',
                'Opcode 256 parent flag 0x08000000 transform path is not supported by the portable request helper.',
                '305 portable interpolation supports only positive-duration mode 0 and the native nonpositive-duration early stop; native evaluator is sampled only at time scale 1.',
                'Linear evaluator output is prior to the movement caller normalizing the angle twice.',
                'Simple motion probes explicitly select time scale 1, CRT SSE2 floor path, no position interpolation, no circular/ellipse mode, no clamp and no flag 0x08000000 transform. Frame scheduler and other motion paths are unverified.',
                'Finite-number, slot/name bounds and unsupported-mode rejection in the C++ API are host guards, not native clamps.',
                'Private derived evidence contains original script bytes; do not publish it as redistributable source.',
            ]}


def main(argv=None):
    parser = argparse.ArgumentParser(description='Bounded read-only TH128 enemy-rule instruction emulation; never launches the game.')
    parser.add_argument('--exe', type=Path, required=True)
    parser.add_argument('--libs', type=Path)
    parser.add_argument('--report', type=Path, default=ROOT / 'local/research/enemy-fixtures.json')
    arguments = parser.parse_args(argv)
    try:
        destination = report_destination(arguments.report)
        if arguments.libs:
            sys.path.insert(0, str(arguments.libs.resolve()))
        result = probe(arguments.exe)
        destination.parent.mkdir(parents=True, exist_ok=True)
        destination.write_text(json.dumps(result, indent=2, ensure_ascii=False, allow_nan=False) + '\n', encoding='utf-8')
        print(f'{result["caseCount"]} bounded original-byte enemy cases; original EXE unchanged; no game process or APIs.')
        print(f'Report: {destination}')
    except (OSError, ValueError, RuntimeError, ImportError) as error:
        print(f'Enemy probe failed: {error}', file=sys.stderr)
        return 1
    return 0


if __name__ == '__main__':
    raise SystemExit(main())
