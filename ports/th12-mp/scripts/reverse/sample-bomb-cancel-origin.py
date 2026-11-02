"""Actual409220 Sanae B clear geometry at the th12_14 input9288 boundary.

Constructed manager storage and one physical bullet reproduce the recorded
pre-Bullet21 position. Original409220,40caa0,40c8b0,414f40 and4286f0 execute
without instruction substitutions. The explicit cancel-effect script -1 and
empty Enemy/Laser lists isolate the circle query; this is not a replay oracle.
"""
import json, pathlib, struct, sys
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
MANAGER, BONUS, ENEMIES, BOMB, LASERS = 0x2000000, 0x2900000, 0x2901000, 0x2902000, 0x2903000
BULLET, SP, STOP = MANAGER + 0x64, 0x100e000, 0x100f000
mu = Uc(UC_ARCH_X86, UC_MODE_32)
for address, size in [(0x400000, (PE.OPTIONAL_HEADER.SizeOfImage + 4095) & ~4095),
                      (0x1000000, 0x10000), (MANAGER, 0xc00000)]:
    mu.mem_map(address, size)
mu.mem_write(0x400000, PE.get_memory_mapped_image())
mu.reg_write(UC_X86_REG_FPCW, 0x37f)
mu.reg_write(UC_X86_REG_MXCSR, 0x1f80)
def put(at, fmt, *values): mu.mem_write(at, struct.pack('<' + fmt, *values))
def get(at, fmt): return list(struct.unpack('<' + fmt, mu.mem_read(at, struct.calcsize('<' + fmt))))
put(0x4b2ed0, 'f', 1)
for at, value in [(0x4b43c8, MANAGER), (0x4b43cc, BONUS), (0x4b43dc, ENEMIES), (0x4b44f4, LASERS)]:
    put(at, 'I', value)
put(BONUS + 0x7c, 'I', 1)  # Active ordinary spell: no cancelled-bullet item conversion.
clear_calls = []
def observe(uc, at, size, user):
    sp = mu.reg_read(UC_X86_REG_ESP)
    clear_calls.append({'entry': hex(at), 'center': get(mu.reg_read(UC_X86_REG_EBX if at == 0x40caa0 else UC_X86_REG_ESI), '3f'),
                        'arguments': get(sp + 4, 'fii')})
for at in [0x40caa0, 0x4286f0]: mu.hook_add(UC_HOOK_CODE, observe, begin=at, end=at)
rows = []
position = [5.678585052490234, 385.36041259765625]
for origin_name, center in [('original-bomb-before-movement-origin', [0.8671875, 423.]),
                            ('former-core-moved-origin', [0.8671875, 418.5])]:
    for age in [60, 61, 62]:
        mu.mem_write(BULLET, bytes(0x9f8))
        put(BULLET + 0x532, 'H', 1)
        put(BULLET + 0x524, 'i', -1)
        put(BULLET + 0x4bc, '3f', *position, 0)
        put(BULLET + 0x4dc, '2f', 4, 4)
        put(BOMB + 0x18, 'if', age, age)
        put(BOMB + 0x508, '3f', *center, 0)
        put(SP, 'I', STOP)
        mu.reg_write(UC_X86_REG_ESP, SP)
        mu.reg_write(UC_X86_REG_EAX, BOMB)
        clear_calls.clear()
        mu.emu_start(0x409220, STOP, count=1000000)
        if mu.reg_read(UC_X86_REG_EIP) != STOP or len(clear_calls) != 2:
            raise RuntimeError('Original Bomb circle budget or missing bullet/laser queries')
        rows.append({'case': origin_name, 'age': age, 'position': position, 'hitbox': 4,
                     'radius': clear_calls[0]['arguments'][0], 'center': clear_calls[0]['center'][:2],
                     'cancelled': get(BULLET + 0x532, 'H')[0] == 3, 'clearCalls': list(clear_calls)})
output = ROOT / 'tests/fixtures/bomb-cancel-origin-original.json'
output.write_text(json.dumps({'schema': 'th12-original-bomb-cancel-origin/1',
    'originalExeSha256': EXE_SHA256, 'wholeGameOracle': False, 'gpuExecuted': False,
    'scope': 'Original409220 full query with one synthetic phase1 bullet, countdown0, script-1, hitbox4, empty Enemy/Laser lists, ordinary active spell. The position comes from the pre-Bullet21 th12_14 input9288 slot1249 trace; centers are explicit constructed conditions. This fixture proves clear geometry only, not Bomb birth order or RNG acceptance.',
    'rows': rows}, indent=2) + '\n', encoding='utf-8')
print(json.dumps({'output': str(output), 'rows': [(r['case'], r['age'], r['radius'], r['cancelled']) for r in rows]}))
