import argparse
import hashlib
import json
from pathlib import Path
import struct
from bullet_probe import NativeBullet,ROOT,EXE_SHA256,datum
from enemy_probe import emulate
from freeze_probe import STACK_POINTER
PLAYER,FONT=0x300000,0x380000
def probe(executable):
    native=NativeBullet(executable)
    try:
        chunks=[native.chunk(0x43f48b,0x43f592)]
        cases=[]
        for power in (0,2999,3000,5999,9999,10000):
            for held,x,y in ((0,0,400),(44,-100,200),(45,100,120)):
                player=bytearray(0x44000);struct.pack_into('<3f',player,0xecc,x,y,0)
                struct.pack_into('<i',player,0x43914,held)
                result=emulate(chunks,0x43f48b,[
                    datum('playerPosition',PLAYER+0xecc,bytes(player[0xecc:0xed8]),writable=False),
                    datum('held',PLAYER+0x43914,struct.pack('<i',held),writable=False),
                    datum('fontState',FONT+0x1c848,bytes(0x50)),
                    datum('playerGlobal',0x4b8a80,struct.pack('<I',PLAYER),writable=False),
                    datum('fontGlobal',0x4b8920,struct.pack('<I',FONT),writable=False),
                    datum('power',0x4b4d6c,struct.pack('<i',power),writable=False),native.constant(0x4a6500,0x800)],
                    registers={'ebx':0,'edi':PLAYER,'ebp':STACK_POINTER+0x100},
                    stack={0x108:struct.pack('<I',PLAYER)},stops=(0x43f58d,),budget=256,
                    stack_capture={'position':(0xf0,12),'arguments':(-12,12)})
                if result['stop']!={'reason':'boundary','address':'0x0043f58d'}:raise ValueError(result['stop'])
                font=bytes.fromhex(result['memory']['fontState']['after'])
                position=bytes.fromhex(result['stackSnapshots']['position']['after'])
                arguments=bytes.fromhex(result['stackSnapshots']['arguments']['after'])
                result.update(input={'power':power,'held':held,'x':x,'y':y},after={
                    'x':struct.unpack_from('<f',position)[0],'y':struct.unpack_from('<f',position,4)[0],
                    'percent':struct.unpack_from('<i',arguments,8)[0],'color':struct.unpack_from('<I',font,4)[0],
                    'scale':struct.unpack_from('<f',font,16)[0],'style':struct.unpack_from('<i',font,36)[0]})
                cases.append(result)
        for start,end in ((0x401feb,0x4020d5),(0x401c1f,0x401c3c),(0x40245e,0x402497)):
            native.chunk(start,end)
        glyphs=[]
        for character in '0123456789%':
            result=emulate([native.chunk(0x402015,0x4020d5)],0x402015,[
                datum('fontBank',FONT+0x1c894,struct.pack('<I',0x390000),writable=False),
                datum('spriteTable',0x390118,struct.pack('<I',0x3a0000),writable=False)],
                registers={'eax':ord(character),'edi':FONT},stops=(0x402232,),budget=64)
            if result['stop']!={'reason':'boundary','address':'0x00402232'}:raise ValueError(result['stop'])
            result.update(character=character,sprite=(result['registers']['ecx']-0x3a0000)//72)
            glyphs.append(result)
        return {'schema':'th128-head-power/1','executable':{'sha256':EXE_SHA256,
            'sha256After':hashlib.sha256(native.executable.read_bytes()).hexdigest()},'cases':cases,
            'code':native.code,'constants':native.constants,'glyphCases':glyphs,'formatCallExecuted':False,'windowsGameProcessLaunched':False,
            'generatorSha256':hashlib.sha256(Path(__file__).read_bytes()).hexdigest(),
            'scope':'complete original head-percent prefix until real formatting call, separate static style7 glyph mapping; no formatted-text/API/allocator returns assigned'}
    finally:native.image.close()
if __name__=='__main__':
    parser=argparse.ArgumentParser();parser.add_argument('--exe',required=True);args=parser.parse_args()
    report=probe(args.exe)
    (ROOT/'local/research/head-power-fixtures.json').write_text(json.dumps(report,indent=2)+'\n',encoding='utf8',newline='\n')
    print('18 actual head power position/color/scale/format-argument prefixes recorded')
