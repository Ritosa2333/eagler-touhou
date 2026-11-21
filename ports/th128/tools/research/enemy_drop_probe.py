import argparse
import hashlib
import json
from pathlib import Path
import struct
from bullet_probe import NativeBullet,ROOT,EXE_SHA256,datum
from enemy_probe import emulate

STATE,RUNNER,CONTEXT,COMMAND=0x300000,0x310000,0x320000,0x330000
HANDLERS={406:0x41a3e2,407:0x41a3f2,408:0x41a415}

class NativeDrops(NativeBullet):
    def command(self,opcode,parameters):
        index=self.read(0x41c7c0+opcode-256,1)[0]
        target=struct.unpack('<I',self.read(0x41c554+index*4,4))[0]
        if target!=HANDLERS[opcode]:raise ValueError('unexpected original drop dispatch')
        before=[1,*range(11,21),0x41200000,0x41a00000]
        payload=struct.pack('<'+('2f' if opcode==408 else str(len(parameters))+'i'),*parameters)
        command=struct.pack('<iHHHBBI',0,opcode,16+len(payload),0,255,len(parameters),0)+payload
        result=emulate([self.chunk(0x41a3e2,0x41a44b),self.chunk(0x414ab0,0x414ad1),
                        self.chunk(0x41e0c0,0x41e0d0),self.chunk(0x41e110,0x41e11e),
                        self.chunk(0x46fed0,0x46ff66),self.chunk(0x46ff70,0x470026),
                        self.chunk(0x41c52c,0x41c52e)],target,
            [datum('drops',STATE+0x3e54,struct.pack('<13I',*before)),
             datum('stateRunner',STATE+0x3f7c,RUNNER,kind='I',writable=False),
             datum('runnerContext',RUNNER+4,CONTEXT,kind='I',writable=False),
             datum('contextInstruction',CONTEXT+4,COMMAND,kind='I',writable=False),
             datum('instruction',COMMAND,command,writable=False)],registers={'eax':opcode,'ebx':STATE,'edx':RUNNER,'edi':COMMAND},
            stops=[0x41c52c],budget=512)
        if result['stop']['reason']!='boundary':raise ValueError(result['stop'])
        final=bytes.fromhex(result['memory']['drops']['after'])
        result.update(opcode=opcode,parameters=parameters,beforeWords=before,afterWords=list(struct.unpack('<13I',final)))
        return result

def probe(executable):
    native=NativeDrops(executable)
    try:
        cases=[native.command(opcode,parameters) for opcode,parameters in
            ((406,()),(407,(2,2)),(407,(0,8)),(407,(10,17)),(407,(1,-1)),(408,(24,24)),(408,(.125,99.5)),(408,(-1,0)))]
        return {'schema':'th128-original-enemy-drops/1','executable':{'sha256':EXE_SHA256,
            'sha256After':hashlib.sha256(native.executable.read_bytes()).hexdigest()},
            'windowsGameProcessLaunched':False,'nativeCpuTrace':False,'cases':cases,'code':native.code,
            'generatorSha256':hashlib.sha256(Path(__file__).read_bytes()).hexdigest()}
    finally:native.image.close()

if __name__=='__main__':
    parser=argparse.ArgumentParser();parser.add_argument('--exe',required=True);arguments=parser.parse_args()
    report=probe(arguments.exe);output=ROOT/'local/research/enemy-drop-fixtures.json'
    output.write_text(json.dumps(report,indent=2)+'\n',encoding='utf-8',newline='\n')
    print('original enemy 406/407/408 drop configuration handlers recorded')
