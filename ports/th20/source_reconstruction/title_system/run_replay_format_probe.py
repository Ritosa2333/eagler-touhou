"""Reproduce native full-entry observations; never changes reference assets."""
from pathlib import Path
import hashlib,json,struct,subprocess
base=Path(__file__).resolve().parent;root=base.parents[1]
manifest=json.loads((root/'reports/source_manifest.json').read_text(encoding='utf-8'))
original=Path(manifest['source_directory'])/'th20.exe'
exe=root/'source_reconstruction/sprite_renderer/pool_test/build/Release/th20_replay_format_probe.exe'
def digest(path):return hashlib.sha256(path.read_bytes()).hexdigest()
cases=[]
inputs=[(f'demo{demo}',root/f'assets/raw/demo{demo}.rpy',None) for demo in range(1,5)]
inputs += [(f'slow{value}',root/'assets/raw/demo1.rpy',value) for value in ['0.1','1','12.5','0.001']]
for name,path,value in inputs:
    before=digest(path)
    for page in [0,1]:
        log=base/f'replay_probe_{name}_page{page}.txt'
        command=[str(exe),str(original),str(path),str(page),str(log)]
        if value is not None:command.append(value)
        result=subprocess.run(command,capture_output=True,text=True,timeout=20)
        raw=log.read_text(encoding='ascii')
        fields=dict(line.split('=',1) for line in raw.splitlines() if '=' in line and not line.startswith('native_access_violation'))
        low_word=None
        if value is not None:
            source_float=struct.unpack('<f',struct.pack('<f',float(value)))[0]
            low_word=struct.unpack('<Q',struct.pack('<d',source_float))[0]&0xffffffff
        cases.append({'name':name,'page':page,'resource_sha256':before,'resource_unchanged':digest(path)==before,'test_storage_slowdown_override':value,'converted_double_low_word':None if low_word is None else f'{low_word:08x}','returncode':result.returncode,'stdout':result.stdout,'stderr':result.stderr,'log':log.name,'log_sha256':digest(log),'fields':fields,'exception_lines':[line for line in raw.splitlines() if line.startswith('native_access_violation')]})
report={'original_sha256':digest(original),'probe_sha256':digest(exe),'scope':'Complete original5240d0 drawing entry with actual decoded original demo metadata, plus explicitly labeled test-storage-only slowdown field changes. No instructions patched; no physical GPU rendering.','cases':cases,'source_hashes':{name:digest(base/name) for name in ['replay_format_probe.cpp','run_replay_format_probe.py','../archive/archive.cpp','../archive/archive.hpp','../replay_system/replay.hpp']}}
(base/'replay_format_probe.json').write_text(json.dumps(report,indent=2)+'\n',encoding='utf-8')
print(json.dumps({'cases':len(cases),'returned':sum(x['returncode']==0 for x in cases),'access_violations':sum((x['returncode']&0xffffffff)==0xc0000005 for x in cases),'all_resources_unchanged':all(x['resource_unchanged'] for x in cases)}))
