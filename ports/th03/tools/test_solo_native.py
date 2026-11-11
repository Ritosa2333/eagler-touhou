"""Run the actual single-player bridge against 16-bit original movement code."""
import json
import struct
from pathlib import Path
from unicorn import Uc, UC_ARCH_X86, UC_MODE_16
from unicorn.x86_const import *

ROOT = Path(__file__).resolve().parents[1]
meta = json.loads((ROOT / 'web/solo-assist.json').read_text())
exe = (ROOT / 'web/native/solo-main.exe').read_bytes()
image = exe[struct.unpack_from('<H', exe, 8)[0] * 16:]
CS, DS, SS = 0x1000, 0x4000, 0x7000
CODE, DATA, STACK = CS * 16, DS * 16, SS * 16 + 0xff00
MAIL = CODE + meta['mailboxCsOffset']
hooks = {h['imageOffset']: h['target'] for h in meta['hooks']}


def machine():
    uc = Uc(UC_ARCH_X86, UC_MODE_16)
    uc.mem_map(0, 0x100000)
    uc.mem_write(CODE, image[meta['codeSegment'] * 16:meta['codeSegment'] * 16 + 0x10000])
    for reg, value in [(UC_X86_REG_CS, CS), (UC_X86_REG_DS, DS), (UC_X86_REG_SS, SS), (UC_X86_REG_SP, 0xff00)]:
        uc.reg_write(reg, value)
    return uc


def call(uc, hook, argument=None):
    uc.reg_write(UC_X86_REG_SP, 0xff00)
    uc.mem_write(STACK, struct.pack('<H', 0x5000) + (struct.pack('<H', argument) if argument is not None else b''))
    uc.emu_start(CODE + hooks[hook], CODE + 0x5000, count=10000)
    assert uc.reg_read(UC_X86_REG_SP) == 0xff02 + (2 if argument is not None else 0)


def frame(mode=1, cpu=(0, 1), demo=0, result=0, quit=0):
    uc = machine()
    uc.mem_write(CODE + 0xa438 - meta['codeSegment'] * 16, b'\xc3')
    uc.mem_write(DATA + 0x1d90, struct.pack('<HH', 0, 0x8000))
    uc.mem_write(0x8000e, bytes(cpu))
    uc.mem_write(0x80028, bytes([mode]))
    uc.mem_write(0x80039, bytes([demo]))
    uc.mem_write(DATA + 0x38dc, bytes([result]))
    uc.mem_write(DATA + 0x65a0, bytes([quit]))
    uc.mem_write(MAIL + 26, b'\x03')
    call(uc, 0x977e)
    return uc, (uc.mem_read(MAIL + 22, 1)[0], uc.mem_read(MAIL + 30, 1)[0])


for mode in (1, 0x80):
    assert frame(mode=mode)[1] == (1, 1), 'Human only; CPU focus must remain off'
for kwargs in [dict(mode=0x81, cpu=(0, 0)), dict(mode=0x82), dict(mode=0x7f), dict(demo=1), dict(cpu=(1, 1)), dict(cpu=(1, 0)), dict(result=2), dict(quit=1)]:
    assert frame(**kwargs)[1] == (0, 0), kwargs


def move(mode=0, focus=False, slot=0, blocked=False, delta=(1600, 1600), position=(2000, 3000)):
    uc, _ = frame()
    player = DATA + 0x65a6 + slot * 0x80
    uc.mem_write(player, struct.pack('<hh', *position))
    uc.mem_write(DATA + 0x6588, struct.pack('<H', player - DATA))
    uc.mem_write(DATA + 0x6590, bytes([slot]))
    uc.mem_write(DATA + 0x6582, bytes([64, 64, 45, 45]))
    uc.mem_write(MAIL + 30, bytes([1 if focus else 0]))
    uc.mem_write(MAIL + 32 + slot * 6, struct.pack('<Hhh', mode, *delta))
    if blocked:
        uc.mem_write(player + 0x11, b'\x01')
    call(uc, 0xdaca, 1)  # original INPUT_UP
    return struct.unpack('<hh', uc.mem_read(player, 4)), struct.unpack('<bb', uc.mem_read(DATA + 0x6586, 2))


assert move()[1] == (0, -64)
assert move(focus=True)[1] == (0, -32)
assert move(focus=True, slot=1)[1] == (0, -64)
assert move(mode=3)[0] == (3600, 4600)
assert move(mode=3, delta=(8191, -8192))[0] == (0x1180, 0x180)
assert move(mode=3, slot=1)[0] == (2000, 3000)
assert move(mode=3, blocked=True)[0] == (2000, 3000)
uc, _ = frame()
uc.mem_write(CODE + 0xc7a5 - meta['codeSegment'] * 16, bytes.fromhex('b83412c3'))
call(uc, 0x9842)
assert uc.reg_read(UC_X86_REG_AX) == 0x1234, 'Original pause must be called'
assert uc.mem_read(MAIL + 22, 1) == b'\x00'
assert uc.mem_read(MAIL + 32, 12) == bytes(12)
print('PASS: solo native focus off/on, CPU isolation, Story/VS gating, demo/result/quit exclusion, unlimited bounds, injury block and original pause')
