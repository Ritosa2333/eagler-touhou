"""Read selected source-build globals using its PDB; never writes process memory."""
import ctypes as C
from ctypes import wintypes as W
import json
from pathlib import Path
import re
import struct
import subprocess
import sys

pid = int(sys.argv[1])
pdb = Path(sys.argv[2]).resolve()
lookup = Path(__file__).resolve().parents[2] / 'build_sources/diagnostics/RelWithDebInfo/th20_pdb_lookup.exe'
kernel = C.WinDLL('kernel32', use_last_error=True)
psapi = C.WinDLL('psapi', use_last_error=True)
kernel.OpenProcess.argtypes = [W.DWORD, W.BOOL, W.DWORD]
kernel.OpenProcess.restype = W.HANDLE
kernel.ReadProcessMemory.argtypes = [W.HANDLE, C.c_void_p, C.c_void_p, C.c_size_t, C.POINTER(C.c_size_t)]
kernel.CloseHandle.argtypes = [W.HANDLE]
psapi.EnumProcessModulesEx.argtypes = [W.HANDLE, C.c_void_p, W.DWORD, C.POINTER(W.DWORD), W.DWORD]
psapi.GetModuleFileNameExW.argtypes = [W.HANDLE, W.HMODULE, W.LPWSTR, W.DWORD]
process = kernel.OpenProcess(0x410, False, pid)
if not process:
    raise C.WinError(C.get_last_error())

def read(address, size):
    data = C.create_string_buffer(size)
    got = C.c_size_t()
    if not kernel.ReadProcessMemory(process, address, data, size, C.byref(got)) or got.value != size:
        raise C.WinError(C.get_last_error())
    return data.raw

def u32(address):
    return struct.unpack('<I', read(address, 4))[0]

try:
    modules = (W.HMODULE * 1024)()
    needed = W.DWORD()
    if not psapi.EnumProcessModulesEx(process, modules, C.sizeof(modules), C.byref(needed), 3):
        raise C.WinError(C.get_last_error())
    base = None
    for module in modules[:needed.value // C.sizeof(W.HMODULE)]:
        filename = C.create_unicode_buffer(32768)
        psapi.GetModuleFileNameExW(process, module, filename, len(filename))
        if Path(filename.value).name == 'th20_source.exe':
            base = module
            break
    if base is None:
        raise RuntimeError('PID is not the independent th20_source.exe build')
    names = ['th20::source::gameplay::controller', 'th20::source::game_session::session',
             'th20::source::program_entry::graphics_state', 'th20::source::platform_window::input',
             'th20::source::platform_window::unrecovered::menu_scene', 'th20::source::program_entry::window_state']
    output = subprocess.check_output([str(lookup), str(pdb)] + ['@' + name for name in names], text=True)
    addresses = {name: base + int(rva, 16) for rva, name in re.findall(r'DATA 0x([0-9a-f]+) (.+)', output)}
    session = addresses[names[1]]
    game = u32(addresses[names[0]])
    result = {'pid': pid, 'image_base': hex(base), 'game': hex(game),
              'scene': u32(addresses[names[2]] + 0xb08), 'next_scene': u32(addresses[names[2]] + 0xb0c),
              'session_flags': u32(session + 0x6c), 'mode': u32(session + 0x70),
              'stage': u32(session + 0x88 + 0x1f4), 'score': struct.unpack('<Q', read(session + 0x88, 8))[0],
              'menu_idle_frames': u32(session + 0x7c)}
    title = u32(addresses[names[4]])
    if title:
        result.update(title_state=u32(title + 0x18), title_phase=u32(title + 0x20), title_age=u32(title + 0x158))
    if game:
        result.update(game_flags=u32(game + 0xe8), game_frame=u32(game + 0x14), load_stage=u32(game + 0x30))
    player = u32(session + 4)
    if player:
        result.update(player_position=struct.unpack('<3f', read(player + 0x614, 12)), player_state=u32(player + 0x10))
    print(json.dumps(result, indent=2))
finally:
    kernel.CloseHandle(process)
