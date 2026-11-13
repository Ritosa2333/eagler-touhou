"""Read ANM (not ECL) dispatch from the pinned executables of the six shipped titles.

The opcode is at instruction +0, length +2, time +4 and references +6.
ECL instead reads its opcode at +4. Never use these tables for ECL no-ops.
"""
import argparse
import hashlib
import importlib
import json
from pathlib import Path

Image = importlib.import_module('th12-pe').Image
ROOT = Path(__file__).resolve().parents[1]


def handlers(title):
    folder = ROOT / f'runtimes/th{title}'
    target_file = folder / ('th12_web/target.json' if title == 12 else 'target.json')
    target = json.loads(target_file.read_text(encoding='utf-8'))
    img = Image(str((target_file.parent / target['executable']).resolve()))
    if hashlib.sha256(img.data).hexdigest() != target['sha256']:
        raise ValueError(f'th{title}: executable digest differs from target.json')
    if title == 12:
        # movsx eax, word [esi]; inc eax; cmp eax, 0x6f; ja default;
        # jmp [eax*4+0x458730]. Includes the -1 sentinel.
        assert img.read(0x45569d, 7) == bytes.fromhex('0fbf064083f86f')
        result = {op: img.u32(0x458730 + (op + 1)*4) for op in range(-1,111)}
        default = 0x457c91
    elif title == 13:
        # Exact guards, opcode load and indirect jumps anchor the domain.
        assert img.read(0x462514, 8) == bytes.fromhex('0fbf073d2c010000')
        assert img.read(0x4635f0, 10) == bytes.fromhex('2d2d0100003dcf000000')
        assert img.read(0x464c6f, 8) == bytes.fromhex('2d5902000083f807')
        result = {op: img.u32(0x46594c + img.read(0x465a20+op+1,1)[0]*4) for op in range(-1,214)}
        result.update({op: img.u32(0x465af8 + img.read(0x465bec+op-301,1)[0]*4) for op in range(301,509)})
        result.update({op: img.u32(0x465cbc+(op-601)*4) for op in range(601,609)})
        result.update({300:0x463565, 600:0x464c21})
        default = 0x464f24
    elif title == 14:
        # TH14 1.00b reads [ebx+0], not the ECL opcode at +4. Its middle
        # band adds 509 and the shape band extends through 610.
        assert img.read(0x4700a3, 8) == bytes.fromhex('0fbf033d2c010000')
        assert img.read(0x470f99, 10) == bytes.fromhex('2d2d0100003dd0000000')
        assert img.read(0x4728cb, 8) == bytes.fromhex('2d5902000083f809')
        result = {op: img.u32(0x472d60 + img.read(0x472e34+op+1,1)[0]*4) for op in range(-1,214)}
        result.update({op: img.u32(0x472f0c + img.read(0x473010+op-301,1)[0]*4) for op in range(301,510)})
        result.update({op: img.u32(0x4730e4+(op-601)*4) for op in range(601,611)})
        result.update({300:0x470eff, 600:0x47287e})
        default = 0x472ab9
    elif title in (16, 17, 18):
        # The newer titles' two bands, located by the instructions of their own guards rather than by any
        # other title's addresses, since those do not transfer:
        #   first  band: cmp eax,0xd6                      (the whole low family)
        #   second band: sub eax,0x12d; cmp eax,0x138/9   (the shape band; TH13 and TH14 use the same
        #                                                   base with a narrower top)
        # Each table pair is read out of the two instructions that follow the guard, and the target of a
        # rejected opcode is the guard's own jump, decoded from its displacement.
        tops = {16: (0xd6, 0x138), 17: (0xd6, 0x138), 18: (0xd6, 0x139)}
        first_top, second_top = tops[title]

        def va_of(off):
            for _, base, size, raw, count in img.sections:
                if raw <= off < raw + count:
                    return base + (off - raw)
            raise ValueError(f'th{title}: file offset {off:#x} is outside every section')

        def u32_at(off):
            return int.from_bytes(img.data[off:off + 4], 'little')

        def unique(pattern):
            needle = bytes.fromhex(pattern)
            hits, off = [], 0
            while True:
                found = img.data.find(needle, off)
                if found < 0:
                    break
                hits.append(found)
                off = found + 1
            if len(hits) != 1:
                raise ValueError(f'th{title}: {len(hits)} sites for {pattern}')
            return hits[0]

        def guard(pattern):
            # The guard's two instructions plus the two that read and use the tables; that shape, not the
            # bare pattern, is what identifies a band.
            needle = bytes.fromhex(pattern)
            found, off = [], 0
            while True:
                i = img.data.find(needle, off)
                if i < 0:
                    break
                found.append(i)
                off = i + 1
            shaped = []
            for at in found:
                head = at + len(needle) + 4      # the branch's rel32, then the table read
                if img.data[head:head + 2] != b'\x0f\xb6':
                    continue
                base = img.data[head + 2] & 7
                tail = head + 7      # 0f b6 + modrm + the table VA = seven bytes
                # jmp dword [index*4+abs32]: the register is the SIB byte's index field, not modrm's
                if img.data[tail:tail + 2] != b'\xff\x24':
                    continue
                sib = img.data[tail + 2]
                if (sib >> 3) & 7 != base or (sib & 0xC0) != 0x80:
                    continue
                # the table's first entry must be a handler: the table itself is data
                if not img.is_code(img.u32(u32_at(tail + 3))):
                    continue
                shaped.append((at, u32_at(head + 3), u32_at(tail + 3)))
            if len(shaped) != 1:
                raise ValueError(f'th{title}: {len(shaped)} shaped sites for {pattern}')
            at, group, jump = shaped[0]
            # the rejection target is the guard's own jump displacement
            end = at + len(needle)
            rejected = va_of(end) + 4 + int.from_bytes(img.data[end:end + 4], 'little')
            return group, jump, rejected, at

        def imm(value):
            return ' '.join('%02x' % byte for byte in value.to_bytes(4, 'little'))

        group1, jump1, reject, first_at = guard('40 3d ' + imm(first_top) + ' 0f 87')
        group2, jump2, _, second_at = guard('2d 2d 01 00 00 3d ' + imm(second_top) + ' 0f 87')
        result = {op: img.u32(jump1 + img.read(group1 + op + 1, 1)[0] * 4)
                  for op in range(-1, first_top)}
        result.update({op: img.u32(jump2 + img.read(group2 + op - 0x12d, 1)[0] * 4)
                      for op in range(0x12d, 0x12d + second_top + 1)})
        # Before incrementing the low-band index, the dispatcher compares the
        # original signed opcode with 300. Equality has its own sprite handler;
        # greater values enter the second band. Opcode 214 is rejected because
        # its incremented index is 215, greater than the guard's 214.
        prefix = img.data[first_at - 17:first_at]
        assert prefix[:5] == bytes.fromhex('3d2c010000')
        assert prefix[5:7] == bytes.fromhex('0f8f')
        assert prefix[11:13] == bytes.fromhex('0f84')
        greater = va_of(first_at - 6) + int.from_bytes(prefix[7:11], 'little', signed=True)
        assert greater == va_of(second_at)
        result[300] = va_of(first_at) + int.from_bytes(prefix[13:17], 'little', signed=True)
        default = reject
    else:
        raise ValueError('only TH12, TH13, TH14, TH16, TH17 and TH18 ANM dispatches are verified')
    if not all(img.is_code(va) for va in result.values()):
        raise ValueError('dispatch target outside .text')
    return img, result, default


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('title', type=int, choices=[12,13,14,16,17,18])
    parser.add_argument('opcodes', type=int, nargs='*')
    parser.add_argument('--count', type=int, default=28)
    parser.add_argument('--json', action='store_true')
    args = parser.parse_args()
    img, table, default = handlers(args.title)
    if args.json:
        print(json.dumps({'domain':'ANM','title':args.title,'default':hex(default),
                          'handlers':{str(op):hex(va) for op,va in table.items()}}, indent=2))
        return
    from capstone import Cs, CS_ARCH_X86, CS_MODE_32
    md = Cs(CS_ARCH_X86, CS_MODE_32)
    for op in args.opcodes:
        va = table.get(op, default)
        print(f'ANM {op}: {va:#010x}')
        for i, ins in enumerate(md.disasm(img.read(va, args.count*16),va)):
            print(f'  {ins.address:08x} {ins.mnemonic:8s} {ins.op_str}')
            if i+1 >= args.count or ins.mnemonic in ('jmp','ret'):
                break


if __name__ == '__main__':
    main()
