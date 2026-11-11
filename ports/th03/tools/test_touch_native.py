"""Execute the patched TH03 movement hook against native code in 16-bit RAM."""
import json
from pathlib import Path
import struct
from unicorn import Uc, UC_ARCH_X86, UC_MODE_16, UC_HOOK_MEM_WRITE
from unicorn.x86_const import *

ROOT = Path(__file__).resolve().parents[1]
META = json.loads((ROOT / 'web/native-patch.json').read_text())
EXE = (ROOT / 'web/native/main.exe').read_bytes()
IMAGE = EXE[struct.unpack_from('<H', EXE, 8)[0] * 16:]
CS, DS, SS = 0x1000, 0x4000, 0x7000
CODE, DATA = CS * 16, DS * 16
MAIL = CODE + META['mailboxCsOffset']
MOVE = next(h['target'] for h in META['hooks'] if h['imageOffset'] == 0xdaca)


def run(slot=0, mode=1, delta=(1600, 1600), position=(2000, 3000), focus=False, blocked=False):
    uc = Uc(UC_ARCH_X86, UC_MODE_16)
    uc.mem_map(0, 0x100000)
    uc.mem_write(CODE, IMAGE[META['codeSegment'] * 16:META['codeSegment'] * 16 + 0x10000])
    for reg, value in [(UC_X86_REG_CS, CS), (UC_X86_REG_DS, DS), (UC_X86_REG_SS, SS),
                       (UC_X86_REG_SP, 0xff00), (UC_X86_REG_EBX, 0x12345678),
                       (UC_X86_REG_ESI, 0x87654321), (UC_X86_REG_EDI, 0x76543210)]:
        uc.reg_write(reg, value)
    player = DATA + 0x65a6 + slot * 0x80
    uc.mem_write(player, struct.pack('<hh', *position))
    uc.mem_write(DATA + 0x6588, struct.pack('<H', player - DATA))
    uc.mem_write(DATA + 0x6590, bytes([slot]))
    uc.mem_write(DATA + 0x6582, bytes([64, 64, 45, 45]))
    uc.mem_write(MAIL + META['fields']['phase'], b'\x01')
    uc.mem_write(MAIL + 30, bytes([(1 << slot) if focus else 0]))
    if blocked:
        uc.mem_write(player + 0x11, b'\x01')
    touch = MAIL + META['fields']['touch'] + slot * 6
    uc.mem_write(touch, struct.pack('<Hhh', mode, *delta))
    # Near Pascal return address and the input word consumed by the wrapper.
    uc.mem_write(SS * 16 + 0xff00, struct.pack('<HH', 0x5000, 0))
    uc.emu_start(CODE + MOVE, CODE + 0x5000, count=10000)
    result = struct.unpack('<hh', uc.mem_read(player, 4))
    pending = struct.unpack('<Hhh', uc.mem_read(touch, 6))
    assert uc.reg_read(UC_X86_REG_SP) == 0xff04
    # The original input routine owns BX. The wrapper must preserve its result.
    assert uc.reg_read(UC_X86_REG_ESI) == 0x87654321
    assert uc.reg_read(UC_X86_REG_EDI) == 0x76543210
    return result, pending, uc.reg_read(UC_X86_REG_AL)


for slot in (0, 1):
    result, pending, status = run(slot=slot)
    dx, dy = result[0] - 2000, result[1] - 3000
    assert dx == dy and 0 < dx * dx + dy * dy <= 64 * 64, (result, pending)
    assert pending[1:] == (1600 - dx, 1600 - dy) and status == 2
    result, pending, _ = run(slot=slot, focus=True, delta=(-1600, 1600))
    assert 0 < (result[0]-2000)**2 + (result[1]-3000)**2 <= 32**2
    result, pending, _ = run(slot=slot, mode=3)
    assert result == (3600, 4600) and pending == (3, 0, 0)
    result, pending, _ = run(slot=slot, mode=3, delta=(8191, -8192))
    assert result == (0x1180, 0x180) and pending == (3, 0, 0)
    result, pending, _ = run(slot=slot, blocked=True)
    assert result == (2000, 3000) and pending == (0, 0, 0)
    result, pending, _ = run(slot=slot, delta=(0, 0))
    assert result == (2000, 3000) and pending == (1, 0, 0)
def render(always=0, focus=0, points=3, dead=0, result=0):
    uc = Uc(UC_ARCH_X86, UC_MODE_16)
    uc.mem_map(0, 0x100000)
    uc.mem_write(CODE, IMAGE[META['codeSegment'] * 16:META['codeSegment'] * 16 + 0x10000])
    # Isolate the appended marker pass from the unchanged native glow renderer.
    uc.mem_write(CODE + 0xdf18 - META['codeSegment'] * 16, bytes.fromhex('c20200'))
    for reg, value in [(UC_X86_REG_CS, CS), (UC_X86_REG_DS, DS), (UC_X86_REG_SS, SS),
                       (UC_X86_REG_SP, 0xff00)]:
        uc.reg_write(reg, value)
    for slot in (0, 1):
        player = DATA + 0x65a6 + slot * 0x80
        uc.mem_write(player, struct.pack('<hh', 2000, 3000))
        uc.mem_write(player + 0x1f, bytes([bool(dead & (1 << slot))]))
    uc.mem_write(MAIL + META['fields']['alwaysPoint'], bytes([always]))
    uc.mem_write(MAIL + META['fields']['points'], bytes([points]))
    uc.mem_write(MAIL + 30, bytes([focus]))
    uc.mem_write(DATA + 0x38dc, bytes([result]))
    writes = []
    uc.hook_add(UC_HOOK_MEM_WRITE, lambda _, access, address, size, value, user: writes.append(address),
                begin=0xa8000, end=0xaffff)
    uc.mem_write(SS * 16 + 0xff00, struct.pack('<HH', 0x5000, 0))
    hook = next(h['target'] for h in META['hooks'] if h['imageOffset'] == 0x990b)
    uc.emu_start(CODE + hook, CODE + 0x5000, count=10000)
    assert uc.reg_read(UC_X86_REG_SP) == 0xff04
    return writes


assert render() == []
assert render(always=1, points=0) == [0xa8000 + 8017, 0xa8000 + 8097, 0xa8000 + 8177, 0xa8000 + 8097]
assert render(always=2, points=0) == [0xa8000 + 8057, 0xa8000 + 8137, 0xa8000 + 8217, 0xa8000 + 8137]
assert len(render(always=3)) == 8
assert render(focus=1) == render(always=1)
assert render(focus=1, points=0) == []
assert render(always=3, dead=3) == []
assert render(always=3, result=2) == []
print('PASS: both native seats, vector speed limit, focus speed, unlimited drag, field bounds, blocked input, consumption, registers and persistent point rendering')
