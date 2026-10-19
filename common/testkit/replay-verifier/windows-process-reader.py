"""Read-only memory snapshots from a normally launched Windows process."""

from __future__ import annotations

import ctypes
from ctypes import wintypes
from pathlib import Path
import struct
import subprocess
import time


PROCESS_VM_READ = 0x0010
PROCESS_TERMINATE = 0x0001
PROCESS_QUERY_LIMITED_INFORMATION = 0x1000
TH32CS_SNAPPROCESS = 0x00000002
INVALID_HANDLE_VALUE = ctypes.c_void_p(-1).value


class PROCESSENTRY32W(ctypes.Structure):
    _fields_ = [
        ("dwSize", wintypes.DWORD), ("cntUsage", wintypes.DWORD),
        ("th32ProcessID", wintypes.DWORD), ("th32DefaultHeapID", ctypes.c_size_t),
        ("th32ModuleID", wintypes.DWORD), ("cntThreads", wintypes.DWORD),
        ("th32ParentProcessID", wintypes.DWORD), ("pcPriClassBase", wintypes.LONG),
        ("dwFlags", wintypes.DWORD), ("szExeFile", wintypes.WCHAR * 260),
    ]


class WindowsProcessReader:
    """Launch a process normally and expose only ReadProcessMemory.

    No debugger is attached and no process memory, input, clock, or game state
    is written. Providers should use ``consistent`` around a monotonic logical
    frame field so a mid-transaction snapshot is rejected.
    """

    def __init__(self, executable: Path, working_directory: Path, *, target_executable_name: str | None = None):
        self.executable = Path(executable).resolve()
        self.working_directory = Path(working_directory).resolve()
        self.target_executable_name = target_executable_name
        self.kernel32 = ctypes.WinDLL("kernel32", use_last_error=True)
        self.kernel32.OpenProcess.argtypes = [wintypes.DWORD, wintypes.BOOL, wintypes.DWORD]
        self.kernel32.OpenProcess.restype = wintypes.HANDLE
        self.kernel32.ReadProcessMemory.argtypes = [wintypes.HANDLE, ctypes.c_void_p,
                                                    ctypes.c_void_p, ctypes.c_size_t,
                                                    ctypes.POINTER(ctypes.c_size_t)]
        self.kernel32.ReadProcessMemory.restype = wintypes.BOOL
        self.kernel32.CreateToolhelp32Snapshot.argtypes = [wintypes.DWORD, wintypes.DWORD]
        self.kernel32.CreateToolhelp32Snapshot.restype = wintypes.HANDLE
        self.kernel32.Process32FirstW.argtypes = [wintypes.HANDLE, ctypes.POINTER(PROCESSENTRY32W)]
        self.kernel32.Process32FirstW.restype = wintypes.BOOL
        self.kernel32.Process32NextW.argtypes = [wintypes.HANDLE, ctypes.POINTER(PROCESSENTRY32W)]
        self.kernel32.Process32NextW.restype = wintypes.BOOL
        self.kernel32.GetExitCodeProcess.argtypes = [wintypes.HANDLE, ctypes.POINTER(wintypes.DWORD)]
        self.kernel32.GetExitCodeProcess.restype = wintypes.BOOL
        self.kernel32.TerminateProcess.argtypes = [wintypes.HANDLE, wintypes.UINT]
        self.kernel32.TerminateProcess.restype = wintypes.BOOL
        self.child = None
        self.handle = None
        self.target_pid = None

    def __enter__(self):
        self.child = subprocess.Popen([str(self.executable)], cwd=self.working_directory)
        self.target_pid = self.child.pid
        try:
            if self.target_executable_name:
                self.target_pid = self._wait_for_descendant(self.child.pid, self.target_executable_name)
        except BaseException:
            self.close()
            raise
        self.handle = self.kernel32.OpenProcess(PROCESS_VM_READ | PROCESS_QUERY_LIMITED_INFORMATION,
                                                False, self.target_pid)
        if not self.handle:
            self.close()
            raise ctypes.WinError(ctypes.get_last_error(), "OpenProcess")
        return self

    def __exit__(self, *_):
        self.close()

    def close(self):
        if self.handle:
            self.kernel32.CloseHandle(self.handle)
            self.handle = None
        if self.child and self.child.poll() is None:
            self.child.terminate()
            try: self.child.wait(timeout=5)
            except subprocess.TimeoutExpired:
                self.child.kill(); self.child.wait(timeout=5)
        if self.target_pid and self.child and self.target_pid != self.child.pid:
            target = self.kernel32.OpenProcess(PROCESS_TERMINATE | PROCESS_QUERY_LIMITED_INFORMATION,
                                               False, self.target_pid)
            if target:
                code = wintypes.DWORD()
                if self.kernel32.GetExitCodeProcess(target, ctypes.byref(code)) and code.value == 259:
                    self.kernel32.TerminateProcess(target, 0)
                self.kernel32.CloseHandle(target)

    def _wait_for_descendant(self, parent_pid: int, executable_name: str, timeout_seconds: float = 10.0) -> int:
        deadline = time.monotonic() + timeout_seconds
        expected = executable_name.casefold()
        while time.monotonic() < deadline:
            snapshot = self.kernel32.CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0)
            if snapshot != INVALID_HANDLE_VALUE:
                try:
                    entry = PROCESSENTRY32W(); entry.dwSize = ctypes.sizeof(entry)
                    ok = self.kernel32.Process32FirstW(snapshot, ctypes.byref(entry))
                    while ok:
                        if entry.th32ParentProcessID == parent_pid and entry.szExeFile.casefold() == expected:
                            return entry.th32ProcessID
                        ok = self.kernel32.Process32NextW(snapshot, ctypes.byref(entry))
                finally:
                    self.kernel32.CloseHandle(snapshot)
            if self.child.poll() is not None:
                break
            time.sleep(0.01)
        raise RuntimeError(f"Timed out waiting for child process {executable_name!r}")

    def read(self, address: int, size: int) -> bytes:
        buffer = (ctypes.c_ubyte * size)()
        done = ctypes.c_size_t()
        if not self.kernel32.ReadProcessMemory(self.handle, address, buffer, size, ctypes.byref(done)) or done.value != size:
            raise ctypes.WinError(ctypes.get_last_error(), f"ReadProcessMemory 0x{address:x}")
        return bytes(buffer)

    def unpack(self, fmt: str, address: int):
        return struct.unpack(fmt, self.read(address, struct.calcsize(fmt)))

    def consistent(self, clock_address: int, capture, *, attempts: int = 8):
        """Return a snapshot only when the u32 clock brackets it unchanged."""
        for _ in range(attempts):
            before, = self.unpack("<I", clock_address)
            value = capture()
            after, = self.unpack("<I", clock_address)
            if before == after:
                return before, value
        return None

    def poll(self, interval_seconds: float = 0.001):
        while self._target_is_running():
            yield
            if interval_seconds > 0:
                time.sleep(interval_seconds)

    def _target_is_running(self) -> bool:
        if not self.handle:
            return False
        code = wintypes.DWORD()
        return bool(self.kernel32.GetExitCodeProcess(self.handle, ctypes.byref(code))) and code.value == 259
