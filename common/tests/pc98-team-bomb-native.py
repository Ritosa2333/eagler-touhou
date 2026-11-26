"""Execute the TH04 cooperative hit updates in its retail guest code."""
import re
import subprocess
from pathlib import Path

from unicorn import Uc, UC_ARCH_X86, UC_MODE_16
from unicorn.x86_const import UC_X86_REG_CS, UC_X86_REG_DS, UC_X86_REG_IP, UC_X86_REG_SS, UC_X86_REG_SP

ROOT = Path(__file__).resolve().parents[2]
CS, DS = 0x10000, 0x30000


def images(game, fixture):
    source = (ROOT / f'ports/th{game}/tests/{fixture}').read_text(encoding='utf8')
    script = re.search(r'script="""(.*?)"""', source, re.S).group(1)
    data = subprocess.check_output(['node', '--input-type=module', '-e', script], cwd=ROOT)
    assert len(data) == 4 * 65536
    return [data[start:start+65536] for start in range(0, len(data), 65536)]


def run(game, code, players, active, ghost, protected):
    cpu = Uc(UC_ARCH_X86, UC_MODE_16)
    cpu.mem_map(0, 0x100000)
    cpu.mem_write(CS, code)
    cpu.reg_write(UC_X86_REG_CS, CS // 16)
    cpu.reg_write(UC_X86_REG_DS, DS // 16)
    cpu.reg_write(UC_X86_REG_SS, 0x5000)
    cpu.reg_write(UC_X86_REG_SP, 0xff00)

    def write(address, value):
        cpu.mem_write(address, bytes([value]))

    def read(address):
        return cpu.mem_read(address, 1)[0]

    write(CS + 0x95a2, players)
    write(DS + 0x4368, active)
    banks = [DS + 0x4669, CS + 0x938b, CS + 0xa871]
    out, stride = CS + 0x95ab, 16
    for site in (0xb49f, 0x9c26, 0x8b79, 0x8bb5):
        write(CS + site, 0xc3)
    cpu.mem_write(CS + 0x6021, b'\x5e\xc9\xc3')
    start, end = 0x8817, 0x885a
    timers = [DS + 0x4662, CS + 0x9386, CS + 0xa86c]

    for seat, hit in enumerate(banks):
        write(out + seat * stride, int(seat == ghost))
        write(hit, 1)
        write(timers[seat], 10 if seat == protected else 0)
        write(hit + 1, 0)

    cpu.emu_start(CS + start, CS + end, count=10000)
    assert cpu.reg_read(UC_X86_REG_IP) == end
    for seat, hit in enumerate(banks):
        skipped = seat >= players or seat == ghost
        timer = 10 - int(not skipped) if seat == protected else 0
        assert read(out + seat * stride) == int(seat == ghost), (game, seat, 'ghost changed')
        miss = not skipped and not active and not timer
        assert read(hit) == int(skipped), (game, seat, active)
        assert read(hit + 1) == (0x28 if miss else 0), (game, seat, active, 'miss')
        if miss:
            timer = 0xc0
        assert read(timers[seat]) == timer, (game, seat, 'personal timer', timer)


for game, fixture in [('04', 'ghost-rescue.py')]:
    for code in images(game, fixture):
        for players in (2, 3):
            for active in (0, 1):
                for ghost in (None, *range(players)):
                    for protected in (None, *range(players)):
                        run(game, code, players, active, ghost, protected)
    print(f'TH{game}: team Bomb, personal cooldown, and ghost invariants pass in CN/JP, normal/Extra')
