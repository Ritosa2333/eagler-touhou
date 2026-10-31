"""Original TH12 coordinate quantization, circular motion and combine evidence.

No candidate code executes. Native465320, CRT floor, ECL300/302/310/311,
413840 movement through413ac5 and413700 combine/clamp execute. The UFO case
uses a constructed state: native310/311 prime a radius0->16 interpolation for
16 original ticks, then trace1204 public fields are restored. Wide explicit
clamp bounds avoid any boundary collision. A labelled control injects one
extra ORIGINAL465320 call on relative coordinates before the native combine;
this control is intentionally altered scheduling, not original-game evidence.
"""
import hashlib
import json
import pathlib
import struct
import sys

ROOT = pathlib.Path(__file__).resolve().parents[2]
sys.path.insert(0, str(ROOT.parent / 'tools/python'))
import pefile
from unicorn import Uc, UC_ARCH_X86, UC_MODE_32, UC_HOOK_CODE
from unicorn.x86_const import *
from native_anm_resource import check_original, EXE_SHA256

EXE = ROOT.parent / 'th12/th12.exe'
check_original(EXE)
PE = pefile.PE(str(EXE))
STATE, CONTEXT, RUNNER, CODE = 0x2000000, 0x2010000, 0x2012000, 0x2020000
SP, TEMP_SP, STOP = 0x100d000, 0x1006000, 0x100f000
TRACE = ROOT / 'artifacts/reverse/world-native-active-th12_14-1500.json'
TRACE_RAW = TRACE.read_bytes()
TRACE_DATA = json.loads(TRACE_RAW)
if TRACE_DATA['originalExeSha256'] != EXE_SHA256:
    raise ValueError('Source trace does not match pinned original executable')
SOURCE = {row['frame']: next(enemy for enemy in row['enemies'] if enemy['id'] == 48)
          for row in TRACE_DATA['frames'] if row['frame'] in [1204, 1205, 1206]}


class Native:
    def __init__(self):
        self.mu = Uc(UC_ARCH_X86, UC_MODE_32)
        for address, size in [(0x400000, (PE.OPTIONAL_HEADER.SizeOfImage + 4095) & ~4095),
                              (0x1000000, 0x10000), (STATE, 0x40000)]:
            self.mu.mem_map(address, size)
        self.mu.mem_write(0x400000, PE.get_memory_mapped_image())
        self.put(0x4b2ed0, 'f', 1)
        self.put(0x4d52d4, 'I', 1)
        self.mu.reg_write(UC_X86_REG_MXCSR, 0x1f80)
        self.mu.reg_write(UC_X86_REG_FPCW, 0x37f)
        self.put(STATE + 0x174c, 'I', RUNNER)
        self.put(RUNNER + 4, 'I', CONTEXT)
        self.put(CONTEXT + 4, 'I', CODE)
        self.quantizations = []
        self.quant_stack = []
        self.combinations = []
        self.label = ''
        self.extra_control = False
        for address in [0x465320, 0x465382, 0x413700, 0x413831]:
            self.mu.hook_add(UC_HOOK_CODE, self.hook, begin=address, end=address)

    def put(self, address, fmt, *values):
        self.mu.mem_write(address, struct.pack('<' + fmt, *values))

    def get(self, address, fmt):
        return list(struct.unpack('<' + fmt, self.mu.mem_read(address, struct.calcsize('<' + fmt))))

    def vector(self, address):
        return {'xy': self.get(address, '2f'), 'bits': self.get(address, '2I')}

    def snapshot(self):
        return {'position': self.get(STATE + 0x34, '3f'),
                'absolute': self.get(STATE + 0x68, '3f'),
                'relative': self.get(STATE + 0x9c, '3f'),
                'delta': self.get(STATE + 0x40, '3f'),
                'angle': self.get(STATE + 0x84, 'f')[0],
                'speed': self.get(STATE + 0x80, 'f')[0],
                'relativeAngle': self.get(STATE + 0xb8, 'f')[0],
                'relativeSpeed': self.get(STATE + 0xb4, 'f')[0],
                'relativeRadius': self.get(STATE + 0xbc, 'f')[0],
                'relativeRadiusDelta': self.get(STATE + 0xc0, 'f')[0],
                'absoluteGeometryFlags': self.get(STATE + 0x98, 'I')[0],
                'relativeGeometryFlags': self.get(STATE + 0xcc, 'I')[0],
                'flags': self.get(STATE + 0x16b8, 'I')[0],
                'relativePolarDuration': self.get(STATE + 0x394, 'i')[0],
                'relativeRadiusDuration': self.get(STATE + 0x40c, 'i')[0]}

    def hook(self, mu, address, size, unused):
        if address == 0x465320:
            pointer = self.mu.reg_read(UC_X86_REG_ESI)
            names = {STATE: 'standalone', STATE + 0x34: 'combined',
                     STATE + 0x68: 'absolute', STATE + 0x9c: 'relative'}
            self.quant_stack.append({'label': self.label, 'component': names.get(pointer, hex(pointer)),
                                     'extraControlCall': self.extra_control,
                                     'caller': hex(self.get(self.mu.reg_read(UC_X86_REG_ESP), 'I')[0]),
                                     'before': self.vector(pointer), 'pointer': pointer})
        elif address == 0x465382:
            entry = self.quant_stack.pop()
            pointer = entry.pop('pointer')
            self.quantizations.append(entry | {'after': self.vector(pointer)})
        elif address == 0x413700:
            self.combinations.append({'label': self.label, 'event': 'enter', 'state': self.snapshot()})
        elif address == 0x413831:
            self.combinations.append({'label': self.label, 'event': 'return', 'state': self.snapshot()})

    def call(self, address, arguments=(), registers=None, until=STOP, stack=SP):
        self.put(stack, 'I' * (len(arguments) + 1), STOP, *arguments)
        self.mu.reg_write(UC_X86_REG_ESP, stack)
        for register, value in (registers or {}).items():
            self.mu.reg_write(register, value)
        self.mu.emu_start(address, until, count=1000000)
        if self.mu.reg_read(UC_X86_REG_EIP) != until:
            raise RuntimeError(f'Original instruction budget exceeded at {address:x}')

    def dispatch(self, opcode, arguments):
        raw = b''.join(struct.pack('<f' if isinstance(value, float) else '<i', value) for value in arguments)
        self.mu.mem_write(CODE, struct.pack('<iHHHBBI', 0, opcode, 16 + len(raw), 0, 63, len(arguments), 0) + raw)
        entry = 0x4165aa if opcode in [300, 302] else 0x416d06 if opcode in [308, 310] else 0x416e03
        self.put(SP + 0x34, 'I', STATE)
        self.mu.reg_write(UC_X86_REG_ESP, SP)
        self.mu.reg_write(UC_X86_REG_EAX, opcode)
        self.mu.reg_write(UC_X86_REG_EBX, STATE)
        self.mu.reg_write(UC_X86_REG_ESI, RUNNER)
        self.mu.emu_start(entry, 0x41962b, count=1000000)
        if self.mu.reg_read(UC_X86_REG_EIP) != 0x41962b:
            raise RuntimeError('Original ECL movement dispatch budget')

    def movement_tick(self, extra_relative_quantization=False):
        self.put(STATE + 0x16b8, 'I', self.get(STATE + 0x16b8, 'I')[0] & ~0x20000)
        self.call(0x413840, (STATE,), until=0x413ac0 if extra_relative_quantization else 0x413ac5)
        if extra_relative_quantization:
            saved = self.mu.context_save()
            self.extra_control = True
            self.call(0x465320, registers={UC_X86_REG_ESI: STATE + 0x9c}, stack=TEMP_SP)
            self.extra_control = False
            self.mu.context_restore(saved)
            self.mu.emu_start(0x413ac0, 0x413ac5, count=1000000)
            if self.mu.reg_read(UC_X86_REG_EIP) != 0x413ac5:
                raise RuntimeError('Original combine continuation budget')


quantization_cases = []
for value in [-4.53000020980835, -4.539999961853027, -4.789999961853027,
              -4.800000190734863, -4.809999942779541, -.009999752044677734,
              -.019999980926513672, -.015, -.005, 0., .005, .015, 4.53000020980835, 127.97999572753906]:
    native = Native()
    native.put(STATE, '2f', value, -value)
    before = native.vector(STATE)
    native.label = 'first-call'
    native.call(0x465320, registers={UC_X86_REG_ESI: STATE})
    once = native.vector(STATE)
    native.label = 'second-call'
    native.call(0x465320, registers={UC_X86_REG_ESI: STATE})
    quantization_cases.append({'requestedValue': value, 'before': before, 'once': once,
                               'twice': native.vector(STATE), 'calls': native.quantizations})


def prepare_trace_state(clamp=True):
    native = Native()
    # The clamp is deliberately broad; all reconstructed positions are inside.
    native.put(STATE + 0x15f4, '4f', 0, 128, 280, 128)
    native.put(STATE + 0x16b8, 'I', 0x10000 if clamp else 0)
    native.put(STATE + 0x34, '3f', 0, 128, 0)
    native.put(STATE + 0x68, '3f', 0, 128, 0)
    speed = float(SOURCE[1204]['relativeSpeed'])
    native.dispatch(310, [0., speed, 0., 0.])
    native.dispatch(311, [60, 0, speed, 16., 0.])
    for _ in range(16):
        native.movement_tick()
    source = SOURCE[1204]
    native.put(STATE + 0x34, '3f', *source['position'], 0)
    native.put(STATE + 0x68, '3f', *source['absolute'], 0)
    native.put(STATE + 0x9c, '3f', *source['relative'], 0)
    native.put(STATE + 0x80, '2f', source['speed'], source['angle'])
    native.put(STATE + 0xb4, '4f', source['relativeSpeed'], source['relativeAngle'], source['relativeRadius'], 0)
    native.put(STATE + 0x16b8, 'I', (source['flags'] & ~0x20000) if clamp else (source['flags'] & ~0x30000))
    native.quantizations.clear()
    native.combinations.clear()
    return native


motion_cases = []
for name, clamp, extra in [('native-once', True, False), ('extra-relative-quant-control', True, True),
                          ('native-unclamped-control', False, False)]:
    native = prepare_trace_state(clamp)
    initial = native.snapshot()
    frames = []
    for frame in [1205, 1206]:
        native.label = f'trace-frame{frame}'
        native.movement_tick(extra)
        frames.append({'traceFrame': frame, 'state': native.snapshot(),
                       'reference': {key: SOURCE[frame][key] for key in
                                     ['position', 'absolute', 'relative', 'relativeAngle', 'relativeSpeed', 'relativeRadius']}})
    motion_cases.append({'name': name, 'extraRelativeQuantization': extra, 'clampEnabled': clamp,
                         'preparedClamp': [0, 128, 280, 128],
                         'setupArgs': {'relative': True, 'angle': 0, 'initialRadius': 0,
                                       'initialRadiusDelta': 0, 'initialSpeed': SOURCE[1204]['relativeSpeed'],
                                       'targetSpeed': SOURCE[1204]['relativeSpeed'], 'targetRadius': 16,
                                       'targetDelta': 0, 'duration': 60, 'mode': 0, 'clamp': clamp},
                         'setupCallArguments': [1, 0, 0, 0, SOURCE[1204]['relativeSpeed'],
                                                SOURCE[1204]['relativeSpeed'], 16, 0, 60, 0, int(clamp)],
                         'preTicks': 16,
                         'restoreCheckpoint': {'traceFrame': 1204, 'state': initial},
                         'restorePositionArguments': [*initial['position'][:2], *initial['absolute'][:2],
                                                      *initial['relative'][:2], initial['angle'],
                                                      initial['relativeAngle'], initial['relativeRadius']],
                         'initial': initial,
                         'frames': frames, 'quantizations': native.quantizations,
                         'combinations': native.combinations})

combine_cases = []
for name, absolute in [('inside', [0, 128]), ('outside', [500, 500])]:
    native = Native()
    native.put(STATE + 0x15f4, '4f', 0, 128, 280, 128)
    native.put(STATE + 0x16b8, 'I', 0x10000)
    native.put(STATE + 0x34, '3f', 0, 128, 0)
    native.put(STATE + 0x68, '3f', *absolute, 0)
    native.put(STATE + 0x9c, '3f', -4.53000020980835, -.2199999988079071, 0)
    before = native.snapshot()
    native.label = 'direct-combine-' + name
    native.call(0x413700, registers={UC_X86_REG_ESI: STATE})
    combine_cases.append({'name': name, 'preparedClamp': [0, 128, 280, 128],
                          'before': before, 'after': native.snapshot(),
                          'quantizations': native.quantizations, 'combinations': native.combinations})

direct_set_cases = []
for opcode in [300, 302]:
    for name, coordinates in [('inside', [0., 128.] if opcode == 300 else [-4.53000020980835, -.2199999988079071]),
                              ('outside', [500., 500.])]:
        native = Native()
        native.put(STATE + 0x15f4, '4f', 0, 128, 280, 128)
        native.put(STATE + 0x16b8, 'I', 0x10000)
        native.put(STATE + 0x34, '3f', 0, 128, 0)
        native.put(STATE + 0x68, '3f', 0, 128, 0)
        native.put(STATE + 0x9c, '3f', -4.53000020980835, -.2199999988079071, 0)
        before = native.snapshot()
        native.label = f'ECL{opcode}-{name}'
        native.dispatch(opcode, coordinates)
        direct_set_cases.append({'opcode': opcode, 'name': name, 'coordinates': coordinates,
                                 'preparedClamp': [0, 128, 280, 128], 'before': before,
                                 'after': native.snapshot(), 'quantizations': native.quantizations,
                                 'combinations': native.combinations})

for frame in motion_cases[0]['frames']:
    for field in ['position', 'absolute', 'relative']:
        if frame['state'][field][:2] != frame['reference'][field]:
            raise RuntimeError(f'Constructed native state did not reproduce trace{frame["traceFrame"]} {field}')

result = {'schema': 'th12-native-enemy-quantization/1', 'originalExeSha256': EXE_SHA256,
          'partial': True, 'acceptedReplayGolden': False, 'wholeGameOracle': False,
          'provider': 'Original465320/floor493290, ECL300/302/310/311, movement413840 to413ac5, combine413700; no execution stubs',
          'traceSource': {'artifact': str(TRACE.relative_to(ROOT)), 'sha256': hashlib.sha256(TRACE_RAW).hexdigest(),
                          'enemy': 48, 'restoredFrame': 1204},
          'preparedInputs': ['SSE2 availability flag1, FPCW037f, MXCSR1f80, gameRate1',
                             'Native310/311 radius0->16 duration60, primed16 actual original movement ticks',
                             'Trace1204 public motion fields restored; center0 and radialDelta0; no whole-state restoration claimed'],
          'controlBoundary': 'Extra control stops413840 before413ac0, saves CPU context, calls original465320 once on relativeXY on separate stack, restores CPU context, resumes native combine',
          'quantizationCases': quantization_cases, 'motionCases': motion_cases,
          'combineCases': combine_cases, 'directSetCases': direct_set_cases}
output = ROOT / 'tests/fixtures/enemy-quantization-native.json'
output.write_text(json.dumps(result, indent=2, allow_nan=False) + '\n', encoding='utf-8')
print(json.dumps({'output': str(output), 'quantizationCases': len(quantization_cases),
                  'motionCases': [{ 'name': case['name'], 'x': [row['state']['position'][0] for row in case['frames']],
                                   'relativeX': [row['state']['relative'][0] for row in case['frames']],
                                   'absoluteX': [row['state']['absolute'][0] for row in case['frames']]}
                                  for case in motion_cases],
                  'combineCases': len(combine_cases), 'directSetCases': len(direct_set_cases)}))
