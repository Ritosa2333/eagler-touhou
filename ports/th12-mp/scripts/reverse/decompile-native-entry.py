"""Decompile a reviewed native entry whose wrapper CFG merges its boundary.

Regions are explicitly limited to inspected original bytes. Outputs are local
research pseudocode; the original program, game runtime and goldens are untouched.
"""
import hashlib,json,logging,pathlib,sys,time
TITLE=pathlib.Path(__file__).resolve().parents[2]
sys.path.insert(0,str(TITLE.parent/'tools/reverse-python'))
import angr
EXE=TITLE.parent/'th12/th12.exe'
EXPECTED='99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417'
OUT=TITLE/'local/reverse/th12-1.00b/native-entries'
ENTRIES=[(0x4154d0,0x419654,'ecl_high_dispatch'),(0x402a40,0x402d90,'stage_load_register'),
         (0x402970,0x402a40,'stage_object_constructor'),(0x4466a0,0x446c50,'replay_menu_tick')]
def run():
    if hashlib.sha256(EXE.read_bytes()).hexdigest()!=EXPECTED:raise RuntimeError('Original identity mismatch')
    OUT.mkdir(parents=True,exist_ok=True);reports=[]
    for begin,end,name in ENTRIES:
        start=time.monotonic();project=angr.Project(str(EXE),auto_load_libs=False)
        cfg=project.analyses.CFGFast(normalize=True,regions=[(begin,end)],
            function_starts=[begin],force_smart_scan=False,force_complete_scan=False,data_references=True)
        f=project.kb.functions.get(begin)
        record={'originalExeSha256':EXPECTED,'address':hex(begin),'regionEnd':hex(end),'researchName':name,
                'entryEvidence':'manually checked native instruction entry; limited CFG region',
                'semanticVerificationComplete':False,'wholeGameOracle':False}
        if f:
            asm=[]
            for b in sorted(f.blocks,key=lambda b:b.addr):
                asm.append(f'\n// block {b.addr:08x}')
                for i in b.capstone.insns:asm.append(f'{i.address:08x} {i.mnemonic:10} {i.op_str}')
            (OUT/f'{begin:08x}.asm').write_bytes(('\n'.join(asm)+'\n').encode('utf-8'))
            result=project.analyses.Decompiler(f,cfg=cfg.model);text=result.codegen.text if result.codegen else None
            record.update(status='generated' if text else 'no-codegen',blocks=len(f.block_addrs_set),
                          errors=[str(error) for error in result.errors])
            if text:
                text=f'// AUTOMATIC PSEUDOCODE; NOT ORIGINAL SOURCE\n// {name} {begin:#x}; EXE {EXPECTED}\n\n'+text
                (OUT/f'{begin:08x}.c').write_bytes(text.encode('utf-8'));record['pseudocodeSha256']=hashlib.sha256(text.encode()).hexdigest()
        else:record['status']='unresolved-entry'
        record['seconds']=time.monotonic()-start;reports.append(record)
        (OUT/'index.json').write_text(json.dumps(reports,indent=2)+'\n',encoding='utf-8')
        print(json.dumps(record),flush=True)
if __name__=='__main__':logging.basicConfig(level=logging.ERROR);run()
