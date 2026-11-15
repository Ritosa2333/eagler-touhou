import argparse
import hashlib
import json
from pathlib import Path
import struct
from bullet_probe import NativeBullet, ROOT, datum, EXE_SHA256
from freeze_probe import emulate, STACK_POINTER

MANAGER, SLOT, PLAYER, CONTROL = 0x800000, 0x800014, 0x300000, 0x3b0000

class NativeItem(NativeBullet):
    def birth(self, position=(0,100,.1),kind=2,angle=-1.5707963705062866,speed=1):
        item=bytearray(0xa40)
        result=emulate([self.chunk(0x429544,0x4296c6),self.chunk(0x429970,0x429990)],0x429544,
            [datum('item',SLOT,bytes(item)),datum('position',0x390000,struct.pack('<3f',*position),writable=False),
             datum('bulletManager',0x4b8930,0x3c0000,kind='I',writable=False),
             datum('bulletResource',0x3c0000+0x8a7f9c,0x710000,kind='I',writable=False),
             self.constant(0x4a6650,4),self.constant(0x4a6640,4)],
            registers={'edi':MANAGER,'ebx':kind,'ecx':0x390000,'ebp':STACK_POINTER+0x100},
            stack={0x108:struct.pack('<I',kind),0x10c:struct.pack('<f',angle),0x110:struct.pack('<f',speed)},budget=4096)
        if result['stop'].get('target')!='0x00402ea0':raise ValueError(result['stop'])
        final=bytes.fromhex(result['memory']['item']['after'])
        result.update(position=list(position),kind=kind,angle=angle,speed=speed,
            fields=struct.unpack_from('<3f',final,0x9d4)+struct.unpack_from('<3f',final,0x9e0)+
                   struct.unpack_from('<ii fII',final,0x9f0)+struct.unpack_from('<4i',final,0xa18))
        return result

    def tick(self,position=(0,100,.1),velocity=(0,-1,0),age=0,state=1,player_state=1,player_y=400):
        item=bytearray(0xa40);struct.pack_into('<3f',item,0x9d4,*position);struct.pack_into('<3f',item,0x9e0,*velocity)
        struct.pack_into('<ii fII',item,0x9f0,age-1,age,float(age),0x4b6f58,1)
        struct.pack_into('<2i',item,0xa18,state,2)
        constants=[self.constant(pointer,size) for pointer,size in
            ((0x4a66a0,4),(0x4a6be0,8),(0x4a682c,4),(0x4a6bd8,8),(0x4a6688,4))]
        result=emulate([self.chunk(0x428640,0x4288b5)],0x428640,
            [datum('item',SLOT,bytes(item)),datum('playerState',PLAYER+0xf78,player_state,writable=False),
             datum('playerY',PLAYER+0xed0,struct.pack('<f',player_y),writable=False),
             datum('controlGlobal',0x4b8950,CONTROL,kind='I',writable=False),datum('bypass',CONTROL+0x5a1c,0,writable=False),
             datum('clock',0x4b6f58,struct.pack('<f',1),writable=False),*constants],
            registers={'edi':SLOT,'ebx':PLAYER},stops=[0x428aaf,0x42872c,0x4290c5],budget=1024)
        if result['stop']['reason']!='boundary':raise ValueError(result['stop'])
        result.update(position=list(position),velocity=list(velocity),age=age,state=state,playerState=player_state,playerY=player_y)
        return result

    def contact(self,position=(0,400,.1),focused=False):
        block=bytearray(0x48)
        struct.pack_into('<6f',block,0,-15,385,0,15,415,0)
        struct.pack_into('<6f',block,24,-50,350,0,50,450,0)
        struct.pack_into('<6f',block,48,-50,350,0,50,450,0)
        item=bytearray(0xa40);struct.pack_into('<3f',item,0x9d4,*position);struct.pack_into('<i',item,0xa18,1)
        result=emulate([self.chunk(0x428640,0x429010)],0x428640,
            [datum('item',SLOT,bytes(item)),datum('state',PLAYER+0xf78,1,writable=False),
             datum('playerY',PLAYER+0xed0,struct.pack('<f',400),writable=False),
             datum('controlGlobal',0x4b8950,CONTROL,kind='I',writable=False),datum('bypass',CONTROL+0x5a1c,0,writable=False),
             datum('clock',0x4b6f58,struct.pack('<f',1),writable=False),
             *[self.constant(pointer,size) for pointer,size in ((0x4a66a0,4),(0x4a6be0,8),(0x4a682c,4),(0x4a6bd8,8),(0x4a6688,4))],
             datum('ranges',PLAYER+0x43934,bytes(block),writable=False),
             datum('holding',0x4db210,8 if focused else 0,writable=False)],
            registers={'edi':SLOT,'ebx':PLAYER},stops=[0x428b52,0x428f92,0x428fbb],budget=1024)
        if result['stop']['reason']!='boundary':raise ValueError(result['stop'])
        result.update(position=list(position),focused=focused)
        return result

    def points(self,value=100000,player_y=400,state=1,kind=2):
        image=bytearray(0xa40);struct.pack_into('<i',image,0xa18,state)
        result=emulate([self.chunk(0x428cfb,0x428e2e),self.chunk(0x48fac0,0x48fb6b)],0x428cfb,
            [datum('item',SLOT,bytes(image)),datum('playerY',PLAYER+0xed0,struct.pack('<f',player_y),writable=False),
             datum('pointValue',0x4b4cdc,value,writable=False),datum('cpu',0x4dcd68,1,writable=False),
             self.constant(0x4a66a0,4)],registers={'edi':SLOT,'ebx':PLAYER},
            stack={0x10:struct.pack('<I',SLOT+0x9d4),0x14:struct.pack('<I',kind)},budget=1024)
        if result['stop'].get('target')!='0x00444010':raise ValueError(result['stop'])
        result.update(value=value,playerY=player_y,state=state,kind=kind,reward=result['registers']['esi'])
        return result

def probe(executable):
    native=NativeItem(executable)
    try:
        births=[native.birth(position=position,kind=kind) for kind in (1,2,3,8) for position in
                ((0,100,.1),(-200,0,.1),(200,448,.1))]
        ticks=[native.tick(position=position,velocity=velocity,age=age) for position,velocity,age in
              (((0,100,.1),(0,-1,0),0),((.125,100,.1),(.3,1,0),10),((0,447,.1),(0,2,0),50),
               ((0,-16,.1),(-.5,-1,0),39),((0,100,.1),(0,1.99,0),0))]
        ticks += [native.tick(age=40),native.tick(player_y=20),native.tick(player_state=2)]
        contacts=[native.contact(position=(horizontal,400,.1),focused=focused) for focused in (False,True)
                  for horizontal in (-50.000003814697266,-50,-49.999996185302734,-15.000000953674316,-15,
                                     -14.999999046325684,0,14.999999046325684,15,15.000000953674316,
                                     49.999996185302734,50,50.000003814697266)]
        points=[native.points(value=value,player_y=y,state=state) for value in (10000,123456,999999)
                for y,state in ((127,1),(128,1),(129,1),(400,1),(432,1),(400,3))]
        return {'schema':'th128-original-item-prefixes/1','executable':{'sha256':EXE_SHA256,
            'sha256After':hashlib.sha256(native.executable.read_bytes()).hexdigest()},'nativeCpuTrace':False,
            'windowsGameProcessLaunched':False,'birthCases':births,'tickCases':ticks,'contactCases':contacts,'pointCases':points,'code':native.code,'constants':native.constants,
            'generatorSha256':hashlib.sha256(Path(__file__).read_bytes()).hexdigest()}
    finally:native.image.close()

def main():
    parser=argparse.ArgumentParser();parser.add_argument('--exe',required=True);arguments=parser.parse_args()
    output=ROOT/'local/research/item-fixtures.json';report=probe(arguments.exe)
    output.write_text(json.dumps(report,indent=2)+'\n',encoding='utf-8',newline='\n')
    print('original item birth/tick prefixes recorded without ANM/call stubs')

if __name__=='__main__':main()
