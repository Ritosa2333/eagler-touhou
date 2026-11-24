import argparse
import hashlib
import json
from pathlib import Path
import struct
from bullet_probe import NativeBullet,ROOT,EXE_SHA256,datum,emulate
MANAGER,OBJECT,POSITION=0x300000,0x390000,0x3a0000
def probe(executable):
    native=NativeBullet(executable)
    try:
        chunks=[native.chunk(0x40fc60,0x40fcf5)];cases=[]
        for lifetime in (5,6,100):
            for delay in (0,1,3):
                for x,y in ((0,0),(50,0),(51,0),(35,35)):
                    image=bytearray(0x68);struct.pack_into('<3f',image,0x10,0,0,0)
                    struct.pack_into('<f',image,0x1c,40);struct.pack_into('<i',image,0x24,lifetime);struct.pack_into('<i',image,0x38,delay)
                    result=emulate(chunks,0x40fc60,[datum('object',OBJECT,bytes(image),writable=False),
                        datum('bucket',MANAGER+0x65a20,struct.pack('<I',OBJECT),writable=False),
                        datum('position',POSITION,struct.pack('<3f',x,y,0),writable=False)],
                        registers={'eax':0,'ecx':0,'edx':MANAGER,'esi':POSITION},arguments=struct.pack('<f',10),budget=256)
                    if result['stop']['reason']!='returned':raise ValueError(result['stop'])
                    result.update(input={'lifetime':lifetime,'delay':delay,'x':x,'y':y,'iceRadius':40,'bulletRadius':10},
                        selected=result['registers']['eax']==OBJECT)
                    cases.append(result)
        return {'schema':'th128-ice-contact/1','executable':{'sha256':EXE_SHA256,
            'sha256After':hashlib.sha256(native.executable.read_bytes()).hexdigest()},'cases':cases,'code':native.code,
            'generatorSha256':hashlib.sha256(Path(__file__).read_bytes()).hexdigest(),'apiReturnsAssigned':False,
            'scope':'complete original40fc60 single-node bucket contact returns; strict lifetime/delay/radius boundary cases; no allocation/API stubs'}
    finally:native.image.close()
if __name__=='__main__':
    parser=argparse.ArgumentParser();parser.add_argument('--exe',required=True);args=parser.parse_args();report=probe(args.exe)
    (ROOT/'local/research/ice-contact-fixtures.json').write_text(json.dumps(report,indent=2)+'\n',encoding='utf8',newline='\n')
    print('36 complete native ice contact helper cases recorded')
