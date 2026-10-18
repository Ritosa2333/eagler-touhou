"""Bind original function evidence and source hashes to Item CPU results."""
from pathlib import Path
import hashlib,json,subprocess,sys
base=Path(__file__).resolve().parent
root=base.parents[1]
groups={
 "controller_item_layout_and_lifecycle":[0x4c21b0,0x4c22e0,0x4c2490,0x4c24c0,0x4bc220,0x4c3c00,0x4c4650,0x4c4f90,0x4c4fd0],
 "spawn_frame_and_draw":[0x4c3c90,0x4c45b0,0x4c4420,0x4c3880,0x4c25a0,0x4c38d0,0x4c4f70,0x4c4f00,0x4c4900,0x4c4920,0x4c4940,0x4c4b70],
 "pickup_rewards_and_player_mutations":[0x4c4960,0x4c4b90,0x4c4da0,0x4c47c0,0x4e10e0,0x4e11a0,0x4e1250,0x4e1310,0x4e1410,0x4e1510,0x4c46a0,0x4a9fb0,0x4a9ec0,0x4a9d80,0x4a9f20,0x4a9e60,0x4aafa0,0x533780,0x533920,0x51b8a0,0x51b920,0x4c4b30,0x4993b0,0x4b8210,0x4b82c0,0x4b8300,0x4b7e20,0x4b7e70,0x477ff0,0x4b8100,0x4b8150,0x4b80b0],
 "compiled_factory_and_engine_adapters":[0x498fd0,0x41cab0,0x4c5010,0x4c2130,0x4c42c0,0x4c46f0,0x4c4740,0x4c4770]}
addresses=sorted({a for group in groups.values() for a in group})
subprocess.run([sys.executable,str(root/'tools/extract_function_evidence.py'),'--output',str(base/'evidence'),*[f'{a:08x}' for a in addresses]],check=True)
paths=list(base.glob('*.hpp'))+list(base.glob('*.cpp'))+[base/'CMakeLists.txt']+list((base/'oracle').glob('*.cpp'))+[base/'oracle/CMakeLists.txt']
dependencies=['runtime_state/state.hpp','runtime_state/state.cpp','game_session/session.hpp','game_session/session.cpp','runtime_core/callback_owner.hpp','runtime_core/runtime_core.cpp','core_scheduler/scheduler.hpp','core_scheduler/scheduler.cpp','sprite_renderer/animation.hpp','sprite_renderer/animation.cpp','sprite_renderer/pool.hpp','sprite_renderer/pool.cpp','ecl_vm/vm.cpp','ecl_vm/math.cpp','gameplay/player_state.hpp','gameplay/player_state.cpp','damage_regions/score.cpp','bomb_system/state.cpp']
paths += [base.parent/name for name in dependencies]+[root/'native_recovered/native_core.hpp',root/'tests/native_cpu_compare.cpp']
hashes={p.relative_to(root).as_posix():hashlib.sha256(p.read_bytes()).hexdigest() for p in paths}
report_path=base/'cpu_validation.json';report=json.loads(report_path.read_text())
if report['failed']:raise SystemExit('Failed Item comparison cannot be recorded as validated')
report['source_hashes']=hashes
report['scope']='Actual original Item constructors, full1536-slot pool reset, spawn/scatter, complete frame/draw, special-item selection, pickup reward bodies and full-type pickup dispatch, Player integer increments, overlay-meter logic, scheduler registration/enable/disable/destruction; compare whole storage/RNG/returns and external event sequence.'
report['boundaries']=['ANM script binding/update/draw and named effect creation are observed external boundaries; production binds their existing source implementations','HUD/score glyph/Player option refresh/phase transition are recorded boundary events; production requires their explicit source implementations','Frame-only stage uses controlled special activation/power reward callbacks; separate special/reward/full-type frame cases execute those original bodies','Heap pointer/context/callback-address normalization only accounts for separately allocated equivalent objects','Original invalid inputs, OOM and original ordinary-pool exhaustion undefined dereference are outside comparison domain']
report_path.write_text(json.dumps(report,indent=2)+'\n')
status={'status':'compiled_source_module','full_game_equivalence':False,'original_va':[f'0x{a:08x}' for a in addresses],'original_va_groups':{k:[f'0x{a:08x}' for a in values] for k,values in groups.items()},'source_hashes':hashes,'cpu_validation':'cpu_validation.json','remaining_source_dependencies':['HUD4b8650/4b8bf0/4b90e0','Player option refresh4faca0','special-owner51b960/global5c6120','special-state513dd0/global5c6118','special-phase transition534d00'],'resolved_score_adapter':'source_reconstruction/small_score/entry_adapter.cpp','limitations':report['boundaries']}
(base/'module_status.json').write_text(json.dumps(status,indent=2)+'\n')
print(f'{len(addresses)} evidence addresses; {report["passed"]} passed; {report["failed"]} failed')
