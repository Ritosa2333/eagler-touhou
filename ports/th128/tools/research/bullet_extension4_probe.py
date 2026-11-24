import argparse
import hashlib
import json
from pathlib import Path
import struct
from bullet_probe import ROOT,EXE_SHA256,datum,emulate
from bullet_extension16_probe import NativeExtension16,BULLET

def fields(image):
    return {'active':struct.unpack_from('<I',image,0xa20)[0],'index':struct.unpack_from('<i',image,0xa3c)[0],
        'speedBits':struct.unpack_from('<I',image,0x9f0)[0],'angleBits':struct.unpack_from('<I',image,0x9f4)[0],
        'velocityBits':list(struct.unpack_from('<3I',image,0x9e4)),
        'accelerationBits':list(struct.unpack_from('<2I',image,0xdb8)),
        'vectorBits':list(struct.unpack_from('<3I',image,0xdcc)),
        'timer':list(struct.unpack_from('<5I',image,0xda4)),'period':struct.unpack_from('<i',image,0xdd8)[0]}

class NativeExtension4(NativeExtension16):
    def integrated4(self,half=False,active=False,time=0,cooldown=0):
        image=bytearray.fromhex(self.initialize4()['memory']['bullet']['after'])
        if not active:struct.pack_into('<I',image,0xa20,0);struct.pack_into('<i',image,0xa3c,1)
        struct.pack_into('<Ii',image,0,0x11,cooldown);struct.pack_into('<i',image,0x438,1)
        struct.pack_into('<3f',image,0x9e4,.5,1.5,0);struct.pack_into('<i',image,0xa18,5)
        struct.pack_into('<H',image,0xa2a,2 if half else 1);struct.pack_into('<h',image,0x11b0,8)
        struct.pack_into('<ifi',image,0x119c,0,0.,0x4b6f58);struct.pack_into('<f',image,0x1180,1)
        if active:struct.pack_into('<iifII',image,0xda4,time-1,time,float(time),0x4b6f58,1)
        sprite=bytearray(0x48);struct.pack_into('<2f',sprite,0x34,16,16);struct.pack_into('<I',image,0x430,0x370000)
        spans=((0x408640,0x408d09),(0x409a90,0x409e01),(0x40a843,0x40a85e),(0x40ab90,0x40accd),
            (0x403280,0x4032bd),(0x402ac0,0x402b18),(0x4728fd,0x472907),(0x40d110,0x40d130),
            (0x490090,0x49014a),(0x491b05,0x491b1c),(0x491b78,0x491c2a))
        result=emulate([*[self.chunk(start,end) for start,end in spans],*self.aim_chunks()],0x408640,
            [datum('bullet',BULLET,bytes(image)),datum('sprite',0x370000,bytes(sprite),writable=False),
             datum('cookie',0x4b01f4,0,writable=False),self.constant(0x408d38,20),self.constant(0x40a860,108),
             self.constant(0x4a6600,16),self.constant(0x4a6658,8),self.constant(0x4a6818,8),*self.data()],
            registers={'ebx':BULLET},stops=[0x408d09],budget=8192)
        if result['stop']['reason']!='boundary':raise ValueError(result['stop'])
        after=bytes.fromhex(result['memory']['bullet']['after'])
        result.update(half=half,initialActive=active,initialTime=time,cooldown=cooldown,stateAfter=fields(after),
            positionBits=list(struct.unpack_from('<3I',after,0x9d8)),lifeClockAfter=struct.unpack_from('<i',after,0x119c)[0],
            cooldownAfter=struct.unpack_from('<i',after,4)[0],graceAfter=struct.unpack_from('<i',after,0xa18)[0])
        return result
    def initialize4(self,acceleration=.0166666675,direction=-999,period=60,sound=-1):
        image=bytearray(0x11b8);struct.pack_into('<3f',image,0x9d8,10,100,.1)
        struct.pack_into('<2f',image,0x9f0,1.,.5);struct.pack_into('<i',image,0xa38,sound);struct.pack_into('<i',image,0xa3c,1)
        struct.pack_into('<4f4i3I',image,0xa44+44,acceleration,direction,0,0,period,-999999,0,0,4,0,0)
        result=emulate([self.chunk(0x409a90,0x409e01),self.chunk(0x40a843,0x40a85e),self.chunk(0x403280,0x4032bd),
            self.chunk(0x40d110,0x40d130),self.chunk(0x4728fd,0x472907),*self.aim_chunks()],0x409a90,
            [datum('bullet',BULLET,bytes(image)),datum('cookie',0x4b01f4,0,writable=False),self.constant(0x40a860,108),*self.data()],
            registers={'ecx':BULLET},budget=4096)
        if result['stop']['reason']!='returned' and result['stop'].get('target')!='0x00458410':raise ValueError(result['stop'])
        result.update(acceleration=acceleration,direction=direction,period=period,sound=sound,stateAfter=fields(bytes.fromhex(result['memory']['bullet']['after'])))
        return result
    def tick4(self,time=0,period=60,acceleration=.0166666675,direction=.5,velocity=(.5,1.5,0),speed=1.):
        image=bytearray.fromhex(self.initialize4(acceleration,direction,period)['memory']['bullet']['after'])
        struct.pack_into('<3f',image,0x9e4,*velocity);struct.pack_into('<2f',image,0x9f0,speed,.5)
        struct.pack_into('<iifII',image,0xda4,time-1,time,float(time),0x4b6f58,1)
        result=emulate([self.chunk(0x40ab90,0x40accd),self.chunk(0x402ac0,0x402b18),
            self.chunk(0x490090,0x49014a),self.chunk(0x491b05,0x491b1c),self.chunk(0x491b78,0x491c2a),*self.aim_chunks()[1:]],0x40ab90,
            [datum('bullet',BULLET,bytes(image)),self.constant(0x4a6818,8),*self.data()],registers={'eax':BULLET},budget=4096)
        if result['stop']['reason']!='returned':raise ValueError(result['stop'])
        result.update(time=time,period=period,acceleration=acceleration,direction=direction,velocity=velocity,speed=speed,
            helperReturn=result['registers']['eax'],stateBefore=fields(image),stateAfter=fields(bytes.fromhex(result['memory']['bullet']['after'])))
        return result

def probe(executable):
    native=NativeExtension4(executable)
    try:
        initial=[native.initialize4(acceleration,direction,period) for acceleration,direction,period in
            ((.0166666675,-999,60),(.0333333351,-999,60),(.125,.25,1),(-.25,.5,60),(.1,1000,30),(0,-999,0))]
        ticks=[native.tick4(time=time,period=period,acceleration=accel,direction=direction,velocity=velocity) for time,period,accel,direction,velocity in
            ((0,60,.0166666675,.5,(.5,1.5,0)),(59,60,.0166666675,.5,(.5,1.5,0)),(60,60,.0166666675,.5,(.5,1.5,0)),
             (0,1,.125,.25,(0,1,0)),(0,0,.125,.25,(0,1,0)),(0,60,0,0,(0,0,0)),(0,60,-.25,.5,(.5,1.5,.125)))]
        integrated=[native.integrated4(half=half,active=active,time=time,cooldown=cooldown) for half,active,time,cooldown in
            ((False,False,0,0),(True,False,0,0),(False,True,30,0),(False,True,59,0),(False,True,60,0),(False,True,30,3))]
        return {'schema':'th128-original-extension4/1','executable':{'sha256':EXE_SHA256,
            'sha256After':hashlib.sha256(native.executable.read_bytes()).hexdigest()},'initializationCases':initial,'tickCases':ticks,
            'integratedCases':integrated,'initializationSound':native.initialize4(sound=40),'code':native.code,'constants':native.constants,'nativeCpuTrace':False,'windowsGameProcessLaunched':False,
            'generatorSha256':hashlib.sha256(Path(__file__).read_bytes()).hexdigest()}
    finally:native.image.close()

if __name__=='__main__':
    parser=argparse.ArgumentParser();parser.add_argument('--exe',required=True);arguments=parser.parse_args()
    report=probe(arguments.exe);output=ROOT/'local/research/bullet-extension4-fixtures.json'
    output.write_text(json.dumps(report,indent=2)+'\n',encoding='utf-8',newline='\n')
    print('original type4 init and actual atan2/sqrt updater paths recorded')
