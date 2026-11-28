from pathlib import Path
import ctypes, json, os, subprocess, time, hashlib

p = Path(__file__).resolve().parent
kernel = ctypes.WinDLL('kernel32', use_last_error=True)
kernel.WaitForSingleObject.argtypes = [ctypes.c_void_p, ctypes.c_ulong]
kernel.WaitForSingleObject.restype = ctypes.c_ulong
kernel.GetExitCodeProcess.argtypes = [ctypes.c_void_p, ctypes.POINTER(ctypes.c_ulong)]
proof = {'scope': 'Same diagnostic host and canonical main.dart AOT as stopped GPU matrix. Default preference retry retains a nonterminated process for bounded native stack collection. Native host self-closes, no activation or app input. Parent owns the kernel handle and terminates only its own retained child after explicit done marker or 90-second safety deadline.', 'aotSha256': hashlib.sha256((p / 'preview/data/app.so').read_bytes()).hexdigest(), 'hostSha256': hashlib.sha256((p / 'preview/gpu-audit.exe').read_bytes()).hexdigest()}
(p / 'retain3-source.json').write_text(json.dumps(proof, indent=2) + '\n')
for attempt in range(1, 4):
    events = p / f'retain3-events-{attempt}.jsonl'
    assert not events.exists()
    env = dict(os.environ, TOUHOU_PROBE_LOG=str(events), TOUHOU_GPU_PREFERENCE='default')
    with (p / f'retain3-engine-{attempt}.log').open('wb') as log:
        process = subprocess.Popen([str(p / 'preview/gpu-audit.exe')], cwd=p / 'preview', env=env, stdout=log, stderr=subprocess.STDOUT, creationflags=subprocess.CREATE_NO_WINDOW)
        handle = int(process._handle)
        state = kernel.WaitForSingleObject(handle, 20000)
        code = ctypes.c_ulong(); kernel.GetExitCodeProcess(handle, ctypes.byref(code))
        result = {'attempt': attempt, 'pid': process.pid, 'initialWait': state, 'initialExitCode': code.value, 'forcedTermination': False}
        if state == 258:
            (p / 'retained3.json').write_text(json.dumps(result, indent=2) + '\n')
            print(json.dumps({'awaitingNativeStack': True, **result}), flush=True)
            end = time.perf_counter() + 90
            while time.perf_counter() < end:
                state = kernel.WaitForSingleObject(handle, 1000)
                if state == 0 or (p / 'capture-done3.json').exists():
                    break
            if state != 0:
                process.terminate(); result['forcedTermination'] = True
                result['finalWait'] = kernel.WaitForSingleObject(handle, 10000)
            else:
                result['finalWait'] = 0
            (p / f'retain3-result-{attempt}.json').write_text(json.dumps(result, indent=2) + '\n')
            print(json.dumps(result), flush=True)
            break
        (p / f'retain3-result-{attempt}.json').write_text(json.dumps(result, indent=2) + '\n')
        print(json.dumps(result), flush=True)
