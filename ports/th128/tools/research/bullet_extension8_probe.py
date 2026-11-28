import argparse
import hashlib
import json
from pathlib import Path
import struct
from bullet_probe import ROOT,datum,emulate
from bullet_extension4_probe import NativeExtension4,BULLET

def probe(executable):
    native=NativeExtension4(executable)
    try:
        cases=[]
        for time,period,acceleration,rotation,speed,angle in ((0,60,.0166666675,.01,1.5,.5),(59,60,-.03,0,2.5,-.5),(60,60,.02,.01,1,.5),(0,1,0,.2,2,3.1),(1,1,0,.2,2,3.1),(0,0,0,0,0,0)):
            image=bytearray(0x11b8);struct.pack_into('<I',image,0xa20,8);struct.pack_into('<2f',image,0x9f0,speed,angle);struct.pack_into('<3f',image,0x9e4,.5,1.5,0)
            struct.pack_into('<iifII',image,0xdec,time-1,time,float(time),0x4b6f58,1);struct.pack_into('<2f',image,0xe00,acceleration,rotation);struct.pack_into('<i',image,0xe20,period)
            spans=((0x40adf0,0x40ae90),(0x46a660,0x46a6f9),(0x40d110,0x40d130),(0x402ac0,0x402b18))
            result=emulate([native.chunk(*span) for span in spans],0x40adf0,[datum('bullet',BULLET,bytes(image)),*native.data()],registers={'eax':BULLET},budget=2048)
            if result['stop']['reason']!='returned':raise ValueError(result['stop'])
            after=bytes.fromhex(result['memory']['bullet']['after']);result.update(input=[time,period,acceleration,rotation,speed,angle],expected=[result['registers']['eax'],struct.unpack_from('<I',after,0xa20)[0],struct.unpack_from('<i',after,0xdf0)[0],*struct.unpack_from('<2I',after,0x9f0),*struct.unpack_from('<3I',after,0x9e4)]);cases.append(result)
        return {'schema':'th128-original-extension8/1','cases':cases,'code':native.code,'constants':native.constants,'apiReturnsAssigned':False,'sourceExeSha256':hashlib.sha256(native.executable.read_bytes()).hexdigest(),'generatorSha256':hashlib.sha256(Path(__file__).read_bytes()).hexdigest()}
    finally:native.image.close()
if __name__=='__main__':
    parser=argparse.ArgumentParser();parser.add_argument('--exe',required=True);report=probe(parser.parse_args().exe)
    (ROOT/'local/research/bullet-extension8-fixtures.json').write_text(json.dumps(report,indent=2)+'\n',encoding='utf8');print('6 complete original ETEX8 angular/speed updates captured')
