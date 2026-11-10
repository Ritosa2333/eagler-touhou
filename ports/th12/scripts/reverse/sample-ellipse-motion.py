"""Original320..323 ellipse dispatch, flag transitions and motion prefix.

Constructed CPU slices, no candidate math, gameplay substitutions, resource
loaders, GPU, original/replay edits or accepted golden updates.
"""
import hashlib, json, pathlib, struct, sys
ROOT = pathlib.Path(__file__).resolve().parents[2]
sys.path.insert(0, str(ROOT.parent / 'tools/python'))
sys.path.insert(0, str(ROOT / 'scripts/reverse'))
from native_anm_resource import check_original, EXE_SHA256
import pefile
from unicorn import Uc, UC_ARCH_X86, UC_MODE_32
from unicorn.x86_const import *
EXE = ROOT.parent / 'th12/th12.exe'; check_original(EXE); PE = pefile.PE(str(EXE))
STATE, CONTEXT, RUNNER, INSTRUCTION = 0x2000000, 0x2010000, 0x2012000, 0x2020000
SP, STOP = 0x100d000, 0x100f000
image = PE.get_memory_mapped_image()
indices = image[0x19890:0x19890 + 356]
targets = struct.unpack_from('<143I', image, 0x19654)
class Native:
    def __init__(self):
        self.mu = Uc(UC_ARCH_X86, UC_MODE_32)
        for address, size in [(0x400000, (PE.OPTIONAL_HEADER.SizeOfImage + 4095) & ~4095),
                              (0x1000000, 0x10000), (STATE, 0x40000)]: self.mu.mem_map(address, size)
        self.mu.mem_write(0x400000, image)
        self.mu.reg_write(UC_X86_REG_FPCW, 0x37f); self.mu.reg_write(UC_X86_REG_MXCSR, 0x1f80)
        self.put(0x4b2ed0, 'f', 1); self.put(0x4d52d4, 'I', 1)
        self.put(STATE + 0x174c, 'I', RUNNER); self.put(RUNNER + 4, 'I', CONTEXT)
        self.put(CONTEXT + 4, 'I', INSTRUCTION)
        self.put(STATE + 0x68, '3f', 10, 100, 0); self.put(STATE + 0x9c, '3f', 3, -7, 0)
        self.put(STATE + 0x34, '3f', 13, 93, 0)
    def put(self, at, fmt, *values): self.mu.mem_write(at, struct.pack('<' + fmt, *values))
    def get(self, at, fmt): return list(struct.unpack('<' + fmt, self.mu.mem_read(at, struct.calcsize('<' + fmt))))
    def dispatch(self, op, args):
        integer_count = 2 if op in [301,303,305,307,309,311,321,323] else 1 if op in [325,326] else 0
        data = b''.join(struct.pack('<i' if i < integer_count else '<f', value) for i, value in enumerate(args))
        self.mu.mem_write(INSTRUCTION, struct.pack('<iHHHBBI', 0, op, 16 + len(data), 0, 63, len(args), 0) + data)
        self.put(SP + 0x34, 'I', STATE)
        for register, value in [(UC_X86_REG_ESP,SP),(UC_X86_REG_EAX,op),(UC_X86_REG_EBX,STATE),
                                (UC_X86_REG_ESI,RUNNER),(UC_X86_REG_EDI,INSTRUCTION)]: self.mu.reg_write(register,value)
        self.mu.emu_start(targets[indices[op-256]], 0x41962b, count=100000)
        if self.mu.reg_read(UC_X86_REG_EIP) != 0x41962b: raise RuntimeError('Original ellipse dispatcher budget')
    def tick(self):
        self.put(STATE + 0x16b8, 'I', 0); self.put(SP, 'II', STOP, STATE)
        self.mu.reg_write(UC_X86_REG_ESP, SP)
        self.mu.emu_start(0x413840, 0x413ac5, count=100000)
        if self.mu.reg_read(UC_X86_REG_EIP) != 0x413ac5: raise RuntimeError('Original movement prefix budget')
    def motion(self, offset):
        at = STATE + offset
        return {'position': self.get(at, '3f'), 'centerOrVelocity': self.get(at + 0xc, '3f'),
                'speed': self.get(at + 0x18, 'f')[0], 'angle': self.get(at + 0x1c, 'f')[0],
                'radius': self.get(at + 0x20, 'f')[0], 'radiusDelta': self.get(at + 0x24, 'f')[0],
                'ellipseAngle': self.get(at + 0x28, 'f')[0], 'ellipseRatio': self.get(at + 0x2c, 'f')[0],
                'flags': self.get(at + 0x30, 'I')[0]}
    def snapshot(self):
        return {'absolute': self.motion(0x68), 'relative': self.motion(0x9c), 'combined': self.motion(0x34),
                'durations': self.get(STATE + 0x358, 'i') + self.get(STATE + 0x3d0, 'i') + self.get(STATE + 0x448, 'i') +
                             self.get(STATE + 0x394, 'i') + self.get(STATE + 0x40c, 'i') + self.get(STATE + 0x484, 'i'),
                'ellipseInterpolatorTimers': self.get(STATE + 0x414 + 0x20, 'iif') + self.get(STATE + 0x450 + 0x20, 'iif')}
base = [0.35,0.02,20,0.125,-0.4,0.6]
keep = -999999.0
def command(op,args): return {'op':op,'args':args}
rows = []
specs = [
 ('absolute_world_heading', [command(320,[0,0,40,0,1.5707963705062866,0.5])], 12),
 ('absolute_rotated_ellipse', [command(320,base)], 60),
 ('relative_rotated_ellipse', [command(322,base)], 60),
 ('ratio_above_one', [command(320,[-2.5,0.04,16,-0.02,-0.9,1.6])], 30),
 ('ratio_zero', [command(320,[0.8,0.04,16,0,0.3,0])], 12),
 ('ratio_negative', [command(320,[0.8,0.04,16,0,0.3,-0.5])], 12),
 ('keep_all_ellipse_fields', [command(320,base),command(320,[keep]*6)], 8),
 ('circle_retains_ellipse', [command(320,base),command(308,[0.2,0.04,16,0])], 12),
 ('relative_circle_retains_ellipse', [command(322,base),command(310,[0.2,0.04,16,0])], 12),
 ('absolute_interpolate_ellipse', [command(320,base),command(321,[10,0,0.07,40,-0.125,1.4,1.5])], 15),
 ('relative_interpolate_ellipse', [command(322,base),command(323,[10,4,0.07,40,-0.125,1.4,1.5])], 15),
 ('zero_duration_ellipse_preserves_flags', [command(320,base),command(321,[10,0,0.07,40,-0.125,1.4,1.5]),command(321,[0,0,keep,keep,keep,keep,keep])], 8),
 ('negative_duration_ellipse_preserves_flags', [command(320,base),command(321,[-1,0,keep,keep,keep,keep,keep])], 8),
 ('positive_position_clears_ellipse', [command(320,base),command(301,[10,0,80,140])], 12),
 ('zero_position_preserves_ellipse', [command(320,base),command(301,[0,0,80,140])], 12),
 ('negative_position_preserves_ellipse', [command(320,base),command(301,[-1,0,80,140])], 12),
 ('direct_linear_clears_ellipse', [command(320,base),command(304,[keep,keep])], 8),
 ('positive_polar_clears_ellipse', [command(320,base),command(305,[10,0,0.2,0.4])], 12),
 ('zero_polar_preserves_ellipse', [command(320,base),command(305,[0,0,0.2,0.4])], 12),
 ('positive_hermite_clears_ellipse', [command(320,base),command(325,[10,0,0,80,140,0,0])], 12),
 ('zero_hermite_flag_behavior', [command(320,base),command(325,[0,0,0,80,140,0,0])], 12),
]
for name, commands, ticks in specs:
    native = Native(); states = []
    for cmd in commands:
        native.dispatch(cmd['op'],cmd['args']); states.append(native.snapshot())
    frames=[]
    for tick in range(ticks): native.tick(); frames.append({'tick':tick+1,**native.snapshot()})
    rows.append({'name':name,'absolute':[10,100],'relative':[3,-7], 'commands':commands,'afterCommands':states,'frames':frames})
output = ROOT / 'tests/fixtures/ellipse-motion-native.json'
output.write_text(json.dumps({'schema':'th12-native-ellipse-motion/1','originalExeSha256':EXE_SHA256,
    'wholeGameOracle':False,'acceptedReplayGolden':False,'gpuExecuted':False,
    'boundary':'Actual320/322 dispatch417093,321/323 dispatch4171fc and parameter/flag transition opcodes, followed by complete movement413840..413ac5. Original465280/4646e0/465390/464780/465320 and CRT math execute; constructed Enemy/context, literal operands, rate1, no instruction hooks or replacements. Stops before ANM/collision/ECL/lifetime callbacks.',
    'interpolationDurationOrder':['absolutePolar','absoluteRadius','absoluteEllipse','relativePolar','relativeRadius','relativeEllipse'],
    'rows':rows},indent=2)+'\n',encoding='utf-8')
print(json.dumps({'output':str(output),'cases':len(rows),'frames':sum(len(r['frames']) for r in rows),
    'interpolationCases':[{'name':r['name'],'bornEllipse':[r['afterCommands'][-1]['absolute']['ellipseAngle'],r['afterCommands'][-1]['absolute']['ellipseRatio']],
                           'lastEllipse':[r['frames'][-1]['absolute']['ellipseAngle'],r['frames'][-1]['absolute']['ellipseRatio']],
                           'lastDurations':r['frames'][-1]['durations']} for r in rows if 'interpolate_ellipse' in r['name']]}))
