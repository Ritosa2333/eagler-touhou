"""Actual Curve ETEX400 lifetime and dispatcher/callback order.

Ten controlled dispatcher/callback cases and three actual ECL611/Laser20
lifecycles. No original instruction, math, cancellation or RNG is replaced.
The callback cases reproduce only the documented Curve update's dispatch
loop; the lifecycle cases execute its real manager and original ANM.
"""
import gc, hashlib, json, math, pathlib, struct, sys
ROOT = pathlib.Path(__file__).resolve().parents[2]
sys.path.insert(0, str(ROOT.parent / 'tools/python'))
import pefile
from unicorn import Uc, UC_ARCH_X86, UC_MODE_32
from unicorn.x86_const import *
from native_anm_resource import check_original, EXE_SHA256
exe = ROOT.parent / 'th12/th12.exe'; check_original(exe)
pe = pefile.PE(str(exe))
rows = []
cases = [
 (2, 0, 1., 0, False), (2, 1, 1., 0, False),
 (2, 2, .5, 1, False), (2, 2, 2., -1, False),
 (2, 120, 1., 0, True), (2, 5, .5, 0, True),
 (2, 5, 2., 0, True), (2, -1, 1., 1, False),
 (0, 2, 1., 1, True), (1, 2, 1., -1, True),
]
for kind, duration, rate, flags, concurrent in cases:
 v = Uc(UC_ARCH_X86, UC_MODE_32)
 for address, count in [(0x400000, (pe.OPTIONAL_HEADER.SizeOfImage + 4095) & ~4095),
                        (0x1000000, 0x10000), (0x2000000, 0x10000)]:
  v.mem_map(address, count)
 v.mem_write(0x400000, pe.get_memory_mapped_image())
 v.reg_write(UC_X86_REG_FPCW, 0x37f); v.reg_write(UC_X86_REG_MXCSR, 0x1f80)
 put = lambda a, f, *vs: v.mem_write(a, struct.pack('<' + f, *vs))
 get = lambda a, f: list(struct.unpack('<' + f, v.mem_read(a, struct.calcsize('<' + f))))
 obj, sp, stop = 0x2000000, 0x100e000, 0x100f000
 put(0x4b2ed0, 'f', rate); put(obj + 0x68, 'f', .33); put(obj + 0x74, 'f', 6)
 angle = get(obj + 0x68, 'f')[0]
 put(obj + 0x5c, '3f', math.cos(angle) * 6, math.sin(angle) * 6, 0)
 put(obj + 0x630, 'i', -1)
 offset = {0: 0x484, 1: 0x4ac, 2: 0x47c}[kind]
 put(obj + offset, 'ffiiIi', -999999, -999999, duration, flags, 0x400, 0)
 if kind == 2:
  put(obj + offset + 24, 'ffiiIi', -0.083333336, 0.05235988, 60, -999999, 8, concurrent)
  put(obj + offset + 48, 'ffiiIi', 0.083333336, 0, 60, -999999, 8, True)
 def call(address):
  put(sp, 'I', stop); v.reg_write(UC_X86_REG_ESP, sp); v.reg_write(UC_X86_REG_ECX, obj)
  v.emu_start(address, stop, count=100000)
  if v.reg_read(UC_X86_REG_EIP) != stop: raise RuntimeError('Native callback instruction budget')
  return v.reg_read(UC_X86_REG_EAX)
 def capture():
  return {'active': get(obj + 0x430, 'I')[0], 'cursor': get(obj + 0x42c, 'i')[0],
    'offscreenGate': get(obj + 0x2f4, 'iif'), 'storedFlags': get(obj + 0x31c, 'i')[0],
    'polarTimer': get(obj + 0xec, 'iif'), 'angle': get(obj + 0x68, 'f')[0],
    'speed': get(obj + 0x74, 'f')[0], 'velocity': get(obj + 0x5c, '3f')}
 frames = [capture()]
 for tick in range(132 if duration == 120 else 16):
  passes = []
  while True:
   dispatcher = {0: 0x428b00, 1: 0x42c1a0, 2: 0x42d030}[kind]
   call(dispatcher)
   polar = call(0x42d800 if kind == 2 else 0x427e30) if get(obj + 0x430, 'I')[0] & 8 else 0
   gate = call(0x42d980) if kind == 2 and get(obj + 0x430, 'I')[0] & 0x400 else 0
   passes.append({'polarReturn': polar, 'gateReturn': gate, 'state': capture()})
   if not polar and not gate: break
   if len(passes) > 4: raise RuntimeError('Unexpected dispatch repeat')
  row = capture(); row['passes'] = passes; frames.append(row)
 rows.append({'kind': kind, 'duration': duration, 'rate': rate, 'flags': flags,
              'concurrent': concurrent, 'frames': frames})

PROVIDER = ROOT / 'scripts/reverse/native-enemy-provider.py'
BOOTSTRAP = PROVIDER.read_text(encoding='utf8').split('if args.routine not in routines:')[0]
helpers = (ROOT / 'scripts/reverse/sample-laser-phase-clear.py').read_text(encoding='utf8').split('records=[]')[0]
exec(compile(helpers, str(ROOT / 'scripts/reverse/sample-laser-phase-clear.py'), 'exec'))
lifecycles = []
for duration in [None, 10, 120]:
 saved = sys.argv; sys.argv = [str(PROVIDER), '--frames', '0', '--seed', '1234']
 ns = {'__file__': str(PROVIDER), '__name__': 'curve_offscreen_lifetime_original'}
 try: exec(compile(BOOTSTRAP, str(PROVIDER), 'exec'), ns)
 finally: sys.argv = saved
 v, put, get, call = (ns[k] for k in ['v', 'put', 'get', 'call'])
 lm = call(0x4281c0)
 code = [ins(300, [floating(190), floating(200)]), ins(500, [word(0)]),
  ins(502, [word(0), word(0), word(13)]), ins(504, [word(0), floating(0), floating(0)]),
  ins(505, [word(0), floating(6), floating(6)]),
  ins(600, [word(0), floating(-1), floating(-1), floating(-1), floating(16)]),
  ins(601, [word(0), word(42), word(-1), word(-1), word(-1), word(0)]),
  ins(508, [word(0), word(-1), word(-1)])]
 index = 0
 if duration is not None:
  code.append(ins(509, [word(0), word(index), word(0), word(0x400), word(duration), word(0),
    floating(-999999), floating(-999999)])); index += 1
 code += [ins(509, [word(0), word(index), word(1), word(8), word(60), word(-999999),
    floating(-0.083333336), floating(0.05235988)]),
  ins(509, [word(0), word(index + 1), word(1), word(8), word(60), word(-999999),
    floating(0.083333336), floating(0)]), ins(611, [word(0)]), ins(83, [word(100000)])]
 raw, header = program(code); base = 0x4300000; v.mem_write(base, raw)
 ns['routines']['ClearProbe'] = (base + 40, base + header)
 put(ns['PROGRAM'] + 8, 'I', len(ns['routines']))
 for index, (_, (text, routine)) in enumerate(sorted(ns['routines'].items())):
  put(ns['PROGRAM'] + 0x100 + index * 8, 'II', text, routine)
 request = 0x2070000; put(request, 'fffiiiII', 0, 0, 0, 0, 0, 10000, 0, 0)
 call(0x412990, base + 40, request)
 def capture_lifecycle():
  obj = get(lm + 0x18, 'I')[0]
  if not obj: return {'count': 0, 'rng': list(get(0x4ce568, 'II'))}
  history = get(obj + 0xf9c, 'I')[0]; count = get(obj + 0x470, 'i')[0]
  return {'count': 1, 'phase': get(obj + 0xc, 'i')[0], 'age': list(get(obj + 0x14, 'iif')),
    'active': get(obj + 0x430, 'I')[0], 'cursor': get(obj + 0x42c, 'i')[0],
    'position': list(get(obj + 0x50, '3f')), 'velocity': list(get(obj + 0x5c, '3f')),
    'angle': get(obj + 0x68, 'f')[0], 'speed': get(obj + 0x74, 'f')[0],
    'offscreenGate': list(get(obj + 0x2f4, 'iif')),
    'offscreenGrace': list(get(obj + 0x438, 'iif')),
    'polarTimer': list(get(obj + 0xec, 'iif')),
    'nodes': [list(get(history + i * 20, '5f')) for i in range(count)],
    'rng': list(get(0x4ce568, 'II'))}
 frames = [capture_lifecycle()]
 for tick in range(132):
  v.reg_write(UC_X86_REG_EDI, lm); call(0x428270)
  frames.append(capture_lifecycle())
 lifecycles.append({'duration': duration, 'position': [190, 200, 0], 'nodeCount': 42,
                   'syntheticEclHex': raw.hex(), 'frames': frames})
 gc.collect()
out = ROOT / 'tests/fixtures/laser-offscreen-gate-original.json'
out.write_text(json.dumps({'schema': 'th12-original-laser-offscreen-gate/1',
 'originalExeSha256': EXE_SHA256, 'samplerSha256': hashlib.sha256(pathlib.Path(__file__).read_bytes()).hexdigest(),
 'wholeGameOracle': False, 'gpuExecuted': False,
 'scope': 'Ten synthetic native dispatcher/callback cases execute original42d030/428b00/42c1a0, '
  '42d800/427e30 and42d980, reproducing only original Curve update dispatch-repeat order; no full '
  'movement, draw, collision or manager state is claimed for these. Three synthetic original '
  'Enemy/ECL611 constructors and full Laser20 lifecycle cases execute actual42c770, original ANM '
  'and original math/RNG; passive Player hurt/audio and item request host boundaries remain explicit. '
  'The42-node curve uses the same ETEX8 values as Boss2 and starts at an explicit controlled position. '
  'No accepted fixture or replay input is changed.', 'callbackCases': rows, 'lifecycles': lifecycles},
 indent=2) + '\n', encoding='utf8')
print(json.dumps({'output': str(out), 'cases': len(rows) + len(lifecycles),
 'callbackSnapshots': sum(len(r['frames']) for r in rows),
 'lifecycleSnapshots': sum(len(r['frames']) for r in lifecycles),
 'lifecycleRetirement': [{'duration': r['duration'], 'firstMissingTick': next((i - 1 for i, f in enumerate(r['frames']) if not f['count']), None),
   'tick60': r['frames'][61].get('position')} for r in lifecycles]}))
