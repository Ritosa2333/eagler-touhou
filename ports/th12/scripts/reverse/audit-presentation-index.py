"""Pinned original presentation/resource/I/O index; no runtime or original writes.

CFG ownership and automatic function boundaries are leads, not semantic proof.
This script uses original PE instructions/resources and the existing static CFG.
"""
import bisect, hashlib, json, pathlib, re, struct, sys
ROOT=pathlib.Path(__file__).resolve().parents[2]
sys.path.insert(0,str(ROOT.parent/'tools/python'))
import capstone, pefile
from native_anm_resource import check_original, EXE_SHA256

EXE=ROOT.parent/'th12/th12.exe';check_original(EXE)
pe=pefile.PE(str(EXE));base=pe.OPTIONAL_HEADER.ImageBase;image=pe.get_memory_mapped_image()
cfg=json.loads((ROOT/'local/reverse/th12-1.00b/functions.json').read_text())
if cfg['identity']['exeSha256']!=EXE_SHA256:raise ValueError('Stale CFG')
functions=cfg['functions'];starts=[int(f['address'],16) for f in functions]
by_address={int(f['address'],16):f for f in functions}
groups={
 'font_hud':{0x401090:'load ascii.anm',0x4015c0:'ASCII formatted text entry',0x401610:'ASCII grouped-number entry',0x4020a0:'ASCII formatted draw entry',0x4021a0:'ASCII grouped-number draw entry',0x41d250:'load front.anm',0x41d560:'bind embedded HUD/three front80 VMs',0x41f473:'HUD text draw basic-block slice',0x4214b0:'UFO inventory ring refresh',0x421690:'UFO inventory sprite blink',0x421a60:'UFO inventory rotating-position interrupts',0x420f90:'spell result front/ascii roots',0x43e250:'point-value display entry'},
 'anm':{0x402520:'VM reset',0x454b80:'sprite select',0x454d10:'reset then bind and first tick',0x454df0:'position then bind',0x454ee0:'bind without full reset',0x4551b0:'typed float source',0x4553f0:'typed integer source',0x455580:'typed integer destination',0x455630:'ANM logical tick',0x45fe60:'ANM bank load entry',0x460fe0:'primary ANM chain tick',0x4610d0:'secondary ANM chain tick',0x461250:'primary append registration',0x4612d0:'primary head registration',0x461350:'secondary append registration',0x4613d0:'secondary head registration',0x4615a0:'primary root factory',0x461720:'attached child factory',0x461840:'detached child factory',0x461920:'VM handle lookup',0x461970:'queued interrupt root and direct children',0x4619e0:'immediate interrupt root and direct children',0x461a70:'registered VM delete',0x462060:'find descendant by script'},
 'renderer':{0x45a570:'quad geometry helper',0x45c900:'ANM draw dispatch entry',0x45c3a0:'mode9/12/13 geometry vertex-array draw',0x45d430:'mode15 circle fan',0x45d5d0:'line strip draw entry',0x459a60:'VM D3D state setup',0x4606b0:'font-to-texture wrapper',0x460760:'left-aligned dynamic text',0x460800:'right-aligned dynamic text',0x44e660:'GDI text rasterization wrapper'},
 'std':{0x402970:'Stage object basic constructor',0x402a40:'Stage resource load and scheduler initialization',0x402f40:'Stage allocation/constructor wrapper',0x403020:'STD tick body and camera transfer',0x403ec0:'registered STD tick callback',0x404620:'STD object ANM tick',0x4049a0:'STD command interpreter',0x4046c0:'STD geometry draw layer',0x403d45:'STD visible bit draw basic-block slice'},
 'message':{0x41fcc0:'MSG interpreter tick',0x420e40:'MSG CP932 decryption',0x420216:'MSG text offset opcode25 basic-block slice',0x42065b:'MSG player expression opcode13 basic-block slice',0x4206d8:'MSG boss expression opcode14 basic-block slice'},
 'ufo':{0x449fa0:'UFO tick/draw registrations',0x44a2d0:'UFO manager tick',0x44a4f0:'right HUD bucket/fill text draw',0x44a5d0:'UFO multiplier/countdown/health presentation',0x44a8a0:'UFO spawn and presentation roots',0x44aca0:'UFO fill and filled interrupt',0x44af00:'UFO finish and score/multiplier display data'},
 'game_load':{0x42e9b0:'sig/text/music resource references',0x42f4b0:'th12.dat archive open reference',0x42feb0:'th12.cfg I/O references'},
 'audio':{0x430150:'MSG music handling entry',0x4537b0:'thbgm.dat open and worker thread create reference',0x4538e0:'BGM file read/seek entry',0x453d90:'sound event entry',0x453e20:'sound event with position entry',0x453f10:'sound stop entry',0x454960:'BGM command queue append'},
 'menu':{0x43f3d0:'title.anm/title_v.anm loading',0x43f760:'title music reference and replay-menu dispatch',0x446560:'replay directory/file enumeration',0x4466a0:'replay-menu state0..5 dispatch',0x449360:'title BGM reference'},
 'replay_save':{0x43b950:'replay input scheduler entry',0x43bc10:'replay naming/writing entry',0x43c350:'replay path/loading entry',0x43c590:'stage recording/input setup',0x43c730:'stage state restore and RNG seed reset',0x43c643:'replay stage resource write basic-block slice',0x43c869:'replay stage resource restore basic-block slice',0x43cf90:'scoreth12.dat read entry',0x43d33d:'scoreth12.dat write reference'},
}
index=[]
for group,entries in groups.items():
 for address,label in entries.items():
  f=by_address.get(address)
  index.append({'address':hex(address),'category':group,'researchLabel':label,
   'entryInCFG':bool(f),'boundaryStatus':'automatic CFG; original entry ABI still needs review' if f else 'interior basic block; do not decompile as independent C function',
   'cfg':f,'semanticStatus':'direct instruction/resource evidence for label only; not whole-function restoration'})

# Literal resource references. A nearest-start owner is explicitly provisional:
# the CFG may split/merge functions and linear decoding does not prove reachability.
patterns=[b'.rpy',b'replay',b'score',b'.anm',b'.wav',b'.ogg',b'.cfg',b'.dat']
strings={base+m.start():m.group().decode('ascii') for m in re.finditer(rb'[\x20-\x7e]{4,}',image)
 if len(m.group())<100 and any(p in m.group().lower() for p in patterns)}
refs={a:[] for a in strings};cs=capstone.Cs(capstone.CS_ARCH_X86,capstone.CS_MODE_32);cs.detail=True;cs.skipdata=True
section=next(s for s in pe.sections if s.Name.startswith(b'.text'))
imports={i.address:{'dll':e.dll.decode('ascii'),'name':i.name.decode('ascii') if i.name else '#'+str(i.ordinal),'address':hex(i.address),'references':[]}
 for e in pe.DIRECTORY_ENTRY_IMPORT for i in e.imports}
for ins in cs.disasm(section.get_data(),base+section.VirtualAddress):
 if ins.id==0:continue
 for op in ins.operands:
  value=op.imm if op.type==capstone.x86.X86_OP_IMM else op.mem.disp if op.type==capstone.x86.X86_OP_MEM else 0
  if value not in refs and value not in imports:continue
  owner=functions[max(0,bisect.bisect_right(starts,ins.address)-1)]['address']
  reference={'instruction':hex(ins.address),'nearestCFGStart':owner,'assembly':ins.mnemonic+' '+ins.op_str}
  if value in refs:refs[value].append(reference)
  if value in imports:imports[value]['references'].append(reference)

def read_anm(path,selection):
 data=path.read_bytes();entry=0;ordinal=0;output=[]
 while True:
  version,ns,nc=struct.unpack_from('<IHH',data,entry)
  if version!=7:raise ValueError('ANMv7 required')
  for n in range(nc):
   source_id,offset=struct.unpack_from('<II',data,entry+64+ns*4+n*8);pc=entry+offset;commands=[]
   while True:
    op,length,time,mask=struct.unpack_from('<HHhH',data,pc)
    if op==65535:break
    if length<8 or (length-8)%4:raise ValueError('ANM instruction length')
    words=list(struct.unpack_from('<'+'I'*((length-8)//4),data,pc+8));floats=struct.unpack_from('<'+'f'*len(words),data,pc+8)
    commands.append({'op':op,'time':time,'mask':mask,'words':words,'floatValues':[v if abs(v)<1e30 else None for v in floats]});pc+=length
   if ordinal in selection:output.append({'scriptOrdinal':ordinal,'sourceId':source_id,'commands':commands})
   ordinal+=1
  nxt=struct.unpack_from('<I',data,entry+36)[0]
  if not nxt:break
  entry+=nxt
 return {'resource':path.name,'sha256':hashlib.sha256(data).hexdigest(),'totalScriptOrdinals':ordinal,'scripts':output}
resources=[read_anm(ROOT/'local/retail'/f'{name}.anm',selection) for name,selection in [
 ('front',{80,81,134,135,136,137}),('bullet',{203,204,205,206,207,208,209,211}),('text',{0,1,2,3,74}),('ascii',{1,2,5,6,7,8,9,10,11,12,13,14})]]
OUT=ROOT/'artifacts/reverse';OUT.mkdir(parents=True,exist_ok=True)
report={'schema':'th12-original-full-presentation-index/1','originalExeSha256':EXE_SHA256,
 'scope':'presentation/resource/I/O entry audit from pinned PE plus existing angr CFG; local files only',
 'wholeGameOracle':False,'wholePresentationOracle':False,'originalSourceRecovered':False,
 'entryIndex':sorted(index,key=lambda r:int(r['address'],16)),
 'resourceReferences':[{'address':hex(a),'text':s,'references':refs[a]} for a,s in strings.items() if refs[a]],
 'selectedImports':[r for r in imports.values() if any(t in (r['dll']+' '+r['name']).lower() for t in ['sound','gdi','font','text','file','dir','write','read','registry','d3d','input'])],
 'anmResources':resources,
 'limitations':['String ownership uses nearest automatic CFG start, not a proven function boundary.',
 'Calls through virtual tables and ANM custom callbacks are not recovered by direct literal xrefs.',
 'Menu/title state machines, audio worker threading, I/O checksums and all field semantics remain separate analysis tasks.',
 'No original GPU/GDI/audio/window/file writes are executed by this index.']}
path=OUT/'full-presentation-entry-index.json';path.write_text(json.dumps(report,indent=2,ensure_ascii=False,allow_nan=False)+'\n',encoding='utf8')
print(json.dumps({'path':str(path),'priorityAddresses':len(index),'literalResourceReferences':sum(len(refs[a]) for a in refs),'selectedImports':len(report['selectedImports']),'anmScripts':sum(len(r['scripts']) for r in resources)}))
