"""Link verified x86 C++ functions into a copy of the original PE.

This is an incremental binary/source reconstruction, not a full source port.
Original sections, entry point and engine code remain in the new executable.
Windows resolves an added normal DLL import; seven entries tail-jump to C++.
"""
from __future__ import annotations

import argparse
import hashlib
import json
import shutil
import struct
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
sys.path.insert(0, str(ROOT / '.cache/binary_python'))
import pefile

PATCHES = [
    (0x22CB0, 'th20_lcg_step'),
    (0x235F0, 'th20_lcg_next'),
    (0x23F50, 'th20_timer_reset'),
    (0x23FE0, 'th20_timer_mode'),
    (0x23F80, 'th20_timer_set'),
    (0x530F0, 'th20_timer_add'),
    (0x533B0, 'th20_timer_tick'),
]
DLL_NAME = 'th20_native_bridge.dll'
EXPECTED_EXE_SHA = 'a274b45fe6ec53511718bb328c2ff169a74e67f95d1b0c74d97d348b955a0897'
PACKAGE_FILES = ['th20_rebuilt.exe', DLL_NAME, 'th20.dat', 'thbgm.dat', 'custom.exe',
                 'Play.cmd', 'Configure.cmd', 'README.txt']


def sha(path: Path) -> str:
    with path.open('rb') as f:
        return hashlib.file_digest(f, 'sha256').hexdigest()


def align(value: int, alignment: int) -> int:
    return (value + alignment - 1) // alignment * alignment


def require(condition: bool, message: str):
    if not condition:
        raise ValueError(message)


def check_not_running(path: Path):
    # Fail before touching an existing package if Windows has mapped its EXE/DLL.
    if sys.platform != 'win32' or not path.exists():
        return
    import ctypes
    from ctypes import wintypes
    kernel = ctypes.WinDLL('kernel32', use_last_error=True)
    kernel.CreateFileW.argtypes = [wintypes.LPCWSTR, wintypes.DWORD, wintypes.DWORD,
                                  ctypes.c_void_p, wintypes.DWORD, wintypes.DWORD, wintypes.HANDLE]
    kernel.CreateFileW.restype = wintypes.HANDLE
    kernel.CloseHandle.argtypes = [wintypes.HANDLE]
    handle = kernel.CreateFileW(str(path), 0x40000000, 0, None, 3, 0x80, None)
    if handle == ctypes.c_void_p(-1).value:
        raise OSError(ctypes.get_last_error(), f'Close the game/configuration tool before rebuilding: {path}')
    kernel.CloseHandle(handle)


def imports(pe):
    return [(d.dll.decode(), i.name.decode() if i.name else i.ordinal,
             i.address - pe.OPTIONAL_HEADER.ImageBase)
            for d in pe.DIRECTORY_ENTRY_IMPORT for i in d.imports]


def relocations(pe):
    result = []
    for block in pe.DIRECTORY_ENTRY_BASERELOC:
        for entry in block.entries:
            if entry.type == 0:
                continue
            if entry.type != 3:
                raise ValueError(f'Unsupported x86 relocation {entry.type}')
            result.append(entry.rva)
    if len(set(result)) != len(result):
        raise ValueError('Duplicate relocations in source')
    return result


def link(source: Path, dll: Path, output: Path) -> dict:
    if sha(source) != EXPECTED_EXE_SHA:
        raise ValueError('Source EXE does not match the analyzed TH20 1.00c binary')
    original = source.read_bytes()
    pe = pefile.PE(data=original)
    dll_pe = pefile.PE(str(dll))
    if pe.FILE_HEADER.Machine != 0x14C or dll_pe.FILE_HEADER.Machine != 0x14C:
        raise ValueError('Both game and bridge must be x86')
    exports = {e.name.decode(): e for e in dll_pe.DIRECTORY_ENTRY_EXPORT.symbols if e.name}
    for _, name in PATCHES:
        if name not in exports or exports[name].forwarder:
            raise ValueError(f'Missing compiled bridge export {name}')
    if 'g_th20_bridge_stats' not in exports:
        raise ValueError('Bridge instrumentation export missing')
    if pe.OPTIONAL_HEADER.DATA_DIRECTORY[4].Size:
        raise ValueError('Signed input requires a different packaging workflow')
    base = pe.OPTIONAL_HEADER.ImageBase
    section_table = pe.sections[0].get_file_offset()
    section_header = section_table + len(pe.sections) * 40
    if section_header + 40 > pe.OPTIONAL_HEADER.SizeOfHeaders:
        raise ValueError('No room for an additional section header')
    if any(original[section_header:section_header + 40]):
        raise ValueError('New section would overwrite nonzero header data')
    section_rva = align(max(s.VirtualAddress + max(s.Misc_VirtualSize, s.SizeOfRawData)
                            for s in pe.sections), pe.OPTIONAL_HEADER.SectionAlignment)
    raw_offset = align(len(original), pe.OPTIONAL_HEADER.FileAlignment)
    blob = bytearray()

    def reserve(size, boundary=4):
        blob.extend(b'\0' * (align(len(blob), boundary) - len(blob)))
        offset = len(blob)
        blob.extend(b'\0' * size)
        return offset

    descriptors = reserve((len(pe.DIRECTORY_ENTRY_IMPORT) + 2) * 20)
    for index, descriptor in enumerate(pe.DIRECTORY_ENTRY_IMPORT):
        blob[index * 20:index * 20 + 20] = descriptor.struct.__pack__()
    name_off = reserve(len(DLL_NAME) + 1, 1)
    blob[name_off:name_off + len(DLL_NAME)] = DLL_NAME.encode()
    lookup_off = reserve((len(PATCHES) + 1) * 4)
    iat_off = reserve((len(PATCHES) + 1) * 4)
    for index, (_, name) in enumerate(PATCHES):
        hint_off = reserve(2 + len(name) + 1, 2)
        blob[hint_off + 2:hint_off + 2 + len(name)] = name.encode()
        for table in (lookup_off, iat_off):
            struct.pack_into('<I', blob, table + index * 4, section_rva + hint_off)
    struct.pack_into('<5I', blob, len(pe.DIRECTORY_ENTRY_IMPORT) * 20,
                     section_rva + lookup_off, 0, 0, section_rva + name_off, section_rva + iat_off)

    old_relocs = relocations(pe)
    for address in old_relocs:
        if any(address < rva + 6 and address + 4 > rva for rva, _ in PATCHES):
            raise ValueError('Original relocation overlaps a function entry patch')
    added_relocs = [rva + 2 for rva, _ in PATCHES]
    all_relocs = sorted(old_relocs + added_relocs)
    relocation_off = reserve(0)
    pages = {}
    for rva in all_relocs:
        pages.setdefault(rva & ~0xFFF, []).append(0x3000 | (rva & 0xFFF))
    for page, entries in sorted(pages.items()):
        if len(entries) % 2:
            entries.append(0)
        blob.extend(struct.pack('<II', page, 8 + 2 * len(entries)))
        blob.extend(struct.pack('<' + 'H' * len(entries), *entries))
    relocation_size = len(blob) - relocation_off
    virtual_size = len(blob)
    raw_size = align(virtual_size, pe.OPTIONAL_HEADER.FileAlignment)
    blob.extend(b'\0' * (raw_size - len(blob)))
    result = bytearray(original)
    result.extend(b'\0' * (raw_offset - len(result)))
    result.extend(blob)
    allowed = set()

    def write(offset, content):
        result[offset:offset + len(content)] = content
        allowed.update(range(offset, offset + len(content)))

    def field(structure, name, fmt, value):
        write(structure.get_field_absolute_offset(name), struct.pack(fmt, value))

    write(section_header, struct.pack('<8sIIIIIIHHI', b'.cpp\0\0\0\0', virtual_size,
          section_rva, raw_size, raw_offset, 0, 0, 0, 0, 0xC0000040))
    field(pe.FILE_HEADER, 'NumberOfSections', '<H', len(pe.sections) + 1)
    field(pe.OPTIONAL_HEADER, 'SizeOfInitializedData', '<I',
          pe.OPTIONAL_HEADER.SizeOfInitializedData + raw_size)
    field(pe.OPTIONAL_HEADER, 'SizeOfImage', '<I',
          align(section_rva + virtual_size, pe.OPTIONAL_HEADER.SectionAlignment))
    for index, rva, size in [(1, section_rva + descriptors, (len(pe.DIRECTORY_ENTRY_IMPORT) + 2) * 20),
                             (5, section_rva + relocation_off, relocation_size)]:
        field(pe.OPTIONAL_HEADER.DATA_DIRECTORY[index], 'VirtualAddress', '<I', rva)
        field(pe.OPTIONAL_HEADER.DATA_DIRECTORY[index], 'Size', '<I', size)
    patch_report = []
    for index, (rva, name) in enumerate(PATCHES):
        offset = pe.get_offset_from_rva(rva)
        iat_rva = section_rva + iat_off + index * 4
        replacement = b'\xFF\x25' + struct.pack('<I', base + iat_rva)
        write(offset, replacement)
        patch_report.append({'name': name, 'rva': hex(rva), 'iat_rva': hex(iat_rva),
                             'original': original[offset:offset + 6].hex(), 'replacement': replacement.hex()})
    checksum_offset = pe.OPTIONAL_HEADER.get_field_absolute_offset('CheckSum')
    write(checksum_offset, b'\0' * 4)
    checksum = pefile.PE(data=result).generate_checksum()
    write(checksum_offset, struct.pack('<I', checksum))
    changed = [i for i, (a, b) in enumerate(zip(original, result)) if a != b]
    if any(i not in allowed for i in changed):
        raise AssertionError('Unexpected changes outside approved headers and function entries')
    final = pefile.PE(data=result)
    expected_imports = imports(pe) + [(DLL_NAME, name, section_rva + iat_off + i * 4)
                                     for i, (_, name) in enumerate(PATCHES)]
    require(imports(final) == expected_imports, 'Imports or original IAT locations changed')
    require(sorted(relocations(final)) == all_relocs, 'Relocation set mismatch')
    require(final.OPTIONAL_HEADER.AddressOfEntryPoint == pe.OPTIONAL_HEADER.AddressOfEntryPoint,
            'Original entry point changed')
    for old_section, new_section in zip(pe.sections, final.sections):
        require(old_section.__pack__() == new_section.__pack__(), 'Original section header changed')
    for index in range(len(pe.OPTIONAL_HEADER.DATA_DIRECTORY)):
        if index not in (1, 5):
            require(pe.OPTIONAL_HEADER.DATA_DIRECTORY[index].__pack__() == final.OPTIONAL_HEADER.DATA_DIRECTORY[index].__pack__(),
                    f'Unexpected data directory change: {index}')
    require(final.verify_checksum(), 'PE checksum mismatch')
    output.write_bytes(result)
    return {'format': 'incremental-native-reconstruction-v1', 'full_cpp_port': False,
            'source_sha256': EXPECTED_EXE_SHA, 'output_sha256': sha(output), 'bridge_sha256': sha(dll),
            'original_entry_rva': hex(pe.OPTIONAL_HEADER.AddressOfEntryPoint),
            'original_imports_preserved': len(imports(pe)), 'original_relocations_preserved': len(old_relocs),
            'added_relocations': len(added_relocs), 'new_section_rva': hex(section_rva),
            'changed_bytes_within_original_file': len(changed),
            'unchanged_original_bytes': len(original) - len(changed),
            'only_expected_headers_and_function_entries_changed': True, 'patches': patch_report,
            'preserved_engine_machine_code': True, 'starts_original_executable': False,
            'structural_verification': 'passed'}


def main():
    manifest = json.loads((ROOT / 'reports/source_manifest.json').read_text(encoding='utf-8'))
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--game-dir', type=Path, default=Path(manifest['source_directory']))
    args = parser.parse_args()
    bridge = ROOT / 'incremental/native_bridge/build/Release' / DLL_NAME
    abi_report_path = ROOT / 'incremental/native_bridge/bridge_abi_validation.json'
    abi_report = json.loads(abi_report_path.read_text(encoding='utf-8'))
    require(abi_report['status'] == 'passed' and abi_report['failures'] == 0,
            'Actual DLL ABI validation has not passed; run build_playable.ps1')
    require(abi_report['source_sha256'] == EXPECTED_EXE_SHA and abi_report['bridge_sha256'] == sha(bridge),
            'ABI report does not match this source and DLL; run build_playable.ps1')
    require(abi_report['native_core_sha256'] == sha(ROOT / 'native_recovered/native_core.hpp'),
            'ABI report does not match the recovered core source; run build_playable.ps1')
    output_dir = ROOT / 'dist/playable'
    output_dir.mkdir(parents=True, exist_ok=True)
    for name in ['th20_rebuilt.exe', DLL_NAME, 'custom.exe']:
        check_not_running(output_dir / name)
    for name in ['th20.exe', 'th20.dat', 'thbgm.dat', 'custom.exe']:
        expected = next(f for f in manifest['files'] if f['name'] == name)
        if sha(args.game_dir / name) != expected['sha256']:
            raise ValueError(f'Source hash mismatch: {name}')
    report = link(args.game_dir / 'th20.exe', bridge, output_dir / 'th20_rebuilt.exe')
    shutil.copy2(bridge, output_dir / DLL_NAME)
    for name in ['th20.dat', 'thbgm.dat', 'custom.exe']:
        target = output_dir / name
        expected = next(f for f in manifest['files'] if f['name'] == name)
        if not target.exists() or sha(target) != expected['sha256']:
            shutil.copy2(args.game_dir / name, target)
        require(sha(target) == expected['sha256'], f'Packaged data hash mismatch: {name}')
    for script, executable in [('Play.cmd', 'th20_rebuilt.exe'), ('Configure.cmd', 'custom.exe')]:
        lines = ['@echo off', 'setlocal', 'cd /d "%~dp0"', 'set "APPDATA=%~dp0userdata"',
                 'if not exist "%APPDATA%" mkdir "%APPDATA%"', f'start "" /D "%~dp0" "%~dp0{executable}"', 'endlocal', '']
        (output_dir / script).write_bytes('\r\n'.join(lines).encode('ascii'))
    (output_dir / 'README.txt').write_text(
        'TH20 可玩增量重建版\n\n双击 Play.cmd 开始游戏；Configure.cmd 调整画面与手柄。\n'
        '方向键移动，Z 确认/射击，X 炸弹，Shift 低速，Esc 暂停。\n'
        'Play.cmd 将存档保存在本目录 userdata，不写入原版存档。\n'
        '请保留整个目录：EXE 需要 th20_native_bridge.dll、th20.dat 与 thbgm.dat。\n\n'
        '本版本保留尚未恢复的原版引擎机器码，并实际使用重新编译的 7 个 C++ 随机数/计时器函数。\n'
        'th20_rebuilt.exe 直接运行游戏引擎，不会启动原 th20.exe。\n'
        '它不是完整纯 C++ 重写；完整游戏的逐帧 1:1 等价性尚未全部证明。\n'
        '源码及构建入口见工程 build_playable.ps1、incremental/native_bridge、native_recovered。\n', encoding='utf-8-sig')
    report['package'] = [{'name': name, 'size': (output_dir / name).stat().st_size, 'sha256': sha(output_dir / name)}
                         for name in PACKAGE_FILES]
    report['abi_validation'] = {'report': str(abi_report_path.relative_to(ROOT)), 'sha256': sha(abi_report_path),
                                'comparisons': abi_report['total'], 'matched_shipped_dll': True}
    report['cpp_source_snapshot'] = [{'path': str(p.relative_to(ROOT)), 'sha256': sha(p)}
                             for p in [ROOT / 'native_recovered/native_core.hpp',
                                       ROOT / 'incremental/native_bridge/bridge.cpp',
                                       ROOT / 'incremental/native_bridge/bridge.hpp']]
    report['source_snapshot_note'] = 'Current source snapshot; build_playable.ps1 compiles this project before ABI validation and linking.'
    content = json.dumps(report, ensure_ascii=False, indent=2) + '\n'
    (output_dir / 'build_manifest.json').write_text(content, encoding='utf-8')
    (ROOT / 'reports/playable_build.json').write_text(content, encoding='utf-8')
    print(json.dumps({'status': 'built', 'exe': str(output_dir / 'th20_rebuilt.exe'),
                      'cpp_functions': len(PATCHES), 'sha256': report['output_sha256']}, ensure_ascii=False))


if __name__ == '__main__':
    main()
