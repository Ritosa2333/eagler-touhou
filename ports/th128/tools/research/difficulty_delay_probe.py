import argparse
import hashlib
import json
from pathlib import Path
import struct
from bullet_probe import NativeBullet,ROOT,datum,emulate,CONTEXT,TASK,STATE,COMMAND
def probe(executable):
    native=NativeBullet(executable)
    try:
        spans=((0x41c214,0x41c292),(0x41e0c0,0x41e0d0),(0x46fed0,0x46ff66),(0x41c52a,0x41c52e));cases=[]
        for difficulty in range(5):
            for clock in (0.,123.5):
                context=bytearray(0x4000);struct.pack_into('<I',context,0x3f7c,TASK);state=struct.pack('<fI',clock,COMMAND)
                result=emulate([native.chunk(*span) for span in spans],0x41c214,
                    [datum('context',CONTEXT,bytes(context)),datum('task',TASK,struct.pack('<2I',0,STATE),writable=False),datum('state',STATE,state),
                     datum('command',COMMAND,struct.pack('<iHHHBBI4i',0,448,32,0,255,4,0,5,5,1,1),writable=False),
                     datum('difficulty',0x4b4d0c,difficulty,writable=False)],registers={'ebx':CONTEXT},stops=(0x41c52c,),budget=1024)
                if result['stop']['reason']!='boundary':raise ValueError(result['stop'])
                result.update(difficulty=difficulty,clock=clock,clockAfter=struct.unpack_from('<f',bytes.fromhex(result['memory']['state']['after']))[0]);cases.append(result)
        return {'schema':'th128-original448-clock-subtraction/1','cases':cases,'code':native.code,
            'sourceExeSha256':hashlib.sha256(native.executable.read_bytes()).hexdigest(),'apiReturnsAssigned':False,
            'scope':'complete original448 scalar handler through actual integer resolvers and own context clock store; no game API returns',
            'generatorSha256':hashlib.sha256(Path(__file__).read_bytes()).hexdigest()}
    finally:native.image.close()
if __name__=='__main__':
    parser=argparse.ArgumentParser();parser.add_argument('--exe',required=True);args=parser.parse_args();report=probe(args.exe)
    (ROOT/'local/research/difficulty-delay-fixtures.json').write_text(json.dumps(report,indent=2)+'\n',encoding='utf8',newline='\n');print('10 original448 difficulty-clock stores recorded')
