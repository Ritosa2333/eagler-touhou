import argparse
import hashlib
import json
from pathlib import Path
import struct
from bullet_probe import NativeBullet,ROOT,EXE_SHA256

def probe(executable):
    native=NativeBullet(executable)
    try:
        cases=[]
        specifications=[(2,8,1,index,0,0,.07853981852531433,1.5,1.5,1) for index in range(8)]
        specifications.extend((mode,count,layers,index,layer,angle,spacing,speed,end,aim)
            for mode,count,layers,index,layer,angle,spacing,speed,end,aim in (
                (2,1,1,0,0,0,.1,1.5,1.5,1),(2,3,1,2,0,0,.1,1.5,1.5,1),
                (2,7,2,6,1,-1.25,.07853981852531433,1.5,.5,-.75),
                (2,8,3,7,2,.125,.25,3,.5,1.234567),
                (2,4,2,1,1,-3.2,.03125,2,1,3.2),
                (3,8,1,0,0,0,.1,1.5,1.5,1),(3,8,1,7,0,.125,.1,1.5,1.5,1),
                (3,3,3,2,2,-1.25,.125,1.5,.5,2)))
        for mode,count,layers,index,layer,angle,spacing,speed,end,aim in specifications:
            emitter=bytearray(0x380)
            struct.pack_into('<2i',emitter,0,18,3)
            struct.pack_into('<4f',emitter,0x14,angle,spacing,speed,end)
            struct.pack_into('<3h',emitter,0x364,count,layers,mode)
            sample=native.geometry_prefix(bytes(emitter),index,layer,aim)
            if sample['stop']['reason']!='boundary':raise ValueError(sample['stop'])
            cases.append(sample)
        payload=native.read(0x4b24d0+18*0x120,0x120)
        profile={'type':18,'sourceAddress':hex(0x4b24d0+18*0x120),'bytes':payload.hex(),
                 'sha256':hashlib.sha256(payload).hexdigest(),'script':struct.unpack_from('<i',payload)[0],
                 'hitboxSize':struct.unpack_from('<f',payload,0x104)[0],
                 'auxiliarySize':struct.unpack_from('<f',payload,0x108)[0],
                 'profileValue':struct.unpack_from('<i',payload,0x10c)[0],
                 'secondaryScript':struct.unpack_from('<i',payload,0x11c)[0]}
        return {'schema':'th128-original-mode2-emission/1','executable':{'sha256':EXE_SHA256,
            'sha256After':hashlib.sha256(native.executable.read_bytes()).hexdigest()},
            'profile':profile,'geometryCases':cases,'code':native.code,'constants':native.constants,
            'windowsGameProcessLaunched':False,'nativeCpuTrace':False,
            'generatorSha256':hashlib.sha256(Path(__file__).read_bytes()).hexdigest(),
            'limitations':['Geometry prefixes stop before the original constructor; no animation or allocator return is invented.',
                           'A table identity is not completion of type18 spawn, ANM, collision, cancellation or freeze behavior.']}
    finally:native.image.close()

if __name__=='__main__':
    parser=argparse.ArgumentParser();parser.add_argument('--exe',required=True);arguments=parser.parse_args()
    report=probe(arguments.exe);output=ROOT/'local/research/mode2-emission-fixtures.json'
    output.write_text(json.dumps(report,indent=2)+'\n',encoding='utf-8',newline='\n')
    print('original aim2/3 geometry prefixes and type18 table identity recorded')
