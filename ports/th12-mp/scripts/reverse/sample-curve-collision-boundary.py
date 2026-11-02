"""Original Curve collision includes accumulated length exactly16."""
import importlib.util,json,pathlib,struct,hashlib
ROOT=pathlib.Path(__file__).resolve().parents[2]
spec=importlib.util.spec_from_file_location('curve_boundary_host',ROOT/'scripts/reverse/sample-laser-bounce.py')
host=importlib.util.module_from_spec(spec);spec.loader.exec_module(host)
records=[]
for speed in [0.,2.,4.,8.,16.,24.]:
 for player in [(5000.,5000.),(25.,132.)]:
  records.append(host.experiment(2,(0.,120.),0.,100.,speed,0,0,extension=0,width=16,
   player_position=player,player_width=1.5,active_phase=True,initial_age=18,steps=1))
out=ROOT/'tests/fixtures/curve-collision-boundary-original.json'
out.write_text(json.dumps({'originalExeSha256':host.EXE_SHA256,'samplerSha256':hashlib.sha256(pathlib.Path(__file__).read_bytes()).hexdigest(),
 'wholeGameOracle':False,'threshold':struct.unpack_from('<f',host.IMAGE,0x4a3e68-0x400000)[0],
 'scope':'Original Curve factory/428270/42c770 and437a80, eight history nodes. Controlled active phase, age18, player half extent1.5 and invulnerability1. Original ANM executes; resource/heap/audio boundaries retained. No replay/GPU acceptance.',
 'records':records},indent=2)+'\n')
print(json.dumps({'cases':len(records),'threshold':struct.unpack_from('<f',host.IMAGE,0x4a3e68-0x400000)[0],'queries':sum(sum(e['kind']=='lineCollision' for e in r['events']) for r in records)}))
