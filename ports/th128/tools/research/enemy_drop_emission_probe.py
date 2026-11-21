import argparse
import hashlib
import json
from pathlib import Path
import struct
from bullet_probe import NativeBullet,ROOT,EXE_SHA256,datum
from enemy_probe import emulate
from freeze_probe import STACK_POINTER

DROPS,POSITION=0x300000,0x390000

class NativeDropEmission(NativeBullet):
    def chunks(self):
        return [self.chunk(0x4148e0,0x414a8f),self.chunk(0x46a460,0x46a4f5),
                self.chunk(0x41f7c0,0x41f7e0),self.chunk(0x46a700,0x46a78e)]
    def data(self,seed):
        return [datum('random',0x4d2ae0,struct.pack('<HHI',seed,0,0)),datum('threadLockFlag',0x4d33f8,0,writable=False),
                *[self.constant(start,size) for start,size in ((0x4a6578,72),(0x4a6660,4),(0x4a6788,16),
                 (0x4a6b10,8),(0x4a6710,8),(0x4a6b08,8))]]
    def first(self,seed=0,count=2,width=24,height=24,empty=False):
        image=bytearray(0x34)
        if not empty:struct.pack_into('<i',image,8,count)
        struct.pack_into('<2f',image,0x2c,width,height)
        result=emulate(self.chunks(),0x4148e0,[datum('drops',DROPS,bytes(image)),
            datum('position',POSITION,struct.pack('<3f',10,100,.1),writable=False),*self.data(seed)],
            registers={'edi':POSITION},arguments=struct.pack('<I',DROPS),budget=2048)
        if empty:
            if result['stop']['reason']!='returned':raise ValueError(result['stop'])
        elif result['stop'].get('target')!='0x004293d0':raise ValueError(result['stop'])
        random=bytes.fromhex(result['memory']['random']['after']);head=bytes.fromhex(result['stackHead'])
        result.update(seedBefore=seed,seedAfter=struct.unpack_from('<H',random)[0],callsAfter=struct.unpack_from('<I',random,4)[0],
                      width=width,height=height,count=count,empty=empty,
                      itemKind=None if empty else struct.unpack_from('<i',head)[0],
                      positionBits=None if empty else list(struct.unpack_from('<3I',head,24)),
                      angleBits=None if empty else struct.unpack_from('<I',head,4)[0],
                      speedBits=None if empty else struct.unpack_from('<I',head,8)[0])
        return result
    def angle_tail(self,seed,angle):
        result=emulate(self.chunks(),0x4149e7,self.data(seed),registers={'ebp':STACK_POINTER+0x100},
            stack={0xf8:struct.pack('<f',angle),0xf4:struct.pack('<i',0),0xfc:struct.pack('<I',DROPS+8)},
            stops=[0x414a45],budget=512,stack_capture={'angle':(0xf8,4)})
        if result['stop']['reason']!='boundary':raise ValueError(result['stop'])
        random=bytes.fromhex(result['memory']['random']['after'])
        result.update(seedBefore=seed,angleBefore=angle,angleAfterBits=struct.unpack('<I',bytes.fromhex(result['stackSnapshots']['angle']['after']))[0],
                      seedAfter=struct.unpack_from('<H',random)[0],callsAfter=struct.unpack_from('<I',random,4)[0],
                      independentAfterItemSuffix=True)
        return result
    def geometry(self,seed,angle,width=24,height=24):
        image=bytearray(0x34);struct.pack_into('<i',image,8,2);struct.pack_into('<2f',image,0x2c,width,height)
        result=emulate(self.chunks(),0x414940,[datum('drops',DROPS,bytes(image),writable=False),
            datum('position',POSITION,struct.pack('<3f',10,100,.1),writable=False),*self.data(seed)],
            registers={'eax':DROPS,'edi':POSITION,'ebp':STACK_POINTER+0x100},
            stack={0xf8:struct.pack('<f',angle),0xf4:struct.pack('<i',1)},stops=[0x4149e2],budget=512,
            stack_capture={'position':(0xe0,12)})
        if result['stop']['reason']!='boundary':raise ValueError(result['stop'])
        head=bytes.fromhex(result['stackHead']);random=bytes.fromhex(result['memory']['random']['after'])
        result.update(seedBefore=seed,seedAfter=struct.unpack_from('<H',random)[0],callsAfter=struct.unpack_from('<I',random,4)[0],
                      angleBefore=angle,width=width,height=height,positionBits=list(struct.unpack('<3I',bytes.fromhex(result['stackSnapshots']['position']['after']))))
        return result

def probe(executable):
    native=NativeDropEmission(executable)
    try:
        entries=[native.first(seed) for seed in (0,1,0x1234,65535)]
        angles=[native.angle_tail(seed,angle) for seed,angle in ((0,0),(1,-1.25),(0x1234,3.0),(65535,-3.0))]
        geometries=[native.geometry(seed,angle,width,height) for seed,angle,width,height in
                    ((0,0,24,24),(1,1.25,24,24),(0x1234,-2.5,.125,99.5),(65535,3,0,0))]
        empty=[native.first(seed,empty=True) for seed in (0,0x1234)]
        return {'schema':'th128-original-additional-drop-prefixes/1','executable':{'sha256':EXE_SHA256,
            'sha256After':hashlib.sha256(native.executable.read_bytes()).hexdigest()},'entryCases':entries,'angleCases':angles,
            'geometryCases':geometries,'emptyCases':empty,'completedNonemptyHelper':False,'code':native.code,'constants':native.constants,
            'nativeCpuTrace':False,'windowsGameProcessLaunched':False,'generatorSha256':hashlib.sha256(Path(__file__).read_bytes()).hexdigest()}
    finally:native.image.close()

if __name__=='__main__':
    parser=argparse.ArgumentParser();parser.add_argument('--exe',required=True);arguments=parser.parse_args()
    report=probe(arguments.exe);output=ROOT/'local/research/enemy-drop-emission-fixtures.json'
    output.write_text(json.dumps(report,indent=2)+'\n',encoding='utf-8',newline='\n')
    print('original nonempty drop prefix, geometry, post-item angle and empty-list returns recorded')
