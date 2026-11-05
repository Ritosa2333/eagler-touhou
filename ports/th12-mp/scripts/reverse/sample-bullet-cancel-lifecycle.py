"""Pinned TH12 scripted cancellation, timer reset and next Bullet21 visit.

Original40c8b0/40d560 and40a020/4099e0 execute. The only instruction stub
is455630: an enabled embedded animation remains alive without GPU/resource
work. No replay bytes, original assets or existing accepted fixture are changed.
"""
import json
import pathlib
import struct
import sys

ROOT = pathlib.Path(__file__).resolve().parents[2]
sys.path.insert(0, str(ROOT.parent / 'tools/python'))
sys.path.insert(0, str(ROOT / 'scripts/reverse'))
from native_anm_resource import check_original, EXE_SHA256
import pefile
from unicorn import Uc, UC_ARCH_X86, UC_MODE_32, UC_HOOK_CODE
from unicorn.x86_const import *

EXE = ROOT.parent / 'th12/th12.exe'
check_original(EXE)
PE = pefile.PE(str(EXE))
MANAGER, BULLET, SP, STOP = 0x2000000, 0x2000064, 0x100e000, 0x100f000

class Native:
    def __init__(self, initial):
        self.mu = Uc(UC_ARCH_X86, UC_MODE_32)
        for at, size in [(0x400000, (PE.OPTIONAL_HEADER.SizeOfImage + 4095) & ~4095),
                         (0x1000000, 0x10000), (MANAGER, 0x600000)]:
            self.mu.mem_map(at, size)
        self.mu.mem_write(0x400000, PE.get_memory_mapped_image())
        self.mu.reg_write(UC_X86_REG_FPCW, 0x37f)
        self.mu.reg_write(UC_X86_REG_MXCSR, 0x1f80)
        self.put(0x4b2ed0, 'f', 1)
        self.put(0x4b43c8, 'I', MANAGER)
        self.put(0x4b44e8, 'I', 0)
        self.put(BULLET, 'Ii', 2, 9)
        self.put(BULLET + 0x532, 'H', initial['phase'])
        self.put(BULLET + 0x4bc, 'fff', *initial['position'], 0.1)
        self.put(BULLET + 0x4c8, 'fff', *initial['velocity'], 0)
        self.put(BULLET + 0x4e4, 'iifII', initial['age'] - 1, initial['age'],
                 initial['age'], 0x4b2ed0, initial['timerFlags'])
        self.put(BULLET + 0x524, 'i', -1)
        self.put(BULLET + 0x520, 'i', 10)
        self.events = []
        self.mu.hook_add(UC_HOOK_CODE, self.tail, begin=0x455630, end=0x455630)

    def put(self, at, fmt, *values):
        self.mu.mem_write(at, struct.pack('<' + fmt, *values))

    def get(self, at, fmt):
        return list(struct.unpack('<' + fmt, self.mu.mem_read(at, struct.calcsize('<' + fmt))))

    def tail(self, mu, at, size, user):
        self.events.append('embedded-animation-tail')
        sp = mu.reg_read(UC_X86_REG_ESP)
        ret = self.get(sp, 'I')[0]
        mu.reg_write(UC_X86_REG_ESP, sp + 8)
        mu.reg_write(UC_X86_REG_EAX, 0)
        mu.reg_write(UC_X86_REG_EIP, ret)

    def call(self, at, manager=False):
        self.put(SP, 'I', STOP)
        self.mu.reg_write(UC_X86_REG_ESP, SP)
        self.mu.reg_write(UC_X86_REG_ESI, MANAGER if manager else BULLET)
        self.mu.emu_start(at, STOP, count=1000000)
        if self.mu.reg_read(UC_X86_REG_EIP) != STOP:
            raise RuntimeError(f'Original cancellation budget exceeded: {at:x}')
        return self.mu.reg_read(UC_X86_REG_EAX)

    def state(self):
        return {'phase': self.get(BULLET + 0x532, 'H')[0],
                'flags': self.get(BULLET, 'I')[0],
                'position': self.get(BULLET + 0x4bc, 'ff'),
                'timer': self.get(BULLET + 0x4e4, 'iif'),
                'timerFlags': self.get(BULLET + 0x4f4, 'I')[0],
                'word4': self.get(BULLET + 4, 'i')[0],
                'interrupt': self.get(BULLET + 8 + 0x3c4, 'h')[0]}

cases = []
for name, phase, position, velocity, age, timer_flags in [
    ('ufo-clear-live-age81', 1, [40.18768310546875, 117.093017578125],
     [-1.3763198852539062, 2.0870418548583984], 81, 1),
    ('ufo-clear-spawning-age14', 2, [-40, 120], [4, -2], 14, 1),
    ('cancel-constructs-uninitialized-timer', 1, [0, 120], [4, 2], 100, 0),
    ('already-cancelling-is-unchanged', 3, [0, 120], [4, 2], 81, 1),
    ('outside-right-retires-on-next-visit', 1, [200, 120], [4, 2], 81, 1),
    ('outside-top-retires-on-next-visit', 2, [0, -8], [4, 2], 14, 1),
]:
    initial = {'phase': phase, 'position': position, 'velocity': velocity,
               'age': age, 'timerFlags': timer_flags}
    n = Native(initial)
    result = n.call(0x40c8b0)
    cancelled = n.state()
    passes = []
    for i in range(3):
        before = len(n.events)
        n.call(0x40a020, manager=True)
        passes.append({'visit': i + 1, **n.state(), 'events': n.events[before:]})
    cases.append({'case': name, 'initial': initial, 'cancelResult': result,
                  'cancelled': cancelled, 'passes': passes})

output = ROOT / 'tests/fixtures/bullet-cancel-lifecycle-original.json'
document = {'schema': 'th12-original-bullet-cancel-lifecycle/1',
            'originalExeSha256': EXE_SHA256, 'wholeGameOracle': False,
            'gpuExecuted': False,
            'boundary': 'Explicit synthetic BulletManager pool with one bullet; original40c8b0,40d560,40a020,4099e0 execute.455630 embedded ANM tail returns0. No miss/collision/RNG branch is reached. Cancel effect is explicitly-1.',
            'rows': cases}
output.write_text(json.dumps(document, indent=2) + '\n', encoding='utf-8')
print(json.dumps({'cases': len(cases), 'output': str(output),
                  'cancelAges': [r['cancelled']['timer'][1] for r in cases],
                  'firstVisitAges': [r['passes'][0]['timer'][1] for r in cases]}))
