"""Actual437810 closed rectangle hit/graze borders at adjacent float32 values.

Native437153..4371ef computes Player's cached bounds, then the complete437810
query executes. Synthetic Player state1/invulnerability1 avoids the miss scene
through the original branch. No collision instruction, callback or RNG is
replaced. This local mathematical fixture is not a replay/whole-game oracle.
"""
import hashlib, json, pathlib, struct, sys
ROOT = pathlib.Path(__file__).resolve().parents[2]
sys.path.insert(0, str(ROOT.parent / 'tools/python'))
from native_anm_resource import check_original, EXE_SHA256
import pefile
from unicorn import Uc, UC_ARCH_X86, UC_MODE_32
from unicorn.x86_const import *

EXE = ROOT.parent / 'th12/th12.exe'
check_original(EXE)
PE = pefile.PE(str(EXE))
image = PE.get_memory_mapped_image()
mu = Uc(UC_ARCH_X86, UC_MODE_32)
for base, size in [(0x400000, (PE.OPTIONAL_HEADER.SizeOfImage + 4095) & ~4095),
                   (0x1000000, 0x10000), (0x2000000, 0x30000)]:
    mu.mem_map(base, size)
mu.mem_write(0x400000, image)
mu.reg_write(UC_X86_REG_FPCW, 0x37f)
mu.reg_write(UC_X86_REG_MXCSR, 0x1f80)
PLAYER, POSITION, SIZE, SP, STOP = 0x2000000, 0x2010000, 0x2010010, 0x100e000, 0x100f000
put = lambda at, fmt, *v: mu.mem_write(at, struct.pack('<' + fmt, *v))
read = lambda at, fmt: list(struct.unpack('<' + fmt, mu.mem_read(at, struct.calcsize('<' + fmt))))
f32 = lambda value: struct.unpack('<f', struct.pack('<f', value))[0]

def adjacent(value, step):
    if not step:
        return value
    if value == 0:
        return struct.unpack('<f', struct.pack('<I', 1 if step > 0 else 0x80000001))[0]
    bits = struct.unpack('<I', struct.pack('<f', value))[0]
    return struct.unpack('<f', struct.pack('<I', bits + (step if value > 0 else -step)))[0]

hit_sizes = {}
for bullet_type in range(31):
    hit, _, effect = struct.unpack_from('<fii', image, 0xaf344 + bullet_type * 0xd0)
    if effect != 2:
        hit_sizes.setdefault(hit, bullet_type)
rows = []
put(0x4b4514, 'I', PLAYER)
put(0x4b43e4, 'I', 0)
put(PLAYER + 0xa28, 'i', 1)
put(PLAYER + 0xc404, 'i', 1)
put(PLAYER + 0xc414, 'I', 0)
for character in range(3):
    player_hit = read(0x4b31a8 + character * 4, 'f')[0]
    for player in [[0., 120.], [-180.1484375, 44.9375], [171.0078125, 431.125]]:
        put(PLAYER + 0x97c, '3f', *player, 0)
        put(PLAYER + 0x9e4, '3f', player_hit * .5, player_hit * .5, 2)
        mu.reg_write(UC_X86_REG_ESP, SP)
        mu.reg_write(UC_X86_REG_EDI, PLAYER)
        mu.emu_start(0x437153, 0x4371ef, count=1000)
        if mu.reg_read(UC_X86_REG_EIP) != 0x4371ef:
            raise RuntimeError('Actual Player bounds block did not complete')
        bounds = read(PLAYER + 0x9cc, '6f')
        for hit, bullet_type in hit_sizes.items():
            put(SIZE, '2f', hit, hit)
            for boundary, extent in [('hit', hit * .5), ('graze', 24.)]:
                points = [('left', [f32(bounds[0] - extent), player[1]], 0),
                          ('right', [f32(bounds[3] + extent), player[1]], 0),
                          ('top', [player[0], f32(bounds[1] - extent)], 1),
                          ('bottom', [player[0], f32(bounds[4] + extent)], 1)]
                for edge, position, axis in points:
                    for ulp in [-1, 0, 1]:
                        point = list(position)
                        point[axis] = adjacent(point[axis], ulp)
                        put(POSITION, '3f', *point, 0)
                        put(SP, 'I', STOP)
                        mu.reg_write(UC_X86_REG_ESP, SP)
                        mu.reg_write(UC_X86_REG_EAX, SIZE)
                        mu.reg_write(UC_X86_REG_ECX, POSITION)
                        mu.emu_start(0x437810, STOP, count=10000)
                        if mu.reg_read(UC_X86_REG_EIP) != STOP:
                            raise RuntimeError('Actual437810 query did not return')
                        rows.append({'character': character, 'playerHit': player_hit, 'player': player,
                                     'playerBounds': bounds, 'type': bullet_type, 'hitbox': hit,
                                     'boundary': boundary, 'edge': edge, 'ulp': ulp,
                                     'position': point, 'nativeReturn': mu.reg_read(UC_X86_REG_EAX)})
output = ROOT / 'tests/fixtures/bullet-rectangle-boundaries-original.json'
output.write_text(json.dumps({'schema': 'th12-original-bullet-rectangle-boundaries/1',
    'originalExeSha256': EXE_SHA256,
    'samplerSha256': hashlib.sha256(pathlib.Path(__file__).read_bytes()).hexdigest(),
    'wholeGameOracle': False, 'gpuExecuted': False,
    'scope': 'Actual437153..4371ef cached Player rectangle computation and complete437810. '
             'Synthetic Player state1/invulnerability1/flags0/no dialogue; actual435d17 hitbox table '
             'and actual4af344 ordinary bullet hitbox table. Original hit gate returns1 without '
             'calling438370. All four hit/graze borders with adjacent float32s, translated fixed128 '
             'positions and all three character rectangles. No collision/ANM/RNG substitution.',
    'rows': rows}, indent=2) + '\n', encoding='utf-8')
print(json.dumps({'output': str(output), 'cases': len(rows), 'hitboxSizes': list(hit_sizes),
                  'returnCounts': {code: sum(row['nativeReturn'] == code for row in rows) for code in [0, 1, 2]}}))
