import argparse
import hashlib
import importlib.metadata
import json
import struct
import sys
from pathlib import Path

from enemy_probe import CODE as ENEMY_CODE
from enemy_probe import EXE_SHA256, RETURN_TRAP, STACK_POINTER, address, datum, emulate, snapshot


ROOT = Path(__file__).resolve().parents[2]
ENEMY_A = 0x300000
ENEMY_B = 0x310000
ENEMY_C = 0x320000
MANAGER = 0x380000
CONTROL = 0x390000
SPAWN = 0x3A0000
CALLBACK = 0x3B0000
SCHEDULER = 0x3C0000
STATE_OFFSET = 0x11C8
NODE_OFFSET = 0x145C
ENEMY_SIZE = 0x5178

SCHEDULE_CODE = {
    'spawn_entry': (0x414FF0, 0x415021),
    'prepared_spawn_to_update': (0x415021, 0x415185),
    'constructor_list_insertion': (0x4151FC, 0x415248),
    'manager_callback_registration': (0x41539F, 0x4153CF),
    'manager_iteration': (0x4158F0, 0x415954),
    'manager_callback_gate': (0x415A20, 0x415A59),
    'constructor_state_clear': (0x415AB8, 0x415B67),
    'constructor_node_initialization': (0x415C0E, 0x415C25),
    'update_to_ecl_boundary': (0x416160, 0x4164E6),
    'pre_ecl_bounds_branches': (0x416530, 0x41658A),
    'guard_return': (0x41734D, 0x417361),
    'callback_registration': (0x4681D0, 0x46826F),
    'tick_scheduler_to_callback': (0x468310, 0x46838F),
    'original_cookie_check': (0x4728FD, 0x472907),
    'literal_main_request': (0x426ACF, 0x426AEE),
}
STATIC_CODE = {
    'constructor_after_initial_update': (0x415185, 0x4151FC),
    'manager_zero_and_global': (0x4152E0, 0x415303),
    'manager_initializer': (0x415310, 0x415475),
    'manager_allocation_wrapper': (0x415840, 0x4158AE),
    'manager_callback_activation': (0x415630, 0x415653),
    'manager_node_unlink': (0x415270, 0x4152D1),
    'lower_constructor_entry': (0x415A70, 0x415AB8),
    'lower_constructor_remaining': (0x415B67, 0x415EB3),
    'runner_to_ecl_core': (0x46FE20, 0x46FE57),
    'literal_main_request_context': (0x426AAD, 0x426ACF),
    'second_literal_main_request': (0x426C50, 0x426C6F),
}
HELPERS = ('linear_evaluator', 'zero_first_pair', 'zero_second_pair', 'reset_pair_timer',
           'timer_step', 'original_memset', 'polar_velocity', 'position_integrator',
           'position_combine', 'original_fsincos', 'original_floor_sse2', 'normalize',
           'animation_lookup')


def report_destination(path):
    destination = Path(path).resolve()
    allowed = (ROOT / 'local' / 'research').resolve()
    if (allowed != ROOT.resolve() / 'local' / 'research' or destination.parent != allowed
            or destination.suffix != '.json' or not destination.name.startswith('enemy-scheduling')
            or destination.drive.lower() == 'e:'):
        raise ValueError('report must be local/research/enemy-scheduling*.json inside this deliverable')
    return destination


def probe(executable):
    binary = Path(executable).read_bytes()
    if hashlib.sha256(binary).hexdigest() != EXE_SHA256:
        raise ValueError('EXE SHA-256 does not match the pinned Japanese executable')

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

    helper_code = {name: ENEMY_CODE[name] for name in HELPERS}
    emulated_code = {**SCHEDULE_CODE, **helper_code}
    chunks = [(start, original(start, end - start)) for start, end in emulated_code.values()]
    decoder = capstone.Cs(capstone.CS_ARCH_X86, capstone.CS_MODE_32)
    decoder.detail = True
    catalog = {}
    direct_calls = {}
    for name, (start, end) in {**emulated_code, **STATIC_CODE}.items():
        payload = original(start, end - start)
        instructions = list(decoder.disasm(payload, start))
        decoded_bytes = sum(item.size for item in instructions)
        complete = decoded_bytes == len(payload)
        catalog[name] = {'start': address(start), 'endExclusive': address(end),
                         'sha256': hashlib.sha256(payload).hexdigest(),
                         'decodedBytes': decoded_bytes, 'undecodedBytes': len(payload) - decoded_bytes,
                         'completeDecoding': complete,
                         'evidence': 'static-decoding' if complete else 'partial-static-decoding',
                         'instructions': [{'address': address(item.address), 'mnemonic': item.mnemonic,
                                           'operands': item.op_str} for item in instructions]}
        for instruction in instructions:
            if (instruction.group(capstone.CS_GRP_CALL)
                    and instruction.operands[0].type == capstone.x86_const.X86_OP_IMM):
                direct_calls[instruction.address] = {'call': address(instruction.address),
                                                     'target': address(instruction.operands[0].imm)}

    constant_ranges = ((0x4A6578, 8), (0x4A6580, 8), (0x4A6588, 8), (0x4A6590, 8),
                       (0x4A65A0, 8), (0x4A65A8, 8), (0x4A65B8, 8), (0x4A65D8, 8),
                       (0x4A6600, 8), (0x4A6608, 8), (0x4A6688, 4), (0x4A6694, 4),
                       (0x4A6698, 8), (0x4A6710, 8), (0x4A682C, 4), (0x4A6C40, 8),
                       (0x4A6CA0, 16), (0x4A6CB0, 16), (0x4A6CC0, 16),
                       (0x4A6CD0, 16), (0x4A6CE0, 16), (0x4A2704, 28), (0x4A28B8, 5))
    environment = [datum('constant-' + address(location), location, original(location, size), writable=False)
                   for location, size in constant_ranges]
    environment.extend((datum('managerGlobal', 0x4B8948, MANAGER, 'I', False),
                        datum('difficulty', 0x4B4D0C, 0, 'I', False),
                        datum('cookie', 0x4B01F4, 0x13579BDF, 'I', False),
                        datum('timeScale', 0x4B6F58, 1.0, 'f', False),
                        datum('crtSse2Enabled', 0x4DBC20, 1, 'I', False),
                        datum('crtFastMemsetEnabled', 0x4DCD68, 0, 'I', False),
                        datum('animationManagerGlobal', 0x4D2E50, 0, 'I', False),
                        datum('schedulerGlobal', 0x4D2E14, SCHEDULER, 'I', False),
                        datum('schedulerThreadFlags', 0x4D33F8, 0, 'I', False)))

    def manager(head=0, tail=0, count=0):
        payload = bytearray(0xCC)
        for offset, value in ((0x24, 7), (0x28, 8), (0xB8, head), (0xBC, tail), (0xC0, count)):
            struct.pack_into('<I', payload, offset, value)
        return datum('manager', MANAGER, bytes(payload))

    def enemy(name, location, flags=0, successor=0, previous=0, interpolation=False, fill=0):
        payload = bytearray([fill]) * ENEMY_SIZE
        for offset, value in ((0, 0x4A2704), (0x50AC, flags), (STATE_OFFSET + 0x278, 0x4B6F58)):
            struct.pack_into('<I', payload, offset, value)
        if not fill:
            struct.pack_into('<III', payload, NODE_OFFSET, location, successor, previous)
            struct.pack_into('<fff', payload, STATE_OFFSET + 0x34, -128.0, -32.0, 0.0)
            struct.pack_into('<fff', payload, STATE_OFFSET + 0x68, -128.0, -32.0, 0.0)
        if interpolation:
            struct.pack_into('<8fiifIiii', payload, STATE_OFFSET + 0x338,
                             0.0, 0.0, 0.0, 2.0, 0.0, 0.0, 0.0, 0.0,
                             -1, 0, 0.0, 0x4B6F58, 1, 2, 0)
        return datum(name, location, bytes(payload))

    def scheduler(linked=False):
        payload = bytearray(0x50)
        if linked:
            struct.pack_into('<I', payload, 0x18, CALLBACK + 0x14)
        return datum('scheduler', SCHEDULER, bytes(payload))

    def callback(linked=False):
        payload = bytearray(0x24)
        struct.pack_into('<I', payload, 0x14, CALLBACK)
        if linked:
            for offset, value in ((0, 21), (4, 3), (8, 0x415A20), (0x1C, SCHEDULER + 0x14),
                                  (0x20, MANAGER)):
                struct.pack_into('<I', payload, offset, value)
        return datum('callback', CALLBACK, bytes(payload))

    def frame():
        return {0: struct.pack('<I', 0), 4: struct.pack('<I', 0), 8: struct.pack('<I', 0),
                12: struct.pack('<I', MANAGER), 16: struct.pack('<I', 0),
                20: struct.pack('<I', RETURN_TRAP), 24: struct.pack('<I', 0),
                28: struct.pack('<I', SPAWN)}

    records = []

    def run(identifier, entry, data, registers=None, stops=(), stack=None, meaning='', stack_capture=None):
        result = emulate(chunks, entry, [*environment, *data], registers=registers,
                         stops=stops, stack=stack, stack_capture=stack_capture, budget=16384)
        observations = {}
        for name, memory in result['memory'].items():
            if name == 'manager':
                fields = {'managerHead': 0xB8, 'managerTail': 0xBC, 'managerCount': 0xC0,
                          'managerCounter24': 0x24, 'managerCounter28': 0x28}
            elif name.startswith('enemy'):
                fields = {name + field: offset for field, offset in
                          (('Flags', 0x50AC), ('NodeOwner', NODE_OFFSET), ('Next', NODE_OFFSET + 4),
                           ('Prev', NODE_OFFSET + 8), ('AbsoluteX', STATE_OFFSET + 0x68),
                           ('AbsoluteSpeed', STATE_OFFSET + 0x80), ('CombinedX', STATE_OFFSET + 0x34),
                           ('InterpolationCurrent', STATE_OFFSET + 0x35C))}
            elif name == 'callback':
                fields = {'callbackPriority': 0, 'callbackFunction': 8, 'callbackContext': 0x20}
            elif name == 'scheduler':
                fields = {'schedulerHead': 0x18, 'schedulerCachedNext': 0x48}
            else:
                continue
            before, after = bytes.fromhex(memory['before']), bytes.fromhex(memory['after'])
            for field, offset in fields.items():
                observations[field] = snapshot(int(memory['address'], 16) + offset,
                                                before[offset:offset + 4], after[offset:offset + 4])
        records.append({'id': identifier, 'evidence': 'bounded-original-byte-emulation',
                        'meaning': meaning, 'observations': observations, **result})

    node_a, node_b, node_c = (location + NODE_OFFSET for location in (ENEMY_A, ENEMY_B, ENEMY_C))
    run('spawn-entry-allocation-boundary', 0x414FF0, [manager()],
        meaning='Actual spawn entry stops before the real allocator; no allocation succeeds in this fixture.')
    run('constructor-state-clear', 0x415AB8,
        [enemy('enemyA', ENEMY_A, 0xA5A5A5A5, fill=0xA5)],
        {'esi': ENEMY_A, 'ebx': 0, 'ebp': STACK_POINTER + 0x100},
        (0x415B67,), meaning='Partial lower constructor clears its embedded 0x3fb0-byte state with real memset.')
    run('constructor-node-init', 0x415C0E, [enemy('enemyA', ENEMY_A, fill=0xA5)],
        {'esi': ENEMY_A, 'ebx': 0}, (0x415C20,), meaning='Partial constructor initializes owner/next/prev from real stores.')
    spawn_data = bytearray(0x54)
    struct.pack_into('<fffiiii', spawn_data, 0, -128.0, -32.0, 0.0, 1000, 2, 20, 0)
    run('prepared-constructor-to-ecl', 0x415021,
        [manager(), enemy('enemyA', ENEMY_A), datum('spawnData', SPAWN, bytes(spawn_data), writable=False)],
        {'ebx': ENEMY_A, 'ebp': STACK_POINTER + 16}, (0x4164E1,), frame(),
        'Partial spawn entry assumes explicit preallocated/initialized storage; real initial update reaches the first ECL call while the manager list is empty. No allocator, lower-constructor completion or ECL execution is claimed.')
    run('insert-empty', 0x4151FC, [manager(), enemy('enemyA', ENEMY_A, 0x80000)],
        {'ebx': ENEMY_A, 'edi': 0, 'ebp': STACK_POINTER + 16}, stack=frame(),
        meaning='Actual constructor suffix with an explicit synthetic saved frame and preallocated enemy; no preceding update is implied.')
    run('insert-tail', 0x4151FC,
        [manager(node_a, node_a, 1), enemy('enemyA', ENEMY_A), enemy('enemyB', ENEMY_B, 0x80000)],
        {'ebx': ENEMY_B, 'edi': 0, 'ebp': STACK_POINTER + 16}, stack=frame(),
        meaning='Actual constructor suffix appends to a terminal old tail; this is not an allocation or a running-manager birth.')
    run('manager-two-guarded', 0x4158F0,
        [manager(node_a, node_b, 2), enemy('enemyA', ENEMY_A, 0x80020, node_b),
         enemy('enemyB', ENEMY_B, 0x80000, previous=node_a)], {'edi': MANAGER}, (0x415954,),
        meaning='Manager loop invokes real guarded updates, original cookie checks and real zero-return guard clears; stops before manager suffix.')
    run('manager-eligible-to-ecl', 0x4158F0,
        [manager(node_a, node_a, 1), enemy('enemyA', ENEMY_A, interpolation=True)],
        {'edi': MANAGER}, (0x4164E1,),
        meaning='Eligible manager path uses synthetic linear interpolation, time scale 1, zero ANM handle and no transform/position interpolation; stops before ECL executes.')
    for identifier, successor in (('cache-next-tail', 0), ('cache-next-nontail', node_b)):
        run(identifier, 0x4158F0,
            [manager(node_a, node_b if successor else node_a, 2 if successor else 1),
             enemy('enemyA', ENEMY_A, successor=successor), enemy('enemyB', ENEMY_B)],
            {'edi': MANAGER}, (0x415927,), meaning='Real manager prefix stops before update with next already copied to EBX.')
    run('cached-null-after-append', 0x41594D,
        [manager(node_a, node_b, 2), enemy('enemyA', ENEMY_A, successor=node_b),
         enemy('enemyB', ENEMY_B, 0x80000, previous=node_a)],
        {'edi': MANAGER, 'esi': node_a, 'ebx': 0}, (0x415953,),
        meaning='Partial manager continuation with explicitly synthetic stale null EBX and an already-appended node. No call result, allocation or interrupted/resumed birth is emulated.')
    run('cached-next-after-append', 0x41594D,
        [manager(node_a, node_c, 3), enemy('enemyA', ENEMY_A, successor=node_b),
         enemy('enemyB', ENEMY_B, 0x80000, node_c, node_a),
         enemy('enemyC', ENEMY_C, 0x80000, previous=node_b)],
        {'edi': MANAGER, 'esi': node_a, 'ebx': node_b}, (0x415953,),
        meaning='Partial continuation with synthetic cached old successor visits it and then its already-appended guarded successor. This is not a continuous spawn-during-ECL trace.')
    run('manager-removal-boundary', 0x4158F0,
        [manager(node_a, node_a, 1), enemy('enemyA', ENEMY_A, 0x4000000)], {'edi': MANAGER},
        meaning='Removal flag bypasses update and stops before the real indirect destructor call.')
    for flags in (None, 1, 2, 4, 0x400, 0):
        identifier = ('wrapper-missing-control' if flags is None else
                      'wrapper-active' if flags == 0 else f'wrapper-blocked-{flags:x}')
        control_data = bytearray(0x64)
        struct.pack_into('<I', control_data, 0x60, flags or 0)
        run(identifier, 0x415A20,
            [manager(node_a, node_a, 1), enemy('enemyA', ENEMY_A, 0x80000),
             datum('controlGlobal', 0x4B8954, 0 if flags is None else CONTROL, 'I', False),
             datum('control', CONTROL, bytes(control_data), writable=False)],
            {'ecx': MANAGER}, (0x415954,),
            meaning='Actual callback gate with a declared synthetic control pointer/word. Active case stops before manager suffix; blocked cases return normally.')
    run('register-manager-callback', 0x41539F, [manager(), scheduler(), callback()],
        {'esi': CALLBACK, 'edi': MANAGER},
        meaning='Partial manager initialization starts with an explicitly preallocated callback; original registration inserts priority 21 in the tick chain, then stops at the next real allocation.')
    run('scheduler-callback-boundary', 0x468310, [scheduler(True), callback(True)],
        {'ebx': SCHEDULER},
        meaning='Original tick scheduler prepares manager ECX and callback EDX; safe emulator intentionally stops before every indirect call, including this known callback.')
    run('literal-main-request-allocation-boundary', 0x426ACF, [manager()], {'edi': 0},
        stack={0x10: bytes([0xA5]) * 0x54}, stack_capture={'rootSpawnData': (0x10, 0x54)},
        meaning='Partial caller with explicit EDI=0 prepares the actual literal main request and enters real 0x414ff0, stopping before allocation. No manager-initialization reachability, root construction, initial ECL execution or browser frame placement is claimed.')

    return {'schema': 'th128-enemy-scheduling-probe/1', 'exeSha256': EXE_SHA256,
            'proof': 'Static x86 decoding and bounded original-byte emulation over explicit synthetic inputs; not a native game/frame trace.',
            'fullNativeScheduleProven': False, 'fullSpawnExecuted': False,
            'browserFrameCadenceProven': False,
            'rootCreation': {'insideManagerInitializerProven': False,
                             'initializerToRootOrderProven': False,
                             'managerInitializerEntry': '0x00415310',
                             'literalNameAddress': '0x004a28b8',
                             'name': original(0x4A28B8, 5).rstrip(b'\0').decode('ascii'),
                             'literalRequestCalls': ['0x00426ae9', '0x00426c6a'],
                             'meaning': 'No direct 0x414ff0 call occurs in the decoded manager initializer. Separate literal-main callers exist; their placement relative to successful manager initialization and callback invocation is not proved here. Indirect initialization callees and full stage-control reachability remain a gap.'},
            'versions': {name: importlib.metadata.version(name) for name in ('pefile', 'capstone', 'unicorn')},
            'statePolicy': 'Shared enemy_probe emulator: original RX code, explicit data spans, bounded stack, original cookie success path, x87 CW 0x037f/MXCSR 0x1f80, no OS/API/allocator stubs or invented return values. Every indirect call is unsupported.',
            'layout': {'managerGlobal': '0x004b8948', 'managerHeadOffset': '0xb8',
                       'managerTailOffset': '0xbc', 'managerCountOffset': '0xc0',
                       'stateOffset': '0x11c8', 'nodeOffset': '0x145c', 'flagsOffset': '0x50ac',
                       'nodeOwnerOffset': 0, 'nodeNextOffset': 4, 'nodePreviousOffset': 8,
                       'updateGuard': '0x00080000', 'removalFlag': '0x04000000',
                       'schedulerGlobal': '0x004d2e14', 'tickCallbackPriority': 21,
                       'controlGlobal': '0x004b8954', 'controlFlagsOffset': '0x60'},
            'code': catalog, 'directCalls': [edge for location, edge in sorted(direct_calls.items())],
            'caseCount': len(records), 'cases': records,
            'limitations': [
                'The actual allocator is never executed. Partial constructor fixtures assume preallocated storage and explicit frame/data inputs; they are not full native spawn traces.',
                'ECL calls are stop boundaries. No real script spawns a child during an emulated manager callback, and no fixture invents an allocator, Windows API or ECL return.',
                'Cached-next continuation fixtures begin at 0x41594d with explicitly synthetic registers; combining their implications with constructor insertion is static reasoning, not a continuous dynamic birth-frame trace.',
                'The manager suffix after 0x415954, scheduler callback dispatch, virtual removal, alternative movement modes/ANMs, exception/cookie failure paths and Windows/thread synchronization are not executed.',
                'No application-wide native frame numbering, callback reentry, freeze/deletion/collision effects or complete browser scheduling policy follows from these bounded fixtures alone.',
                'Callback priority 21 is a chain-sort key, not once-per-browser-frame cadence. Literal-main request callsites do not establish root creation during EnemyManager initialization or initialization-to-root execution order.'
            ]}


def main(argv=None):
    parser = argparse.ArgumentParser(description='Read-only bounded original-byte enemy scheduling evidence')
    parser.add_argument('--exe', type=Path, required=True)
    parser.add_argument('--libs', type=Path)
    parser.add_argument('--report', type=Path, default=ROOT / 'local/research/enemy-scheduling.json')
    options = parser.parse_args(argv)
    destination = report_destination(options.report)
    if options.libs:
        sys.path.insert(0, str(options.libs.resolve()))
    report = probe(options.exe)
    destination.parent.mkdir(parents=True, exist_ok=True)
    destination.write_text(json.dumps(report, indent=2, allow_nan=False) + '\n', encoding='utf-8')
    print(json.dumps({'report': str(destination), 'caseCount': report['caseCount'],
                      'stops': {case['id']: case['stop'] for case in report['cases']}}))
    return int(any(case['stop']['reason'] not in ('returned', 'boundary', 'unsupported_call')
                   for case in report['cases']))


if __name__ == '__main__':
    raise SystemExit(main())
