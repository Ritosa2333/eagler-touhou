"""Pinned original spell-life bookkeeping and 412050 hurt calculation.

The post-Spell-start slice418947..41962b and post-end slice41897a..41962b
execute without presentation; 412050 executes completely. Inputs are explicit
synthetic health states, not a full-game/replay or rendering oracle.
"""
import pathlib, sys, struct, json
ROOT = pathlib.Path(__file__).resolve().parents[2]
sys.path.insert(0, str(ROOT.parent / 'tools/python'))
import pefile
from unicorn import Uc, UC_ARCH_X86, UC_MODE_32
from unicorn.x86_const import *
from native_anm_resource import check_original, EXE_SHA256
exe = ROOT.parent / 'th12/th12.exe'
check_original(exe)
pe = pefile.PE(str(exe))
mu = Uc(UC_ARCH_X86, UC_MODE_32)
for at, size in [(0x400000, (pe.OPTIONAL_HEADER.SizeOfImage+4095)&~4095),
                  (0x1000000, 0x10000), (0x2000000, 0x10000)]:
    mu.mem_map(at, size)
mu.mem_write(0x400000, pe.get_memory_mapped_image())
ENEMY, LIFE, SP, STOP = 0x2000000, 0x2002648, 0x100e000, 0x100f000
put = lambda at, fmt, *v: mu.mem_write(at, struct.pack('<'+fmt, *v))
read = lambda at, fmt: list(struct.unpack('<'+fmt, mu.mem_read(at, struct.calcsize('<'+fmt))))


def call(address, end, regs):
    put(SP, 'I', STOP)
    mu.reg_write(UC_X86_REG_ESP, SP)
    for reg, value in regs.items():
        mu.reg_write(reg, value & 0xffffffff)
    mu.emu_start(address, end, count=10000)
    if mu.reg_read(UC_X86_REG_EIP) != end:
        raise RuntimeError('Native health probe instruction budget')


records = []
for initial, threshold, flags, spell, damages in [
    (1500, 0, 0, True, [15]*64), (1500, 0, 2, True, [1]*28),
    (1500, 0, 0, True, [80]*64), (1500, 1000, 0, True, [1,6,7,8,79,80]*16),
    (3, 0, 0, True, [1,6,7,8,79,80]), (1007,1000,0,True,[1,6,7,8,79,80]),
    (1500, 0, 0, False, [15]*64), (1500, 0, 2, False, [80]*20)]:
    put(LIFE, '5iI', initial, initial, initial-threshold, initial*7, threshold, flags)
    if spell:
        call(0x418947, 0x41962b, {UC_X86_REG_EBX: ENEMY+0x1040})
    def state():
        return {'life': read(LIFE,'i')[0], 'raw': read(LIFE+12,'i')[0], 'flags':read(LIFE+20,'I')[0]}
    birth = state()
    frames = []
    for damage in damages:
        call(0x412050, STOP, {UC_X86_REG_ECX:LIFE, UC_X86_REG_EAX:damage})
        frames.append({'damage':damage, **state()})
    call(0x41897a, 0x41962b, {UC_X86_REG_EBX: ENEMY+0x1040})
    records.append({'initial':initial,'threshold':threshold,'flags':flags,'spell':spell,
                    'birth':birth,'frames':frames,'end':state()})
out = ROOT / 'tests/fixtures/enemy-life-original.json'
out.write_text(json.dumps({'schema':'th12-original-enemy-life/1','originalExeSha256':EXE_SHA256,
    'wholeGameOracle':False,'gpuExecuted':False,'logicHooks':False,
    'provider':'Actual418947..41962b,41897a..41962b and complete412050; explicit life inputs',
    'records':records},indent=2)+'\n',encoding='utf8')
print(json.dumps({'records':len(records),'hits':sum(len(r['frames']) for r in records)}))
