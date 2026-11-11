"""Read-only disassembly of the local original TH03 executable."""
import argparse
import gzip
import hashlib
from pathlib import Path
import struct
from prepare_assets import files

ROOT = Path(__file__).resolve().parents[1]


def executable(lang='jp', path='YUMEZIKU/MAIN.EXE'):
    data = gzip.decompress((ROOT / f'web/disks/3-{lang}.hdi.gz').read_bytes())
    entry = next(e for e in files(data) if e[0] == path)
    return b''.join(data[at:at + 8192] for at in entry[1])[:entry[2]]


def main():
    from capstone import Cs, CS_ARCH_X86, CS_MODE_16
    parser = argparse.ArgumentParser()
    parser.add_argument('--lang', default='jp')
    parser.add_argument('--start', type=lambda x: int(x, 16), default=0)
    parser.add_argument('--size', type=lambda x: int(x, 16), default=0x180)
    parser.add_argument('--find', default='')
    parser.add_argument('--file', default='YUMEZIKU/MAIN.EXE')
    parser.add_argument('--patched', action='store_true')
    parser.add_argument('--reloc-segment', type=lambda x: int(x, 16))
    args = parser.parse_args()
    exe = (ROOT / 'web/native/main.exe').read_bytes() if args.patched else executable(args.lang, args.file)
    hsize = struct.unpack_from('<H', exe, 8)[0] * 16 if exe[:2] == b'MZ' else 0
    image = exe[hsize:]
    print(f'SHA256={hashlib.sha256(exe).hexdigest()} header={hsize:#x} '
          f'image={len(image):#x} entry={struct.unpack_from("<H",exe,22)[0]:04x}:'
          f'{struct.unpack_from("<H",exe,20)[0]:04x}')
    if args.reloc_segment is not None:
        count = struct.unpack_from('<H', exe, 6)[0]
        table = struct.unpack_from('<H', exe, 24)[0]
        for i in range(count):
            off, seg = struct.unpack_from('<HH', exe, table + i * 4)
            at = seg * 16 + off
            target = struct.unpack_from('<H', image, at)[0]
            if target == args.reloc_segment:
                print(f'{at:#x}: segment={target:#x} context={image[max(0,at-8):at+4].hex()}')
        return
    if args.find:
        needle = bytes.fromhex(args.find)
        at = image.find(needle)
        while at >= 0:
            print(f'{at:05x}: {image[at:at+len(needle)+16].hex()}')
            at = image.find(needle, at + 1)
        return
    for ins in Cs(CS_ARCH_X86, CS_MODE_16).disasm(image[args.start:args.start + args.size], args.start):
        print(f'{ins.address:05x} {ins.bytes.hex():22} {ins.mnemonic} {ins.op_str}')


if __name__ == '__main__':
    main()
