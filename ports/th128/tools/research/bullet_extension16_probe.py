import argparse
import hashlib
import json
from pathlib import Path
import struct
from bullet_probe import NativeBullet,ROOT,EXE_SHA256,datum,emulate

BULLET,PLAYER=0x390000,0x3b0000

def fields(image):
    return {'active':struct.unpack_from('<I',image,0xa20)[0],'index':struct.unpack_from('<i',image,0xa3c)[0],
        'angleBits':struct.unpack_from('<I',image,0x9f4)[0],'speedBits':struct.unpack_from('<I',image,0x9f0)[0],
        'velocityBits':list(struct.unpack_from('<3I',image,0x9e4)),
        'extensionAngleBits':struct.unpack_from('<I',image,0xe4c)[0],'extensionSpeedBits':struct.unpack_from('<I',image,0xe48)[0],
        'timer':list(struct.unpack_from('<5I',image,0xe34)),
        'period':struct.unpack_from('<i',image,0xe68)[0],'limit':struct.unpack_from('<i',image,0xe6c)[0],
        'count':struct.unpack_from('<i',image,0xe70)[0],'mode':struct.unpack_from('<i',image,0xe74)[0],
        'angleMode':struct.unpack_from('<i',image,0xe78)[0]}

class NativeExtension16(NativeBullet):
    def integrated(self,half=False,active=False,time=0,cooldown=0):
        image=bytearray.fromhex(self.initialize()['memory']['bullet']['after'])
        if not active:
            struct.pack_into('<I',image,0xa20,0);struct.pack_into('<i',image,0xa3c,1)
        struct.pack_into('<Ii',image,0,0x11,cooldown);struct.pack_into('<i',image,0x438,1)
        struct.pack_into('<3f',image,0x9e4,.5,1.5,0);struct.pack_into('<i',image,0xa18,5)
        struct.pack_into('<H',image,0xa2a,2 if half else 1);struct.pack_into('<h',image,0x11b0,8)
        struct.pack_into('<ifi',image,0x119c,0,0.,0x4b6f58);struct.pack_into('<f',image,0x1180,1)
        if active:struct.pack_into('<iifII',image,0xe34,time-1,time,float(time),0x4b6f58,1)
        sprite=bytearray(0x48);struct.pack_into('<2f',sprite,0x34,16,16);struct.pack_into('<I',image,0x430,0x370000)
        result=emulate([self.chunk(0x408640,0x408d09),self.chunk(0x409a90,0x409e01),self.chunk(0x40a843,0x40a85e),
            self.chunk(0x40b200,0x40b3c5),self.chunk(0x403280,0x4032bd),self.chunk(0x4728fd,0x472907),
            self.chunk(0x40d110,0x40d130),self.chunk(0x46a660,0x46a6f9),*self.aim_chunks()],0x408640,
            [datum('bullet',BULLET,bytes(image)),datum('sprite',0x370000,bytes(sprite),writable=False),
             datum('cookie',0x4b01f4,0,writable=False),self.constant(0x408d38,20),
             self.constant(0x40a860,108),self.constant(0x4a6600,16),self.constant(0x4a6658,8),*self.data()],
            registers={'ebx':BULLET},stops=[0x408d09],budget=8192)
        if result['stop']['reason']!='boundary':raise ValueError(result['stop'])
        after=bytes.fromhex(result['memory']['bullet']['after'])
        result.update(half=half,initialActive=active,initialTime=time,cooldown=cooldown,stateAfter=fields(after),
            positionBits=list(struct.unpack_from('<3I',after,0x9d8)),lifeClockAfter=struct.unpack_from('<i',after,0x119c)[0],
            cooldownAfter=struct.unpack_from('<i',after,4)[0],graceAfter=struct.unpack_from('<i',after,0xa18)[0])
        return result
    def aim_chunks(self):
        return [self.chunk(start,end) for start,end in ((0x43c3a0,0x43c40c),(0x49007a,0x490084),
            (0x491380,0x4913be),(0x491443,0x4914bb),(0x491667,0x4916f3),(0x49289c,0x4928bc),
            (0x4928ec,0x492928),(0x4917c3,0x4917ca))]
    def data(self):
        return [datum('clock',0x4b6f58,struct.pack('<f',1),writable=False),
            datum('playerPointer',0x4b8a80,PLAYER,kind='I',writable=False),
            datum('player',PLAYER+0xecc,struct.pack('<2f',0,400),writable=False),
            datum('crtDispatch',0x4b7720,0,writable=False),datum('crtExceptions',0x4dbc10,0,writable=False),
            *[self.constant(start,size) for start,size in ((0x4a66f0,4),(0x4a6d3a,10),(0x4a6d4c,16),(0x4a9e80,80),
                (0x4a6578,72),(0x4a6a30,8),(0x4a6694,12),(0x4a6718,8),(0x4a6ad0,8))]]
    def initialize(self,sound=-1,offset=0,speed=-999,period=60,limit=1,mode=2):
        image=bytearray(0x11b8);struct.pack_into('<3f',image,0x9d8,10,100,.1)
        struct.pack_into('<2f',image,0x9f0,3.,.5);struct.pack_into('<I',image,0xa20,0)
        struct.pack_into('<i',image,0xa38,sound);struct.pack_into('<i',image,0xa3c,1)
        struct.pack_into('<4f4i3I',image,0xa44+44,offset,speed,-999999,-999999,period,limit,mode,0,16,0,0)
        result=emulate([self.chunk(0x409a90,0x409e01),self.chunk(0x40a843,0x40a85e),
            self.chunk(0x403280,0x4032bd),self.chunk(0x4728fd,0x472907),*self.aim_chunks()],0x409a90,
            [datum('bullet',BULLET,bytes(image)),datum('cookie',0x4b01f4,0,writable=False),
             self.constant(0x40a860,108),*self.data()],registers={'ecx':BULLET},budget=4096)
        if result['stop']['reason'] not in ('returned','unsupported_call'):raise ValueError(result['stop'])
        result.update(sound=sound,offset=offset,speed=speed,period=period,limit=limit,mode=mode,
            stateBefore=fields(image),stateAfter=fields(bytes.fromhex(result['memory']['bullet']['after'])))
        return result
    def tick(self,time=0,period=60,limit=1,count=0,sound=-1,speed=3.,angle=.5,offset=0):
        image=bytearray(0x11b8);struct.pack_into('<3f',image,0x9d8,10,100,.1)
        struct.pack_into('<2f',image,0x9f0,speed,angle);struct.pack_into('<I',image,0xa20,16)
        struct.pack_into('<i',image,0xa38,sound)
        struct.pack_into('<iifII',image,0xe34,time-1,time,float(time),0x4b6f58,1)
        struct.pack_into('<2f',image,0xe48,speed,offset);struct.pack_into('<5i',image,0xe68,period,limit,count,2,0)
        result=emulate([self.chunk(0x40b200,0x40b3c5),self.chunk(0x46a660,0x46a6f9),
            self.chunk(0x40d110,0x40d130),*self.aim_chunks()],0x40b200,
            [datum('bullet',BULLET,bytes(image)),*self.data()],registers={'esi':BULLET},budget=4096)
        if result['stop']['reason'] not in ('returned','unsupported_call'):raise ValueError(result['stop'])
        result.update(time=time,period=period,limit=limit,count=count,sound=sound,speed=speed,angle=angle,offset=offset,
            stateBefore=fields(image),stateAfter=fields(bytes.fromhex(result['memory']['bullet']['after'])))
        return result

def probe(executable):
    native=NativeExtension16(executable)
    try:
        initialization=[native.initialize(offset=offset,speed=speed,period=period,limit=limit) for offset,speed,period,limit in
            ((0,-999,60,1),(.25,-999,30,2),(-.5,2.5,1,1),(0,0,60,1))]
        ticks=[native.tick(time=time,period=period,limit=limit,count=count,speed=speed,offset=offset)
            for time,period,limit,count,speed,offset in ((0,60,1,0,3,0),(1,60,1,0,3,0),(30,60,1,0,3,0),
                (59,60,1,0,3,0),(60,60,1,0,3,0),(60,60,2,0,3,.25),(60,60,2,1,3,-.5),(0,1,1,0,2.5,0),
                (1,1,1,0,2.5,0),(0,0,1,0,3,0))]
        sound=native.initialize(sound=40);update_sound=native.tick(time=60,sound=40)
        integrated=[native.integrated(half=half,active=active,time=time,cooldown=cooldown) for half,active,time,cooldown in
            ((False,False,0,0),(True,False,0,0),(False,True,30,0),(False,True,59,0),(False,True,60,0),(False,True,30,3))]
        return {'schema':'th128-original-type16-extension-prefixes/1','executable':{'sha256':EXE_SHA256,
            'sha256After':hashlib.sha256(native.executable.read_bytes()).hexdigest()},'initializationCases':initialization,
            'tickCases':ticks,'integratedCases':integrated,'initializationSound':sound,'updateSound':update_sound,'code':native.code,'constants':native.constants,
            'nativeCpuTrace':False,'windowsGameProcessLaunched':False,'generatorSha256':hashlib.sha256(Path(__file__).read_bytes()).hexdigest()}
    finally:native.image.close()

if __name__=='__main__':
    parser=argparse.ArgumentParser();parser.add_argument('--exe',required=True);arguments=parser.parse_args()
    report=probe(arguments.exe);output=ROOT/'local/research/bullet-extension16-fixtures.json'
    output.write_text(json.dumps(report,indent=2)+'\n',encoding='utf-8',newline='\n')
    print('original type16 executor initialization and mode2 update paths recorded')
