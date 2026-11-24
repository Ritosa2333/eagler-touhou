import argparse
import hashlib
import json
from pathlib import Path
import struct
from bullet_probe import NativeBullet,ROOT,EXE_SHA256,CONTEXT,TASK,STATE,COMMAND,HANDLER_STOP,datum,emulate

def probe(executable):
    native=NativeBullet(executable)
    try:
        index=native.read(0x41c7c0+427-256,1)[0]
        if struct.unpack('<I',native.read(0x41c554+4*index,4))[0]!=0x41a238:raise ValueError('unexpected427 dispatch')
        chunks=[native.chunk(0x41a238,0x41a286),native.chunk(0x41e0c0,0x41e0d0),native.chunk(0x41e110,0x41e11e),
            native.chunk(0x46fed0,0x46ff66),native.chunk(0x46ff70,0x470026),native.chunk(0x414db0,0x414ddc),
            native.chunk(HANDLER_STOP,HANDLER_STOP+2)]
        cases=[]
        for maximum,boss in ((10900,0),(13400,1),(1,7)):
            for slot,life,color in ((0,6300.0,0xffff8080),(1,2000.0,0xff800000),(0,0.0,0),(1,-1.5,0xffffffff),(2,3100.,0xffffffff),(3,1000.,0xffff0000)):
                context=bytearray(0x4000);struct.pack_into('<I',context,0x3f7c,TASK)
                struct.pack_into('<i',context,0x3e40,maximum);struct.pack_into('<i',context,0x3ef4,boss)
                state=bytearray(0x1020);struct.pack_into('<I',state,4,COMMAND)
                command=struct.pack('<iHHHBBIifI',0,427,28,0,255,3,0,slot,life,color)
                hud=bytearray([0xa5])*0x5c00
                result=emulate(chunks,0x41a238,[datum('context',CONTEXT,bytes(context)),datum('state',STATE,bytes(state)),
                    datum('task',TASK,struct.pack('<2I',0,STATE),writable=False),datum('command',COMMAND,command,writable=False),
                    datum('hudGlobal',0x4b8950,0x380000,'I',False),datum('hud',0x380000,bytes(hud))],
                    registers={'ebx':CONTEXT},stops=(HANDLER_STOP,),budget=256)
                if result['stop']!={'reason':'boundary','address':'0x0041c52c'}:raise ValueError(result['stop'])
                offset=0x5918+boss*0x54+slot*8;after=bytes.fromhex(result['memory']['hud']['after'])
                expected=bytearray(hud);expected[offset:offset+8]=after[offset:offset+8]
                if bytes(expected)!=after:raise ValueError('427 changed unexpected HUD bytes')
                ratio_bits,stored_color=struct.unpack_from('<2I',after,offset)
                result.update(input={'maximum':maximum,'boss':boss,'slot':slot,'life':life,'color':color},
                    after={'offset':offset,'ratioBits':ratio_bits,'color':stored_color})
                cases.append(result)
        return {'schema':'th128-original-boss-gauge427/1','executable':{'sha256':EXE_SHA256,
            'sha256After':hashlib.sha256(native.executable.read_bytes()).hexdigest()},'cases':cases,'code':native.code,'constants':native.constants,
            'scope':'complete unmasked427 handler and original414db0 HUD setter on explicit maximum/boss/slot/HUD state; no allocation/callback returns; display/health animation not claimed',
            'nativeCpuTrace':False,'windowsGameProcessLaunched':False,
            'generatorSha256':hashlib.sha256(Path(__file__).read_bytes()).hexdigest()}
    finally:native.image.close()

if __name__=='__main__':
    parser=argparse.ArgumentParser();parser.add_argument('--exe',required=True);arguments=parser.parse_args()
    report=probe(arguments.exe)
    (ROOT/'local/research/boss-gauge-fixtures.json').write_text(json.dumps(report,indent=2)+'\n',encoding='utf-8',newline='\n')
    print('18 complete original427 normalization/HUD setter cases recorded')
