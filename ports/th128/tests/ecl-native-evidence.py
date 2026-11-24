import argparse
import hashlib
import json
import pathlib
import struct
import sys


EXE_SHA256 = 'ba515750ece845b56640fe440592fc39ebb7bdd1ed87c541b67a19b148e767d4'
BASE = 0x400000
DISPATCH = 0x46FCC0
DISPATCH_INDICES = 0x46FDC0
DISPATCH_END = 0x46FBE5
HANDLERS = {0: 0x46FBE5, 22: 0x46FBE5, 50: 0x46E524,
            71: 0x46EEF5, 72: 0x46F2AE, 84: 0x46F5ED, 85: 0x46F64C}


def float_bits(value):
    return struct.unpack('<I', struct.pack('<f', value))[0]


def emulate(image, opcode, values, expected_type, expected_bits):
    from unicorn import Uc, UC_ARCH_X86, UC_MODE_32, UC_PROT_READ, UC_PROT_EXEC
    from unicorn.x86_const import UC_X86_REG_EBX, UC_X86_REG_ESP, UC_X86_REG_EBP, UC_X86_REG_EIP, UC_X86_REG_FPCW

    emulator = Uc(UC_ARCH_X86, UC_MODE_32)
    for address, size in ((0x46E000, 0x3000), (0x48F000, 0x1000)):
        emulator.mem_map(address, size)
        emulator.mem_write(address, image[address - BASE:address - BASE + size])
        emulator.mem_protect(address, size, UC_PROT_READ | UC_PROT_EXEC)
    state, stack = 0x200000, 0x230000
    emulator.mem_map(state, 0x3000)
    emulator.mem_map(stack, 0x10000)
    emulator.mem_map(0x4DC000, 0x1000)
    emulator.mem_protect(0x4DC000, 0x1000, UC_PROT_READ)
    for index, (kind, bits) in enumerate(values):
        emulator.mem_write(state + 8 + index * 8, struct.pack('<B3xI', ord(kind), bits))
    emulator.mem_write(state + 0x1008, struct.pack('<I', len(values) * 8))
    emulator.reg_write(UC_X86_REG_EBX, state)
    emulator.reg_write(UC_X86_REG_ESP, stack + 0x8000)
    emulator.reg_write(UC_X86_REG_EBP, stack + 0x9000)
    emulator.reg_write(UC_X86_REG_FPCW, 0x37F)
    emulator.emu_start(HANDLERS[opcode], DISPATCH_END, timeout=100000, count=1024)
    assert emulator.reg_read(UC_X86_REG_EIP) == DISPATCH_END, 'handler exceeded bounded dispatch stop'
    assert struct.unpack('<I', emulator.mem_read(state + 0x1008, 4))[0] == 8, 'unexpected native expression stack depth'
    actual_type, actual_bits = struct.unpack('<B3xI', emulator.mem_read(state + 8, 8))
    assert (chr(actual_type), actual_bits) == (expected_type, expected_bits), (
        opcode, values, chr(actual_type), hex(actual_bits), expected_type, hex(expected_bits))
    assert emulator.reg_read(UC_X86_REG_ESP) == stack + 0x8000, 'unbalanced native machine stack'
    return {'opcode': opcode, 'handler': hex(HANDLERS[opcode]),
            'input': [{'type': kind, 'bits': hex(bits)} for kind, bits in values],
            'result': {'type': chr(actual_type), 'bits': hex(actual_bits)}}


def evidence(executable):
    import capstone
    import pefile

    assert executable.stat().st_size <= 16 * 1024 * 1024, 'executable size cap'
    original = executable.read_bytes()
    digest = hashlib.sha256(original).hexdigest()
    assert digest == EXE_SHA256, 'native evidence requires the recorded Japanese executable hash'
    pe = pefile.PE(data=original)
    assert pe.OPTIONAL_HEADER.ImageBase == BASE
    image = pe.get_memory_mapped_image()
    entries = {}
    for opcode, expected in HANDLERS.items():
        entry = struct.unpack_from('<I', image, DISPATCH - BASE + 4 * image[DISPATCH_INDICES - BASE + opcode])[0]
        assert entry == expected, f'changed handler for opcode {opcode}'
        entries[str(opcode)] = hex(entry)
    decoder = capstone.Cs(capstone.CS_ARCH_X86, capstone.CS_MODE_32)
    logical_not = list(decoder.disasm(image[0x46EEF5 - BASE:0x46EF39 - BASE], 0x46EEF5))
    assert any(item.mnemonic == 'sete' and item.op_str == 'cl' for item in logical_not)
    float_negate = list(decoder.disasm(image[0x46F64C - BASE:0x46F6CA - BASE], 0x46F64C))
    assert any(item.mnemonic == 'fchs' for item in float_negate)
    cases = [
        (71, [('i', 0)], 'i', 1),
        (71, [('i', 7)], 'i', 0),
        (71, [('i', 0x80000000)], 'i', 0),
        (71, [('f', float_bits(0.5))], 'i', 1),
        (71, [('f', float_bits(2.75))], 'i', 0),
        (85, [('f', float_bits(3.5))], 'f', float_bits(-3.5)),
        (85, [('f', 0x80000000)], 'f', 0),
        (85, [('i', 7)], 'f', float_bits(-7.0)),
        (84, [('i', 0x80000000)], 'i', 0x80000000),
        (50, [('i', 0x7fffffff), ('i', 1)], 'i', 0x80000000),
    ]
    results = [emulate(image, *case) for case in cases]
    assert hashlib.sha256(executable.read_bytes()).hexdigest() == digest
    return {'schema': 'th128-ecl-native-evidence/1', 'executableSha256': digest,
            'gameExecutableLaunched': False, 'originalFilesWritten': False,
            'dispatchTable': hex(DISPATCH), 'dispatchIndices': hex(DISPATCH_INDICES),
            'handlers': entries, 'instructionBudgetPerProbe': 1024,
            'opcode22Note': 'Retail core dispatch shares the advance-only slot with opcode 0. '
                            'The portable VM deliberately requires host acceptance of decoded Sm registration; '
                            'it does not claim debug callback triggering is verified.',
            'cases': results}


def main():
    root = pathlib.Path(__file__).resolve().parents[1]
    sys.path.insert(0, str(root.parents[1] / 'work/reverse-libs'))
    parser = argparse.ArgumentParser()
    parser.add_argument('--exe', type=pathlib.Path, required=True)
    args = parser.parse_args()
    print(json.dumps(evidence(args.exe), indent=2))


if __name__ == '__main__':
    main()
