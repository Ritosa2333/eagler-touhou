"""Original focus-marker creation, tracking and retirement, without GPU calls.

Executes the complete 4364f0 movement callback with no options and zero input
movement. The player positions and elapsed timer are explicit host inputs.
454d10/4615a0/461970 and the primary scheduler execute unchanged retail code.
This fixture verifies presentation logic, not whole-game or pixel equivalence.
"""
import pathlib, sys, struct, json, hashlib
ROOT = pathlib.Path(__file__).resolve().parents[2]
sys.path.insert(0, str(ROOT.parent / 'tools/python'))
import pefile
from unicorn import Uc, UC_ARCH_X86, UC_MODE_32
from unicorn.x86_const import *
from native_anm_resource import check_original, EXE_SHA256, install_resource
exe = ROOT.parent / 'th12/th12.exe'
check_original(exe)
pe = pefile.PE(str(exe))
mu = Uc(UC_ARCH_X86, UC_MODE_32)
for base, size in [(0x400000, (pe.OPTIONAL_HEADER.SizeOfImage+4095)&~4095),
                   (0x1000000, 0x10000), (0x2000000, 0x2000000)]:
    mu.mem_map(base, size)
mu.mem_write(0x400000, pe.get_memory_mapped_image())
MANAGER, RESOURCE, PLAYER, ENEMY, BULLET, SP, STOP = (
    0x2000000, 0x3400000, 0x2a00000, 0x2b00000, 0x2c00000, 0x100e000, 0x100f000)
put = lambda at, fmt, *values: mu.mem_write(at, struct.pack('<'+fmt, *values))
read = lambda at, fmt: list(struct.unpack('<'+fmt, mu.mem_read(at, struct.calcsize('<'+fmt))))
raw = (ROOT / 'local/retail/bullet.anm').read_bytes()
install_resource(mu, raw, RESOURCE, 0)
install_resource(mu, (ROOT / 'local/retail/pl00.anm').read_bytes(), 0x3700000, 4)
put(PLAYER+0x10, 'I', 0x3700000)
put(0x4ce8cc, 'I', MANAGER)
put(0x4b43dc, 'I', ENEMY)
put(ENEMY+0x70, 'I', 1)
put(0x4b43c8, 'I', BULLET)
put(BULLET+0x4debdc, 'I', RESOURCE)
put(0x4b2ed0, 'f', 1)
put(0x4ce568, 'II', 0x1234, 0)
put(0x4ce560, 'II', 0xbeef, 0)
put(PLAYER+0x825c, 'f', 1)
mu.reg_write(UC_X86_REG_FPCW, 0x37f)
mu.reg_write(UC_X86_REG_FPSW, 0)
mu.reg_write(UC_X86_REG_FPTAG, 0xffff)
mu.reg_write(UC_X86_REG_MXCSR, 0x1f80)


def call(address, registers):
    put(SP, 'I', STOP)
    mu.reg_write(UC_X86_REG_ESP, SP)
    for reg, value in registers.items():
        mu.reg_write(reg, value)
    try:
        mu.emu_start(address, STOP, count=1000000)
    except Exception as error:
        raise RuntimeError(f'Native {address:08x}, frame{frame}, EIP={mu.reg_read(UC_X86_REG_EIP):08x}') from error
    if mu.reg_read(UC_X86_REG_EIP) != STOP:
        raise RuntimeError('Native focus callback exceeded its instruction budget')


def nodes():
    result = []
    head = read(MANAGER+0x8856b8, 'I')[0]
    while head:
        vm, head = read(head, 'II')
        result.append({'script': read(vm+0x3ea, 'h')[0],
            'clock': read(vm+0x6c, 'i')[0], 'pendingInterrupt': read(vm+0x3c4, 'h')[0],
            'scale': read(vm+0x40, '2f'), 'position': read(vm+0x424, '3f'),
            'engine': read(vm+0x430, '3f'), 'offset': read(vm+0x43c, '3f'),
            'rotation': read(vm+0x24, '3f'), 'flags': read(vm+0x47c, 'I')[0],
            'color': read(vm+0x3bc, 'I')[0], 'layer': read(vm+0x20, 'i')[0]})
        if len(result) > 20:
            raise RuntimeError('Unexpected focus animation list size')
    return result


frames = []
for frame in range(64):
    held = 8 if frame < 29 or 32 <= frame < 48 else 0
    x, y = -30+frame/8, 400-frame/4
    put(0x4d49d0, 'I', held)
    put(PLAYER+0xa48, 'i', frame)
    put(PLAYER+0x988, 'ii', int(x*128), int(y*128))
    call(0x4364f0, {UC_X86_REG_EDI: PLAYER})
    call(0x460fe0, {UC_X86_REG_EDI: MANAGER})
    frames.append({'frame': frame+1, 'held': held, 'playerFixed': [int(x*128), int(y*128)],
                   'nodes': nodes(), 'rng': read(0x4ce568, 'II'),
                   'focus': read(PLAYER+0xc598, 'I')[0],
                   'handle': read(PLAYER+0x8258, 'I')[0],
                   'movedFixed': read(PLAYER+0x988, 'ii')})
out = ROOT / 'tests/fixtures/player-focus-original.json'
out.write_text(json.dumps({'schema': 'th12-original-player-focus/1',
    'originalExeSha256': EXE_SHA256, 'resourceSha256': hashlib.sha256(raw).hexdigest(),
    'resource': 'bullet.anm', 'wholeGameOracle': False, 'gpuExecuted': False,
    'provider': 'Actual4364f0,4615a0,454d10,461970,455630,460fe0; no logic hooks',
    'frames': frames}, indent=2)+'\n', encoding='utf8')
print(json.dumps({'output': str(out), 'frames': len(frames)}))
