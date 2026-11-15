import argparse
import hashlib
import json
from pathlib import Path
import struct
from bullet_probe import NativeBullet,ROOT,EXE_SHA256,datum
from enemy_probe import emulate

STATE,RUNNER,CONTEXT,COMMAND=0x300000,0x340000,0x350000,0x360000

def probe(executable):
    native=NativeBullet(executable)
    try:
        cases=[]
        for opcode,slot,values in ((506,0,(11,1)),(506,15,(65535,32768)),(506,3,(65536,-1)),
                                 (505,0,(2.8,1.)),(505,15,(-1.25,.125))):
            index=native.read(0x41c7c0+opcode-256,1)[0];target=struct.unpack('<I',native.read(0x41c554+index*4,4))[0]
            expected=0x41aa79 if opcode==505 else 0x41aab9
            if target!=expected:raise ValueError('unexpected direct emitter parameter dispatch')
            payload=struct.pack('<i',slot)+struct.pack('<2f' if opcode==505 else '<2i',*values)
            command=struct.pack('<iHHHBBI',0,opcode,28,0,255,3,0)+payload
            before=bytes([0xa5])*(16*0x380)
            result=emulate([native.chunk(0x41aa79,0x41aafb),native.chunk(0x41e0c0,0x41e0d0),
                native.chunk(0x41e110,0x41e11e),native.chunk(0x46fed0,0x46ff66),native.chunk(0x46ff70,0x470026),
                native.chunk(0x41c52c,0x41c52e)],target,
                [datum('emitters',STATE+0x4a0,before),datum('runner',STATE+0x3f7c,RUNNER,kind='I',writable=False),
                 datum('context',RUNNER+4,CONTEXT,kind='I',writable=False),datum('instructionPointer',CONTEXT+4,COMMAND,kind='I',writable=False),
                 datum('instruction',COMMAND,command,writable=False)],registers={'ebx':STATE},stops=[0x41c52c],budget=512)
            if result['stop']['reason']!='boundary':raise ValueError(result['stop'])
            after=bytes.fromhex(result['memory']['emitters']['after']);offset=slot*0x380
            result.update(opcode=opcode,slot=slot,values=values,counts=list(struct.unpack_from('<2h',after,offset+0x364)),
                          speedBits=list(struct.unpack_from('<2I',after,offset+0x1c)),
                          adjacentUnchanged=after[:offset]==before[:offset] and after[offset+0x380:]==before[offset+0x380:])
            result['memory'].pop('emitters');cases.append(result)
        return {'schema':'th128-original-direct-emitter-parameters/1','executable':{'sha256':EXE_SHA256,
            'sha256After':hashlib.sha256(native.executable.read_bytes()).hexdigest()},'cases':cases,'code':native.code,
            'windowsGameProcessLaunched':False,'nativeCpuTrace':False,'generatorSha256':hashlib.sha256(Path(__file__).read_bytes()).hexdigest()}
    finally:native.image.close()

if __name__=='__main__':
    parser=argparse.ArgumentParser();parser.add_argument('--exe',required=True);arguments=parser.parse_args()
    report=probe(arguments.exe);output=ROOT/'local/research/emitter-parameters-fixtures.json'
    output.write_text(json.dumps(report,indent=2)+'\n',encoding='utf-8',newline='\n')
    print('original505/506 direct speed/count stores recorded')
