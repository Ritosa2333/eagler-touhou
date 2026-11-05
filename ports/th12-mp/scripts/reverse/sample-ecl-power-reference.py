"""Actual ECL -9930 getters and Stage6 exit-point worker with Item births.

Four difficulty masks exercise dynamic399/400/401 power. Actual retail worker
and independently authored same-semantic bytecode are cross-checked before
accepting synthetic fixture bytes. No original rule, RNG or input is edited.
"""
import hashlib, json, pathlib, struct, sys, gc
ROOT = pathlib.Path(__file__).resolve().parents[2]
PROVIDER = ROOT / 'scripts/reverse/native-enemy-provider.py'
BOOTSTRAP = PROVIDER.read_text(encoding='utf8').split('SPAWN=0x2070000;')[0]
helper = (ROOT / 'scripts/reverse/sample-last-enemy-id.py').read_text(encoding='utf8').split('birth_cases =')[0]
exec(compile(helper, str(ROOT / 'scripts/reverse/sample-last-enemy-id.py'), 'exec'))
BOOTSTRAP = PROVIDER.read_text(encoding='utf8').split('SPAWN=0x2070000;')[0]
def worker():
 code = []; labels = {}; jumps = []
 def mark(name): labels[name] = sum(map(len, code))
 def jump(op, target): jumps.append((len(code), target)); code.append(ins(op, [i(0), i(0)]))
 jump(12, 'loop')
 mark('loop')
 code.extend([ins(42, [i(-9930)], mask=1), ins(42, [i(400)]), ins(69)])
 jump(13, 'wait')
 code.extend([ins(406), ins(407, [i(2), i(1)]), ins(409)])
 mark('wait'); code.append(ins(83, [i(1)])); jump(12, 'loop')
 offsets = []; at = 0
 for command in code: offsets.append(at); at += len(command)
 for index, target in jumps:
  op = struct.unpack_from('<H', code[index], 4)[0]
  code[index] = ins(op, [i(labels[target] - offsets[index]), i(0)])
 return code
records = []; getters = []
powers = [399, 400, 400, 399, 401, 400, 399, 400, 400, 400, 399, 401]
for difficulty in range(4):
 captures = []
 for retail in [True, False]:
  # Only this bootstrap initial condition is synthetic. Its stage6 resource
  # selection loads the genuine MbossGirl00Item bytecode for the retail pass.
  base_fixture = json.loads((ROOT / 'artifacts/reverse/enemy-input-bomb-lifecycle-sanae-b.json').read_text())
  base_fixture.update(stage=6, difficulty=difficulty, scope='Controlled local ECL power-reference fixture')
  fixture_path = ROOT / f'artifacts/reverse/ecl-power-reference-bootstrap-d{difficulty}.json'
  fixture_path.write_text(json.dumps(base_fixture), encoding='utf8')
  saved = sys.argv; sys.argv = [str(PROVIDER), '--frames', '0', '--native-player', '--native-items',
    '--native-bullets', '--native-lasers', '--active-collision', '--input', str(fixture_path)]
  ns = {'__file__': str(PROVIDER), '__name__': 'ecl_power_reference_original'}
  try: exec(compile(BOOTSTRAP, str(PROVIDER), 'exec'), ns)
  finally: sys.argv = saved
  from unicorn.x86_const import UC_X86_REG_ECX
  mu, put, get, call = (ns[k] for k in ['v', 'put', 'get', 'call'])
  target = 'MbossGirl00Item' if retail else 'PointTrail'
  bodies = [('main', [ins(402, [i(0xa0)]), ins(300, [f(12), f(100)]),
    ins(408, [f(32), f(32)]), ins(15, [string(target)]), ins(83, [i(8)]), ins(1)])]
  if not retail: bodies.append(('PointTrail', worker()))
  raw, entries = scpt(bodies); base = 0x4300000; mu.mem_write(base, raw)
  for routine, (text, header) in entries.items(): ns['routines'][routine] = (base + text, base + header)
  put(ns['PROGRAM'] + 8, 'I', len(ns['routines']))
  for index, (_, (text, header)) in enumerate(sorted(ns['routines'].items())):
   put(ns['PROGRAM'] + 0x100 + index * 8, 'II', text, header)
  put(0x4ce568, 'II', 1234, 0); put(0x4b0c48, 'i', powers[0])
  # Real stage6 main spawns MBoss with primary drop1. Original406 resets
  # only counted drops, so retain this pending small-P until first409.
  request = 0x2070000; put(request, 'fffiiiII', 0, 0, 0, 0, 1, 10000, 0, 0)
  enemy = call(0x412990, base + entries['main'][0], request)
  def capture(tick):
   items = []
   for slot in range(600):
    item = ns['item_manager'] + 0x14 + slot * 0x9d8; state, kind = get(item + 0x9b0, 'ii')
    if state: items.append({'slot': slot, 'state': state, 'type': kind,
      'position': list(get(item + 0x96c, '2f')), 'velocity': list(get(item + 0x978, '2f')),
      'age': get(item + 0x98c, 'i')[0], 'birthKind': get(item + 0x9b8, 'i')[0]})
   return {'tick': tick, 'power': get(0x4b0c48, 'i')[0], 'rng': list(get(0x4ce568, 'II')),
     'enemies': get(ns['MANAGER'] + 0x70, 'i')[0], 'items': items}
  frames = [capture(-1)]
  for tick, power in enumerate(powers):
   ns['frame'] = tick; put(0x4b0c48, 'i', power)
   call(0x413190, ns['MANAGER']); call(0x425c20, ns['item_manager'])
   frames.append(capture(tick))
  captures.append(frames)
  if not retail:
   records.append({'difficulty': difficulty, 'syntheticEclHex': raw.hex(), 'seed': 1234, 'initialDrop': 1,
     'powers': powers, 'frames': frames, 'retailCrossCheck': frames == captures[0]})
  if not getters:
   for power in [0, 399, 400, 401, 16777217, -2147483647]:
    put(0x4b0c48, 'i', power); mu.reg_write(UC_X86_REG_ECX, enemy)
    integer = call(0x419a20, (-9930) & 0xffffffff)
    mu.reg_write(UC_X86_REG_ECX, enemy); call(0x41a000, (-9930) & 0xffffffff)
    mu.mem_write(0x10fd000, b'\xd9\x1d\x00\xd0\x0f\x02\xc3'); call(0x10fd000)
    getters.append({'power': power, 'integerWord': integer, 'floatValue': get(0x20fd000, 'f')[0]})
  gc.collect()
 if captures[0] != captures[1]:
  out = ROOT / f'artifacts/reverse/ecl-power-reference-crosscheck-d{difficulty}.json'
  out.write_text(json.dumps({'retail': captures[0], 'synthetic': captures[1]}, indent=2), encoding='utf8')
  raise RuntimeError('Retail/synthetic worker mismatch: ' + str(out))
out = ROOT / 'tests/fixtures/ecl-power-reference-original.json'
out.write_text(json.dumps({'schema': 'th12-original-ecl-power-reference/1', 'originalExeSha256': ns['SHA'],
 'samplerSha256': hashlib.sha256(pathlib.Path(__file__).read_bytes()).hexdigest(),
 'wholeGameOracle': False, 'gpuExecuted': False,
 'scope': 'Actual419a20 getter419dba reads signed raw4b0c48 power; float41a000 getter41a427 uses signed fild. '
  'Four original difficulty masks execute actual Enemy412990/Enemy18 and Item425c20, real Item4273f0 '
  'allocation/ANM/RNG. Retail stage6 MbossGirl00Item and independently authored synthetic equivalent '
  'worker produce identical snapshots. An original ECL15 starts the worker after full birth; creation '
  'guard is retained. Dynamic399/400/401 values are controlled inputs before each Enemy18 visit. '
  'Parent main exits using actual op1 after8 passes, so its async worker stops too. Player/Bomb/Bullet/UFO '
  'updates, full application, replay, STD and GPU are excluded; bootstrapped player state stays away '
  'from the item trail. No accepted fixture, original resource or replay input is changed. Only synthetic '
  'SCPT bytes are stored in this fixture.', 'getterCases': getters, 'records': records}, indent=2) + '\n', encoding='utf8')
print(json.dumps({'output': str(out), 'getterCases': len(getters), 'difficultyCases': len(records),
 'snapshots': sum(len(r['frames']) for r in records),
 'itemsByTick': [[len(f['items']) for f in r['frames']] for r in records]}))
