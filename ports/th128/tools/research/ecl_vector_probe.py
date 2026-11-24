import argparse
import hashlib
import json
from pathlib import Path
import struct
from bullet_probe import NativeBullet,ROOT,datum,emulate

def probe(executable):
    native=NativeBullet(executable)
    try:
        cases=[]
        for opcode in (81,82,86):
            for first,second in ((0.,64.),(.5,120.),(1.5707963705062866,32.),(-3.1415927410125732,24.),(10.,-2.),(0.000000001,22.),(-1000.,16.)):
                state=0x380000;command=0x390000;image=bytearray(0x1020);struct.pack_into('<I',image,4,command)
                payload=struct.pack('<iHHHBBI4f',0,81,32,3,255,4,0,0.,4.,first,second) if opcode==81 else struct.pack('<iHHHBBIf',0,82,20,1,255,1,0,0.) if opcode==82 else struct.pack('<iHHHBBI3f',0,86,28,1,255,3,0,0.,first,second)
                if opcode==82:struct.pack_into('<f',image,8,first)
                start,end=(0x46f850,0x46f8c3) if opcode==81 else (0x46f911,0x46f93b) if opcode==82 else (0x46f8c3,0x46f911)
                chunks=[native.chunk(start,end),native.chunk(0x46ff70,0x470026),native.chunk(0x470170,0x4701c9),native.chunk(0x470380,0x4703a0),native.chunk(0x46a700,0x46a790),native.chunk(0x48fac0,0x48fadc),native.chunk(0x46fbe5,0x46fbe8)]
                result=emulate(chunks,start,[datum('state',state,bytes(image)),datum('command',command,payload,writable=False),datum('sse',0x4dcd68,1,writable=False),native.constant(0x4a6578,72)],registers={'ebx':state},stops=(0x46fbe5,),budget=4096)
                if result['stop']['reason']!='boundary':raise ValueError(result['stop'])
                after=bytes.fromhex(result['memory']['state']['after']);result.update(opcode=opcode,input=[first,second],outputBits=list(struct.unpack_from('<2I' if opcode==81 else '<I',after,8)));cases.append(result)
        return {'schema':'th128-original-ecl81-86/1','cases':cases,'code':native.code,'constants':native.constants,'sourceExeSha256':hashlib.sha256(native.executable.read_bytes()).hexdigest(),
            'generatorSha256':hashlib.sha256(Path(__file__).read_bytes()).hexdigest(),'apiReturnsAssigned':False,'scope':'Complete original81/86 scalar handlers and real argument/destination/polar/angle helpers; no API stubs.'}
    finally:native.image.close()
if __name__=='__main__':
    parser=argparse.ArgumentParser();parser.add_argument('--exe',required=True);report=probe(parser.parse_args().exe)
    (ROOT/'local/research/ecl-vector-fixtures.json').write_text(json.dumps(report,indent=2)+'\n',encoding='utf8');print('21 complete original81/82/86 handlers captured')
