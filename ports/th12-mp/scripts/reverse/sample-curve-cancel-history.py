"""Actual Curve circle/rectangle cancellation and subsequent history update.

Original ECL611 factory/ANM/ETEX200 initialize each object. The eight history
nodes are explicit synthetic state input, not candidate or replay geometry.
Original42df00/42d9c0,42c770 and Primary27 run unchanged. No game rule or RNG
instruction is replaced; pointItem requests use the declared passive host.
"""
import gc, hashlib, json, pathlib, struct, sys
ROOT = pathlib.Path(__file__).resolve().parents[2]
sys.path.insert(0, str(ROOT.parent / 'tools/python'))
PROVIDER = ROOT / 'scripts/reverse/native-enemy-provider.py'
BOOTSTRAP = PROVIDER.read_text(encoding='utf8').split('if args.routine not in routines:')[0]
helpers = (ROOT / 'scripts/reverse/sample-laser-phase-clear.py').read_text(encoding='utf8').split('records=[]')[0]
exec(compile(helpers, str(ROOT / 'scripts/reverse/sample-laser-phase-clear.py'), 'exec'))
from unicorn import UC_HOOK_CODE
from unicorn.x86_const import UC_X86_REG_ECX, UC_X86_REG_EDI, UC_X86_REG_ESP

patterns = {
 'leading-prefix-with-internal-hit': [0, 4, 24, 32, 8, 12, 40, 48],
 'internal-hit-disconnects-old-head': [40, 32, 8, 12, 24, 48, 56, 64],
 'leading-prefix-only': [0, 4, 24, 32, 40, 48, 56, 64],
}
variants = [
 ('convert-unprotected', 1, 0, False), ('no-convert-unprotected', 0, 1, False),
 ('protected-gated', 1, 1, True), ('protected-bypass', 1, 0, True),
 ('no-convert-no-force', 0, 0, False),
]
records = []
for method in ['circle', 'rectangle']:
 for pattern, xs in patterns.items():
  for variant, convert, force, protected in variants:
   saved = sys.argv
   sys.argv = [str(PROVIDER), '--frames', '0', '--seed', '1234', '--native-d3dx-vec2']
   ns = {'__file__': str(PROVIDER), '__name__': 'original_curve_cancel_history'}
   try: exec(compile(BOOTSTRAP, str(PROVIDER), 'exec'), ns)
   finally: sys.argv = saved
   v, put, get, call = (ns[k] for k in ['v', 'put', 'get', 'call'])
   lm = call(0x4281c0)
   code = [ins(300, [floating(0), floating(100)]), ins(500, [word(0)]),
     ins(502, [word(0), word(0), word(6)]), ins(505, [word(0), floating(3), floating(3)]),
     ins(504, [word(0), floating(0), floating(0)]),
     ins(600, [word(0), floating(-1), floating(-1), floating(-1), floating(16)]),
     ins(601, [word(0), word(8), word(-1), word(-1), word(-1), word(0)]),
     ins(508, [word(0), word(-1), word(-1)])]
   if protected:
    code.append(ins(509, [word(0), word(0), word(0), word(0x200), word(5), word(-999999),
                         floating(-999999), floating(-999999)]))
   code += [ins(611, [word(0)]), ins(83, [word(100000)])]
   raw, header = program(code); base = 0x4300000; v.mem_write(base, raw)
   ns['routines']['ClearProbe'] = (base + 40, base + header)
   put(ns['PROGRAM'] + 8, 'I', len(ns['routines']))
   for index, (_, (text, routine)) in enumerate(sorted(ns['routines'].items())):
    put(ns['PROGRAM'] + 0x100 + index * 8, 'II', text, routine)
   request = 0x2070000; put(request, 'fffiiiII', 0, 0, 0, 0, 0, 10000, 0, 0)
   call(0x412990, base + 40, request)
   v.reg_write(UC_X86_REG_EDI, lm); call(0x428270)
   obj = get(lm + 0x18, 'I')[0]; assert get(obj, 'I')[0] == 0x4a06ac
   history = get(obj + 0xf9c, 'I')[0]
   input_nodes = [[x, 100., 0., .2 + .05 * index, 1.5 + .5 * index] for index, x in enumerate(xs)]
   for index, node in enumerate(input_nodes): put(history + index * 20, '5f', *node)
   put(obj + 0x50, '3f', xs[0], 100, 0)
   center, dimensions = 0x2070100, 0x2070110
   put(center, '3f', 0, 100, 0); put(dimensions, '3f', 32, 32, 0)
   phase, calls, items_start = 'before', [], len(ns['events'])
   def observe(mu, address, size, unused):
    esp = mu.reg_read(UC_X86_REG_ESP)
    row = {'phase': phase, 'entry': hex(address), 'caller': hex(get(esp, 'I')[0])}
    if address == 0x454d10:
     row['script'] = get(esp + 8, 'i')[0]
    elif address == 0x464440:
     row['rngBefore'] = list(get(0x4ce568, 'II'))
    calls.append(row)
   for entry in [0x42df00, 0x42d9c0, 0x42c770, 0x454d10, 0x464440, 0x4273f0]:
    v.hook_add(UC_HOOK_CODE, observe, begin=entry, end=entry)
   def capture(label):
    count = get(obj + 0x470, 'i')[0]
    assert 0 <= count <= 8
    effects = []; link = get(0x5000000 + 0x8856b8, 'I')[0]
    while link:
     vm, link = get(link, 'II')
     effects.append({'script': get(vm + 0x3ea, 'h')[0], 'clock': get(vm + 0x6c, 'i')[0],
       'engine': list(get(vm + 0x430, '3f')), 'rotation': list(get(vm + 0x24, '3f')),
       'layer': get(vm + 0x20, 'i')[0]})
    return {'phase': label, 'position': list(get(obj + 0x50, '3f')),
      'velocity': list(get(obj + 0x5c, '3f')), 'angleWidthSpeed': list(get(obj + 0x68, '4f')),
      'laserPhase': get(obj + 0xc, 'i')[0], 'retirePending': get(obj + 0x7c, 'B')[0],
      'protection': get(obj + 0x44c, 'i')[0], 'grazeAge': list(get(obj + 0x28, 'iif')),
      'nodes': [list(get(history + index * 20, '5f')) for index in range(count)],
      'gameSeedCounter': list(get(0x4ce568, 'II')), 'primaryEffects': effects,
      'pointRequests': [event for event in ns['events'][items_start:] if event.get('kind') == 'itemSpawn']}
   frames = [capture(phase)]
   phase = 'after-clear'; v.reg_write(UC_X86_REG_ECX, obj)
   returned = call(0x42df00, center, int.from_bytes(floating(16), 'little'), convert, force) if method == 'circle' \
      else call(0x42d9c0, center, dimensions, convert, force)
   frames.append(capture(phase))
   phase = 'after-update'; v.reg_write(UC_X86_REG_ECX, obj); call(0x42c770)
   frames.append(capture(phase))
   phase = 'after-primary'; v.reg_write(UC_X86_REG_EDI, 0x5000000); call(0x460fe0)
   frames.append(capture(phase))
   records.append({'method': method, 'pattern': pattern, 'variant': variant, 'convert': convert,
     'force': force, 'protected': protected, 'syntheticEclHex': raw.hex(), 'seed': 1234,
     'center': [0, 100, 0], 'radius': 16, 'dimensions': [32, 32, 0],
     'returned': returned, 'frames': frames, 'actualCalls': calls})
   gc.collect()
out = ROOT / 'tests/fixtures/curve-cancel-history-original.json'
out.write_text(json.dumps({'schema': 'th12-original-curve-cancel-history/1',
 'originalExeSha256': ns['SHA'], 'wholeGameOracle': False, 'gpuExecuted': False,
 'samplerSha256': hashlib.sha256(pathlib.Path(__file__).read_bytes()).hexdigest(),
 'providerBootstrapSha256': hashlib.sha256(BOOTSTRAP.encode()).hexdigest(),
 'nativeD3dxVec2': ns['d3dx_vec2'],
 'scope': 'Actual Enemy/ECL611 Curve constructor and bullet ANM initialize each object; one original '
          'Laser20 visit processes actual ETEX200 protection. Eight explicit synthetic history node '
          'positions/angles/speeds and matching current XYZ are controlled local state inputs. '
          'Current velocity/angle/speed remain original factory values. Real42df00/42d9c0 cancellation, '
          '42c770 history/motion/query/Cap tick and Primary27 execute without replaced gameplay or RNG. '
          'Actual4273f0 point requests use the provider passive item-allocation boundary; item collection, '
          'score, full application and replay acceptance are excluded. Real cancellation ANM/RNG runs. '
          'All new fixture bytes are synthetic except recorded original outputs; no original assets or '
          'replay bytes are embedded and no accepted golden is changed.', 'records': records}, indent=2) + '\n', encoding='utf8')
print(json.dumps({'output': str(out), 'cases': len(records), 'snapshots': sum(len(r['frames']) for r in records),
 'examples': [[r['method'], r['pattern'], r['variant'], r['returned'],
   [len(f['nodes']) for f in r['frames']], [f['position'][0] for f in r['frames']],
   [f['gameSeedCounter'][1] for f in r['frames']]] for r in records[:10]]}))
