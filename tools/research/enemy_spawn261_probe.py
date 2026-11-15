import argparse
import hashlib
import json
from pathlib import Path
import struct
from bullet_probe import NativeBullet,ROOT,EXE_SHA256,datum
from enemy_probe import emulate

STATE,RUNNER,CONTEXT,COMMAND=0x300000,0x310000,0x320000,0x330000
def probe(executable):
    native=NativeBullet(executable)
    try:
        name=b'child\0\0\0';payload=struct.pack('<I',len(name))+name+struct.pack('<ffiii',10,20,30,1000,2)
        command=struct.pack('<iHHHBBI',0,261,16+len(payload),0,255,6,0)+payload
        spans=((0x418720,0x4188f9),(0x41e1d0,0x41e209),(0x41e190,0x41e1c1),(0x47bb30,0x47bbaa))
        result=emulate([native.chunk(start,end) for start,end in spans],0x418894,
            [datum('parent',STATE+0x34,struct.pack('<2f',100,200),writable=False),
             datum('runner',STATE+0x3f7c,RUNNER,kind='I',writable=False),datum('context',RUNNER+4,CONTEXT,kind='I',writable=False),
             datum('instructionPointer',CONTEXT+4,COMMAND,kind='I',writable=False),datum('command',COMMAND,command,writable=False),
             datum('inherited',STATE+0x23c,struct.pack('<12I',*range(12)),writable=False),
             datum('contextTag',RUNNER+0x11c0,0x1234,kind='I',writable=False),datum('cpu',0x4dcd68,0,writable=False)],
            registers={'eax':261,'ebx':STATE,'edx':RUNNER,'edi':COMMAND},stack={0x18:struct.pack('<I',COMMAND)},
            stack_capture={'request':(0x3f4,0x54)},budget=2048)
        if result['stop'].get('target')!='0x00414ff0':raise ValueError(result['stop'])
        values=struct.unpack('<3f5I12II',bytes.fromhex(result['stackSnapshots']['request']['after']))
        return {'schema':'th128-original-enemy261-request/1','executable':{'sha256':EXE_SHA256,
            'sha256After':hashlib.sha256(native.executable.read_bytes()).hexdigest()},'result':result,'position':list(values[:3]),
            'mode':values[6],'inheritedWords':list(values[8:20]),'code':native.code,
            'nativeCpuTrace':False,'windowsGameProcessLaunched':False,'generatorSha256':hashlib.sha256(Path(__file__).read_bytes()).hexdigest()}
    finally:native.image.close()

if __name__=='__main__':
    parser=argparse.ArgumentParser();parser.add_argument('--exe',required=True);arguments=parser.parse_args()
    report=probe(arguments.exe);output=ROOT/'local/research/enemy-spawn261-fixtures.json'
    output.write_text(json.dumps(report,indent=2)+'\n',encoding='utf-8',newline='\n')
    print('original261 absolute mode1 request prefix recorded')
