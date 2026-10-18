"""Bind source hashes and original evidence ranges to the completed CPU run."""
from pathlib import Path
import csv, hashlib, json, subprocess, sys
directory=Path(__file__).resolve().parent
root=directory.parents[1]
groups={
 'construction_and_lifecycle':[0x478ec0,0x412710,0x4bfde0,0x4bfee0,0x4bfe80,0x4bffb0,0x4c0120,0x4c20d0,0x4c2090,0x4c0b30,0x4c0cb0],
 'allocation_handles_and_motion':[0x4c0c60,0x4c0d00,0x4c0e10,0x4c0e60,0x4c1eb0,0x4c1f00,0x4c1f30,0x4c1ff0,0x4c2030,0x4c1ae0,0x4c1b80,0x4c1c10,0x4c1c50,0x4c1d80,0x488a20,0x47a4a0,0x4c03d0,0x4c02d0],
 'collision':[0x4c1030,0x456fe0,0x457300,0x457040,0x456920,0x457610,0x4580c0,0x458b60,0x4562e0,0x4578b0,0x4570f0,0x457380,0x456b40,0x456d50,0x456690,0x4567d0,0x457c10,0x458670,0x459190],
 'damage_and_callbacks':[0x4c0480,0x4c0ce0,0x4ff4e0,0x4ff840,0x4ff5e0,0x4e14b0,0x488550,0x4c1fc0,0x4c1ac0,0x4c1e90,0x4c0f70,0x4c0fb0,0x506360,0x504af0,0x506a00,0x4bfcd0,0x4bfd60,0x4a9f80,0x533720]}
addresses=[a for group in groups.values() for a in group]
subprocess.run([sys.executable,str(root/'tools/extract_function_evidence.py'),'--output',str(directory/'evidence'),*[f'{a:08x}' for a in addresses]],check=True)
paths=list(directory.glob('*.hpp'))+list(directory.glob('*.cpp'))+[directory/'CMakeLists.txt']+list((directory/'oracle').glob('*.cpp'))+list((directory/'oracle').glob('*.inc'))+[directory/'oracle/CMakeLists.txt']
dependencies=['runtime_state/state.hpp','runtime_state/state.cpp','runtime_state/motion.hpp','runtime_state/motion.cpp','core_scheduler/scheduler.hpp','core_scheduler/scheduler.cpp','runtime_core/runtime_core.hpp','runtime_core/runtime_core.cpp','runtime_core/callback_owner.hpp','ecl_vm/math.hpp','ecl_vm/math.cpp','ecl_vm/vm.cpp','game_session/session.hpp','game_session/session.cpp','sprite_renderer/sprite.hpp','sprite_renderer/animation.hpp','sprite_renderer/animation.cpp','sprite_renderer/pool.hpp','sprite_renderer/pool.cpp','sprite_renderer/loading_interrupt.hpp','sprite_renderer/loading_interrupt.cpp','gameplay/enemy.hpp','gameplay/enemy_variables.hpp','gameplay/enemy_variables.cpp','bomb_system/bomb.hpp']
paths += [directory.parent/name for name in dependencies]+[root/'native_recovered/native_core.hpp',root/'tests/native_cpu_compare.cpp']
hashes={p.relative_to(root).as_posix():hashlib.sha256(p.read_bytes()).hexdigest() for p in paths}
report_path=directory/'cpu_validation.json';report=json.loads(report_path.read_text(encoding='utf8'))
if report['failed']:raise SystemExit('Cannot mark failing validation as complete')
report['source_hashes']=hashes
report['validation_scope']='Real region layouts/lists/motion/collision/damage loop and player-shot callbacks; aggregate Item creation and callback table are controlled boundaries; null-active Bomb path; arbitrary scheduling and invalid original inputs excluded.'
report_path.write_text(json.dumps(report,indent=2)+'\n',encoding='utf8')
status={'status':'compiled_source_module','full_game_equivalence':False,'original_va':[f'0x{a:08x}' for a in addresses],
 'original_va_groups':{k:[f'0x{a:08x}' for a in v] for k,v in groups.items()},'source_hashes':hashes,'cpu_validation':'cpu_validation.json',
 'unrecovered_game_symbols':[],'resolved_item_adapter':'source_reconstruction/item_system/entry_adapter.cpp',
 'limitations':['Whole-game equivalence is not claimed','Aggregate Item creation and custom hit callback boundary use recorded fixture events','Bomb aggregate fixture uses no active Bomb; production calls real Bomb source','Invalid original inputs and allocation failure behavior are excluded','Full player-entity and player-shot creation belong to other modules']}
(directory/'module_status.json').write_text(json.dumps(status,indent=2)+'\n',encoding='utf8')
print(len(addresses),'original evidence entries;',report['passed'],'checks;',report['failed'],'failed')
