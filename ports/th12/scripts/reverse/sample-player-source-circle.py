"""Original 439ed0 circle boundaries with unrounded x87 coordinate deltas.

Runs the complete original damage query, with all friendly slots inactive and
the original inactive Bomb query. No instruction hooks replace game behavior.
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
PLAYER, POSITION, TARGET, SIZE, BOMB = 0x2000000, 0x2010000, 0x2010100, 0x2010200, 0x2011000
SP, STOP = 0x100d000, 0x100f000
cases = [
    ((-6.520016193389893, 120.230712890625), (13.696134567260742, 100.93021392822266), 27.94999122619629),
    ((-14.597220420837402, 115.62703704833984), (4.8580780029296875, 108.17620849609375), 20.83323097229004),
    ((-18.39908218383789, 130.953857421875), (19.352602005004883, 149.9247589111328), 42.25026321411133),
    ((11.56931209564209, 129.0081787109375), (-6.1008758544921875, 118.42350769042969), 20.597835540771484),
    ((18.841623306274414, 126.1375503540039), (-17.883052825927734, 141.769775390625), 39.91326141357422),
    ((13.561982154846191, 106.46549987792969), (-17.504558563232422, 105.40715789794922), 31.084562301635742),
    ((4.977455139160156, 126.79623413085938), (-5.533365726470947, 111.5255126953125), 18.538400650024414),
    ((-4.586094379425049, 130.61993408203125), (-14.926565170288086, 127.0465316772461), 10.940500259399414),
    ((0, 120), (12, 120), 12),
    ((0, 120), (12.000001, 120), 12),
]
rows = []
for source_position, target_position, radius in cases:
    mu = Uc(UC_ARCH_X86, UC_MODE_32)
    for address, size in [(0x400000, (PE.OPTIONAL_HEADER.SizeOfImage + 4095) & ~4095),
                          (0x1000000, 0x10000), (PLAYER, 0x20000)]:
        mu.mem_map(address, size)
    mu.mem_write(0x400000, PE.get_memory_mapped_image())
    mu.reg_write(UC_X86_REG_FPCW, 0x37f)
    mu.reg_write(UC_X86_REG_MXCSR, 0x1f80)
    def put(at, fmt, *values): mu.mem_write(at, struct.pack('<' + fmt, *values))
    def read(at, fmt): return list(struct.unpack('<' + fmt, mu.mem_read(at, struct.calcsize('<' + fmt))))
    put(0x4b4514, 'I', PLAYER)
    put(0x4b43c4, 'I', BOMB)
    put(0x4b0c44, 'I', 0)
    put(PLAYER + 0xa34, 'I', 1)
    put(POSITION, '3f', *source_position, 0)
    put(TARGET, '3f', *target_position, 0)
    put(SIZE, '2f', 1, 1)
    put(SP, 'IIffii', STOP, POSITION, radius, 0, 15, 4)
    mu.reg_write(UC_X86_REG_ESP, SP)
    mu.reg_write(UC_X86_REG_EAX, PLAYER)
    mu.emu_start(0x4390f0, STOP, count=100000)
    if mu.reg_read(UC_X86_REG_EIP) != STOP: raise RuntimeError('Circle constructor budget')
    distances = []
    def capture(uc, at, size, user):
        distances.append(read(mu.reg_read(UC_X86_REG_ESP) + 0x2c, 'f')[0])
    mu.hook_add(UC_HOOK_CODE, capture, begin=0x43a3c4, end=0x43a3c4)
    put(SP, 'III', STOP, TARGET, SIZE)
    mu.reg_write(UC_X86_REG_ESP, SP)
    mu.emu_start(0x439ed0, STOP, count=100000)
    if mu.reg_read(UC_X86_REG_EIP) != STOP or len(distances) != 1:
        raise RuntimeError('Complete damage query budget / circle capture')
    source = PLAYER + 0x8988
    rows.append({'source': read(source + 0x18, '2f'), 'target': read(TARGET, '2f'),
                 'radius': read(source, 'f')[0], 'distanceSquared': distances[0],
                 'damage': mu.reg_read(UC_X86_REG_EAX), 'accumulated': read(source + 0x64, 'i')[0],
                 'scoreStored': read(0x4b0c44, 'i')[0]})
output = ROOT / 'tests/fixtures/player-source-circle-original.json'
output.write_text(json.dumps({'schema': 'th12-original-player-source-circle/1',
    'originalExeSha256': EXE_SHA256, 'wholeGameOracle': False, 'gpuExecuted': False,
    'boundary': 'Actual4390f0 birth and complete439ed0 query including inactive406de0 Bomb; all256 friendly slots inactive,128 sources with only slot0 active. Read-only hook43a3c4 captures the float distance store; no game instruction substitutions.',
    'rows': rows}, indent=2) + '\n', encoding='utf-8')
print(json.dumps({'cases': len(rows), 'damages': [r['damage'] for r in rows], 'output': str(output)}))
