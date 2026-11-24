import argparse
import hashlib
import json
from pathlib import Path
import struct
from bullet_probe import NativeBullet,ROOT,datum,emulate

def probe(executable):
    native=NativeBullet(executable)
    try:
        spell=0x380000;cases=[];starts=[];bonuses=[]
        for mode in (0,1):
            image=bytearray(0xc0);struct.pack_into('<I',image,0x7c,0xf0)
            result=emulate([native.chunk(0x40dd70,0x40de49)],0x40dd70,
                [datum('spell',spell,bytes(image)),datum('spellGlobal',0x4b8934,spell,kind='I',writable=False),datum('title',0x390000,b'Native card\0',writable=False),datum('system',0x3a0000+0x3c,mode,writable=False),datum('systemGlobal',0x4b892c,0x3a0000,kind='I',writable=False),datum('fontGlobal',0x4b8920,0,kind='I',writable=False),datum('replay',0x3b0000+0x10,1,writable=False),datum('replayGlobal',0x4b8a84,0x3b0000,kind='I',writable=False)],arguments=struct.pack('<3I',1,0x390000,2280),stops=(0x40de49,),budget=512)
            if result['stop']['reason']!='boundary':raise ValueError(result['stop'])
            after=bytes.fromhex(result['memory']['spell']['after']);result.update(mode=mode,flags=struct.unpack_from('<I',after,0x7c)[0],age=struct.unpack_from('<i',after,0x28)[0],id=struct.unpack_from('<i',after,0x78)[0]);starts.append(result)
        for difficulty in range(4):
            result=emulate([native.chunk(0x40e125,0x40e13d),native.chunk(0x40e15a,0x40e199)],0x40e125,
                [datum('spell',spell,bytes(0xc0)),datum('difficulty',0x4b4d0c,difficulty,writable=False),datum('stage',0x4b4d14,1,writable=False),native.constant(0x40e300,0x38)],registers={'edi':spell},stops=(0x40e199,),budget=64)
            if result['stop']['reason']!='boundary':raise ValueError(result['stop'])
            after=bytes.fromhex(result['memory']['spell']['after']);result.update(difficulty=difficulty,bonus=struct.unpack_from('<i',after,0x80)[0]);bonuses.append(result)
        for age in (0,299,300,301,1000,2279):
            for flags in (7,0x17):
                image=bytearray(0xc0);struct.pack_into('<iifII',image,0x24,age-1,age,float(age),0x4b6f58,1);struct.pack_into('<I',image,0x7c,flags);struct.pack_into('<2i',image,0x84,200000,2280)
                result=emulate([native.chunk(0x40d87a,0x40d8e3),native.chunk(0x402ac0,0x402b18)],0x40d87a,
                    [datum('spell',spell,bytes(image)),datum('clock',0x4b6f58,struct.pack('<f',1),writable=False),native.constant(0x4a6694,12),native.constant(0x4a6588,8)],registers={'edi':spell},stops=(0x40d8e3,),budget=256)
                if result['stop']['reason']!='boundary':raise ValueError(result['stop'])
                after=bytes.fromhex(result['memory']['spell']['after']);result.update(input=[age,flags],ageAfter=struct.unpack_from('<i',after,0x28)[0],bonusAfter=struct.unpack_from('<i',after,0x80)[0]);cases.append(result)
        return {'schema':'th128-original-spell-rules/1','starts':starts,'bonuses':bonuses,'ticks':cases,'code':native.code,'constants':native.constants,'sourceExeSha256':hashlib.sha256(native.executable.read_bytes()).hexdigest(),'generatorSha256':hashlib.sha256(Path(__file__).read_bytes()).hexdigest(),'apiReturnsAssigned':False,'scope':'Original start scalar prefix before allocations, first-stage bonus lookup, complete bonus-decay/timer prefix. No graphics/sound call returns invented.'}
    finally:native.image.close()
if __name__=='__main__':
    parser=argparse.ArgumentParser();parser.add_argument('--exe',required=True);report=probe(parser.parse_args().exe)
    (ROOT/'local/research/spell-rules-fixtures.json').write_text(json.dumps(report,indent=2)+'\n',encoding='utf8');print('original spell start flags, four bonus bases and12 decay/clock cases recorded')
