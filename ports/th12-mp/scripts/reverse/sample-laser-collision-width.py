"""Actual three-class laser width branches and original437a80 query results.

Factories, manager, object ticks, embedded ANM and line collision execute.
Synthetic active-phase/age setup is explicit; positive invulnerability retains
the original hit gate without invoking a miss scene. No replay acceptance.
"""
import importlib.util, json, pathlib
ROOT = pathlib.Path(__file__).resolve().parents[2]
spec = importlib.util.spec_from_file_location('laser_native_host', ROOT / 'scripts/reverse/sample-laser-bounce.py')
host = importlib.util.module_from_spec(spec)
spec.loader.exec_module(host)
records = []
for kind in [0, 1, 2]:
    for width in [3., 3.000000238418579, 16., 31.999998092651367, 32., 32.000003814697266, 48., 64.]:
        records.append(host.experiment(kind, (0., 120.), 0., 100., 24. if kind == 2 else 0.,
                       0, 0, extension=0, width=width, active_phase=True, initial_age=18, steps=1))
records.append(host.experiment(1, (16., 144.), -0.6162238717079163, 480., 8., 0, 0,
               extension=0, width=16., player_position=(14.2265625, 177.984375), player_width=1.5,
               active_phase=True, initial_age=18, steps=1))
for kind in [0, 1]:
    for width in [16., 32., 48.]:
        for y in [120., 144., 151., 170.]:
            records.append(host.experiment(kind, (-50., 120.), 0., 100., 0., 0, 0, extension=0,
                           width=width, player_position=(25., y), player_width=1.5,
                           active_phase=True, initial_age=18, steps=1))
output = ROOT / 'tests/fixtures/laser-collision-width-original.json'
output.write_text(json.dumps({'schema': 'th12-original-laser-collision-width/1',
    'originalExeSha256': host.EXE_SHA256, 'wholeGameOracle': False, 'gpuExecuted': False,
    'scope': 'Actual428450 factory and428270 manager/object updates, original embedded ANM '
             'and437a80 line query; allocator/free/audio output are host boundaries. '
             'Synthetic activePhase and initialAge18 are written after factory to isolate '
             'collision branches; Player state1/invulnerability1, explicit cached hitWidth. '
             'The4118 pose comes from actual Native replay prefix, executed here independently. '
             'No scene/application/wholeReplay acceptance.', 'records': records}, indent=2) + '\n', encoding='utf-8')
print(json.dumps({'output': str(output), 'records': len(records),
    'queries': sum(sum(e['kind'] == 'lineCollision' for e in r['events']) for r in records),
    'real4118': [e for e in records[24]['events'] if e['kind'] == 'lineCollision']}))
