"""Execute the guest menu input routine for both games' native key masks."""
from pathlib import Path
import os
import subprocess
import tempfile

from unicorn import Uc, UC_ARCH_X86, UC_MODE_16
from unicorn.x86_const import (UC_X86_REG_AX, UC_X86_REG_BX, UC_X86_REG_CX,
                              UC_X86_REG_CS, UC_X86_REG_DI, UC_X86_REG_DS,
                              UC_X86_REG_DX, UC_X86_REG_ES, UC_X86_REG_SI,
                              UC_X86_REG_SP, UC_X86_REG_SS)

HERE = Path(__file__).resolve().parent
NASM = os.environ.get('NASM', 'nasm')


def run(code, current, previous, selected, rows):
    cpu = Uc(UC_ARCH_X86, UC_MODE_16)
    cpu.mem_map(0, 0x20000)
    cpu.mem_write(0x1000, code)
    cpu.mem_write(0x18000, b'\x00\x20')
    for reg, value in ((UC_X86_REG_AX, current), (UC_X86_REG_BX, previous),
                       (UC_X86_REG_CX, rows << 8 | selected),
                       (UC_X86_REG_SS, 0x1800), (UC_X86_REG_SP, 0)):
        cpu.reg_write(reg, value)
    cpu.emu_start(0x1000, 0x2000, count=100)
    return (cpu.reg_read(UC_X86_REG_AX), cpu.reg_read(UC_X86_REG_BX),
            cpu.reg_read(UC_X86_REG_CX) & 0xff)


def main():
    for game, confirm, cancel in (('th02', 0x90, 0x40), ('th04', 0x2020, 0x1000)):
        with tempfile.TemporaryDirectory() as tmp:
            source = Path(tmp) / 'menu.asm'
            binary = Path(tmp) / 'menu.bin'
            source.write_text(
                'bits 16\n%define PRACTICE_UP 1\n%define PRACTICE_DOWN 2\n'
                '%define PRACTICE_LEFT 4\n%define PRACTICE_RIGHT 8\n'
                f'%define PRACTICE_CONFIRM {confirm}\n%define PRACTICE_CANCEL {cancel}\n'
                f'%include "{(HERE / "practice-menu-input.inc").as_posix()}"\n',
                encoding='ascii')
            subprocess.run([NASM, '-f', 'bin', str(source), '-o', str(binary)], check=True)
            code = binary.read_bytes()
        cases = [
            (0, 0, 0, 3, (0, 0, 0)),
            (1, 0, 0, 3, (1, 1, 2)),
            (2, 0, 2, 3, (1, 2, 0)),
            (2, 2, 1, 3, (0, 2, 1)),
            (4, 0, 1, 3, (4, 4, 1)),
            (8, 0, 1, 3, (5, 8, 1)),
            (confirm & -confirm, 0, 1, 3, (2, confirm & -confirm, 1)),
            (cancel | 1, 0, 1, 3, (3, cancel | 1, 1)),
            (cancel, cancel, 1, 3, (0, cancel, 1)),
            (1, 0, 0, 1, (1, 1, 0)),
        ]
        for current, previous, selected, rows, expected in cases:
            actual = run(code, current, previous, selected, rows)
            assert actual == expected, (game, current, previous, selected, actual, expected)
        print(f'{game}: {len(cases)} native input cases passed')

    with tempfile.TemporaryDirectory() as tmp:
        binary = Path(tmp) / 'values.bin'
        subprocess.run([NASM, '-f', 'bin', str(HERE / 'practice-menu-values.inc'),
                        '-o', str(binary)], check=True)
        code = binary.read_bytes()
    cases = [(0, 0, 0, 6, 0), (4, 0, 0, 6, 6), (5, 6, 0, 6, 0),
             (4, 4, 1, 5, 3), (5, 4, 1, 5, 5), (0, 9, 1, 5, 5),
             (0, 0, 1, 5, 1), (5, 1, 1, 1, 1), (4, 128, 1, 128, 127)]
    for action, value, minimum, maximum, expected in cases:
        cpu = Uc(UC_ARCH_X86, UC_MODE_16)
        cpu.mem_map(0, 0x20000)
        cpu.mem_write(0x1000, code)
        cpu.mem_write(0x18000, b'\x00\x20')
        cpu.mem_write(0x3000, bytes([value]))
        for reg, number in ((UC_X86_REG_AX, action), (UC_X86_REG_DX, maximum << 8 | minimum),
                            (UC_X86_REG_DS, 0), (UC_X86_REG_SI, 0x3000),
                            (UC_X86_REG_SS, 0x1800), (UC_X86_REG_SP, 0)):
            cpu.reg_write(reg, number)
        cpu.emu_start(0x1000, 0x2000, count=100)
        assert cpu.mem_read(0x3000, 1)[0] == expected
        assert cpu.reg_read(UC_X86_REG_AX) & 0xff == expected
    print(f'value editing: {len(cases)} native cases passed')

    # Three exported offsets let the fixture call the actual 16-bit routines.
    with tempfile.TemporaryDirectory() as tmp:
        source, binary = Path(tmp) / 'text.asm', Path(tmp) / 'text.bin'
        source.write_text('bits 16\ndw practice_text_save, practice_text_restore, practice_text_write\n'
                          f'%include "{(HERE / "practice-text.inc").as_posix()}"\n', encoding='ascii')
        subprocess.run([NASM, '-f', 'bin', str(source), '-o', str(binary)], check=True)
        code = binary.read_bytes()
    offsets = [int.from_bytes(code[i:i+2], 'little') for i in (0, 2, 4)]
    cpu = Uc(UC_ARCH_X86, UC_MODE_16)
    cpu.mem_map(0, 0x100000)
    cpu.mem_write(0x1000, code)
    cpu.mem_write(0x18000, b'\x00\x20')
    cpu.mem_write(0xa0000, b'\x41\x00\x42\x00')
    cpu.mem_write(0xa2000, b'\xe1\x00\xc1\x00')
    cpu.mem_write(0x3000, b'OK\0')
    def text_call(entry):
        for reg, number in ((UC_X86_REG_CS, 0x100), (UC_X86_REG_ES, 0xa000),
                            (UC_X86_REG_DS, 0), (UC_X86_REG_DI, 0),
                            (UC_X86_REG_SI, 0x3000), (UC_X86_REG_BX, 0xe1),
                            (UC_X86_REG_CX, 2), (UC_X86_REG_SS, 0x1800),
                            (UC_X86_REG_SP, 0)):
            cpu.reg_write(reg, number)
        cpu.emu_start(0x1000 + entry, 0x3000, count=100)
    text_call(offsets[0])
    text_call(offsets[2])
    assert bytes(cpu.mem_read(0xa0000, 4)) == b'O\0K\0'
    assert bytes(cpu.mem_read(0xa2000, 4)) == b'\xe1\0\xe1\0'
    text_call(offsets[1])
    assert bytes(cpu.mem_read(0xa0000, 4)) == b'A\0B\0'
    assert bytes(cpu.mem_read(0xa2000, 4)) == b'\xe1\0\xc1\0'
    print('text plane: save, draw and restore passed')

    with tempfile.TemporaryDirectory() as tmp:
        source, binary = Path(tmp) / 'assists.asm', Path(tmp) / 'assists.bin'
        source.write_text('bits 16\ndw practice_assists_poll, practice_assists_open, practice_assists\n'
                          f'%include "{(HERE / "practice-assists.inc").as_posix()}"\n', encoding='ascii')
        subprocess.run([NASM, '-f', 'bin', str(source), '-o', str(binary)], check=True)
        code = binary.read_bytes()
    entry, opened, assists = [int.from_bytes(code[i:i+2], 'little') for i in (0, 2, 4)]
    cpu = Uc(UC_ARCH_X86, UC_MODE_16)
    cpu.mem_map(0, 0x30000)
    cpu.mem_write(0x1000, code)
    cpu.mem_write(0x18000, b'\x00\x20')
    def poll(backspace, functions, u):
        for reg, number in ((UC_X86_REG_CS, 0x100), (UC_X86_REG_AX, functions << 8 | backspace),
                            (UC_X86_REG_DX, u), (UC_X86_REG_SS, 0x1800), (UC_X86_REG_SP, 0)):
            cpu.reg_write(reg, number)
        cpu.emu_start(0x1000 + entry, 0x3000, count=100)
        return tuple(cpu.mem_read(0x1000 + at, 1)[0] for at in (opened, assists))
    assert poll(0, 4, 0) == (0, 0)  # F1 while closed
    assert poll(0x40, 4, 0) == (1, 0)  # held F1 does not activate on opening
    assert poll(0x40, 4, 0) == (1, 0)
    assert poll(0, 0, 0) == (1, 0)
    assert poll(0, 4, 0x40) == (1, 0x21)
    assert poll(0, 4, 0x40) == (1, 0x21)
    assert poll(0x40, 0, 0) == (0, 0x21)
    assert poll(0, 4, 0) == (0, 0x21)
    print('guest assist toggles: 8 sequential cases passed')

    with tempfile.TemporaryDirectory() as tmp:
        source, binary = Path(tmp) / 'draw.asm', Path(tmp) / 'draw.bin'
        includes = ('practice-assists.inc', 'practice-assists-draw.inc', 'practice-text.inc')
        source.write_text('bits 16\ndw practice_assists_draw, practice_assists_open, practice_assists\n'
                          + ''.join(f'%include "{(HERE / name).as_posix()}"\n' for name in includes),
                          encoding='ascii')
        subprocess.run([NASM, '-f', 'bin', str(source), '-o', str(binary)], check=True)
        code = binary.read_bytes()
    entry, opened, assists = [int.from_bytes(code[i:i+2], 'little') for i in (0, 2, 4)]
    cpu = Uc(UC_ARCH_X86, UC_MODE_16)
    cpu.mem_map(0, 0x100000)
    cpu.mem_write(0x1000, code)
    original = bytes((n * 7) & 255 for n in range(1200))
    attrs = bytes((n * 11) & 255 for n in range(1200))
    cpu.mem_write(0xa0000, original)
    cpu.mem_write(0xa2000, attrs)
    cpu.mem_write(0x18000, b'\x00\x20')
    def draw(open_value, flag_value):
        cpu.mem_write(0x1000 + opened, bytes([open_value]))
        cpu.mem_write(0x1000 + assists, bytes([flag_value]))
        cpu.reg_write(UC_X86_REG_CS, 0x100)
        cpu.reg_write(UC_X86_REG_SS, 0x1800)
        cpu.reg_write(UC_X86_REG_SP, 0)
        cpu.emu_start(0x1000 + entry, 0x3000, count=10000)
    draw(1, 0x21)
    assert bytes(cpu.mem_read(0xa0000 + 176, 6)) == b'[\x00F\x001\x00'
    assert cpu.mem_read(0xa0000 + 176 + 5*160, 1)[0] == ord('[')
    draw(1, 0)
    assert cpu.mem_read(0xa0000 + 176, 1)[0] == ord(' ')
    draw(0, 0)
    assert bytes(cpu.mem_read(0xa0000, 1200)) == original
    assert bytes(cpu.mem_read(0xa2000, 1200)) == attrs
    print('guest assist menu: draw, update and restore passed')

    with tempfile.TemporaryDirectory() as tmp:
        source, binary = Path(tmp) / 'power.asm', Path(tmp) / 'power.bin'
        source.write_text('bits 16\ndw practice_power_lock, practice_assists, updates\n'
                          '%define POWER_BYTE 0x3000\n%define POWER_REFRESH call refresh\n'
                          'refresh: inc byte [cs:updates]\nret\nupdates: db 0\n'
                          'practice_assists: db 0\n'
                          f'%include "{(HERE / "practice-power.inc").as_posix()}"\n', encoding='ascii')
        subprocess.run([NASM, '-f', 'bin', str(source), '-o', str(binary)], check=True)
        code = binary.read_bytes()
    entry, assists, updates = [int.from_bytes(code[i:i+2], 'little') for i in (0, 2, 4)]
    cpu = Uc(UC_ARCH_X86, UC_MODE_16)
    cpu.mem_map(0, 0x30000)
    cpu.mem_write(0x1000, code)
    cpu.mem_write(0x18000, b'\x00\x20')
    def power(flag, value):
        cpu.mem_write(0x1000 + assists, bytes([flag]))
        cpu.mem_write(0x3000, bytes([value]))
        for reg, number in ((UC_X86_REG_CS, 0x100), (UC_X86_REG_DS, 0),
                            (UC_X86_REG_SS, 0x1800), (UC_X86_REG_SP, 0)):
            cpu.reg_write(reg, number)
        cpu.emu_start(0x1000 + entry, 0x3000, count=100)
        return cpu.mem_read(0x3000, 1)[0], cpu.mem_read(0x1000 + updates, 1)[0]
    assert power(8, 32) == (32, 0)
    assert power(8, 60) == (32, 1)
    assert power(8, 32) == (32, 1)
    assert power(0, 60) == (60, 1)
    assert power(8, 60) == (60, 1)
    assert power(8, 10) == (60, 2)
    print('guest power lock: capture, restore and release passed')


if __name__ == '__main__':
    main()
