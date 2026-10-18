import hashlib,json,pathlib
module=pathlib.Path(__file__).resolve().parent
root=module.parents[1]
mapping={'completion.cpp':[0x4bc570,0x4bd980],'progress.cpp':[0x4bd780,0x4bddc0,0x4bcf60],'playtime.cpp':[0x4bce10,0x4bcef0,0x4bcf30],'replay_access.cpp':[0x488740,0x488770,0x488800]}
paths=list(module.glob('*.cpp'))+list(module.glob('*.hpp'))+[module/'CMakeLists.txt',module/'oracle/compare.cpp',module/'oracle/CMakeLists.txt']
hashes={p.relative_to(root).as_posix():hashlib.sha256(p.read_bytes()).hexdigest()for p in paths}
report=json.loads((module/'cpu_validation.json').read_text());report['source_hashes']=hashes
report['validation_scope']='126976 comparisons: complete_stage original body with external UI/player/audio boundaries; three full Replay getters. Unlock/grant/playtime helpers have 49152 combined checks with profile getters in progress_state/cpu_validation.json; do not count those twice.'
(module/'cpu_validation.json').write_text(json.dumps(report,indent=2)+'\n')
status=dict(status='compiled_source_module',full_game_equivalence=False,evidence_map={n:[f'0x{v:08x}'for v in a]for n,a in mapping.items()},original_va=[f'0x{v:08x}'for a in mapping.values()for v in a],source_hashes=hashes,cpu_validation='cpu_validation.json',unrecovered_game_symbols=['Pause finish_practice/finish_replay (being restored in pause_system)','announce_achievement 0052f900'],limits=['Whole game UI/audio/renderer integration not covered by isolated oracle','Replay owning lifecycle belongs to replay_system'])
(module/'module_status.json').write_text(json.dumps(status,indent=2)+'\n')
