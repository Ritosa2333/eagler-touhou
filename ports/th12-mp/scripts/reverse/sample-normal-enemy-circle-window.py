"""Local original body-collision branch on explicit candidate-window geometry.

This does not substitute for the replay's actual actor capture. It establishes
what the original413fc8..414048 and437980 instructions do with these parameters,
including real4391c0 award/ANM/RNG. No gameplay result or random call is mocked.
"""
import hashlib, json, pathlib, struct, sys
ROOT = pathlib.Path(__file__).resolve().parents[2]
PROVIDER = ROOT / 'scripts/reverse/native-enemy-provider.py'
BOOTSTRAP = PROVIDER.read_text(encoding='utf-8').split('SPAWN=0x2070000;')[0]
summary_path = ROOT / 'artifacts/reverse/th12_14-s3-d5c3-graze-events-summary.json'
summary = json.loads(summary_path.read_text(encoding='utf-8'))
records = []
for source in summary['rows']:
    if source['inputTick'] not in [8905, 8947]:
        continue
    for actor in source['potentialEnemyCircleQueries']:
        if not (actor['queryResult'] == 2 and actor['cadenceWouldAward'] and actor['bodyCollisionEnabled']):
            continue
        saved = sys.argv
        sys.argv = [str(PROVIDER), '--frames', '0', '--native-player', '--native-items',
                    '--native-bullets', '--native-lasers', '--active-collision',
                    '--instruction-budget', '20000000', '--input',
                    str(ROOT / 'artifacts/reverse/enemy-input-bomb-lifecycle-sanae-b.json'),
                    '--output', str(ROOT / 'artifacts/reverse/normal-enemy-circle-bootstrap.json')]
        ns = {'__file__': str(PROVIDER), '__name__': 'original_normal_enemy_circle'}
        try:
            exec(compile(BOOTSTRAP, str(PROVIDER), 'exec'), ns)
        finally:
            sys.argv = saved
        from unicorn import UC_HOOK_CODE
        from unicorn.x86_const import UC_X86_REG_ESP, UC_X86_REG_EBX, UC_X86_REG_EIP, UC_X86_REG_EDI
        mu, put, get = (ns[key] for key in ['v', 'put', 'get'])
        state = 0x20e0000
        player = source['player']
        put(state + 0x16b8, 'I', actor['flags'])
        put(state + 0x16a8, 'i', actor['bodyImmunity'])
        put(state + 0x1770, 'I', 0)  # Native413fe2 reads this State callback pointer.
        put(state + 0xd8, '2f', *actor['hitbox'][2:4])
        put(state + 0x270, 'i', actor['collisionAge'])
        put(state + 0x34, '3f', *actor['position'], 0)
        put(ns['PLAYER'] + 0x97c, '3f', *player['position'], 0)
        put(ns['PLAYER'] + 0xa28, 'i', player['state'])
        put(ns['PLAYER'] + 0xc404, 'i', player['invulnerability'])
        put(ns['PLAYER'] + 0xc414, 'I', 0)
        put(ns['stage_manager'] + 0x6d30, 'I', 0)
        sht = get(ns['PLAYER'] + 0xa2c, 'I')[0]
        put(sht + 4, 'f', player['effectiveCircleHitRadius'])
        put(ns['SP'] + 0x10, 'I', state + 0x34)
        mu.reg_write(UC_X86_REG_ESP, ns['SP'])
        mu.reg_write(UC_X86_REG_EBX, state)
        events = []

        def observe(mu, address, size, unused):
            esp = mu.reg_read(UC_X86_REG_ESP)
            events.append({'entry': hex(address), 'caller': hex(get(esp, 'I')[0]),
                           'radius': get(esp + 4, 'f')[0] if address == 0x437980 else None,
                           'grazeBefore': get(0x4b0cdc, 'i')[0],
                           'rngBefore': list(get(0x4ce568, 'II'))})
        for entry in [0x437980, 0x4391c0]:
            mu.hook_add(UC_HOOK_CODE, observe, begin=entry, end=entry)
        before = {'graze': get(0x4b0cdc, 'i')[0], 'rng': list(get(0x4ce568, 'II'))}
        mu.emu_start(0x413fc8, 0x414048, count=2000000)
        if mu.reg_read(UC_X86_REG_EIP) != 0x414048:
            raise RuntimeError('Actual normal body branch did not reach414048')
        after = {'graze': get(0x4b0cdc, 'i')[0], 'rng': list(get(0x4ce568, 'II'))}
        mu.reg_write(UC_X86_REG_EDI, 0x5000000)
        ns['call'](0x460fe0)
        after_primary = {'graze': get(0x4b0cdc, 'i')[0], 'rng': list(get(0x4ce568, 'II'))}
        records.append({'inputTickLabel': source['inputTick'], 'candidateActor': actor,
                        'syntheticPlayer': player, 'before': before, 'after': after,
                        'afterPrimary27': after_primary,
                        'originalEntryCalls': events})
output = ROOT / 'artifacts/reverse/normal-enemy-circle-candidate-geometries-native-query.json'
output.write_text(json.dumps({'schema': 'th12-original-local-normal-enemy-circle/1',
    'originalExeSha256': ns['SHA'], 'wholeGameOracle': False, 'actualReplayActorsCaptured': False,
    'candidateWindowSha256': hashlib.sha256(summary_path.read_bytes()).hexdigest(),
    'candidateWasmSha256': summary['wasmSha256'],
    'scope': 'Candidate post-frame body geometry and candidate collisionAge are explicit synthetic '
             'inputs to actual413fc8..414048/437980/4391c0. The selected original manager/resource/heap/'
             'audio host environment executes actual ANM children and RNG, including one real '
             '460fe0 Primary27 visit after the body branch. Does not prove that the '
             'original replay supplied the same actor/geometry; pending original replay caller capture.',
    'records': records}, indent=2) + '\n', encoding='utf-8')
print(json.dumps({'output': str(output), 'cases': len(records),
                  'deltas': [[row['inputTickLabel'], row['after']['graze'] - row['before']['graze'],
                              row['after']['rng'][1] - row['before']['rng'][1]] for row in records]}))
