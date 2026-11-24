import argparse
import hashlib
import json
from pathlib import Path
import struct
from bullet_probe import NativeBullet,ROOT,EXE_SHA256,datum,emulate

def probe(executable):
    native=NativeBullet(executable)
    try:
        integers=[];floats=[]
        for identifier in (-9953,-9952,-9951,-9950):
            expected_int={-9953:0x41cb8e,-9952:0x41cba0,-9951:0x41cbb3,-9950:0x41cbc6}[identifier]
            int_target=struct.unpack('<I',native.read(0x41cfc4+(identifier+10000)*4,4))[0]
            float_target=struct.unpack('<I',native.read(0x41daa8+(identifier+10000)*4,4))[0]
            if int_target!=expected_int:raise ValueError('unexpected integer difficulty-condition dispatch')
            for difficulty in range(5):
                result=emulate([native.chunk(0x41c950,0x41cbf8)],0x41c950,
                    [native.constant(0x41cfc4,368),datum('difficulty',0x4b4d0c,difficulty,writable=False)],
                    arguments=struct.pack('<i',identifier),budget=256)
                if result['stop']['reason']!='returned':raise ValueError(result['stop'])
                result.update(identifier=identifier,difficulty=difficulty,result=result['registers']['eax']);integers.append(result)
                final_load={-9953:0x41d81f,-9952:0x41d84b,-9951:0x41d87b,-9950:0x41d8ab}[identifier]
                floating=emulate([native.chunk(0x41d2c0,0x41d2e0),native.chunk(float_target,final_load+4)],0x41d2c0,
                    [native.constant(0x41daa8,368),native.constant(0x4a6828,8),datum('difficulty',0x4b4d0c,difficulty,writable=False)],
                    arguments=struct.pack('<i',identifier),stops=[final_load],budget=256)
                if floating['stop']['reason']!='boundary':raise ValueError(floating['stop'])
                head=bytes.fromhex(floating['stackHead'])
                floating.update(identifier=identifier,difficulty=difficulty,result=float(struct.unpack_from('<i',head,4)[0]),
                    completedFloatQuery=False);floats.append(floating)
        return {'schema':'th128-original-difficulty-condition-integers/1','executable':{'sha256':EXE_SHA256,
            'sha256After':hashlib.sha256(native.executable.read_bytes()).hexdigest()},'integerCases':integers,'floatCases':floats,
            'code':native.code,'constants':native.constants,'nativeCpuTrace':False,'windowsGameProcessLaunched':False,
            'generatorSha256':hashlib.sha256(Path(__file__).read_bytes()).hexdigest(),
            'limitations':['Integer getters execute complete original returns; float prefixes stop before the final original fild/return and are not integrated from these samples.']}
    finally:native.image.close()

if __name__=='__main__':
    parser=argparse.ArgumentParser();parser.add_argument('--exe',required=True);arguments=parser.parse_args()
    report=probe(arguments.exe);output=ROOT/'local/research/difficulty-condition-fixtures.json'
    output.write_text(json.dumps(report,indent=2)+'\n',encoding='utf-8',newline='\n')
    print('original integer difficulty-condition returns and float pre-return stack values recorded')
