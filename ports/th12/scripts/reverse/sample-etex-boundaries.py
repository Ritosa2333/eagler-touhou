"""Independent original TH12 ETEX 400/1000 wait and retry boundary probes.

The original dispatch, countdown, corner geometry, and motion update execute.
ANM update and sound are explicit external boundaries. Sprite dimensions are
prepared native inputs. Vec2Normalize executes the pinned real D3DX9_40 DLL's
SSE2 implementation, not Python/candidate geometry. No runtime/golden edits.
"""
import hashlib
import json
import math
import pathlib
import struct
import sys

ROOT = pathlib.Path(__file__).resolve().parents[2]
sys.path.insert(0, str(ROOT.parent / 'tools/python'))
import pefile
from unicorn import Uc, UC_ARCH_X86, UC_MODE_32, UC_HOOK_CODE
from unicorn.x86_const import *
from native_anm_resource import check_original, EXE_SHA256

EXE = ROOT.parent / 'th12/th12.exe'
DLL = pathlib.Path(r'C:\Windows\SysWOW64\D3DX9_40.dll')
DLL_SHA256 = '16fd909aeb68d0d1aca8529dc7f78880b97d6649d70ce8d03a2c858bc28e216b'
check_original(EXE)
if hashlib.sha256(DLL.read_bytes()).hexdigest() != DLL_SHA256:
    raise ValueError('Pinned D3DX9_40 dependency required; no native execution')
PE, DEPENDENCY = pefile.PE(str(EXE)), pefile.PE(str(DLL))
BULLET, SPRITE = 0x2000000, 0x2010000
SP, STOP = 0x100e000, 0x100f000


class Native:
    def __init__(self):
        self.mu = Uc(UC_ARCH_X86, UC_MODE_32)
        for address, size in [(0, 0x10000),
                              (0x400000, (PE.OPTIONAL_HEADER.SizeOfImage + 4095) & ~4095),
                              (0x1000000, 0x10000), (BULLET, 0x20000),
                              (0x10000000, (DEPENDENCY.OPTIONAL_HEADER.SizeOfImage + 4095) & ~4095)]:
            self.mu.mem_map(address, size)
        self.mu.mem_write(0x400000, PE.get_memory_mapped_image())
        self.mu.mem_write(0x10000000, DEPENDENCY.get_memory_mapped_image())
        self.mu.reg_write(UC_X86_REG_FPCW, 0x37f)
        self.mu.reg_write(UC_X86_REG_MXCSR, 0x1f80)
        self.mu.reg_write(UC_X86_REG_FPTAG, 0xffff)
        self.mu.reg_write(UC_X86_REG_FPSW, 0)
        self.put(0x4982e4, 'I', 0x101fded0)
        self.call(0x103e18fe, (0x103f8048,))
        if self.get(0x103f8104, 'I')[0] != 0x103e0517:
            raise RuntimeError('Unexpected genuine SSE2 Vec2Normalize entry')
        self.events = []
        self.context = ''
        self.mu.hook_add(UC_HOOK_CODE, self.hook)

    def put(self, address, fmt, *values):
        self.mu.mem_write(address, struct.pack('<' + fmt, *values))

    def get(self, address, fmt):
        return list(struct.unpack('<' + fmt, self.mu.mem_read(address, struct.calcsize('<' + fmt))))

    def ret(self, pop=0, result=0):
        stack = self.mu.reg_read(UC_X86_REG_ESP)
        target = self.get(stack, 'I')[0]
        self.mu.reg_write(UC_X86_REG_ESP, stack + 4 + pop)
        self.mu.reg_write(UC_X86_REG_EAX, result)
        self.mu.reg_write(UC_X86_REG_EIP, target)

    def snapshot(self):
        return {'position': self.get(BULLET + 0x4bc, 'ff'),
                'velocity': self.get(BULLET + 0x4c8, 'ff'),
                'speed': self.get(BULLET + 0x4d4, 'f')[0],
                'angle': self.get(BULLET + 0x4d8, 'f')[0],
                'active': self.get(BULLET + 0x528, 'I')[0],
                'cursor': self.get(BULLET + 0x548, 'i')[0],
                'gateRemaining': self.get(BULLET + 0x974, 'i')[0],
                'gateValue': self.get(BULLET + 0x978, 'f')[0],
                'waitRemaining': self.get(BULLET + 0x808, 'i')[0],
                'waitValue': self.get(BULLET + 0x80c, 'f')[0],
                'accelerationTicks': self.get(BULLET + 0x738, 'i')[0]}

    def hook(self, mu, address, size, unused):
        names = {0x40aa60: 'dispatch', 0x40aab2: 'sequential-gate',
                 0x40aac4: 'duplicate-kind-gate', 0x40ad3c: 'start400',
                 0x40ae9a: 'start1000', 0x40b527: 'consume-record',
                 0x40c480: 'tick400', 0x40c499: 'after400-decrement',
                 0x40c880: 'timer-completion-check', 0x40c892: 'complete400',
                 0x40b6f0: 'acceleration', 0x409dda: 'completion-retry',
                 0x455630: 'anm-tick-boundary'}
        if address in names:
            self.events.append({'context': self.context, 'event': names[address],
                                'address': hex(address), **self.snapshot()})
        if address == 0x40c6d0:
            stack = self.mu.reg_read(UC_X86_REG_ESP)
            self.events.append({'context': self.context, 'event': 'corner-projections',
                                'cross': self.get(stack + 0x18, '4f'),
                                'dot': self.get(stack + 0x28, '4f')})
        elif address == 0x40c85e:
            stack = self.mu.reg_read(UC_X86_REG_ESP)
            self.events.append({'context': self.context, 'event': 'ray-geometry-gate',
                                'sideDotExtrema': [self.get(stack + 8, 'f')[0],
                                                   self.get(stack + 0x10, 'f')[0]]})
        elif address == 0x101fded0:
            stack = self.mu.reg_read(UC_X86_REG_ESP)
            self.events.append({'context': self.context, 'event': 'actual-dll-normalize',
                                'input': self.get(self.get(stack + 8, 'I')[0], 'ff')})
        if address == 0x455630:
            self.ret(4)
        elif address == 0x453d90:
            self.ret()
        elif address == 0x453e20:
            self.ret(4)

    def call(self, address, arguments=(), registers=None, until=STOP):
        self.put(SP, 'I' * (1 + len(arguments)), STOP, *arguments)
        self.mu.reg_write(UC_X86_REG_ESP, SP)
        for register, value in (registers or {}).items():
            self.mu.reg_write(register, value)
        try:
            self.mu.emu_start(address, until, count=1000000)
        except Exception as error:
            raise RuntimeError(f'Native entry {address:x}, eip {self.mu.reg_read(UC_X86_REG_EIP):x}: {error}') from error
        if self.mu.reg_read(UC_X86_REG_EIP) != until:
            raise RuntimeError(f'Original instruction budget exhausted at {address:x}')
        return self.mu.reg_read(UC_X86_REG_EAX)

    def seed(self, records, position=(0, 120), angle=0, speed=0, sprite=(16, 16), rate=1):
        self.mu.mem_write(BULLET, bytes(0x9f8))
        self.mu.mem_write(SPRITE, bytes(0x48))
        self.put(0x4b2ed0, 'f', rate)
        self.put(BULLET, 'I', 1)  # collision-disabled synthetic input
        self.put(BULLET + 0x3fc, 'I', SPRITE)
        # API tuple is width,height; native Sprite+34 is height,+38 width.
        self.put(SPRITE + 0x34, 'ff', sprite[1], sprite[0])
        self.put(BULLET + 0x4bc, 'fff', *position, 0)
        self.put(BULLET + 0x4c8, 'fff', math.cos(angle) * speed, math.sin(angle) * speed, 0)
        self.put(BULLET + 0x4d4, 'ff', speed, angle)
        self.put(BULLET + 0x520, 'i', 1000)
        self.put(BULLET + 0x532, 'H', 1)
        self.put(BULLET + 0x544, 'i', -1)
        for index, row in enumerate(records):
            self.put(BULLET + 0x550 + index * 24, 'ffiiII',
                     row.get('a', 0), row.get('b', 0), row.get('c', 0), row.get('d', 0),
                     row['kind'], int(row.get('concurrent', False)))
        self.events.clear()


def record(kind, c=0, d=0, concurrent=False, a=0, b=0):
    return {'kind': kind, 'c': c, 'd': d, 'concurrent': concurrent, 'a': a, 'b': b}


native = Native()
sequence_cases = []
for kind in [0x400, 0x1000]:
    for duration in [-1, 0, 1, 2, 3]:
        for concurrent in [False, True]:
            records = [record(kind, duration), record(4, 2, concurrent=concurrent, a=.5, b=-999)]
            native.seed(records)
            native.context = 'creation-suffix'
            # Exact original creation suffix: interrupt2, dispatch, BirthTick.
            # Stop before its pool-cursor epilogue; prepared slot is explicit.
            native.call(0x40a9cc, registers={UC_X86_REG_EBX: BULLET,
                        UC_X86_REG_ESI: BULLET + 8, UC_X86_REG_EAX: 0}, until=0x40a9f0)
            samples = [{'step': 'after-birth', **native.snapshot()}]
            for frame in range(1, 7):
                native.context = f'update{frame}'
                native.call(0x4099e0, (BULLET,))
                samples.append({'step': f'update{frame}', **native.snapshot()})
            sequence_cases.append({'records': records, 'samples': samples, 'events': native.events.copy()})

active_gate_cases = []
for records in [
    [record(4, 3, a=.5, b=-999), record(0x400, 2)],
    [record(4, 3, a=.5, b=-999), record(0x400, 2, concurrent=True)],
    [record(0x400, 2), record(0x400, 3, concurrent=True), record(4, 2, a=.5, b=-999)],
    [record(4, 1, a=.5, b=-999), record(0x400, 3, concurrent=True), record(0x1000, 4, concurrent=True)],
    [record(0x400, 0), record(0x1000, 0), record(4, 2, a=.5, b=-999)],
]:
    native.seed(records)
    native.context = 'creation-suffix'
    native.call(0x40a9cc, registers={UC_X86_REG_EBX: BULLET,
                UC_X86_REG_ESI: BULLET + 8, UC_X86_REG_EAX: 0}, until=0x40a9f0)
    samples = [{'step': 'after-birth', **native.snapshot()}]
    for frame in range(1, 9):
        native.context = f'update{frame}'
        native.call(0x4099e0, (BULLET,))
        samples.append({'step': f'update{frame}', **native.snapshot()})
    active_gate_cases.append({'records': records, 'samples': samples, 'events': native.events.copy()})

geometry_cases = []
for sprite in [(8, 8), (16, 16), (64, 64), (32, 14)]:
    positions = [(0, 120), (200, 120), (-200, 120), (0, -12), (0, 460),
                 (200, -12), (-200, 460), (192, 0), (200, 0)]
    left, right, top, bottom = -192 - sprite[0] / 2, 192 + sprite[0] / 2, -sprite[1] / 2, 448 + sprite[1] / 2
    for delta in [-.001, 0, .001]:
        positions += [(left + delta, 120), (right + delta, 120),
                      (0, top + delta), (0, bottom + delta)]
    for position in positions:
        angles = [0, math.pi / 4, math.pi / 2, math.pi, -math.pi / 2, -math.pi / 4]
        for corner in [(left, top), (right, top), (left, bottom), (right, bottom)]:
            angle = math.atan2(corner[1] - position[1], corner[0] - position[0])
            bits = struct.unpack('<I', struct.pack('<f', angle))[0]
            angles += [struct.unpack('<f', struct.pack('<I', bit))[0]
                       for bit in [bits - 1 if bits else 0x80000001, bits, bits + 1]]
        for angle in angles:
            records = [record(0x400, 10, 1)]
            native.seed(records, position, angle, sprite=sprite)
            native.context = 'dispatch'
            native.call(0x40aa60, registers={UC_X86_REG_ECX: BULLET})
            before = native.snapshot()
            native.context = 'first-gate-tick'
            completed = native.call(0x40c480, registers={UC_X86_REG_EDI: BULLET})
            geometry_cases.append({'sprite': sprite, 'position': position, 'angle': native.get(BULLET + 0x4d8, 'f')[0],
                                   'duration': 10, 'd': 1, 'before': before, 'completed': completed,
                                   'after': native.snapshot(), 'events': [event for event in native.events
                                   if event['event'] in ['actual-dll-normalize', 'corner-projections', 'ray-geometry-gate']]})

# Boolean d profile, timer endpoint, and geometry completion followed by a
# sequential acceleration in the SAME original4099e0 call.
ray_retry_cases = []
for d in [0, 1, 2, -1]:
    for duration in [0, 1, 3, 30]:
        for position, angle in [((200, 120), 0), ((200, 120), math.pi),
                                ((0, 120), 0), ((200, -12), -math.pi / 4)]:
            records = [record(0x400, duration, d), record(4, 2, a=.5, b=-999)]
            native.seed(records, position, angle)
            native.context = 'creation-suffix'
            native.call(0x40a9cc, registers={UC_X86_REG_EBX: BULLET,
                        UC_X86_REG_ESI: BULLET + 8, UC_X86_REG_EAX: 0}, until=0x40a9f0)
            before = native.snapshot()
            native.context = 'first-update'
            native.call(0x4099e0, (BULLET,))
            ray_retry_cases.append({'records': records, 'position': position,
                                    'angle': angle, 'before': before, 'after': native.snapshot(),
                                    'events': native.events.copy()})

output = ROOT / 'tests/fixtures/etex-boundaries-native.json'
output.write_text(json.dumps({'schema': 'th12-native-etex-boundaries/1',
    'originalExeSha256': EXE_SHA256, 'd3dx9_40Sha256': DLL_SHA256,
    'wholeGameOracle': False, 'gpuExecuted': False,
    'provider': 'Original40aa60,4099e0,40c480,409970,40d4d0; actual D3DX9_40 Vec2Normalize SSE2',
    'preparedInputs': ['Bullet slot phase1, collision-disabled, lifetime grace1000',
                       'Native Sprite+34 height/+38 width supplied explicitly; sprite tuple is width,height',
                       'Creation suffix40a9cc..40a9f0 runs with callback0 and prepared slot'],
    'externalBoundaries': ['455630 records ANM tick and returns0',
                           '453d90/453e20 sound no-op',
                           'DLL CPU/registry detection replaced by its own SSE2 initializer103e18fe'],
    'sequenceCases': sequence_cases, 'activeGateCases': active_gate_cases, 'geometryCases': geometry_cases,
    'rayRetryCases': ray_retry_cases}, indent=2, allow_nan=False) + '\n', encoding='utf-8')
print(json.dumps({'output': str(output), 'sequenceCases': len(sequence_cases),
                  'activeGateCases': len(active_gate_cases),
                  'geometryCases': len(geometry_cases), 'rayRetryCases': len(ray_retry_cases)}))
