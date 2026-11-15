import argparse
import hashlib
import json
from pathlib import Path
import struct
from bullet_probe import NativeBullet,ROOT,EXE_SHA256
from freeze_probe import emulate

def probe(executable):
    native=NativeBullet(executable)
    try:
        target=struct.unpack('<I',native.read(0x41daa8+11*4,4))[0]
        if target!=0x41d3be:raise ValueError('unexpected -9989 dispatch target')
        prefix=emulate([native.chunk(0x41d3be,0x41d3d0)],target,registers={'ecx':0x300000},budget=16)
        if prefix['stop'].get('target')!='0x0043c3a0':raise ValueError(prefix['stop'])
        return {'schema':'th128-original-ecl-player-aim-prefix/1',
            'executable':{'sha256':EXE_SHA256,'sha256After':hashlib.sha256(native.executable.read_bytes()).hexdigest()},
            'dispatchTarget':target,'prefix':prefix,'completedGlobalQuery':False,'nativeCpuTrace':False,
            'windowsGameProcessLaunched':False,'code':native.code,
            'reusedCompleteHelperEvidence':'bullet-lifecycle-fixtures.json: actual-manager-aim-prefix',
            'generatorSha256':hashlib.sha256(Path(__file__).read_bytes()).hexdigest()}
    finally:native.image.close()

if __name__=='__main__':
    parser=argparse.ArgumentParser();parser.add_argument('--exe',required=True);arguments=parser.parse_args()
    report=probe(arguments.exe);output=ROOT/'local/research/ecl-player-aim-fixtures.json'
    output.write_text(json.dumps(report,indent=2)+'\n',encoding='utf-8',newline='\n')
    print('original -9989 dispatch prefix requires the existing native aim helper')
