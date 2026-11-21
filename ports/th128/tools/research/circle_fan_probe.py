import argparse
import hashlib
import json
from pathlib import Path
import struct
from bullet_probe import NativeBullet,ROOT,datum,emulate
def probe(executable):
    native=NativeBullet(executable)
    try:
        cases=[];modes=[];manager=0x300000;buffer=0x3a0000
        for horizontal,vertical,radius,angle,count in ((320.,160.,256.,0.,36),(128.,16.,64.,.5,12),(-10.,400.,-32.,-1.,8),(0.,0.,0.,0.,4)):
            size=(count+2)*20
            result=emulate([native.chunk(0x462a00,0x462aa9),native.chunk(0x4630a0,0x4630c0),native.chunk(0x46a660,0x46a6f9)],0x462a00,
                [datum('managerBuffer',manager+0x8b96fc,buffer,kind='I',writable=False),datum('vertices',buffer,bytes(size)),native.constant(0x4a6578,72)],
                registers={'edi':manager,'eax':0x80ff00ff},arguments=struct.pack('<4fiI',horizontal,vertical,radius,angle,count,0x01ff50ff),stops=(0x462a9e,),budget=10000)
            if result['stop']['reason']!='boundary':raise ValueError(result['stop'])
            after=bytes.fromhex(result['memory']['vertices']['after'])
            result.update(input=[horizontal,vertical,radius,angle,count],vertices=[list(struct.unpack_from('<5I',after,index*20)) for index in range(count+2)])
            cases.append(result)
        for opcode,start,end in ((104,0x45ba44,0x45ba78),(105,0x45ba78,0x45bad6)):
            result=emulate(([native.chunk(start,end),native.chunk(0x45baac,0x45bad6)] if opcode==104 else [native.chunk(start,end)])+[native.chunk(0x459e1a,0x459e31)],start,
                [datum('vm',0x380000,bytes(0x4e8)),datum('command',0x390000,struct.pack('<HHhHfi',opcode,16,0,0,256.,36),writable=False)],
                registers={'edi':0x380000,'esi':0x390000},stops=(0x459e1a,),budget=128)
            if result['stop']['reason']!='boundary':raise ValueError(result['stop'])
            image=bytes.fromhex(result['memory']['vm']['after']);result.update(opcode=opcode,mode=struct.unpack_from('<I',image,0x4b0)[0]>>24&31,
                radius=struct.unpack_from('<f',image,0x60)[0],count=struct.unpack_from('<i',image,0x430)[0]);modes.append(result)
        negative=emulate([native.chunk(0x45a5d7,0x45a5fd)],0x45a5d7,
            [datum('fontGlobal',0x4b8920,0x300000,kind='I',writable=False),datum('fontAnm',0x300000+0x1c894,0x710000,kind='I',writable=False)],
            registers={'eax':0xffffffff,'edi':0x380000},stops=(0x45a5f8,),budget=64)
        if negative['stop']['reason']!='boundary':raise ValueError(negative['stop'])
        negative['sprite']=negative['registers']['ecx']
        return {'schema':'th128-original-untextured-circle-fan/1','cases':cases,'modeCases':modes,'negativeSprite':negative,'code':native.code,'constants':native.constants,
            'sourceExeSha256':hashlib.sha256(native.executable.read_bytes()).hexdigest(),'apiReturnsAssigned':False,
            'scope':'original462a00 CPU vertex prefix through real4630a0 polar and46a660 angle helpers; stop before graphics API. Original104/105 mode-bit stores.',
            'generatorSha256':hashlib.sha256(Path(__file__).read_bytes()).hexdigest()}
    finally:native.image.close()
if __name__=='__main__':
    parser=argparse.ArgumentParser();parser.add_argument('--exe',required=True);args=parser.parse_args();report=probe(args.exe)
    (ROOT/'local/research/circle-fan-fixtures.json').write_text(json.dumps(report,indent=2)+'\n',encoding='utf8',newline='\n');print('4 original CPU fan buffers and2 original procedural mode stores recorded')
