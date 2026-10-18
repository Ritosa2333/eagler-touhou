"""Read-only inspection of a running 32-bit TH20 verification process.

No code injection, remote execution, writes, input, focus, screenshots, or process
suspension. Samples are asynchronous observations, not a deterministic replay test.
"""
from __future__ import annotations
import argparse
import ctypes as C
from ctypes import wintypes as W
import datetime
import json
from pathlib import Path
import struct
import time

K32 = C.WinDLL('kernel32', use_last_error=True)
K32.OpenProcess.argtypes = [W.DWORD, W.BOOL, W.DWORD]
K32.OpenProcess.restype = W.HANDLE
K32.ReadProcessMemory.argtypes = [W.HANDLE, C.c_void_p, C.c_void_p, C.c_size_t, C.POINTER(C.c_size_t)]
K32.ReadProcessMemory.restype = W.BOOL
K32.CloseHandle.argtypes = [W.HANDLE]
K32.CreateToolhelp32Snapshot.argtypes = [W.DWORD, W.DWORD]
K32.CreateToolhelp32Snapshot.restype = W.HANDLE


class MODULEENTRY32W(C.Structure):
    _fields_ = [('dwSize', W.DWORD), ('th32ModuleID', W.DWORD), ('th32ProcessID', W.DWORD),
                ('GlblcntUsage', W.DWORD), ('ProccntUsage', W.DWORD), ('modBaseAddr', C.c_void_p),
                ('modBaseSize', W.DWORD), ('hModule', W.HMODULE),
                ('szModule', W.WCHAR * 256), ('szExePath', W.WCHAR * 260)]


K32.Module32FirstW.argtypes = [W.HANDLE, C.POINTER(MODULEENTRY32W)]
K32.Module32NextW.argtypes = [W.HANDLE, C.POINTER(MODULEENTRY32W)]


class Remote:
    def __init__(self, pid):
        self.handle = K32.OpenProcess(0x0010 | 0x1000, False, pid)
        if not self.handle:
            raise C.WinError(C.get_last_error())
        snap = K32.CreateToolhelp32Snapshot(0x08 | 0x10, pid)
        if snap == C.c_void_p(-1).value:
            raise C.WinError(C.get_last_error())
        entry = MODULEENTRY32W(dwSize=C.sizeof(MODULEENTRY32W))
        self.modules = []
        try:
            ok = K32.Module32FirstW(snap, C.byref(entry))
            while ok:
                self.modules.append(dict(name=entry.szModule, path=entry.szExePath,
                                         base=entry.modBaseAddr, size=entry.modBaseSize))
                ok = K32.Module32NextW(snap, C.byref(entry))
        finally:
            K32.CloseHandle(snap)
        if not self.modules:
            raise RuntimeError('No process modules found')
        self.base = self.modules[0]['base']

    def read(self, address, size):
        if address < 0x10000 or address + size > 0x100000000:
            raise ValueError(f'Invalid x86 address span: {address:#x}+{size:#x}')
        result = C.create_string_buffer(size)
        read = C.c_size_t()
        if not K32.ReadProcessMemory(self.handle, address, result, size, C.byref(read)):
            raise C.WinError(C.get_last_error())
        if read.value != size:
            raise RuntimeError('Short process memory read')
        return result.raw

    def value(self, rva, fmt='I'):
        return struct.unpack('<' + fmt, self.read(self.base + rva, struct.calcsize('<' + fmt)))[0]

    def export(self, module, name):
        """Resolve an export from mapped PE metadata, without loading the DLL locally."""
        base = module['base']
        dos = self.read(base, 0x40)
        peoff = struct.unpack_from('<I', dos, 0x3c)[0]
        head = self.read(base + peoff, 0x100)
        if head[:4] != b'PE\0\0' or struct.unpack_from('<H', head, 24)[0] != 0x10b:
            raise ValueError('Expected a mapped PE32 export image')
        export_rva, export_size = struct.unpack_from('<II', head, 24 + 96)
        if not export_rva:
            raise KeyError('Module has no exports')
        fields = struct.unpack('<IIHHIIIIIII', self.read(base + export_rva, 40))
        _, _, _, _, _, ordinal_base, nfunc, nnames, funcs, names, ordinals = fields
        for i in range(nnames):
            name_rva = struct.unpack('<I', self.read(base + names + 4 * i, 4))[0]
            observed = self.read(base + name_rva, 128).split(b'\0', 1)[0].decode('ascii')
            if observed == name:
                index = struct.unpack('<H', self.read(base + ordinals + 2 * i, 2))[0]
                rva = struct.unpack('<I', self.read(base + funcs + 4 * index, 4))[0]
                if export_rva <= rva < export_rva + export_size:
                    raise ValueError('Forwarded export cannot be inspected as data')
                return base + rva
        raise KeyError(f'Export {name} not found in {module["name"]}')

    def close(self):
        K32.CloseHandle(self.handle)


def snapshot(remote):
    """Addresses use analyzed 1.00c image / committed thprac reference as evidence."""
    scalar = dict(difficulty=(0x1b0a60, 'I'), player_pointer=(0x1ba56c, 'I'),
                  enemy_manager_pointer=(0x1ba570, 'I'), game_thread_pointer=(0x1ba828, 'I'),
                  replay_manager_pointer=(0x1c60fc, 'I'), main_menu_pointer=(0x1c6124, 'I'),
                  mode_flags=(0x1ba5d4, 'I'), stage=(0x1ba7e4, 'I'),
                  input_bits=(0x1b88b8, 'I'), input_pressed_bits=(0x1b88c0, 'I'))
    state = {name: remote.value(rva, fmt) for name, (rva, fmt) in scalar.items()}
    globals_base = 0x1ba568 + 0x88
    fields = dict(score=(0x00, 'Q'), character=(0x08, 'i'), story_stone=(0x0c, 'i'),
                  narrow_shot_stone=(0x10, 'i'), wide_shot_stone=(0x14, 'i'), assist_stone=(0x18, 'i'),
                  power=(0x30, 'i'), point_value=(0x44, 'i'), hyper_meter=(0x4c, 'i'),
                  summon_meter=(0x5c, 'i'), life_stocks=(0xb8, 'i'), bomb_stocks=(0xcc, 'i'),
                  graze=(0xe4, 'i'))
    state['side0'] = {name: remote.value(globals_base + off, fmt) for name, (off, fmt) in fields.items()}
    replay = state['replay_manager_pointer']
    if replay:
        state['replay'] = dict(frame=struct.unpack('<i', remote.read(replay + 0x250, 4))[0],
                              stage=struct.unpack('<i', remote.read(replay + 0x258, 4))[0],
                              filename=remote.read(replay + 0x260, 256).split(b'\0', 1)[0].decode('cp932', errors='replace'))
    state['save_directory'] = remote.read(remote.base + 0x1b67e1, 260).split(b'\0', 1)[0].decode('cp932', errors='replace')
    state['utc'] = datetime.datetime.now(datetime.timezone.utc).isoformat()
    return state


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('pid', type=int)
    parser.add_argument('--output', type=Path, required=True)
    parser.add_argument('--samples', type=int, default=1)
    parser.add_argument('--interval', type=float, default=1.0)
    parser.add_argument('--dll')
    parser.add_argument('--export', action='append', default=[], help='NAME:STRUCT_FORMAT (e.g. counters:7Q)')
    args = parser.parse_args()
    remote = Remote(args.pid)
    try:
        exports = {}
        if args.dll:
            matching = [m for m in remote.modules if m['name'].casefold() == args.dll.casefold()]
            if len(matching) != 1:
                raise RuntimeError(f'Expected one module {args.dll}, got {len(matching)}')
            for spec in args.export:
                name, fmt = spec.split(':', 1)
                exports[name] = (remote.export(matching[0], name), fmt)
        report = dict(pid=args.pid, executable=remote.modules[0], modules=remote.modules,
                      limitations=['Asynchronous read-only samples are not frame-locked.',
                                   'No deterministic replay equivalence claim.'], samples=[])
        for i in range(args.samples):
            sample = snapshot(remote)
            sample['cpp_exports'] = {name: list(struct.unpack('<' + fmt, remote.read(addr, struct.calcsize('<' + fmt))))
                                     for name, (addr, fmt) in exports.items()}
            report['samples'].append(sample)
            if i + 1 < args.samples:
                time.sleep(args.interval)
        args.output.parent.mkdir(parents=True, exist_ok=True)
        args.output.write_text(json.dumps(report, indent=2), encoding='utf-8')
        print(json.dumps(dict(output=str(args.output), executable=remote.modules[0],
                             samples=report['samples']), indent=2))
    finally:
        remote.close()


if __name__ == '__main__':
    main()
