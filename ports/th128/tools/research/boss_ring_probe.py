import argparse
import hashlib
import json
from pathlib import Path
import struct
from bullet_probe import NativeBullet,ROOT,datum,emulate,CONTEXT,TASK,STATE,COMMAND

BULLET=0x390000
def word(data,offset):return struct.unpack_from('<I',data,offset)[0]
def fields(data):
    return {'active':word(data,0xa20),'timer':list(struct.unpack_from('<5I',data,0xd5c)),
        'velocityBits':list(struct.unpack_from('<3I',data,0x9e4)),'speedBits':word(data,0x9f0),'angleBits':word(data,0x9f4),'index':word(data,0xa3c)}

def probe(executable,prepare=False):
    native=NativeBullet(executable)
    try:
        handlers=[];geometry=[];steps=[];spawns=[]
        for slot,radius in ((0,0.),(1,24.),(15,-24.)):
            context=bytearray(0x4000);struct.pack_into('<I',context,0x3f7c,TASK);state=bytearray(0x1020);struct.pack_into('<I',state,4,COMMAND)
            command=struct.pack('<iHHHBBIif',0,524,24,0,255,2,0,slot,radius)
            spans=((0x41a9a0,0x41a9cf),(0x41e0c0,0x41e0d0),(0x46fed0,0x46ff66),(0x41e110,0x41e11e),(0x46ff70,0x470026),(0x41c52c,0x41c52e))
            result=emulate([native.chunk(*span) for span in spans],0x41a9a0,
                [datum('context',CONTEXT,bytes(context)),datum('task',TASK,struct.pack('<2I',0,STATE),writable=False),
                 datum('state',STATE,bytes(state)),datum('command',COMMAND,command,writable=False)],registers={'ebx':CONTEXT},stops=(0x41c52c,),budget=1024)
            if result['stop']['reason']!='boundary':raise ValueError(result['stop'])
            result.update(slot=slot,radius=radius,radiusBits=word(bytes.fromhex(result['memory']['context']['after']),0x4c4+slot*0x380));handlers.append(result)
        for angle in (0.,.5,4.,7.):
            for radius in (0.,24.,-24.):
                emitter=bytearray(0x380);struct.pack_into('<2i',emitter,0,4,6);struct.pack_into('<3f',emitter,8,10,144,0)
                struct.pack_into('<5f',emitter,0x14,angle,0,1.5,1.,radius);struct.pack_into('<2hH',emitter,0x364,1,1,3)
                manager=bytearray(0x64);struct.pack_into('<I',manager,0x10,BULLET)
                spans=((0x4091f0,0x40971d),(0x46a660,0x46a6f9),(0x40d110,0x40d130))
                result=emulate([native.chunk(*span) for span in spans],0x4091f0,
                    [datum('emitter',0x370000,bytes(emitter),writable=False),datum('manager',0x380000,bytes(manager)),datum('bullet',BULLET,bytes(0x11b8)),
                     native.constant(0x409a38,36),native.constant(0x4a6578,72),native.constant(0x4a68e8,8),native.constant(0x4a6aa0,4)],
                    arguments=struct.pack('<4If',0x380000,0x370000,0,0,0.),stops=(0x409717,),budget=2048)
                if result['stop']['reason']!='boundary':raise ValueError(result['stop'])
                image=bytes.fromhex(result['memory']['bullet']['after']);result.update(angle=angle,radius=radius,
                    positionBits=list(struct.unpack_from('<3I',image,0x9d8)),velocityBits=list(struct.unpack_from('<3I',image,0x9e4)))
                geometry.append(result)
        for color in (0,6,15):
            emitter=bytearray(0x380);struct.pack_into('<2i',emitter,0,4,color);struct.pack_into('<i',emitter,0x48,2);struct.pack_into('<i',emitter,0x38,1)
            image=bytearray(0x11b8);struct.pack_into('<I',image,0,0x13);struct.pack_into('<H',image,0xa2a,1);struct.pack_into('<h',image,0x11b0,4)
            struct.pack_into('<3f',image,0x9d8,-128,100,.1);struct.pack_into('<3f',image,0x9e4,.5,1.5,0)
            result=emulate([native.chunk(0x4097d0,0x4099df)],0x4097d0,
                [datum('emitter',0x370000,bytes(emitter),writable=False),datum('bullet',BULLET,bytes(image)),native.constant(0x4b2950,288),native.constant(0x409a5c,40),native.constant(0x4a6680,8)],
                registers={'ebx':BULLET,'edi':0x370000,'ebp':0x108100},stack={0x10c:struct.pack('<I',0x370000)},stops=(0x4099df,),budget=2048)
            if result['stop']['reason']!='boundary':raise ValueError(result['stop'])
            after=bytes.fromhex(result['memory']['bullet']['after']);result.update(color=color,flags=word(after,0),profileValue=word(after,0xa40),motionState=struct.unpack_from('<H',after,0xa2a)[0]);spawns.append(result)
        for time in (0,1,8,16,17):
            for angle in (0.,.5,4.):
                image=bytearray(0x11b8);struct.pack_into('<I',image,0xa20,1);struct.pack_into('<2f',image,0x9f0,1.5,angle)
                struct.pack_into('<3f',image,0x9e4,.5,1.5,0);struct.pack_into('<iifII',image,0xd5c,time-1,time,float(time),0x4b6f58,1)
                result=emulate([native.chunk(0x40ab10,0x40ab84),native.chunk(0x40d110,0x40d130),native.chunk(0x402ac0,0x402b18)],0x40ab10,
                    [datum('bullet',BULLET,bytes(image)),datum('clock',0x4b6f58,struct.pack('<f',1),writable=False),native.constant(0x4a6808,8),native.constant(0x4a6848,8),native.constant(0x4a6588,8),native.constant(0x4a6694,4),native.constant(0x4a6698,8)],
                    registers={'edx':BULLET},budget=1024)
                if result['stop']['reason']!='returned':raise ValueError(result['stop'])
                result.update(time=time,angle=angle,after=fields(bytes.fromhex(result['memory']['bullet']['after'])));steps.append(result)
        profile=native.read(0x4b2950,288);profile_report={'schema':'th128-original-bullet-profiles/1','sourceExeSha256':hashlib.sha256(native.binary).hexdigest(),
            'profiles':[{'type':4,'address':'0x004b2950','hex':profile.hex(),'sha256':hashlib.sha256(profile).hexdigest()}]}
        if prepare:
            local=ROOT/'local';catalog=json.loads((local/'catalog.json').read_text(encoding='utf8'))
            for variant in ('ja','zh'):
                destination=(local/catalog['variants'][variant]['manifest']).parent/'animations/bullet-native-profiles.json'
                if not destination.resolve().is_relative_to(local.resolve()) or destination.is_symlink():raise ValueError('unsafe prepared profile destination')
                destination.write_text(json.dumps(profile_report,indent=2)+'\n',encoding='utf8',newline='\n')
        return {'schema':'th128-original-boss-ring/1','handlers':handlers,'geometry':geometry,'spawns':spawns,'extension1Steps':steps,'profile':profile_report,
            'code':native.code,'constants':native.constants,'sourceExeSha256':hashlib.sha256(native.executable.read_bytes()).hexdigest(),
            'scope':'actual524 handler and nonzero birth-radius prefix, type4 post-animation constructor prefix, complete original ETEX1 unit step with real polar/timer helpers; no API returns assigned',
            'generatorSha256':hashlib.sha256(Path(__file__).read_bytes()).hexdigest(),'apiReturnsAssigned':False}
    finally:native.image.close()
if __name__=='__main__':
    parser=argparse.ArgumentParser();parser.add_argument('--exe',required=True);parser.add_argument('--prepare',action='store_true');args=parser.parse_args();report=probe(args.exe,args.prepare)
    (ROOT/'local/research/boss-ring-fixtures.json').write_text(json.dumps(report,indent=2)+'\n',encoding='utf8',newline='\n')
    print('original524,12 radial births,3 typed4 tail prefixes and15 complete ETEX1 steps captured')
