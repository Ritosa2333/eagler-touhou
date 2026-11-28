import argparse
import hashlib
import json
from pathlib import Path
import struct
from bullet_probe import NativeBullet,ROOT,EXE_SHA256,datum
from freeze_probe import emulate

BULLET=0x390000
def probe(executable):
    native=NativeBullet(executable)
    try:
        cases=[];flagCases=[]
        target=struct.unpack('<I',native.read(0x45d0d4+83*4,4))[0]
        if target!=0x45c501:raise ValueError('unexpected original ANM82 dispatch')
        for previous,value in ((0,1),(0xffffffff,0),(0x20000000,1),(6,2)):
            result=emulate([native.chunk(0x45c501,0x45c52a),native.chunk(0x459e1a,0x459e31)],target,
                [datum('flags',BULLET+0x4b0,previous,kind='I'),datum('pc',BULLET+0x424,0),
                 datum('command',0x370000,struct.pack('<HHhHI',82,12,0,0,value),writable=False)],
                registers={'edi':BULLET,'esi':0x370000},stops=[0x459e1a],budget=64)
            if result['stop']['reason']!='boundary':raise ValueError(result['stop'])
            result.update(previous=previous,value=value,flagsAfter=struct.unpack('<I',bytes.fromhex(result['memory']['flags']['after']))[0]);flagCases.append(result)
        for angle,enabled in ((0,True),(1.5707963705062866,True),(-3.1415927410125732,True),(3.,True),(0,False)):
            image=bytearray(0xa00);struct.pack_into('<f',image,0x34,.25);struct.pack_into('<I',image,0x4b8,0x40000000 if enabled else 0)
            struct.pack_into('<f',image,0x9f4,angle)
            result=emulate([native.chunk(0x408f88,0x408fdc),native.chunk(0x46a660,0x46a6f9)],0x408f88,
                [datum('bullet',BULLET,bytes(image)),native.constant(0x4a66f0,4),native.constant(0x4a6578,72)],
                registers={'esi':BULLET,'ebx':4},stops=[0x408fb8],budget=256)
            if result['stop']['reason']!='boundary':raise ValueError(result['stop'])
            final=bytes.fromhex(result['memory']['bullet']['after'])
            result.update(angle=angle,enabled=enabled,rotationAfter=struct.unpack_from('<f',final,0x34)[0],
                          rotationBits=struct.unpack_from('<I',final,0x34)[0],flagsAfter=struct.unpack_from('<I',final,0x4b8)[0])
            cases.append(result)
        return {'schema':'th128-original-bullet-draw-rotation/1','executable':{'sha256':EXE_SHA256,
            'sha256After':hashlib.sha256(native.executable.read_bytes()).hexdigest()},'cases':cases,'flagCases':flagCases,'code':native.code,'constants':native.constants,
            'nativeCpuTrace':False,'windowsGameProcessLaunched':False,'generatorSha256':hashlib.sha256(Path(__file__).read_bytes()).hexdigest()}
    finally:native.image.close()

if __name__=='__main__':
    parser=argparse.ArgumentParser();parser.add_argument('--exe',required=True);arguments=parser.parse_args()
    report=probe(arguments.exe);output=ROOT/'local/research/bullet-draw-rotation-fixtures.json'
    output.write_text(json.dumps(report,indent=2)+'\n',encoding='utf-8',newline='\n')
    print('original draw-time directional rotation prefix recorded')
