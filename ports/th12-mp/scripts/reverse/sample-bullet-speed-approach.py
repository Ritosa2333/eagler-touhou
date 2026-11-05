"""Actual TH12 20000000 dispatch/40b800 motion and type28->4 ANM probes."""
import hashlib, importlib.util, json, pathlib, struct, sys

ROOT = pathlib.Path(__file__).resolve().parents[2]
# Reuse only the audited original CPU host definition, never its sampled rows.
HOST = ROOT / 'scripts/reverse/sample-etex-boundaries.py'
exec(compile(HOST.read_text(encoding='utf-8').split('native = Native()')[0], str(HOST), 'exec'))


class ApproachNative(Native):
    def snapshot(self):
        return {
            **super().snapshot(),
            'approachTicks': self.get(BULLET + 0x940, 'i')[0],
            'approachDuration': self.get(BULLET + 0x964, 'i')[0],
            'approachDeltaWords': self.get(BULLET + 0x950, '5I'),
        }

    def hook(self, mu, address, size, unused):
        if address in [0x40b468, 0x40b527, 0x40b800, 0x40b81a, 0x409dda]:
            self.events.append({'context': self.context, 'address': hex(address), **self.snapshot()})
        super().hook(mu, address, size, unused)


def record(kind, c=0, a=0, b=-999999, concurrent=False):
    return {'kind': kind, 'c': c, 'd': 0, 'a': a, 'b': b, 'concurrent': concurrent}


native = ApproachNative()
player = 0x2030000
native.mu.mem_map(player, 0x10000)
native.put(0x4b4514, 'I', player)
native.put(player + 0x97c, 'ff', 55, 300)
motion_cases = []
specifications = [
    ('anchor-target-60', [record(0x20000000, 60, .06)], -.6752786636352539, .7588597536087036, 66),
    ('duration-minus1', [record(0x20000000, -1, .06), record(4, 2, .25)], .5, 1, 5),
    ('duration-zero', [record(0x20000000, 0, .06), record(4, 2, .25)], .5, 1, 5),
    ('duration-one', [record(0x20000000, 1, .06), record(4, 2, .25)], .5, 1, 5),
    ('duration-two', [record(0x20000000, 2, .06), record(4, 2, .25)], .5, 1, 6),
    ('different-heading', [record(0x20000000, 3, 2, -1.25)], .5, 1, 6),
    ('aim-heading', [record(0x20000000, 3, .25, 999)], .5, 1, 6),
    ('concurrent-acceleration-first', [record(4, 1, .25), record(0x20000000, 3, 2, concurrent=True)], .5, 1, 6),
    ('concurrent-approach-first', [record(0x20000000, 3, 2), record(4, 1, .25, concurrent=True)], .5, 1, 6),
    ('duplicate-active-kind', [record(0x20000000, 1, .25), record(0x20000000, 2, .5, concurrent=True)], .5, 1, 6),
]
for name, commands, angle, speed, steps in specifications:
    native.seed(commands, angle=angle, speed=speed)
    native.context = 'dispatch'
    native.call(0x40aa60, registers={UC_X86_REG_ECX: BULLET})
    snapshots = [{'step': 0, **native.snapshot()}]
    for step in range(1, steps + 1):
        native.context = f'update{step}'
        native.call(0x4099e0, (BULLET,))
        snapshots.append({'step': step, **native.snapshot()})
    motion_cases.append({'name': name, 'records': commands, 'angle': angle, 'speed': speed,
                         'position': [0, 120], 'player': [55, 300],
                         'samples': snapshots, 'events': native.events.copy()})

# Separate actual retail-ANM probe covers delayed morph and shared RNG, rather
# than assuming a timer completion is equivalent to an appearance transition.
HELPER = ROOT / 'scripts/reverse/sample-laser-phase-clear.py'
exec(compile(HELPER.read_text(encoding='utf-8').split('records=[]')[0], str(HELPER), 'exec'))
saved = sys.argv
sys.argv = [str(PROVIDER), '--frames', '1', '--seed', '4660', '--native-bullets']
ns = {'__file__': str(PROVIDER), '__name__': 'native_bullet_speed_approach'}
try:
    exec(compile(BOOTSTRAP, str(PROVIDER), 'exec'), ns)
finally:
    sys.argv = saved
v, put, get, call = (ns[name] for name in ['v', 'put', 'get', 'call'])
from unicorn.x86_const import UC_X86_REG_ESI
code = [ins(300, [floating(20), floating(90)]), ins(500, [word(0)]),
        ins(507, [word(0), word(1)]), ins(502, [word(0), word(28), word(8)]),
        ins(505, [word(0), floating(.7588597536087036), floating(0)]),
        ins(504, [word(0), floating(-.6752786636352539), floating(0)]),
        ins(508, [word(0), word(-1), word(-1)])]
for index, kind, c, d, a in [(0, 2, 1, 0, -999999), (1, 0x20000000, 60, 0, .06),
                            (2, 0x800, 4, 8, -999999), (3, 0x1000, 1000, 0, -999999)]:
    code.append(ins(509, [word(0), word(index), word(0), word(kind), word(c), word(d),
                          floating(a), floating(-999999)]))
code += [ins(501, [word(0)]), ins(83, [word(100000)])]
raw, header = program(code)
base = 0x4300000
v.mem_write(base, raw)
ns['routines']['ClearProbe'] = (base + 40, base + header)
put(ns['PROGRAM'] + 8, 'I', len(ns['routines']))
for index, (_, (text, routine)) in enumerate(sorted(ns['routines'].items())):
    put(ns['PROGRAM'] + 0x100 + index * 8, 'II', text, routine)
request = 0x2070000
put(request, 'fffiiiII', 0, 0, 0, 0, 0, 10000, 0, 0)
call(0x412990, base + 40, request)
pointer = next(iter(ns['bullet_ids']))


def animation_snapshot(input_tick):
    return {
        'inputTick': input_tick, 'phase': get(pointer + 0x532, 'H')[0],
        'typeColor': list(get(pointer + 0x9f4, 'HH')), 'age': get(pointer + 0x4e8, 'i')[0],
        'active': get(pointer + 0x528, 'I')[0], 'cursor': get(pointer + 0x548, 'i')[0],
        'position': list(get(pointer + 0x4bc, 'ff')), 'velocity': list(get(pointer + 0x4c8, 'ff')),
        'speed': get(pointer + 0x4d4, 'f')[0], 'angle': get(pointer + 0x4d8, 'f')[0],
        'approachTicks': get(pointer + 0x940, 'i')[0],
        'animationTime': get(pointer + 8 + 0x6c, 'i')[0],
        'script': get(pointer + 8 + 0x3ea, 'h')[0], 'sprite': get(pointer + 8 + 0x3e4, 'h')[0],
        'integers': list(get(pointer + 8 + 0x3fc, '6i')), 'rng': list(get(0x4ce568, 'II')),
    }


frames = [animation_snapshot(-1)]
for tick in range(82):
    v.reg_write(UC_X86_REG_ESI, ns['BULLETS'])
    call(0x40a020)
    frames.append(animation_snapshot(tick))
output = ROOT / 'tests/fixtures/bullet-speed-approach-original.json'
decoder_path = ROOT / 'scripts/reverse/decompile-resources.py'
spec = importlib.util.spec_from_file_location('speed_approach_retail_ecl', decoder_path)
decoder = importlib.util.module_from_spec(spec)
spec.loader.exec_module(decoder)
retail = decoder.ecl_index(ROOT / 'site/assets/stage04.ecl')
anchor_at2 = next(r for r in retail['routines'] if r['name'] == 'Anchor00_at2')
retail_command = next(c for c in anchor_at2['commands'] if c['opcode'] == 509
                      and c['operandWords'][1] == 1 and c['operandWords'][3] == 0x20000000)
assert retail_command['rankMask'] & 15 == 15
output.write_text(json.dumps({
    'schema': 'th12-original-bullet-speed-approach/1', 'originalExeSha256': EXE_SHA256,
    'wholeGameOracle': False, 'acceptedReplayGolden': False, 'gpuExecuted': False,
    'motionScope': 'Actual40aa60 dispatch,40b800 and4099e0 with prepared phase1/collision-disabled slot, raw sprite16x16, lifetime grace1000, rate1 and Player aim coordinates55,300. Native math and timer run unchanged; ANM/audio output are explicit external boundaries inherited from sample-etex-boundaries host.',
    'animationScope': 'Actual ECL emitter,40aa60,40b800,40a020/4099e0 and retail bullet ANM/RNG. Constructed original manager/resource host, passive Player hurt/audio; no Player/Enemy/Primary27 application scheduler or GPU.',
    'bulletAnmSha256': hashlib.sha256((ROOT / 'local/retail/bullet.anm').read_bytes()).hexdigest(),
    'retailDifficultyScope': {'file': 'stage04.ecl', 'sha256': retail['sha256'],
        'routine': 'Anchor00_at2', 'instructionOffset': retail_command['offset'],
        'rankMask': retail_command['rankMask'], 'difficulties': ['Easy', 'Normal', 'Hard', 'Lunatic'],
        'scope': 'Actual Anchor00_at2 record1 target-speed command is shared by all four difficulty bits. Dispatch/40b800 do not read difficulty; these short CPU fixtures cover common semantics. Separate harder-difficulty Anchor00_at2h emissions retain the original ECL condition and are not changed.'},
    'motionCases': motion_cases,
    'animationCases': [{'name': 'anchor-type28-target06-morph4', 'syntheticEclHex': raw.hex(), 'frames': frames}],
}, indent=2) + '\n', encoding='utf-8')
print(json.dumps({'output': str(output), 'motionCases': len(motion_cases),
                  'morphFrames': [[f['inputTick'], f['typeColor'], f['approachTicks'], f['phase']]
                                 for f in frames if f['inputTick'] in [-1, 7, 8, 65, 66, 67, 68, 75]]}))
