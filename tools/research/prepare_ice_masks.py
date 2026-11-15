import argparse
import hashlib
import json
from pathlib import Path
from bullet_probe import NativeBullet,ROOT,EXE_SHA256
from ice_mask_probe import PROFILES
def prepare(executable):
    native=NativeBullet(executable)
    try:
        profiles=[]
        for selector,(address,rows,words) in PROFILES.items():
            payload=native.read(address,rows*words*4)
            profiles.append({'selector':selector,'rows':rows,'words':words,'hex':payload.hex(),
                'sha256':hashlib.sha256(payload).hexdigest(),'address':f'0x{address:08x}'})
        report={'schema':'th128-original-ice-masks/1','sourceExeSha256':EXE_SHA256,'profiles':profiles}
        local=ROOT/'local';catalog=json.loads((local/'catalog.json').read_text(encoding='utf8'))
        for variant in ('ja','zh'):
            destination=(local/catalog['variants'][variant]['manifest']).parent/'animations/ice-masks.json'
            if not destination.resolve().is_relative_to(local.resolve()) or destination.is_symlink():raise ValueError('unsafe native-mask destination')
            destination.write_text(json.dumps(report,indent=2)+'\n',encoding='utf8',newline='\n')
        if hashlib.sha256(native.executable.read_bytes()).hexdigest()!=EXE_SHA256:raise ValueError('original EXE changed')
        return report
    finally:native.image.close()
if __name__=='__main__':
    parser=argparse.ArgumentParser();parser.add_argument('--exe',required=True);args=parser.parse_args();prepare(args.exe)
    print('five original ice bitmaps prepared for current local variants; original EXE unchanged')
