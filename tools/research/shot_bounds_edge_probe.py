import argparse
import hashlib
import json
from pathlib import Path
from bullet_probe import ROOT
from player_shot_tick_probe import NativePlayerShotTick


def probe(executable):
    native=NativePlayerShotTick(executable)
    try:
        cases=[native.rotated_bounds(position=(horizontal,vertical,.1),sprite=(64,16),scale=(1,.8))
            for horizontal in (126.,146.,494.,514.,168.66,451.34) for vertical in (32.,8.,-16.)]
        return {'schema':'th128-original-shot-bounds-edges/1','cases':cases,'code':native.code,'constants':native.constants,'apiReturnsAssigned':False,
            'sourceExeSha256':hashlib.sha256(native.executable.read_bytes()).hexdigest(),'generatorSha256':hashlib.sha256(Path(__file__).read_bytes()).hexdigest()}
    finally:native.image.close()


if __name__=='__main__':
    parser=argparse.ArgumentParser();parser.add_argument('--exe',required=True);report=probe(parser.parse_args().exe)
    (ROOT/'local/research/shot-bounds-edge-fixtures.json').write_text(json.dumps(report,indent=2)+'\n',encoding='utf8')
    print('18 actual rotated shot corner buffers at bottom-edge spawn trajectories recorded')
