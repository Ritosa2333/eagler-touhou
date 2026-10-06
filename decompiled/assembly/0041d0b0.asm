
// block 0041d0b0
0041d0b0  push       ebp
0041d0b1  push       esi
0041d0b2  mov        esi, dword ptr [0x4b43cc]
0041d0b8  mov        ecx, dword ptr [esi + 0xa8]
0041d0be  mov        eax, ecx
0041d0c0  cdq        
0041d0c1  push       edi
0041d0c2  mov        edi, 0x64
0041d0c7  idiv       edi
0041d0c9  mov        ebp, 0x3e8
0041d0ce  mov        edi, edx
0041d0d0  cdq        
0041d0d1  idiv       ebp
0041d0d3  mov        eax, edx
0041d0d5  add        eax, 0x3a6
0041d0da  cdq        
0041d0db  idiv       ebp
0041d0dd  lea        eax, [edi + 0x43]
0041d0e0  mov        edi, 0x64
0041d0e5  mov        ebp, edx
0041d0e7  cdq        
0041d0e8  idiv       edi
0041d0ea  mov        eax, 0x14f8b589
0041d0ef  add        ebp, edx
0041d0f1  imul       ecx
0041d0f3  sar        edx, 0xd
0041d0f6  mov        eax, edx
0041d0f8  shr        eax, 0x1f
0041d0fb  lea        ecx, [edx + eax - 0x16]
0041d0ff  cmp        ecx, ebp
0041d101  je         0x41d119

// block 0041d103
0041d103  mov        edx, dword ptr [esp + 0x10]
0041d107  pop        edi
0041d108  pop        esi
0041d109  mov        dword ptr [edx], 0x3e7
0041d10f  mov        dword ptr [ebx], 0x63
0041d115  pop        ebp
0041d116  ret        4

// block 0041d119
0041d119  mov        eax, dword ptr [esi + 0xa8]
0041d11f  cdq        
0041d120  mov        ecx, edi
0041d122  idiv       ecx
0041d124  mov        esi, 0x3e8
0041d129  mov        ecx, edx
0041d12b  cdq        
0041d12c  idiv       esi
0041d12e  mov        eax, edx
0041d130  add        eax, 0x3a6
0041d135  cdq        
0041d136  idiv       esi
0041d138  mov        eax, dword ptr [esp + 0x10]
0041d13c  mov        dword ptr [eax], edx
0041d13e  lea        eax, [ecx + 0x43]
0041d141  cdq        
0041d142  mov        ecx, edi
0041d144  idiv       ecx
0041d146  pop        edi
0041d147  pop        esi
0041d148  pop        ebp
0041d149  mov        dword ptr [ebx], edx
0041d14b  ret        4
