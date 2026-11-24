#!/usr/bin/env python3
"""核查源码引用的每个原版代码地址恰是函数入口、指令边界或内嵌指针表（反幻觉地址的门禁）。

分类（逐个不同地址）：
  entry     Ghidra 导出函数的入口
  listed    导出函数体内，反汇编列表里恰是一条指令的首地址
  operand   导出函数体内，恰是某条指令的 32 位立即数或位移操作数（常量、全局地址的字节位置）
  table     代码节内嵌的跳转表或指针表（连续 u32 且都指向代码节）
  sweep     不在导出函数内，从最近的直接调用目标线性反汇编后恰是一条指令的首地址
  callback  不在导出函数内、没有直接调用者的函数入口：前面是 int3 填充，或地址作为 u32 出现在
            映像别处（回调表、虚表）
  data      头部、非代码节，或代码节里登记在 DATA_IN_CODE 的数据表
其余列为 UNVERIFIED 并以 1 退出。
用法（容器内，th13-dev 镜像）：
  python3 tools/decomp/address-check.py <端口目录> <decomp 目录> <th13.exe> [输出 tsv]
"""
import bisect
import json
import pathlib
import re
import struct
import sys

import capstone
from capstone.x86 import X86_OP_IMM, X86_OP_MEM

port, decomp, exe_path = (pathlib.Path(arg) for arg in sys.argv[1:4])
image = exe_path.read_bytes()
pe = struct.unpack_from('<I', image, 0x3c)[0]
count = struct.unpack_from('<H', image, pe + 6)[0]
image_base = struct.unpack_from('<I', image, pe + 24 + 28)[0]
table = pe + 24 + struct.unpack_from('<H', image, pe + 20)[0]
regions = [('headers', image_base, 0, image_base + struct.unpack_from('<I', image, pe + 24 + 60)[0])]
for index in range(count):
    name, vsize, va, rsize, roff = struct.unpack_from('<8sIIII', image, table + index * 40)
    regions.append((name.rstrip(b'\0').decode(), image_base + va, roff, image_base + va + (vsize or rsize)))
text_va, text_off, text_end = next((start, roff, end) for name, start, roff, end in regions if name == '.text')


def offset_of(address):
    for _, start, roff, end in regions:
        if start <= address < end:
            return roff + address - start
    raise SystemExit(f'0x{address:x} 不在映像内')


# 代码节里按数据用的表：地址 → 说明。
DATA_IN_CODE = {0x41F9D8: 'ECL 指令二级分派的字节表，索引 0x41f760 的处理函数指针表'}

functions = sorted((int(item['address'], 16), item['bodyBytes'])
                   for item in json.loads((decomp / 'functions.json').read_text())['functions'])
starts = [start for start, _ in functions]
listed = {}
for asm in (decomp / 'functions').glob('*.asm'):
    for address, raw, _ in re.findall(r'^([0-9a-f]{8})\s+([0-9a-f]+)\s+(.*)$', asm.read_text(errors='replace'),
                                      re.MULTILINE):
        listed[int(address, 16)] = len(raw) // 2
ordered = sorted(listed)

call_targets = set()
for offset in range(text_end - text_va - 5):
    if image[text_off + offset] in (0xE8, 0xE9):
        target = text_va + offset + 5 + struct.unpack_from('<i', image, text_off + offset + 1)[0]
        if text_va <= target < text_end:
            call_targets.add(target)
call_targets = sorted(call_targets)

sites = {}
skip_dirs = {'third_party', 'build', 'node_modules'}
skip_files = {'MsgPlayerCp932.hpp', 'deflate_golden.hpp', 'color01_expected.hpp', 'rgba64_png.hpp'}
for root in ('web/cpp', 'portable'):
    for path in (port / root).rglob('*'):
        if not path.is_file() or path.suffix not in ('.cpp', '.hpp', '.inc', '.h'):
            continue
        if skip_dirs & set(path.relative_to(port / root).parts) or path.name in skip_files:
            continue
        if path.name.endswith('.gen.hpp'):
            continue
        name = str(path.relative_to(port / 'web/cpp')) if root == 'web/cpp' else str(path.relative_to(port))
        for token in re.findall(r'0x4[0-9a-fA-F]{5}\b', path.read_text(encoding='utf-8')):
            sites.setdefault(int(token, 16), set()).add(name)

disassembler = capstone.Cs(capstone.CS_ARCH_X86, capstone.CS_MODE_32)
disassembler.detail = True


def in_exported_function(address):
    position = bisect.bisect_right(starts, address) - 1
    if position < 0:
        return None
    start, size = functions[position]
    return start if start <= address < start + size else None


def is_pointer_table(address):
    if address % 4 or offset_of(address) + 8 > text_off + text_end - text_va:
        return False
    first, second = struct.unpack_from('<II', image, offset_of(address))
    return all(text_va <= value < text_end for value in (first, second))


def is_operand_start(address):
    """address 恰是包含它的那条指令里 32 位立即数或位移操作数的首字节。"""
    position = bisect.bisect_right(ordered, address) - 1
    if position < 0:
        return False
    start = ordered[position]
    if not start < address < start + listed[start]:
        return False
    instruction = next(disassembler.disasm(image[offset_of(start):offset_of(start) + 16], start), None)
    if instruction is None or instruction.size < 4:
        return False
    # 立即数在指令末尾；指令没有立即数时，无基址的位移是最后 4 字节。
    candidates = [(operand.imm, instruction.size - operand.size) for operand in instruction.operands
                  if operand.type == X86_OP_IMM and operand.size == 4]
    if not any(operand.type == X86_OP_IMM for operand in instruction.operands):
        candidates += [(operand.mem.disp, instruction.size - 4) for operand in instruction.operands
                       if operand.type == X86_OP_MEM and not operand.mem.base and not operand.mem.index]
    return any(start + at == address and
               struct.unpack_from('<I', instruction.bytes, at)[0] == value & 0xffffffff
               for value, at in candidates if 0 <= at <= instruction.size - 4)


def is_callback_entry(address):
    """没有直接调用者的函数入口：前一个字节是 int3，或地址作为 u32 出现在映像别处。"""
    if image[offset_of(address) - 1] == 0xCC:
        return True
    needle = struct.pack('<I', address)
    position = image.find(needle)
    while position >= 0:
        if position % 4 == 0 and position != offset_of(address):
            return True
        position = image.find(needle, position + 1)
    return False


def sweep_reaches(address):
    """从不超过 address 的最近直接调用目标线性反汇编，返回是否恰在 address 处起一条指令。"""
    position = bisect.bisect_right(call_targets, address) - 1
    while position >= 0:
        start = call_targets[position]
        position -= 1
        if address - start > 0x3000:
            return None
        for instruction in disassembler.disasm(image[offset_of(start):offset_of(address) + 16], start):
            if instruction.address == address:
                return f'{instruction.mnemonic} {instruction.op_str}'.strip()
            if instruction.address > address:
                break
    return None


def classify(address):
    """返回 (类别, 说明)；类别以 UNVERIFIED 开头表示未能归类。"""
    start = in_exported_function(address)
    if start is not None:
        if start == address:
            return 'entry', ''
        if address in listed:
            return 'listed', ''
        return ('operand', '') if is_operand_start(address) else ('UNVERIFIED-in-function', f'function 0x{start:x}')
    if not text_va <= address < text_end:
        region = next((name for name, low, _, high in regions if low <= address < high), None)
        return ('data', region) if region else ('UNVERIFIED-outside', '')
    if address in DATA_IN_CODE:
        return 'data', DATA_IN_CODE[address]
    if is_pointer_table(address):
        return 'table', ''
    reached = sweep_reaches(address)
    if reached:
        return 'sweep', reached
    return ('callback', '') if is_callback_entry(address) else ('UNVERIFIED-sweep', '')


classes, unverified, rows = {}, [], ['address\tclass\tdetail\tfiles']
for address in sorted(sites):
    kind, detail = classify(address)
    classes.setdefault(kind, []).append(address)
    rows.append(f'0x{address:06x}\t{kind}\t{detail}\t' + ' '.join(sorted(sites[address])))
    if kind.startswith('UNVERIFIED'):
        unverified.append((address, kind, detail))
for kind, addresses in sorted(classes.items()):
    print(f'{kind:24s} {len(addresses)}')
for address, kind, detail in unverified:
    print(f'  UNVERIFIED 0x{address:06x} {kind} {detail} ' + ' '.join(sorted(sites[address])[:3]))
if len(sys.argv) > 4:
    pathlib.Path(sys.argv[4]).write_text('\n'.join(rows) + '\n')

sys.exit(1 if unverified else 0)
