import argparse
import json
import struct
from pathlib import Path

from unicorn.x86_const import UC_X86_REG_EAX, UC_X86_REG_EIP, UC_X86_REG_ESP

from .machine import RETURN, Machine

PLAYER = 0x004c22c4
TRANCE_FLAGS = 0x004be830
DIALOG_MANAGER = 0x004c2190
PLAYER_X = 0x5b8
PLAYER_Y = 0x5bc
HIT_SIZES = 0x660
PLAYER_STATE = 0x65c
HIT_TIMER = 0x14688
PLAYER_FLAGS = 0x14698
HIT_HANDLER = 0x444ca0
CIRCLE_COLLISION = 0x444140
SIZE_TABLES = {'4c0ba8': (0x4c0ba8, 4), '4c0bb8': (0x4c0bb8, 4), '4c0bc8': (0x4c0bc8, 4), '4c0bd8': (0x4c0bd8, 4)}
ANM_FLOAT_READ = 0x461e80
ANM_INT_READ = 0x4621b0
ANM_FLOAT_WRITE = 0x4622c0
ANM_INT_WRITE = 0x462380
ANM_VISUAL_RNG = 0x4dc31c
ANM_GAME_RNG = 0x4dc324
ANM_CAMERA = {'translation': 0x4dcaf4, 'reference': 0x4dcb18, 'origin': 0x4dcb30, 'extra': 0x4dcb3c}
INTERP_FUNCTION = 0x4120f0
ANGLE_INTERP_FUNCTION = 0x4662e0
INTERP_RATE = 0x4c0a28
GVARS_INT_READ = 0x420380
GVARS_FLOAT_READ = 0x420d00
PRACTICE_SCORE_POINTER = 0x4c22cc
PRACTICE_DIALOG_POINTER = 0x4c2194
ANM_LOOKUP = 0x46ff10
SAVE_IMAGE_FUNCTION = 0x449e50
SAVE_MALLOC_SITE = 0x47a0ed
SAVE_COMPRESS_SITE = 0x449fc6
SAVE_IMAGE_POINTER = 0x4c22cc
SAVE_MALLOC_RESULT = 0x30000000


def bits(value):
    return struct.unpack('<I', struct.pack('<f', value))[0]


def f32_bits(machine, address):
    return struct.unpack('<I', machine.read(address, 4))[0]


def collision_probe(machine):
    player = machine.alloc(0x15000)
    sizes = machine.alloc(8)
    bullet = machine.alloc(8)
    machine.write(PLAYER, struct.pack('<I', player))
    machine.write(player + HIT_SIZES, struct.pack('<I', sizes))
    machine.write(HIT_HANDLER, b'\xc3')

    def run(bx, by, size, radius, state=0, invulnerable=0, flags=0, trance=0, dialog=0):
        machine.write(sizes, struct.pack('<ff', 0.0, size))
        machine.write(player + PLAYER_X, struct.pack('<ff', 0.0, 0.0))
        machine.write(player + PLAYER_STATE, struct.pack('<i', state))
        machine.write(player + HIT_TIMER, struct.pack('<i', invulnerable))
        machine.write(player + PLAYER_FLAGS, struct.pack('<I', flags))
        machine.write(TRANCE_FLAGS, struct.pack('<I', trance))
        machine.write(DIALOG_MANAGER, struct.pack('<I', dialog))
        machine.write(bullet, struct.pack('<ff', bx, by))
        return machine.call(CIRCLE_COLLISION, [bits(radius)], eax=bullet) & 0xffffffff

    gate = []
    for state in range(6):
        for invulnerable in (-1, 0, 1):
            gate.append({'state': state, 'invulnerable': invulnerable,
                         'result': run(0.0, 0.0, 2.0, 1.0, state=state, invulnerable=invulnerable)})
    extras = {'trance': run(0.0, 0.0, 2.0, 1.0, trance=0x100), 'flag2': run(0.0, 0.0, 2.0, 1.0, flags=2),
              'flag1': run(0.0, 0.0, 2.0, 1.0, flags=1), 'dialog': run(0.0, 0.0, 2.0, 1.0, dialog=0x20000000)}
    machine.write(TRANCE_FLAGS, struct.pack('<I', 0))
    machine.write(DIALOG_MANAGER, struct.pack('<I', 0))
    sweeps = []
    for size, radius in ((2.0, 1.0), (3.0, 2.0), (3.0, 0.5)):
        rows = []
        for distance in (0.0, 0.5 * radius, 0.99 * (size ** 2 + radius ** 2) ** 0.5, (size ** 2 + radius ** 2) ** 0.5,
                         1.01 * (size ** 2 + radius ** 2) ** 0.5, 41.0, 42.0, 42.011, 42.012, 43.0, 100.0):
            rows.append([distance, run(float(distance), 0.0, size, radius)])
        sweeps.append({'size': size, 'radius': radius, 'rows': rows})
    return {'gate': gate, 'extras': extras, 'sweeps': sweeps}


def anm_state(machine):
    return list(struct.unpack('<HxxI', machine.read(ANM_VISUAL_RNG, 8)) +
                struct.unpack('<HxxI', machine.read(ANM_GAME_RNG, 8)))


def anm_setup(machine, vm, parent, rotation=0.25, scale=1.75, angle=2.5, bound=1000):
    machine.write(vm + 0x4ac, struct.pack('<4i', 11, -22, 33, 44))
    machine.write(vm + 0x4bc, struct.pack('<4f', 1.5, -2.25, 3.125, 4.5))
    machine.write(vm + 0x4cc, struct.pack('<2i', 55, 66))
    machine.write(vm + 0x3c, struct.pack('<3f', 7.5, 8.5, 9.5))
    machine.write(vm + 0x48, struct.pack('<3f', rotation, 0.5, 0.75))
    machine.write(vm + 0x4d4, struct.pack('<f', scale))
    machine.write(vm + 0x4d8, struct.pack('<f', angle))
    machine.write(vm + 0x4dc, struct.pack('<I', bound))
    machine.write(parent + 0x50, struct.pack('<f', 1.25))
    machine.write(vm + 0x570, struct.pack('<I', parent))
    machine.write(ANM_VISUAL_RNG, struct.pack('<HxxI', 0x1234, 0))
    machine.write(ANM_GAME_RNG, struct.pack('<HxxI', 0x5678, 0))
    machine.write(ANM_CAMERA['translation'], struct.pack('<3f', 10.0, 20.0, 1000.0))
    machine.write(ANM_CAMERA['reference'], struct.pack('<3f', 0.1, 0.2, 0.3))
    machine.write(ANM_CAMERA['origin'], struct.pack('<3f', 100.0, 200.0, 300.0))
    machine.write(ANM_CAMERA['extra'], struct.pack('<3f', -1.0, -2.0, -3.0))


def anm_call_float(machine, function, vm, argument, out, stub):
    code = b'\x68' + struct.pack('<I', argument)
    call_at = len(code)
    code += b'\xe8' + b'\0\0\0\0' + b'\x83\xc4\x04' + b'\xd9\x1d' + struct.pack('<I', out)
    code += b'\x68' + struct.pack('<I', RETURN) + b'\xc3'
    relative = function - (stub + call_at + 5)
    code = code[:call_at + 1] + struct.pack('<i', relative) + code[call_at + 5:]
    machine.write(stub, code)
    machine.start(stub, RETURN, eax=vm, ecx=0, edx=0, ebx=0, esi=0, edi=0)
    return machine.f32(out)


def stage_gvars_probe(exe):
    identifiers = list(range(-10000, -9906))
    stack = 0x20080000
    document = {'schema': 'th13/oracle-probes/v1', 'setup': {
        'enemy': {'position': [11.0, -22.0, 33.0], 'motion_delta': [0.25, -0.5],
                  'vel_pos': [1.5, -2.25, 0.0], 'vel_speed': 1234.0, 'vel_angle': -7.25,
                  'vel_radius': 9.5, 'pos_pos': [7.5, 8.5, 0.0], 'pos_speed': -10.125,
                  'pos_angle': 11.75, 'pos_radius': 13.25, 'integers': [-5, 6, -7, 8],
                  'floats': [1.25, -2.75, 3.5, -4.125, 5.25, -6.375, 7.125, -8.25],
                  'timer': 76, 'clock': 77, 'clock_float': 77.0, 'life': 1234,
                  'flags': 0xabcd, 'invulnerable': 0, 'script_id': 0x11223344,
                  'trace_id': 7},
        'boss': {'position': [211.0, -222.0, 0.0], 'motion_delta': [200.25, -250.5],
                 'integers': [-55, 66, -77, 88], 'floats': [-11.25, 12.25, -13.5, 14.5],
                 'script_id': 17, 'trace_id': 23},
        'third': {'flags': 0x31, 'invulnerable': 5},
        'player': {'x': 192.5, 'y': 288.25},
        'anm': {'script': -1234}, 'manager': {'count': 3, 'last_created': 77,
        'writable_integers': [3, -4, 5], 'shared_integers': [9, -10, 11, 12],
        'shared_floats': [13.0, -14.0, 15.5, -16.25, 17.75, -18.5, 2.25, -3.5]},
        'globals': {'stage': 5, 'difficulty': 3, 'rank': 20, 'spell_id': 20,
                    'selected_spell_id': 21, 'lives': 2, 'replay_key': 0,
                    'sse_integer': 1}, 'random': {'seed': 0x5678, 'count': 0}}}
    for kind, function in (('int', GVARS_INT_READ), ('float', GVARS_FLOAT_READ)):
        machine = Machine(exe)
        enemy = machine.alloc(0x6000)
        boss = machine.alloc(0x6000)
        third = machine.alloc(0x6000)
        manager = machine.alloc(0x400)
        player = machine.alloc(0x2000)
        anm = machine.alloc(0x600)
        game_info = machine.alloc(0x100)
        enemies = machine.alloc(16)
        out = machine.alloc(4)
        int_stub = machine.alloc(32)
        float_stub = machine.alloc(32)
        machine.write(0x4c2188, struct.pack('<I', manager))
        machine.write(0x4c22c4, struct.pack('<I', player))
        machine.write(manager + 0x5c, struct.pack('<I', boss))
        machine.write(manager + 0xb0, struct.pack('<I', enemies))
        nodes = [machine.alloc(8) for _ in range(3)]
        for index, target in enumerate((enemy, boss, third)):
            machine.write(nodes[index], struct.pack('<2I', target, nodes[index + 1] if index < 2 else 0))
        machine.write(enemies, struct.pack('<I', nodes[0]))
        machine.write(manager + 0xb8, struct.pack('<I', 3))
        machine.write(manager + 0xc0, struct.pack('<I', 77))
        machine.write(manager + 0x10, struct.pack('<3i', 3, -4, 5))
        machine.write(manager + 0x2c, struct.pack('<4i', 9, -10, 11, 12))
        machine.write(manager + 0x3c,
                      struct.pack('<8f', 13.0, -14.0, 15.5, -16.25, 17.75, -18.5, 2.25, -3.5))
        machine.write(enemy + 0x1230, struct.pack('<3f', 11.0, -22.0, 33.0))
        machine.write(enemy + 0x1264, struct.pack('<2f', 0.25, -0.5))
        machine.write(enemy + 0x1274, struct.pack('<3f', 1.5, -2.25, 0.0))
        machine.write(enemy + 0x128c, struct.pack('<3f', 1234.0, -7.25, 9.5))
        machine.write(enemy + 0x12b8, struct.pack('<3f', 7.5, 8.5, 0.0))
        machine.write(enemy + 0x12d0, struct.pack('<3f', -10.125, 11.75, 13.25))
        machine.write(enemy + 0x1474, struct.pack('<4i', -5, 6, -7, 8))
        machine.write(enemy + 0x1484,
                      struct.pack('<8f', 1.25, -2.75, 3.5, -4.125, 5.25, -6.375, 7.125, -8.25))
        machine.write(enemy + 0x14a4, struct.pack('<3i', 76, 77, bits(77.0)))
        machine.write(enemy + 0x5138, struct.pack('<2i', 1234, 1234))
        machine.write(enemy + 0x521c, struct.pack('<I', 0xabcd))
        machine.write(enemy + 0x51bc, struct.pack('<i', 0))
        machine.write(enemy + 0x11e4, struct.pack('<2i', 0x11223344, 7))
        machine.write(enemy + 0x130c, struct.pack('<I', 0))
        machine.write(boss + 0x1230, struct.pack('<3f', 211.0, -222.0, 0.0))
        machine.write(boss + 0x1264, struct.pack('<2f', 200.25, -250.5))
        machine.write(boss + 0x1474, struct.pack('<4i', -55, 66, -77, 88))
        machine.write(boss + 0x1484, struct.pack('<4f', -11.25, 12.25, -13.5, 14.5))
        machine.write(boss + 0x11e4, struct.pack('<2i', 17, 23))
        machine.write(third + 0x521c, struct.pack('<I', 0x31))
        machine.write(third + 0x51bc, struct.pack('<i', 5))
        machine.write(player + 0x5b8, struct.pack('<2f', 192.5, 288.25))
        machine.write(anm + 0x4aa, struct.pack('<hxx', -1234))
        machine.write(0x4be7b8, struct.pack('<2i', 5, 3))
        machine.write(0x4be7c4, struct.pack('<I', 3))
        machine.write(0x4be7cc, struct.pack('<I', 20))
        machine.write(0x4be7d4, struct.pack('<I', 21))
        machine.write(0x4be7e8, struct.pack('<I', 2))
        machine.write(0x4c2194, struct.pack('<I', game_info))
        machine.write(game_info + 0x74, struct.pack('<I', 1))
        machine.write(0x4dcc30, struct.pack('<I', 0))
        machine.write(0x4e6848, struct.pack('<I', 1))

        def redirect_anm(uc):
            uc.mem_write(ANM_LOOKUP, b'\xb8' + struct.pack('<I', anm) + b'\xc3')

        machine.hook_code(ANM_LOOKUP, redirect_anm)
        machine.write(ANM_LOOKUP, b'\xb8' + struct.pack('<I', anm) + b'\xc3')
        machine.write(int_stub, b'\xa3' + struct.pack('<I', out) + b'\x68' +
                      struct.pack('<I', RETURN) + b'\xc3')
        machine.write(float_stub, b'\xd9\x1d' + struct.pack('<I', out) + b'\x68' +
                      struct.pack('<I', RETURN) + b'\xc3')

        def reset_random():
            machine.write(ANM_GAME_RNG, struct.pack('<HxxI', 0x5678, 0))

        def rng_state():
            return dict(zip(('seed', 'count'), struct.unpack('<HxxI', machine.read(ANM_GAME_RNG, 8))))

        rows = []
        for identifier in identifiers:
            reset_random()
            return_stub = int_stub if kind == 'int' else float_stub
            machine.write(stack, struct.pack('<2I', return_stub, identifier & 0xffffffff))
            machine.reg_write('esp', stack)
            machine.reg_write('ecx', enemy)
            machine.uc.emu_start(function, RETURN)
            if kind == 'int':
                raw = struct.unpack('<i', machine.read(out, 4))[0]
                row = {'id': identifier, 'value': raw}
            else:
                row = {'id': identifier, 'bits': f32_bits(machine, out)}
            state = rng_state()
            if state != {'seed': 0x5678, 'count': 0}:
                row['rng'] = state
            rows.append(row)
        document[kind] = rows
    return document


def anm_variables_probe(exe):
    identifiers = [float(value) for value in range(10000, 10033)] + [10004.5, 10012.5, 9999.0, 10033.0, 5.0, -3.25]
    document = {'schema': 'th13/oracle-probes/v1', 'float_read': [], 'int_read': [], 'float_dest': [],
                'int_dest': [], 'setup': {
                    'integers': [11, -22, 33, 44], 'floats': [1.5, -2.25, 3.125, 4.5],
                    'extra_integers': [55, 66], 'position': [7.5, 8.5, 9.5],
                    'rotation': [0.25, 0.5, 0.75], 'random_scale': 1.75, 'random_angle': 2.5,
                    'random_bound': 1000, 'parent_rotation_z': 1.25, 'visual_seed': 0x1234,
                    'game_seed': 0x5678, 'translation': [10.0, 20.0, 1000.0],
                    'reference': [0.1, 0.2, 0.3], 'origin': [100.0, 200.0, 300.0]}}
    for kind, function in (('float', ANM_FLOAT_READ), ('int', ANM_INT_READ)):
        for identifier in identifiers:
            machine = Machine(exe)
            vm = machine.alloc(0x1000)
            parent = machine.alloc(0x100)
            arg = machine.alloc(4)
            out = machine.alloc(4)
            stub = machine.alloc(32)
            anm_setup(machine, vm, parent)
            if kind == 'float':
                value = anm_call_float(machine, function, vm, bits(identifier), out, stub)
            else:
                value = machine.start(function, RETURN, eax=int(identifier), ecx=vm, edx=0, ebx=0, esi=0, edi=0)
                value = value - 0x100000000 if value >= 0x80000000 else value
            document[kind + '_read'].append({'id': identifier, 'value': value, 'state': anm_state(machine)})
    for kind, function, registers in (('float', ANM_FLOAT_WRITE, {'edi': 'ARG', 'esi': 'VM'}),
                                      ('int', ANM_INT_WRITE, {'eax': 'ARG', 'edx': 'VM'})):
        for identifier in identifiers:
            machine = Machine(exe)
            vm = machine.alloc(0x1000)
            parent = machine.alloc(0x100)
            arg = machine.alloc(4)
            out = machine.alloc(4)
            stub = machine.alloc(32)
            anm_setup(machine, vm, parent)
            if kind == 'float':
                machine.write(arg, struct.pack('<I', bits(identifier)))
            else:
                machine.write(arg, struct.pack('<I', struct.unpack('<I', struct.pack('<i', int(identifier)))[0]))
            named = {name: (arg if value == 'ARG' else vm) for name, value in registers.items()}
            target = machine.start(function, RETURN, eax=named.get('eax', 0), ecx=named.get('ecx', 0),
                                   edx=named.get('edx', 0), ebx=named.get('ebx', 0),
                                   esi=named.get('esi', 0), edi=named.get('edi', 0))
            document[kind + '_dest'].append({'id': identifier, 'offset': None if target == arg else target - vm})
    return document


def interpolation_probe(exe):
    machine = Machine(exe)
    machine.write(INTERP_RATE, struct.pack('<f', 1.0))
    block = machine.alloc(0x100)
    out = machine.alloc(4)
    stub = machine.alloc(32)
    code = b'\xd9\x05' + struct.pack('<I', INTERP_RATE)
    call_at = len(code)
    code += b'\xe8' + b'\0\0\0\0' + b'\xd9\x1d' + struct.pack('<I', out)
    code += b'\x68' + struct.pack('<I', RETURN) + b'\xc3'
    relative = INTERP_FUNCTION - (stub + call_at + 5)
    code = code[:call_at + 1] + struct.pack('<i', relative) + code[call_at + 5:]
    machine.write(stub, code)

    def run(start, end, frames, mode):
        machine.write(block, struct.pack('<12I', bits(start), bits(end), 0, 0, 0, 0xfff0bdc1, 0xffffffff,
                                        0, INTERP_RATE, 1, frames & 0xffffffff, mode & 0xffffffff))
        values = []
        for frame in range(frames + 3):
            try:
                machine.start(stub, RETURN, edi=block)
            except Exception as error:
                values.append({'frame': frame, 'error': str(error)})
                break
            duration = struct.unpack('<i', machine.read(block + 0x28, 4))[0]
            values.append({'frame': frame, 'value': machine.f32(out), 'duration': duration})
        return values

    cases = []
    for start, end, frames, mode in ((0.5, 2.0, 30, 0), (1.0, 0.25, 20, 1), (0.5, 3.0, 45, 0),
                                     (1.0, 0.5, 10, 18), (2.0, 1.0, 1, 0), (1.0, 2.0, 0, 0)):
        cases.append({'start': start, 'end': end, 'frames': frames, 'mode': mode,
                      'values': run(start, end, frames, mode)})
    return {'schema': 'th13/oracle-probes/v1', 'cases': cases}


def interpolation_angle_probe(exe):
    machine = Machine(exe)
    machine.write(INTERP_RATE, struct.pack('<f', 1.0))
    block = machine.alloc(0x100)
    out = machine.alloc(4)
    stub = machine.alloc(32)
    code = b'\xdb\xe3' + b'\xd9\x05' + struct.pack('<I', INTERP_RATE)
    call_at = len(code)
    code += b'\xe8' + b'\0\0\0\0'
    code += b'\x68' + struct.pack('<I', RETURN) + b'\xc3'
    relative = ANGLE_INTERP_FUNCTION - (stub + call_at + 5)
    code = code[:call_at + 1] + struct.pack('<i', relative) + code[call_at + 5:]
    machine.write(stub, code)

    def run(start, end, frames, mode, control1=0.0, control2=0.0):
        machine.write(block, struct.pack('<12I', bits(start), bits(end), bits(control1), bits(control2), 0,
                                         0xfff0bdc1, 0xffffffff, 0, INTERP_RATE, 1, frames & 0xffffffff,
                                         mode & 0xffffffff))
        values = []
        for frame in range(frames + 1):
            try:
                machine.start(stub, RETURN, edi=block, ebx=out)
            except Exception as error:
                values.append({'frame': frame, 'error': str(error)})
                break
            duration = struct.unpack('<i', machine.read(block + 0x28, 4))[0]
            values.append({'frame': frame, 'value': machine.f32(out), 'duration': duration})
        return values

    cases = []
    for start, end, frames, mode, control1, control2 in (
        (3.5, 8.25, 6, 7, 0.0, 0.0),
        (4.25, 0.75, 6, 17, 0.0, 2.5),
        (5.25, -6.5, 12, 8, 8.75, -9.25),
        (5.75, -8.25, 12, 0, 0.0, 0.0),
        (-7.25, 9.5, 12, 1, 0.0, 0.0),
        (9.75, -12.25, 15, 18, 0.0, 0.0),
        (-10.5, 13.75, 15, 22, 0.0, 0.0),
    ):
        cases.append({'start': start, 'end': end, 'control1': control1, 'control2': control2,
                      'frames': frames, 'mode': mode, 'values': run(start, end, frames, mode, control1, control2)})
    return {'schema': 'th13/oracle-probes/v1', 'cases': cases}


def practice_scan(machine, stage, difficulty, start, stop):
    machine.write(0x4be81c, struct.pack('<I', stage))
    machine.write(0x4be7c4, struct.pack('<I', difficulty))
    machine.write(machine.u32(PRACTICE_SCORE_POINTER), bytes(0x30000))
    try:
        machine.start(start, stop, eax=stage, edi=0, esi=machine.u32(PRACTICE_DIALOG_POINTER))
    except Exception as error:
        return ['ERR ' + str(error) + ' eax=' + hex(machine.reg_read('eax'))]
    score = machine.u32(PRACTICE_SCORE_POINTER)
    return [offset for offset in range(0x30000) if machine.read(score + offset, 1)[0]]


def practice_rows_probe(exe):
    machine = Machine(exe)
    score = machine.alloc(0x30000)
    dialog = machine.alloc(0x1000)
    machine.write(PRACTICE_SCORE_POINTER, struct.pack('<I', score))
    machine.write(PRACTICE_DIALOG_POINTER, struct.pack('<I', dialog))
    machine.write(0x4be7bc, struct.pack('<I', 0))
    machine.write(0x4be7b8, struct.pack('<I', 0))
    document = {'schema': 'th13/oracle-probes/v1', 'play_flag': [], 'pass_flag': []}
    for stage in (1, 2, 6, 7):
        for difficulty in (0, 3):
            document['play_flag'].append({'stage': stage, 'difficulty': difficulty,
                                          'offsets': practice_scan(machine, stage, difficulty, 0x42ba42, 0x42ba6f)})
    for stage in (1, 2, 6, 7):
        for difficulty in (0, 3):
            document['pass_flag'].append({'stage': stage, 'difficulty': difficulty,
                                          'offsets': practice_scan(machine, stage, difficulty, 0x42d087, 0x42d0bc)})
    return document


def save_image_probe(exe):
    machine = Machine(exe)
    snapshot = machine.alloc(0x100)
    header = machine.alloc(32)
    machine.map(SAVE_MALLOC_RESULT, 0x400000)
    machine.write(SAVE_IMAGE_POINTER, struct.pack('<I', snapshot))
    machine.write(snapshot, struct.pack('<I', header))
    machine.write(header, struct.pack('<8I', 0x1B680, 0x1B690, 4, 0, 0x100, 0, 0, 0))
    for column in range(5):
        address = snapshot + 8 + column * 0x56dc
        machine.write(address, struct.pack('<HxxI', 0x5243, 0x12345678))
        machine.write(snapshot + 0x14 + column * 0x56dc, struct.pack('<I', column))
    markers = []
    for char in (0, 3):
        origin = snapshot + char * 0x56dc
        values = ((0x7c8, b'\x44\x33\x22\x11'), (0x7fc + 8, b'\xab'), (0x7fd + 8, b'\xcd'),
                  (0x7f8 + 8, b'\x88\x77\x66\x55'), (0x7fc + 16 + 192, b'\xef'),
                  (0x7fd + 16 + 192, b'\x9a'), (0x7f8 + 16 + 192, b'\x21\x43\x65\x87'))
        for offset, value in values:
            machine.write(origin + offset, value)
            markers.append({'char': char, 'game_offset': offset, 'bytes': value.hex()})

    def on_malloc(uc):
        esp = uc.reg_read(UC_X86_REG_ESP)
        return_address = struct.unpack('<I', bytes(uc.mem_read(esp, 4)))[0]
        uc.reg_write(UC_X86_REG_ESP, esp + 4)
        uc.reg_write(UC_X86_REG_EAX, SAVE_MALLOC_RESULT)
        uc.reg_write(UC_X86_REG_EIP, return_address)

    captured = {}

    def on_compress(uc):
        esp = uc.reg_read(UC_X86_REG_ESP)
        destination, length = struct.unpack('<II', bytes(uc.mem_read(esp, 8)))
        captured['image'] = bytes(uc.mem_read(destination, length))
        machine.stop()

    machine.hook_code(SAVE_MALLOC_SITE, on_malloc)
    machine.hook_code(SAVE_COMPRESS_SITE, on_compress)
    machine.start(SAVE_IMAGE_FUNCTION, RETURN)
    image = captured.get('image', b'')
    result = {'schema': 'th13/oracle-probes/v1', 'image_size': len(image), 'markers': []}
    for marker in markers:
        value = bytes.fromhex(marker['bytes'])
        offsets = []
        cursor = 0
        while True:
            at = image.find(value, cursor)
            if at < 0:
                break
            offsets.append(at)
            cursor = at + 1
        marker['file_offsets'] = [hex(offset) for offset in offsets]
        result['markers'].append(marker)
    return result


def main(argv=None):
    parser = argparse.ArgumentParser()
    parser.add_argument('--exe', default='/work/games/th13/th13.exe')
    parser.add_argument('--out', default='/work/artifacts/th13-fidelity/collision.json')
    parser.add_argument('--out-dir')
    options = parser.parse_args(argv)
    machine = Machine(options.exe)
    tables = {name: [machine.f32(address + 4 * index) for index in range(count)]
              for name, (address, count) in SIZE_TABLES.items()}
    document = {'schema': 'th13/oracle-probes/v1', 'tables': tables, 'collision': collision_probe(machine)}
    if options.out_dir:
        Path(options.out_dir).mkdir(parents=True, exist_ok=True)
        documents = {
            'collision.json': document,
            'anm-variables.json': anm_variables_probe(options.exe),
            'interpolation.json': interpolation_probe(options.exe),
            'interpolation-angle.json': interpolation_angle_probe(options.exe),
            'practice-rows.json': practice_rows_probe(options.exe),
            'save-image.json': save_image_probe(options.exe),
            'stage-gvars.json': stage_gvars_probe(options.exe),
        }
        for name, output in documents.items():
            with open(Path(options.out_dir) / name, 'w') as handle:
                handle.write(json.dumps(output, indent=1, sort_keys=True) + '\n')
        return document
    text = json.dumps(document, indent=1, sort_keys=True) + '\n'
    with open(options.out, 'w') as handle:
        handle.write(text)
    return text


if __name__ == '__main__':
    print(main())
