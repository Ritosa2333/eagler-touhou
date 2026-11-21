import argparse
import hashlib
import json
from pathlib import Path
import struct
from bullet_probe import NativeBullet,ROOT,EXE_SHA256,CONTEXT,TASK,STATE,COMMAND,HANDLER_STOP,datum,emulate

def fields(payload):
    return {'life':struct.unpack_from('<i',payload,0x5004)[0],
        'remaining':struct.unpack_from('<i',payload,0x500c)[0],
        'threshold':struct.unpack_from('<i',payload,0x5014)[0],
        'records':[list(struct.unpack_from('<4I',payload,0x50c4+slot*16)) for slot in range(8)],
        'timer':list(struct.unpack_from('<3I',payload,0x1434))+list(struct.unpack_from('<2I',payload,0x1440)),
        'flags':struct.unpack_from('<I',payload,0x50ac)[0]}

def probe(executable):
    native=NativeBullet(executable)
    try:
        index=native.read(0x41c7c0+414-256,1)[0]
        if struct.unpack('<I',native.read(0x41c554+4*index,4))[0]!=0x41b2a7:raise ValueError('unexpected414 handler')
        chunks=[native.chunk(0x41b2a7,0x41b2fa),native.chunk(0x41e0c0,0x41e0d0),native.chunk(0x46fed0,0x46ff66),
            native.chunk(0x41de30,0x41de4e),native.chunk(HANDLER_STOP,HANDLER_STOP+2)]
        registrations=[]
        for slot,threshold,limit in ((0,6300,1800),(0,8200,1800),(7,1,0),(7,-1,1800),(0,0,1800),(1,100,-1)):
            payload=struct.pack('<iiiI',slot,threshold,limit,8)+b'MBoss2\0\0'
            command=struct.pack('<iHHHBBI',0,414,16+len(payload),0,255,4,0)+payload
            context=bytearray(0x4000);struct.pack_into('<I',context,0x3f7c,TASK)
            state=bytearray(0x1020);struct.pack_into('<I',state,4,COMMAND)
            enemy=bytearray([0xa5])*0x5178;struct.pack_into('<I',enemy,4,STATE)
            result=emulate(chunks,0x41b2a7,[datum('context',CONTEXT,bytes(context)),datum('state',STATE,bytes(state)),
                datum('enemy',TASK,bytes(enemy)),datum('command',COMMAND,command,writable=False)],
                registers={'ebx':CONTEXT},stack={0x18:struct.pack('<I',COMMAND)},stops=(HANDLER_STOP,),budget=256)
            if result['stop']!={'reason':'boundary','address':'0x0041c52c'}:raise ValueError(result['stop'])
            after=bytes.fromhex(result['memory']['enemy']['after'])
            result.update(input={'slot':slot,'threshold':threshold,'limit':limit,'name':'MBoss2'},before=fields(enemy),after=fields(after))
            registrations.append(result)
        query_chunks=[native.chunk(0x41dea0,0x41e0b8)];queries=[]
        variants=[('single',[(0,6300,1800,0x360000,0x360010)]),('inactive',[]),
            ('late',[(7,6300,1800,0x360000,0x360010)]),
            ('priority',[(0,6300,1800,0x360000,0x360010),(1,10000,1,0x360020,0x360030)]),
            ('timeless',[(0,6300,0,0x360000,0x360010)])]
        for name,records in variants:
            for life,elapsed,flags in ((10900,0,0x10),(10900,120,0x10),(6301,1799,0x10),(6300,0,0x10),
                (6299,1800,0x10),(10900,1800,0x10),(10900,1800,0),(10900,1801,1),
                (-1,1800,1),(10900,-1,0x10)):
                enemy=bytearray(0x5178)
                for slot in range(8):struct.pack_into('<4i',enemy,0x50c4+slot*16,-1,-1,0,0)
                for slot,threshold,limit,health_target,time_target in records:
                    struct.pack_into('<4I',enemy,0x50c4+slot*16,threshold,limit,health_target,time_target)
                struct.pack_into('<i',enemy,0x5004,life);struct.pack_into('<i',enemy,0x1434,elapsed-1)
                struct.pack_into('<ifII',enemy,0x1438,elapsed,float(elapsed),0x4b6f58,1)
                struct.pack_into('<I',enemy,0x50ac,0x81000000)
                spell=bytearray(0x84);struct.pack_into('<I',spell,0x7c,flags);struct.pack_into('<i',spell,0x28,30)
                hud=bytearray([0xa5])*0x5b00;manager=bytearray(0xcc);struct.pack_into('<i',manager,0x18,9)
                player=bytearray(0x40);struct.pack_into('<i',player,0x3c,1)
                result=emulate(query_chunks,0x41dea0,[datum('enemy',CONTEXT,bytes(enemy)),datum('hud',0x380000,bytes(hud)),
                    datum('hudGlobal',0x4b8950,0x380000,'I',False),datum('spell',0x3a0000,bytes(spell)),
                    datum('spellGlobal',0x4b8934,0x3a0000,'I',False),datum('manager',0x3b0000,bytes(manager)),
                    datum('managerGlobal',0x4b8948,0x3b0000,'I',False),datum('player',0x3d0000,bytes(player),writable=False),
                    datum('playerGlobal',0x4b892c,0x3d0000,'I',False)],registers={'ecx':CONTEXT},budget=512)
                if result['stop']['reason']!='returned':raise ValueError(result['stop'])
                after=bytes.fromhex(result['memory']['enemy']['after']);after_hud=bytes.fromhex(result['memory']['hud']['after'])
                after_spell=bytes.fromhex(result['memory']['spell']['after']);after_manager=bytes.fromhex(result['memory']['manager']['after'])
                result.update(input={'variant':name,'life':life,'elapsed':elapsed,'spellFlags':flags},before=fields(enemy),after=fields(after),
                    target=result['registers']['eax'],hudCountdown=list(struct.unpack_from('<2I',after_hud,0x5a24)),
                    spellFlagsAfter=struct.unpack_from('<I',after_spell,0x7c)[0],spell80After=struct.unpack_from('<I',after_spell,0x80)[0],
                    managerWord18=struct.unpack_from('<i',after_manager,0x18)[0])
                queries.append(result)
        return {'schema':'th128-original-enemy-phase414-query/1','executable':{'sha256':EXE_SHA256,
            'sha256After':hashlib.sha256(native.executable.read_bytes()).hexdigest()},'registrationCases':registrations,
            'queryCases':queries,'code':native.code,'constants':native.constants,'nativeCpuTrace':False,'windowsGameProcessLaunched':False,
            'scope':'complete unmasked414 handlers and complete41dea0 queries on declared eight-record tables/timers/HUD/spell/player/manager state; no native allocations/API returns; not phase VM reexecution',
            'generatorSha256':hashlib.sha256(Path(__file__).read_bytes()).hexdigest()}
    finally:native.image.close()

if __name__=='__main__':
    parser=argparse.ArgumentParser();parser.add_argument('--exe',required=True);arguments=parser.parse_args()
    report=probe(arguments.exe)
    (ROOT/'local/research/enemy-phase-fixtures.json').write_text(json.dumps(report,indent=2)+'\n',encoding='utf-8',newline='\n')
    print('six complete414 registrations and fifty complete original phase queries recorded')
