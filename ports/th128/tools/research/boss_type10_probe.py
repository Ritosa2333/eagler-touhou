import argparse
import hashlib
import json
from pathlib import Path
import struct
from bullet_probe import NativeBullet,ROOT,datum,emulate,STACK_POINTER
from bullet_lifecycle_probe import bullet_state


def probe(executable,prepare=False):
    native=NativeBullet(executable)
    try:
        cases=[];profile=native.read(0x4b24d0+10*0x120,288)
        for color in (0,4,8,15):
            emitter=bytearray(0x380);struct.pack_into('<2i',emitter,0,10,color);struct.pack_into('<i',emitter,0x38,1);struct.pack_into('<i',emitter,0x48,2)
            image=bytearray(0x11b8);struct.pack_into('<I',image,0,0x13);struct.pack_into('<H',image,0xa2a,1);struct.pack_into('<h',image,0x11b0,10)
            struct.pack_into('<3f',image,0x9d8,-128,100,.1);struct.pack_into('<3f',image,0x9e4,.5,1.5,0)
            result=emulate([native.chunk(0x4097d0,0x4099df)],0x4097d0,
                [datum('emitter',0x370000,bytes(emitter),writable=False),datum('bullet',0x390000,bytes(image)),native.constant(0x4b24d0+10*0x120,288),native.constant(0x409a5c,40),native.constant(0x4a6680,8)],
                registers={'ebx':0x390000,'edi':0x370000,'ebp':STACK_POINTER+0x100},stack={0x10c:struct.pack('<I',0x370000)},stops=(0x4099df,),budget=2048)
            if result['stop']['reason']!='boundary':raise ValueError(result['stop'])
            final=bytes.fromhex(result['memory']['bullet']['after']);result.update(color=color,state=bullet_state(final),cancellation=struct.unpack_from('<i',final,0xa1c)[0]);cases.append(result)
        metadata={'schema':'th128-original-boss-bullet-profiles/1','sourceExeSha256':hashlib.sha256(native.executable.read_bytes()).hexdigest(),
            'profiles':[{'type':10,'hex':profile.hex(),'sha256':hashlib.sha256(profile).hexdigest()}]}
        if prepare:
            local=ROOT/'local';catalog=json.loads((local/'catalog.json').read_text(encoding='utf8'))
            for record in catalog['variants'].values():
                destination=(local/record['manifest']).parent/'animations/bullet-boss-profiles.json'
                destination.write_text(json.dumps(metadata,indent=2)+'\n',encoding='utf8')
        return {'schema':'th128-original-boss-type10/1','cases':cases,'profile':metadata['profiles'][0],'code':native.code,'constants':native.constants,
            'apiReturnsAssigned':False,'sourceExeSha256':metadata['sourceExeSha256'],'generatorSha256':hashlib.sha256(Path(__file__).read_bytes()).hexdigest()}
    finally:native.image.close()


if __name__=='__main__':
    parser=argparse.ArgumentParser();parser.add_argument('--exe',required=True);parser.add_argument('--prepare',action='store_true');args=parser.parse_args();report=probe(args.exe,args.prepare)
    (ROOT/'local/research/boss-type10-fixtures.json').write_text(json.dumps(report,indent=2)+'\n',encoding='utf8')
    print('original type10 profile and four real constructor tails recorded')
