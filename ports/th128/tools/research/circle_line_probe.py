import argparse
import hashlib
import json
import struct
from pathlib import Path
from bullet_probe import NativeBullet,ROOT,datum,emulate
def probe(executable):
    native=NativeBullet(executable)
    try:
        cases=[];manager=0x300000;buffer=0x3a0000
        for x,y,radius,angle,count in ((320.,300.,64.,0.,64),(157.,406.5625,128.,.5,64),(0.,0.,0.,-.2,12)):
            result=emulate([native.chunk(0x462d60,0x462e08),native.chunk(0x4630a0,0x4630c0),native.chunk(0x46a660,0x46a6f9)],0x462d60,
                [datum('bufferPointer',manager+0x8b96fc,buffer,kind='I',writable=False),datum('vertices',buffer,bytes((count+1)*20)),native.constant(0x4a6578,72)],registers={'ebx':manager},arguments=struct.pack('<4fiI',x,y,radius,angle,count,0x8040c0ff),stops=(0x462e08,),budget=10000)
            if result['stop']['reason']!='boundary':raise ValueError(result['stop'])
            data=bytes.fromhex(result['memory']['vertices']['after']);result.update(input=[x,y,radius,angle,count],vertices=[list(struct.unpack_from('<5I',data,index*20)) for index in range(count+1)]);cases.append(result)
        return {'schema':'th128-original-circle-line/1','cases':cases,'code':native.code,'constants':native.constants,'sourceExeSha256':hashlib.sha256(native.executable.read_bytes()).hexdigest(),'generatorSha256':hashlib.sha256(Path(__file__).read_bytes()).hexdigest(),'apiReturnsAssigned':False}
    finally:native.image.close()
if __name__=='__main__':
    parser=argparse.ArgumentParser();parser.add_argument('--exe',required=True);report=probe(parser.parse_args().exe)
    (ROOT/'local/research/circle-line-fixtures.json').write_text(json.dumps(report,indent=2)+'\n',encoding='utf8');print('3 original mode17 line buffers captured before graphics API')
