import sys,struct,pathlib,re,json
root=pathlib.Path(__file__).resolve().parents[2]
sys.path.insert(0,str(root/'.cache/binary_python'))
import pefile
path=pathlib.Path(r'E:\BaiduNetdiskDownload\PCGAME-Touhou.Kinjoukyou.Fossilized.Wonders-P2P\Touhou.Kinjoukyou.Fossilized.Wonders\th20.exe')
pe=pefile.PE(str(path));data=pe.get_memory_mapped_image()
ctors=[0x5323c0,0x532510,0x52fb40,0x5306a0,0x534ee0,0x535c10,0x531100,0x531650,0x5323c0,0x5326e0,0x532610,0x530d40,0x530b90,0x538260,0x537130,0x532270,0x531cc0,0x5326e0]
base=struct.unpack_from('<30I',data,0x575810-0x400000)
standard=dict(enumerate(base))
for slot,va in {2:0x52fbd0,3:0x52fd10,4:0x52ff70,5:0x5301c0,6:0x530370,20:0x52ffd0,21:0x52fd70}.items():standard[slot]=va
results=[]
for i,ctor in enumerate(ctors):
 text=(root/f'analysis/ghidra/pseudocode/{ctor:08x}.c').read_text()
 vtable=int(re.search(r'PTR_FUN_([0-9a-f]{8})',text)[1],16)
 vals=struct.unpack_from('<30I',data,vtable-0x400000)
 changes={f'{j*4:02x}':f'{v:08x}' for j,v in enumerate(vals) if v!=standard[j]}
 print(i,hex(ctor),hex(vtable),changes)
 results.append(dict(index=i,constructor=f'{ctor:08x}',vtable=f'{vtable:08x}',methods={f'{j*4:02x}':f'{v:08x}'for j,v in enumerate(vals)}))
(pathlib.Path(__file__).parent/'evidence/weapon_vtables.json').write_text(json.dumps(results,indent=2))
