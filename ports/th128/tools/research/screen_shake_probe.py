import argparse
import hashlib
import json
from pathlib import Path
import struct
from bullet_probe import NativeBullet,ROOT,datum,emulate


def probe(executable):
    native=NativeBullet(executable)
    try:
        cases=[]
        for time in (0,1,14,28,29):
            state=bytearray(0x44);struct.pack_into('<3i',state,0x1c,30,12,0);struct.pack_into('<iifII',state,0x30,time-1,time,float(time),0x4b6f58,1)
            result=emulate([native.chunk(0x457010,0x457106),native.chunk(0x402ac0,0x402b18),native.chunk(0x46a460,0x46a4f5)],0x457010,
                [datum('effect',0x390000,bytes(state)),datum('random',0x4d2ae0,struct.pack('<HHI',0x1234,0,0)),
                 datum('offset',0x4d3384,bytes(8)),datum('threadFlags',0x4d33f8,0),datum('paused',0x4d2ad4,0),datum('clock',0x4b6f58,struct.pack('<f',1),writable=False),
                 native.constant(0x4a6698,8),native.constant(0x4a6694,4),native.constant(0x4a6588,8)],registers={'ecx':0x390000},budget=1024)
            if result['stop']['reason']!='returned':raise ValueError(result['stop'])
            random=bytes.fromhex(result['memory']['random']['after'])
            result.update(time=time,offsetBits=list(struct.unpack('<2I',bytes.fromhex(result['memory']['offset']['after']))),
                seed=struct.unpack_from('<H',random)[0],randomCalls=struct.unpack_from('<I',random,4)[0],returned=result['registers']['eax']);cases.append(result)
        return {'schema':'th128-original-screen-shake/1','cases':cases,'code':native.code,'constants':native.constants,'apiReturnsAssigned':False,
            'sourceExeSha256':hashlib.sha256(native.executable.read_bytes()).hexdigest(),'generatorSha256':hashlib.sha256(Path(__file__).read_bytes()).hexdigest()}
    finally:native.image.close()


if __name__=='__main__':
    parser=argparse.ArgumentParser();parser.add_argument('--exe',required=True);report=probe(parser.parse_args().exe)
    (ROOT/'local/research/screen-shake-fixtures.json').write_text(json.dumps(report,indent=2)+'\n',encoding='utf8')
    print('5 complete original shake ticks including original timer and shared RNG helpers recorded')
