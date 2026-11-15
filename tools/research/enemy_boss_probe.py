import argparse
import hashlib
import json
from pathlib import Path
import struct
from bullet_probe import NativeBullet,ROOT,EXE_SHA256,CONTEXT,TASK,STATE,COMMAND,HANDLER_STOP,datum,emulate

MANAGER=0x380000

def probe(executable):
    native=NativeBullet(executable)
    try:
        chunks=[native.chunk(0x41a472,0x41a517),native.chunk(0x41e0c0,0x41e0d0),
                native.chunk(0x46fed0,0x46ff66),native.chunk(HANDLER_STOP,HANDLER_STOP+2)]
        for opcode,target in ((411,0x41a472),(412,0x41a4bb)):
            index=native.read(0x41c7c0+opcode-256,1)[0]
            if struct.unpack('<I',native.read(0x41c554+4*index,4))[0]!=target:
                raise ValueError('unexpected original boss handler dispatch')

        def run(opcode,argument,flags,retained=3):
            context=bytearray(0x4000)
            struct.pack_into('<I',context,0x3f7c,TASK)
            struct.pack_into('<I',context,0x3ee4,flags)
            struct.pack_into('<i',context,0x3ef4,retained)
            words=[0x11111111,0x22222222,0x33333333,0x44444444,0x55555555,0x66666666]
            struct.pack_into('<6I',context,0x3e3c,*words)
            state=bytearray(0x1020);struct.pack_into('<I',state,4,COMMAND)
            manager=bytearray(0xcc)
            owners=list(range(100,108));manager_flags=0x55abcdef
            struct.pack_into('<8I',manager,0x5c,*owners)
            struct.pack_into('<I',manager,0x7c,manager_flags)
            command=struct.pack('<iHHHBBIi',0,opcode,20,0,255,1,0,argument)
            result=emulate(chunks,0x41a472 if opcode==411 else 0x41a4bb,
                [datum('context',CONTEXT,bytes(context)),datum('state',STATE,bytes(state)),
                 datum('task',TASK,struct.pack('<2I',0,STATE),writable=False),
                 datum('command',COMMAND,command,writable=False),datum('manager',MANAGER,bytes(manager)),
                 datum('managerGlobal',0x4b8948,MANAGER,'I',False)],
                registers={'ebx':CONTEXT},stops=(HANDLER_STOP,),budget=256)
            if result['stop']!={'reason':'boundary','address':'0x0041c52c'}:raise ValueError(result['stop'])
            after=bytes.fromhex(result['memory']['context']['after'])
            manager_after=bytes.fromhex(result['memory']['manager']['after'])
            result.update(input={'flags':flags,'slot':argument,'retainedSlot':retained,'managerFlags':manager_flags,
                'owners':owners,'life':argument,'lifeWords':words},
                after={'flags':struct.unpack_from('<I',after,0x3ee4)[0],
                    'retainedSlot':struct.unpack_from('<i',after,0x3ef4)[0],
                    'managerFlags':struct.unpack_from('<I',manager_after,0x7c)[0],
                    'owners':list(struct.unpack_from('<8I',manager_after,0x5c)),
                    'lifeWords':list(struct.unpack_from('<6I',after,0x3e3c))})
            return result

        bindings=[run(412,slot,flags,retained) for slot in (-3,-1,0,1,7)
                  for flags in (0,0x1000000,0xffffffff) for retained in (0,3)]
        life=[run(411,value,flags) for value in (0,-1,10900,13400,-2147483648,2147483647)
              for flags in (0,0x1000000,0x2000000,0xffffffff)]
        return {'schema':'th128-original-enemy-boss-stores/1','executable':{'sha256':EXE_SHA256,
            'sha256After':hashlib.sha256(native.executable.read_bytes()).hexdigest()},
            'bindingCases':bindings,'lifeCases':life,'code':native.code,'constants':native.constants,
            'ownerIdentity':TASK,'bossTableOffset':0x5c,'managerFlagsOffset':0x7c,
            'scope':'complete411/412 handlers on explicit preallocated state, unmasked operands, eight bounded boss slots; original parameter helpers execute; no allocator/API/callback stubs',
            'nativeCpuTrace':False,'windowsGameProcessLaunched':False,
            'generatorSha256':hashlib.sha256(Path(__file__).read_bytes()).hexdigest()}
    finally:native.image.close()

if __name__=='__main__':
    parser=argparse.ArgumentParser();parser.add_argument('--exe',required=True);arguments=parser.parse_args()
    report=probe(arguments.exe)
    (ROOT/'local/research/enemy-boss-fixtures.json').write_text(json.dumps(report,indent=2)+'\n',encoding='utf-8',newline='\n')
    print('54 complete original boss binding/life handler paths recorded')
