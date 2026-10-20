
// block 004d9f98
004d9f98  aaa        
004d9f99  sar        byte ptr [ecx], cl
004d9f9b  sbb        dword ptr [eax + 0x3fc06a3c], edi
004d9fa1  salc       
004d9fa2  cmp        dword ptr ds:[edi], ecx
004d9fa5  adc        eax, 0xe98cf3f0
004d9faa  mov        al, byte ptr [0xb058c70a]
004d9faf  aas        
004d9fb0  pop        ebx
004d9fb1  pushfd     
004d9fb2  mov        ebp, 0xa864274b
004d9fb8  jo         0x4d9f71

// block 004d9fba
004d9fba  xchg       ecx, eax
