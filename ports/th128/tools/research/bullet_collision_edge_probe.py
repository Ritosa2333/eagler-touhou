import argparse
import hashlib
import json
from pathlib import Path
import struct
from bullet_probe import NativeBullet, ROOT, datum, emulate


def probe(executable):
    native=NativeBullet(executable)
    try:
        cases=[]
        for horizontal,vertical,player_x,player_y in ((2**-17,400.,0.,400.),(-2**-60,100.,0.,400.),
            (2**-149,400.,0.,400.),(0.,400.,2**-149,400.),(2**-90,398.,0.,400.),(2**-17,358.,0.,400.),
            (-2**-100,-65536.,65536.,2**-149)):
            center,player,profile=0x3a0000,0x300000,0x380000
            result=emulate([native.chunk(0x43c570,0x43c658)],0x43c570,
                [datum('center',center,struct.pack('<2f',horizontal,vertical),writable=False),
                 datum('position',player+0xecc,struct.pack('<2f',player_x,player_y),writable=False),
                 datum('profilePointer',player+0xf7c,profile,kind='I',writable=False),datum('radius',profile+4,struct.pack('<f',2.),writable=False),
                 datum('playerGlobal',0x4b8a80,player,kind='I',writable=False),native.constant(0x4a6910,8),native.constant(0x4a6a00,4)],
                registers={'eax':center},arguments=struct.pack('<f',2.4),stops=(0x43c608,),budget=256)
            if result['stop']['reason']=='returned':outcome=2 if result['registers']['eax']==2 else 0
            elif result['stop']=={'reason':'boundary','address':'0x0043c608'}:outcome=1
            else:raise ValueError(result['stop'])
            result.update(input=list(struct.unpack('<6I',struct.pack('<6f',horizontal,vertical,2.4,player_x,player_y,2.))),geometry=outcome);cases.append(result)
        return {'schema':'th128-original-small-coordinate-collision/1','cases':cases,'code':native.code,'constants':native.constants,
            'apiReturnsAssigned':False,'sourceExeSha256':hashlib.sha256(native.executable.read_bytes()).hexdigest(),
            'generatorSha256':hashlib.sha256(Path(__file__).read_bytes()).hexdigest()}
    finally:native.image.close()


if __name__=='__main__':
    parser=argparse.ArgumentParser();parser.add_argument('--exe',required=True);report=probe(parser.parse_args().exe)
    (ROOT/'local/research/bullet-collision-edge-fixtures.json').write_text(json.dumps(report,indent=2)+'\n',encoding='utf8')
    print('7 original tiny/subnormal coordinate circle geometry cases recorded')
