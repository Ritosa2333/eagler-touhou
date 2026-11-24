import argparse
import hashlib
import json
from pathlib import Path
import struct
from bullet_probe import NativeBullet,ROOT,EXE_SHA256,datum
from freeze_probe import emulate

STATE,ANIMATION=0x300000,0x390000

def probe(executable):
    native=NativeBullet(executable)
    try:
        cases=[]
        for width,height,scale_x,scale_y in ((48,32,1,1),(48,32,-2,-.5),(32,32,1,1),
            (32,48,1,1),(12.5,.125,.25,2),(0,32,1,0),(64,96,0,-1)):
            result=emulate([native.chunk(0x416417,0x416453)],0x416417,
                [datum('scale',ANIMATION+0x40,struct.pack('<2f',scale_x,scale_y),writable=False),
                 datum('spriteSize',ANIMATION+0x60,struct.pack('<2f',width,height),writable=False),
                 datum('bounds',STATE+0x3e20,bytes(8))],registers={'eax':ANIMATION,'ebx':STATE},
                stops=[0x416453],budget=64)
            if result['stop']['reason']!='boundary':raise ValueError(result['stop'])
            final=bytes.fromhex(result['memory']['bounds']['after'])
            result.update(width=width,height=height,scaleX=scale_x,scaleY=scale_y,bounds=list(struct.unpack('<2f',final)))
            cases.append(result)
        return {'schema':'th128-original-enemy-appearance/1','executable':{'sha256':EXE_SHA256,
            'sha256After':hashlib.sha256(native.executable.read_bytes()).hexdigest()},'cases':cases,'code':native.code,
            'windowsGameProcessLaunched':False,'nativeCpuTrace':False,
            'generatorSha256':hashlib.sha256(Path(__file__).read_bytes()).hexdigest()}
    finally:native.image.close()

if __name__=='__main__':
    parser=argparse.ArgumentParser();parser.add_argument('--exe',required=True);arguments=parser.parse_args()
    report=probe(arguments.exe);output=ROOT/'local/research/enemy-appearance-fixtures.json'
    output.write_text(json.dumps(report,indent=2)+'\n',encoding='utf-8',newline='\n')
    print('original rectangular/scaled enemy culling prefix recorded')
