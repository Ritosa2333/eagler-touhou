import argparse
import hashlib
import json
from pathlib import Path
from player_shot_tick_probe import NativePlayerShotTick,ROOT

def probe(executable):
    native=NativePlayerShotTick(executable)
    try:
        cases=[]
        for horizontal in (-180.,-121.290001,-100.,-.125,0.,.125,100.,121.290001,180.):
            for speed in (3.,24.):
                sample=native.tick(position=(horizontal,-35.7999992,.1),speed=speed,active=2)
                if sample['stop']!={'reason':'boundary','address':'0x0043f823'}:raise ValueError(sample['stop'])
                cases.append(sample)
        for horizontal,velocity in ((1.,2**-40),(-1.,-2**-40),(180.,2**-100),(-180.,-2**-100)):
            sample=native.integrate(position=(horizontal,100,.1),velocity=(velocity,-3.,0))
            if sample['stop']['reason']!='returned':raise ValueError(sample['stop'])
            sample['integratorOnly']=True;cases.append(sample)
        return {'schema':'th128-player-shot-exponent-edges/1','cases':cases,'code':native.code,'constants':native.constants,
            'executableSha256':hashlib.sha256(native.executable.read_bytes()).hexdigest(),
            'scope':'original shot movement prefix through real polar/integrator/floor helpers, plus complete integrator returns; no animation lookup is simulated',
            'generatorSha256':hashlib.sha256(Path(__file__).read_bytes()).hexdigest(),'apiReturnsAssigned':False}
    finally:native.image.close()

if __name__=='__main__':
    parser=argparse.ArgumentParser();parser.add_argument('--exe',required=True);arguments=parser.parse_args()
    report=probe(arguments.exe);(ROOT/'local/research/shot-motion-edge-fixtures.json').write_text(json.dumps(report,indent=2)+'\n',encoding='utf8',newline='\n')
    print('22 original shot-motion exponent-edge cases captured')
