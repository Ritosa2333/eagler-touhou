import argparse
import hashlib
import json
from pathlib import Path
import struct
from bullet_probe import NativeBullet,ROOT,EXE_SHA256,datum
from freeze_probe import emulate,STACK_POINTER

PLAYER,CONTROL,LASER=0x300000,0x350000,0x390000
BULLET=0x3b0000

class NativeRespawn(NativeBullet):
    def cancel_record(self,motion):
        image=bytearray(0x11b8)
        struct.pack_into('<H',image,0xa2a,motion)
        struct.pack_into('<i',image,0xa1c,-1)
        struct.pack_into('<iifII',image,0x1184,122,123,123.,0x4b6f58,1)
        struct.pack_into('<i',image,0x119c,91)
        result=emulate([self.chunk(0x40ca10,0x40cb84)],0x40ca10,
            [datum('bullet',BULLET,bytes(image))],registers={'esi':BULLET+0x4d0,'ebx':0,'ebp':STACK_POINTER+0x100},
            stack={0xfc:struct.pack('<i',1)},stops=[0x40cb84],budget=256)
        if result['stop']['reason']!='boundary':raise ValueError(result['stop'])
        final=bytes.fromhex(result['memory']['bullet']['after'])
        result.update(motionBefore=motion,motionAfter=struct.unpack_from('<H',final,0xa2a)[0],
                      ageAfter=struct.unpack_from('<i',final,0x1188)[0],lifeClockAfter=struct.unpack_from('<i',final,0x119c)[0])
        return result

    def control(self,selector,value):
        image=bytes(0x2750+8*0x4e8)
        result=emulate([self.chunk(0x427190,0x4271b5),self.chunk(0x41fd50,0x41fe20)],0x427190,
            [datum('selector',0x4b4d04,selector),datum('controlPointer',0x4b8950,CONTROL,kind='I',writable=False),
             datum('value',0x4b4d08,value,writable=False),
             datum('control',CONTROL,image)],registers={'eax':0x4b4cc0},budget=1024)
        if result['stop']['reason']!='returned':raise ValueError(result['stop'])
        final=bytes.fromhex(result['memory']['control']['after'])
        result.update(selector=selector,value=value,pendingInterrupts=[struct.unpack_from('<H',final,0x2750+index*0x4e8+0x3f8)[0] for index in range(8)])
        result['memory'].pop('control');return result

    def entry(self,elapsed):
        data=[datum('rawVertical',PLAYER+0xedc,61440),datum('pixelVertical',PLAYER+0xed0,struct.pack('<f',480)),
              datum('stateTimer',PLAYER+0xf80,struct.pack('<iifII',elapsed-1,elapsed,float(elapsed),0x4b6f58,1)),
              datum('movementDeltas',PLAYER+0xf70,struct.pack('<2i',576,-256)),datum('dirtyFields',PLAYER+0x8888,bytes(0x640)),
              datum('rapidShotTimer',PLAYER+0x438fc,struct.pack('<iifII',5,6,6.,0x4b6f58,1)),
              datum('laserPointer',0x4b8a60,LASER,kind='I',writable=False),datum('laser',LASER,bytes(0x5e8)),
              datum('savedPosition',PLAYER+0xf58,struct.pack('<3f',12,400,.1),writable=False),
              self.constant(0x4a65f0,8),self.constant(0x4a6c18,8),self.constant(0x4a6990,8),
              self.constant(0x4a6a90,8),self.constant(0x4a6b08,8)]
        result=emulate([self.chunk(0x43b6a2,0x43b76d),self.chunk(0x43a340,0x43a376),
                        self.chunk(0x42a7c0,0x42a820)],0x43b6a2,data,
            registers={'esi':PLAYER,'ebp':STACK_POINTER+0x100},
            stack={0x108:struct.pack('<I',PLAYER)},stops=[0x43b703,0x43b9ee],budget=1024)
        if result['stop']['reason']!='boundary':raise ValueError(result['stop'])
        result.update(elapsed=elapsed,rawVertical=result['memory']['rawVertical']['afterI32'],
                      pixelVertical=result['memory']['pixelVertical']['afterF32'],
                      radiusBits=struct.unpack_from('<I',bytes.fromhex(result['stackHead']),0)[0] if elapsed<30 else None)
        return result

    def entry_radius(self,elapsed):
        result=emulate([self.chunk(0x43b716,0x43b73e)],0x43b716,
            [self.constant(0x4a6c18,8),self.constant(0x4a6990,8),self.constant(0x4a6a90,8)],
            registers={'esi':PLAYER,'edi':0,'ebp':STACK_POINTER+0x100},
            stack={0xfc:struct.pack('<i',elapsed)},stops=[0x43b73e],budget=64)
        if result['stop']['reason']!='boundary':raise ValueError(result['stop'])
        result.update(elapsed=elapsed,radiusBits=struct.unpack_from('<I',bytes.fromhex(result['stackHead']))[0])
        return result

def probe(executable):
    native=NativeRespawn(executable)
    try:
        entries=[native.entry(elapsed) for elapsed in (0,1,2,15,29,30,59,60)]
        controls=[native.control(selector,value) for selector,value in ((0,0),(1,3),(2,0),(7,7))]
        radii=[native.entry_radius(elapsed) for elapsed in (0,1,2,15,29)]
        cancellations=[native.cancel_record(motion) for motion in (0,1,2,3,4)]
        return {'schema':'th128-original-respawn-entry/1','executable':{'sha256':EXE_SHA256,
            'sha256After':hashlib.sha256(native.executable.read_bytes()).hexdigest()},
            'windowsGameProcessLaunched':False,'nativeCpuTrace':False,'entryCases':entries,'controlCases':controls,
            'radiusCases':radii,'cancelCases':cancellations,'code':native.code,'constants':native.constants,
            'generatorSha256':hashlib.sha256(Path(__file__).read_bytes()).hexdigest()}
    finally:native.image.close()

if __name__=='__main__':
    parser=argparse.ArgumentParser();parser.add_argument('--exe',required=True);arguments=parser.parse_args()
    report=probe(arguments.exe);output=ROOT/'local/research/player-respawn-fixtures.json'
    output.write_text(json.dumps(report,indent=2)+'\n',encoding='utf-8',newline='\n')
    print('original respawn entry motion and explicit empty-control/laser paths recorded')
