"""Record a reproducible, source-only check of the four supplied demo assets."""
import hashlib,json,pathlib,subprocess
BASE=pathlib.Path(__file__).resolve().parent
archive=pathlib.Path(r'E:\BaiduNetdiskDownload\PCGAME-Touhou.Kinjoukyou.Fossilized.Wonders-P2P\Touhou.Kinjoukyou.Fossilized.Wonders\th20.dat')
exe=BASE/'oracle/build/Release/th20_replay_demo_verify.exe'
run=subprocess.run([str(exe),str(archive)],capture_output=True,text=True,check=True)
result=json.loads(run.stdout.splitlines()[-1]);result['observations']=run.stdout.splitlines()[:-1]
def digest(path):
    with path.open('rb') as stream:return hashlib.file_digest(stream,'sha256').hexdigest()
result['archive_sha256']=digest(archive);result['verifier_sha256']=digest(exe)
inputs=['oracle/demo_verify.cpp','load.cpp','replay.hpp','../archive/resource_manager.cpp','../archive/archive.cpp','../runtime_core/runtime_core.cpp','../platform_services/services.cpp','../platform_services/clock.cpp','../game_session/session.cpp','../core_scheduler/scheduler.cpp','../platform_services/configuration.cpp']
result['source_hashes']={p:digest(BASE/p) for p in inputs}
result['scope']='Actual source file loader, archive lookup, decryption, shared LZSS decode, record/input/FPS stream layout; no whole-game playback validation.'
(BASE/'demo_validation.json').write_text(json.dumps(result,indent=2)+'\n',encoding='utf8')
print(run.stdout,end='')
