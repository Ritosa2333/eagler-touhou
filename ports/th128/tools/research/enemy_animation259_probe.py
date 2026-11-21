import argparse
import hashlib
import json
from pathlib import Path
import struct
from bullet_probe import NativeBullet,ROOT,EXE_SHA256,CONTEXT,TASK,STATE,COMMAND,HANDLER_STOP,datum,emulate

def probe(executable):
    native=NativeBullet(executable)
    try:
        index=native.read(0x41c7c0+259-256,1)[0]
        if struct.unpack('<I',native.read(0x41c554+4*index,4))[0]!=0x418997:raise ValueError('unexpected259 dispatch')
        chunks=[native.chunk(0x418997,0x418a69),native.chunk(0x41e0c0,0x41e0d0),
            native.chunk(0x46fed0,0x46ff66),native.chunk(0x4679b0,0x4679f6),native.chunk(0x467420,0x467479),
            native.chunk(0x41f2a0,0x41f2a1),native.chunk(HANDLER_STOP,HANDLER_STOP+2)]
        cases=[]
        for slot,script in ((1,99),(2,189)):
            for flags in (0,0x20):
                context=bytearray(0x4000);struct.pack_into('<I',context,0x3f7c,TASK)
                struct.pack_into('<I',context,0x3ee4,flags)
                state=bytearray(0x1020);struct.pack_into('<I',state,4,COMMAND)
                manager=bytearray(0xcc);struct.pack_into('<I',manager,0x80,0x390000)
                command=struct.pack('<iHHHBBIii',0,259,24,0,255,2,0,slot,script)
                result=emulate(chunks,0x418997,[datum('context',CONTEXT,bytes(context)),datum('state',STATE,bytes(state)),
                    datum('task',TASK,struct.pack('<2I',0,STATE),writable=False),datum('command',COMMAND,command,writable=False),
                    datum('manager',0x380000,bytes(manager),writable=False),datum('managerGlobal',0x4b8948,0x380000,'I',False),
                    datum('animationManagerGlobal',0x4d2e50,0,'I',False)],
                    registers={'ebx':CONTEXT},stops=(0x41f2a0,HANDLER_STOP),budget=256)
                if result['stop']!={'reason':'boundary','address':'0x0041f2a0'}:raise ValueError(result['stop'])
                result.update(input={'slot':slot,'script':script,'flags':flags},request={'slot':result['registers']['edi'],
                    'script':struct.unpack_from('<i',bytes.fromhex(result['stackHead']),8)[0],
                    'bank':0,'resourcePointer':result['registers']['ecx'],'layer':result['registers']['eax']})
                if result['request']['resourcePointer']!=0x390000:raise ValueError('259 did not use native manager bank0')
                cases.append(result)
        return {'schema':'th128-original-enemy-animation259-prefix/1','executable':{'sha256':EXE_SHA256,
            'sha256After':hashlib.sha256(native.executable.read_bytes()).hexdigest()},'cases':cases,'code':native.code,
            'constants':native.constants,'allocationExecuted':False,'nativeCpuTrace':False,'windowsGameProcessLaunched':False,
            'scope':'actual MBoss positive nonprimary259 requests; original empty-handle retirement and parameter helpers execute; stop at real ANM creation entry before any allocation or return',
            'generatorSha256':hashlib.sha256(Path(__file__).read_bytes()).hexdigest()}
    finally:native.image.close()

if __name__=='__main__':
    parser=argparse.ArgumentParser();parser.add_argument('--exe',required=True);arguments=parser.parse_args()
    report=probe(arguments.exe)
    (ROOT/'local/research/enemy-animation259-fixtures.json').write_text(json.dumps(report,indent=2)+'\n',encoding='utf-8',newline='\n')
    print('four actual MBoss259 requests stop at original ANM allocation entry')
