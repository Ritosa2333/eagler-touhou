import argparse
import hashlib
import json
from pathlib import Path
from bullet_probe import ROOT
from type18_bullet_probe import NativeType18

def probe(executable,prepare=False):
    native=NativeType18(executable)
    try:
        records=[]
        for selected,color in ((23,6),(29,13)):
            payload=native.read(0x4b24d0+selected*0x120,0x120)
            records.append({'type':selected,'color':color,'profile':{'hex':payload.hex(),'sha256':hashlib.sha256(payload).hexdigest()},'spawn':native.spawn_typed(selected,color),
                'ticks':[native.tick(selected_type=selected,**values) for values in ({},{'motionState':1},{'age':8},{'flags':0x1b})]})
        report={'schema':'th128-original-spell-bullet-types/1','types':records,'code':native.code,'constants':native.constants,'sourceExeSha256':hashlib.sha256(native.executable.read_bytes()).hexdigest(),'generatorSha256':hashlib.sha256(Path(__file__).read_bytes()).hexdigest(),'apiReturnsAssigned':False}
        if prepare:
            catalog=json.loads((ROOT/'local/catalog.json').read_text(encoding='utf8'))
            for record in catalog['variants'].values():
                output=(ROOT/'local'/record['manifest']).parent/'animations/bullet-spell-profiles.json'
                output.write_text(json.dumps({'schema':'th128-original-spell-bullet-profiles/1','sourceExeSha256':report['sourceExeSha256'],'profiles':[{'type':entry['type'],**entry['profile']} for entry in records]},indent=2)+'\n',encoding='utf8')
        return report
    finally:native.image.close()
if __name__=='__main__':
    parser=argparse.ArgumentParser();parser.add_argument('--exe',required=True);parser.add_argument('--prepare',action='store_true');arguments=parser.parse_args();report=probe(arguments.exe,arguments.prepare)
    (ROOT/'local/research/spell-bullet-fixtures.json').write_text(json.dumps(report,indent=2)+'\n',encoding='utf8');print('original23/29 profiles and actual spawn/update prefixes recorded')
