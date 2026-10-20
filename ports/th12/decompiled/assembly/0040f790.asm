
// block 0040f790
0040f790  mov        eax, dword ptr [edx + 8]
0040f793  test       eax, eax
0040f795  je         0x40f7ac

// block 0040f797
0040f797  cmp        ecx, eax
0040f799  jl         0x40f79f

// block 0040f79b
0040f79b  dec        eax
0040f79c  mov        dword ptr [edx], eax
0040f79e  ret        

// block 0040f79f
0040f79f  xor        eax, eax
0040f7a1  test       ecx, ecx
0040f7a3  setl       al
0040f7a6  dec        eax
0040f7a7  and        eax, ecx
0040f7a9  mov        dword ptr [edx], eax
0040f7ab  ret        

// block 0040f7ac
0040f7ac  mov        dword ptr [edx], ecx
0040f7ae  mov        eax, ecx
0040f7b0  ret        
