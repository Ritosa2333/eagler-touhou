from pathlib import Path
import hashlib,json,subprocess,sys
base=Path(__file__).resolve().parent;root=base.parents[1]
addresses=[0x50fd00,0x50fd50,0x50fdd0,0x50fea0,0x50ff70,0x510110,0x510640,0x510710,0x510810,0x510820,0x510850,0x510870,0x510890,0x5108d0,0x4bd5a0,0x421810,0x506de0]
subprocess.run([sys.executable,str(root/'tools/extract_function_evidence.py'),'--output',str(base/'evidence'),*[f'{a:08x}' for a in addresses]],check=True)
paths=list(base.glob('*.hpp'))+list(base.glob('*.cpp'))+[base/'CMakeLists.txt']+list((base/'oracle').glob('*.cpp'))+[base/'oracle/CMakeLists.txt']
dependencies=['runtime_state/state.hpp','runtime_state/state.cpp','game_session/session.hpp','game_session/session.cpp','runtime_core/callback_owner.hpp','runtime_core/runtime_core.cpp','core_scheduler/scheduler.hpp','core_scheduler/scheduler.cpp','sprite_renderer/animation.hpp','sprite_renderer/animation.cpp','sprite_renderer/pool.hpp','sprite_renderer/pool.cpp','text_renderer/text.hpp','text_renderer/ascii.cpp','text_renderer/dynamic.cpp','sprite_renderer/binding.hpp','sprite_renderer/quad.hpp','program_entry/program_entry.hpp','platform_window/draw_controls.cpp']
paths += [base.parent/name for name in dependencies]+[root/'native_recovered/native_core.hpp',root/'tests/native_cpu_compare.cpp']
hashes={p.relative_to(root).as_posix():hashlib.sha256(p.read_bytes()).hexdigest() for p in paths}
report_path=base/'cpu_validation.json';report=json.loads(report_path.read_text())
if report['failed']:raise SystemExit('Cannot register failing SmallScore comparison')
report['source_hashes']=hashes
report['scope']='4096 Entry constructor cases,128 complete owner constructors,4096 integer spawn cases,4096 complete update/draw frames,128 scheduler/initialization/destruction cases. All compare real original instruction bodies against genuine C++.'
report['boundaries']=['Sprite initialization/selection, descriptor query, quad draw, fog and text commit use recorded external boundaries; production calls recovered source','Text setters run actual original writes; all Text state and per-glyph/per-line arguments are compared','Pointer and callback normalization only accounts for separately allocated equivalent runtime objects','Original invalid slot/digit indexes, corrupt ownership, allocation failure and graphics output are outside this module oracle']
report_path.write_text(json.dumps(report,indent=2)+'\n')
status={'status':'compiled_source_module','full_game_equivalence':False,'original_va':[f'0x{a:08x}' for a in addresses],'source_hashes':hashes,'cpu_validation':'cpu_validation.json','unrecovered_game_symbols':[],'source_dependencies':['th20_text_renderer','th20_sprite_renderer','th20_platform_window','th20_core_scheduler','th20_runtime_state','th20_game_session'],'limitations':report['boundaries']}
(base/'module_status.json').write_text(json.dumps(status,indent=2)+'\n');print(f'{len(addresses)} evidence addresses; {report["passed"]} passed; {report["failed"]} failed')
