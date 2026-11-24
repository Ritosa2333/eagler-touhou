import argparse
import hashlib
import json
from pathlib import Path
import struct
from bullet_probe import NativeBullet,ROOT,EXE_SHA256,datum,emulate,STACK_POINTER
from bullet_lifecycle_probe import bullet_state
from bullet_tick_probe import frame_input,semantic,ANIMATION_BOUNDARY,BULLET,SPRITE

EMITTER=0x370000

class NativeType18(NativeBullet):
    def spawn_typed(self,selectedType,color):
        if selectedType not in (18,20,23,29) or not 0<=color<=15:raise ValueError('explicit supported type/color required')
        emitter=bytearray(0x380);struct.pack_into('<2i',emitter,0,selectedType,color);struct.pack_into('<i',emitter,0x48,2);struct.pack_into('<i',emitter,0x374,40)
        struct.pack_into('<i',emitter,0x38,1)
        image=bytearray(0x11b8);struct.pack_into('<I',image,0,0x13);struct.pack_into('<H',image,0xa2a,1);struct.pack_into('<h',image,0x11b0,selectedType)
        struct.pack_into('<3f',image,0x9d8,-128,100,.1);struct.pack_into('<3f',image,0x9e4,.5,1.5,0)
        result=emulate([self.chunk(0x4097d0,0x4099df)],0x4097d0,
            [datum('emitter',EMITTER,bytes(emitter),writable=False),datum('bullet',BULLET,bytes(image)),
             self.constant(0x4b24d0+selectedType*0x120,0x120),self.constant(0x409a5c,40),self.constant(0x4a6680,8)],
            registers={'ebx':BULLET,'edi':EMITTER,'ebp':STACK_POINTER+0x100},stack={0x10c:struct.pack('<I',EMITTER)},stops=[0x4099df],budget=2048)
        if result['stop']['reason']!='boundary':raise ValueError(result['stop'])
        after=bytes.fromhex(result['memory']['bullet']['after']);result.update(state=bullet_state(after),selectedType=selectedType,color=color,extension=2,flagsBefore=0x13,
            input={'type':selectedType,'color':color},postAnimationRequired=True,completedConstructor=False)
        result['state']['cancellationScript']=struct.unpack_from('<i',after,0xa1c)[0];result['state']['profileValue']=struct.unpack_from('<i',after,0xa40)[0]
        return result
    def spawn(self,color=3,extension=2,flags=0x13):
        emitter=bytearray(0x380);struct.pack_into('<2i',emitter,0,18,color)
        struct.pack_into('<i',emitter,0x38,1);struct.pack_into('<i',emitter,0x48,extension)
        struct.pack_into('<I',emitter,0x36c,35);struct.pack_into('<i',emitter,0x374,40)
        image=bytearray(0x11b8);struct.pack_into('<I',image,0,flags)
        struct.pack_into('<H',image,0xa2a,1);struct.pack_into('<h',image,0x11b0,18)
        struct.pack_into('<3f',image,0x9d8,-128,100,.1);struct.pack_into('<3f',image,0x9e4,.5,1.5,0)
        result=emulate([self.chunk(0x4097d0,0x4099df)],0x4097d0,
            [datum('emitter',EMITTER,bytes(emitter),writable=False),datum('bullet',BULLET,bytes(image)),
             self.constant(0x4b24d0+18*0x120,0x120),self.constant(0x409a5c,40),self.constant(0x4a6680,8)],
            registers={'ebx':BULLET,'edi':EMITTER,'ebp':STACK_POINTER+0x100},
            stack={0x10c:struct.pack('<I',EMITTER)},stops=[0x4099df],budget=2048)
        if result['stop']['reason']!='boundary':raise ValueError(result['stop'])
        after=bytes.fromhex(result['memory']['bullet']['after'])
        result.update(color=color,extension=extension,flagsBefore=flags,state=bullet_state(after),
                      postAnimationRequired=True,completedConstructor=False)
        result['state']['cancellationScript']=struct.unpack_from('<i',after,0xa1c)[0]
        return result

    def tick(self,selected_type=18,**changes):
        if selected_type not in (18,20,23,29):raise ValueError('explicit supported native bullet type required')
        values=frame_input(**changes);values['type']=selected_type
        image=bytearray(0x11b8)
        struct.pack_into('<Ii',image,0,values['flags'],values['cooldown'])
        struct.pack_into('<I',image,0x430,SPRITE if values['hasSprite'] else 0)
        struct.pack_into('<i',image,0x438,values['integer0'])
        struct.pack_into('<3f',image,0x9d8,*values['position']);struct.pack_into('<3f',image,0x9e4,*values['velocity'])
        hitbox=8.5 if selected_type==18 else 2.4 if selected_type==29 else 7.
        struct.pack_into('<2f',image,0x9f8,hitbox,hitbox);struct.pack_into('<i',image,0xa18,values['grace'])
        struct.pack_into('<I',image,0xa20,values['activeExtensions']);struct.pack_into('<H',image,0xa2a,values['motionState'])
        struct.pack_into('<i',image,0xa3c,1);struct.pack_into('<f',image,0x1180,1)
        struct.pack_into('<i',image,0x1188,values['age'])
        struct.pack_into('<ifi',image,0x119c,values['lifeClock'],values['lifeTime'],0x4b6f58)
        struct.pack_into('<h',image,0x11b0,selected_type)
        sprite=bytearray(0x48);struct.pack_into('<2f',sprite,0x34,values['spriteHeight'],values['spriteWidth'])
        spans=((0x408640,ANIMATION_BOUNDARY),(0x408560,0x4085ca),(0x407fb0,0x408055),
               (0x409a90,0x409aef),(0x40a843,0x40a85e),(0x4728fd,0x472907))
        constants=((0x408d38,20),(0x4a6588,8),(0x4a65a8,8),(0x4a6694,12),(0x4a6600,8),
                   (0x4a6608,8),(0x4a65d8,8),(0x4a6658,8),(0x4b01f4,4),(0x4a6718,8),(0x4a6ad0,8))
        result=emulate([self.chunk(start,end) for start,end in spans],0x408640,
            [datum('bullet',BULLET,bytes(image)),datum('sprite',SPRITE,bytes(sprite),writable=False),
             datum('clock',0x4b6f58,struct.pack('<f',1),writable=False),
             *[self.constant(start,size) for start,size in constants]],registers={'ebx':BULLET},stops=[ANIMATION_BOUNDARY],budget=2048)
        stop=result['stop']
        if stop['reason']=='boundary':continuation='NeedsAnimation';boundary=ANIMATION_BOUNDARY
        elif stop['reason']=='returned':continuation='Removed';boundary=0x4086c6
        elif stop.get('target') in ('0x0043c410','0x0043c570'):continuation='NeedsCollision';boundary=int(stop['address'],16)
        else:raise ValueError(stop)
        after=bytes.fromhex(result['memory']['bullet']['after'])
        result.update(input=values,next=semantic(after,values),nativeTypeBefore=struct.unpack_from('<h',image,0x11b0)[0],
                      previousLifeClock=struct.unpack_from('<i',after,0x1198)[0],
                      continuation=continuation,boundary=boundary,completedTick=False)
        result['memory'].pop('bullet');return result

def probe(executable):
    native=NativeType18(executable)
    try:
        spawns=[native.spawn(color,extension,flags) for color,extension,flags in ((3,2,0x13),(0,0,3),(15,2,0x23))]
        ticks=[native.tick(**changes) for changes in ({},{'age':7},{'age':8},{'motionState':1},
            {'motionState':3},{'integer0':1},{'age':8,'flags':0x11},{'position':(400,400,.1),'grace':0},
            {'flags':0x1b},{'velocity':(-1.25,.875,0),'motionState':1,'flags':0x11})]
        return {'schema':'th128-original-type18-bullet-prefixes/1','executable':{'sha256':EXE_SHA256,
            'sha256After':hashlib.sha256(native.executable.read_bytes()).hexdigest()},
            'spawnCases':spawns,'tickCases':ticks,'code':native.code,'constants':native.constants,
            'completedConstructor':False,'windowsGameProcessLaunched':False,'nativeCpuTrace':False,
            'generatorSha256':hashlib.sha256(Path(__file__).read_bytes()).hexdigest()}
    finally:native.image.close()

def probe_type20(executable):
    native=NativeType18(executable)
    try:
        spawnCases=[native.spawn_typed(20,6),native.spawn_typed(20,0)]
        ticks=[native.tick(selected_type=20,**changes) for changes in ({},{'age':7},{'age':8},{'motionState':1})]
        profile=native.read(0x4b24d0+20*0x120,288)
        return {'schema':'th128-original-type20-bullet-prefixes/1','executable':{'sha256':EXE_SHA256,
            'sha256After':hashlib.sha256(native.executable.read_bytes()).hexdigest()},'spawnCases':spawnCases,'tickCases':ticks,
            'profile':{'bytes':profile.hex(),'sha256':hashlib.sha256(profile).hexdigest(),'type':20,
                       'script':struct.unpack_from('<i',profile)[0],'hitboxSize':struct.unpack_from('<f',profile,0x104)[0]},
            'code':native.code,'constants':native.constants,'nativeCpuTrace':False,'windowsGameProcessLaunched':False,
            'generatorSha256':hashlib.sha256(Path(__file__).read_bytes()).hexdigest()}
    finally:native.image.close()

def prepare_profile(executable,selected_type=18):
    native=NativeType18(executable)
    try:
        if selected_type not in (18,20):raise ValueError('supported explicit typed profile required')
        directory=ROOT/'local/runtime';destination=directory/f'bullet-profile-{selected_type}.bin'
        if directory.resolve()!=directory or destination.is_symlink() or directory.drive.lower()=='e:':
            raise ValueError('type18 profile output must stay in owned local/runtime')
        source=0x4b24d0+selected_type*288;payload=native.read(source,288)
        if struct.unpack_from('<i',payload)[0]!=(72 if selected_type==18 else 74):raise ValueError('unexpected typed profile identity')
        directory.mkdir(parents=True,exist_ok=True);destination.write_bytes(payload)
        report={'schema':'th128-original-bullet-profile/1','type':selected_type,'bytes':288,'sourceAddress':f'0x{source:08x}',
                'sha256':hashlib.sha256(payload).hexdigest(),'exeSha256':EXE_SHA256,'rawOriginalTable':True}
        (directory/f'bullet-profile-{selected_type}.json').write_text(json.dumps(report,indent=2)+'\n',encoding='utf-8',newline='\n')
        return report
    finally:native.image.close()

if __name__=='__main__':
    parser=argparse.ArgumentParser();parser.add_argument('--exe',required=True);parser.add_argument('--prepare-profile',action='store_true');parser.add_argument('--type20',action='store_true');arguments=parser.parse_args()
    if arguments.type20:
        report=probe_type20(arguments.exe);output=ROOT/'local/research/type20-bullet-fixtures.json'
        output.write_text(json.dumps(report,indent=2)+'\n',encoding='utf-8',newline='\n')
        if arguments.prepare_profile:prepare_profile(arguments.exe,20)
        print('original typed20 spawn/update prefixes recorded');raise SystemExit(0)
    report=probe(arguments.exe);output=ROOT/'local/research/type18-bullet-fixtures.json'
    output.write_text(json.dumps(report,indent=2)+'\n',encoding='utf-8',newline='\n')
    if arguments.prepare_profile:prepare_profile(arguments.exe)
    print('original type18 post-animation spawn and common update prefixes recorded')
