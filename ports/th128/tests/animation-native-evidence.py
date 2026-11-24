import argparse
import collections
import hashlib
import json
import pathlib
import struct
import sys


EXE_SHA256 = 'ba515750ece845b56640fe440592fc39ebb7bdd1ed87c541b67a19b148e767d4'
HANDLERS = {111: (0x45A4A1, 0x45A4D6), 113: (0x45B3E1, 0x45B48E),
            114: (0x45B7B9, 0x45B810)}


def emulate_flag_handler(image, value, mask, previous, local):
    from unicorn import Uc, UC_ARCH_X86, UC_MODE_32, UC_PROT_EXEC, UC_PROT_READ
    from unicorn.x86_const import UC_X86_REG_EDI, UC_X86_REG_ESI, UC_X86_REG_ESP, UC_X86_REG_EIP

    emulator = Uc(UC_ARCH_X86, UC_MODE_32)
    for address in (0x459000, 0x45A000):
        emulator.mem_map(address, 4096)
        emulator.mem_write(address, image.get_data(address - 0x400000, 4096))
        emulator.mem_protect(address, 4096, UC_PROT_EXEC | UC_PROT_READ)
    state, script, stack = 0x100000, 0x110000, 0x120000
    for address in (state, script, stack):
        emulator.mem_map(address, 4096)
    emulator.mem_write(state + 0x4B4, struct.pack('<I', previous))
    emulator.mem_write(state + 0x430, struct.pack('<I', local))
    emulator.mem_write(script, struct.pack('<HHhHI', 111, 12, 0, mask, value))
    emulator.reg_write(UC_X86_REG_EDI, state)
    emulator.reg_write(UC_X86_REG_ESI, script)
    emulator.reg_write(UC_X86_REG_ESP, stack + 2048)
    emulator.emu_start(0x45A4A1, 0x459E1A, count=128)
    assert emulator.reg_read(UC_X86_REG_EIP) == 0x459E1A, 'bounded handler did not return to dispatch'
    assert emulator.reg_read(UC_X86_REG_ESP) == stack + 2048, 'stack imbalance'
    flags = struct.unpack('<I', emulator.mem_read(state + 0x4B4, 4))[0]
    next_pc = struct.unpack('<I', emulator.mem_read(state + 0x424, 4))[0]
    resolved = local if mask and value == 10000 else value
    assert flags == ((previous & ~0xC0) | ((resolved << 6) & 0xC0))
    assert next_pc == script + 12
    return {'value': value, 'mask': mask, 'previousFlags2': previous,
            'integer0': local, 'resultFlags2': flags, 'nextPcDelta': next_pc - script}


def emulate_angle_normalization(image, value):
    from unicorn import Uc, UC_ARCH_X86, UC_MODE_32, UC_PROT_EXEC, UC_PROT_READ
    from unicorn.x86_const import UC_X86_REG_ESP, UC_X86_REG_EIP, UC_X86_REG_FPCW

    emulator = Uc(UC_ARCH_X86, UC_MODE_32)
    for address in (0x46A000, 0x4A6000):
        emulator.mem_map(address, 4096)
        emulator.mem_write(address, image.get_data(address - 0x400000, 4096))
        emulator.mem_protect(address, 4096, UC_PROT_EXEC | UC_PROT_READ)
    stack, output, return_trap = 0x100000, 0x110000, 0x120000
    for address in (stack, output, return_trap):
        emulator.mem_map(address, 4096)
    emulator.mem_write(return_trap, b'\xd9\x1d' + struct.pack('<I', output))
    emulator.mem_protect(return_trap, 4096, UC_PROT_EXEC | UC_PROT_READ)
    stack_pointer = stack + 2048
    input_bytes = struct.pack('<f', value)
    emulator.mem_write(stack_pointer, struct.pack('<I', return_trap) + input_bytes)
    emulator.reg_write(UC_X86_REG_ESP, stack_pointer)
    emulator.reg_write(UC_X86_REG_FPCW, 0x037F)
    emulator.emu_start(0x46A700, return_trap + 6, count=1024)
    assert emulator.reg_read(UC_X86_REG_EIP) == return_trap + 6, 'angle helper exceeded cap'
    assert emulator.reg_read(UC_X86_REG_ESP) == stack_pointer + 8, 'angle helper stack imbalance'
    actual = bytes(emulator.mem_read(output, 4))
    expected = struct.unpack('<f', input_bytes)[0]
    pi, tau = struct.unpack('<2f', struct.pack('<2I', 0x40490FDB, 0x40C90FDB))
    steps = 0
    while expected > pi and steps < 34:
        expected = struct.unpack('<f', struct.pack('<f', expected - tau))[0]
        steps += 1
    while expected < -pi and steps < 34:
        expected = struct.unpack('<f', struct.pack('<f', expected + tau))[0]
        steps += 1
    assert actual == struct.pack('<f', expected), 'native bounded angle mismatch'
    return {'inputFloatBits': input_bytes.hex(), 'outputFloatBits': actual.hex()}


def evidence(executable, root):
    import capstone
    import pefile

    with executable.open('rb') as source:
        data = source.read(16 * 1024 * 1024 + 1)
    assert len(data) <= 16 * 1024 * 1024, 'executable size cap'
    digest = hashlib.sha256(data).hexdigest()
    assert digest == EXE_SHA256, 'not the pinned Japanese TH128 PE'
    image = pefile.PE(data=data)
    assert image.OPTIONAL_HEADER.ImageBase == 0x400000
    table = struct.unpack('<116I', image.get_data(0x5D0D4, 464))
    decoder = capstone.Cs(capstone.CS_ARCH_X86, capstone.CS_MODE_32)
    handlers = {}
    for opcode, (start, end) in HANDLERS.items():
        assert table[opcode + 1] == start, 'dispatch-table evidence mismatch'
        code = image.get_data(start - 0x400000, end - start)
        handlers[str(opcode)] = {
            'address': hex(start), 'endExclusive': hex(end),
            'sha256': hashlib.sha256(code).hexdigest(), 'bytes': code.hex(),
            'instructions': [{'address': hex(item.address), 'bytes': item.bytes.hex(),
                              'mnemonic': item.mnemonic, 'operands': item.op_str}
                             for item in decoder.disasm(code, start)]}
    cases = [emulate_flag_handler(image, value, 0, previous, 0)
             for previous in (0, 0xFFFFFFFF, 0xA5A55A5A)
             for value in (0, 1, 2, 3, 4, 0xFFFFFFFF)]
    cases += [emulate_flag_handler(image, 10000, 1, 0xA5A55A5A, value)
              for value in (0, 1, 2, 3, 0xFFFFFFFF)]
    cases += [emulate_flag_handler(image, 12345, 1, 0xA5A55A5A, 3)]
    angle_values = [0, 1, -1, 3.1415927410125732, -3.1415927410125732,
                    9.42477798461914, -9.42477798461914, 1000, -1000,
                    3.4028234663852886e38, -3.4028234663852886e38]
    angle_cases = [emulate_angle_normalization(image, value) for value in angle_values]
    catalog = json.loads((root / 'local/catalog.json').read_text(encoding='utf-8'))
    manifest_path = root / 'local' / catalog['variants']['ja']['manifest']
    manifest = json.loads(manifest_path.read_text(encoding='utf-8'))
    occurrences = {str(opcode): [] for opcode in HANDLERS}
    for animation in manifest['animations']:
        path = manifest_path.parent / animation['file']
        parsed = json.loads(path.read_text(encoding='utf-8'))
        for script in parsed['scripts']:
            for command in script['commands']:
                if str(command['opcode']) in occurrences:
                    occurrences[str(command['opcode'])].append({
                        'file': animation['name'], 'globalScript': script['index'],
                        **{key: command[key] for key in ('offset', 'time', 'mask', 'raw')}})
    assert [len(occurrences[str(opcode)]) for opcode in HANDLERS] == [163, 20, 1]
    assert hashlib.sha256(executable.read_bytes()).hexdigest() == digest
    return {'schema': 'th128-anm-native-evidence/1',
            'date': '2026-10-29', 'executableSha256': digest,
            'windowsGameProcessLaunched': False, 'originalFilesWritten': False,
            'dispatch': {'address': '0x459e31', 'table': '0x45d0d4',
                         'rule': 'sign-extend opcode; add one; reject index > 115'},
            'handlers': handlers, 'opcode111BoundedEmulationCases': cases,
            'normalization': {'address': '0x46a700', 'emulationInstructionCap': 1024,
                              'cases': angle_cases, 'maximumAdjustments': 34},
            'emulationInstructionCap': 128, 'occurrences': occurrences,
            'coverage': manifest['opcodeCoverage']['anmObserved'],
            'limits': [
                'Opcode 111 flag write only is function-emulated; external update gates are not.',
                'Opcodes 113 and 114 are statically inspected, not function-emulated.',
                'All three opcodes remain explicit runtime stops, not assumed compatible.',
                'No Windows APIs, game loop, render loop, or allocator are emulated.']}


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument('--exe', type=pathlib.Path, required=True)
    parser.add_argument('--report', type=pathlib.Path)
    args = parser.parse_args()
    root = pathlib.Path(__file__).resolve().parents[1]
    if args.report:
        destination = args.report.resolve()
        assert destination.parent == (root / 'local/research').resolve()
        assert destination.name.startswith('anm-') and destination.suffix == '.json'
    sys.path.insert(0, str(root.parents[1] / 'work/reverse-libs'))
    report = evidence(args.exe, root)
    if args.report:
        destination.parent.mkdir(parents=True, exist_ok=True)
        destination.write_text(json.dumps(report, indent=2) + '\n', encoding='utf-8')
    print('Original-byte ANM evidence: 3 handlers; 24 bounded opcode111 cases; '
          '163/20/1 original occurrences; no game process launched.')
    return 0


if __name__ == '__main__':
    raise SystemExit(main())
