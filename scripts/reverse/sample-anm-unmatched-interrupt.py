"""Actual455630 unmatched IRQ control flow and rate-scaled common tail.

Synthetic sprite-free ANM banks are local inputs, not retail/replay bytes.
454d10,455630 and464a20 execute unchanged; no gameplay or RNG hooks.
"""
import hashlib, json, pathlib, struct, sys
ROOT = pathlib.Path(__file__).resolve().parents[2]
sys.path.insert(0, str(ROOT.parent / 'tools/python'))
import pefile
from unicorn import Uc, UC_ARCH_X86, UC_MODE_32, UC_HOOK_CODE
from unicorn.x86_const import UC_X86_REG_ESP, UC_X86_REG_EIP, UC_X86_REG_ECX, UC_X86_REG_ESI, UC_X86_REG_FPCW
from native_anm_resource import check_original, EXE_SHA256, install_resource
exe = ROOT.parent / 'th12/th12.exe'; check_original(exe)
pe = pefile.PE(str(exe)); image = pe.get_memory_mapped_image()
def cmd(op, fmt='', values=(), time=0, mask=0):
    args = struct.pack('<' + fmt, *values)
    return struct.pack('<HHhH', op & 65535, len(args) + 8, time, mask) + args
def bank(script):
    h = bytearray(72); struct.pack_into('<I', h, 0, 7)
    struct.pack_into('<H', h, 6, 1); struct.pack_into('<iI', h, 64, 0, 72)
    return bytes(h) + script
records = []
for handler in ['none', 'other-only', 'fallback', 'matching']:
 for delta in [.5, 1., 2.]:
  for state in ['running', 'held', 'paused']:
   for fixed_rate in [False, True]:
    setup = cmd(50, 'ff', [1.5, 2.5]) + cmd(54, 'ff', [.125, -.25]) \
      + cmd(60, 'iiff', [8, 0, 3.5, 4.5]) + cmd(53, 'fff', [0, 0, .125]) \
      + cmd(70, 'f', [.0625]) + cmd(71, 'f', [-.03125])
    if state == 'held': setup += cmd(63)
    script = setup + cmd(7, 'ff', [10004, 42.], time=1, mask=1) \
      + cmd(50, 'ff', [1.2, 1.3], time=1) + cmd(63, time=1)
    if handler != 'none':
      irq = 7 if handler == 'other-only' else -1 if handler == 'fallback' else 2
      script += cmd(64, 'i', [irq], time=5) + cmd(7, 'ff', [10004, 99.], time=5, mask=1) \
        + cmd(50, 'ff', [9., 10.], time=5) + cmd(63, time=5)
    script += cmd(-1, time=100)
    raw = bank(script); mu = Uc(UC_ARCH_X86, UC_MODE_32)
    mu.mem_map(0x400000, (pe.OPTIONAL_HEADER.SizeOfImage + 4095) & ~4095)
    mu.mem_write(0x400000, image); mu.mem_map(0x1000000, 0x10000); mu.mem_map(0x2000000, 0x20000)
    obj, res, manager, stack, stop = 0x2000000, 0x2001000, 0x2003000, 0x100e000, 0x100f000
    put = lambda at, fmt, *v: mu.mem_write(at, struct.pack('<' + fmt, *v))
    read = lambda at, fmt: list(struct.unpack('<' + fmt, mu.mem_read(at, struct.calcsize('<' + fmt))))
    resource = install_resource(mu, raw, res); script_base = read(resource['scripts'], 'I')[0]
    put(0x4ce8cc, 'I', manager); put(0x4b2ed0, 'f', delta)
    put(0x4ce568, 'II', 0x1234, 0); put(0x4ce560, 'II', 0xbeef, 0)
    mu.reg_write(UC_X86_REG_FPCW, 0x37f)
    calls = []; phase = 'bind'
    def observe(v, address, size, unused):
      if v.reg_read(UC_X86_REG_ESI) == obj + 0x68:
        esp = v.reg_read(UC_X86_REG_ESP)
        calls.append({'phase': phase, 'caller': hex(read(esp, 'I')[0]),
          'delta': read(esp + 4, 'f')[0], 'gameRate': read(0x4b2ed0, 'f')[0],
          'timeBefore': read(obj + 0x70, 'f')[0]})
    mu.hook_add(UC_HOOK_CODE, observe, begin=0x464a20, end=0x464a20)
    def call(entry, args, ecx=0):
      put(stack, 'I' * (len(args) + 1), stop, *args)
      mu.reg_write(UC_X86_REG_ESP, stack); mu.reg_write(UC_X86_REG_ECX, ecx)
      mu.emu_start(entry, stop, count=300000)
      if mu.reg_read(UC_X86_REG_EIP) != stop: raise RuntimeError('Native ANM budget exceeded')
    def snapshot():
      ptr = read(obj + 0x3f0, 'I')[0]; saved = read(obj + 0x3dc, 'I')[0]
      return {'clock': read(obj + 0x6c, 'i')[0], 'time': read(obj + 0x70, 'f')[0],
        'pc': ptr - script_base, 'savedPC': saved - script_base if saved else None,
        'savedTime': read(obj + 0x3d0, 'f')[0], 'pendingInterrupt': read(obj + 0x3c4, 'h')[0],
        'flags': read(obj + 0x47c, 'I')[0], 'scale': read(obj + 0x40, '2f'),
        'rotation': read(obj + 0x24, '3f'), 'uv': read(obj + 0x60, '2f'),
        'float0': read(obj + 0x40c, 'f')[0], 'gameRate': read(0x4b2ed0, 'f')[0],
        'gameSeedCounter': read(0x4ce568, 'II'), 'displaySeedCounter': read(0x4ce560, 'II')}
    call(0x454d10, [obj, 0], res)
    # The flag is an explicit initial input to subsequent ticks, so the
    # birth tick retains its original global rate before this external write.
    if fixed_rate: put(obj + 0x47c, 'I', read(obj + 0x47c, 'I')[0] | 0x80000000)
    frames = [snapshot()]; actions = []
    for tick in range(6):
      action = {'interrupt': 2 if tick % 2 == 0 else 0, 'paused': state == 'paused' and tick == 0}
      flags = read(obj + 0x47c, 'I')[0]
      put(obj + 0x47c, 'I', (flags | 0x40000) if action['paused'] else (flags & ~0x40000))
      if action['interrupt']: put(obj + 0x3c4, 'h', action['interrupt'])
      phase = 'tick' + str(tick); call(0x455630, [obj]); actions.append(action); frames.append(snapshot())
    records.append({'handler': handler, 'delta': delta, 'initialState': state, 'fixedRate': fixed_rate,
      'syntheticBankHex': raw.hex(), 'actions': actions, 'frames': frames, 'timerSubtractCalls': calls})
out = ROOT / 'tests/fixtures/anm-unmatched-interrupt-original.json'
out.write_text(json.dumps({'schema': 'th12-original-anm-unmatched-interrupt/1',
  'originalExeSha256': EXE_SHA256, 'samplerSha256': hashlib.sha256(pathlib.Path(__file__).read_bytes()).hexdigest(),
  'wholeGameOracle': False, 'wholePresentationOracle': False, 'gpuExecuted': False,
  'timerFastRange': {'lower': struct.unpack_from('<d', image, 0x4a3db0 - 0x400000)[0],
    'upper': struct.unpack_from('<f', image, 0x4a3dac - 0x400000)[0]},
  'scope': 'Actual454d10 reset/birth tick and455630/464a20 execute synthetic sprite-free ANM. '
    'No instruction hooks replace logic or RNG. Controlled per-tick IRQ/40000 pause flag and the '
    'post-birth80000000 fixed-rate flag are explicit inputs. '
    'Cases cover absent/mismatched/fallback/matched handlers, normal/held/paused execution, gameRate0.5/1/2, '
    '80000000 fixed-rate flag, scale interpolation and rotation/UV velocity common tail. '
    'GPU, full scene, replay acceptance and retail bytes are excluded; no accepted golden is modified.',
  'records': records}, indent=2) + '\n', encoding='utf8')
print(json.dumps({'path': str(out), 'cases': len(records), 'snapshots': sum(len(r['frames']) for r in records)}))
