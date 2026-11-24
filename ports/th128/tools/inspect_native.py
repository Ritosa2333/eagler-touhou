import argparse
import importlib.metadata
import sys
from pathlib import Path

from th128_assets.formats import FormatError, require
from th128_assets.pipeline import atomic_json, identity, validate_report_destination


def find_string_addresses(data, base, names):
    targets = {}
    for name in names:
        token = name.encode('ascii') + b'\0'
        position, addresses = 0, []
        while True:
            offset = data.find(token, position)
            if offset < 0:
                break
            if offset == 0 or data[offset - 1] == 0:
                addresses.append(base + offset)
            position = offset + len(token)
        targets[name] = addresses
    return targets


def scan_references(code, address, targets):
    import capstone
    from capstone.x86_const import X86_OP_IMM, X86_OP_MEM

    lookup, references = {}, {name: [] for name in targets}
    for name, addresses in targets.items():
        for target in addresses:
            lookup.setdefault(target, []).append(name)
    decoder = capstone.Cs(capstone.CS_ARCH_X86, capstone.CS_MODE_32)
    decoder.detail = True
    decoder.skipdata = True
    previous = []
    for instruction in decoder.disasm(code, address):
        if not instruction.id:
            previous.clear()
            continue
        found = set()
        for operand in instruction.operands:
            if operand.type == X86_OP_IMM:
                found.update(lookup.get(operand.imm & 0xFFFFFFFF, []))
            elif operand.type == X86_OP_MEM and operand.mem.base == 0 and operand.mem.index == 0:
                found.update(lookup.get(operand.mem.disp & 0xFFFFFFFF, []))
        record = {'address': f'0x{instruction.address:08x}', 'bytes': instruction.bytes.hex(),
                  'mnemonic': instruction.mnemonic, 'operands': instruction.op_str}
        for name in found:
            references[name].append({**record, 'precedingInstructions': previous[-3:]})
        previous.append(record)
        if len(previous) > 3:
            previous.pop(0)
    return references


def inspect(executable):
    import pefile

    with executable.open('rb') as source:
        binary = source.read(16 * 1024 * 1024 + 1)
    require(len(binary) <= 16 * 1024 * 1024, 'unexpectedly large TH128 executable')
    image = pefile.PE(data=binary)
    require(image.FILE_HEADER.Machine == 0x14C and image.OPTIONAL_HEADER.Magic == 0x10B, 'expected x86 PE32')
    base = image.OPTIONAL_HEADER.ImageBase
    names = ['ice.anm', 'pl00.anm', 'title.anm', 'title_v.anm', 'bullet.anm',
             'st_a1_1.ecl', 'stage_a1.std', 'thbgm.fmt']
    targets = {name: [] for name in names}
    for section in image.sections:
        found = find_string_addresses(section.get_data(), base + section.VirtualAddress, names)
        for name, addresses in found.items():
            targets[name].extend(addresses)
    hints = {'thprac-player-pointer': 0x4B8A80, 'thprac-motivation': 0x4B4D64,
             'thprac-perfect-freeze': 0x4B4D70, 'thprac-ice-power': 0x4B4D6C,
             'thprac-freeze-area': 0x4B4D60}
    targets.update({name: [address] for name, address in hints.items()})
    references = {name: [] for name in targets}
    for section in image.sections:
        if section.Characteristics & 0x20000000:
            found = scan_references(section.get_data(), base + section.VirtualAddress, targets)
            for name, values in found.items():
                references[name].extend(values)
    report = {'schema': 'th128-static-reference-index/1', 'executable': identity(executable),
              'executed': False, 'sourceRecovered': False, 'imageBase': f'0x{base:08x}',
              'entryPoint': f'0x{base + image.OPTIONAL_HEADER.AddressOfEntryPoint:08x}',
              'analysisTools': {name: importlib.metadata.version(name) for name in ('pefile', 'capstone')},
              'targets': {name: [f'0x{address:08x}' for address in addresses] for name, addresses in targets.items()},
              'references': references,
              'externalHints': {'origin': 'thprac/src/thprac_th128.cpp',
                                'commit': '33c78d989cca8891af07b513b468d88323c4fc1e',
                                'nativeFieldSemanticsDynamicallyVerified': False},
              'limitations': ['Linear disassembly is not a recovered function graph or original source.',
                              'Absolute operands are static candidates, not executed evidence.',
                              'Labels from thprac remain hypotheses until checked on this executable.']}
    image.close()
    return report


def main(argv=None):
    parser = argparse.ArgumentParser(description='Read-only x86 static reference index; does not run the original game.')
    parser.add_argument('--exe', type=Path, required=True)
    parser.add_argument('--report', type=Path, default=Path(__file__).resolve().parents[1] / 'docs/native-static-index.json')
    arguments = parser.parse_args(argv)
    try:
        protected = [arguments.exe.resolve().parent]
        validate_report_destination(arguments.report, protected)
        report = inspect(arguments.exe)
        atomic_json(validate_report_destination(arguments.report, protected), report)
        for name, references in report['references'].items():
            print(f'{name}: {len(references)} static references')
        print(f'Written: {arguments.report}')
    except (FormatError, OSError, ImportError, ValueError) as error:
        print(f'Static inspection failed: {error}', file=sys.stderr)
        return 1
    return 0


if __name__ == '__main__':
    raise SystemExit(main())
