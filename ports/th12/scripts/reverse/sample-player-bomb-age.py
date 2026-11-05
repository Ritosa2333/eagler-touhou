"""Actual Player16 normal Bomb and deathbomb timer branches.

Constructed startup/inputs, original Player/SHT/Bomb/ANM managers and full
436ba0. States/ages are controlled initial conditions; death cases enter
through original438370. No game instruction, timer result or RNG is replaced.
"""
import json, pathlib, sys
ROOT = pathlib.Path(__file__).resolve().parents[2]
PROVIDER = ROOT / 'scripts/reverse/native-enemy-provider.py'
BOOTSTRAP = PROVIDER.read_text(encoding='utf-8').split('SPAWN=0x2070000;')[0]
cases = [(1, age, 2, 2, False) for age in [0, 29, 59, 60, 4452]]
cases += [(1, 4452, 0, 2, False), (1, 4452, 2, 0, False), (1, 4452, 2, 2, True)]
cases += [(4, age, 2, 2, False) for age in [0, 1, 7, 8]]
cases += [(4, 7, 0, 2, False), (4, 7, 2, 0, False), (4, 7, 2, 2, True), (4, 8, 0, 2, False)]
rows = []
for state, age, held, bombs, active in cases:
    saved = sys.argv
    sys.argv = [str(PROVIDER), '--frames', '0', '--native-player', '--native-items',
        '--native-bullets', '--native-lasers', '--active-collision',
        '--instruction-budget', '20000000',
        '--input', str(ROOT / 'artifacts/reverse/enemy-input-bomb-lifecycle-sanae-b.json'),
        '--output', str(ROOT / 'artifacts/reverse/player-bomb-age-bootstrap.json')]
    ns = {'__file__': str(PROVIDER), '__name__': 'original_player_bomb_age'}
    try: exec(compile(BOOTSTRAP, str(PROVIDER), 'exec'), ns)
    finally: sys.argv = saved
    from unicorn import UC_HOOK_CODE
    from unicorn.x86_const import UC_X86_REG_EAX, UC_X86_REG_ESI, UC_X86_REG_ESP
    mu, put, get, call = (ns[k] for k in ['v', 'put', 'get', 'call'])
    player, bomb = ns['PLAYER'], ns['bomb_manager']
    put(ns['MANAGER'] + 0x70, 'i', 1) # Explicit live-enemy startup gate.
    put(0x4b0ca0, 'i', bombs)
    if active: call(0x406bf0)
    if state == 4:
        put(player + 0xc404, 'i', 0)
        mu.reg_write(UC_X86_REG_ESI, player)
        call(0x438370)
    mu.reg_write(UC_X86_REG_EAX, player + 0xa30)
    call(0x4067e0, age)
    put(0x4d49d0, 'I', held)
    trace = []
    def observe(uc, address, size, user):
        stack = mu.reg_read(UC_X86_REG_ESP)
        trace.append({'entry': hex(address), 'caller': hex(get(stack, 'I')[0]),
                      'argument': get(stack + 4, 'i')[0] if address == 0x4067e0 else None})
    for address in [0x4067e0, 0x406bf0, 0x422f20, 0x4381e0]:
        mu.hook_add(UC_HOOK_CODE, observe, begin=address, end=address)
    def snap():
        return {'state': get(player + 0xa28, 'i')[0], 'stateAge': get(player + 0xa34, 'i')[0],
                'invulnerability': get(player + 0xc404, 'i')[0],
                'bombs': get(0x4b0ca0, 'i')[0], 'lives': get(0x4b0c98, 'i')[0],
                'bombActive': get(bomb + 0x3c, 'i')[0]}
    before = snap()
    call(0x436ba0, player)
    rows.append({'initial': {'state': state, 'stateAge': age, 'held': held,
                             'bombs': bombs, 'bombActive': active},
                 'before': before, 'after': snap(), 'trace': trace})
output = ROOT / 'tests/fixtures/player-bomb-age-original.json'
output.write_text(json.dumps({'schema': 'th12-original-player-bomb-age/1',
    'originalExeSha256': ns['SHA'], 'wholeGameOracle': False, 'gpuExecuted': False,
    'scope': 'Original Player constructor/SHT, Item/Bomb/ANM/resource startup and full436ba0. '
             'Synthetic SanaeB header and live-enemy count1; controlled state timers set through4067e0. '
             'Death cases enter actual438370. No Bomb17, ANM27 or GPU pass. '
             'Host-only resource/heap/audio boundaries remain those of the selected-manager provider.',
    'timerEvidence': 'Normal436d18 calls406bf0 without Timer reset; death436da8 calls4067e0(60). '
                     '436d51 confirms Miss before evaluating Bomb when stateAge>=8. '
                     '437405..437475 advances the selected state timer in the common tail.',
    'rows': rows}, indent=2) + '\n', encoding='utf-8')
print(json.dumps({'output': str(output), 'cases': len(rows),
    'results': [{'initial': r['initial'], 'after': r['after']} for r in rows]}))
