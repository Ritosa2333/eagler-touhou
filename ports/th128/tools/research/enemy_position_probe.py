import argparse
import hashlib
import json
from pathlib import Path
import struct
from bullet_probe import NativeBullet,ROOT,EXE_SHA256,CONTEXT,TASK,STATE,COMMAND,HANDLER_STOP,datum,emulate

def probe(executable):
    native=NativeBullet(executable)
    try:
        initial=[];steps=[]
        chunks=[native.chunk(0x4191bd,0x4192f0),native.chunk(0x41e0c0,0x41e0d0),native.chunk(0x41e110,0x41e11e),
            native.chunk(0x46fed0,0x46ff66),native.chunk(0x46ff70,0x470026),native.chunk(0x406f80,0x406fb4),
            native.chunk(HANDLER_STOP,HANDLER_STOP+2)]
        for duration,x,y in ((120,0,144),(1,10,20),(120,-999999,144),(120,0,-999999),(0,0,144),(-1,0,144)):
            context=bytearray(0x4000);struct.pack_into('<I',context,0x3f7c,TASK);struct.pack_into('<3f',context,0x68,5,-32,.125)
            state=bytearray(0x1020);struct.pack_into('<I',state,4,COMMAND)
            command=struct.pack('<iHHHBBIiiff',0,301,32,0,255,4,0,duration,4,x,y)
            result=emulate(chunks,0x4191bd,[datum('context',CONTEXT,bytes(context)),datum('state',STATE,bytes(state)),
                datum('task',TASK,struct.pack('<2I',0,STATE),writable=False),datum('command',COMMAND,command,writable=False),
                datum('zeroVector',0x4d2e54,bytes(12),writable=False),native.constant(0x4a6c40,8)],
                registers={'ebx':CONTEXT,'eax':301,'edx':TASK},stops=(HANDLER_STOP,),budget=512)
            if result['stop']!={'reason':'boundary','address':'0x0041c52c'}:raise ValueError(result['stop'])
            block=bytes.fromhex(result['memory']['context']['after'])[0x2a0:0x2ec]
            result.update(input={'position':[5,-32,.125],'duration':duration,'mode':4,'x':x,'y':y},
                after={'duration':struct.unpack_from('<i',block,0x44)[0],'mode':struct.unpack_from('<i',block,0x48)[0],
                    'timer':list(struct.unpack_from('<3I',block,0x30)),
                    'start':list(struct.unpack_from('<3I',block)),'target':list(struct.unpack_from('<3I',block,12))})
            initial.append(result)
        step_chunks=[native.chunk(0x4065d0,0x4068df),native.chunk(0x402ac0,0x402b18),
            native.chunk(0x46b190,0x46b1c2)]
        for elapsed in (0,1,29,59,60,118,119,120):
            track=bytearray(0x4c);struct.pack_into('<6f',track,0,5,-32,.125,0,144,0)
            struct.pack_into('<iifIIii',track,0x30,elapsed-1,elapsed,float(elapsed),0x4b6f58,1,120,4)
            result=emulate(step_chunks,0x4065d0,[datum('track',0x380000,bytes(track)),datum('output',0x390000,bytes(12)),
                datum('unitClock',0x4b6f58,1,'f',False),native.constant(0x4a6698,8),native.constant(0x4a6694,4),
                native.constant(0x4a6588,8),native.constant(0x46b428,64)],registers={'edi':0x380000,'ebx':0x390000},budget=512)
            if result['stop']['reason']!='returned':raise ValueError(result['stop'])
            after=bytes.fromhex(result['memory']['track']['after'])
            result.update(input={'start':[5,-32,.125],'target':[0,144,0],'duration':120,'elapsed':elapsed},
                after={'duration':struct.unpack_from('<i',after,0x44)[0],'timer':list(struct.unpack_from('<3I',after,0x30)),
                    'position':list(struct.unpack('<3I',bytes.fromhex(result['memory']['output']['after'])))})
            steps.append(result)
        return {'schema':'th128-original-enemy301-position/1','executable':{'sha256':EXE_SHA256,
            'sha256After':hashlib.sha256(native.executable.read_bytes()).hexdigest()},'initializationCases':initial,'stepCases':steps,
            'code':native.code,'constants':native.constants,'nativeCpuTrace':False,'windowsGameProcessLaunched':False,
            'scope':'complete unmasked301 initialization and complete4065d0 mode4 original interpolation returns with unit clock; other modes/callers are not inferred',
            'generatorSha256':hashlib.sha256(Path(__file__).read_bytes()).hexdigest()}
    finally:native.image.close()

if __name__=='__main__':
    parser=argparse.ArgumentParser();parser.add_argument('--exe',required=True);arguments=parser.parse_args()
    report=probe(arguments.exe)
    (ROOT/'local/research/enemy-position-fixtures.json').write_text(json.dumps(report,indent=2)+'\n',encoding='utf-8',newline='\n')
    print('six original301 initializations and eight complete mode4 interpolation steps recorded')
