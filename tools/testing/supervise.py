"""Own test descendants after a driver exits, including detached processes."""
import ctypes
import os
from pathlib import Path
import signal
import subprocess
import sys
import time


def windows(command):
    from ctypes import wintypes as w

    class Limits(ctypes.Structure):
        _fields_ = [("process_time", ctypes.c_int64), ("job_time", ctypes.c_int64),
                    ("flags", w.DWORD), ("min_working_set", ctypes.c_size_t),
                    ("max_working_set", ctypes.c_size_t), ("active_limit", w.DWORD),
                    ("affinity", ctypes.c_size_t), ("priority", w.DWORD), ("scheduling", w.DWORD)]

    class Io(ctypes.Structure):
        _fields_ = [(name, ctypes.c_uint64) for name in
                    ("read_ops", "write_ops", "other_ops", "read_bytes", "write_bytes", "other_bytes")]

    class Extended(ctypes.Structure):
        _fields_ = [("basic", Limits), ("io", Io), ("process_memory", ctypes.c_size_t),
                    ("job_memory", ctypes.c_size_t), ("peak_process", ctypes.c_size_t), ("peak_job", ctypes.c_size_t)]

    class Accounting(ctypes.Structure):
        _fields_ = [(name, ctypes.c_int64) for name in ("user", "kernel", "period_user", "period_kernel")] + [
            (name, w.DWORD) for name in ("faults", "total", "active", "terminated")]

    class Startup(ctypes.Structure):
        _fields_ = [("cb", w.DWORD), ("reserved", w.LPWSTR), ("desktop", w.LPWSTR), ("title", w.LPWSTR)] + [
            (name, w.DWORD) for name in ("x", "y", "x_size", "y_size", "x_chars", "y_chars", "fill", "flags")] + [
            ("show", w.WORD), ("reserved_size", w.WORD), ("reserved_bytes", ctypes.POINTER(w.BYTE)),
            ("stdin", w.HANDLE), ("stdout", w.HANDLE), ("stderr", w.HANDLE)]

    class Process(ctypes.Structure):
        _fields_ = [("process", w.HANDLE), ("thread", w.HANDLE), ("pid", w.DWORD), ("tid", w.DWORD)]

    kernel = ctypes.WinDLL("kernel32", use_last_error=True)
    signatures = {
        "CreateJobObjectW": ([ctypes.c_void_p, w.LPCWSTR], w.HANDLE),
        "SetInformationJobObject": ([w.HANDLE, ctypes.c_int, ctypes.c_void_p, w.DWORD], w.BOOL),
        "QueryInformationJobObject": ([w.HANDLE, ctypes.c_int, ctypes.c_void_p, w.DWORD, ctypes.c_void_p], w.BOOL),
        "AssignProcessToJobObject": ([w.HANDLE, w.HANDLE], w.BOOL),
        "CreateProcessW": ([w.LPCWSTR, w.LPWSTR, ctypes.c_void_p, ctypes.c_void_p, w.BOOL, w.DWORD,
                            ctypes.c_void_p, w.LPCWSTR, ctypes.POINTER(Startup), ctypes.POINTER(Process)], w.BOOL),
        "GetStdHandle": ([w.DWORD], w.HANDLE), "ResumeThread": ([w.HANDLE], w.DWORD),
        "WaitForSingleObject": ([w.HANDLE, w.DWORD], w.DWORD),
        "GetExitCodeProcess": ([w.HANDLE, ctypes.POINTER(w.DWORD)], w.BOOL),
        "TerminateProcess": ([w.HANDLE, w.UINT], w.BOOL), "CloseHandle": ([w.HANDLE], w.BOOL),
    }
    for name, (args, result) in signatures.items():
        function = getattr(kernel, name)
        function.argtypes, function.restype = args, result
    job = kernel.CreateJobObjectW(None, None)
    if not job:
        raise ctypes.WinError(ctypes.get_last_error())
    process = Process()
    try:
        limits = Extended()
        limits.basic.flags = 0x2000  # JOB_OBJECT_LIMIT_KILL_ON_JOB_CLOSE
        if not kernel.SetInformationJobObject(job, 9, ctypes.byref(limits), ctypes.sizeof(limits)):
            raise ctypes.WinError(ctypes.get_last_error())
        startup = Startup()
        startup.cb, startup.flags = ctypes.sizeof(startup), 0x100
        startup.stdin, startup.stdout, startup.stderr = [kernel.GetStdHandle(value & 0xffffffff) for value in (-10, -11, -12)]
        arguments = ctypes.create_unicode_buffer(subprocess.list2cmdline(command))
        if not kernel.CreateProcessW(None, arguments, None, None, True, 0x08000004, None, None,
                                     ctypes.byref(startup), ctypes.byref(process)):
            raise ctypes.WinError(ctypes.get_last_error())
        if not kernel.AssignProcessToJobObject(job, process.process):
            kernel.TerminateProcess(process.process, 1)
            raise ctypes.WinError(ctypes.get_last_error())
        if kernel.ResumeThread(process.thread) == 0xffffffff:
            raise ctypes.WinError(ctypes.get_last_error())
        if kernel.WaitForSingleObject(process.process, 0xffffffff) != 0:
            raise ctypes.WinError(ctypes.get_last_error())
        code = w.DWORD()
        if not kernel.GetExitCodeProcess(process.process, ctypes.byref(code)):
            raise ctypes.WinError(ctypes.get_last_error())
        accounting = Accounting()
        for _ in range(10):
            if not kernel.QueryInformationJobObject(job, 1, ctypes.byref(accounting), ctypes.sizeof(accounting), None):
                raise ctypes.WinError(ctypes.get_last_error())
            if not accounting.active:
                return code.value
            time.sleep(0.01)
        print("Test driver left running descendants", file=sys.stderr)
        return 1
    finally:
        kernel.CloseHandle(job)
        if process.thread:
            kernel.CloseHandle(process.thread)
        if process.process:
            kernel.CloseHandle(process.process)


def posix(command):
    if not sys.platform.startswith("linux"):
        raise RuntimeError("Process supervision requires Windows Job Objects or Linux child subreapers")
    libc = ctypes.CDLL(None, use_errno=True)
    if libc.prctl(36, 1, 0, 0, 0):  # PR_SET_CHILD_SUBREAPER
        raise OSError(ctypes.get_errno(), "Cannot own test descendants")
    child = subprocess.Popen(command, start_new_session=True)
    code = child.wait()
    children_file = Path(f"/proc/self/task/{os.getpid()}/children")
    # Reap completed children before deciding whether a driver leaked work.
    for _ in range(10):
        while True:
            try:
                if os.waitpid(-1, os.WNOHANG)[0] == 0:
                    break
            except ChildProcessError:
                return code
        time.sleep(0.01)
    leaked = children_file.read_text().split()
    while leaked:
        for pid in leaked:
            try:
                os.kill(int(pid), signal.SIGKILL)
            except ProcessLookupError:
                pass
        try:
            os.waitpid(-1, 0)
        except ChildProcessError:
            break
        leaked = children_file.read_text().split()
    print("Test driver left running descendants", file=sys.stderr)
    return 1


if __name__ == "__main__":
    if len(sys.argv) < 3 or sys.argv[1] != "--":
        raise SystemExit("usage: supervise.py -- command [arguments...]")
    raise SystemExit(windows(sys.argv[2:]) if os.name == "nt" else posix(sys.argv[2:]))
