"""Original STD17 background grid, constructors, Stage12 update and draw vertices.

Uses the effect sampler's texture-free ANM/heap host, with a loaded text.anm
bank and the real stage05 STD stream. No STD or grid instruction is replaced.
The only new host boundary is D3DXVec3Normalize. Draw45c3a0 records CPU-filled
strips and returns before D3D; the @R render-target pixels are outside scope.
"""
import hashlib, json, math, pathlib, struct
ROOT = pathlib.Path(__file__).resolve().parents[2]
source = ROOT / 'scripts/reverse/sample-ufo-summon-effect.py'
ns = {'__file__': str(source), '__name__': 'stage_background_effect_native_definitions'}
exec(compile(source.read_text(encoding='utf-8').split('\nscenarios = []\n')[0], str(source), 'exec'), ns)
Native = ns['Native']
from unicorn import UC_HOOK_CODE
from unicorn.x86_const import *
STAGE, CODE, HEADER = 0x2050000, 0x2080000, 0x2090000
STD = (ROOT / 'local/retail/stage05.std').read_bytes()
TEXT = (ROOT / 'local/retail/text.anm').read_bytes()
class StageNative(Native):
    def __init__(self):
        super().__init__()
        text = ns['install_resource'](self.mu, TEXT, 0x6700000, bank_id=8)
        self.put(0x4cee70, 'I', text['resource'])
        self.put(0x4cea94, 'I', 1)
        self.put(0x4b43c0, 'I', STAGE)
        self.put(0x4b43cc, 'I', 0)  # Declared no active Spell/Bonus manager.
        start = struct.unpack_from('<I', STD, 8)[0]
        self.mu.mem_write(CODE, STD[start:])
        for offset, value in [(0x10, HEADER), (0x1c, CODE), (0x4c, CODE), (0x35bc, 1)]:
            self.put(STAGE + offset, 'I', value)
        self.put(STAGE + 0x38, 'iifII', -1, 0, 0, 0x4b2ed0, 1)
        self.put(STAGE + 0x3618, '3f', 0, 0, 1)
        self.put(STAGE + 0x3624, '3f', 0, 1, 0)
        self.submissions = []
        self.mu.hook_add(UC_HOOK_CODE, self.stage_hook, begin=0x46c6bc, end=0x46c6bc)
        self.mu.hook_add(UC_HOOK_CODE, self.stage_hook, begin=0x45c3a0, end=0x45c3a0)
    def stage_hook(self, mu, at, size, user):
        sp = mu.reg_read(UC_X86_REG_ESP)
        if at == 0x46c6bc:
            dest, source = self.get(sp + 4, '2I')
            xyz = self.get(source, '3f')
            length = math.sqrt(sum(n*n for n in xyz))
            self.put(dest, '3f', *(n/length if length else 0 for n in xyz))
            self.ret(dest, 8)
        else:
            pointer, count = self.get(sp + 4, '2I')
            vm = mu.reg_read(UC_X86_REG_EAX)
            self.submissions.append({'id': self.get(vm, 'I')[0],
                'sprite': self.get(vm + 0x3e4, 'h')[0],
                'layer': self.get(vm + 0x20, 'i')[0],
                'flags': self.get(vm + 0x47c, 'I')[0],
                'vertices': [[*self.get(pointer + i*28, '4f'),
                              self.get(pointer + i*28 + 16, 'I')[0],
                              *self.get(pointer + i*28 + 20, '2f')] for i in range(count)]})
            self.ret(0, 8)
    def snapshot(self):
        grid = self.get(STAGE + 0x35dc, 'I')[0]
        result = {'frame': self.tick, 'clock': self.get(STAGE + 0x3c, 'i')[0],
                  'callbackCount': self.get(STAGE + 0x35d8, 'i')[0], 'rng': self.rng(),
                  'effectFields': self.get(STAGE + 0x35e0, '2f') + self.get(STAGE + 0x35e8, 'I') + self.get(STAGE + 0x35ec, '2f') + self.get(STAGE + 0x35f4, 'i'),
                  'dimensions': self.get(grid, '2i') if grid else [0, 0], 'vertices': [], 'draws': []}
        if not grid: return result
        width, height = self.get(grid, '2i')
        pointer = self.get(grid + 0x10, 'I')[0]
        result['vertices'] = [[*self.get(pointer + i*28, '4f'),
                               self.get(pointer + i*28 + 16, 'I')[0],
                               *self.get(pointer + i*28 + 20, '2f')] for i in range(width*height)]
        vms = self.get(grid + 0xc, 'I')[0]
        self.submissions.clear()
        for i in range(width-1):
            vm = self.get(vms + i*4, 'I')[0]
            self.call(0x40ff00, registers={UC_X86_REG_ECX: vm})
            self.call(0x45c900, (ns['MANAGER'],), {UC_X86_REG_EAX: vm})
        result['draws'] = list(self.submissions)
        return result
native = StageNative()
samples = []
for tick in range(2801):
    native.tick = tick
    native.call(0x403ec0, registers={UC_X86_REG_ECX: STAGE})
    native.call(0x460fe0, registers={UC_X86_REG_EDI: ns['MANAGER']})
    if tick in [0, 2563, 2564, 2565, 2566, 2570, 2600, 2684, 2800]:
        samples.append(native.snapshot())
output = ROOT / 'tests/fixtures/stage-background-effect-native.json'
output.write_text(json.dumps({'schema': 'th12-native-stage-background-effect/1',
    'originalExeSha256': ns['EXE_SHA256'], 'stdSha256': hashlib.sha256(STD).hexdigest(),
    'textAnmSha256': hashlib.sha256(TEXT).hexdigest(), 'wholeGameOracle': False, 'gpuExecuted': False,
    'scope': 'Original stage05.std,403ec0/403020/4049a0,40ff10,4314d0,410020,4101f0,460fe0 and45c900. Synthetic Stage has objectCount0 and eight inactive embedded VMs; texture-free text.anm host, no active Spell/Bonus manager, heap host and D3DXVec3Normalize arithmetic boundary.45c3a0 records actual strip vertices before D3D; @R texture pixels and the GPU are not executed.',
    'samples': samples}, indent=2) + '\n', encoding='utf-8')
print(json.dumps({'output': str(output), 'samples': len(samples),
                  'vertices': sum(len(s['vertices']) for s in samples),
                  'draws': sum(len(s['draws']) for s in samples)}))
