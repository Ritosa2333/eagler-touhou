"""Actual moving-laser mirror extensions and factory/embedded ANM execution.

Local laser-manager experiment, not a whole-game or replay golden. Synthetic
parameters/player and host heap/audio output are explicit; all factory, manager,
extension, movement and embedded original bullet ANM instructions execute.
"""
import collections, json, math, pathlib, struct, sys
ROOT = pathlib.Path(__file__).resolve().parents[2]
sys.path.insert(0, str(ROOT.parent / 'tools/python'))
sys.path.insert(0, str(ROOT / 'scripts/reverse'))
from native_anm_resource import check_original, EXE_SHA256, install_resource
import pefile
from unicorn import Uc, UC_ARCH_X86, UC_MODE_32, UC_HOOK_CODE
from unicorn.x86_const import *
EXE = ROOT.parent / 'th12/th12.exe'
check_original(EXE)
PE = pefile.PE(str(EXE))
IMAGE = PE.get_memory_mapped_image()
VTABLES = {kind: list(struct.unpack_from('<20I', IMAGE, pointer - 0x400000))
           for kind, pointer in enumerate([0x4a0654, 0x4a0704, 0x4a06ac])}
CONSTANTS = {hex(pointer): struct.unpack_from('<' + fmt, IMAGE, pointer - 0x400000)[0]
             for pointer, fmt in [(0x4a3d60, 'd'), (0x4a3d28, 'd'), (0x4a3d50, 'd'),
                                  (0x4a3d98, 'f'), (0x4a3d88, 'f'), (0x4a3d80, 'd'),
                                  (0x4a4180, 'd')]}

def f32(value): return struct.unpack('<f', struct.pack('<f', value))[0]
def bits(value): return struct.unpack('<I', struct.pack('<f', value))[0]

def experiment(kind, position, angle, length, speed, c, flags, changed_speed=-1., extension=0x100,
               width=8., player_position=(5000., 5000.), player_width=2., active_phase=False,
               initial_age=0, steps=4):
    v = Uc(UC_ARCH_X86, UC_MODE_32)
    for address, size in [(0, 0x10000), (0x400000, (PE.OPTIONAL_HEADER.SizeOfImage + 4095) & ~4095),
                          (0x1000000, 0x10000), (0x2000000, 0x1000000),
                          (0x3000000, 0x1000000), (0x5000000, 0x1000000),
                          (0x6000000, 0x1000000)]: v.mem_map(address, size)
    v.mem_write(0x400000, IMAGE)
    v.reg_write(UC_X86_REG_FPCW, 0x37f)
    v.reg_write(UC_X86_REG_MXCSR, 0x1f80)
    LM, PLAYER, SHT, ENEMIES, PARAM = 0x2000000, 0x2010000, 0x2020000, 0x2030000, 0x2040000
    SP, STOP = 0x100e000, 0x100f000
    def put(at, fmt, *values): v.mem_write(at, struct.pack('<' + fmt, *values))
    def get(at, fmt): return list(struct.unpack('<' + fmt, v.mem_read(at, struct.calcsize('<' + fmt))))
    def ret(pop=0, result=0):
        sp = v.reg_read(UC_X86_REG_ESP)
        target = get(sp, 'I')[0]
        v.reg_write(UC_X86_REG_ESP, sp + 4 + pop)
        v.reg_write(UC_X86_REG_EAX, result)
        v.reg_write(UC_X86_REG_EIP, target)
    heap = 0x3000000
    events, frame, collision_query = [], -1, None
    def hook(mu, address, size, user):
        nonlocal heap, collision_query
        sp = v.reg_read(UC_X86_REG_ESP)
        if collision_query and address == collision_query['returnAt']:
            collision_query.pop('returnAt')
            collision_query['nativeReturn'] = v.reg_read(UC_X86_REG_EAX)
            events.append(collision_query)
            collision_query = None
        elif address == 0x437a80:
            collision_query = {'frame': frame, 'kind': 'lineCollision', 'caller': hex(get(sp, 'I')[0]),
                               'returnAt': get(sp, 'I')[0], 'position': get(v.reg_read(UC_X86_REG_EAX), '3f'),
                               'angleTransverseLength': get(sp + 4, '3f')}
        elif address == 0x4391c0:
            events.append({'frame': frame, 'kind': 'grazeAward'})
        if address in [0x46c9ea, 0x46d04a]:
            amount = get(sp + 4, 'I')[0]
            allocation = heap
            heap += (max(amount, 1) + 15) & ~15
            ret(result=allocation)
        elif address in [0x46ca4f, 0x46c941]: ret()
        elif address in [0x453e20, 0x453d90, 0x453f10]:
            events.append({'frame': frame, 'kind': 'soundOutput', 'entry': hex(address)})
            ret(4 if address == 0x453e20 else 0)
        elif address == 0x428450:
            source = v.reg_read(UC_X86_REG_EDI)
            events.append({'frame': frame, 'kind': 'factory', 'laserKind': get(sp + 4, 'i')[0],
                           'profileHex': bytes(v.mem_read(source, {0: 0x1e8, 1: 0x208, 2: 0x1e0}[get(sp + 4, 'i')[0]])).hex()})
    v.hook_add(UC_HOOK_CODE, hook)
    def call(address, arguments=(), registers=None):
        put(SP, 'I' * (len(arguments) + 1), STOP, *arguments)
        v.reg_write(UC_X86_REG_ESP, SP)
        for register, value in (registers or {}).items(): v.reg_write(register, value)
        v.emu_start(address, STOP, count=3000000)
        if v.reg_read(UC_X86_REG_EIP) != STOP: raise RuntimeError(f'Native laser budget {address:x}')
        return v.reg_read(UC_X86_REG_EAX)
    put(0x4b2ed0, 'f', 1.)
    put(0x4d52d4, 'I', 1)
    put(0x4d52dc, 'I', 1)
    put(0x4ce8cc, 'I', 0x5000000)
    put(0x4ce568, 'II', 42, 0)
    put(0x4b4514, 'I', PLAYER)
    put(0x4b43dc, 'I', ENEMIES)
    put(PLAYER + 0xa28, 'i', 1)
    put(PLAYER + 0xa2c, 'I', SHT)
    put(SHT + 4, 'f', 2.)
    put(PLAYER + 0x9e4, '2f', player_width, player_width)
    put(PLAYER + 0xc404, 'i', 1)
    put(PLAYER + 0x97c, '3f', *player_position, 0.)
    put(PLAYER + 0x9cc, '6f', player_position[0] - player_width, player_position[1] - player_width, 0.,
        player_position[0] + player_width, player_position[1] + player_width, 0.)
    resource = install_resource(v, (ROOT / 'local/retail/bullet.anm').read_bytes(), 0x6000000, 6)
    call(0x427fa0, registers={UC_X86_REG_EDI: LM})
    put(LM + 0x464, 'I', LM + 0x10)
    put(LM + 0x488, 'I', resource['resource'])
    put(0x4b43d4, 'I', 0x2060000)
    put(0x2060010, 'I', resource['resource'])
    put(0x4b43c8, 'I', 0x2080000)
    # +4debdc lies outside synthetic object, in the mapped object arena.
    put(0x2080000 + 0x4debdc, 'I', resource['resource'])
    if kind == 0:
        put(PARAM, '3f', *position, 0.)
        put(PARAM + 12, '5f', angle, length, length, 0., width)
        put(PARAM + 32, 'f', speed)
        put(PARAM + 36, 'HH', 17, 4)
        put(PARAM + 0x30, 'ffiiII', changed_speed, 0., c, flags, extension, 0)
        put(PARAM + 0x1e0, 'ii', -1, -1)
    elif kind == 2:
        put(PARAM, '3f', *position, 0.)
        put(PARAM + 12, '3f', angle, width, speed)
        put(PARAM + 24, 'HHi', 0, 4, 8)
        put(PARAM + 0x28, 'ffiiII', changed_speed, 0., c, flags, extension, 0)
        put(PARAM + 0x1d8, 'ii', -1, -1)
    elif kind == 1:
        put(PARAM, '3f', *position, 0.)
        put(PARAM + 12, '5f', 0., 0., 0., angle, 0.)
        put(PARAM + 32, '4f', length, length, width, speed)
        put(PARAM + 48, '4i', 8, 3, 1000 if active_phase else 30, 5)
        put(PARAM + 64, '2i', -1, -1)
        put(PARAM + 80, 'HHI', 17, 4, 2)
        put(PARAM + 0x58, 'ffiiII', changed_speed, 0., c, flags, extension, 0)
    else: raise ValueError(kind)
    call(0x428450, [kind], {UC_X86_REG_EDI: PARAM})
    laser = get(LM + 0x18, 'I')[0]
    if active_phase:
        put(laser + 0xc, 'i', 2)
        put(laser + 0x70, 'f', width)
    if initial_age:
        put(laser + 0x14, 'iif', initial_age - 1, initial_age, float(initial_age))
    def capture():
        rows = []
        node = get(LM + 0x18, 'I')[0]
        while node:
            actual_kind = {0x4a0654: 0, 0x4a0704: 1, 0x4a06ac: 2}[get(node, 'I')[0]]
            parameter_size = {0: 0x1e8, 1: 0x208, 2: 0x1e0}[actual_kind]
            vm = node + {0: 0x63c, 1: 0x660, 2: 0x634}[actual_kind]
            rows.append({'id': get(node + 0x80, 'I')[0], 'kind': actual_kind,
                         'phase': get(node + 0xc, 'i')[0], 'age': get(node + 0x18, 'i')[0],
                         'position': get(node + 0x50, '3f'), 'velocity': get(node + 0x5c, '3f'),
                         'geometry': get(node + 0x68, '5f'), 'active': get(node + 0x430, 'I')[0],
                         'cursor': get(node + 0x42c, 'i')[0], 'bounce': get(node + 0x168, 'f')[0:1] + get(node + 0x17c, 'iii'),
                         'parameterHex': bytes(v.mem_read(node + 0x454, parameter_size)).hex(),
                         'animationFlags': get(vm + 0x47c, 'II')})
            node = get(node + 8, 'I')[0]
            if len(rows) > 256: raise RuntimeError('Laser list cycle')
        return {'frame': frame, 'count': get(LM + 0x468, 'i')[0],
                'rng': get(0x4ce568, 'II'), 'nodes': rows}
    frames = [capture()]
    for frame in range(steps):
        call(0x428270, registers={UC_X86_REG_EDI: LM})
        frames.append(capture())
    return {'input': {'kind': kind, 'position': list(map(f32, position)), 'angle': f32(angle),
                      'length': length, 'speed': speed, 'c': c, 'flags': flags,
                      'changedSpeed': changed_speed, 'extension': extension, 'width': width,
                      'playerPosition': list(map(f32, player_position)), 'playerWidth': player_width,
                      'activePhase': active_phase, 'initialAge': initial_age}, 'frames': frames, 'events': events}

def main():
    records = []
    for flags, position, angle in [(1, (20., 2.), -math.pi / 2), (2, (20., 446.), math.pi / 2),
                                   (4, (-190., 120.), math.pi), (8, (190., 120.), 0.),
                                   (15, (190., 446.), math.pi / 4), (31, (190., 446.), math.pi / 4)]:
        for c in [0, 1, 2, 3]:
            for speed in [-1., 3.]: records.append(experiment(0, position, angle, 16., 2., c, flags, speed))
    for flags, position, angle in [(1, (0., 16.), -math.pi / 2), (2, (0., 432.), math.pi / 2),
                                   (4, (-176., 120.), math.pi), (8, (176., 120.), 0.),
                                   (0, (190., 446.), math.pi / 4), (15, (0., 120.), .3)]:
        records.append(experiment(0, position, angle, 16., 2., 2, flags))
    for changed_speed in [0., .001, -990., 999.]:
        records.append(experiment(0, (190., 120.), 0., 16., 2., 2, 8, changed_speed))
    for c in [0, 1, 2]: records.append(experiment(2, (190., 120.), 0., 16., 2., c, 8))
    for length in [-1., 0., 1.]: records.append(experiment(0, (0., 120.), 0., length, 0., 0, 0))
    for kind in [0, 1, 2]:
        for c in [0, 1]: records.append(experiment(kind, (0., 120.), .3, 16., 2., c, 0, extension=0x10000000))
    output = ROOT / 'tests/fixtures/laser-extensions-original.json'
    output.write_text(json.dumps({'schema': 'th12-original-laser-bounce/1', 'originalExeSha256': EXE_SHA256,
        'wholeGameOracle': False, 'vtables': VTABLES, 'constants': CONSTANTS,
        'scope': 'Actual factories/manager ticks including native embedded bullet ANM; synthetic params/far-away Player; malloc/free and audio output only substituted. Registered sentinel/resource are explicit local callback fixtures, application scheduler not executed.',
        'records': records}, indent=2) + '\n', encoding='utf-8')
    print(json.dumps({'output': str(output), 'records': len(records), 'vtables': VTABLES,
                      'constants': CONSTANTS, 'spawned': sum(len(r['events']) > 1 for r in records)}))

if __name__ == '__main__': main()
