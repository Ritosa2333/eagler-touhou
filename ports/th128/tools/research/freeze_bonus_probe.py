import hashlib
import json
from pathlib import Path
import struct
from bullet_probe import NativeBullet,ROOT,EXE_SHA256,datum,emulate

def probe(executable):
    native=NativeBullet(executable)
    try:
        chunks=[native.chunk(0x40f910,0x40f9c3),native.chunk(0x48fac0,0x48fb6b)]
        constants=[native.constant(0x4a6b80,8),native.constant(0x4a6590,8)]
        cases=[]
        for pixels in (0,1,17198,17203,17204,34406,34407,68812,68813,103219,103220,172032):
            for score in (0,999999990):
                result=emulate(chunks,0x40f910,[datum('pixels',0x390000,pixels),datum('manager',0x300000+0x6b204,bytes(8)),
                    datum('score',0x4b4cc4,score),datum('cpu',0x4dcd68,1)]+constants,
                    registers={'ebx':0x300000,'ecx':0x390000},stops=(0x40f9c3,),budget=256)
                if result['stop']['reason']!='boundary':raise ValueError(result['stop'])
                data=bytes.fromhex(result['memory']['manager']['after'])
                result.update(input={'pixels':pixels,'score':score},bonus=struct.unpack_from('<i',data)[0],
                    percent=struct.unpack_from('<f',data,4)[0],score=struct.unpack('<i',bytes.fromhex(result['memory']['score']['after']))[0])
                cases.append(result)
        return {'schema':'th128-native-freeze-bonus/1','sourceExeSha256':EXE_SHA256,'cases':cases,'code':native.code,
            'scope':'original40f910..40f9c3 score/bonus prefix, real ftol helper; excludes original sliding UI/level-up calls',
            'generatorSha256':hashlib.sha256(Path(__file__).read_bytes()).hexdigest(),'apiReturnsAssigned':False}
    finally:native.image.close()

if __name__=='__main__':
    import argparse
    parser=argparse.ArgumentParser();parser.add_argument('--exe',required=True);arguments=parser.parse_args()
    report=probe(arguments.exe);(ROOT/'local/research/freeze-bonus-fixtures.json').write_text(json.dumps(report,indent=2)+'\n',encoding='utf8',newline='\n')
    print('24 native mask-area/bonus/score cases recorded')
