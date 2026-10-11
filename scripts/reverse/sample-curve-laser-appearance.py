"""Actual Curve factory/42e6d0 color binding and42cca0 CPU UV generation."""
import hashlib, json, pathlib, struct, sys
ROOT = pathlib.Path(__file__).resolve().parents[2]
HOST = ROOT / 'scripts/reverse/sample-custom-geometry.py'
exec(compile(HOST.read_text(encoding='utf-8').split('records=[]')[0], str(HOST), 'exec'))
profiles = json.loads((ROOT / 'artifacts/reverse/combat/native-probe.json').read_text())['laserProfiles']
template = bytes.fromhex(next(row['factory']['profileHex'] for row in profiles if row['factory']['kind'] == 2))
records = []
for color in [0, 6, 13]:
    native = Native()
    lm, player, sht, params = 0x2000000, 0x2010000, 0x2020000, 0x2040000
    native.put(0x4b4514, 'I', player)
    native.put(0x4b43dc, 'I', 0x2030000)
    native.put(player + 0xa28, 'i', 1)
    native.put(player + 0xa2c, 'I', sht)
    native.put(sht + 4, 'f', 2)
    native.put(player + 0x97c, '3f', 5000, 5000, 0)
    native.put(player + 0x9cc, '6f', 4999, 4999, 0, 5001, 5001, 0)
    native.call(0x427fa0, regs={UC_X86_REG_EDI: lm})
    native.put(lm + 0x464, 'I', lm + 0x10)
    native.put(lm + 0x488, 'I', native.resource['resource'])
    native.put(0x4b43d4, 'I', 0x2060000)
    native.put(0x2060010, 'I', native.resource['resource'])
    profile = bytearray(template)
    # Declared source parameter fields copied by42c3f0; no original ANM bytes
    # are embedded in this fixture. Remaining template words are state input.
    struct.pack_into('<ffHHi', profile, 0x10, 16, 3, 0, color, 42)
    native.mu.mem_write(params, bytes(profile))
    callbacks = []
    def callback_trace(mu, address, size, unused):
        if address == 0x42e6d0:
            vm = mu.reg_read(UC_X86_REG_ECX)
            owner = native.get(vm + 0x4b0, 'I')[0]
            callbacks.append({'entry': hex(address), 'ownerColor': native.get(owner + 0x452, 'h')[0]})
        elif address == 0x42e6e2:
            callbacks[-1]['actualReturnEax'] = mu.reg_read(UC_X86_REG_EAX)
    native.mu.hook_add(UC_HOOK_CODE, callback_trace, begin=0x42e6d0, end=0x42e6e2)
    rng_before = {'game': native.get(0x4ce568, 'II'), 'display': native.get(0x4ce560, 'II')}
    native.call(0x428450, [2], {UC_X86_REG_EDI: params})
    obj = native.get(lm + 0x18, 'I')[0]
    vm = obj + 0x634
    native.call(0x428270, regs={UC_X86_REG_EDI: lm})
    native.draws = []
    native.label = f'curve-type0-color{color}'
    native.call(0x42cca0, regs={UC_X86_REG_ECX: obj})
    state = native.state(vm)
    state['script'] = native.get(vm + 0x3ea, 'h')[0]
    state['spriteCallback'] = hex(native.get(vm + 0x4ac, 'I')[0])
    cap = obj + 0xae8
    cap_state = native.state(cap)
    cap_state['script'] = native.get(cap + 0x3ea, 'h')[0]
    records.append({'type': 0, 'color': color, 'configuredWidth': 16, 'historyCount': 42,
                    'profileHex': bytes(profile).hex(), 'callbackTrace': callbacks,
                    'state': state, 'capState': cap_state, 'draws': native.draws,
                    'rngBefore': rng_before,
                    'rngAfter': {'game': native.get(0x4ce568, 'II'), 'display': native.get(0x4ce560, 'II')}})
    assert records[-1]['rngBefore'] == records[-1]['rngAfter']
output = ROOT / 'tests/fixtures/curve-laser-appearance-original.json'
output.write_text(json.dumps({'schema': 'th12-original-curve-appearance/1',
    'originalExeSha256': EXE_SHA256, 'resourceSha256': hashlib.sha256(RAW).hexdigest(),
    'wholeGameOracle': False, 'acceptedReplayGolden': False, 'gpuExecuted': False,
    'factoryRngInitialCondition': {'game': [0x1234, 0], 'display': [0xbeef, 0]},
    'capAnimationScope': 'These retail cap scripts use fixed angular velocity and an alternating1.2/1.5 scale loop, not random getters. Both actual native RNG counters stay unchanged across factory, one update and draw; display seed is a declared host condition, not the cap timeline cause.',
    'scope': 'Actual428450 Curve factory/42c3f0,42e6d0 sprite callback with real EAX return,454ee0/455630 original ANM,428270 and42cca0 CPU draw. Declared prepared parameters select stage05 MBoss_at1 type0/color6/width16/history42; colors0/13 are controls. Original bullet ANM is required locally, never embedded here. Host inherits sample-custom-geometry heap/audio/GPU submission boundaries. Sprite identity and84 CPU strip vertices/UV are checked, not GPU/pixel equivalence or a whole replay. The callback does not read difficulty.',
    'records': records}, indent=2) + '\n', encoding='utf-8')
print(json.dumps({'output': str(output), 'records': [
    {'color': r['color'], 'script': r['state']['script'], 'sprite': r['state']['sprite'],
     'callbackReturn': r['callbackTrace'][0]['actualReturnEax'],
     'vertices': len(r['draws'][0]['vertices'])} for r in records]}))
