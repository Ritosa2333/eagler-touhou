"""Actual six-loadout Player16 death/deathbomb resource lifecycles.

Controlled stock/fragment states are inputs, not replay trajectories. Each
case executes real438370 and forty real436ba0 callbacks, including actual
death commit4381e0 and respawn422f60 when reached. No game rule is replaced.
"""
import gc, hashlib, json, pathlib, sys
ROOT = pathlib.Path(__file__).resolve().parents[2]
PROVIDER = ROOT / 'scripts/reverse/native-enemy-provider.py'
BOOTSTRAP = PROVIDER.read_text(encoding='utf8').split('SPAWN=0x2070000;')[0]
rows = []
for loadout, name in enumerate(['reimu-a', 'reimu-b', 'marisa-a', 'marisa-b', 'sanae-a', 'sanae-b']):
 fixture_path = ROOT / f'artifacts/reverse/enemy-input-bomb-lifecycle-{name}.json'
 if not fixture_path.exists(): raise RuntimeError('Existing bootstrap loadout fixture missing: ' + name)
 for stock in [0, 1, 2]:
  for fragments in [0, 2, 4]:
   for deathbomb in [False, True]:
    saved = sys.argv
    sys.argv = [str(PROVIDER), '--frames', '0', '--native-player', '--native-items', '--native-bullets',
     '--native-lasers', '--active-collision', '--instruction-budget', '20000000', '--input', str(fixture_path)]
    ns = {'__file__': str(PROVIDER), '__name__': 'player_death_resource_original'}
    try: exec(compile(BOOTSTRAP, str(PROVIDER), 'exec'), ns)
    finally: sys.argv = saved
    from unicorn import UC_HOOK_CODE
    from unicorn.x86_const import UC_X86_REG_EAX, UC_X86_REG_ESI, UC_X86_REG_ESP
    v, put, get, call = (ns[k] for k in ['v', 'put', 'get', 'call'])
    player, bomb = ns['PLAYER'], ns['bomb_manager']
    put(ns['MANAGER'] + 0x70, 'i', 1)
    put(0x4b0c98, '4i', 4, 2, stock, fragments); put(0x4b0ccc, 'i', -300)
    put(0x4b0c48, 'i', 100)
    put(player + 0xa48, 'i', 120); put(player + 0x988, '2i', 111, 54144)
    put(player + 0x97c, '3f', 111 / 128, 423, 0); put(player + 0xc404, 'i', 0)
    phase, calls = 'before-collision', []
    def observe(mu, address, size, unused):
     sp = mu.reg_read(UC_X86_REG_ESP)
     calls.append({'phase': phase, 'entry': hex(address), 'caller': hex(get(sp, 'I')[0]),
       'lifeBomb': list(get(0x4b0c98, '4i'))})
    for entry in [0x438370, 0x4381e0, 0x422f60, 0x41cf40, 0x422f20, 0x406bf0]:
     v.hook_add(UC_HOOK_CODE, observe, begin=entry, end=entry)
    def capture(label):
     return {'phase': label, 'state': get(player + 0xa28, 'i')[0],
       'stateAge': get(player + 0xa34, 'i')[0], 'invulnerability': get(player + 0xc404, 'i')[0],
       'lifeBomb': list(get(0x4b0c98, '4i')), 'rank': get(0x4b0ccc, 'i')[0],
       'power': get(0x4b0c48, 'i')[0], 'bombActive': get(bomb + 0x3c, 'i')[0]}
    frames = [capture(phase)]
    v.reg_write(UC_X86_REG_ESI, player); call(0x438370)
    phase = 'after-collision'; frames.append(capture(phase))
    if deathbomb:
     v.reg_write(UC_X86_REG_EAX, player + 0xa30); call(0x4067e0, 7)
     frames.append(capture('controlled-window-age7'))
    for tick in range(40):
     phase = f'player-tick-{tick}'
     held = 2 if deathbomb and tick == 0 else 0
     put(0x4d49d0, 'I', held)
     call(0x436ba0, player)
     frames.append(capture(phase))
    rows.append({'loadout': loadout, 'stock': stock, 'fragments': fragments,
     'deathbomb': deathbomb, 'initial': {'lives': 4, 'lifeFragments': 2, 'power': 100,
       'rank': -300, 'fixedX': 111, 'fixedY': 54144, 'totalAge': 120},
     'frames': frames, 'actualCalls': calls})
    gc.collect()
 print(json.dumps({'loadout': loadout, 'completedCases': len(rows)}), flush=True)
out = ROOT / 'tests/fixtures/player-death-resources-original.json'
out.write_text(json.dumps({'schema': 'th12-original-player-death-resources/1',
 'originalExeSha256': ns['SHA'], 'samplerSha256': hashlib.sha256(pathlib.Path(__file__).read_bytes()).hexdigest(),
 'providerBootstrapSha256': hashlib.sha256(BOOTSTRAP.encode()).hexdigest(),
 'wholeGameOracle': False, 'gpuExecuted': False,
 'scope': 'Actual six-loadout startup and retail ANM/SHT initialize each controlled stock/fragment case. '
  'Original438370 collision, full436ba0 Player16 and real4381e0/422f60 death/respawn execute; stock0 '
  'deathbomb attempts fail naturally and continue through ordinary death. Successful deathbomb cases '
  'use an explicit controlled age7 input in the actual window. Forty Player16 callbacks follow. '
  'Bomb17/Primary27/Enemy18/Item22/STD/application/GPU are excluded; no bullet/UFO collection occurs. '
  'Selected-manager provider heap/resource loader and audio output host boundaries remain explicit. '
  'HUD update41cf40 executes its original code; no resource, movement, collision, ANM, timer, math or '
  'RNG instruction is replaced. No original replay input or accepted fixture is changed.', 'rows': rows},
 indent=2) + '\n', encoding='utf8')
print(json.dumps({'output': str(out), 'cases': len(rows), 'snapshots': sum(len(r['frames']) for r in rows),
 'ordinaryRespawn': [{'loadout': r['loadout'], 'stock': r['stock'], 'fragments': r['fragments'],
    'finalLifeBomb': r['frames'][-1]['lifeBomb']} for r in rows if not r['deathbomb'] and r['fragments'] == 2]}))
