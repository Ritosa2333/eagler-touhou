import argparse
import hashlib
import json
from pathlib import Path
import struct
from bullet_probe import NativeBullet,ROOT,datum,emulate,STACK_POINTER


def probe(executable):
    native=NativeBullet(executable)
    try:
        gates=[];scalars=[];waits=[];message=0x390000;hud=0x380000;command=0x3a0000
        for present,counter in ((False,0),(True,0),(True,1),(True,2)):
            result=emulate([native.chunk(0x41b3b5,0x41b3dd),native.chunk(0x414df0,0x414e02),native.chunk(0x41c52c,0x41c52e)],0x41b3b5,
                [datum('hudGlobal',0x4b8950,hud,kind='I',writable=False),datum('messagePointer',hud+0x5a1c,message if present else 0,kind='I',writable=False),
                 datum('gate',message+0x17c,counter,writable=False)],stops=(0x41c52c,0x41c52e),budget=128)
            if result['stop']['reason']!='boundary':raise ValueError(result['stop'])
            result.update(present=present,counter=counter,held=result['stop']['address']=='0x0041c52e');gates.append(result)
        for opcode,start,end,payload in ((10,0x4241e0,0x4241f8,struct.pack('<i',1)),(12,0x42445e,0x42446d,b''),
            (28,0x423769,0x423780,struct.pack('<2f',180,256)),(29,0x424852,0x42486a,struct.pack('<i',1))):
            image=bytearray(0x1b0);struct.pack_into('<I',image,0x180,0x10)
            result=emulate([native.chunk(start,end),native.chunk(0x4248be,0x4248de)],start,
                [datum('message',message,bytes(image)),datum('command',command,struct.pack('<HBB',0,opcode,len(payload))+payload,writable=False)],
                registers={'esi':message,'eax':command},stops=(0x4248be,),budget=128)
            if result['stop']['reason']!='boundary':raise ValueError(result['stop'])
            result.update(opcode=opcode);scalars.append(result)
        for remaining,pressed in ((0,0),(5,0),(1,0),(5,1),(5,512)):
            image=bytearray(0x1b0);struct.pack_into('<iifII',image,0x2c,remaining-1,remaining,float(remaining),0x4b6f58,1)
            frame=STACK_POINTER+0x200
            result=emulate([native.chunk(0x4243b2,0x42445e),native.chunk(0x403280,0x4032bd),native.chunk(0x46aa20,0x46aa85),native.chunk(0x48fac0,0x48fadc),native.chunk(0x424966,0x424979),native.chunk(0x4248be,0x4248de)],0x4243b2,
                [datum('message',message,bytes(image)),datum('command',command,struct.pack('<HBBi',38,11,4,600),writable=False),
                 datum('pressed',0x4db21c,pressed,kind='I',writable=False),datum('sse',0x4dcd68,1,writable=False),datum('clock',0x4b6f58,struct.pack('<f',1),writable=False),
                 native.constant(0x4a6688,4),native.constant(0x4a6698,8),native.constant(0x4a6694,4),native.constant(0x4a6588,8)],
                registers={'esi':message,'eax':command,'ebp':frame},stack={frame-0x54-STACK_POINTER:struct.pack('<I',message)},
                stops=(0x424966,0x42443b,0x4248be),budget=512)
            if result['stop']['reason']!='boundary':raise ValueError(result['stop'])
            final=bytes.fromhex(result['memory']['message']['after']);result.update(remaining=remaining,pressed=pressed,nextRemaining=struct.unpack_from('<i',final,0x30)[0]);waits.append(result)
        return {'schema':'th128-original-message-control/1','gateCases':gates,'scalarCases':scalars,'waitCases':waits,'code':native.code,'constants':native.constants,'apiReturnsAssigned':False,
            'sourceExeSha256':hashlib.sha256(native.executable.read_bytes()).hexdigest(),'generatorSha256':hashlib.sha256(Path(__file__).read_bytes()).hexdigest()}
    finally:native.image.close()


if __name__=='__main__':
    parser=argparse.ArgumentParser();parser.add_argument('--exe',required=True);report=probe(parser.parse_args().exe)
    (ROOT/'local/research/message-control-fixtures.json').write_text(json.dumps(report,indent=2)+'\n',encoding='utf8')
    print('original419 gates, MSG scalar fields and11 countdown/confirmation branches recorded')
