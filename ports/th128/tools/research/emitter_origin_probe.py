import argparse
import hashlib
import json
from pathlib import Path
import struct
from bullet_probe import NativeBullet,ROOT,CONTEXT,TASK,STATE,COMMAND,HANDLER_STOP,datum,emulate

def probe(executable):
    native=NativeBullet(executable)
    try:
        cases=[]
        for opcode,start,end in ((503,0x41a903,0x41a93b),(523,0x41a93b,0x41a9a0),(525,0x41a9cf,0x41aa39)):
            for slot in (0,3,15):
                for first,second in ((.5,32.),(-991.,80.),(-990.,48.),(-989.,-20.)):
                    image=bytearray(0x4000);struct.pack_into('<I',image,0x3f7c,TASK);state=bytearray(0x1020);struct.pack_into('<I',state,4,COMMAND)
                    command=struct.pack('<iHHHBBIiff',0,opcode,28,0,255,3,0,slot,first,second)
                    spans=[(start,end),(0x41e0c0,0x41e0d0),(0x41e110,0x41e11e),(0x46fed0,0x46ff66),(0x46ff70,0x470026),(0x41f750,0x41f770),(HANDLER_STOP,HANDLER_STOP+2)]
                    result=emulate([native.chunk(*span) for span in spans],start,[datum('context',CONTEXT,bytes(image)),datum('state',STATE,bytes(state)),datum('task',TASK,struct.pack('<2I',0,STATE),writable=False),datum('command',COMMAND,command,writable=False),native.constant(0x4a6718,8)],registers={'ebx':CONTEXT},stops=(HANDLER_STOP,),budget=512)
                    if result['stop']['reason']!='boundary':raise ValueError(result['stop'])
                    after=bytes.fromhex(result['memory']['context']['after']);offset=(0x3d60 if opcode==525 else 0x3ca0)+slot*12
                    result.update(opcode=opcode,slot=slot,input=[first,second],resultBits=list(struct.unpack_from('<3I',after,offset)));cases.append(result)
        origins=[]
        for absolute in (False,True):
            image=bytearray(0x4000);struct.pack_into('<3f',image,0x34,20,100,.1);struct.pack_into('<3f',image,0x3ca0,32,-12,0);struct.pack_into('<3f',image,0x3d60,-176,48,float(absolute))
            result=emulate([native.chunk(0x41a7c6,0x41a89a)],0x41a7c6,[datum('context',CONTEXT,bytes(image)),native.constant(0x4a6b00,8)],registers={'ebx':CONTEXT,'eax':0},stops=(0x41a89a,),budget=128)
            if result['stop']['reason']!='boundary':raise ValueError(result['stop'])
            after=bytes.fromhex(result['memory']['context']['after']);result.update(absolute=absolute,resultBits=list(struct.unpack_from('<3I',after,0x4a8)));origins.append(result)
        return {'schema':'th128-original-emitter-origins/1','cases':cases,'origins':origins,'code':native.code,'constants':native.constants,'apiReturnsAssigned':False,
            'sourceExeSha256':hashlib.sha256(native.executable.read_bytes()).hexdigest(),'generatorSha256':hashlib.sha256(Path(__file__).read_bytes()).hexdigest()}
    finally:native.image.close()
if __name__=='__main__':
    parser=argparse.ArgumentParser();parser.add_argument('--exe',required=True);report=probe(parser.parse_args().exe)
    (ROOT/'local/research/emitter-origin-fixtures.json').write_text(json.dumps(report,indent=2)+'\n',encoding='utf8');print('36 original503/523/525 setters and2 emission origins captured')
