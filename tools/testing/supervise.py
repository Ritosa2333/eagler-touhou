"""Own test descendants after a driver exits, including detached processes."""
import ctypes
import hashlib
import json
import os
from pathlib import Path
import re
import signal
import subprocess
import sys
import tempfile
import time


def write_cleanup_report(path, report):
    if not path:
        return
    target = Path(path)
    target.parent.mkdir(parents=True, exist_ok=True)
    descriptor, temporary = tempfile.mkstemp(prefix=f"{target.name}.", suffix=".pending", dir=target.parent)
    try:
        with os.fdopen(descriptor, "w", encoding="utf-8", newline="\n") as output:
            json.dump(report, output, indent=2)
            output.write("\n")
        os.replace(temporary, target)
    except BaseException:
        try:
            os.unlink(temporary)
        except FileNotFoundError:
            pass
        raise


def sha256_file(path):
    digest = hashlib.sha256()
    with open(path, "rb") as source:
        for chunk in iter(lambda: source.read(1024 * 1024), b""):
            digest.update(chunk)
    return digest.hexdigest()


def windows_path_key(path):
    return os.path.normcase(os.path.abspath(os.path.normpath(path)))


def is_full_windows_image_path(path):
    if not isinstance(path, str) or not path or path.strip() != path:
        return False
    if re.match(r"^[A-Za-z]:[\\/]", path):
        return True
    return re.match(r"^(?:\\\\|//)(?![?.][\\/])[^\\/]+[\\/][^\\/]+(?:[\\/]|$)", path) is not None


def job_process_ids(kernel, job):
    # JOBOBJECT_BASIC_PROCESS_ID_LIST has two DWORDs followed by ULONG_PTR ids.
    size = 8192
    buffer = ctypes.create_string_buffer(size)
    if not kernel.QueryInformationJobObject(job, 3, buffer, size, None):
        raise ctypes.WinError(ctypes.get_last_error())
    raw = buffer.raw
    assigned = int.from_bytes(raw[0:4], "little")
    returned = int.from_bytes(raw[4:8], "little")
    width = ctypes.sizeof(ctypes.c_void_p)
    count = min(returned, (size - 8) // width)
    return assigned, [int.from_bytes(raw[8 + index * width:8 + (index + 1) * width], "little")
                      for index in range(count)]


def process_image_path(kernel, handle, w):
    size = w.DWORD(32768)
    image = ctypes.create_unicode_buffer(size.value)
    if not kernel.QueryFullProcessImageNameW(handle, 0, image, ctypes.byref(size)):
        raise ctypes.WinError(ctypes.get_last_error())
    return image.value


def windows(command, cleanup_images=(), cleanup_report_path=None):
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
        "OpenProcess": ([w.DWORD, w.BOOL, w.DWORD], w.HANDLE),
        "IsProcessInJob": ([w.HANDLE, w.HANDLE, ctypes.POINTER(w.BOOL)], w.BOOL),
        "QueryFullProcessImageNameW": ([w.HANDLE, w.DWORD, w.LPWSTR, ctypes.POINTER(w.DWORD)], w.BOOL),
        "TerminateProcess": ([w.HANDLE, w.UINT], w.BOOL), "CloseHandle": ([w.HANDLE], w.BOOL),
    }
    for name, (args, result) in signatures.items():
        function = getattr(kernel, name)
        function.argtypes, function.restype = args, result
    job = kernel.CreateJobObjectW(None, None)
    if not job:
        raise ctypes.WinError(ctypes.get_last_error())
    process = Process()
    cleanup_report = {"schema": "eagler-touhou/process-cleanup/1", "platform": "win32",
                      "declared": cleanup_images, "observed": [], "remaining": [],
                      "status": "not-requested" if not cleanup_images else "running"}
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
        cleanup_report["commandExitCode"] = int(code.value)
        if cleanup_images:
            declarations = [{"path": image["path"], "sha256": image["sha256"].lower()}
                            for image in cleanup_images]
            try:
                _, pids = job_process_ids(kernel, job)
                for pid in pids:
                    item = {"pid": pid, "path": None, "sha256": None, "action": "unidentified"}
                    access = 0x0001 | 0x1000 | 0x00100000  # terminate, query and wait
                    handle = kernel.OpenProcess(access, False, pid)
                    if not handle:
                        item["error"] = ctypes.WinError(ctypes.get_last_error()).strerror
                        cleanup_report["observed"].append(item)
                        continue
                    try:
                        in_job = w.BOOL()
                        if not kernel.IsProcessInJob(handle, job, ctypes.byref(in_job)):
                            item["error"] = ctypes.WinError(ctypes.get_last_error()).strerror
                            cleanup_report["observed"].append(item)
                            continue
                        if not in_job.value:
                            item["action"] = "no-longer-in-job"
                            cleanup_report["observed"].append(item)
                            continue
                        item["path"] = process_image_path(kernel, handle, w)
                        try:
                            item["sha256"] = sha256_file(item["path"])
                        except OSError as error:
                            item["error"] = f"Cannot hash process image: {error}"
                        path_matches = [index for index, declared in enumerate(declarations)
                                        if windows_path_key(item["path"]) == windows_path_key(declared["path"])]
                        declaration_index = next((index for index in path_matches
                            if item["sha256"] == declarations[index]["sha256"]), None)
                        if declaration_index is None:
                            item["action"] = "not-declared" if not path_matches else "sha256-mismatch"
                        else:
                            item["declarationIndex"] = declaration_index
                            state = kernel.WaitForSingleObject(handle, 0)
                            if state == 0:
                                exit_code = w.DWORD()
                                if kernel.GetExitCodeProcess(handle, ctypes.byref(exit_code)):
                                    item["action"], item["exitCode"] = "already-exited", int(exit_code.value)
                                else:
                                    item["action"] = "unmatched"
                                    item["error"] = ctypes.WinError(ctypes.get_last_error()).strerror
                            elif state == 0x102:  # WAIT_TIMEOUT
                                cleanup_exit_code = 0xE0000001
                                if not kernel.TerminateProcess(handle, cleanup_exit_code):
                                    item["action"] = "unmatched"
                                    item["error"] = ctypes.WinError(ctypes.get_last_error()).strerror
                                else:
                                    waited = kernel.WaitForSingleObject(handle, 5000)
                                    exit_code = w.DWORD()
                                    if waited == 0 and kernel.GetExitCodeProcess(handle, ctypes.byref(exit_code)):
                                        item["action"], item["exitCode"] = "terminated", int(exit_code.value)
                                    else:
                                        item["action"] = "termination-unconfirmed"
                            else:
                                item["action"] = "unmatched"
                                item["error"] = f"Unexpected process wait result: {state}"
                        cleanup_report["observed"].append(item)
                    except OSError as error:
                        item["error"] = str(error)
                        cleanup_report["observed"].append(item)
                    finally:
                        kernel.CloseHandle(handle)
            except OSError as error:
                cleanup_report["error"] = f"Cannot inspect Job Object descendants: {error}"
        accounting = Accounting()
        # Console hosts can outlive their exiting compiler briefly. Wait for
        # natural shutdown within a fixed budget; surviving children still fail.
        natural_exit_deadline = time.monotonic() + 1.0
        while True:
            if not kernel.QueryInformationJobObject(job, 1, ctypes.byref(accounting), ctypes.sizeof(accounting), None):
                raise ctypes.WinError(ctypes.get_last_error())
            if not accounting.active:
                if code.value != 0:
                    cleanup_report["status"] = "command-failed"
                elif cleanup_report.get("error"):
                    cleanup_report["status"] = "failed-inspection"
                else:
                    cleanup_report["status"] = "passed"
                write_cleanup_report(cleanup_report_path, cleanup_report)
                return code.value if code.value else (1 if cleanup_report.get("error") else 0)
            if time.monotonic() >= natural_exit_deadline:
                break
            time.sleep(0.01)
        try:
            _, remaining_pids = job_process_ids(kernel, job)
            for pid in remaining_pids:
                item = {"pid": pid, "path": None, "sha256": None}
                handle = kernel.OpenProcess(0x1000 | 0x00100000, False, pid)
                if handle:
                    try:
                        item["path"] = process_image_path(kernel, handle, w)
                        try:
                            item["sha256"] = sha256_file(item["path"])
                        except OSError as error:
                            item["error"] = f"Cannot hash process image: {error}"
                    except OSError as error:
                        item["error"] = str(error)
                    finally:
                        kernel.CloseHandle(handle)
                cleanup_report["remaining"].append(item)
        except OSError as error:
            cleanup_report["error"] = f"Cannot inspect remaining Job Object descendants: {error}"
        cleanup_report["status"] = "failed-descendants"
        write_cleanup_report(cleanup_report_path, cleanup_report)
        print("Test driver left running descendants", file=sys.stderr)
        return code.value if code.value else 1
    finally:
        kernel.CloseHandle(job)
        if process.thread:
            kernel.CloseHandle(process.thread)
        if process.process:
            kernel.CloseHandle(process.process)


def posix(command, cleanup_images=(), cleanup_report_path=None):
    if not sys.platform.startswith("linux"):
        raise RuntimeError("Process supervision requires Windows Job Objects or Linux child subreapers")
    if cleanup_images:
        write_cleanup_report(cleanup_report_path, {
            "schema": "eagler-touhou/process-cleanup/1", "platform": sys.platform,
            "declared": cleanup_images, "observed": [], "remaining": [],
            "status": "unsupported-platform",
            "error": "cleanupImages declarations are supported only by the Windows Job Object supervisor",
        })
        print("cleanupImages declarations are supported only on Windows", file=sys.stderr)
        return 1
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


def main(arguments):
    cleanup_images = []
    cleanup_report_path = None
    index = 0
    try:
        while index < len(arguments) and arguments[index] != "--":
            option = arguments[index]
            if option == "--cleanup-report" and index + 1 < len(arguments):
                cleanup_report_path = arguments[index + 1]
                index += 2
            elif option == "--cleanup-image" and index + 2 < len(arguments):
                cleanup_images.append({"path": arguments[index + 1], "sha256": arguments[index + 2]})
                index += 3
            else:
                raise ValueError(f"invalid supervisor option: {option}")
        if index >= len(arguments) or arguments[index] != "--" or index + 1 >= len(arguments):
            raise ValueError("usage: supervise.py [--cleanup-report path] [--cleanup-image path sha256] -- command [arguments...]")
        if cleanup_images and not cleanup_report_path:
            raise ValueError("cleanupImages requires --cleanup-report")
        for image in cleanup_images:
            if not is_full_windows_image_path(image["path"]):
                raise ValueError("cleanup image path must be drive-qualified or a complete UNC-share path")
            if not re.fullmatch(r"[0-9a-fA-F]{64}", image["sha256"]):
                raise ValueError("cleanup image SHA-256 must contain 64 hexadecimal digits")
        command = arguments[index + 1:]
    except ValueError as error:
        print(str(error), file=sys.stderr)
        return 2
    return windows(command, cleanup_images, cleanup_report_path) if os.name == "nt" else posix(command, cleanup_images, cleanup_report_path)


if __name__ == "__main__":
    raise SystemExit(main(sys.argv[1:]))
