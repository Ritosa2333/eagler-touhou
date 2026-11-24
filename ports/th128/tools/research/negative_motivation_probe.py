import argparse
import hashlib
import json
from pathlib import Path
from bullet_probe import ROOT
from shot_enemy_collision_probe import NativeShotEnemyCollision


def probe(executable):
    native=NativeShotEnemyCollision(executable)
    try:
        cases=[native.impact(value,flag) for value,flag in ((-8530,0),(-10000,0),(-9999,0),(-10001,0),(-2,0),(-2147483648,0),(-10001,1))]
        return {'schema':'th128-original-negative-motivation/1','cases':cases,'code':native.code,'constants':native.constants,
            'apiReturnsAssigned':False,'sourceExeSha256':hashlib.sha256(native.executable.read_bytes()).hexdigest(),'generatorSha256':hashlib.sha256(Path(__file__).read_bytes()).hexdigest()}
    finally:native.image.close()


if __name__=='__main__':
    parser=argparse.ArgumentParser();parser.add_argument('--exe',required=True);report=probe(parser.parse_args().exe)
    (ROOT/'local/research/negative-motivation-fixtures.json').write_text(json.dumps(report,indent=2)+'\n',encoding='utf8')
    print('7 real negative motivation impact branches recorded')
