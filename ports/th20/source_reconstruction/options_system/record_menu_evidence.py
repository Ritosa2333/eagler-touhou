from pathlib import Path
import json,hashlib
root=Path(__file__).resolve().parents[2]
maps={
'help_system':{'help.cpp':[0x4bee70,0x4bef80,0x4bf990,0x4bfc70,0x4bf0a0,0x4bfc20,0x4bf930,0x4bf8d0],'texture.cpp':[0x44dc20,0x449d80]},
'options_system':{'lifecycle.cpp':[0x4dfc50,0x4dfd20,0x4e0e80,0x4e1080,0x4dd910],'update.cpp':[0x4dfe00,0x4e0f80],'volume.cpp':[0x4e0fc0,0x4e0f40],'draw.cpp':[0x4e07b0]},
'key_config':{'lifecycle.cpp':[0x4c5530,0x4c56b0,0x4c58d0,0x4c7cb0,0x4010f0],'update.cpp':[0x4c61d0,0x4c61a0,0x4c5790,0x4c7160,0x4c6230,0x4c7dd0,0x4c5ee0,0x4c7d10,0x4c5e20,0x4c6130,0x4c6210],'draw.cpp':[0x4c5870,0x4c7770,0x4c6b30,0x421890]}}
extra=[root/'source_reconstruction/text_renderer/menu_style.hpp',root/'source_reconstruction/text_renderer/menu_style.cpp',root/'source_reconstruction/text_renderer/oracle/menu_compare.inl',root/'source_reconstruction/input/input.hpp']
def hashed(paths):return {p.relative_to(root).as_posix():hashlib.sha256(p.read_bytes()).hexdigest() for p in paths}
allhash={}
for name,mapping in maps.items():
 directory=root/'source_reconstruction'/name
 sources=list(directory.glob('*.hpp'))+list(directory.glob('*.cpp'))+[directory/'CMakeLists.txt']+list((directory/'oracle').glob('*.cpp'))+list((directory/'oracle').glob('CMakeLists.txt'))
 hashes=hashed(sources+extra);allhash.update(hashes)
 status={'status':'compiled_source_module','full_game_equivalence':False,'source_hashes':hashes,'original_va':[f'0x{v:08x}'for values in mapping.values()for v in values],'evidence_map':{k:[f'0x{v:08x}'for v in values]for k,values in mapping.items()},'unrecovered_game_symbols':[]}
 if name=='help_system':status['validation_scope']='Win32 compilation only. Original CPU frame/lifecycle and real D3D image replacement/upload remain unverified.'
 elif name=='options_system':status['validation_scope']='393216 update/volume CPU assertions. 32768 full update cases with screen/audio/device/create/save/retire service boundaries; all65536 raw music/effect-volume pairs, command enqueue boundary. Shared draw_validation has18432 new checks including KeyConfig and textstyle, do not count twice.'
 else:status['validation_scope']='229376 CPU assertions over32768 full update cases, including real Cursor history save/restore with preallocated deque blocks. Audio/device rebuild/retire boundaries. Separate shared draw_validation includes both KeyConfig pages.'
 status['limitations']=['Owning constructor/destructor/factory not full original CPU compared','New source-only GDI/D3D upload paths are not validated by cached-text draw comparison']
 if name!='help_system':
  status['cpu_validation']='cpu_validation.json';p=directory/'cpu_validation.json';report=json.loads(p.read_text());report['source_hashes']=hashes;report['validation_scope']=status['validation_scope'];p.write_text(json.dumps(report,indent=2)+'\n')
 (directory/'module_status.json').write_text(json.dumps(status,indent=2)+'\n')
p=root/'source_reconstruction/options_system/draw_validation.json';r=json.loads(p.read_text());r['source_hashes']=allhash;r['original_sha256']='a274b45fe6ec53511718bb328c2ff169a74e67f95d1b0c74d97d348b955a0897';r['original_va']=['0x0046b590','0x004e07b0','0x004c7770','0x004c6b30'];r['duplicates_in']='text_renderer/cpu_validation.json total57113=38681 prior+18432 new';p.write_text(json.dumps(r,indent=2)+'\n')
for name in ['text_renderer']:
 directory=root/'source_reconstruction'/name
 sources=list(directory.glob('*.hpp'))+list(directory.glob('*.cpp'))+[directory/'CMakeLists.txt']+list((directory/'oracle').glob('*.cpp'))+list((directory/'oracle').glob('*.inl'))+[directory/'oracle/CMakeLists.txt']
 hashes=hashed(sources);hashes.update(allhash)
 p=directory/'cpu_validation.json';r=json.loads(p.read_text());r['source_hashes']=hashes;r['menu_checks_included']=18432;p.write_text(json.dumps(r,indent=2)+'\n')
 p=directory/'module_status.json';r=json.loads(p.read_text());r['source_hashes']=hashes;r['additional_menu_validation']='../options_system/draw_validation.json';p.write_text(json.dumps(r,indent=2)+'\n')
