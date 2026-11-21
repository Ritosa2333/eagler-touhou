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
        for active,remaining,duration in ((False,0,0),(False,0,1),(False,0,60),(True,0,60),(True,1,60),(True,59,60)):
            image=bytearray(0x11b8);struct.pack_into('<I',image,0,0x11);struct.pack_into('<I',image,0x430,0x370000);struct.pack_into('<i',image,0x438,1)
            struct.pack_into('<3f',image,0x9d8,10,100,.1);struct.pack_into('<3f',image,0x9e4,.5,1.5,0);struct.pack_into('<2f',image,0x9f0,1.,.5)
            struct.pack_into('<i',image,0xa18,5);struct.pack_into('<I',image,0xa20,0x80000000 if active else 0);struct.pack_into('<H',image,0xa2a,1);struct.pack_into('<i',image,0xa38,-1);struct.pack_into('<i',image,0xa3c,2 if active else 1)
            struct.pack_into('<4f4i3I',image,0xa44+44,0,0,0,0,duration,-999999,0,0,0x80000000,0,0)
            struct.pack_into('<4f4i3I',image,0xa44+88,.0166666675,-999.,0,0,30,-999999,0,0,4,0,0)
            if active:struct.pack_into('<iifII',image,0xec4,remaining+1,remaining,float(remaining),0x4b6f58,1)
            struct.pack_into('<ifi',image,0x119c,0,0.,0x4b6f58);struct.pack_into('<f',image,0x1180,1)
            sprite=bytearray(0x48);struct.pack_into('<2f',sprite,0x34,16,16)
            spans=((0x408640,0x408d09),(0x409a90,0x40a85e),(0x40ab90,0x40accd),(0x403280,0x4032bd),(0x402ac0,0x402b18),(0x46aa20,0x46aa85),(0x48fac0,0x48fadc),(0x4728fd,0x472907),(0x40d110,0x40d130),(0x490090,0x49014a),(0x491b05,0x491b1c),(0x491b78,0x491c2a))
            result=emulate([*[native.chunk(*span) for span in spans],*native.aim_chunks()],0x408640,
                [datum('bullet',BULLET,bytes(image)),datum('sprite',0x370000,bytes(sprite),writable=False),datum('cookie',0x4b01f4,0,writable=False),datum('sse',0x4dcd68,1,writable=False),native.constant(0x408d38,20),native.constant(0x40a860,108),native.constant(0x4a6600,16),native.constant(0x4a6658,8),native.constant(0x4a6688,4),native.constant(0x4a6818,8),*native.data()],registers={'ebx':BULLET},stops=(0x408d09,),budget=12000)
            if result['stop']['reason']!='boundary':raise ValueError(result['stop'])
            after=bytes.fromhex(result['memory']['bullet']['after']);result.update(input=[int(active),remaining,duration],expected=[*struct.unpack_from('<6I',after,0x9d8),*struct.unpack_from('<2I',after,0x9f0),struct.unpack_from('<i',after,0xa3c)[0],struct.unpack_from('<I',after,0xa20)[0],struct.unpack_from('<i',after,0xec8)[0],struct.unpack_from('<i',after,0xda8)[0]]);cases.append(result)
        return {'schema':'th128-original-bullet-delay/1','cases':cases,'code':native.code,'constants':native.constants,'sourceExeSha256':hashlib.sha256(native.executable.read_bytes()).hexdigest(),'generatorSha256':hashlib.sha256(Path(__file__).read_bytes()).hexdigest(),'apiReturnsAssigned':False}
    finally:native.image.close()
if __name__=='__main__':
    parser=argparse.ArgumentParser();parser.add_argument('--exe',required=True);report=probe(parser.parse_args().exe)
    (ROOT/'local/research/bullet-delay-fixtures.json').write_text(json.dumps(report,indent=2)+'\n',encoding='utf8');print('6 complete original delay/next-acceleration pre-ANM updates captured')
