"""Recover constant allocation sizes associated with retained diagnostic names.

Requires the observed wrapper pattern: source diagnostic -> direct call ->
constant push immediately followed by call to 0x0054275D, then initialization.
This records allocation extents; it does not infer member layouts or ownership.
"""
import csv,json,pathlib,sys
ROOT=pathlib.Path(__file__).resolve().parents[1]
sys.path.insert(0,str(ROOT/'.cache'/'binary_python'))
import pefile
from capstone import Cs,CS_ARCH_X86,CS_MODE_32,CS_OP_IMM
SOURCE=pathlib.Path(r'E:\BaiduNetdiskDownload\PCGAME-Touhou.Kinjoukyou.Fossilized.Wonders-P2P\Touhou.Kinjoukyou.Fossilized.Wonders\th20.exe')
OUT=ROOT/'analysis'/'binary'
def main():
    pe=pefile.PE(str(SOURCE));base=pe.OPTIONAL_HEADER.ImageBase
    dis=Cs(CS_ARCH_X86,CS_MODE_32);dis.detail=True
    def instructions(va,n=160):return list(dis.disasm(pe.get_data(va-base,n),va))
    def direct_call(i):return i.mnemonic=='call' and i.operands and i.operands[0].type==CS_OP_IMM
    with (OUT/'source_location_xrefs.csv').open(encoding='utf-8',newline='') as f:refs=list(csv.DictReader(f))
    recovered=[]
    for ref in refs:
        site=int(ref['reference_va'],16)
        insns=instructions(site,48)
        call=next((i for i in insns if direct_call(i)),None)
        if not call:continue
        wrapper=call.operands[0].imm
        if not 0x401000<=wrapper<0x56b47b:continue
        ws=instructions(wrapper)
        first_return=next((i for i,x in enumerate(ws) if x.mnemonic.startswith('ret')),None)
        if first_return is not None:ws=ws[:first_return+1]
        allocator_call=next((i for i in range(len(ws)) if direct_call(ws[i])),None)
        if allocator_call is None or allocator_call==0:continue
        alloc=ws[allocator_call];previous=ws[allocator_call-1]
        if alloc.operands[0].imm!=0x54275d or previous.mnemonic!='push' or previous.operands[0].type!=CS_OP_IMM:continue
        amount=previous.operands[0].imm
        if not 0<amount<64*1024*1024:continue
        later_calls=[i.operands[0].imm for i in ws[allocator_call+1:] if direct_call(i)]
        init=''
        if len(later_calls)>=2 and later_calls[0]==0x40c080:init=f'0x{later_calls[1]:08x}'
        recovered.append(dict(source_path=ref['source_path'],source_line=ref['source_line'],diagnostic_type=ref['type_or_context'],diagnostic_reference_va=ref['reference_va'],factory_candidate_va=ref['candidate_function'],wrapper_va=f'0x{wrapper:08x}',allocation_call_va=f'0x{alloc.address:08x}',allocation_routine_va='0x0054275d',allocation_bytes=amount,allocation_hex=f'0x{amount:x}',initialization_function_candidate=init,confidence='Exact immediate and call chain observed; semantic type/layout not proven'))
    fields=list(recovered[0]) if recovered else []
    with (OUT/'allocation_size_evidence.csv').open('w',encoding='utf-8',newline='') as f:
        w=csv.DictWriter(f,fieldnames=fields);w.writeheader();w.writerows(recovered)
    result=dict(diagnostic_sites_with_constant_allocation=len(recovered),unique_wrappers=len(set(x['wrapper_va'] for x in recovered)),unique_diagnostic_types=len(set(x['diagnostic_type'] for x in recovered)),scope='Observed constant allocation sizes correlated to source diagnostic strings; no member layout inference')
    (OUT/'allocation_size_summary.json').write_text(json.dumps(result,indent=2),encoding='utf-8');print(json.dumps(result,indent=2))
if __name__=='__main__':main()
