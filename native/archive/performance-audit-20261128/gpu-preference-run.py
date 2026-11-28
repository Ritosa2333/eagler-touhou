from pathlib import Path
import ctypes, json, os, subprocess, time, hashlib

p = Path(__file__).resolve().parent
kernel = ctypes.WinDLL('kernel32', use_last_error=True)
kernel.WaitForSingleObject.argtypes = [ctypes.c_void_p, ctypes.c_ulong]
kernel.WaitForSingleObject.restype = ctypes.c_ulong
kernel.GetExitCodeProcess.argtypes = [ctypes.c_void_p, ctypes.POINTER(ctypes.c_ulong)]
for index, mode in enumerate(['high', 'default', 'default', 'high', 'high', 'default'], 1):
    events = p / f'events-{index}-{mode}.jsonl'
    result_file = p / f'result-{index}-{mode}.json'
    assert not events.exists() and not result_file.exists()
    env = dict(os.environ, TOUHOU_PROBE_LOG=str(events), TOUHOU_GPU_PREFERENCE=mode)
    with (p / f'engine-{index}-{mode}.log').open('wb') as log:
        process = subprocess.Popen([str(p / 'preview/gpu-audit.exe')], cwd=p / 'preview', env=env, stdout=log, stderr=subprocess.STDOUT, creationflags=subprocess.CREATE_NO_WINDOW)
        started = time.perf_counter()
        samples = []
        print(json.dumps({'run': index, 'mode': mode, 'pid': process.pid}), flush=True)
        while time.perf_counter() - started < 35:
            state = kernel.WaitForSingleObject(int(process._handle), 1000)
            code = ctypes.c_ulong()
            ok = kernel.GetExitCodeProcess(int(process._handle), ctypes.byref(code))
            samples.append({'elapsedSeconds': time.perf_counter() - started, 'wait': state, 'codeRead': bool(ok), 'exitCode': code.value})
            if state == 0:
                break
        result = {'run': index, 'mode': mode, 'pid': process.pid, 'observations': samples, 'kernelSignaled': state == 0, 'exitCode': code.value, 'forcedTermination': False}
        if state == 258:
            process.terminate()
            process.wait(timeout=15)
            result['forcedTermination'] = True
        result_file.write_text(json.dumps(result, indent=2) + '\n')
        print(json.dumps({k: v for k, v in result.items() if k != 'observations'}), flush=True)
        if result['forcedTermination'] or result['exitCode'] != 0:
            raise SystemExit('Native close failed; keep evidence before further runs')
proof = {'scope': 'One native diagnostic EXE; only GPU preference changes through environment. Same ordinary production main.dart AOT, engine, plugin DLLs, resources, Skia, guest Windows metadata, window dimensions and timer. Native phase markers and actual adapter identification. All runs self-close without activation. Shared Windows desktop; warm repeated launches, not cold boot, power, foreground FPS or visual acceptance.', 'aotSha256': hashlib.sha256((p / 'preview/data/app.so').read_bytes()).hexdigest(), 'hostSha256': hashlib.sha256((p / 'preview/gpu-audit.exe').read_bytes()).hexdigest()}
(p / 'source.json').write_text(json.dumps(proof, indent=2) + '\n')
