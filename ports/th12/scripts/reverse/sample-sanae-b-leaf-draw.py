"""Actual type5 Sanae B leaf factory, Primary27 updates and line-strip draw.

The native effect sampler supplies texture-free ANM and host allocation/D3D
boundaries. Original40fbe0,4105e0,4103f0,410590/45d5d0 execute; dummy D3D calls
record actual submitted FVF44 vertices. No translated game or GPU executes.
"""
import hashlib, json, pathlib, re
ROOT = pathlib.Path(__file__).resolve().parents[2]
sampler = ROOT / 'scripts/reverse/sample-ufo-summon-effect.py'
namespace = {'__file__': str(sampler), '__name__': 'sanae_b_leaf_native_definitions'}
exec(compile(sampler.read_text(encoding='utf-8').split('\nscenarios = []\n')[0], str(sampler), 'exec'), namespace)
Native = namespace['Native']
POSITION, OUT, MANAGER = [namespace[k] for k in ['POSITION', 'OUT', 'MANAGER']]
UC_X86_REG_EDI = namespace['UC_X86_REG_EDI']

class LeafNative(Native):
    def vm(self, pointer):
        result = super().vm(pointer)
        private = self.get(pointer + 0x478, 'I')[0]
        if private and result['type'] == 5:
            result['effect'] = {'pointsXY': self.get(private, '32f'),
                                'colors': self.get(private + 0x80, '16I'),
                                'heading': self.get(private + 0xc0, 'f')[0],
                                'previous': self.get(private + 0xc4, 'i')[0],
                                'timer': self.get(private + 0xc8, 'i')[0],
                                'timerValue': self.get(private + 0xcc, 'f')[0]}
        return result

scenarios = []
for seed, position in [(0x1234, [224.8671875, 439., 0.]),
                        (0xbeef, [45.62999725341797, 399.75, 0.])]:
    native = LeafNative(seed)
    native.put(POSITION, '3f', *position)
    native.call(0x40fbe0, (OUT, 5, POSITION))
    birth = {'tick': -1, 'rng': native.rng(), 'nodes': native.nodes()}
    native.capture_draws()
    frames = []
    for tick in range(18):
        native.tick = tick
        before = native.rng()
        native.call(0x460fe0, registers={UC_X86_REG_EDI: MANAGER})
        frames.append({'tick': tick, 'rngBefore': before, 'rngAfter': native.rng(), 'nodes': native.nodes()})
        native.capture_draws()
    scenarios.append({'seed': seed, 'position': position, 'birth': birth,
                      'frames': frames, 'draws': native.draws})
output = ROOT / 'tests/fixtures/sanae-b-leaf-draw-native.json'
document = {'schema': 'th12-native-sanae-b-leaf-draw/1',
            'originalExeSha256': namespace['EXE_SHA256'],
            'bulletAnmSha256': hashlib.sha256(namespace['RAW']).hexdigest(),
            'wholeGameOracle': False, 'acceptedReplayGolden': False, 'gpuExecuted': False,
            'scope': 'Actual40fbe0(type5),4105e0,460fe0,4103f0,410590 and45d5d0. Explicit screen-space birth positions, synthetic ANM manager and resource host; malloc/free and empty normal-quad flush boundaries, D3D submissions captured without GPU. No Player/Bomb manager or whole-replay acceptance.',
            'scenarios': scenarios}
serialized = json.dumps(document, indent=2, allow_nan=False)
serialized = re.sub(r'\[[\s\d.,eE+\-]+\]', lambda match: re.sub(r'\s+', '', match[0]), serialized)
output.write_text(serialized + '\n', encoding='utf-8')
print(json.dumps({'output': str(output), 'cases': len(scenarios),
                  'draws': sum(len(s['draws']) for s in scenarios),
                  'vertices': sum(len(d['vertices']) for s in scenarios for d in s['draws'])}))
