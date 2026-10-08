"""Original UFO defeat loop44b04f..44b09c and subsequent trail CPU behavior.

Reuses the independent native effect sampler's prepared ANM/resource/host
boundaries. Executes the actual ten-factory loop, rather than substituting RNG
counts. Does not run the surrounding Enemy/item reward managers or GPU.
"""
import hashlib, json, pathlib, re
ROOT = pathlib.Path(__file__).resolve().parents[2]
sampler = ROOT / 'scripts/reverse/sample-ufo-summon-effect.py'
namespace = {'__file__': str(sampler), '__name__': 'ufo_effect_fixture_definitions'}
# Load only the independent sampler definitions; its scenario export is a
# standalone script and must not regenerate unrelated fixtures on import.
exec(compile(sampler.read_text(encoding='utf-8').split('\nscenarios = []\n')[0], str(sampler), 'exec'), namespace)
Native = namespace['Native']
POSITION, OUT, SP = [namespace[k] for k in ['POSITION', 'OUT', 'SP']]
UC_X86_REG_EAX, UC_X86_REG_ECX, UC_X86_REG_EDI, UC_X86_REG_ESI, UC_X86_REG_ESP, UC_X86_REG_EIP = [namespace[k] for k in ['UC_X86_REG_EAX','UC_X86_REG_ECX','UC_X86_REG_EDI','UC_X86_REG_ESI','UC_X86_REG_ESP','UC_X86_REG_EIP']]
scenarios = []
for seed, position in [(0x1234, [-7.079999923706055, 78.94000244140625, 0]), (0xbeef, [140.25, 177.5, 0])]:
    native = Native(seed)
    ascii_resource = namespace['install_resource'](native.mu, (ROOT / 'local/retail/ascii.anm').read_bytes(), 0x6700000, bank_id=11)
    native.put(0x4b43b8, 'I', 0x2015000)
    native.put(0x2015000 + 0x18fb4, 'I', ascii_resource['resource'])
    native.put(POSITION, '3f', *position)
    native.call(0x4615a0, (native.resource['resource'], OUT, 208, 0),
                {UC_X86_REG_EAX: 23, UC_X86_REG_ECX: POSITION})
    bullet_birth = {'rng': native.rng(), 'nodes': native.nodes()}
    ufo, enemy = 0x2012000, 0x2013000
    native.put(ufo + 0x34, 'I', enemy)
    native.put(enemy + 0x1074, '3f', *position)
    native.mu.reg_write(UC_X86_REG_ESP, SP)
    native.mu.reg_write(UC_X86_REG_ESI, ufo)
    native.mu.emu_start(0x44b04f, 0x44b09c, count=1000000)
    if native.mu.reg_read(UC_X86_REG_EIP) != 0x44b09c:
        raise RuntimeError('Original defeat-loop instruction budget')
    effect_birth = {'tick': -1, 'rng': native.rng(), 'nodes': native.nodes()}
    frames = []
    native.capture_draws()
    for tick in range(18):
        native.tick = tick
        before = native.rng()
        native.call(0x460fe0, registers={UC_X86_REG_EDI: namespace['MANAGER']})
        frames.append({'tick': tick, 'rngBefore': before, 'rngAfter': native.rng(),
                       'rngCallDelta': native.rng()['calls'] - before['calls'], 'nodes': native.nodes()})
        if tick in [0, 1, 8, 14, 15, 16, 17]: native.capture_draws()
    scenarios.append({'scenario': 'defeat-ten-trails', 'seed': seed, 'position': position,
                      'bulletBirth': bullet_birth, 'effectBirth': effect_birth,
                      'frames': frames, 'births': native.births, 'draws': native.draws})
result = {'schema': 'th12-native-ufo-defeat-effect/1', 'originalExeSha256': namespace['EXE_SHA256'],
          'bulletAnmSha256': hashlib.sha256(namespace['RAW']).hexdigest(),
          'wholeGameOracle': False, 'acceptedReplayGolden': False, 'gpuExecuted': False,
          'boundary': 'Original4615a0 bullet208 bind; actual44b04f..44b09c coordinate conversion and ten40fbe0(type1) factory loop; complete460fe0 Primary27 passes, original4102b0/4103f0/410590 motion/colors/draw/removal. Host malloc/free and pending-quad flush boundaries as sample-ufo-summon-effect.py; D3D submissions captured without GPU. Surrounding Enemy/UFO/item managers not run.',
          'scenarios': scenarios}
output = ROOT / 'tests/fixtures/ufo-defeat-effect-native.json'
serialized = json.dumps(result, indent=2, allow_nan=False)
serialized = re.sub(r'\[[\s\d.,eE+\-]+\]', lambda match: re.sub(r'\s+', '', match[0]), serialized)
output.write_text(serialized + '\n', encoding='utf-8')
print(json.dumps({'output': str(output), 'bytes': output.stat().st_size,
                 'cases': [{'seed': s['seed'], 'birthCalls': s['effectBirth']['rng']['calls'],
                            'firstTickDelta': s['frames'][0]['rngCallDelta'],
                            'secondTickDelta': s['frames'][1]['rngCallDelta']} for s in scenarios]}))
