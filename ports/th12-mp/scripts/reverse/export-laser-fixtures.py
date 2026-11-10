"""Freeze explicit native laser experiments, not gameplay/replay goldens."""
import hashlib,json,pathlib
ROOT=pathlib.Path(__file__).resolve().parents[2]
sources=[ROOT/'artifacts/reverse/combat'/s for s in ['laser-lifecycle.json','native-probe.json']]
life,probe=[json.loads(p.read_text()) for p in sources]
assert not life['wholeGameOracle'] and not probe['wholeGameOracle']
assert len(life['records'])==12 and len(probe['laserGeometry'])==60
out=ROOT/'tests/native/laser-original.json';out.parent.mkdir(parents=True,exist_ok=True)
out.write_text(json.dumps({'schema':'th12-native-laser-cpp-fixtures/1','originalExeSha256':life['originalExeSha256'],
 'wholeGameOracle':False,'sources':[{'path':str(p.relative_to(ROOT)),'sha256':hashlib.sha256(p.read_bytes()).hexdigest()} for p in sources],
 'hostFixtures':life['fixtures'],'geometryFixtures':probe['fixtures'],
 'records':life['records'],'geometry':probe['laserGeometry']},indent=2)+'\n')
print(out)
