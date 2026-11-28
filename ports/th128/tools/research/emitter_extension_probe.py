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
        for slot,index,integers,floats in ((0,1,(0,16,60,1,2,0),(0.,-1.,-999999.,-999999.)),
             (15,17,(7,3,-1,123,456,-789),(.125,-2.5,9.25,-17.5))):
            parameters=struct.pack('<8i4f',slot,index,*integers,*floats)
            command=struct.pack('<iHHHBBI',0,537,64,0,255,12,0)+parameters
            before=bytes([0xa5])*(16*0x380)
            result=emulate([native.chunk(0x41b0a5,0x41b196),native.chunk(0x41e0c0,0x41e0d0),
                native.chunk(0x41e110,0x41e11e),native.chunk(0x46fed0,0x46ff66),native.chunk(0x46ff70,0x470026),
                native.chunk(0x41c52c,0x41c52e)],0x41b0a5,
                [datum('emitters',STATE+0x4a0,before),datum('runner',STATE+0x3f7c,RUNNER,kind='I',writable=False),
                 datum('context',RUNNER+4,CONTEXT,kind='I',writable=False),datum('instructionPointer',CONTEXT+4,COMMAND,kind='I',writable=False),
                 datum('instruction',COMMAND,command,writable=False)],registers={'ebx':STATE},stops=[0x41c52c],budget=1024)
            if result['stop']['reason']!='boundary':raise ValueError(result['stop'])
            after=bytes.fromhex(result['memory']['emitters']['after']);offset=slot*0x380+0x28+index*44
            result.update(slot=slot,index=index,integers=integers,floats=floats,
                recordBefore=list(struct.unpack_from('<11I',before,offset)),recordAfter=list(struct.unpack_from('<11I',after,offset)),
                adjacentUnchanged=after[:offset]==before[:offset] and after[offset+44:]==before[offset+44:])
            result['memory'].pop('emitters');cases.append(result)
        return {'schema':'th128-original-emitter537-fields/1','executable':{'sha256':EXE_SHA256,
            'sha256After':hashlib.sha256(native.executable.read_bytes()).hexdigest()},'cases':cases,'code':native.code,
            'windowsGameProcessLaunched':False,'nativeCpuTrace':False,'generatorSha256':hashlib.sha256(Path(__file__).read_bytes()).hexdigest()}
    finally:native.image.close()

if __name__=='__main__':
    parser=argparse.ArgumentParser();parser.add_argument('--exe',required=True);arguments=parser.parse_args()
    report=probe(arguments.exe);output=ROOT/'local/research/emitter-extension-fixtures.json'
    output.write_text(json.dumps(report,indent=2)+'\n',encoding='utf-8',newline='\n')
    print('original537 complete selected44-byte record stores recorded')
