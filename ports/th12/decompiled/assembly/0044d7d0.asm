
// block 0044d7d0
0044d7d0  mov        eax, ecx
0044d7d2  shr        eax, 0x14
0044d7d5  mov        edx, ecx
0044d7d7  and        eax, 0xf
0044d7da  shr        edx, 0xc
0044d7dd  shl        eax, 4
0044d7e0  and        edx, 0xf
0044d7e3  or         eax, edx
0044d7e5  mov        edx, dword ptr [0x4b0e68]
0044d7eb  shr        ecx, 4
0044d7ee  and        ecx, 0xf
0044d7f1  shl        eax, 4
0044d7f4  or         eax, ecx
0044d7f6  xor        ecx, ecx
0044d7f8  cmp        dword ptr [0x4b0e54], ecx
0044d7fe  jle        0x44d811

// block 0044d800
0044d800  mov        word ptr [edx], ax
0044d803  add        ecx, 2
0044d806  add        edx, 2
0044d809  cmp        ecx, dword ptr [0x4b0e54]
0044d80f  jl         0x44d800

// block 0044d811
0044d811  ret        
