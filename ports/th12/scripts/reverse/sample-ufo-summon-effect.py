"""Independent original TH12 effect2 UFO-summon factory/list timing sample.

Runs retail40fbe0, effect2/type1 callbacks, real bullet.anm bind/tick, native
primary intrusive scheduling, RNG, removal, and pool allocation. The execution
boundaries are host malloc/free, a no-input quad flush, and dummy D3D methods
that record actual native vertex/state submissions. A following script0 VM is
an explicit alternate chain input, not a full UFO/game provider. Outputs are
partial constructed CPU fixtures, never accepted replay goldens.
"""
import hashlib
import json
import pathlib
import re
import struct
import sys

ROOT = pathlib.Path(__file__).resolve().parents[2]
sys.path.insert(0, str(ROOT.parent / 'tools/python'))
import pefile
from unicorn import Uc, UC_ARCH_X86, UC_MODE_32, UC_HOOK_CODE
from unicorn.x86_const import *
from native_anm_resource import check_original, EXE_SHA256, install_resource

EXE = ROOT.parent / 'th12/th12.exe'
RESOURCE_FILE = ROOT / 'local/retail/bullet.anm'
check_original(EXE)
PE = pefile.PE(str(EXE))
RAW = RESOURCE_FILE.read_bytes()
EFFECTS, POSITION, OUT = 0x2000000, 0x2010000, 0x2010100
MANAGER, RESOURCE, SP, STOP = 0x3000000, 0x6000000, 0x100e000, 0x100f000
DEVICE, VTABLE, GEOMETRY = 0x2020000, 0x2021000, 0x2800000
SET_RENDER_STATE, SET_STAGE_STATE, SET_SAMPLER_STATE, SET_FVF, DRAW_UP = [0x100d000 + i * 16 for i in range(5)]


class Native:
    def __init__(self, seed=0x1234):
        self.mu = Uc(UC_ARCH_X86, UC_MODE_32)
        for address, size in [(0, 0x10000),
                              (0x400000, (PE.OPTIONAL_HEADER.SizeOfImage + 4095) & ~4095),
                              (0x1000000, 0x10000), (0x2000000, 0x1000000),
                              (MANAGER, 0x900000), (RESOURCE, 0x800000)]:
            self.mu.mem_map(address, size)
        self.mu.mem_write(0x400000, PE.get_memory_mapped_image())
        self.mu.reg_write(UC_X86_REG_FPCW, 0x37f)
        self.mu.reg_write(UC_X86_REG_MXCSR, 0x1f80)
        self.mu.reg_write(UC_X86_REG_FPTAG, 0xffff)
        self.mu.reg_write(UC_X86_REG_FPSW, 0)
        self.put(0x4ce8cc, 'I', MANAGER)
        self.put(0x4b43d4, 'I', EFFECTS)
        self.put(0x4b2ed0, 'f', 1)
        self.put(0x4d52d4, 'I', 1)
        self.put(0x4d52dc, 'I', 1)
        self.put(0x4ce568, 'H', seed)
        self.put(0x4ce56c, 'I', 0)
        self.resource = install_resource(self.mu, RAW, RESOURCE, bank_id=0)
        self.put(EFFECTS + 0x10, 'I', self.resource['resource'])
        self.heap = 0x2200000
        self.tick = -1
        self.events = []
        self.factory_stack = []
        self.rng_stack = []
        self.vm_ticks = {}
        self.vm_kind = {}
        self.births = []
        self.host_allocations = 0
        self.host_frees = 0
        self.draws = []
        self.device_state = {'renderStates': {}, 'stageStates': {}, 'samplerStates': {}, 'fvf': 0}
        self.draw_id = 0
        self.put(0x4ce8f0, 'I', DEVICE)
        self.put(DEVICE, 'I', VTABLE)
        for offset, method in [(0xe4, SET_RENDER_STATE), (0x10c, SET_STAGE_STATE),
                               (0x114, SET_SAMPLER_STATE), (0x164, SET_FVF), (0x14c, DRAW_UP)]:
            self.put(VTABLE + offset, 'I', method)
        for address in [0x46c9ea, 0x46d04a, 0x46c941, 0x46ca4f,
                        0x40fbe0, 0x40fd0c, 0x454d10, 0x455630,
                        0x410720, 0x4107e0, 0x4102b0, 0x4103f0,
                        0x464440, 0x4644d8, 0x461450, 0x461250, 0x45a3c0,
                        SET_RENDER_STATE, SET_STAGE_STATE, SET_SAMPLER_STATE, SET_FVF, DRAW_UP]:
            self.mu.hook_add(UC_HOOK_CODE, self.hook, begin=address, end=address)

    def put(self, address, fmt, *values):
        self.mu.mem_write(address, struct.pack('<' + fmt, *values))

    def get(self, address, fmt):
        return list(struct.unpack('<' + fmt, self.mu.mem_read(address, struct.calcsize('<' + fmt))))

    def rng(self):
        return {'seed': self.get(0x4ce568, 'H')[0], 'calls': self.get(0x4ce56c, 'I')[0]}

    def ret(self, value=0, pop=0):
        stack = self.mu.reg_read(UC_X86_REG_ESP)
        target = self.get(stack, 'I')[0]
        self.mu.reg_write(UC_X86_REG_ESP, stack + 4 + pop)
        self.mu.reg_write(UC_X86_REG_EAX, value)
        self.mu.reg_write(UC_X86_REG_EIP, target)

    def vm(self, pointer):
        kind = self.vm_kind.get(pointer, 0)
        private = self.get(pointer + 0x478, 'I')[0]
        result = {'id': self.get(pointer, 'I')[0], 'type': kind,
                  'bank': self.get(pointer + 0x3e6, 'h')[0],
                  'script': self.get(pointer + 0x3ea, 'h')[0],
                  'sprite': self.get(pointer + 0x3e4, 'h')[0],
                  'spriteSelected': self.get(pointer + 0x3f4, 'I')[0] != 0,
                  'enginePosition': self.get(pointer + 0x430, '3f'),
                  'scriptPosition': self.get(pointer + 0x424, '3f'),
                  'rotation': self.get(pointer + 0x24, '3f'),
                  'clock': self.get(pointer + 0x6c, 'i')[0],
                  'clockValue': self.get(pointer + 0x70, 'f')[0],
                  'nativeAnmTickCalls': self.vm_ticks.get(pointer, 0),
                  'layer': self.get(pointer + 0x20, 'i')[0],
                  'flags': self.get(pointer + 0x47c, 'I')[0],
                  'primaryColor': self.get(pointer + 0x3bc, 'I')[0],
                  'callbacks': [hex(value) for value in self.get(pointer + 0x488, '4I')]}
        if private and kind == 2:
            result['effect'] = {'position': self.get(private, '3f'),
                                'previous': self.get(private + 0xc, 'i')[0],
                                'timer': self.get(private + 0x10, 'i')[0],
                                'timerValue': self.get(private + 0x14, 'f')[0],
                                'radius': self.get(private + 0x20, 'f')[0],
                                'colors': self.get(private + 0x24, '2I')}
        elif private and kind == 1:
            result['effect'] = {'pointsXY': self.get(private, '32f'),
                                'colors': self.get(private + 0x80, '16I'),
                                'heading': self.get(private + 0xc0, 'f')[0],
                                'previous': self.get(private + 0xc4, 'i')[0],
                                'timer': self.get(private + 0xc8, 'i')[0],
                                'timerValue': self.get(private + 0xcc, 'f')[0]}
        return result

    def nodes(self):
        node = self.get(MANAGER + 0x8856b8, 'I')[0]
        result = []
        while node:
            pointer, node = self.get(node, 'II')
            result.append(self.vm(pointer))
            if len(result) > 256:
                raise RuntimeError('Unexpected original primary chain cycle')
        return result

    def capture_draws(self):
        # There are no prepared pending quads. Make the actual custom helpers
        # establish their diffuse material on the intercepted dummy device.
        self.put(MANAGER + 0x4b5640, 'B', 0xff)
        self.put(MANAGER + 0x4b5647, 'B', 1)
        self.put(MANAGER + 0x4b5642, 'B', 0)
        node = self.get(MANAGER + 0x8856b8, 'I')[0]
        while node:
            pointer, node = self.get(node, 'II')
            callback = self.get(pointer + 0x48c, 'I')[0]
            if callback not in [0x410840, 0x410590]:
                continue
            self.draw_id = self.get(pointer, 'I')[0]
            self.put(MANAGER + 0x8856b0, 'I', GEOMETRY)
            before = self.rng()
            self.call(callback, registers={UC_X86_REG_ECX: pointer})
            if self.rng() != before:
                raise RuntimeError('Original custom draw unexpectedly consumed game RNG')

    def hook(self, mu, address, size, unused):
        stack = self.mu.reg_read(UC_X86_REG_ESP)
        if address == 0x45a3c0:
            self.ret()  # Prepared input has no pending normal quads to flush.
        elif address == SET_RENDER_STATE:
            state, value = self.get(stack + 8, '2I')
            self.device_state['renderStates'][state] = value
            self.ret(pop=12)
        elif address in [SET_STAGE_STATE, SET_SAMPLER_STATE]:
            stage, state, value = self.get(stack + 8, '3I')
            kind = 'stageStates' if address == SET_STAGE_STATE else 'samplerStates'
            self.device_state[kind][f'{stage}:{state}'] = value
            self.ret(pop=16)
        elif address == SET_FVF:
            self.device_state['fvf'] = self.get(stack + 8, 'I')[0]
            self.ret(pop=8)
        elif address == DRAW_UP:
            primitive, count, vertices, stride = self.get(stack + 8, '4I')
            if stride != 20 or primitive not in [3, 6]:
                raise RuntimeError('Unexpected original UFO effect vertex format/topology')
            vertex_count = count + (2 if primitive == 6 else 1)
            self.draws.append({'tick': self.tick, 'id': self.draw_id,
                               'primitive': 'triangle-fan' if primitive == 6 else 'line-strip',
                               'primitiveCount': count, 'vertexStride': stride,
                               'vertices': [[*self.get(vertices + i * stride, '4f'),
                                             self.get(vertices + i * stride + 16, 'I')[0]]
                                            for i in range(vertex_count)],
                               'deviceState': json.loads(json.dumps(self.device_state))})
            self.ret(pop=20)
        elif address in [0x46c9ea, 0x46d04a]:
            amount = self.get(stack + 4, 'I')[0]
            pointer = self.heap
            self.heap += (amount + 15) & ~15
            if self.heap >= 0x2f00000:
                raise RuntimeError('Prepared host heap exhausted')
            self.host_allocations += 1
            self.ret(pointer)
        elif address in [0x46c941, 0x46ca4f]:
            self.host_frees += 1
            self.ret()
        elif address == 0x40fbe0:
            out, kind, position = self.get(stack + 4, '3I')
            self.factory_stack.append({'tick': self.tick, 'type': kind,
                                       'position': self.get(position, '3f'),
                                       'rngBefore': self.rng(), 'out': out})
        elif address == 0x40fd0c:
            entry = self.factory_stack.pop()
            pointer = self.mu.reg_read(UC_X86_REG_ESI)
            self.vm_kind[pointer] = entry['type']
            self.births.append({key: value for key, value in entry.items() if key != 'out'} |
                               {'rngAfter': self.rng(), 'animation': self.vm(pointer)})
        elif address == 0x455630:
            pointer = self.get(stack + 4, 'I')[0]
            self.vm_ticks[pointer] = self.vm_ticks.get(pointer, 0) + 1
        elif address == 0x454d10:
            pointer, script = self.get(stack + 4, 'Ii')
            self.events.append({'tick': self.tick, 'event': 'native-bind',
                                'vmAddress': hex(pointer), 'script': script,
                                'rng': self.rng()})
        elif address in [0x410720, 0x4102b0]:
            pointer = self.mu.reg_read(UC_X86_REG_ECX)
            kind = 2 if address == 0x410720 else 1
            self.vm_kind[pointer] = kind
            self.events.append({'tick': self.tick, 'event': 'initializer', 'type': kind,
                                'vmAddress': hex(pointer), 'position': self.get(self.mu.reg_read(UC_X86_REG_EDX), '3f'),
                                'nativeAnmTickCallsBefore': self.vm_ticks.get(pointer, 0), 'rng': self.rng()})
        elif address in [0x4107e0, 0x4103f0]:
            pointer = self.mu.reg_read(UC_X86_REG_ECX)
            private = self.get(pointer + 0x478, 'I')[0]
            offset = 0x10 if address == 0x4107e0 else 0xc8
            self.events.append({'tick': self.tick, 'event': 'before-tick',
                                'id': self.get(pointer, 'I')[0], 'type': self.vm_kind[pointer],
                                'timerBefore': self.get(private + offset, 'i')[0], 'rng': self.rng()})
        elif address == 0x464440:
            rng_pointer = self.mu.reg_read(UC_X86_REG_ESI)
            self.rng_stack.append({'tick': self.tick, 'event': 'native-rng32',
                                   'caller': hex(self.get(stack, 'I')[0]),
                                   'rngObject': hex(rng_pointer),
                                   'seedBefore': self.get(rng_pointer, 'H')[0],
                                   'callsBefore': self.get(rng_pointer + 4, 'I')[0]})
        elif address == 0x4644d8:
            entry = self.rng_stack.pop()
            rng_pointer = self.mu.reg_read(UC_X86_REG_ESI)
            self.events.append(entry | {'seedAfter': self.get(rng_pointer, 'H')[0],
                                       'callsAfter': self.get(rng_pointer + 4, 'I')[0],
                                       'result': self.mu.reg_read(UC_X86_REG_EAX)})
        elif address == 0x461450:
            self.events.append({'tick': self.tick, 'event': 'native-remove',
                                'animation': self.vm(self.mu.reg_read(UC_X86_REG_ESI)), 'rng': self.rng()})
        elif address == 0x461250:
            pointer = self.mu.reg_read(UC_X86_REG_EBX)
            self.events.append({'tick': self.tick, 'event': 'primary-tail-register',
                                'vmAddress': hex(pointer), 'layer': self.get(pointer + 0x20, 'i')[0],
                                'rng': self.rng()})

    def call(self, address, arguments=(), registers=None):
        self.put(SP, 'I' * (1 + len(arguments)), STOP, *arguments)
        self.mu.reg_write(UC_X86_REG_ESP, SP)
        for register, value in (registers or {}).items():
            self.mu.reg_write(register, value)
        try:
            self.mu.emu_start(address, STOP, count=5000000)
        except Exception as error:
            raise RuntimeError(f'Native entry {address:x}, eip {self.mu.reg_read(UC_X86_REG_EIP):x}: {error}') from error
        if self.mu.reg_read(UC_X86_REG_EIP) != STOP:
            raise RuntimeError(f'Original instruction budget exhausted at {address:x}')


scenarios = []
for following_node in [False, True]:
    native = Native()
    native.put(POSITION, '3f', 0, 128, 0)  # Original44a8cc..44a8e8 summon position.
    native.call(0x40fbe0, (OUT, 2, POSITION))
    effect_birth = {'tick': -1, 'rng': native.rng(), 'nodes': native.nodes()}
    native.capture_draws()
    if following_node:
        native.call(0x4615a0, (native.resource['resource'], OUT + 4, 0, 0),
                    {UC_X86_REG_EAX: 23, UC_X86_REG_ECX: 0})
    frames = []
    for tick in range(121):
        native.tick = tick
        before = native.rng()
        native.call(0x460fe0, registers={UC_X86_REG_EDI: MANAGER})
        frames.append({'tick': tick, 'rngBefore': before, 'rngAfter': native.rng(),
                       'rngCallDelta': native.rng()['calls'] - before['calls'],
                       'nodes': native.nodes()})
        if tick in [0, 1, 8, 15, 20, 49, 50, 63, 64, 120]:
            native.capture_draws()
    scenarios.append({'scenario': 'following-held-VM' if following_node else 'isolated-tail',
                      'seed': 0x1234, 'position': [0, 128, 0],
                      'effectBirth': effect_birth, 'frames': frames,
                      'births': native.births, 'events': native.events,
                      'draws': native.draws,
                      'hostAllocations': native.host_allocations, 'hostFrees': native.host_frees})

output = ROOT / 'tests/fixtures/ufo-summon-effect-native.json'
full_output = ROOT / 'artifacts/reverse/ufo-summon-effect-native-full.json'
result = {'schema': 'th12-native-ufo-summon-effect/1',
    'originalExeSha256': EXE_SHA256, 'bulletAnmSha256': hashlib.sha256(RAW).hexdigest(),
    'partial': True, 'acceptedReplayGolden': False,
    'wholeGameOracle': False, 'gpuExecuted': False,
    'provider': 'Original40fbe0,410720,4107e0,4102b0,4103f0,461250,460fe0,455630,464440 and removal;410840/410590/45d430/45d5d0 CPU geometry',
    'effectTable': {'address': '0x4af190', 'entrySize': 24, 'script': 0,
                    'initializer': '0x410720', 'beforeTick': '0x4107e0',
                    'customDraw': '0x410840', 'delete': '0x410890', 'interrupt': '0x4108a0'},
    'scheduling': {'primaryTickPriority': 27, 'primaryTickEntry': '0x460c30',
                   'primaryTickRegistrationEvidence': '0x45e158..0x45e167',
                   'effectLayer': 13, 'layerDrawPriority': 26,
                   'drawEntry': '0x460d90', 'drawRegistrationEvidence': '0x45e5b5..0x45e5cc'},
    'preparedInputs': ['GPU-free retail bullet.anm tables, prepared bank namespace0',
                       'EffectManager+10 bound to that original resource',
                       'Zero-filled native ANMManager pool and rate1',
                       'Following-held-VM scenario appends original4615a0 bullet script0 VM after effect2'],
    'externalBoundaries': ['46c9ea/46d04a malloc: prepared zero-filled monotonic host heap',
                           '46c941/46ca4f free: no-op; native removal/pool flags/list changes execute',
                           '45a3c0 pending-quad flush no-op; no normal quads supplied',
                           'D3D SetRenderState/SetTextureStageState/SetSamplerState/SetFVF record state and return0',
                           'D3D DrawPrimitiveUP captures actual original20-byte vertices; no GPU submission'],
    'pointLayout': 'type1.effect.pointsXY stores16 consecutive{x,y} pairs in native index order',
    'scenarios': scenarios}
# Keep numerical vectors compact while preserving readable object structure.
def save(path, value):
    serialized = json.dumps(value, indent=2, allow_nan=False)
    serialized = re.sub(r'\[[\s\d.,eE+\-]+\]', lambda match: re.sub(r'\s+', '', match[0]), serialized)
    path.parent.mkdir(parents=True, exist_ok=True)
    path.write_text(serialized + '\n', encoding='utf-8')


save(full_output, result)
selected_frames = [0, 1, 2, 8, 15, 16, 20, 49, 50, 63, 64, 65, 119, 120]
compact = {key: value for key, value in result.items() if key != 'scenarios'}
compact.update({'schema': 'th12-native-ufo-summon-effect/2',
                'capture': 'compact-selected-state', 'selectedFrames': selected_frames,
                'fullTraceArtifact': 'artifacts/reverse/ufo-summon-effect-native-full.json',
                'scenarios': []})
for scenario in scenarios:
    reduced = {key: value for key, value in scenario.items() if key not in ['frames', 'births', 'events']}
    reduced['birthCounts'] = {'emitter': sum(birth['type'] == 2 for birth in scenario['births']),
                              'trail': sum(birth['type'] == 1 for birth in scenario['births']),
                              'total': len(scenario['births'])}
    reduced['frames'] = []
    for frame in scenario['frames']:
        selected = frame['tick'] in selected_frames
        reduced['frames'].append({**{key: value for key, value in frame.items() if key != 'nodes'},
                                  'hasFullState': selected,
                                  'activeEffectCounts': {
                                      'emitter': sum(node['type'] == 2 for node in frame['nodes']),
                                      'trail': sum(node['type'] == 1 for node in frame['nodes'])},
                                  'activeNodeCount': len(frame['nodes']),
                                  'nodes': frame['nodes'] if selected else [
                                      {'id': node['id'], 'type': node['type']} for node in frame['nodes']]})
    compact['scenarios'].append(reduced)
save(output, compact)
print(json.dumps({'output': str(output), 'fullTrace': str(full_output),
                 'compactBytes': output.stat().st_size, 'fullTraceBytes': full_output.stat().st_size,
                 'compactSha256': hashlib.sha256(output.read_bytes()).hexdigest(),
                 'fullTraceSha256': hashlib.sha256(full_output.read_bytes()).hexdigest(), 'scenarios': [
    {'scenario': row['scenario'], 'birthCalls': row['effectBirth']['rng']['calls'],
     'firstTickCalls': row['frames'][0]['rngCallDelta'],
     'secondTickCalls': row['frames'][1]['rngCallDelta'],
     'type1Births': sum(b['type'] == 1 for b in row['births']),
     'draws': len(row['draws']),
     'finalCalls': row['frames'][-1]['rngAfter']['calls']} for row in scenarios]}))
