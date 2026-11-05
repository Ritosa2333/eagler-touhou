"""Original455630 interpolation boundaries after caller-supplied VM scale.

Complete actual ANM binding/update instructions execute. Synthetic bytecode
isolates duration-zero/negative/positive scale interpolation; bullet script42
adds a real retail regression. No game instruction substitutions, GPU, replay or
whole-game claim. The external writes match Laser20's42b089..42b0bc ordering.
"""
import hashlib, json, pathlib, struct, sys
ROOT = pathlib.Path(__file__).resolve().parents[2]
sys.path.insert(0, str(ROOT.parent / 'tools/python'))
sys.path.insert(0, str(ROOT / 'scripts/reverse'))
from native_anm_resource import check_original, EXE_SHA256, install_resource
import pefile
from unicorn import Uc, UC_ARCH_X86, UC_MODE_32
from unicorn.x86_const import *
EXE = ROOT.parent / 'th12/th12.exe'
check_original(EXE)
PE = pefile.PE(str(EXE))
def instruction(op, formats='', values=()):
    out = bytearray(struct.pack('<HHhH', op & 65535, 8 + 4 * len(values), 0, 0))
    for fmt, value in zip(formats, values): out.extend(struct.pack('<' + fmt, value))
    return bytes(out)
def bank(commands):
    header = bytearray(72)
    struct.pack_into('<I', header, 0, 7)
    struct.pack_into('<H', header, 6, 1)
    struct.pack_into('<iI', header, 64, 0, 72)
    return bytes(header) + b''.join(commands)
cases = [(f'duration{duration}', duration, bank([instruction(50, 'ff', (2., 3.)),
           instruction(60, 'iiff', (duration, 0, 1., 1.)), instruction(63), instruction(-1)]), 0, 0)
         for duration in [-2, -1, 0, 1, 2, 4]]
raw_bullet = (ROOT / 'local/retail/bullet.anm').read_bytes()
cases.append(('retail-bullet42-irq2', None, raw_bullet, 42, 2))
records = []
for name, duration, raw, script, interrupt in cases:
    mu = Uc(UC_ARCH_X86, UC_MODE_32)
    for address, size in [(0x400000, (PE.OPTIONAL_HEADER.SizeOfImage + 4095) & ~4095),
                          (0x1000000, 0x10000), (0x2000000, 0x1000000)]: mu.mem_map(address, size)
    mu.mem_write(0x400000, PE.get_memory_mapped_image())
    mu.reg_write(UC_X86_REG_FPCW, 0x37f)
    mu.reg_write(UC_X86_REG_MXCSR, 0x1f80)
    VM, RESOURCE, MANAGER, SP, STOP = 0x2000000, 0x2010000, 0x2500000, 0x100e000, 0x100f000
    def put(at, fmt, *values): mu.mem_write(at, struct.pack('<' + fmt, *values))
    def get(at, fmt): return list(struct.unpack('<' + fmt, mu.mem_read(at, struct.calcsize('<' + fmt))))
    install_resource(mu, raw, RESOURCE)
    put(0x4ce8cc, 'I', MANAGER)
    put(0x4b2ed0, 'f', 1.)
    put(0x4ce568, 'II', 42, 0)
    put(0x4ce560, 'II', 0xbeef, 0)
    def call(address, arguments):
        put(SP, 'I' * (len(arguments) + 1), STOP, *arguments)
        mu.reg_write(UC_X86_REG_ESP, SP)
        if address == 0x454d10: mu.reg_write(UC_X86_REG_ECX, RESOURCE)
        mu.emu_start(address, STOP, count=200000)
        if mu.reg_read(UC_X86_REG_EIP) != STOP: raise RuntimeError(f'Native ANM budget {name}')
    def snapshot(tick, external=None):
        return {'tick': tick, 'externalScale': external, 'scale': get(VM + 0x40, '2f'),
                'flags': get(VM + 0x47c, 'I')[0], 'clock': get(VM + 0x6c, 'i')[0],
                'primary': get(VM + 0x3bc, 'I')[0], 'layer': get(VM + 0x20, 'i')[0],
                'ended': get(VM + 0x3f0, 'I')[0] == 0, 'rng': get(0x4ce568, 'II')}
    call(0x454d10, [VM, script])
    frames = [snapshot(0)]
    if interrupt:
        put(VM + 0x3c4, 'h', interrupt)
        call(0x455630, [VM])
        frames.append(snapshot(1))
    for tick in range(2, 8):
        external = [struct.unpack('<f', struct.pack('<f', (tick + 1) / 16.))[0],
                    struct.unpack('<f', struct.pack('<f', 480. / 14.))[0]]
        put(VM + 0x40, '2f', *external)
        put(VM + 0x47c, 'I', get(VM + 0x47c, 'I')[0] | 8)
        call(0x455630, [VM])
        frames.append(snapshot(tick, external))
    records.append({'name': name, 'duration': duration, 'script': script,
                    'interrupt': interrupt, 'resourceSha256': hashlib.sha256(raw).hexdigest(),
                    'syntheticBankHex': raw.hex() if duration is not None else None,
                    'frames': frames})
output = ROOT / 'tests/fixtures/anm-external-scale-original.json'
output.write_text(json.dumps({'schema': 'th12-original-anm-external-scale/1',
    'originalExeSha256': EXE_SHA256, 'wholeGameOracle': False, 'gpuExecuted': False,
    'scope': 'Complete454d10/454b80/455630, without instruction hooks. Six synthetic '
             'duration boundaries and actual bullet.anm script42 IRQ2; GPU-free ANMFile '
             'tables only. ExternalScale stores precede native update like Laser20.',
    'records': records}, indent=2) + '\n', encoding='utf-8')
print(json.dumps({'output': str(output), 'records': len(records),
                  'scales': {r['name']: [f['scale'] for f in r['frames']] for r in records}}))
